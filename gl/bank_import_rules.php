<?php
/**********************************************************************
    [CUSTOM] Bank statement import — allocation rules management.
    Map recurring bank description substrings to GL accounts so imported
    transactions auto-allocate before manual review. Part of Banking & GL.
    Also supports CSV export/import of the rule list (portable between
    companies).
***********************************************************************/
$page_security = 'SA_BANKIMPORT';
$path_to_root = "..";
include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/gl/includes/db/bank_import_rules_db.inc");

// ---------------------------------------------------------------------------
//  CSV export (must run before any HTML is emitted)
// ---------------------------------------------------------------------------
if (isset($_GET['export'])) {
	header('Content-Type: text/csv; charset=utf-8');
	header('Content-Disposition: attachment; filename="bank_import_rules.csv"');
	echo "pattern,gl_account_code,gl_account_name,trans_type,vat\n";
	$exp = get_bank_import_rules(true);
	while ($r = db_fetch($exp)) {
		echo '"'.str_replace('"', '""', $r['pattern']).'",'
			.'"'.$r['gl_account'].'",'
			.'"'.str_replace('"', '""', $r['account_name']).'",'
			.$r['trans_type'].','.(int)$r['vat']."\n";
	}
	exit;
}

$js = "";
if ($SysPrefs->use_popup_windows)
	$js .= get_js_open_window(800, 500);
page(_($help_context = "Bank import allocation rules"), false, false, "", $js);

