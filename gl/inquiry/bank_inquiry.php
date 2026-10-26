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
$page_security = 'SA_BANKTRANSVIEW';
$path_to_root="../..";
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/db_pager.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/gl/includes/gl_db.inc");
include_once($path_to_root . "/includes/banking.inc");

//-----------------------------------------------------------------------------------
// Free text search helpers.
//
// A term matches a row when it is found in the row text (type, reference, date,
// payee/person, memo, formatted amounts) or - when it parses as an amount - when
// it matches the transaction amount. Optional comparison operators (>, <, >=,
// <=, <>) are supported for amounts. Multiple terms must ALL match (AND).
//-----------------------------------------------------------------------------------
if (!function_exists('bank_inquiry_parse_terms'))
{
	function bank_inquiry_parse_terms($search)
	{
		$search = trim((string)$search);
		if ($search === '')
			return array();
		return preg_split('/\s+/', $search, -1, PREG_SPLIT_NO_EMPTY);
	}

	// Returns array('op' => ..., 'value' => float) when $term is an amount,
	// otherwise null. Accepts "1,234.56", "1 234,56", "R 1234.56", "(500.00)",
	// ">1000", "<-250".
	function bank_inquiry_parse_amount_term($term)
	{
		$t = trim($term);
		if ($t === '')
			return null;

		$op = '=';
		if (preg_match('/^(>=|<=|<>|=>|=<|>|<|=)/', $t, $m)) {
			$op = $m[1];
			$t = trim(substr($t, strlen($m[1])));
			if ($op == '=>') $op = '>=';
			if ($op == '=<') $op = '<=';
		}
		// drop a leading currency marker
		$t = preg_replace('/^(R|\$|\x{20AC}|\x{00A3})\s*/u', '', $t);

		$neg = false;
		if (preg_match('/^\((.+)\)$/', $t, $m)) {	// accounting negatives (100.00)
			$neg = true;
			$t = trim($m[1]);
		}
		$t = str_replace(array(" ", "\xC2\xA0", "\xE2\x80\xAF", "'"), '', $t);

		if (!preg_match('/^-?[\d.,]+$/', $t) || !preg_match('/\d/', $t))
			return null;

		$lastComma = strrpos($t, ',');
		$lastDot = strrpos($t, '.');
		if ($lastComma !== false && $lastDot !== false) {
			if ($lastComma > $lastDot) {			// 1.234,56
				$t = str_replace('.', '', $t);
				$t = str_replace(',', '.', $t);
			} else {								// 1,234.56
				$t = str_replace(',', '', $t);
			}
		} elseif ($lastComma !== false) {
			if (preg_match('/,\d{1,2}$/', $t))		// 1234,56
				$t = str_replace(',', '.', $t);
			else									// 1,234
				$t = str_replace(',', '', $t);
		}
		if (!is_numeric($t))
			return null;

		$value = (float)$t;
		if ($neg)
			$value = -$value;

		return array('op' => $op, 'value' => $value);
	}

	function bank_inquiry_amount_match($op, $wanted, $amount)
	{
		$amount = (float)$amount;
		// bank inquiry shows debits as positive amounts and credits as negative,
		// so an amount typed the way it reads on a statement ("1 234.56") has to
		// match either sign unless the user gave one explicitly.
		$candidates = ($wanted < 0 ? array($amount) : array($amount, -$amount));

		switch ($op) {
			case '>':  return $amount > $wanted;
			case '<':  return $amount < $wanted;
			case '>=': return $amount >= $wanted;
			case '<=': return $amount <= $wanted;
			case '<>': return abs($amount - $wanted) >= 0.005;
			default:
				foreach ($candidates as $c)
					if (abs($c - $wanted) < 0.005)
						return true;
				return false;
		}
	}

	function bank_inquiry_row_match($terms, $haystack, $amount)
	{
		foreach ($terms as $term) {
			if (stripos($haystack, $term) !== false)
				continue;
			$num = bank_inquiry_parse_amount_term($term);
			if ($num !== null && bank_inquiry_amount_match($num['op'], $num['value'], $amount))
				continue;
			return false;
		}
		return true;
	}
}

$js = "";
if ($SysPrefs->use_popup_windows)
	$js .= get_js_open_window(800, 500);
if (user_use_date_picker())
	$js .= get_js_date_picker();
page(_($help_context = "Bank Account Inquiry"), isset($_GET['bank_account']) && !isset($_GET['TransAfterDate']), false, "", $js, false, "", true);

