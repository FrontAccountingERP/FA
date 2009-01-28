<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU Affero General Public License,
	AGPL, as published by the Free Software Foundation, either version 
	3 of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/agpl-3.0.html>.
***********************************************************************/
$page_security = 1;
$path_to_root="../..";

include($path_to_root . "/includes/session.inc");

page(_("View Bank Deposit"), true);

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");

include_once($path_to_root . "/gl/includes/gl_db.inc");

if (isset($_GET["trans_no"]))
{
	$trans_no = $_GET["trans_no"];
}

// get the pay-to bank payment info
$result = get_bank_trans(systypes::bank_deposit(), $trans_no);

if (db_num_rows($result) != 1)
	display_db_error("duplicate payment bank transaction found", "");

$to_trans = db_fetch($result);

$company_currency = get_company_currency();

$show_currencies = false;

if ($to_trans['bank_curr_code'] != $company_currency)
{
	$show_currencies = true;
}

echo "<center>";

display_heading(_("GL Deposit") . " #$trans_no");

echo "<br>";
start_table("$table_style width=80%");

if ($show_currencies)
{
	$colspan1 = 5;
	$colspan2 = 8;
}
else
{
	$colspan1 = 3;
	$colspan2 = 6;
}
start_row();
label_cells(_("To Bank Account"), $to_trans['bank_account_name'], "class='tableheader2'");
if ($show_currencies)
	label_cells(_("Currency"), $to_trans['bank_curr_code'], "class='tableheader2'");
label_cells(_("Amount"), number_format2($to_trans['amount'], user_price_dec()), "class='tableheader2'", "align=right");
label_cells(_("Date"), sql2date($to_trans['trans_date']), "class='tableheader2'");
end_row();
start_row();
label_cells(_("From"), payment_person_types::person_name($to_trans['person_type_id'], $to_trans['person_id']), "class='tableheader2'", "colspan=$colspan1");
label_cells(_("Deposit Type"), bank_account_types::transfer_type($to_trans['account_type']), "class='tableheader2'");
end_row();
start_row();
label_cells(_("Reference"), $to_trans['ref'], "class='tableheader2'", "colspan=$colspan2");
end_row();
comments_display_row(systypes::bank_deposit(), $trans_no);

end_table(1);

is_voided_display(systypes::bank_deposit(), $trans_no, _("This deposit has been voided."));

$items = get_gl_trans(systypes::bank_deposit(), $trans_no);

if (db_num_rows($items) == 0)
{
	display_note(_("There are no items for this deposit."));
}
else
{

	display_heading2(_("Items for this Deposit"));
	if ($show_currencies)
		display_heading2(_("Item Amounts are Shown in :") . " " . $company_currency);

    start_table("$table_style width=80%");
    $th = array(_("Account Code"), _("Account Description"),
    	_("Amount"), _("Memo"));
    table_header($th);

    $k = 0; //row colour counter
	$total_amount = 0;

    while ($item = db_fetch($items))
    {

		if ($item["account"] != $to_trans["account_code"])
		{
    		alt_table_row_color($k);

        	label_cell($item["account"]);
    		label_cell($item["account_name"]);
            amount_cell(abs($item["amount"]));
    		label_cell($item["memo_"]);
    		end_row();
    		$total_amount += abs($item["amount"]);
		}
	}

	label_row(_("Total"), number_format2($total_amount, user_price_dec()),"colspan=2 align=right", "align=right");

	end_table(1);

	display_allocations_from($to_trans['person_type_id'], $to_trans['person_id'], 2, $trans_no, $to_trans['amount']);
}

end_page(true);
?>