// ---------------------------------------------------------------------------
//  Resolve an account code or name to a numeric code (validated against the
//  chart). Returns null if not found.
// ---------------------------------------------------------------------------
function bank_import_resolve_account($gl)
{
	if ($gl === '' || $gl === null)
		return null;
	$esc = db_escape($gl);
	$res = db_query("SELECT account_code FROM ".TB_PREF."chart_master
		WHERE inactive=0 AND (account_code=$esc OR account_name=$esc
		OR CONCAT(account_code,' - ',account_name)=$esc) LIMIT 1");
	if (db_num_rows($res) > 0) {
		$row = db_fetch($res);
		return $row['account_code'];
	}
	return null;
}

// render a "Type" select (0=both, 1=payment, 2=deposit)
function bank_import_type_select($name, $value)
{
	echo "<tr><td class='label'>"._("Type")."</td><td><select name='$name'>";
	foreach (array(0 => _("Both"), 1 => _("Payment"), 2 => _("Deposit")) as $v => $lbl)
		echo "<option value='$v' ".($v == $value ? "selected" : "").">$lbl</option>";
	echo "</select></td></tr>";
}

// ---------------------------------------------------------------------------
//  Handle import (CSV upload)
// ---------------------------------------------------------------------------
if (isset($_POST['_import']) && isset($_FILES['imp_rules']) && $_FILES['imp_rules']['tmp_name'] != '') {
	$fp = @fopen($_FILES['imp_rules']['tmp_name'], 'r');
	if (!$fp) {
		display_error(_("Cannot read the rules file."));
	} else {
		$added = 0; $skipped = 0; $dup = 0; $warn = ''; $first = true;
		while (($data = fgetcsv($fp, 0, ',')) !== false) {
			if ($first) { $first = false; continue; } // skip header row
			if (count($data) < 3) continue;
			$pattern = trim($data[0]);
			$gl = trim($data[1]);
			$name = isset($data[2]) ? trim($data[2]) : '';
			$trans_type = isset($data[3]) ? (int)trim($data[3]) : 0;
			$vat = (isset($data[4]) && trim($data[4]) !== '') ? (int)trim($data[4]) : 0;
			if ($pattern === '') { $skipped++; continue; }

			if (bank_import_rule_exists($pattern)) {
				// re-importing the same CSV must not duplicate rules
				$dup++;
				continue;
			}
			$code = bank_import_resolve_account($gl);
			if ($code === null && $name !== '')
				$code = bank_import_resolve_account($name);
			if ($code === null) {
				$warn .= sprintf(_("Skipped '%s': account not found. "), $pattern);
				$skipped++;
				continue;
			}
			add_bank_import_rule($pattern, $code, $trans_type, $vat);
			$added++;
		}
		fclose($fp);
		display_notification(sprintf(_("%d rule(s) imported, %d skipped, %d already existed."),
			$added, $skipped, $dup));
		if ($warn !== '')
			display_error($warn);
	}
}

// ---------------------------------------------------------------------------
//  Handle add / update / delete
// ---------------------------------------------------------------------------
if (isset($_POST['_add']) || isset($_POST['_update'])) {
	$pattern = trim(get_post('pattern'));
	$gl_account = trim(get_post('gl_account'));
	$trans_type = (int)get_post('trans_type');
	$vat = isset($_POST['vat']) ? 1 : 0;
	$inactive = isset($_POST['inactive']) ? 1 : 0;

	if ($pattern === '' || $gl_account === '') {
		display_error(_("Pattern and GL account are required."));
	} else {
		$code = bank_import_resolve_account($gl_account);
		if ($code === null) {
			display_error(sprintf(_("GL account '%s' not found."), $gl_account));
		} elseif (isset($_POST['_add']) && bank_import_rule_exists($pattern)) {
			display_error(sprintf(_("A rule for '%s' already exists - edit it instead."), $pattern));
		} else {
			if (isset($_POST['_add']))
				add_bank_import_rule($pattern, $code, $trans_type, $vat);
			else
				update_bank_import_rule((int)get_post('id'), $pattern, $code, $trans_type, $inactive, $vat);
			display_notification(_("Rule saved."));
		}
	}
}
if (isset($_POST['_delete'])) {
	delete_bank_import_rule((int)get_post('id'));
	display_notification(_("Rule deleted."));
}

// edit pre-fill (GET selected_id — the "Edit" link is a GET)
$selected_id = isset($_GET['selected_id']) ? (int)$_GET['selected_id'] : (int)get_post('selected_id', -1);
$edit = array('id' => -1, 'pattern' => '', 'gl_account' => '', 'trans_type' => 0, 'vat' => 0, 'inactive' => 0);
if ($selected_id > 0) {
	$res = db_query("SELECT * FROM ".TB_PREF."bank_import_rules WHERE id=".db_escape($selected_id));
	if (db_num_rows($res) > 0)
		$edit = db_fetch($res);
	else
		$selected_id = -1;
}

// GL account list for the datalist
$gl_rows = db_query("SELECT account_code, account_name FROM ".TB_PREF."chart_master
	WHERE inactive=0 ORDER BY account_code");

start_form();

echo "<datalist id='gl_acc_list'>";
while ($g = db_fetch($gl_rows))
	echo "<option value='".$g['account_code']."'>".$g['account_code']." - ".$g['account_name']."</option>";
echo "</datalist>";

start_table(TABLESTYLE2);
if ($selected_id > 0) {
	hidden('id', $selected_id);
	text_row(_("Pattern"), 'pattern', $edit['pattern'], 30, 100);
	echo "<tr><td class='label'>"._("GL Account")."</td><td><input name='gl_account' list='gl_acc_list' value='".$edit['gl_account']."'></td></tr>";
	bank_import_type_select('trans_type', $edit['trans_type']);
	check_row(_("VAT inclusive (split net + VAT)"), 'vat', $edit['vat']);
	check_row(_("Inactive"), 'inactive', $edit['inactive']);
	submit_center_first('_update', _("Update"), _("Update this rule"), false);
	echo "<a href='".htmlspecialchars($_SERVER['PHP_SELF'])."'>"._("Cancel")."</a></center>";
} else {
	text_row(_("Pattern"), 'pattern', '', 30, 100);
	echo "<tr><td class='label'>"._("GL Account")."</td><td><input name='gl_account' list='gl_acc_list' placeholder='"._("type account code or name")."'></td></tr>";
	bank_import_type_select('trans_type', 0);
	check_row(_("VAT inclusive (split net + VAT)"), 'vat', 0);
	submit_center('_add', _("Add Rule"), true, false, false);
}
end_table(1);
end_form();

// create GL account shortcut + export link
echo "<center>".viewer_link(_("Create new GL account"), "gl/manage/gl_accounts.php")."</center><br>";
echo "<center><a href='".htmlspecialchars($_SERVER['PHP_SELF'])."?export=1'>"._("Export rules (CSV)")."</a></center><br>";

start_form(true);
start_table(TABLESTYLE2);
file_row(_("Import rules CSV"), 'imp_rules', 'imp_rules');
echo "<tr><td></td><td>"._("CSV columns: pattern, gl_account_code, gl_account_name, trans_type (header row skipped).")."</td></tr>";
submit_center('_import', _("Import Rules"), true, false, false);
end_table(1);
end_form();

// ---------------------------------------------------------------------------
//  List of existing rules
// ---------------------------------------------------------------------------
start_table(TABLESTYLE);
$th = array(_("Pattern"), _("GL Account"), _("Type"), _("VAT"), _("Status"), "", "");
table_header($th);
$rules = get_bank_import_rules(true);
$k = 0;
while ($r = db_fetch($rules)) {
	alt_table_row_color($k);
	label_cell($r['pattern']);
	label_cell($r['gl_account'].' - '.$r['account_name']);
	label_cell($r['trans_type'] == 1 ? _("Payment") : ($r['trans_type'] == 2 ? _("Deposit") : _("Both")));
	label_cell(!empty($r['vat']) ? _("Yes") : _("No"));
	label_cell($r['inactive'] ? "<span style='color:red'>"._("Inactive")."</span>" : _("Active"));
	echo "<td><a href='".htmlspecialchars($_SERVER['PHP_SELF'])."?selected_id=".$r['id']."'>"._("Edit")."</a></td>";
	echo "<td><form method='post' style='display:inline'>"
		."<input type='hidden' name='id' value='".$r['id']."'>"
		."<input type='submit' name='_delete' value='"._("Delete")."' onclick='return confirm(\""._("Delete this rule?")."\")'></form></td>";
	end_row();
}
end_table(1);

end_page();
