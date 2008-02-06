<?php

$page_security = 1;
$path_to_root="../..";
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/sales/includes/sales_ui.inc");

include_once($path_to_root . "/sales/includes/sales_db.inc");

$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 600);
page(_("View Sales Dispatch"), true, false, "", $js);


if (isset($_GET["trans_no"]))
{
	$trans_id = $_GET["trans_no"];
} 
elseif (isset($_POST["trans_no"]))
{
	$trans_id = $_POST["trans_no"];
}

// 3 different queries to get the information - what a JOKE !!!!

$myrow = get_customer_trans($trans_id, 13);

$branch = get_branch($myrow["branch_code"]);

$sales_order = get_sales_order($myrow["order_"]);

display_heading(_("DISPATCH NOTE") .' '. _('#').$trans_id);

echo "<br>";
start_table("$table_style2 width=95%");
echo "<tr valign=top><td>"; // outer table

/*Now the customer charged to details in a sub table*/
start_table("$table_style width=100%");
$th = array(_("Charge To"));
table_header($th);

label_row(null, $myrow["DebtorName"] . "<br>" . nl2br($myrow["address"]), "nowrap");

end_table();

/*end of the small table showing charge to account details */

echo "</td><td>"; // outer table

/*end of the main table showing the company name and charge to details */

start_table("$table_style width=100%");
$th = array(_("Charge Branch"));
table_header($th);

label_row(null, $branch["br_name"] . "<br>" . nl2br($branch["br_address"]), "nowrap");
end_table();

echo "</td><td>"; // outer table

start_table("$table_style width=100%");
$th = array(_("Delivered To"));
table_header($th);

label_row(null, $sales_order["deliver_to"] . "<br>" . nl2br($sales_order["delivery_address"]),
	"nowrap");
end_table();

echo "</td><td>"; // outer table

start_table("$table_style width=100%");
start_row();
label_cells(_("Reference"), $myrow["reference"], "class='tableheader2'");
label_cells(_("Currency"), $sales_order["curr_code"], "class='tableheader2'");
label_cells(_("Our Order No"), 
	get_customer_trans_view_str(systypes::sales_order(),$sales_order["order_no"]), "class='tableheader2'");
end_row();
start_row();
label_cells(_("Customer Order Ref."), $sales_order["customer_ref"], "class='tableheader2'");
label_cells(_("Shipping Company"), $myrow["shipper_name"], "class='tableheader2'");
label_cells(_("Sales Type"), $myrow["sales_type"], "class='tableheader2'");
end_row();
start_row();
label_cells(_("Dispatch Date"), sql2date($myrow["tran_date"]), "class='tableheader2'", "nowrap");
label_cells(_("Due Date"), sql2date($myrow["due_date"]), "class='tableheader2'", "nowrap");
end_row();
comments_display_row(13, $trans_id);
end_table();

echo "</td></tr>";
end_table(1); // outer table


$result = get_customer_trans_details(13, $trans_id);

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
		if($myrow2['quantity']==0) continue;
		alt_table_row_color($k);

		$net = ((1 - $myrow2["discount_percent"]) * $myrow2["FullUnitPrice"] * -$myrow2["quantity"]);
		$sub_total += $net;

	    if ($myrow2["discount_percent"] == 0)
	    {
		  	$display_discount = "";
	    } 
	    else 
	    {
		  	$display_discount = number_format2($myrow2["discount_percent"]*100,user_percent_dec()) . "%";
	    }

	label_cell($myrow2["stock_id"]);
	label_cell($myrow2["StockDescription"]);
        qty_cell(-$myrow2["quantity"]);
        label_cell($myrow2["units"], "align=right");
        amount_cell($myrow2["FullUnitPrice"]);
        label_cell($display_discount, "nowrap align=right");
        amount_cell($net);
	end_row();
	} //end while there are line items to print out

} 
else
	display_note(_("There are no line items on this dispatch."), 1, 2);

$display_sub_tot = number_format2($sub_total,user_price_dec());
$display_freight = number_format2($myrow["ov_freight"],user_price_dec());

/*Print out the delivery note text entered */
label_row(_("Sub-total"), $display_sub_tot, "colspan=6 align=right", 
	"nowrap align=right width=15%");
label_row(_("Shipping"), $display_freight, "colspan=6 align=right", "nowrap align=right");

$tax_items = get_customer_trans_tax_details(13, $trans_id);
display_customer_trans_tax_details($tax_items, 6);

$display_total = number_format2($myrow["ov_freight"]+$myrow["ov_amount"],user_price_dec());

label_row(_("TOTAL VALUE"), $display_total, "colspan=6 align=right",
	"nowrap align=right");
end_table(1);

is_voided_display(13, $trans_id, _("This dispatch has been voided."));

end_page(true);

?>