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
$page_security = 'SA_SUPPTRANSVIEW';
$path_to_root = "../..";
include($path_to_root . "/purchasing/includes/po_class.inc");

include($path_to_root . "/includes/session.inc");
include($path_to_root . "/purchasing/includes/purchasing_ui.inc");

$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
page(_($help_context = "View Purchase Order"), true, false, "", $js);


if (!isset($_GET['trans_no']))
{
	die ("<br>" . _("This page must be called with a purchase order number to review."));
}

display_heading(_("Purchase Order") . " #" . $_GET['trans_no']);

$purchase_order = new purch_order;

read_po($_GET['trans_no'], $purchase_order);
echo "<br>";
display_po_summary($purchase_order, true);

start_table("$table_style width=90%", 6);
echo "<tr><td valign=top>"; // outer table

display_heading2(_("Line Details"));

start_table("colspan=9 $table_style width=100%");

$th = array(_("Item Code"), _("Item Description"), _("Quantity"), _("Unit"), _("Price"),
	_("Line Total"), _("Requested By"), _("Quantity Received"), _("Quantity Invoiced"));
table_header($th);
$total = $k = 0;
$overdue_items = false;

foreach ($purchase_order->line_items as $stock_item)
{

	$line_total = $stock_item->quantity * $stock_item->price;

	// if overdue and outstanding quantities, then highlight as so
	if (($stock_item->quantity - $stock_item->qty_received > 0)	&&
		date1_greater_date2(Today(), $stock_item->req_del_date))
	{
    	start_row("class='overduebg'");
    	$overdue_items = true;
	}
	else
	{
		alt_table_row_color($k);
	}

	label_cell($stock_item->stock_id);
	label_cell($stock_item->item_description);
	$dec = get_qty_dec($stock_item->stock_id);
	qty_cell($stock_item->quantity, false, $dec);
	label_cell($stock_item->units);
	amount_decimal_cell($stock_item->price);
	amount_cell($line_total);
	label_cell($stock_item->req_del_date);
	qty_cell($stock_item->qty_received, false, $dec);
	qty_cell($stock_item->qty_inv, false, $dec);
	end_row();

	$total += $line_total;
}

$display_total = number_format2($total,user_price_dec());
label_row(_("Total Excluding Tax/Shipping"), $display_total,
	"align=right colspan=5", "nowrap align=right", 3);

end_table();

if ($overdue_items)
	display_note(_("Marked items are overdue."), 0, 0, "class='overduefg'");

//----------------------------------------------------------------------------------------------------

$k = 0;

$grns_result = get_po_grns($_GET['trans_no']);

if (db_num_rows($grns_result) > 0)
{

    echo "</td><td valign=top>"; // outer table

    display_heading2(_("Deliveries"));
    start_table($table_style);
    $th = array(_("#"), _("Reference"), _("Delivered On"));
    table_header($th);
    while ($myrow = db_fetch($grns_result))
    {
		alt_table_row_color($k);

    	label_cell(get_trans_view_str(ST_SUPPRECEIVE,$myrow["id"]));
    	label_cell($myrow["reference"]);
    	label_cell(sql2date($myrow["delivery_date"]));
    	end_row();
    }
    end_table();;
}

$invoice_result = get_po_invoices_credits($_GET['trans_no']);

$k = 0;

if (db_num_rows($invoice_result) > 0)
{

    echo "</td><td valign=top>"; // outer table

    display_heading2(_("Invoices/Credits"));
    start_table($table_style);
    $th = array(_("#"), _("Date"), _("Total"));
    table_header($th);
    while ($myrow = db_fetch($invoice_result))
    {
    	alt_table_row_color($k);

    	label_cell(get_trans_view_str($myrow["type"],$myrow["trans_no"]));
    	label_cell(sql2date($myrow["tran_date"]));
    	amount_cell($myrow["Total"]);
    	end_row();
    }
    end_table();
}

echo "</td></tr>";

end_table(1); // outer table

//----------------------------------------------------------------------------------------------------

end_page(true);

?>
