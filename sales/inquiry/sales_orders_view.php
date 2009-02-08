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
$page_security = 2;
$path_to_root="../..";

include($path_to_root . "/includes/db_pager.inc");
include($path_to_root . "/includes/session.inc");
include($path_to_root . "/sales/includes/sales_ui.inc");
include_once($path_to_root . "/reporting/includes/reporting.inc");

$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 600);
if ($use_date_picker)
	$js .= get_js_date_picker();

if (isset($_GET['OutstandingOnly']) && ($_GET['OutstandingOnly'] == true))
{
	$_POST['order_view_mode'] = 'OutstandingOnly';
	$_SESSION['page_title'] = _("Search Outstanding Sales Orders");
}
elseif (isset($_GET['InvoiceTemplates']) && ($_GET['InvoiceTemplates'] == true))
{
	$_POST['order_view_mode'] = 'InvoiceTemplates';
	$_SESSION['page_title'] = _("Search Template for Invoicing");
}
elseif (isset($_GET['DeliveryTemplates']) && ($_GET['DeliveryTemplates'] == true))
{
	$_POST['order_view_mode'] = 'DeliveryTemplates';
	$_SESSION['page_title'] = _("Select Template for Delivery");
}
elseif (!isset($_POST['order_view_mode']))
{
	$_POST['order_view_mode'] = false;
	$_SESSION['page_title'] = _("Search All Sales Orders");
}

page($_SESSION['page_title'], false, false, "", $js);

if (isset($_GET['selected_customer']))
{
	$selected_customer = $_GET['selected_customer'];
}
elseif (isset($_POST['selected_customer']))
{
	$selected_customer = $_POST['selected_customer'];
}
else
	$selected_customer = -1;

//---------------------------------------------------------------------------------------------

if (isset($_POST['SelectStockFromList']) && ($_POST['SelectStockFromList'] != "") &&
	($_POST['SelectStockFromList'] != reserved_words::get_all()))
{
 	$selected_stock_item = $_POST['SelectStockFromList'];
}
else
{
	unset($selected_stock_item);
}
//---------------------------------------------------------------------------------------------
//	Query format functions
//
function check_overdue($row)
{
	return ($row['type'] == 0
		&& date1_greater_date2(Today(), sql2date($row['ord_date']))
		&& ($row['TotDelivered'] < $row['TotQuantity']));
}

function view_link($dummy, $order_no)
{
	return  get_customer_trans_view_str(systypes::sales_order(), $order_no);
}

function prt_link($row)
{
	return print_document_link($row['order_no'], _("Print"), true, 30, ICON_PRINT);
}

function edit_link($row) 
{
  return pager_link( _("Edit"),
    "/sales/sales_order_entry.php?" . SID . "ModifyOrderNumber=" . $row['order_no'], ICON_EDIT);
}

function dispatch_link($row)
{
  return pager_link( _("Dispatch"),
	"/sales/customer_delivery.php?" . SID . "OrderNumber=" .$row['order_no'], ICON_DOC);
}

function invoice_link($row)
{
  return pager_link( _("Invoice"),
	"/sales/sales_order_entry.php?" . SID . "NewInvoice=" .$row["order_no"], ICON_DOC);
}

function delivery_link($row)
{
  return pager_link( _("Delivery"),
	"/sales/sales_order_entry.php?" . SID . "NewDelivery=" .$row['order_no'], ICON_DOC);
}

function tmpl_checkbox($row)
{
	$name = "chgtpl" .$row['order_no'];
	$value = $row['type'] ? 1:0;

// save also in hidden field for testing during 'Update'

 return checkbox(null, $name, $value, true,
 	_('Set this order as a template for direct deliveries/invoices'))
	. hidden('last['.$row['order_no'].']', $value, false);
}
//---------------------------------------------------------------------------------------------
// Update db record if respective checkbox value has changed.
//
function change_tpl_flag($id)
{
	global	$Ajax;
	
  	$sql = "UPDATE ".TB_PREF."sales_orders SET type = !type WHERE order_no=$id";

  	db_query($sql, "Can't change sales order type");
	$Ajax->activate('orders_tbl');
}

$id = find_submit('_chgtpl');
if ($id != -1)
	change_tpl_flag($id);

if (isset($_POST['Update']) && isset($_POST['last'])) {
	foreach($_POST['last'] as $id => $value)
		if ($value != check_value('chgtpl'.$id))
			change_tpl_flag($id);
}

//---------------------------------------------------------------------------------------------
//	Order range form
//
if (get_post('_OrderNumber_changed')) // enable/disable selection controls
{
	$disable = get_post('OrderNumber') !== '';

  	if ($_POST['order_view_mode']!='DeliveryTemplates' 
		&& $_POST['order_view_mode']!='InvoiceTemplates') {
			$Ajax->addDisable(true, 'OrdersAfterDate', $disable);
			$Ajax->addDisable(true, 'OrdersToDate', $disable);
	}
	$Ajax->addDisable(true, 'StockLocation', $disable);
	$Ajax->addDisable(true, '_SelectStockFromList_edit', $disable);
	$Ajax->addDisable(true, 'SelectStockFromList', $disable);

	if ($disable) {
		$Ajax->addFocus(true, 'OrderNumber');
	} else
		$Ajax->addFocus(true, 'OrdersAfterDate');

	$Ajax->activate('orders_tbl');
}

start_form(false, false, $_SERVER['PHP_SELF'] .SID);

