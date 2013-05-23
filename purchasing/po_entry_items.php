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
$path_to_root = "..";
$page_security = 'SA_PURCHASEORDER';
include_once($path_to_root . "/purchasing/includes/po_class.inc");
include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/purchasing/includes/purchasing_ui.inc");
include_once($path_to_root . "/purchasing/includes/db/suppliers_db.inc");
include_once($path_to_root . "/reporting/includes/reporting.inc");

set_page_security( @$_SESSION['PO']->trans_type,
	array(	ST_PURCHORDER => 'SA_PURCHASEORDER',
			ST_SUPPRECEIVE => 'SA_GRN',
			ST_SUPPINVOICE => 'SA_SUPPLIERINVOICE'),
	array(	'NewOrder' => 'SA_PURCHASEORDER',
			'ModifyOrderNumber' => 'SA_PURCHASEORDER',
			'AddedID' => 'SA_PURCHASEORDER',
			'NewGRN' => 'SA_GRN',
			'AddedGRN' => 'SA_GRN',
			'NewInvoice' => 'SA_SUPPLIERINVOICE',
			'AddedPI' => 'SA_SUPPLIERINVOICE')
);

$js = '';
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();

if (isset($_GET['ModifyOrderNumber']) && is_numeric($_GET['ModifyOrderNumber'])) {

	$_SESSION['page_title'] = _($help_context = "Modify Purchase Order #") . $_GET['ModifyOrderNumber'];
	create_new_po(ST_PURCHORDER, $_GET['ModifyOrderNumber']);
	copy_from_cart();
} elseif (isset($_GET['NewOrder'])) {

	$_SESSION['page_title'] = _($help_context = "Purchase Order Entry");
	create_new_po(ST_PURCHORDER, 0);
	copy_from_cart();
} elseif (isset($_GET['NewGRN'])) {

	$_SESSION['page_title'] = _($help_context = "Direct GRN Entry");
	create_new_po(ST_SUPPRECEIVE, 0);
	copy_from_cart();
} elseif (isset($_GET['NewInvoice'])) {

	$_SESSION['page_title'] = _($help_context = "Direct Purchase Invoice Entry");
	create_new_po(ST_SUPPINVOICE, 0);
	copy_from_cart();
}

page($_SESSION['page_title'], false, false, "", $js);

//---------------------------------------------------------------------------------------------------

check_db_has_suppliers(_("There are no suppliers defined in the system."));

check_db_has_purchasable_items(_("There are no purchasable inventory items defined in the system."));

//---------------------------------------------------------------------------------------------------------------

if (isset($_GET['AddedID'])) 
{
	$order_no = $_GET['AddedID'];
	$trans_type = ST_PURCHORDER;	

	if (!isset($_GET['Updated']))
		display_notification_centered(_("Purchase Order has been entered"));
	else
		display_notification_centered(_("Purchase Order has been updated") . " #$order_no");
	display_note(get_trans_view_str($trans_type, $order_no, _("&View this order")), 0, 1);

	display_note(print_document_link($order_no, _("&Print This Order"), true, $trans_type), 0, 1);

	display_note(print_document_link($order_no, _("&Email This Order"), true, $trans_type, false, "printlink", "", 1));

	hyperlink_params($path_to_root . "/purchasing/po_receive_items.php", _("&Receive Items on this Purchase Order"), "PONumber=$order_no");

	hyperlink_params($_SERVER['PHP_SELF'], _("Enter &Another Purchase Order"), "NewOrder=yes");
	
	hyperlink_no_params($path_to_root."/purchasing/inquiry/po_search.php", _("Select An &Outstanding Purchase Order"));
	
	display_footer_exit();	

} elseif (isset($_GET['AddedGRN'])) {

	$trans_no = $_GET['AddedGRN'];
	$trans_type = ST_SUPPRECEIVE;

	display_notification_centered(_("Direct GRN has been entered"));

	display_note(get_trans_view_str($trans_type, $trans_no, _("&View this GRN")), 0);

    $clearing_act = get_company_pref('grn_clearing_act');
	if ($clearing_act)	
		display_note(get_gl_view_str($trans_type, $trans_no, _("View the GL Journal Entries for this Delivery")), 1);
// not yet
//	display_note(print_document_link($trans_no, _("&Print This GRN"), true, $trans_type), 0, 1);

	hyperlink_params("$path_to_root/purchasing/supplier_invoice.php",
		_("Entry purchase &invoice for this receival"), "New=1");

	hyperlink_params("$path_to_root/admin/attachments.php", _("Add an Attachment"), 
		"filterType=$trans_type&trans_no=$trans_no");

	hyperlink_params($_SERVER['PHP_SELF'], _("Enter &Another GRN"), "NewGRN=Yes");
	
	display_footer_exit();	

} elseif (isset($_GET['AddedPI'])) {

	$trans_no = $_GET['AddedPI'];
	$trans_type = ST_SUPPINVOICE;

	display_notification_centered(_("Direct Purchase Invoice has been entered"));

	display_note(get_trans_view_str($trans_type, $trans_no, _("&View this Invoice")), 0);

// not yet
//	display_note(print_document_link($trans_no, _("&Print This Invoice"), true, $trans_type), 0, 1);

	display_note(get_gl_view_str($trans_type, $trans_no, _("View the GL Journal Entries for this Invoice")), 1);

	hyperlink_params("$path_to_root/purchasing/supplier_payment.php", _("Entry supplier &payment for this invoice"),
		"PInvoice=".$trans_no);

	hyperlink_params("$path_to_root/admin/attachments.php", _("Add an Attachment"), 
		"filterType=$trans_type&trans_no=$trans_no");

	hyperlink_params($_SERVER['PHP_SELF'], _("Enter &Another Direct Invoice"), "NewInvoice=Yes");
	
	display_footer_exit();	
}
//--------------------------------------------------------------------------------------------------

