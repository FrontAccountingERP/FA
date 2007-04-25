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
page(_("Issue an Invoice and Deliver Items for a Sales Order"), false, false, "", $js);

//---------------------------------------------------------------------------------------------------------------

if (isset($_GET['AddedID'])) 
{
	$invoice_no = $_GET['AddedID'];
	$trans_type = 10;

	display_notification(_("Invoice processed"), true);
	display_note(get_customer_trans_view_str($trans_type, $invoice_no, _("View this invoice")), 0, 1);

   	display_note(get_gl_view_str($trans_type, $invoice_no, _("View the GL Journal Entries for this Invoice")));

	if ($_SESSION['Items']->direct_invoice)
		hyperlink_params("$path_to_root/sales/sales_order_entry.php", _("Issue Another Invoice"), "NewInvoice=Yes");
	else
		hyperlink_params("$path_to_root/sales/inquiry/sales_orders_view.php", _("Select Another Order For Invoicing"), "OutstandingOnly=1");

	unset($_SESSION['Items']->line_items);
	unset($_SESSION['Items']);
	display_footer_exit();
}

//---------------------------------------------------------------------------------------------------------------

if (!isset($_GET['OrderNumber']) && !isset($_SESSION['ProcessingOrder']) && 
	!isset($_GET['process_invoice'])) 
{
	/* This page can only be called with an order number for invoicing*/
	display_error(_("This page can only be opened if an order has been selected. Please select an order first."));

	hyperlink_no_params("$path_to_root/sales/inquiry/sales_orders_view.php", _("Select a sales order to invoice"));

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

	session_register("Items");
	session_register("ProcessingOrder");

	$_SESSION['ProcessingOrder'] = $_GET['OrderNumber'];
	$_SESSION['Items'] = new cart;

	/*read in all the selected order into the Items cart  */

	if (read_sales_order($_SESSION['ProcessingOrder'], $_SESSION['Items'], true)) 
	{

    	if ($_SESSION['Items']->count_items() == 0) 
    	{
    		hyperlink_params($path_to_root . "/sales/inquiry/sales_orders_view.php", _("Select a different sales order to invoice"), "OutstandingOnly=1");
    		die ("<br><b>" . _("There are no ordered items with a quantity left to deliver. There is nothing left to invoice.") . "</b>");
    	}
	} 
	else 
	{
		hyperlink_no_params("/sales_orders_view.php", _("Select a sales order to invoice"));
		die ("<br><b>" . _("This order item could not be retrieved. Please select another order.") . "</b>");
	}

} 
else 
{
	/* if processing, a dispatch page has been called and ${$StkItm->stock_id} would have been set from the post */
	foreach ($_SESSION['Items']->line_items as $itm) 
	{

		if (isset($_SESSION['Items']->line_items[$itm->stock_id]) && 
			isset($_POST[$itm->stock_id]) && is_numeric($_POST[$itm->stock_id]) && 
			$_POST[$itm->stock_id] <= ($_SESSION['Items']->line_items[$itm->stock_id]->quantity - 
			$_SESSION['Items']->line_items[$itm->stock_id]->qty_inv))
		{
			$_SESSION['Items']->line_items[$itm->stock_id]->qty_dispatched = $_POST[$itm->stock_id];
		}

		if (isset($_POST[$itm->stock_id . "Desc"]) && strlen($_POST[$itm->stock_id . "Desc"]) > 0) 
		{
			$_SESSION['Items']->line_items[$itm->stock_id]->item_description = $_POST[$itm->stock_id . "Desc"];
		}
	}
}

//---------------------------------------------------------------------------------------------------------------

function order_changed_error()
{
	global $path_to_root;
	display_note(_("This order has been changed or invoiced since this delivery was started to be confirmed. Processing halted."), 1, 0);
	display_note(_("To enter and confirm this dispatch/invoice the order must be re-selected and re-read again to update the changes made by the other user."), 1, 0);

	hyperlink_no_params("$path_to_root/sales/inquiry/sales_orders_view.php", _("Select a sales order for confirming deliveries and invoicing"));

	unset($_SESSION['Items']->line_items);
	unset($_SESSION['Items']);
	unset($_SESSION['ProcessingOrder']);
	exit;
}

//---------------------------------------------------------------------------------------------------------------

