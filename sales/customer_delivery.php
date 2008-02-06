<?php

$page_security = 2;
$path_to_root="..";
include_once($path_to_root . "/sales/includes/cart_class.inc");
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/includes/manufacturing.inc");
include_once($path_to_root . "/sales/includes/sales_db.inc");
include_once($path_to_root . "/sales/includes/sales_ui.inc");

include_once($path_to_root . "/taxes/tax_calc.inc");

$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();

if(isset($_GET['ModifyDelivery'])) {
	$_SESSION['page_title'] = _("Modifying Delivery Note") . " #".$_GET['ModifyDelivery']; 
}
else { 
	$_SESSION['page_title'] = _("Deliver Items for a Sales Order");
}

page($_SESSION['page_title'], false, false, "", $js); 

if (isset($_GET['AddedID'])) 
{
	$dispatch_no = $_GET['AddedID'];
	$trans_type = 13;

	display_notification(_("Dispatch processed:") . ' '.$_GET['AddedID'], true);

	display_note(get_customer_trans_view_str($trans_type, $dispatch_no, _("View this dispatch")), 0, 1);

 	display_note(get_gl_view_str($trans_type, $dispatch_no, _("View the GL Journal Entries for this Dispatch")));

	hyperlink_params("$path_to_root/sales/customer_invoice.php", _("Invoice This Delivery"), "DeliveryNumber=$dispatch_no");

	if ($_SESSION['Items']->trans_type=='invoice')
		hyperlink_params("$path_to_root/sales/sales_order_entry.php", _("Make Another Dispatch"), "NewDispatch=Yes");
	else
		hyperlink_params("$path_to_root/sales/inquiry/sales_orders_view.php", _("Select Another Order For Dispatch"), "OutstandingOnly=1");

	display_footer_exit();
}

//---------------------------------------------------------------------------------------------------------------

if (!isset($_GET['OrderNumber']) && !isset($_SESSION['ProcessingOrder']) && 
 !isset($_GET['ModifyDelivery']) && !isset($_GET['process_delivery'])) 
{
	/* This page can only be called with an order number for invoicing*/
	display_error(_("This page can only be opened if an order or delivery note has been selected. Please select it first."));

	hyperlink_params("$path_to_root/sales/inquiry/sales_orders_view.php", _("Select a Sales Order to Delivery"), "OutstandingOnly=1");

	end_page();
	exit;

} 
elseif (isset($_GET['OrderNumber']) && $_GET['OrderNumber'] > 0) 
{

	if (isset($_SESSION['Items']))
	{
		unset($_SESSION['Items']->line_items);
		unset ($_SESSION['Items']);
	}

	$_SESSION['ProcessingOrder'] = $_GET['OrderNumber'];
	$_SESSION['Items'] = new cart;
	
	/*read in all the selected order into the Items cart  */

	if (read_sales_order($_SESSION['ProcessingOrder'], $_SESSION['Items'])) 
	{
        	if ($_SESSION['Items']->count_items() == 0) 
    		{
    		hyperlink_params($path_to_root . "/sales/inquiry/sales_orders_view.php", _("Select a different sales order to delivery"), "OutstandingOnly=1");
    		die ("<br><b>" . _("This order has no items. There is nothing to delivery.") . "</b>");
    		}
	} 
	else 
	{
		hyperlink_no_params("/sales_orders_view.php", _("Select a sales order to dispatch"));
		die ("<br><b>" . _("This order items could not be retrieved. Please select another order.") . "</b>");
	}

} 
elseif (isset($_GET['ModifyDelivery']) && $_GET['ModifyDelivery'] > 0) {
	if (isset($_SESSION['Items']))
	{
		unset($_SESSION['Items']->line_items);
		unset ($_SESSION['Items']);
	}
	$_SESSION['Items'] = new cart;
	
	if(read_sales_delivery($_GET['ModifyDelivery'],$_SESSION['Items'] ))
	{
    	    if ($_SESSION['Items']->count_items() == 0) 
    	    {
    		hyperlink_params($path_to_root . "/sales/inquiry/sales_orders_view.php", _("Select a different delivery"), "OutstandingOnly=1");
    		die ("<br><b>" . _("This delivery has all items invoiced. There is nothing to modify.") . "</b>");
    	    }
	} 
	else 
	{
		hyperlink_no_params("/sales_orders_view.php", _("Select a sales order to dispatch"));
		die ("<br><b>" . _("This sales delivery item could not be retrieved. Please select another delivery.") . "</b>");
	}

	$_SESSION['ProcessingOrder'] = $_SESSION['Items']->order_no;
} else 
{
 // Update cart delivery quantities/descriptions
	foreach ($_SESSION['Items']->line_items as $line=>$itm) 
	{
	  if(isset($_POST['Line'.$line])) {
		$line_qty = $_POST['Line'.$line];
		if (is_numeric($line_qty) && $_POST['Line'.$line] <= ($itm->quantity - $itm->qty_done))
		{
			$_SESSION['Items']->line_items[$line]->qty_dispatched = $line_qty;
		}
	  }
	
	  if(isset($_POST['Line'.$line.'Desc'])) {
		$line_desc = $_POST['Line'.$line.'Desc'];
		if (strlen($line_desc) > 0) 
		{
			$_SESSION['Items']->line_items[$line]->item_description = $line_desc;
		}
	  }
	}
}

