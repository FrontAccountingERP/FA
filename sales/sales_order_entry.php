<?php

$page_security = 1;
$path_to_root="..";
include_once($path_to_root . "/sales/includes/cart_class.inc");

include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/sales/includes/sales_ui.inc");
include_once($path_to_root . "/sales/includes/ui/sales_order_ui.inc");
include_once($path_to_root . "/sales/includes/sales_db.inc");

$js = get_js_form_entry("StockID2", "stock_id", "qty");
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();

if (isset($_GET['ModifyOrderNumber'])) 
{
	page(_("Modifying Sales Order") . " #".$_GET['ModifyOrderNumber'], false, false, "", $js); 
}	
elseif (isset($_GET['NewInvoice']) || (isset($_SESSION['Items']) && $_SESSION['Items']->direct_invoice)) 
{
	page(_("Sales Invoices"), false, false, "", $js); 
} 
else 
{
	page(_("Sales Order Entry"), false, false, "", $js); 
}


//--------------------------------------------------------------------------------
if (isset($_GET['AddedID'])) 
{
	$order_no = $_GET['AddedID'];
	$trans_type = systypes::sales_order();

 	display_notification_centered(_("Order has been entered.") . " #$order_no");

	display_note(get_trans_view_str($trans_type, $order_no, _("View this order")));

	hyperlink_params($path_to_root . "/sales/customer_invoice.php", _("Confirm Order Delivery Quantities and Produce Invoice"), "OrderNumber=$order_no");

	hyperlink_params($_SERVER['PHP_SELF'], _("Enter a New Order"), "NewOrder=Yes");

	display_footer_exit();
}
//--------------------------------------------------------------------------------

if (isset($_GET['UpdatedID'])) 
{
	$order_no = $_GET['UpdatedID'];
	$trans_type = systypes::sales_order();

 	display_notification_centered(_("Order has been updated.") . " #$order_no");

	display_note(get_trans_view_str($trans_type, $order_no, _("View this order")));

	hyperlink_params($path_to_root . "/sales/customer_invoice.php", _("Confirm Order Delivery Quantities and Produce Invoice"), "OrderNumber=$order_no");

	hyperlink_params($path_to_root . "/sales/inquiry/sales_orders_view.php", _("Select A Different Order"), "OutstandingOnly=1");

	display_footer_exit();
}

//--------------------------------------------------------------------------------------------------

function copy_to_so()
{
	if ($_SESSION['Items']->direct_invoice)
		$_SESSION['Items']->memo_ = $_POST['InvoiceText'];

	$_SESSION['Items']->orig_order_date = $_POST['OrderDate'];
	$_SESSION['Items']->delivery_date = $_POST['delivery_date'];
	$_SESSION['Items']->cust_ref = $_POST['cust_ref'];
	$_SESSION['Items']->freight_cost = $_POST['freight_cost'];
	$_SESSION['Items']->Comments = $_POST['Comments'];

	$_SESSION['Items']->deliver_to = $_POST['deliver_to'];
	$_SESSION['Items']->delivery_address = $_POST["delivery_address"];
	$_SESSION['Items']->phone = $_POST["phone"];
	$_SESSION['Items']->Location = $_POST["Location"];
	$_SESSION['Items']->ship_via = $_POST["ship_via"];
}

//--------------------------------------------------------------------------------------------------

function copy_from_so()
{
	if ($_SESSION['Items']->direct_invoice)
		$_POST['InvoiceText'] = $_SESSION['Items']->memo_;

	$_POST['OrderDate'] = $_SESSION['Items']->orig_order_date;
	$_POST['delivery_date'] = $_SESSION['Items']->delivery_date;
	$_POST['cust_ref'] = $_SESSION['Items']->cust_ref;
	$_POST['freight_cost'] = $_SESSION['Items']->freight_cost;
	$_POST['Comments'] = $_SESSION['Items']->Comments;

	$_POST['deliver_to'] = $_SESSION['Items']->deliver_to;
	$_POST["delivery_address"] = $_SESSION['Items']->delivery_address;
	$_POST["phone"] = $_SESSION['Items']->phone;
	$_POST["Location"] = $_SESSION['Items']->Location;
	$_POST["ship_via"] = $_SESSION['Items']->ship_via;
}


