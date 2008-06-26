<?php

$page_security = 4;

$path_to_root="..";
include_once($path_to_root . "/purchasing/includes/po_class.inc");

include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/purchasing/includes/purchasing_ui.inc");

$js = '';
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();

if (isset($_GET['ModifyOrderNumber'])) 
{
	page(_("Modify Purchase Order #") . $_GET['ModifyOrderNumber'], false, false, "", $js);
} 
else 
{
	page(_("Purchase Order Entry"), false, false, "", $js);
}

//---------------------------------------------------------------------------------------------------

check_db_has_suppliers(_("There are no suppliers defined in the system."));

check_db_has_purchasable_items(_("There are no purchasable inventory items defined in the system."));

//---------------------------------------------------------------------------------------------------------------

if (isset($_GET['AddedID'])) 
{
	$order_no = $_GET['AddedID'];
	$trans_type = systypes::po();	

	if (!isset($_GET['Updated']))
		display_notification_centered(_("Purchase Order has been entered"));
	else
		display_notification_centered(_("Purchase Order has been updated") . " #$order_no");
	display_note(get_trans_view_str($trans_type, $order_no, _("View this order")));

	hyperlink_params($path_to_root . "/purchasing/po_receive_items.php", _("Receive Items on this Purchase Order"), "PONumber=$order_no");

	hyperlink_params($_SERVER['PHP_SELF'], _("Enter Another Purchase Order"), "NewOrder=yes");
	
	hyperlink_no_params($path_to_root."/purchasing/inquiry/po_search.php", _("Select An Outstanding Purchase Order"));
	
	display_footer_exit();	
}

//--------------------------------------------------------------------------------------------------
function line_start_focus() {
  global 	$Ajax;

  $Ajax->activate('items_table');
  set_focus('_stock_id_edit');
}
//--------------------------------------------------------------------------------------------------

function copy_to_po()
{
	$_SESSION['PO']->supplier_id = $_POST['supplier_id'];	
	$_SESSION['PO']->orig_order_date = $_POST['OrderDate'];
	$_SESSION['PO']->reference = $_POST['ref'];
	$_SESSION['PO']->requisition_no = $_POST['Requisition'];
	$_SESSION['PO']->Comments = $_POST['Comments'];	
	$_SESSION['PO']->Location = $_POST['StkLocation'];
	$_SESSION['PO']->delivery_address = $_POST['delivery_address'];
}

//--------------------------------------------------------------------------------------------------

function copy_from_po()
{
	$_POST['supplier_id'] = $_SESSION['PO']->supplier_id;	
	$_POST['OrderDate'] = $_SESSION['PO']->orig_order_date;	
    $_POST['Requisition'] = $_SESSION['PO']->requisition_no;
    $_POST['ref'] = $_SESSION['PO']->reference;
	$_POST['Comments'] = $_SESSION['PO']->Comments;
    $_POST['StkLocation'] = $_SESSION['PO']->Location;
    $_POST['delivery_address'] = $_SESSION['PO']->delivery_address;	
}

//--------------------------------------------------------------------------------------------------

function unset_form_variables() {
	unset($_POST['stock_id']);
    unset($_POST['qty']);
    unset($_POST['price']);
    unset($_POST['req_del_date']);
}

//---------------------------------------------------------------------------------------------------

function handle_delete_item($line_no)
{
	if($_SESSION['PO']->some_already_received($line_no) == 0)
	{
		$_SESSION['PO']->remove_from_order($line_no);
		unset_form_variables();
	} 
	else 
	{
		display_error(_("This item cannot be deleted because some of it has already been received."));
	}	
    line_start_focus();
}

//---------------------------------------------------------------------------------------------------

function handle_cancel_po()
{
	global $path_to_root;
	
	//need to check that not already dispatched or invoiced by the supplier
	if(($_SESSION['PO']->order_no != 0) && 
		$_SESSION['PO']->any_already_received() == 1)
	{
		display_error(_("This order cannot be cancelled because some of it has already been received.") 
			. "<br>" . _("The line item quantities may be modified to quantities more than already received. prices cannot be altered for lines that have already been received and quantities cannot be reduced below the quantity already received."));
		return;
	}
	
	if($_SESSION['PO']->order_no != 0)
	{
		delete_po($_SESSION['PO']->order_no);
	}	

	$_SESSION['PO']->clear_items();
	$_SESSION['PO'] = new purch_order;

	display_note(_("This purchase order has been cancelled."), 0, 1);

	hyperlink_params($path_to_root . "/purchasing/po_entry_items.php", _("Enter a new purchase order"), "NewOrder=Yes");
	echo "<br>";

	end_page();
	exit;
}