//---------------------------------------------------------------------------------------------------------------

function order_changed_error()
{
	global $path_to_root;
	display_note(_("This order has been changed or invoiced since this delivery was started to be confirmed. Processing halted."), 1, 0);
	display_note(_("To enter and confirm this dispatch the order must be re-selected and re-read again to update the changes made by the other user."), 1, 0);

	hyperlink_no_params("$path_to_root/sales/inquiry/sales_orders_view.php", _("Select a sales order for confirming deliveries"));

	unset($_SESSION['ProcessingOrder']);
	exit;
}

//---------------------------------------------------------------------------------------------------------------

function check_order_changed()
{
	global $debug;

	/*Now need to check that the order details are the same as they were when
			they were read into the Items array.
	If they've changed then someone else may have dispatch them */

	$sql = "SELECT id, stk_code, quantity, qty_sent FROM ".TB_PREF."sales_order_details WHERE
		order_no = " . $_SESSION['ProcessingOrder']. " ORDER BY id";

	$result = db_query($sql,"retreive sales order details");

	if (db_num_rows($result) != count($_SESSION['Items']->line_items))
	{

		/*there should be the same number of items returned from this query as 
		   count of lines on the delivery notes - if  not	then someone has 
		   already invoiced or credited some lines */
    	if ($debug == 1)
    	{
    		display_note($sql, 1, 0);
    		display_note("No rows returned by sql:" . db_num_rows($result), 1, 0);
    		display_note("Count of items in the cart " . count($_SESSION['Items']->line_items), 1, 0);
    	}

		return false;
	}
	$line=0;
	while ($myrow = db_fetch($result)) 
	{
		$stk_itm = $myrow["stk_code"];
		if ($_SESSION['Items']->line_items[$line]->quantity != $myrow["quantity"] ||
			$_SESSION['Items']->line_items[$line]->qty_done != $myrow["qty_sent"])
		{
			display_note(_("Original order for") . " '" . $myrow["stk_code"] . "' " .
				_("has a quantity of") . " " . $myrow["quantity"] . " " . 
				_("and an delivered quantity of") . " " . $myrow["qty_sent"] . " " .
				_("the cart shows quantity of") . " " . 
				$_SESSION['Items']->line_items[$line]->quantity . " " . 
				_("and delivered quantity of") . " " . 
				$_SESSION['Items']->line_items[$line]->qty_done, 1, 0);

			return false;
		}
	$line++;
	} /*loop through all line items of the order to ensure none have been invoiced */
	return true;
}

//---------------------------------------------------------------------------------------------------------------

