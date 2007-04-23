<?php

$page_security = 2;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

include($path_to_root . "/sales/includes/sales_ui.inc");

$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 600);
if ($use_date_picker)
	$js .= get_js_date_picker();

if (isset($_GET['OutstandingOnly']) && ($_GET['OutstandingOnly'] == true)) 
{
	$_POST['OutstandingOnly'] = true;
	page(_("Search Outstanding Sales Orders"), false, false, "", $js);
} 
else 
{
	$_POST['OutstandingOnly'] = false;
	page(_("Search All Sales Orders"), false, false, "", $js);
}

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

start_form(false, false, $_SERVER['PHP_SELF'] ."?OutstandingOnly=" . $_POST['OutstandingOnly'] .SID);

start_table("class='tablestyle_noborder'");
start_row();
ref_cells(_("#:"), 'OrderNumber');
date_cells(_("from:"), 'OrdersAfterDate', null, -30);
date_cells(_("to:"), 'OrdersToDate', null, 1);

locations_list_cells(_("Location:"), 'StockLocation', null, true);

stock_items_list_cells(_("Item:"), 'SelectStockFromList', null, true);

submit_cells('SearchOrders', _("Search"));

hidden('OutstandingOnly', $_POST['OutstandingOnly']);

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

//---------------------------------------------------------------------------------------------

$sql = "SELECT ".TB_PREF."sales_orders.order_no, ".TB_PREF."debtors_master.curr_code, ".TB_PREF."debtors_master.name, ".TB_PREF."cust_branch.br_name,
	".TB_PREF."sales_orders.customer_ref, ".TB_PREF."sales_orders.ord_date, ".TB_PREF."sales_orders.deliver_to, ".TB_PREF."sales_orders.delivery_date, ";
$sql .= " Sum(".TB_PREF."sales_order_details.qty_invoiced) AS TotInvoiced, ";
$sql .= " Sum(".TB_PREF."sales_order_details.quantity) AS TotQuantity, ";

$sql .= " Sum(".TB_PREF."sales_order_details.unit_price*".TB_PREF."sales_order_details.quantity*(1-".TB_PREF."sales_order_details.discount_percent)) AS OrderValue
	FROM ".TB_PREF."sales_orders, ".TB_PREF."sales_order_details, ".TB_PREF."debtors_master, ".TB_PREF."cust_branch
		WHERE ".TB_PREF."sales_orders.order_no = ".TB_PREF."sales_order_details.order_no
			AND ".TB_PREF."sales_orders.debtor_no = ".TB_PREF."debtors_master.debtor_no
			AND ".TB_PREF."sales_orders.branch_code = ".TB_PREF."cust_branch.branch_code
			AND ".TB_PREF."debtors_master.debtor_no = ".TB_PREF."cust_branch.debtor_no ";

//figure out the sql required from the inputs available
if (isset($_POST['OrderNumber']) && $_POST['OrderNumber'] != "") 
{
	$sql .= " AND ".TB_PREF."sales_orders.order_no LIKE '%". $_POST['OrderNumber'] ."' GROUP BY ".TB_PREF."sales_orders.order_no";
} 
else 
{

	$date_after = date2sql($_POST['OrdersAfterDate']);
	$date_before = date2sql($_POST['OrdersToDate']);

	$sql .= " AND ".TB_PREF."sales_orders.ord_date >= '$date_after'";
	$sql .= " AND ".TB_PREF."sales_orders.ord_date <= '$date_before'";

	if ($selected_customer != -1)
		$sql .= " AND ".TB_PREF."sales_orders.debtor_no='" . $selected_customer . "'";

	if (isset($selected_stock_item))
		$sql .= " AND ".TB_PREF."sales_order_details.stk_code='". $selected_stock_item ."'";

	if (isset($_POST['StockLocation']) && $_POST['StockLocation'] != reserved_words::get_all())
		$sql .= " AND ".TB_PREF."sales_orders.from_stk_loc = '". $_POST['StockLocation'] . "' ";

	if ($_POST['OutstandingOnly'] == true)
		$sql .= " AND ".TB_PREF."sales_order_details.qty_invoiced < ".TB_PREF."sales_order_details.quantity";

	$sql .= " GROUP BY ".TB_PREF."sales_orders.order_no, ".TB_PREF."sales_orders.debtor_no, ".TB_PREF."sales_orders.branch_code,
		".TB_PREF."sales_orders.customer_ref, ".TB_PREF."sales_orders.ord_date, ".TB_PREF."sales_orders.deliver_to";

} //end not order number selected

$result = db_query($sql,"No orders were returned");

//-----------------------------------------------------------------------------------

if ($result) 
{

	/*show a table of the orders returned by the sql */

	start_table("$table_style colspan=6 width=95%");
	$th = array(_("Order #"), _("Customer"), _("Branch"), _("Cust Order #"), _("Order Date"),
		_("Required By"), _("Delivery To"), _("Order Total"), _("Currency"), "", "");
	table_header($th);

	$j = 1;
	$k = 0; //row colour counter
	$overdue_items = false;
	while ($myrow = db_fetch($result)) 
	{

		$view_page = get_customer_trans_view_str(systypes::sales_order(), $myrow["order_no"]);
		$formated_del_date = sql2date($myrow["delivery_date"]);
		$formated_order_date = sql2date($myrow["ord_date"]);

    	// if overdue orders, then highlight as so
    	if (date1_greater_date2(Today(), $formated_del_date))
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
		label_cell($myrow["customer_ref"]);
		label_cell($formated_order_date);
		label_cell($formated_del_date);
		label_cell($myrow["deliver_to"]);
		amount_cell($myrow["OrderValue"]);
		label_cell($myrow["curr_code"]);

		if ($_POST['OutstandingOnly'] == true || $myrow["TotInvoiced"] < $myrow["TotQuantity"]) 
		{
    		$modify_page = $path_to_root . "/sales/sales_order_entry.php?" . SID . "ModifyOrderNumber=" . $myrow["order_no"];
    		$issue_invoice = $path_to_root . "/sales/customer_invoice.php?" . SID . "OrderNumber=" .$myrow["order_no"];

    		label_cell("<a href='$modify_page'>" . _("Edit") . "</a>");
    		label_cell("<a href='$issue_invoice'>" . _("Invoice") . "</a>");
		}
		else
		{
    		label_cell("");
    		label_cell("");
		}
		end_row();;

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
   		display_note(_("Marked items are overdue."), 0, 1, "class='overduefg'");
}

echo "<br>";

end_page();
?>