//---------------------------------------------------------------------------------------------------

function check_data()
{
    if (!check_num('qty',0))
    {
	   	display_error(_("The quantity of the order item must be numeric and not less than zero."));
		set_focus('qty');
	   	return false;
    }

    if (!check_num('price', 0))
    {
	   	display_error(_("The price entered must be numeric and not less than zero."));
		set_focus('price');
	   	return false;	   
    }
    if (!is_date($_POST['req_del_date'])){
    		display_error(_("The date entered is in an invalid format."));
		set_focus('req_del_date');
   		return false;    	 
    }
     
    return true;	
}

//---------------------------------------------------------------------------------------------------

function handle_update_item()
{
	$allow_update = check_data(); 

	if ($allow_update && 
		($_SESSION['PO']->line_items[$_POST['line_no']]->qty_inv > input_num('qty') ||
		$_SESSION['PO']->line_items[$_POST['line_no']]->qty_received > input_num('qty')))
	{
		display_error(_("You are attempting to make the quantity ordered a quantity less than has already been invoiced or received.  This is prohibited.") .
			"<br>" . _("The quantity received can only be modified by entering a negative receipt and the quantity invoiced can only be reduced by entering a credit note against this item."));
		set_focus('qty');
		return;
	}
	
	$_SESSION['PO']->update_order_item($_POST['line_no'], input_num('qty'), input_num('price'),
  		$_POST['req_del_date']);
	unset_form_variables();
    line_start_focus();
}

//---------------------------------------------------------------------------------------------------

function handle_add_new_item()
{
	$allow_update = check_data();
	
	if ($allow_update == true)
	{ 
		if (count($_SESSION['PO']->line_items) > 0)
		{
		    foreach ($_SESSION['PO']->line_items as $order_item) 
		    {

    			/* do a loop round the items on the order to see that the item
    			is not already on this order */
   			    if (($order_item->stock_id == $_POST['stock_id']) && 
   			    	($order_item->Deleted == false)) 
   			    {
				  	$allow_update = false;
				  	display_error(_("The selected item is already on this order."));
			    }
		    } /* end of the foreach loop to look for pre-existing items of the same code */
		}

		if ($allow_update == true)
		{
		   	$sql = "SELECT description, units, mb_flag
				FROM ".TB_PREF."stock_master WHERE stock_id = '". $_POST['stock_id'] . "'";

		    $result = db_query($sql,"The stock details for " . $_POST['stock_id'] . " could not be retrieved");

		    if (db_num_rows($result) == 0)
		    {
				$allow_update = false;
		    }		    

			if ($allow_update)
		   	{
				$myrow = db_fetch($result);
				$_SESSION['PO']->add_to_order ($_POST['line_no'], $_POST['stock_id'], input_num('qty'), 
					$myrow["description"], input_num('price'), $myrow["units"],
					$_POST['req_del_date'], 0, 0);

				unset_form_variables();
				$_POST['stock_id']	= "";
	   		} 
	   		else 
	   		{
			     display_error(_("The selected item does not exist or it is a kit part and therefore cannot be purchased."));
		   	}

		} /* end of if not already on the order and allow input was true*/
    }
	line_start_focus();
}

//---------------------------------------------------------------------------------------------------

