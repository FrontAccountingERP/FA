<?php

$page_security = 2;
$path_to_root="../..";
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

//-----------------------------------------------------------------------------------
// Ajax updates
//
if (get_post('SearchOrders')) 
{
	$Ajax->activate('orders_tbl');
} elseif (get_post('_OrderNumber_changed')) 
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

end_table();
end_form();

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

function change_tpl_flag($id)
{
	global	$Ajax;
	
  	$sql = "UPDATE ".TB_PREF."sales_orders SET type = !type WHERE order_no=$id";

  	db_query($sql, "Can't change sales order type");
	$Ajax->activate('orders_tbl');
}
//---------------------------------------------------------------------------------------------
$id = find_submit('_chgtpl');
if ($id != -1)
	change_tpl_flag($id);

//---------------------------------------------------------------------------------------------

$sql = "SELECT ".TB_PREF."sales_orders.order_no, ".TB_PREF."debtors_master.curr_code, ".TB_PREF."debtors_master.name, ".TB_PREF."cust_branch.br_name,
	".TB_PREF."sales_orders.ord_date, ".TB_PREF."sales_orders.deliver_to, ".TB_PREF."sales_orders.delivery_date,
	".TB_PREF."sales_orders.type, ";
$sql .= " Sum(".TB_PREF."sales_order_details.qty_sent) AS TotDelivered, ";
$sql .= " Sum(".TB_PREF."sales_order_details.quantity) AS TotQuantity, ";
$sql .= " Sum(".TB_PREF."sales_order_details.unit_price*".TB_PREF."sales_order_details.quantity*(1-".TB_PREF."sales_order_details.discount_percent)) AS OrderValue, ";

//if ($_POST['order_view_mode']=='InvoiceTemplates' || $_POST['order_view_mode']=='DeliveryTemplates')
  $sql .= TB_PREF."sales_orders.comments, ";
//else
  $sql .= TB_PREF."sales_orders.customer_ref";

$sql .=	" FROM ".TB_PREF."sales_orders, ".TB_PREF."sales_order_details, ".TB_PREF."debtors_master, ".TB_PREF."cust_branch
		WHERE ".TB_PREF."sales_orders.order_no = ".TB_PREF."sales_order_details.order_no
			AND ".TB_PREF."sales_orders.debtor_no = ".TB_PREF."debtors_master.debtor_no
			AND ".TB_PREF."sales_orders.branch_code = ".TB_PREF."cust_branch.branch_code
			AND ".TB_PREF."debtors_master.debtor_no = ".TB_PREF."cust_branch.debtor_no ";