function can_process()
{
	if ($_SESSION['Items']->direct_invoice) 
	{
		$edate = _("The entered invoice date is invalid.");
	} 
	else 
	{	
		$edate = _("The entered order date is invalid.");
	}	
	if (!is_date($_POST['OrderDate'])) 
	{
		display_error($edate);
		return false;
	}
	if ($_SESSION['Items']->direct_invoice && !is_date_in_fiscalyear($_POST['OrderDate'])) 
	{
		display_error(_("The entered date is not in fiscal year"));
		return false;
	}
	else
	{
		$_SESSION['Items']->orig_order_date = $_POST['OrderDate'];
	}
	if (count($_SESSION['Items']->line_items) == 0)
	{
		display_error(_("You must enter at least one line entry."));
		return false;
	}
	if (strlen($_POST['deliver_to']) <= 1)
	{
		display_error(_("You must enter the person or company to whom delivery should be made to."));
		return false;
	}

	if (strlen($_POST['delivery_address']) <= 1)
	{
		display_error( _("You should enter the street address in the box provided. Orders cannot be accepted without a valid street address."));
		return false;
	}

	if ($_POST['freight_cost'] == "")
		$_POST['freight_cost'] = 0;

	if (!is_numeric($_POST['freight_cost']))
	{
		display_error(_("The shipping cost entered is expected to be numeric."));
		return false;
	}

	if (!is_date($_POST['delivery_date'])) 
	{
		display_error(_("The delivery date is invalid."));
		return false;
	}

	if (date1_greater_date2($_SESSION['Items']->orig_order_date, $_POST['delivery_date'])) 
	{
		display_error(_("The requested delivery date is before the date of the order."));
		return false;
	}

	$_SESSION['Items']->deliver_to = $_POST['deliver_to'];
	$_SESSION['Items']->delivery_date = $_POST['delivery_date'];
	$_SESSION['Items']->delivery_address = $_POST['delivery_address'];
	$_SESSION['Items']->phone =$_POST['phone'];
	if (isset($_POST['email']))
		$_SESSION['Items']->email =$_POST['email'];
	else	
		$_SESSION['Items']->email = "";
	$_SESSION['Items']->Location = $_POST['Location'];
	$_SESSION['Items']->cust_ref = $_POST['cust_ref'];
	$_SESSION['Items']->Comments = $_POST['Comments'];
	$_SESSION['Items']->freight_cost = $_POST['freight_cost'];
	$_SESSION['Items']->ship_via = $_POST['ship_via'];

    return true;
}

//-----------------------------------------------------------------------------------------------------------

if (isset($_POST['ProcessOrder']) && $_SESSION['Items']->order_no == 0 && can_process())
{

	$order_no = add_sales_order($_SESSION['Items']);
	
	if ($_SESSION['Items']->direct_invoice) 
	{
		$_SESSION['Items']->memo_ = $_POST['InvoiceText'];
		$_SESSION['Items']->memo_ = str_replace("'", "\\'", $_SESSION['Items']->memo_);
		$_SESSION['Items']->order_no = $order_no;
   		meta_forward("$path_to_root/sales/customer_invoice.php", "process_invoice=Yes");
		
	} 
	else 
	{
	
		unset($_SESSION['Items']->line_items);
		unset($_SESSION['Items']);

   		meta_forward($_SERVER['PHP_SELF'], "AddedID=$order_no");
   	}	
}

//-------------------------------------------------------------------------------------------------------------

if (isset($_POST['ProcessOrder']) && $_SESSION['Items']->order_no != 0 && can_process())
{

	update_sales_order($_SESSION['Items']->order_no, $_SESSION['Items']);

	$order_no = $_SESSION['Items']->order_no;

	unset($_SESSION['Items']->line_items);
	unset($_SESSION['Items']);

	meta_forward($_SERVER['PHP_SELF'], "UpdatedID=$order_no");
}


//--------------------------------------------------------------------------------

