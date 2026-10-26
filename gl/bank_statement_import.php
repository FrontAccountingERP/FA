<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU General Public License, GPL,
	as published by the Free Software Foundation, either version 3
	of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the License here <http://www.gnu.org/licenses/gpl-3.0.html>.
***********************************************************************/
// [CUSTOM] Native bank statement import (raw bank / DocuClipper CSV).
// Flow: upload -> parse (precise per-line errors) -> auto-allocate via rules
// -> manual allocation review -> post to GL (bank_trans + gl_trans + audit).
// Part of Banking & General Ledger.

$page_security = 'SA_BANKIMPORT';
$path_to_root = "..";
include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/data_checks.inc");
include_once($path_to_root . "/includes/types.inc");
include_once($path_to_root . "/gl/includes/gl_db.inc");
include_once($path_to_root . "/gl/includes/db/gl_db_bank_accounts.inc");
include_once($path_to_root . "/gl/includes/db/gl_db_bank_trans.inc");
include_once($path_to_root . "/gl/includes/bank_import_parse.inc");
include_once($path_to_root . "/gl/includes/db/bank_import_rules_db.inc");

$js = "";
if ($SysPrefs->use_popup_windows)
	$js .= get_js_open_window(800, 500);

page(_($help_context = "Bank Statement Import"), false, false, "", $js);

check_db_has_bank_accounts(_("There are no bank accounts defined in the system."));

// ---------------------------------------------------------------------------
//  Posting helper
// ---------------------------------------------------------------------------
// Post one imported bank transaction as a proper bank payment/deposit with
// balanced double-entry GL + audit trail. $amount is signed (negative=out).
// $vat=true splits the offset into net + VAT (amount treated as VAT-inclusive).
function bank_import_post_one($bank_act, $date, $ref, $amount, $gl_account, $memo, $vat=false)
{
	$type = ($amount < 0) ? ST_BANKPAYMENT : ST_BANKDEPOSIT;
	$bank_gl_account = get_bank_gl_account($bank_act);
	$trans_no = get_next_trans_no($type);
	// add_bank_trans/add_gl_trans/add_audit_trail expect a DISPLAY date and
	// convert internally via date2sql(); our parser holds SQL dates, so convert.
	$display_date = sql2date($date);

	add_bank_trans($type, $trans_no, $bank_act, $ref, $display_date, $amount, 0, '');
	add_gl_trans($type, $trans_no, $display_date, $bank_gl_account, 0, 0, $memo, $amount);

	$vat_account = '';
	$vat_rate = 0.0;
	if ($vat) {
		$vi = bank_import_vat_info();
		$vat_account = ($amount < 0) ? $vi['purchasing_account'] : $vi['sales_account'];
		$vat_rate = $vi['rate'];
	}

	if ($vat_account !== '' && $vat_rate > 0) {
		// offset is VAT-inclusive: split into net + VAT (sum = -amount)
		$net = $amount / (1 + $vat_rate);
		$vat_amount = $amount - $net;
		add_gl_trans($type, $trans_no, $display_date, $gl_account, 0, 0, $memo, -$net);
		add_gl_trans($type, $trans_no, $display_date, $vat_account, 0, 0, $memo, -$vat_amount);
	} else {
		add_gl_trans($type, $trans_no, $display_date, $gl_account, 0, 0, $memo, -$amount);
	}
	add_audit_trail($type, $trans_no, $display_date);
	return $trans_no;
}