function check_order_changed()
{
	global $debug;

	/*Now need to check that the order details are the same as they were when
			they were read into the Items array.
	If they've changed then someone else may have invoiced them  -
		as modified for bug pointed out by Sherif 1-7-03*/

	$sql = "SELECT stk_code, quantity, qty_invoiced FROM ".TB_PREF."sales_order_details WHERE
		quantity - qty_invoiced > 0
		AND order_no = " . $_SESSION['ProcessingOrder'];

	$result = db_query($sql,"retreive sales order details");

	if (db_num_rows($result) != count($_SESSION['Items']->line_items))
	{

		/*there should be the same number of items returned from this query as there are lines on the invoice -
			if  not	then someone has already invoiced or credited some lines */
    	if ($debug == 1)
    	{
    		display_note($sql, 1, 0);
    		display_note("No rows returned by sql:" . db_num_rows($result), 1, 0);
    		display_note("Count of items in the session " . count($_SESSION['Items']->line_items), 1, 0);
    	}

		return false;
	}

	while ($myrow = db_fetch($result)) 
	{
		$stk_itm = $myrow["stk_code"];
		if ($_SESSION['Items']->line_items[$stk_itm]->quantity != $myrow["quantity"] ||
			$_SESSION['Items']->line_items[$stk_itm]->qty_inv != $myrow["qty_invoiced"])
		{
			display_note(_("Original order for") . " " . $myrow["stk_code"] . " " .
				_("has a quantity of") . " " . $myrow["quantity"] . " " . 
				_("and an invoiced quantity of") . " " . $myrow["qty_invoiced"] . " " .
				_("the session shows quantity of") . " " . 
				$_SESSION['Items']->line_items[$stk_itm]->quantity . " " . 
				_("and quantity invoice of") . " " . 
				$_SESSION['Items']->line_items[$stk_itm]->qty_inv, 1, 0);

			return false;
		}
	} /*loop through all line items of the order to ensure none have been invoiced */

	return true;
}


//---------------------------------------------------------------------------------------------------------------

function check_data()
{
	if (!isset($_POST['DispatchDate']) || !is_date($_POST['DispatchDate']))	
	{
		display_error(_("The entered invoice date is invalid."));
		return false;
	}
	if (!is_date_in_fiscalyear($_POST['DispatchDate'])) 
	{
		display_error(_("The entered invoice date is not in fiscal year."));
		return false;
	}
	if (!isset($_POST['due_date']) || !is_date($_POST['due_date']))	
	{
		display_error(_("The entered invoice due date is invalid."));
		return false;
	}

	if (!references::is_valid($_POST['ref'])) 
	{
		display_error(_("You must enter a reference."));
		return false;
	}

	if (!is_new_reference($_POST['ref'], 10)) 
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
		display_error(_("There are no item quantities on this invoice."));
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
        			display_error(_("The invoice cannot be processed because there is an insufficient quantity for component:") .
        				" " . $itm->stock_id . " - " .  $itm->item_description);
        			return false;
        		}
    		}
    	}
	}

	return true;
}

//---------------------------------------------------------------------------------------------------------------

function process_invoice($invoicing=false)
{
	if ($invoicing) 
	{
		read_sales_order($_SESSION['Items']->order_no, $_SESSION['Items'], true);
		$duedate = get_invoice_duedate($_SESSION['Items']->customer_id, $_SESSION['Items']->delivery_date);
		$invoice_no = add_sales_invoice($_SESSION['Items'],
			$_SESSION['Items']->delivery_date, $duedate, $_SESSION['Items']->order_no,
			$_SESSION['Items']->tax_group_id, $_SESSION['Items']->freight_cost,
			$_SESSION['Items']->Location, $_SESSION['Items']->ship_via,
			$_SESSION['Items']->default_sales_type,	references::get_next(10),
 			$_SESSION['Items']->memo_, 0);
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

		$invoice_no = add_sales_invoice($_SESSION['Items'],
			$_POST['DispatchDate'], $_POST['due_date'],	$_SESSION['ProcessingOrder'],
			$_POST['tax_group_id'],	$_POST['ChargeFreightCost'], $_POST['Location'],
			$_POST['ship_via'],	$_POST['sales_type_id'], $_POST['ref'],
			$_POST['InvoiceText'], $bo_policy);
		unset($_SESSION['ProcessingOrder']);
	}

   	meta_forward($_SERVER['PHP_SELF'], "AddedID=$invoice_no");
}

//---------------------------------------------------------------------------------------------------------------
if (isset($_GET['process_invoice']))
	process_invoice(true);
elseif (isset($_POST['process_invoice']))
	process_invoice();

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
	$_POST['ref'] = references::get_next(10);

ref_cells(_("Reference"), 'ref', null, "class='tableheader2'");

if (!isset($_POST['tax_group_id']))
	$_POST['tax_group_id'] = $_SESSION['Items']->tax_group_id;
label_cell(_("Tax Group"), "class='tableheader2'");	
tax_groups_list_cells(null, 'tax_group_id', $_POST['tax_group_id'], false, null, true);

label_cells(_("For Sales Order"), get_customer_trans_view_str(systypes::sales_order(), $_SESSION['ProcessingOrder']), "class='tableheader2'");

end_row();
start_row();