function check_item_data() 
{
	if (!is_numeric($_POST['qty']) || $_POST['qty'] < 0 || $_POST['Disc'] > 100 || 
		$_POST['Disc'] < 0)
	{
		display_error( _("The item could not be updated because you are attempting to set the quantity ordered to less than 0, or the discount percent to more than 100."));
		return false;
	} 
	elseif($_SESSION['Items']->some_already_delivered($_POST['stock_id']) != 0 &&
		$_SESSION['Items']->line_items[$_POST['stock_id']]->price != $_POST['price']) 
	{
   		display_error(_("The item you attempting to modify the price for has already had some quantity invoiced at the old price. The item unit price cannot be modified retrospectively."));
   		return false;
   	} 
   	elseif($_SESSION['Items']->some_already_delivered($_POST['stock_id']) != 0 && 
   		$_SESSION['Items']->line_items[$_POST['stock_id']]->discount_percent != ($_POST['Disc']/100)) 
   	{
   		display_error(_("The item you attempting to modify has had some quantity invoiced at the old discount percent. The items discount cannot be modified retrospectively."));
   		return false;
   	} 
   	elseif (isset($_SESSION['Items']->line_items[$_POST['stock_id']]) && $_SESSION['Items']->line_items[$_POST['stock_id']]->qty_inv > $_POST['qty'])
   	{
   		display_error(_("You attempting to make the quantity ordered a quantity less than has already been invoiced. The quantity delivered and invoiced cannot be modified retrospectively."));
   		return false;
   	} 	
   	return true;
}

function handle_update_item() 
{
    if($_POST['UpdateItem'] != "" && check_item_data())
    {
    	$_SESSION['Items']->update_cart_item($_POST['stock_id'], $_POST['qty'], 
    		$_POST['price'], ($_POST['Disc'] / 100));
    }
}

//--------------------------------------------------------------------------------

function handle_delete_item() 
{   
    if($_GET['Delete'] != "")
    {
    	$stock_id = $_GET['Delete']; 
    	if($_SESSION['Items']->some_already_delivered($stock_id) == 0)
    	{
    		$_SESSION['Items']->remove_from_cart($stock_id);
    	} 
    	else 
    	{
    		display_error(_("This item cannot be deleted because some of it has already been invoiced."));
    	}
    }
}

//--------------------------------------------------------------------------------

function handle_new_item()
{
	if (!check_item_data())
		return;
		
	add_to_order($_SESSION['Items'], $_POST['stock_id'], $_POST['qty'], 
		$_POST['price'], $_POST['Disc']/100);
	$_POST['StockID2'] = $_POST['stock_id']	= "";
}

//--------------------------------------------------------------------------------	

function  handle_cancel_order()
{
	global $path_to_root;
	
    if ($_POST['CancelOrder'] != "") 
    {
    	$ok_to_delete = 1;	//assume this in the first instance
    
		if (($_SESSION['Items']->order_no != 0) && 
			sales_order_has_invoices($_SESSION['Items']->order_no)) 
		{
			$ok_to_delete = 0;
			display_error(_("This order cannot be cancelled because some of it has already been invoiced. However, the line item quantities may be modified."));
		}
    
    	if ($ok_to_delete == 1)
    	{
    		if($_SESSION['Items']->order_no != 0)
    		{
    			delete_sales_order($_SESSION['Items']->order_no);
    		}
    		$diriv = $_SESSION['Items']->direct_invoice;
    		$_SESSION['Items']->clear_items();
    		$_SESSION['Items'] = new cart;
    		$_SESSION['Items']->direct_invoice = $diriv;
    		if ($diriv) 
    		{
    			display_note(_("This sales invoice has been cancelled as requested."), 1);
				hyperlink_params($path_to_root . "/sales/sales_order_entry.php", _("Enter a New Sales Invoice"), SID . "&NewInvoice=Yes");
    		} 
    		else 
    		{
    			display_note(_("This sales order has been cancelled as requested."), 1);
				hyperlink_params($path_to_root . "/sales/sales_order_entry.php", _("Enter a New Sales Order"), SID . "&NewOrder=Yes");
    		}
    		br(1);
    		end_page();			
    		exit;
    	}
    }
}
	
//--------------------------------------------------------------------------------

function handle_new_order() 
{
	/*New order entry - clear any existing order details from the Items object and initiate a newy*/
	if (isset($_SESSION['Items']))
	{
		unset ($_SESSION['Items']->line_items);
		unset ($_SESSION['Items']);
	}
	
	session_register("Items");

	$_SESSION['Items'] = new cart;
	if (isset($_GET['NewInvoice']))
		$_SESSION['Items']->direct_invoice = true;
   	$_SESSION['Items']->customer_id = "";
	$_POST['OrderDate'] = Today();   	
	if (!is_date_in_fiscalyear($_POST['OrderDate']))
		$_POST['OrderDate'] = end_fiscalyear();
	$_SESSION['Items']->orig_order_date = $_POST['OrderDate'];
}

