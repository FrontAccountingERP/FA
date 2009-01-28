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
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/sales/includes/sales_db.inc");

$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 600);

page(_("View Customer Payment"), true, false, "", $js);

if (isset($_GET["trans_no"]))
{
	$trans_id = $_GET["trans_no"];
}

$receipt = get_customer_trans($trans_id, systypes::cust_payment());

display_heading(sprintf(_("Customer Payment #%d"),$trans_id));

echo "<br>";
start_table("$table_style width=80%");
start_row();
label_cells(_("From Customer"), $receipt['DebtorName'], "class='tableheader2'");
label_cells(_("Into Bank Account"), $receipt['bank_account_name'], "class='tableheader2'");
label_cells(_("Date of Deposit"), sql2date($receipt['tran_date']), "class='tableheader2'");
end_row();
start_row();
label_cells(_("Payment Currency"), $receipt['curr_code'], "class='tableheader2'");
label_cells(_("Amount"), price_format($receipt['ov_amount']), "class='tableheader2'");
label_cells(_("Discount"), price_format($receipt['ov_discount']), "class='tableheader2'");
end_row();
start_row();
label_cells(_("Payment Type"), 
	bank_account_types::transfer_type($receipt['BankTransType']), "class='tableheader2'");
label_cells(_("Reference"), $receipt['reference'], "class='tableheader2'", "colspan=4");
end_row();
comments_display_row(systypes::cust_payment(), $trans_id);

end_table(1);

$voided = is_voided_display(systypes::cust_payment(), $trans_id, _("This customer payment has been voided."));

if (!$voided)
{
	display_allocations_from(payment_person_types::customer(), $receipt['debtor_no'], systypes::cust_payment(), $trans_id, $receipt['Total']);
}

end_page(true);
?>