function can_commit()
{
	if (!is_date($_POST['OrderDate'])) 
	{
		display_error(_("The entered order date is invalid."));
		set_focus('OrderDate');
		return false;
	} 
	
	if (!$_SESSION['PO']->order_no) 
	{
    	if (!references::is_valid($_SESSION['PO']->reference)) 
    	{
    		display_error(_("There is no reference entered for this purchase order."));
			set_focus('ref');
    		return false;
    	} 
    	
    	if (!is_new_reference($_SESSION['PO']->reference, systypes::po())) 
    	{
    		display_error(_("The entered reference is already in use."));
			set_focus('ref');
    		return false;
    	}
	}
	
	if ($_SESSION['PO']->delivery_address == "")
	{
		display_error(_("There is no delivery address specified."));
		set_focus('delivery_address');
		return false;
	} 
	
	if (!isset($_SESSION['PO']->Location) || $_SESSION['PO']->Location == "")
	{
		display_error(_("There is no location specified to move any items into."));
		set_focus('StkLocation');
		return false;
	} 
	
	if ($_SESSION['PO']->order_has_items() == false)
	{
     	display_error (_("The order cannot be placed because there are no lines entered on this order."));
     	return false;
	}
		
	return true;
}

//---------------------------------------------------------------------------------------------------

function handle_commit_order()
{
	copy_to_po();

	if (can_commit())
	{

		if ($_SESSION['PO']->order_no == 0)
		{ 
			
			/*its a new order to be inserted */
			$order_no = add_po($_SESSION['PO']);
			 
			unset($_SESSION['PO']);
			 
        	meta_forward($_SERVER['PHP_SELF'], "AddedID=$order_no");	

		} 
		else 
		{ 

			/*its an existing order need to update the old order info */
			$order_no = update_po($_SESSION['PO']);
			
			unset($_SESSION['PO']);
			
        	meta_forward($_SERVER['PHP_SELF'], "AddedID=$order_no&Updated=1");	
		}
	}	
}
//---------------------------------------------------------------------------------------------------
$id = find_submit('Delete');
if ($id != -1)
	handle_delete_item($id);

if (isset($_POST['Delete']) || isset($_POST['Edit']))
{
	copy_from_po();
}
	
if (isset($_POST['Commit']))
{
	handle_commit_order();
}
if (isset($_POST['UpdateLine']))
{
	copy_to_po();
	handle_update_item();
}
if (isset($_POST['EnterLine']))
{
	copy_to_po();
	handle_add_new_item();
} 
if (isset($_POST['CancelOrder'])) 
{
	handle_cancel_po();
}
if (isset($_POST['CancelUpdate']))
{
	copy_to_po();
	unset_form_variables();
}
if (isset($_GET['ModifyOrderNumber']) && $_GET['ModifyOrderNumber'] != "")
{
	create_new_po();
	
	$_SESSION['PO']->order_no = $_GET['ModifyOrderNumber'];	

	/*read in all the selected order into the Items cart  */
	read_po($_SESSION['PO']->order_no, $_SESSION['PO']);
	copy_from_po();
}
if (isset($_POST['CancelUpdate']) || isset($_POST['UpdateLine'])) {
	line_start_focus();
}

//--------------------------------------------------------------------------------

if (isset($_GET['NewOrder']))
{
	create_new_po();
} 
else 
{
	if (!isset($_POST['supplier_id']))
		$_POST['supplier_id'] = $_SESSION['PO']->supplier_id;
	if (!isset($_POST['OrderDate']))		
		$_POST['OrderDate'] = $_SESSION['PO']->orig_order_date;
	if (!isset($_POST['Requisition']))		
		$_POST['Requisition'] = $_SESSION['PO']->requisition_no;
	if (!isset($_POST['Comments']))		
		$_POST['Comments'] = $_SESSION['PO']->Comments;
}

//---------------------------------------------------------------------------------------------------

start_form(false, true);

display_po_header($_SESSION['PO']);
echo "<br>";

display_po_items($_SESSION['PO']);

start_table($table_style2);
textarea_row(_("Memo:"), 'Comments', null, 70, 4);

end_table(1);

div_start('controls', 'items_table');
if ($_SESSION['PO']->order_has_items()) 
{
	if ($_SESSION['PO']->order_no)
		submit_center_first('Commit', _("Update Order"), '', true);
	else
		submit_center_first('Commit', _("Place Order"), '', true);
	submit_center_last('CancelOrder', _("Cancel Order")); 	
}
else
	submit_center('CancelOrder', _("Cancel Order")); 	
div_end();
//---------------------------------------------------------------------------------------------------

end_form();
end_page();
?>