if (!isset($_POST['sales_type_id']))
	$_POST['sales_type_id'] = $_SESSION['Items']->default_sales_type;
label_cell(_("Sales Type"), "class='tableheader2'");	
sales_types_list_cells(null, 'sales_type_id', $_POST['sales_type_id']);

if (!isset($_POST['Location']))
	$_POST['Location'] = $_SESSION['Items']->Location;
label_cell(_("Delivery From"), "class='tableheader2'");	
locations_list_cells(null, 'Location', $_POST['Location'], false, true);

if (!isset($_POST['ship_via']))
	$_POST['ship_via'] = $_SESSION['Items']->ship_via;
label_cell(_("Shipping Company"), "class='tableheader2'");	
shippers_list_cells(null, 'ship_via', $_POST['ship_via']);
end_row();

end_table();

echo "</td><td>";// outer table

start_table("$table_style width=90%");

// set this up here cuz it's used to calc qoh
if (!isset($_POST['DispatchDate']) || !is_date($_POST['DispatchDate']))
{
	$_POST['DispatchDate'] = Today();
	if (!is_date_in_fiscalyear($_POST['DispatchDate']))
		$_POST['DispatchDate'] = end_fiscalyear();
}
date_row(_("Date"), 'DispatchDate', $_POST['DispatchDate'], 0, 0, 0, "class='tableheader'");

if (!isset($_POST['due_date']) || !is_date($_POST['due_date']))
	//$_POST['due_date'] = $_POST['DispatchDate'];
	$_POST['due_date'] = get_invoice_duedate($_SESSION['Items']->customer_id, $_POST['DispatchDate']);

date_row(_("Due Date"), 'due_date', $_POST['due_date'], 0, 0, 0, "class='tableheader'");
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

foreach ($_SESSION['Items']->line_items as $ln_itm) 
{

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

	text_cells(null, $ln_itm->stock_id . "Desc", $ln_itm->item_description, 30, 50);
	qty_cell($ln_itm->quantity);
	label_cell($ln_itm->units);
	qty_cell($ln_itm->qty_inv);

	text_cells(null, $ln_itm->stock_id, $ln_itm->qty_dispatched, 10, 10);

	$display_discount_percent = number_format2($ln_itm->discount_percent*100,user_percent_dec()) . "%";

	$line_total = ($ln_itm->qty_dispatched * $ln_itm->price * (1 - $ln_itm->discount_percent));

	amount_cell($ln_itm->price);
	label_cell($ln_itm->tax_type_name);
	label_cell($display_discount_percent, "nowrap align=right");
	amount_cell($line_total);

	//label_cell(get_tax_free_price_for_item($ln_itm->stock_id, $line_total, $_POST['tax_group_id']));

	end_row();
}

/*Don't re-calculate freight if some of the order has already been delivered -
depending on the business logic required this condition may not be required.
It seems unfair to charge the customer twice for freight if the order
was not fully delivered the first time ?? */

if (!isset($_POST['ChargeFreightCost']) || $_POST['ChargeFreightCost'] == "") 
{
    if ($_SESSION['Items']->any_already_delivered() == 1) 
    {
    	$_POST['ChargeFreightCost'] = 0;
    } 
    else 
    {
    	$_POST['ChargeFreightCost'] = $_SESSION['Items']->freight_cost;
    }
    if (!is_numeric($_POST['ChargeFreightCost']))
    {
    	$_POST['ChargeFreightCost'] = 0;
    }
}

start_row();

small_amount_cells(_("Shipping Cost"), 'ChargeFreightCost', null, "colspan=9 align=right");

$inv_items_total = $_SESSION['Items']->get_items_total_dispatch();

$display_sub_total = number_format2($inv_items_total + $_POST['ChargeFreightCost'],user_price_dec());

label_row(_("Sub-total"), $display_sub_total, "colspan=9 align=right","align=right");

$taxes = $_SESSION['Items']->get_taxes($_POST['tax_group_id'], $_POST['ChargeFreightCost']);
$tax_total = display_edit_tax_items($taxes, 9);

$display_total = number_format2(($inv_items_total + $_POST['ChargeFreightCost'] + $tax_total), user_price_dec());

label_row(_("Invoice Total"), $display_total, "colspan=9 align=right","align=right");

end_table(1);

if ($has_marked)
	display_note(_("Marked items have insufficient quantities in stock."), 0, 1, "class='red'");

start_table($table_style2);

policy_list_row(_("Action For Balance"), "bo_policy", null);

textarea_row(_("Memo"), 'InvoiceText', null, 50, 4);

end_table(1);

submit_center_first('Update', _("Update"));
submit_center_last('process_invoice', _("Process Invoice"));

end_form();

//---------------------------------------------------------------------------------------------

end_page();

?>