check_db_has_bank_accounts(_("There are no bank accounts defined in the system."));

//-----------------------------------------------------------------------------------
// Ajax updates
//
if (get_post('Show'))
{
	$Ajax->activate('trans_tbl');
}
//------------------------------------------------------------------------------------------------

if (isset($_GET['bank_account']))
	$_POST['bank_account'] = $_GET['bank_account'];

start_form();

// One shared datalist for the GL account filter (type-as-you-type, same control
// the bank import / allocation rules pages use).
echo "<datalist id='gl_acc_list'>";
$gl_res = get_gl_accounts();
while ($gl_row = db_fetch($gl_res))
	echo "<option value='".htmlspecialchars($gl_row['account_code'])."'>"
		.htmlspecialchars($gl_row['account_code']." - ".$gl_row['account_name'])."</option>";
echo "</datalist>";

start_table(TABLESTYLE_NOBORDER);
start_row();
if (!$page_nested)
	bank_accounts_list_cells(_("Account:"), 'bank_account', null);

date_cells(_("From:"), 'TransAfterDate', '', null, -user_transaction_days());
date_cells(_("To:"), 'TransToDate');
end_row();

start_row();
text_cells(_("Search:"), 'search', null, 30, 60, _('Amount, description, reference or payee. Prefix an amount with &gt; or &lt; to search a value range.'));
check_cells(_("All dates:"), 'search_all_dates', null, false, _('Ignore the From/To dates while searching'));

// GL account filter: restrict the list to transactions that were coded to one
// account (the counter/offset account of the bank transaction). Type a code, a
// name, or pick from the list; an empty box means every account.
echo "<td>"._("GL Account:")."</td><td>";
echo "<input name='gl_account' list='gl_acc_list' size='22' maxlength='60'"
	." style='max-width:180px' placeholder='"._('All accounts')."'"
	." value='".htmlspecialchars(trim(html_entity_decode(get_post('gl_account', ''), ENT_QUOTES, 'UTF-8')))."'"
	." title='"._('Only show the transactions posted to this GL account')."'>";
echo "</td>\n";

submit_cells('Show',_("Show"),'', '', 'default');
end_row();
end_table();
end_form();

//------------------------------------------------------------------------------------------------

if (!isset($_POST['bank_account']))
	$_POST['bank_account'] = "";

$from_date = get_post('TransAfterDate', '');
$to_date = get_post('TransToDate', '');
$search = trim(html_entity_decode(get_post('search', ''), ENT_QUOTES, 'UTF-8'));
$terms = bank_inquiry_parse_terms($search);

