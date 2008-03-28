<?php

$page_security = 2;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

include($path_to_root . "/purchasing/includes/purchasing_ui.inc");
include_once($path_to_root . "/reporting/includes/reporting.inc");

$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();
page(_("Search Outstanding Purchase Orders"), false, false, "", $js);

if (isset($_GET['order_number']))
{
	$_POST['order_number'] = $_GET['order_number'];
}

//---------------------------------------------------------------------------------------------

start_form(false, true);

start_table("class='tablestyle_noborder'");
start_row();
ref_cells(_("#:"), 'order_number');

date_cells(_("from:"), 'OrdersAfterDate', null, -30);
date_cells(_("to:"), 'OrdersToDate');

locations_list_cells(_("Location:"), 'StockLocation', null, true);

stock_items_list_cells(_("Item:"), 'SelectStockFromList', null, true);

submit_cells('SearchOrders', _("Search"));
end_row();
end_table();

end_form();

//---------------------------------------------------------------------------------------------

if (isset($_POST['order_number']) && ($_POST['order_number'] != ""))
{
	$order_number = $_POST['order_number'];
}

if (isset($_POST['SelectStockFromList']) && ($_POST['SelectStockFromList'] != "") &&
	($_POST['SelectStockFromList'] != $all_items))
{
 	$selected_stock_item = $_POST['SelectStockFromList'];
}
else
{
	unset($selected_stock_item);
}

$today = date2sql(Today());

//figure out the sql required from the inputs available
$sql = "SELECT ".TB_PREF."purch_orders.order_no, ".TB_PREF."suppliers.supp_name, ".TB_PREF."purch_orders.into_stock_location,
	".TB_PREF."purch_orders.ord_date, ".TB_PREF."purch_orders.requisition_no, ".TB_PREF."purch_orders.reference,
	".TB_PREF."suppliers.curr_code, ".TB_PREF."locations.location_name,
	Sum(".TB_PREF."purch_order_details.unit_price*".TB_PREF."purch_order_details.quantity_ordered) AS OrderValue,
	Sum(".TB_PREF."purch_order_details.delivery_date < '" . $today. "'
	AND (".TB_PREF."purch_order_details.quantity_ordered > ".TB_PREF."purch_order_details.quantity_received)) As OverDue
	FROM ".TB_PREF."purch_orders, ".TB_PREF."purch_order_details, ".TB_PREF."suppliers, ".TB_PREF."locations
	WHERE ".TB_PREF."purch_orders.order_no = ".TB_PREF."purch_order_details.order_no
	AND ".TB_PREF."purch_orders.supplier_id = ".TB_PREF."suppliers.supplier_id
	AND ".TB_PREF."locations.loc_code = ".TB_PREF."purch_orders.into_stock_location
	AND (".TB_PREF."purch_order_details.quantity_ordered > ".TB_PREF."purch_order_details.quantity_received) ";

if (isset($order_number) && $order_number != "")
{
	$sql .= "AND ".TB_PREF."purch_orders.reference LIKE '%". $order_number . "%'";
}
else
{

	$data_after = date2sql($_POST['OrdersAfterDate']);
	$data_before = date2sql($_POST['OrdersToDate']);

	$sql .= "  AND ".TB_PREF."purch_orders.ord_date >= '$data_after'";
	$sql .= "  AND ".TB_PREF."purch_orders.ord_date <= '$data_before'";

	if (isset($_POST['StockLocation']) && $_POST['StockLocation'] != $all_items)
	{
		$sql .= " AND ".TB_PREF."purch_orders.into_stock_location = '". $_POST['StockLocation'] . "' ";
	}

	if (isset($selected_stock_item))
	{
		$sql .= " AND ".TB_PREF."purch_order_details.item_code='". $selected_stock_item ."' ";
	}
} //end not order number selected

$sql .= " GROUP BY ".TB_PREF."purch_orders.order_no";

$result = db_query($sql,"No orders were returned");

print_hidden_script(18);

/*show a table of the orders returned by the sql */

start_table("$table_style colspan=7 width=80%");

if (isset($_POST['StockLocation']) && $_POST['StockLocation'] == $all_items)
	$th = array(_("#"), _("Reference"), _("Supplier"), _("Location"),
		_("Supplier's Reference"), _("Order Date"), _("Currency"), _("Order Total"),"","","");
else
	$th = array(_("#"), _("Reference"), _("Supplier"),
		_("Supplier's Reference"), _("Order Date"), _("Currency"), _("Order Total"),"","","");

table_header($th);

$j = 1;
$k = 0; //row colour counter
$overdue_items = false;
while ($myrow = db_fetch($result))
{

	if ($myrow["OverDue"] == 1)
	{
		 start_row("class='overduebg'");
		 $overdue_items = true;
	}
	else
	{
		alt_table_row_color($k);
	}

	$modify = "$path_to_root/purchasing/po_entry_items.php?" . SID . "ModifyOrderNumber=" . $myrow["order_no"];
	$receive = "$path_to_root/purchasing/po_receive_items.php?" . SID . "PONumber=" . $myrow["order_no"];

	$date = sql2date($myrow["ord_date"]);

	label_cell(get_trans_view_str(systypes::po(), $myrow["order_no"]));
	label_cell($myrow["reference"]);
	label_cell($myrow["supp_name"]);
	if (isset($_POST['StockLocation']) && $_POST['StockLocation'] == $all_items)
		label_cell($myrow["location_name"]);
	label_cell($myrow["requisition_no"]);
	label_cell($date);
	label_cell($myrow["curr_code"]);
	amount_cell($myrow["OrderValue"]);
	label_cell("<a href=$modify>" . _("Edit") . "</a>");
  	label_cell(print_document_link($myrow['order_no'], _("Print")));
	label_cell("<a href=$receive>" . _("Receive") . "</a>");
	end_row();

	$j++;
	If ($j == 12)
	{
		$j = 1;
		table_header($th);
	}
//end of page full new headings if
}
//end of while loop

end_table();

if ($overdue_items)
	display_note(_("Marked orders have overdue items."), 0, 1, "class='overduefg'");

end_page();
?>
