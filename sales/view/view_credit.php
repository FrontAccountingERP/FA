<?php

$page_security = 1;
$path_to_root="../..";
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");

include_once($path_to_root . "/sales/includes/sales_db.inc");

$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
page(_("View Credit Note"), true, false, "", $js);

if (isset($_GET["trans_no"]))
{
	$trans_id = $_GET["trans_no"];
} 
elseif (isset($_POST["trans_no"]))
{
	$trans_id = $_POST["trans_no"];
}

$myrow = get_customer_trans($trans_id, 11);

$branch = get_branch($myrow["branch_code"]);

display_heading("<font color=red>" . sprintf(_("CREDIT NOTE #%d"), $trans_id). "</font>");
echo "<br>";

start_table("$table_style2 width=95%");
echo "<tr valign=top><td>"; // outer table

/*Now the customer charged to details in a sub table*/
start_table("$table_style width=100%");
$th = array(_("Customer"));
table_header($th);

label_row(null, $myrow["DebtorName"] . "<br>" . nl2br($myrow["address"]), "nowrap");

end_table();
/*end of the small table showing charge to account details */

echo "</td><td>"; // outer table

start_table("$table_style width=100%");
$th = array(_("Branch"));
table_header($th);

label_row(null, $branch["br_name"] . "<br>" . nl2br($branch["br_address"]), "nowrap");
end_table();

echo "</td><td>"; // outer table

start_table("$table_style width=100%");
start_row();
label_cells(_("Ref"), $myrow["reference"], "class='tableheader2'");
label_cells(_("Date"), sql2date($myrow["tran_date"]), "class='tableheader2'");
label_cells(_("Currency"), $myrow["curr_code"], "class='tableheader2'");
end_row();
start_row();
label_cells(_("Sales Type"), $myrow["sales_type"], "class='tableheader2'");
label_cells(_("Shipping Company"), $myrow["shipper_name"], "class='tableheader2'");
end_row();
comments_display_row(11, $trans_id);
end_table();

echo "</td></tr>";
end_table(1); // outer table

$sub_total = 0;

$result = get_customer_trans_details(11, $trans_id);

start_table("$table_style width=95%");

if (db_num_rows($result) > 0)
{
	$th = array(_("Item Code"), _("Item Description"), _("Quantity"),
		_("Unit"), _("Price"), _("Discount %"), _("Total"));
	table_header($th);	

	$k = 0;	//row colour counter
	$sub_total = 0;

	while ($myrow2 = db_fetch($result))
	{

		alt_table_row_color($k);

		$value = round(((1 - $myrow2["discount_percent"]) * $myrow2["unit_price"] * $myrow2["quantity"]), 
		   user_price_dec());
		$sub_total += $value;

		if ($myrow2["discount_percent"] == 0)
		{
			$display_discount = "";
		} 
		else 
		{
		   $display_discount = percent_format($myrow2["discount_percent"]*100) . "%";
		}

		label_cell($myrow2["stock_id"]);
		label_cell($myrow2["StockDescription"]);
		qty_cell($myrow2["quantity"]);
		label_cell($myrow2["units"], "align=right");
		amount_cell($myrow2["unit_price"]);
		label_cell($display_discount, "align=right");
		amount_cell($value);
		end_row();
	} //end while there are line items to print out
} 
else
	display_note(_("There are no line items on this credit note."), 1, 2);

$display_sub_tot = price_format($sub_total);
$display_freight = price_format($myrow["ov_freight"]);

$credit_total = $myrow["ov_freight"]+$myrow["ov_gst"]+$myrow["ov_amount"]+$myrow["ov_freight_tax"];
$display_total = price_format($credit_total);

/*Print out the invoice text entered */
if ($sub_total != 0)
	label_row(_("Sub Total"), $display_sub_tot, "colspan=6 align=right",
		"nowrap align=right width=15%");
label_row(_("Shipping"), $display_freight, "colspan=6 align=right", "nowrap align=right");

$tax_items = get_customer_trans_tax_details(11, $trans_id);
display_customer_trans_tax_details($tax_items, 6);

label_row("<font color=red>" . _("TOTAL CREDIT") . "</font",
	"<font color=red>$display_total</font>", "colspan=6 align=right", "nowrap align=right");
end_table(1);

$voided = is_voided_display(11, $trans_id, _("This credit note has been voided."));

if (!$voided)
	display_allocations_from(payment_person_types::customer(), 
		$myrow['debtor_no'], 11, $trans_id, $credit_total);

/* end of check to see that there was an invoice record to print */

end_page(true);

?>