function line_start_focus() {
  global 	$Ajax;

  $Ajax->activate('items_table');
  set_focus('_stock_id_edit');
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
	} else {
		unset($_SESSION['PO']);
		meta_forward($path_to_root.'/index.php','application=AP');
	}

	$_SESSION['PO']->clear_items();
	$_SESSION['PO'] = new purch_order;

	display_notification(_("This purchase order has been cancelled."));

	hyperlink_params($path_to_root . "/purchasing/po_entry_items.php", _("Enter a new purchase order"), "NewOrder=Yes");
	echo "<br>";

	end_page();
	exit;
}

//---------------------------------------------------------------------------------------------------

function check_data()
{
	if(!get_post('stock_id_text', true)) {
		display_error( _("Item description cannot be empty."));
		set_focus('stock_id_edit');
		return false;
	}

	$dec = get_qty_dec($_POST['stock_id']);
	$min = 1 / pow(10, $dec);
    if (!check_num('qty',$min))
    {
    	$min = number_format2($min, $dec);
	   	display_error(_("The quantity of the order item must be numeric and not less than ").$min);
		set_focus('qty');
	   	return false;
    }

    if (!check_num('price', 0))
    {
	   	display_error(_("The price entered must be numeric and not less than zero."));
		set_focus('price');
	   	return false;	   
    }
    if ($_SESSION['PO']->trans_type == ST_PURCHORDER && !is_date($_POST['req_del_date'])){
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

	if ($allow_update)
	{
		if ($_SESSION['PO']->line_items[$_POST['line_no']]->qty_inv > input_num('qty') ||
			$_SESSION['PO']->line_items[$_POST['line_no']]->qty_received > input_num('qty'))
		{
			display_error(_("You are attempting to make the quantity ordered a quantity less than has already been invoiced or received.  This is prohibited.") .
				"<br>" . _("The quantity received can only be modified by entering a negative receipt and the quantity invoiced can only be reduced by entering a credit note against this item."));
			set_focus('qty');
			return;
		}
	
		$_SESSION['PO']->update_order_item($_POST['line_no'], input_num('qty'), input_num('price'),
  			@$_POST['req_del_date'], $_POST['item_description'] );
		unset_form_variables();
	}	
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
   			    if (($order_item->stock_id == $_POST['stock_id'])) 
   			    {
					display_warning(_("The selected item is already on this order."));
			    }
		    } /* end of the foreach loop to look for pre-existing items of the same code */
		}

		if ($allow_update == true)
		{
			$result = get_short_info($_POST['stock_id']);

			if (db_num_rows($result) == 0)
			{
				$allow_update = false;
			}

			if ($allow_update)
			{
				$myrow = db_fetch($result);
				$_SESSION['PO']->add_to_order (count($_SESSION['PO']->line_items), $_POST['stock_id'], input_num('qty'), 
					get_post('stock_id_text'), //$myrow["description"], 
					input_num('price'), '', // $myrow["units"], (retrived in cart)
					$_SESSION['PO']->trans_type == ST_PURCHORDER ? $_POST['req_del_date'] : '', 0, 0);

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
	global $Refs;

	if (!get_post('supplier_id')) 
	{
		display_error(_("There is no supplier selected."));
		set_focus('supplier_id');
		return false;
	} 
	
	if (!is_date($_POST['OrderDate'])) 
	{
		display_error(_("The entered order date is invalid."));
		set_focus('OrderDate');
		return false;
	} 
	
	if ($_SESSION['PO']->trans_type != ST_PURCHORDER && !is_date_in_fiscalyear($_POST['OrderDate'])) 
	{
		display_error(_("The entered date is not in fiscal year"));
		set_focus('OrderDate');
		return false;
	}

	if (($_SESSION['PO']->trans_type==ST_SUPPINVOICE) && !is_date($_POST['due_date'])) 
	{
		display_error(_("The entered due date is invalid."));
		set_focus('due_date');
		return false;
	} 
	
	if (!$_SESSION['PO']->order_no) 
	{
    	if (!$Refs->is_valid(get_post('ref'))) 
    	{
    		display_error(_("There is no reference entered for this purchase order."));
			set_focus('ref');
    		return false;
    	} 
    	
    	if (!is_new_reference(get_post('ref'), $_SESSION['PO']->trans_type)) 
    	{
    		display_error(_("The entered reference is already in use."));
			set_focus('ref');
    		return false;
    	}
	}
	
	if ($_SESSION['PO']->trans_type == ST_SUPPINVOICE && !$Refs->is_valid(get_post('supp_ref'))) 
	{
		display_error(_("You must enter a supplier's invoice reference."));
		set_focus('supp_ref');
		return false;
	}
	if ($_SESSION['PO']->trans_type==ST_SUPPINVOICE 
		&& is_reference_already_there($_SESSION['PO']->supplier_id, get_post('supp_ref'), $_SESSION['PO']->order_no))
	{
		display_error(_("This invoice number has already been entered. It cannot be entered again.") . " (" . get_post('supp_ref') . ")");
		set_focus('supp_ref');
		return false;
	}
	if ($_SESSION['PO']->trans_type == ST_PURCHORDER && get_post('delivery_address') == '')
	{
		display_error(_("There is no delivery address specified."));
		set_focus('delivery_address');
		return false;
	} 
	if (get_post('StkLocation') == '')
	{
		display_error(_("There is no location specified to move any items into."));
		set_focus('StkLocation');
		return false;
	} 
	if (!db_has_currency_rates($_SESSION['PO']->curr_code, $_POST['OrderDate']))
		return false;
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
	$cart = &$_SESSION['PO'];

	if (can_commit()) {

		copy_to_cart();
		if ($cart->trans_type != ST_PURCHORDER) {
			// for direct grn/invoice set same dates for lines as for whole document
			foreach ($cart->line_items as $line_no =>$line)
				$cart->line_items[$line_no]->req_del_date = $cart->orig_order_date;
		}
		if ($cart->order_no == 0) { // new po/grn/invoice
			/*its a new order to be inserted */
			$ref = $cart->reference;
			if ($cart->trans_type != ST_PURCHORDER) {
				$cart->reference = 'auto';
				begin_transaction();	// all db changes as single transaction for direct document
			}
			$order_no = add_po($cart);
			new_doc_date($cart->orig_order_date); 
        	$cart->order_no = $order_no;

			if ($cart->trans_type == ST_PURCHORDER) {
				unset($_SESSION['PO']);
        		meta_forward($_SERVER['PHP_SELF'], "AddedID=$order_no");
        	}
			//Direct GRN
			if ($cart->trans_type == ST_SUPPRECEIVE)
				$cart->reference = $ref;
			if ($cart->trans_type != ST_SUPPINVOICE)	
				$cart->Comments = $cart->reference; //grn does not hold supp_ref
			foreach($cart->line_items as $key => $line)
				$cart->line_items[$key]->receive_qty = $line->quantity;
			$grn_no = add_grn($cart);
			if ($cart->trans_type == ST_SUPPRECEIVE) {
				commit_transaction(); // save PO+GRN
				unset($_SESSION['PO']);
        		meta_forward($_SERVER['PHP_SELF'], "AddedGRN=$grn_no");
			}
//			Direct Purchase Invoice
 			$inv = new supp_trans(ST_SUPPINVOICE);
			$inv->Comments = $cart->Comments;
			$inv->supplier_id = $cart->supplier_id;
			$inv->tran_date = $cart->orig_order_date;
			$inv->due_date = $cart->due_date;
			$inv->reference = $ref;
			$inv->supp_reference = $cart->supp_ref;
			$inv->tax_included = $cart->tax_included;
			$supp = get_supplier($cart->supplier_id);
			$inv->tax_group_id = $supp['tax_group_id'];

			$inv->ov_amount = $inv->ov_gst = $inv->ov_discount = 0;

			$total = 0;
			foreach($cart->line_items as $key => $line) {
				$inv->add_grn_to_trans($line->grn_item_id, $line->po_detail_rec, $line->stock_id,
					$line->item_description, $line->receive_qty, 0, $line->receive_qty,
					$line->price, $line->price, true, get_standard_cost($line->stock_id), '');
				$inv->ov_amount += round2(($line->receive_qty * $line->price), user_price_dec());
			}
			$inv->tax_overrides = $cart->tax_overrides;
			if (!$inv->tax_included) {
				$taxes = $inv->get_taxes($inv->tax_group_id, 0, false);
				foreach( $taxes as $taxitem) {
					$total += isset($taxitem['Override']) ? $taxitem['Override'] : $taxitem['Value'];
				}
			}
			$inv->ex_rate = $cart->ex_rate;

			$inv_no = add_supp_invoice($inv);
			commit_transaction(); // save PO+GRN+PI
			// FIXME payment for cash terms. (Needs cash account selection)
			unset($_SESSION['PO']);
       		meta_forward($_SERVER['PHP_SELF'], "AddedPI=$inv_no");
		}
		else { // order modification

			$order_no = update_po($cart);
			unset($_SESSION['PO']);
        	meta_forward($_SERVER['PHP_SELF'], "AddedID=$order_no&Updated=1");	
		}
	}
}
//---------------------------------------------------------------------------------------------------
$id = find_submit('Delete');
if ($id != -1)
	handle_delete_item($id);

if (isset($_POST['Commit']))
{
	handle_commit_order();
}
if (isset($_POST['UpdateLine']))
	handle_update_item();

if (isset($_POST['EnterLine']))
	handle_add_new_item();

if (isset($_POST['CancelOrder'])) 
	handle_cancel_po();

if (isset($_POST['CancelUpdate']))
	unset_form_variables();

if (isset($_POST['CancelUpdate']) || isset($_POST['UpdateLine'])) {
	line_start_focus();
}

//---------------------------------------------------------------------------------------------------

start_form();

display_po_header($_SESSION['PO']);
echo "<br>";

display_po_items($_SESSION['PO']);

start_table(TABLESTYLE2);
textarea_row(_("Memo:"), 'Comments', null, 70, 4);

end_table(1);

div_start('controls', 'items_table');
$process_txt = _("Place Order");
$update_txt = _("Update Order");
$cancel_txt = _("Cancel Order");
if ($_SESSION['PO']->trans_type == ST_SUPPRECEIVE) {
	$process_txt = _("Process GRN");
	$update_txt = _("Update GRN");
	$cancel_txt = _("Cancel GRN");
}	
elseif ($_SESSION['PO']->trans_type == ST_SUPPINVOICE) {
	$process_txt = _("Process Invoice");
	$update_txt = _("Update Invoice");
	$cancel_txt = _("Cancel Invoice");
}	
if ($_SESSION['PO']->order_has_items()) 
{
	if ($_SESSION['PO']->order_no)
		submit_center_first('Commit', $update_txt, '', 'default');
	else
		submit_center_first('Commit', $process_txt, '', 'default');
	submit_center_last('CancelOrder', $cancel_txt); 	
}
else
	submit_center('CancelOrder', $cancel_txt, true, false, 'cancel');
div_end();
//---------------------------------------------------------------------------------------------------

end_form();
end_page();
?>