function check_data()
{
	if (!isset($_POST['DispatchDate']) || !is_date($_POST['DispatchDate']))	
	{
		display_error(_("The entered date of delivery is invalid."));
		return false;
	}
	if (!is_date_in_fiscalyear($_POST['DispatchDate'])) 
	{
		display_error(_("The entered date of delivery is not in fiscal year."));
		return false;
	}
	if (!isset($_POST['due_date']) || !is_date($_POST['due_date']))	
	{
		display_error(_("The entered dead-line for invoice is invalid."));
		return false;
	}

	if (!references::is_valid($_POST['ref'])) 
	{
		display_error(_("You must enter a reference."));
		return false;
	}

	if (!is_new_reference($_POST['ref'], 13)) 
	{
		display_error(_("The entered reference is already in use."));
		return false;
	}
	if ($_POST['ChargeFreightCost'] == "")
		$_POST['ChargeFreightCost'] = 0;
	if (!is_numeric($_POST['ChargeFreightCost']) || $_POST['ChargeFreightCost'] < 0)	
	{
		display_error(_("The entered shipping value is not numeric."));
		return false;
	}

	if ($_SESSION['Items']->has_items_dispatch() == 0 && $_POST['ChargeFreightCost'] == 0)	
	{
		display_error(_("There are no item quantities on this delivery note."));
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------------------------------------------

function check_qoh()
{
	if (!sys_prefs::allow_negative_stock())
	{
    	foreach ($_SESSION['Items']->line_items as $itm) 
    	{

			if ($itm->qty_dispatched && has_stock_holding($itm->mb_flag))
			{
				$qoh = get_qoh_on_date($itm->stock_id, $_POST['Location'], $_POST['DispatchDate']);

        		if ($itm->qty_dispatched > $qoh) 
        		{
        			display_error(_("The delivery cannot be processed because there is an insufficient quantity for item:") .
        				" " . $itm->stock_id . " - " .  $itm->item_description);
        			return false;
        		}
    		}
    	}
	}

	return true;
}

//---------------------------------------------------------------------------------------------------------------

function process_delivery($get_from_order=false)
{
	if ($get_from_order)
	{   // entry point for direct delivery - cart contains completed order;
	    // we should have qty_dispatched and standard cost set anyway
		unset($_SESSION['Items']->line_items);
	    read_sales_order($_SESSION['Items']->order_no, $_SESSION['Items']);

		$duedate = get_invoice_duedate($_SESSION['Items']->customer_id, $_SESSION['Items']->delivery_date);
		$delivery_no = add_sales_delivery($_SESSION['Items'],
			$_SESSION['Items']->delivery_date, $duedate, $_SESSION['Items']->order_no,
			$_SESSION['Items']->tax_group_id, $_SESSION['Items']->freight_cost,
			$_SESSION['Items']->Location, $_SESSION['Items']->ship_via,
			$_SESSION['Items']->default_sales_type,	references::get_next(13),
 			$_SESSION['Items']->memo_,0);
	} 
	else 
	{
	
		if (!check_data())
			return;

		if (!check_order_changed())
			order_changed_error();

		if (!check_qoh())
			return;

		if ($_POST['bo_policy'])
			$bo_policy = 0;
		else
			$bo_policy = 1;
			
		$delivery_no = add_sales_delivery($_SESSION['Items'],
			$_POST['DispatchDate'], $_POST['due_date'],	$_SESSION['ProcessingOrder'],
			$_SESSION['Items']->tax_group_id,$_POST['ChargeFreightCost'], $_POST['Location'],
			$_POST['ship_via'],	$_POST['sales_type_id'], $_POST['ref'],
			$_POST['InvoiceText'], $bo_policy);
		unset($_SESSION['ProcessingOrder']);
	}
   	meta_forward($_SERVER['PHP_SELF'], "AddedID=$delivery_no");
}

//---------------------------------------------------------------------------------------------------------------
if (isset($_GET['process_delivery']))
	process_delivery(true);
elseif (isset($_POST['process_delivery']))
	process_delivery();

//-------------------------------------------------------------------------------------------------

start_form(false, true);

start_table("$table_style2 width=80%", 5);
echo "<tr><td>"; // outer table

start_table("$table_style width=100%");
start_row();
label_cells(_("Customer"), $_SESSION['Items']->customer_name, "class='tableheader2'");
label_cells(_("Branch"), get_branch_name($_SESSION['Items']->Branch), "class='tableheader2'");
label_cells(_("Currency"), $_SESSION['Items']->customer_currency, "class='tableheader2'");
end_row();
start_row();

if (!isset($_POST['ref']))
	$_POST['ref'] = references::get_next(13);

ref_cells(_("Reference"), 'ref', null, "class='tableheader2'");

label_cells(_("For Sales Order"), get_customer_trans_view_str(systypes::sales_order(), $_SESSION['ProcessingOrder']), "class='tableheader2'");

if (!isset($_POST['sales_type_id']))
	$_POST['sales_type_id'] = $_SESSION['Items']->default_sales_type;
label_cell(_("Sales Type"), "class='tableheader2'");	
sales_types_list_cells(null, 'sales_type_id', $_POST['sales_type_id']);

end_row();
start_row();

if (!isset($_POST['Location']))
	$_POST['Location'] = $_SESSION['Items']->Location;
label_cell(_("Delivery From"), "class='tableheader2'");	
locations_list_cells(null, 'Location', $_POST['Location'], false, true);

if (!isset($_POST['ship_via']))
	$_POST['ship_via'] = $_SESSION['Items']->ship_via;
label_cell(_("Shipping Company"), "class='tableheader2'");	
shippers_list_cells(null, 'ship_via', $_POST['ship_via']);

// set this up here cuz it's used to calc qoh
if (!isset($_POST['DispatchDate']) || !is_date($_POST['DispatchDate']))
{
	$_POST['DispatchDate'] = Today();
	if (!is_date_in_fiscalyear($_POST['DispatchDate']))
		$_POST['DispatchDate'] = end_fiscalyear();
}
date_cells(_("Date"), 'DispatchDate', $_POST['DispatchDate'], 0, 0, 0, "class='tableheader'");
end_row();

end_table();

echo "</td><td>";// outer table

start_table("$table_style width=90%");

if (!isset($_POST['due_date']) || !is_date($_POST['due_date']))

	$_POST['due_date'] = get_invoice_duedate($_SESSION['Items']->customer_id, $_POST['DispatchDate']);

date_row(_("Invoice Dead-line"), 'due_date', $_POST['due_date'], 0, 0, 0, "class='tableheader'");
end_table();

echo "</td></tr>";
end_table(1); // outer table

display_heading(_("Invoice Items"));

start_table("$table_style width=80%");
$th = array(_("Item Code"), _("Item Description"), _("Ordered"), _("Units"), _("Delivered"),
	_("This Delivery"), _("Price"), _("Tax Type"), _("Discount"), _("Total"));
table_header($th);
$k = 0;
$has_marked = false;
$show_qoh = true;

foreach ($_SESSION['Items']->line_items as $line=>$ln_itm) 
{	
	if($ln_itm->quantity==$ln_itm->qty_done) continue; //this line is fully delivered
    // if it's a non-stock item (eg. service) don't show qoh
    if (sys_prefs::allow_negative_stock() || !has_stock_holding($ln_itm->mb_flag) ||
		$ln_itm->qty_dispatched == 0)
    	$show_qoh = false;

	if ($show_qoh)
		$qoh = get_qoh_on_date($ln_itm->stock_id, $_POST['Location'], $_POST['DispatchDate']);

	if ($show_qoh && ($ln_itm->qty_dispatched > $qoh)) 
	{
		// oops, we don't have enough of one of the component items
		start_row("class='stockmankobg'");
		$has_marked = true;
	} 
	else
		alt_table_row_color($k);

	view_stock_status_cell($ln_itm->stock_id);

	text_cells(null, 'Line'.$line.'Desc', $ln_itm->item_description, 30, 50);
	qty_cell($ln_itm->quantity);
	label_cell($ln_itm->units);
	qty_cell($ln_itm->qty_done);

	text_cells(null, 'Line'.$line, $ln_itm->qty_dispatched, 10, 10);

	$display_discount_percent = number_format2($ln_itm->discount_percent*100,user_percent_dec()) . "%";

	$line_total = ($ln_itm->qty_dispatched * $ln_itm->price * (1 - $ln_itm->discount_percent));

	amount_cell($ln_itm->price);
	label_cell($ln_itm->tax_type_name);
	label_cell($display_discount_percent, "nowrap align=right");
	amount_cell($line_total);

	end_row();
}
  
$_POST['ChargeFreightCost'] = $_SESSION['Items']->freight_cost;

if (!is_numeric($_POST['ChargeFreightCost']))
{
    	$_POST['ChargeFreightCost'] = 0;
}

start_row();

small_amount_cells(_("Shipping Cost"), 'ChargeFreightCost', null, "colspan=9 align=right");

$inv_items_total = $_SESSION['Items']->get_items_total_dispatch();

$display_sub_total = number_format2($inv_items_total + $_POST['ChargeFreightCost'],user_price_dec());

label_row(_("Sub-total"), $display_sub_total, "colspan=9 align=right","align=right");

$taxes = $_SESSION['Items']->get_taxes($_SESSION['Items']->tax_group_id, $_POST['ChargeFreightCost']);
$tax_total = display_edit_tax_items($taxes, 9);

$display_total = number_format2(($inv_items_total + $_POST['ChargeFreightCost'] + $tax_total), user_price_dec());

label_row(_("Amount Total"), $display_total, "colspan=9 align=right","align=right");

end_table(1);

if ($has_marked)
	display_note(_("Marked items have insufficient quantities in stock."), 0, 1, "class='red'");

start_table($table_style2);

policy_list_row(_("Action For Balance"), "bo_policy", null);

textarea_row(_("Memo"), 'InvoiceText', null, 50, 4);

end_table(1);

submit_center_first('Update', _("Update"));
submit_center_last('process_delivery', _("Process Dispatch"));

end_form();

//---------------------------------------------------------------------------------------------

end_page();

?>