// Resolve a typed code / name / "code - name" to a real chart account. Inactive
// accounts are allowed here: the inquiry is read-only and history on a retired
// account is exactly what people come looking for.
$gl_typed = trim(html_entity_decode(get_post('gl_account', ''), ENT_QUOTES, 'UTF-8'));
$gl_filter = '';
$gl_info = null;
if ($gl_typed !== '')
{
	$gl_esc = db_escape($gl_typed);
	$gl_q = db_query("SELECT account_code, account_name FROM ".TB_PREF."chart_master
		WHERE account_code=$gl_esc OR account_name=$gl_esc
			OR CONCAT(account_code,' - ',account_name)=$gl_esc LIMIT 1");
	if (db_num_rows($gl_q))
	{
		$gl_info = db_fetch($gl_q);
		$gl_filter = $gl_info['account_code'];
	}
	else
		display_error(sprintf(_("GL account '%s' was not found - showing all accounts."),
			htmlspecialchars($gl_typed)));
}

// "All dates" widens the range to the account's whole history; the running
// balance stays truthful because the opening balance is taken from before the
// first transaction shown.
if (check_value('search_all_dates'))
{
	$span = get_bank_trans_date_range($_POST['bank_account']);
	if ($span && !empty($span['first_date']))
	{
		$from_date = sql2date($span['first_date']);
		$to_date = sql2date($span['last_date']);
	}
}

$result = get_bank_trans_for_bank_account($_POST['bank_account'], $from_date, $to_date);	

// Transactions coded to the selected GL account (empty array = no filter).
$gl_keys = $gl_filter !== ''
	? get_bank_trans_gl_account_keys($gl_filter, $from_date, $to_date)
	: array();

div_start('trans_tbl');
if (!$page_nested)
{
	$act = get_bank_account($_POST["bank_account"]);
	display_heading($act['bank_account_name']." - ".$act['bank_curr_code']);
}

start_table(TABLESTYLE);

$th = array(_("Type"), _("#"), _("Reference"), _("Date"),
	_("Debit"), _("Credit"), _("Balance"), _("Person/Item"), _("Memo"), "", "");
table_header($th);

$bfw = get_balance_before_for_bank_account($_POST['bank_account'], $from_date);

$credit = $debit = 0;
start_row("class='inquirybg' style='font-weight:bold'");
label_cell(_("Opening Balance")." - ".$from_date, "colspan=4");
display_debit_or_credit_cells($bfw);
label_cell("");
label_cell("", "colspan=4");

end_row();
$running_total = $bfw;
if ($bfw > 0 ) 
	$debit += $bfw;
else 
	$credit += $bfw;
$j = 1;
$k = 0; //row colour counter
$rows_in_range = 0;
$rows_matched = 0;
$matched_debit = $matched_credit = 0;
while ($myrow = db_fetch($result))
{
	$rows_in_range++;

	$running_total += $myrow["amount"];

	$trandate = sql2date($myrow["trans_date"]);
	$person = payment_person_name($myrow["person_type_id"],$myrow["person_id"]);
	$memo = get_comments_string($myrow["type"], $myrow["trans_no"]);

 	if ($myrow["amount"] > 0 ) 
 		$debit += $myrow["amount"];
 	else 
 		$credit += $myrow["amount"];

	// GL account filter: only rows coded to the selected account
	if ($gl_filter !== '' && !isset($gl_keys[$myrow["type"].":".$myrow["trans_no"]]))
		continue;

	// free text search: match the text of the row, or the amount itself
	if (count($terms))
	{
		$haystack = $systypes_array[$myrow["type"]]."	".$myrow["ref"]."	".$trandate."	"
			.$myrow["trans_date"]."	"
			.price_format(abs($myrow["amount"]))."	".price_format($myrow["amount"])."	"
			.($person === null ? '' : $person)."	".($memo === null ? '' : $memo);
		if (!bank_inquiry_row_match($terms, $haystack, $myrow["amount"]))
			continue;
	}

	$rows_matched++;
	if ($myrow["amount"] > 0)
		$matched_debit += $myrow["amount"];
	else
		$matched_credit += $myrow["amount"];

	alt_table_row_color($k);

	label_cell($systypes_array[$myrow["type"]]);
	label_cell(get_trans_view_str($myrow["type"],$myrow["trans_no"]));
	label_cell(get_trans_view_str($myrow["type"],$myrow["trans_no"],$myrow['ref']));
	label_cell($trandate);
	display_debit_or_credit_cells($myrow["amount"]);
	amount_cell($running_total);

	label_cell($person);

	label_cell($memo);
	label_cell(get_gl_view_str($myrow["type"], $myrow["trans_no"]));
	if (!$page_nested)
		label_cell(trans_editor_link($myrow["type"], $myrow["trans_no"]));

	end_row();

	if ($j == 12)
	{
		$j = 1;
		table_header($th);
	}
	$j++;
}
//end of while loop

if (count($terms) || $gl_filter !== '')
{
	if ($rows_matched == 0)
	{
		start_row("class='inquirybg'");
		label_cell(check_value('search_all_dates')
			? _("No transactions match your search.")
			: _("No transactions match your search in this period. Tick 'All dates' to search the account's whole history."),
			"colspan=11");
		end_row();
	}
	else
	{
		$filter_parts = array();
		if (count($terms))
			$filter_parts[] = "'".htmlspecialchars($search)."'";
		if ($gl_filter !== '')
			$filter_parts[] = _("GL account")." ".htmlspecialchars($gl_info['account_code']." - ".$gl_info['account_name']);

		start_row("class='inquirybg' style='font-weight:bold'");
		label_cell(sprintf(_("%d of %d transaction(s) match %s"), $rows_matched, $rows_in_range, implode(" + ", $filter_parts)), "colspan=4");
		amount_cell($matched_debit);
		amount_cell($matched_credit);
		label_cell("");
		label_cell("", "colspan=4");
		end_row();
	}
}

start_row("class='inquirybg' style='font-weight:bold'");
label_cell(_("Ending Balance")." - ". $to_date, "colspan=4");
amount_cell($debit);
amount_cell(-$credit);
//display_debit_or_credit_cells($running_total);
amount_cell($debit+$credit);
label_cell("", "colspan=4");
end_row();
end_table(2);
div_end();
//------------------------------------------------------------------------------------------------

end_page();