//--------------------------------------------------------------------------------	

if (isset($_GET['ModifyOrderNumber']) && $_GET['ModifyOrderNumber'] != "")
{

	if (isset($_SESSION['Items']))
	{
		unset ($_SESSION['Items']->line_items);
		unset ($_SESSION['Items']);
	}

	session_register("Items");

	$_SESSION['Items'] = new cart;
	$_SESSION['Items']->order_no = $_GET['ModifyOrderNumber']; 

	/*read in all the selected order into the Items cart  */
	
	read_sales_order($_SESSION['Items']->order_no, $_SESSION['Items']);
}

//--------------------------------------------------------------------------------

if (isset($_POST['CancelOrder']))
 	handle_cancel_order();

if (isset($_GET['Delete']) || isset($_GET['Edit']))
	copy_from_so();
	
if (isset($_GET['Delete']))
	handle_delete_item();

if (isset($_POST['UpdateItem']) || isset($_POST['AddItem']))
	copy_to_so();
	
if (isset($_POST['UpdateItem']))
	handle_update_item();

if (isset($_POST['AddItem']))
	handle_new_item();
	
//--------------------------------------------------------------------------------

if (isset($_GET['NewOrder']) || isset($_GET['NewInvoice']))
{
	handle_new_order();
} 
else 
{
	if (!isset($_POST['customer_id']))
		$_POST['customer_id'] = $_SESSION['Items']->customer_id;
	if (!isset($_POST['branch_id']))		
		$_POST['branch_id'] = $_SESSION['Items']->Branch;
	if (!isset($_POST['OrderDate']))		
		$_POST['OrderDate'] = $_SESSION['Items']->orig_order_date;
}

//--------------------------------------------------------------------------------	

check_db_has_stock_items(_("There are no inventory items defined in the system."));

check_db_has_customer_branches(_("There are no customers, or there are no customers with branches. Please define customers and customer branches."));		

if ($_SESSION['Items']->direct_invoice) 
{
	$idate = _("Invoice Date:");
	$orderitems = _("Sales Invoice Items");
	$deliverydetails = _("Enter Delivery Details and Confirm Invoice");
	$cancelorder = _("Cancel Invoice");
} 
else 
{
	$idate = _("Order Date:");
	$orderitems = _("Sales Order Items");
	$deliverydetails = _("Enter Delivery Details and Confirm Order");
	$cancelorder = _("Cancel Order");
}
start_form(false, true);

$customer_error = display_order_header($_SESSION['Items'], 
	($_SESSION['Items']->any_already_delivered() == 0), $idate);

if ($customer_error == "")
{
	start_table("$table_style width=80%", 10);
	echo "<tr><td>";		
	display_order_summary($orderitems, $_SESSION['Items'], true);
	echo "</td></tr>";
	echo "<tr><td>";		
	display_delivery_details($_SESSION['Items']);
	echo "</td></tr>";
	end_table(1);    	
} 
else
{
	display_error($customer_error);
}

if ($_SESSION['Items']->direct_invoice) 
{
	$porder = _("Place Invoice");
	$corder = _("Commit Invoice Changes");
	$eorder = _("Edit Invoice Items");
} 
else 
{
	$porder = _("Place Order");
	$corder = _("Commit Order Changes");
	$eorder = _("Edit Order Items");
}

if ($_SESSION['Items']->order_no == 0)
{
	submit_center_first('ProcessOrder', $porder);
} 
else 
{
	submit_center_first('ProcessOrder', $corder);
}

/*    
echo " ";
submit('EditOrder', $eorder);
if ($_SESSION['Items']->count_items() >= 1)
{
  	echo " ";
	submit('DeliveryDetails', $deliverydetails);
	echo "   ";
}
*/		
submit_center_last('CancelOrder', $cancelorder);
	
end_form();

//--------------------------------------------------------------------------------

if (!isset($_SESSION['Items']))
{
	/* It must be a new order being created $_SESSION['Items'] would be set up from the order
	modification code above if a modification to an existing order.  */

	session_register("Items");

	$_SESSION['Items'] = new cart;
	if (isset($_GET['NewInvoice']))
		$_SESSION['Items']->direct_invoice = true;
	$_SESSION['Items']->order_no = 0;	
}

//--------------------------------------------------------------------------------

end_page();
?>