start_table("class='tablestyle_noborder'");
start_row();
ref_cells(_("#:"), 'OrderNumber', '',null, '', true);
if ($_POST['order_view_mode'] != 'DeliveryTemplates' && $_POST['order_view_mode'] != 'InvoiceTemplates')
{
  	date_cells(_("from:"), 'OrdersAfterDate', '', null, -30);
  	date_cells(_("to:"), 'OrdersToDate', '', null, 1);
}
locations_list_cells(_("Location:"), 'StockLocation', null, true);

stock_items_list_cells(_("Item:"), 'SelectStockFromList', null, true);

submit_cells('SearchOrders', _("Search"),'',_('Select documents'), true);

hidden('order_view_mode', $_POST['order_view_mode']);

end_row();

end_table(1);
end_form();
//---------------------------------------------------------------------------------------------
//	Orders inquiry table
//
$sql = "SELECT 
		sorder.order_no,
		debtor.name,
		branch.br_name,"
		.($_POST['order_view_mode']=='InvoiceTemplates' 
		   	|| $_POST['order_view_mode']=='DeliveryTemplates' ?
		 "sorder.comments, " : "sorder.customer_ref, ")
		."sorder.ord_date,
		sorder.delivery_date,
		sorder.deliver_to,
		Sum(line.unit_price*line.quantity*(1-line.discount_percent)) AS OrderValue,
		sorder.type,
		debtor.curr_code,
		Sum(line.qty_sent) AS TotDelivered,
		Sum(line.quantity) AS TotQuantity
	FROM ".TB_PREF."sales_orders as sorder, "
		.TB_PREF."sales_order_details as line, "
		.TB_PREF."debtors_master as debtor, "
		.TB_PREF."cust_branch as branch
		WHERE sorder.order_no = line.order_no
		AND sorder.debtor_no = debtor.debtor_no
		AND sorder.branch_code = branch.branch_code
		AND debtor.debtor_no = branch.debtor_no";

if (isset($_POST['OrderNumber']) && $_POST['OrderNumber'] != "")
{
	// search orders with number like ...
	$sql .= " AND sorder.order_no LIKE '%". $_POST['OrderNumber'] ."'"
 			." GROUP BY sorder.order_no";
}
else	// ... or select inquiry constraints
{
  	if ($_POST['order_view_mode']!='DeliveryTemplates' && $_POST['order_view_mode']!='InvoiceTemplates')
  	{
		$date_after = date2sql($_POST['OrdersAfterDate']);
		$date_before = date2sql($_POST['OrdersToDate']);

		$sql .=  " AND sorder.ord_date >= '$date_after'"
				." AND sorder.ord_date <= '$date_before'";
  	}
	if ($selected_customer != -1)
		$sql .= " AND sorder.debtor_no='" . $selected_customer . "'";

	if (isset($selected_stock_item))
		$sql .= " AND line.stk_code='". $selected_stock_item ."'";

	if (isset($_POST['StockLocation']) && $_POST['StockLocation'] != reserved_words::get_all())
		$sql .= " AND sorder.from_stk_loc = '". $_POST['StockLocation'] . "' ";

	if ($_POST['order_view_mode']=='OutstandingOnly')
		$sql .= " AND line.qty_sent < line.quantity";
	elseif ($_POST['order_view_mode']=='InvoiceTemplates' || $_POST['order_view_mode']=='DeliveryTemplates')
		$sql .= " AND sorder.type=1";

	$sql .= " GROUP BY sorder.order_no,
				sorder.debtor_no,
				sorder.branch_code,
				sorder.customer_ref,
				sorder.ord_date,
				sorder.deliver_to";
}

$cols = array(
	_("Order #") => array('fun'=>'view_link'),
	_("Customer"),
	_("Branch"), 
	_("Comments"),
	_("Order Date") => 'date',
	_("Required By") =>array('type'=>'date', 'ord'=>''),
	_("Delivery To"), 
	_("Order Total") => array('type'=>'amount', 'ord'=>''),
	'Type' => 'skip',
	_("Currency") => array('align'=>'center')
);

if ($_POST['order_view_mode'] == 'OutstandingOnly') {
	array_replace($cols, 3, 1, _("Cust Order Ref"));
	array_append($cols, array(array('insert'=>true, 'fun'=>'dispatch_link')));

} elseif ($_POST['order_view_mode'] == 'InvoiceTemplates') {
	array_replace($cols, 3, 1, _("Description"));
	array_append($cols, array( array('insert'=>true, 'fun'=>'invoice_link')));

} else if ($_POST['order_view_mode'] == 'DeliveryTemplates') {
	array_replace($cols, 3, 1, _("Description"));
	array_append($cols, array(
			array('insert'=>true, 'fun'=>'delivery_link'))
	);

} else {
	 array_append($cols,array(
			_("Tmpl") => array('insert'=>true, 'fun'=>'tmpl_checkbox'),
					array('insert'=>true, 'fun'=>'edit_link'),
					array('insert'=>true, 'fun'=>'prt_link')));
};


$table =& new_db_pager('orders_tbl', $sql, $cols);
$table->set_marker('check_overdue', _("Marked items are overdue."));

if (get_post('SearchOrders')) {
	$table->set_sql($sql);
	$table->set_columns($cols);
}
$table->width = "80%";
start_form();

display_db_pager($table);
submit_center('Update', _("Update"), true, '', null);

end_form();
end_page();
?>