//figure out the sql required from the inputs available
if (isset($_POST['OrderNumber']) && $_POST['OrderNumber'] != "")
{
// if ($_POST['OrderNumber'] != '*')  // TODO paged table
	$sql .= " AND ".TB_PREF."sales_orders.order_no LIKE '%". $_POST['OrderNumber'] ."'";
 $sql .= " GROUP BY ".TB_PREF."sales_orders.order_no";
}
else
{
  	if ($_POST['order_view_mode']!='DeliveryTemplates' && $_POST['order_view_mode']!='InvoiceTemplates')
  	{
		$date_after = date2sql($_POST['OrdersAfterDate']);
		$date_before = date2sql($_POST['OrdersToDate']);

		$sql .= " AND ".TB_PREF."sales_orders.ord_date >= '$date_after'";
		$sql .= " AND ".TB_PREF."sales_orders.ord_date <= '$date_before'";
  	}
	if ($selected_customer != -1)
		$sql .= " AND ".TB_PREF."sales_orders.debtor_no='" . $selected_customer . "'";

	if (isset($selected_stock_item))
		$sql .= " AND ".TB_PREF."sales_order_details.stk_code='". $selected_stock_item ."'";

	if (isset($_POST['StockLocation']) && $_POST['StockLocation'] != reserved_words::get_all())
		$sql .= " AND ".TB_PREF."sales_orders.from_stk_loc = '". $_POST['StockLocation'] . "' ";

	if ($_POST['order_view_mode']=='OutstandingOnly')
		$sql .= " AND ".TB_PREF."sales_order_details.qty_sent < ".TB_PREF."sales_order_details.quantity";
	elseif ($_POST['order_view_mode']=='InvoiceTemplates' || $_POST['order_view_mode']=='DeliveryTemplates')
		$sql .= " AND ".TB_PREF."sales_orders.type=1";

	$sql .= " GROUP BY ".TB_PREF."sales_orders.order_no, ".TB_PREF."sales_orders.debtor_no, ".TB_PREF."sales_orders.branch_code,
		".TB_PREF."sales_orders.customer_ref, ".TB_PREF."sales_orders.ord_date, ".TB_PREF."sales_orders.deliver_to";

} //end not order number selected
$result = db_query($sql,"No orders were returned");
//-----------------------------------------------------------------------------------
if ($result)
{
	start_form();
	/*show a table of the orders returned by the sql */
	div_start('orders_tbl');

	start_table("$table_style colspan=6 width=95%");
	$th = array(_("Order #"), _("Customer"), _("Branch"), _("Cust Order #"), _("Order Date"),
		_("Required By"), _("Delivery To"), _("Order Total"), _("Currency"), "");

  	if($_POST['order_view_mode']=='InvoiceTemplates' || $_POST['order_view_mode']=='DeliveryTemplates')
	{
		$th[3] = _('Description');
	} elseif ($_POST['order_view_mode'] != 'OutstandingOnly') {
		$th[9] = _('Tmpl');
	 $th[] =''; $th[] ='';
	} 

	table_header($th);

	$j = 1;
	$k = 0; //row colour counter
	$overdue_items = false;
	while ($myrow = db_fetch($result))
	{
		$view_page = get_customer_trans_view_str(systypes::sales_order(), $myrow["order_no"]);
		$formated_del_date = sql2date($myrow["delivery_date"]);
		$formated_order_date = sql2date($myrow["ord_date"]);
		if (isset($_POST['Update']) && 
				check_value( "chgtpl".$myrow["order_no"]) != $myrow["type"]) {
				change_tpl_flag($myrow["order_no"]);
				$myrow['type'] = !$myrow['type'];
		}
//	    $not_closed =  $myrow['type'] && ($myrow["TotDelivered"] < $myrow["TotQuantity"]);

    	// if overdue orders, then highlight as so
    	if ($myrow['type'] == 0 && date1_greater_date2(Today(), $formated_del_date))
    	{
        	 start_row("class='overduebg'");
        	 $overdue_items = true;
    	}
    	else
    	{
			alt_table_row_color($k);
    	}

		label_cell($view_page);
		label_cell($myrow["name"]);
		label_cell($myrow["br_name"]);
	  	if($_POST['order_view_mode']=='InvoiceTemplates' || $_POST['order_view_mode']=='DeliveryTemplates')
		  	label_cell($myrow["comments"]);
	  	else
		  	label_cell($myrow["customer_ref"]);
		label_cell($formated_order_date);
		label_cell($formated_del_date);
		label_cell($myrow["deliver_to"]);
		amount_cell($myrow["OrderValue"]);
		label_cell($myrow["curr_code"]);
		if ($_POST['order_view_mode']=='OutstandingOnly'/* || $not_closed*/)
		{
    		$delivery_note = $path_to_root . "/sales/customer_delivery.php?" . SID . "OrderNumber=" .$myrow["order_no"];
    		label_cell("<a href='$delivery_note'>" . _("Dispatch") . "</a>");
		}
  		elseif ($_POST['order_view_mode']=='InvoiceTemplates')
		{
    		$select_order= $path_to_root . "/sales/sales_order_entry.php?" . SID . "NewInvoice=" .$myrow["order_no"];
    		label_cell("<a href='$select_order'>" . _("Invoice") . "</a>");
		}
  		elseif ($_POST['order_view_mode']=='DeliveryTemplates')
		{
  			$select_order= $path_to_root . "/sales/sales_order_entry.php?" . SID . "NewDelivery=" .$myrow["order_no"];
    		label_cell("<a href='$select_order'>" . _("Delivery") . "</a>");
		}
		else
		{
		  	check_cells( null, "chgtpl" .$myrow["order_no"], $myrow["type"], true, 
				_('Set this order as a template for direct deliveries/invoices'));

  		  	$modify_page = $path_to_root . "/sales/sales_order_entry.php?" . SID . "ModifyOrderNumber=" . $myrow["order_no"];
  		  	label_cell("<a href='$modify_page'>" . _("Edit") . "</a>");
  		  	label_cell(print_document_link($myrow['order_no'], _("Print"), true, 30));
		}
		end_row();;

		$j++;
		if ($j == 12)
		{
			$j = 1;
			table_header($th);
		}
		//end of page full new headings if
	}
	//end of while loop
	end_table();

   if ($overdue_items)
   		display_note(_("Marked items are overdue."), 0, 1, "class='overduefg'");
	else
		echo '<br>';
	div_end();
	submit_center('Update', _("Update"), true, '', null);
	end_form();
}

echo "<br>";
end_page();
?>