// Read the default (first active) tax type: rate + VAT GL accounts.
// No tax type configured means VAT splitting is unavailable — never fall back
// to a hard-coded rate, it would post wrong figures silently.
function bank_import_vat_info()
{
	$res = db_query("SELECT rate, sales_gl_code, purchasing_gl_code FROM ".TB_PREF."tax_types
		WHERE inactive=0 AND rate > 0 ORDER BY id LIMIT 1");
	if (db_num_rows($res) > 0) {
		$row = db_fetch($res);
		return array(
			'rate' => (float)$row['rate'] / 100.0,
			'sales_account' => $row['sales_gl_code'],
			'purchasing_account' => $row['purchasing_gl_code'],
		);
	}
	return array('rate' => 0.0, 'sales_account' => '', 'purchasing_account' => '');
}

// ---------------------------------------------------------------------------
//  State
// ---------------------------------------------------------------------------
$bank_account = '';
$sep = ',';
$parsed = null;         // freshly parsed rows (this request)
$parse_error = '';

// transactions carried in session across parse -> review -> post
$session = isset($_SESSION['bank_import']) ? $_SESSION['bank_import'] : null;

// ---------------------------------------------------------------------------
//  POST: import / post to GL
// ---------------------------------------------------------------------------
$post_result = null;
if (isset($_POST['import']) && $session !== null) {
	if (!isset($_POST['backup_confirm'])) {
		// Blocked: must confirm backup first. Keep the session (preview) AND the
		// allocations already typed, so ticking the box and retrying does not
		// throw away a whole statement's manual review.
		if (isset($_POST['alloc']) && is_array($_POST['alloc'])) {
			$vat_map = (isset($_POST['vat']) && is_array($_POST['vat'])) ? $_POST['vat'] : array();
			foreach ($_POST['alloc'] as $i => $v) {
				if (!isset($session['trans'][$i]))
					continue;
				$session['trans'][$i]['gl_account'] = trim($v);
				$session['trans'][$i]['vat'] = isset($vat_map[$i]) ? 1 : 0;
			}
			$_SESSION['bank_import'] = $session;
		}
		$post_result = array('ok' => 0, 'fail' => 0, 'error' => '', 'rows' => array(),
			'blocked' => _("You must confirm you have made a backup before importing."));
	} else {
	$bank_act = $session['bank_account'];
	$ok = 0; $fail = 0; $post_error = ''; $post_rows = array();
	foreach ($session['trans'] as $i => $t) {
		if (!empty($t['errors'])) {
			// rows that failed validation are never posted, but they ARE
			// reported so the summary totals account for every line
			$fail++;
			$reason = implode('; ', $t['errors']);
			$post_rows[] = array('line' => $t['line'], 'desc' => $t['desc'], 'status' => 'fail', 'reason' => $reason);
			continue;
		}
		$gl = isset($_POST["alloc"][$i]) ? trim($_POST["alloc"][$i]) : '';
		if ($gl === '') {
			$fail++;
			$reason = sprintf(_("no GL account selected for '%s' (%s)"), $t['desc'], price_format($t['amount']));
			$post_error .= sprintf(_("Line %d: %s. "), $t['line'], $reason);
			$post_rows[] = array('line' => $t['line'], 'desc' => $t['desc'], 'status' => 'fail', 'reason' => $reason);
			continue;
		}
		// Resolve a code (numeric or typed name / "code - name") to a real,
		// active chart account. db_escape() returns the value already
		// single-quoted + escaped. Rule-supplied codes go through this too, so
		// a rule pointing at a deleted account fails loudly instead of posting.
		$esc = db_escape($gl);
		$res = db_query("SELECT account_code FROM ".TB_PREF."chart_master
			WHERE inactive=0 AND (account_code=$esc OR account_name=$esc
			OR CONCAT(account_code,' - ',account_name)=$esc) LIMIT 1");
		if (db_num_rows($res) == 0) {
			$fail++;
			$reason = sprintf(_("account '%s' not found"), $gl);
			$post_error .= sprintf(_("Line %d: %s. "), $t['line'], $reason);
			$post_rows[] = array('line' => $t['line'], 'desc' => $t['desc'], 'status' => 'fail', 'reason' => $reason);
			continue;
		}
		$row = db_fetch($res);
		$gl = $row['account_code'];

		$memo = $t['desc'];
		if ($t['ref'] !== '')
			$memo .= ' ['.$t['ref'].']';
		$vat = isset($_POST['vat'][$i]);
		bank_import_post_one($bank_act, $t['date'], $t['ref'], $t['amount'], $gl, $memo, $vat);
		$ok++;
		$post_rows[] = array('line' => $t['line'], 'desc' => $t['desc'], 'status' => 'ok', 'reason' => '');
	}
	$post_result = array('ok' => $ok, 'fail' => $fail, 'error' => $post_error, 'rows' => $post_rows);
	unset($_SESSION['bank_import']);
	$session = null;
	} // end backup_confirm else
}

// ---------------------------------------------------------------------------
//  POST: parse + auto-allocate
// ---------------------------------------------------------------------------
if (isset($_POST['parse']) && isset($_FILES['imp']) && $_FILES['imp']['tmp_name'] != '') {
	$bank_account = get_post('bank_account');
	// separator is always comma (fixed format)

	$fp = @fopen($_FILES['imp']['tmp_name'], 'r');
	if (!$fp) {
		$parse_error = _("Could not open uploaded file.");
	} else {
		$rows = array();
		while (($data = fgetcsv($fp, 0, $sep)) !== false)
			$rows[] = $data;
		fclose($fp);

		if (count($rows) == 0) {
			$parse_error = _("File is empty.");
		} else {
			// Fixed statement format: Date, Description, Debit, Credit.
			// Row 0 is the header and is skipped; debit (col 2) is money out,
			// credit (col 3) is money in, so amount = credit - debit.
			$parsed = array();
			for ($r = 1; $r < count($rows); $r++) {
				$row = $rows[$r];
				if (count($row) == 1 && trim((string)$row[0]) === '')
					continue; // blank trailing line
				$errs = array();

				// date (col 0)
				$raw_date = isset($row[0]) ? $row[0] : '';
				$d = bank_import_date($raw_date);
				if ($d === null)
					$errs[] = sprintf(_("Invalid date: '%s'"), $raw_date);
				elseif (!is_date_in_fiscalyear($d, true))
					$errs[] = _("Date is outside the fiscal year or in a closed period - create the fiscal year first (Setup > Fiscal Years)");

				// description (col 1)
				$desc = isset($row[1]) ? trim($row[1]) : '';
				if ($desc === '')
					$errs[] = _("Missing description");

				// amounts (cols 2/3)
				$raw_debit = isset($row[2]) ? $row[2] : '';
				$raw_credit = isset($row[3]) ? $row[3] : '';
				$debit = bank_import_amount($raw_debit);
				$credit = bank_import_amount($raw_credit);
				$amount = null;
				if ($debit !== null || $credit !== null) {
					$debit = $debit === null ? 0.0 : $debit;
					$credit = $credit === null ? 0.0 : $credit;
					if ($debit != 0.0 && $credit != 0.0)
						$errs[] = _("Both debit and credit present on one line");
					$amount = $credit - $debit;
					if ($amount == 0.0)
						$errs[] = _("Zero amount");
				} else
					$errs[] = sprintf(_("Invalid amount: '%s' / '%s'"), $raw_debit, $raw_credit);

				// auto-allocation (only for valid rows): the matched rule also
				// carries whether this payee's amounts are VAT-inclusive
				$gl_account = null;
				$row_vat = 0;
				if (empty($errs) && $amount !== null) {
					$type_hint = ($amount < 0) ? 1 : 2; // 1=payment, 2=deposit
					$rule = bank_import_find_rule($desc, $type_hint);
					if ($rule !== null) {
						$gl_account = $rule['gl_account'];
						$row_vat = !empty($rule['vat']) ? 1 : 0;
					}
				}

				$parsed[] = array(
					'line' => $r + 1,
					'date' => $d,
					'desc' => $desc,
					'amount' => $amount,
					'ref' => '',
					'gl_account' => $gl_account,
					'vat' => $row_vat,
					'errors' => $errs,
				);
			}

			// persist for the review/post step
			$_SESSION['bank_import'] = array(
				'bank_account' => $bank_account,
				'trans' => $parsed,
			);
			$session = $_SESSION['bank_import'];
		}
	}
}

// ---------------------------------------------------------------------------
//  Display
// ---------------------------------------------------------------------------
if ($post_result !== null) {
	if (isset($post_result['blocked'])) {
		display_error($post_result['blocked']);
	} else {
	// Verbose, visible result box in the page body (not just the top msgbox).
	div_start('import_result');
	start_table(TABLESTYLE, "width='95%'");
	echo "<tr><td colspan='2' style='padding:8px;font-weight:bold;'>"
		._("Import result").": "
		.sprintf(_("%d imported successfully, %d failed."), $post_result['ok'], $post_result['fail'])
		."</td></tr>";
	end_table(1);
	div_end();

	if ($post_result['fail'] > 0) {
		div_start('import_failures');
		start_table(TABLESTYLE, "width='95%'");
		$th = array(_("Line"), _("Description"), _("Reason"));
		table_header($th);
		$k = 0;
		foreach ($post_result['rows'] as $r) {
			if ($r['status'] != 'fail') continue;
			alt_table_row_color($k);
			label_cell($r['line']);
			label_cell($r['desc']);
			label_cell("<span style='color:red'>".htmlspecialchars($r['reason'])."</span>");
			end_row();
		}
		end_table(1);
		div_end();
	}

	// keep the top msgbox summary too
	display_note(sprintf(_("%d transaction(s) imported successfully."), $post_result['ok']));
	if ($post_result['fail'] > 0)
		display_error(sprintf(_("%d transaction(s) not imported. See details below."), $post_result['fail']));
	} // end else (not blocked)
}

start_form(true);

start_table(TABLESTYLE2, "width='90%'");

$accounts = get_bank_accounts();
$sel_bank = ($session !== null) ? $session['bank_account'] : $bank_account;

echo "<tr><td>"._("Bank Account")."</td><td>";
echo "<select name='bank_account'>";
while ($acc = db_fetch($accounts)) {
	echo "<option value='".$acc['id']."' ".($acc['id'] == $sel_bank ? "selected" : "").">"
		.$acc['bank_account_name']." - ".$acc['bank_curr_code']." (".$acc['account_name'].")</option>";
}
echo "</select></td></tr>";

file_row(_("Bank statement CSV"), 'imp', 'imp');

echo "<tr><td colspan='2'><span style='color:#888'>"._("Format: Date, Description, Debit, Credit (header row is skipped).")."</span></td></tr>";

end_table(1);

submit_center('parse', _("Parse / Preview"), true, false, 'nonajax');

end_form();

// ---------------------------------------------------------------------------
//  Preview + allocation review + import
// ---------------------------------------------------------------------------
if ($parse_error !== '')
	display_error($parse_error);

if ($session !== null) {
	$trans = $session['trans'];
	$err_total = 0; $unalloc = 0;
	foreach ($trans as $p) {
		$err_total += count($p['errors']);
		if (empty($p['errors']) && ($p['gl_account'] === null || $p['gl_account'] === ''))
			$unalloc++;
	}

	// GL account list for the allocation dropdowns
	$gl_rows = db_query("SELECT account_code, account_name FROM ".TB_PREF."chart_master
		WHERE inactive=0 ORDER BY account_code");
	$gl_accounts = array();
	while ($g = db_fetch($gl_rows))
		$gl_accounts[$g['account_code']] = $g['account_code']." - ".$g['account_name'];

	echo "<form method='post'>";
	// Shared datalist for GL account allocation — far lighter than one full
	// <select> per row (197 rows x ~110 accounts = ~21k <option> elements).
	echo "<datalist id='gl_acc_list'>";
	foreach ($gl_accounts as $code => $name)
		echo "<option value='$code'>".htmlspecialchars($name)."</option>";
	echo "</datalist>";

	$vi = bank_import_vat_info();
	$vat_available = ($vi['rate'] > 0 && $vi['sales_account'] !== '' && $vi['purchasing_account'] !== '');

	div_start('preview');
	start_table(TABLESTYLE, "width='95%'");
	$th = array(_("Line"), _("Date"), _("Description"), _("Amount"), _("GL Account"), _("VAT"), _("Errors"));
	table_header($th);
	$k = 0;
	foreach ($trans as $i => $p) {
		alt_table_row_color($k);
		label_cell($p['line']);
		label_cell($p['date']);
		label_cell($p['desc']);
		amount_cell($p['amount']);
		if (empty($p['errors'])) {
			$val = ($p['gl_account'] !== null) ? $p['gl_account'] : '';
			$tag = ($val === '')
				? "<br><span style='color:orange'>"._("manual")."</span>"
				: "<br><span style='color:green'>"._("auto")."</span>";
			echo "<td><input name='alloc[$i]' list='gl_acc_list' value='".htmlspecialchars($val)."' size='28' placeholder='"._("account code or name")."'>".$tag."</td>";
			if ($vat_available)
				echo "<td style='text-align:center'><input type='checkbox' name='vat[$i]' value='1'"
					.(!empty($p['vat']) ? " checked" : "")."></td>";
			else
				label_cell(_("n/a"));
			label_cell("&nbsp;");
		} else {
			label_cell("&nbsp;");
			label_cell("&nbsp;");
			label_cell("<span style='color:red'>".implode('; ', $p['errors'])."</span>");
		}
		end_row();
	}
	end_table(1);
	div_end();

	if ($err_total > 0) {
		display_error(sprintf(_("%d error(s) across %d transaction line(s). Rows with errors will not be imported."),
			$err_total, count($trans)));
	}
	if ($unalloc > 0) {
		display_note(sprintf(_("%d valid transaction(s) have no GL account. Select one (or define a rule) before importing."), $unalloc));
	}
	if (!$vat_available)
		display_note(_("VAT splitting is unavailable: no active tax type with a GL account is configured (Setup > Taxes)."));

	echo "<center><label style='font-weight:bold;color:#a00;'><input type='checkbox' name='backup_confirm' value='1'> "
		._("I confirm I have made a backup before importing")."</label></center>";
	// shortcut for the user who has not made that backup yet
	if (user_check_access('SA_BACKUP'))
		echo "<center><span style='color:#666'>"._("Not backed up yet?")."</span> "
			.viewer_link(_("Open the Backup window"), "admin/backups.php")."</center>";
	echo "<center>".viewer_link(_("Create new GL account"), "gl/manage/gl_accounts.php")."</center><br>";

	submit_center('import', _("Import to General Ledger"), true, false, 'nonajax');
	echo "</form>";
}

end_page();
