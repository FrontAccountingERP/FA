<?php

$page_security = 2;
$path_to_root="../..";
include_once($path_to_root . "/sales/includes/cart_class.inc");

include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/date_functions.inc");

include_once($path_to_root . "/sales/includes/sales_ui.inc");
include_once($path_to_root . "/sales/includes/sales_db.inc");

$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 600);

page(_("View Sales Order"), true, false, "", $js);

display_heading(sprintf(_("Sales Order #%d"),$_GET['trans_no']));

if (isset($_SESSION['Items']))
{
	unset ($_SESSION['Items']);
}

$_SESSION['Items'] = new Cart(30, $_GET['trans_no'], true);

start_table("$table_style2 width=95%", 5);
echo "<tr valign=top><td>";
display_heading2(_("Order Information"));
echo "</td><td>";
display_heading2(_("Deliveries"));
echo "</td><td>";
display_heading2(_("Invoices/Credits"));
echo "</td></tr>";

echo "<tr valign=top><td>";

start_table("$table_style width=95%");
label_row(_("Customer Name"), $_SESSION['Items']->customer_name, "class='tableheader2'",
	"colspan=3");
start_row();
label_cells(_("Customer Order Ref."), $_SESSION['Items']->cust_ref, "class='tableheader2'");
label_cells(_("Deliver To Branch"), $_SESSION['Items']->deliver_to, "class='tableheader2'");
end_row();
start_row();
label_cells(_("Ordered On"), $_SESSION['Items']->document_date, "class='tableheader2'");
label_cells(_("Requested Delivery"), $_SESSION['Items']->due_date, "class='tableheader2'");
end_row();
start_row();
label_cells(_("Order Currency"), $_SESSION['Items']->customer_currency, "class='tableheader2'");
label_cells(_("Deliver From Location"), $_SESSION['Items']->location_name, "class='tableheader2'");
end_row();

label_row(_("Delivery Address"), nl2br($_SESSION['Items']->delivery_address),
	"class='tableheader2'", "colspan=3");
label_row(_("Telephone"), $_SESSION['Items']->phone, "class='tableheader2'", "colspan=3");
label_row(_("E-mail"), "<a href='mailto:" . $_SESSION['Items']->email . "'>" . $_SESSION['Items']->email . "</a>",
	"class='tableheader2'", "colspan=3");
label_row(_("Comments"), $_SESSION['Items']->Comments, "class='tableheader2'", "colspan=3");
end_table();

echo "</td><td valign='top'>";

start_table($table_style);
display_heading2(_("Delivery Notes"));

$th = array(_("#"), _("Ref"), _("Date"), _("Total"));
table_header($th);

$sql = "SELECT * FROM ".TB_PREF."debtor_trans WHERE type=13 AND order_=" . $_GET['trans_no'];
$result = db_query($sql,"The related delivery notes could not be retreived");

$delivery_total = 0;
$k = 0;

while ($del_row = db_fetch($result))
{

	alt_table_row_color($k);

	$this_total = $del_row["ov_freight"]+ $del_row["ov_amount"] + $del_row["ov_freight_tax"]  + $del_row["ov_gst"] ;
	$delivery_total += $this_total;

	label_cell(get_customer_trans_view_str($del_row["type"], $del_row["trans_no"]));
	label_cell($del_row["reference"]);
	label_cell(sql2date($del_row["tran_date"]));
	amount_cell($this_total);
	end_row();

}

label_row(null, price_format($delivery_total), "", "colspan=4 align=right");

end_table();
echo "</td><td valign='top'>";

start_table($table_style);
display_heading2(_("Sales Invoices"));

$th = array(_("#"), _("Ref"), _("Date"), _("Total"));
table_header($th);

$sql = "SELECT * FROM ".TB_PREF."debtor_trans WHERE type=10 AND order_=" . $_GET['trans_no'];
$result = db_query($sql,"The related invoices could not be retreived");

$invoices_total = 0;
$k = 0;

while ($inv_row = db_fetch($result))
{

	alt_table_row_color($k);

	$this_total = $inv_row["ov_freight"] + $inv_row["ov_freight_tax"]  + $inv_row["ov_gst"] + $inv_row["ov_amount"];
	$invoices_total += $this_total;

	label_cell(get_customer_trans_view_str($inv_row["type"], $inv_row["trans_no"]));
	label_cell($inv_row["reference"]);
	label_cell(sql2date($inv_row["tran_date"]));
	amount_cell($this_total);
	end_row();

}

label_row(null, price_format($invoices_total), "", "colspan=4 align=right");

end_table();

display_heading2(_("Credit Notes"));

start_table($table_style);
$th = array(_("#"), _("Ref"), _("Date"), _("Total"));
table_header($th);

$sql = "SELECT * FROM ".TB_PREF."debtor_trans WHERE type=11 AND order_=" . $_GET['trans_no'];
$result = db_query($sql,"The related credit notes could not be retreived");

$credits_total = 0;
$k = 0;

while ($credits_row = db_fetch($result))
{

	alt_table_row_color($k);

	$this_total = $credits_row["ov_freight"] + $credits_row["ov_freight_tax"]  + $credits_row["ov_gst"] + $credits_row["ov_amount"];
	$credits_total += $this_total;

	label_cell(get_customer_trans_view_str($credits_row["type"], $credits_row["trans_no"]));
	label_cell($credits_row["reference"]);
	label_cell(sql2date($credits_row["tran_date"]));
	amount_cell(-$this_total);
	end_row();

}

label_row(null, "<font color=red>" . price_format(-$credits_total) . "</font>",
	"", "colspan=4 align=right");


end_table();

echo "</td></tr>";

end_table();

echo "<center>";
display_heading2(_("Line Details"));

start_table("colspan=9 width=95% $table_style");
$th = array(_("Item Code"), _("Item Description"), _("Quantity"), _("Unit"),
	_("Price"), _("Discount"), _("Total"), _("Quantity Delivered"));
table_header($th);

$k = 0;  //row colour counter

foreach ($_SESSION['Items']->line_items as $stock_item) {

	$line_total = round2($stock_item->quantity * $stock_item->price * (1 - $stock_item->discount_percent),
	   user_price_dec());

	alt_table_row_color($k);

	label_cell($stock_item->stock_id);
	label_cell($stock_item->item_description);
	$dec = get_qty_dec($stock_item->stock_id);
	qty_cell($stock_item->quantity, false, $dec);
	label_cell($stock_item->units);
	amount_cell($stock_item->price);
	amount_cell($stock_item->discount_percent * 100);
	amount_cell($line_total);

	qty_cell($stock_item->qty_done, false, $dec);
	end_row();
}

$items_total = $_SESSION['Items']->get_items_total();

$display_total = price_format($items_total + $_SESSION['Items']->freight_cost);

label_row(_("Shipping"), price_format($_SESSION['Items']->freight_cost),
	"align=right colspan=6", "nowrap align=right");
label_row(_("Total Order Value"), $display_total, "align=right colspan=6",
	"nowrap align=right");

end_table(2);

end_page(true);

?>
