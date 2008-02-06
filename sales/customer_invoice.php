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
page(_("Issue an Invoice for Delivery Note(s)"), false, false, "", $js);

//---------------------------------------------------------------------------------------------------------------

if (isset($_GET['AddedID'])) 
{
	$invoice_no = $_GET['AddedID'];
	$trans_type = 10;

	display_notification(_("Selected deliveries has been processed"), true);

	display_note(get_customer_trans_view_str($trans_type, $invoice_no, _("View this invoice")), 0, 1);

 	display_note(get_gl_view_str($trans_type, $invoice_no, _("View the GL Journal Entries for this Invoice")));

	hyperlink_params("$path_to_root/sales/inquiry/sales_deliveries_view.php", _("Select Another Delivery For Invoicing"), "OutstandingOnly=1");

	display_footer_exit();
}

//---------------------------------------------------------------------------------------------------------------

if (!isset($_GET['DeliveryNumber']) && !isset($_SESSION['ProcessingDelivery']) && 
	!isset($_GET['BatchInvoice']) && !isset($_GET['process_invoice'])) 
{
	/* This page can only be called with a delivery for invoicing*/
	display_error(_("This page can only be opened after delivery selection. Please select delivery to invoicing first."));

	hyperlink_no_params("$path_to_root/sales/inquiry/sales_deliveries_view.php", _("Select Delivery to Invoice"));

	end_page();
	exit;
} 
elseif (isset($_GET['DeliveryNumber'])|| isset($_GET['BatchInvoice'])) 
{


	if (isset($_SESSION['Items']))
	{
		unset($_SESSION['Items']->line_items);
		unset ($_SESSION['Items']);
	}

    if(isset($_GET['BatchInvoice'])) {
	  $_SESSION['ProcessingDelivery'] = $_SESSION['DeliveryBatch'];
	  unset($_SESSION['DeliveryBatch']);
    }
    else
	  $_SESSION['ProcessingDelivery'] = array($_GET['DeliveryNumber']);

    $_SESSION['Items'] = new cart('delivery');
    $_SESSION['Items']->trans_no = $_SESSION['ProcessingDelivery'];

	/*read in all the selected deliveries into the Items cart  */
	if (read_sales_delivery($_SESSION['ProcessingDelivery'], $_SESSION['Items'])) 
	{
  	  if ($_SESSION['Items']->count_items() == 0) 
	  {
  		hyperlink_params($path_to_root . "/sales/inquiry/sales_deliveries_view.php", _("Select a different delivery to invoice"), "OutstandingOnly=1");
  		die ("<br><b>" . _("There are no delivered items with a quantity left to invoice. There is nothing left to invoice.") . "</b>");
      }
	} 
	else 
	{
		hyperlink_no_params("/sales_deliveries_view.php", _("Select a delivery note to invoice"));
		die ("<br><b>" . _("This delivery note could not be retrieved. Please select another delivery.") . "</b>");
	}
} 
else 
{
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

function delivery_changed_error()
{
	global $path_to_root;
	display_note(_("This delivery note has been changed or invoiced since this invoice was started to be confirmed. Processing halted."), 1, 0);
	display_note(_("To enter and confirm this invoice the order must be re-selected and re-read again to update the changes made by the other user."), 1, 0);

	hyperlink_no_params("$path_to_root/sales/inquiry/sales_deliveries_view.php", _("Select a sales order for invoicing"));

	unset($_SESSION['ProcessingDelivery']);
	exit;
}

//---------------------------------------------------------------------------------------------------------------

function check_delivery_changed()
{
	global $debug;

	/*Now need to check that the delivery note details are the same 
	as they were when they were read into the Items array.
	If they've changed then someone else may have invoiced them */

	$sql = "SELECT id, stock_id, quantity, qty_done FROM "
		.TB_PREF."debtor_trans_details WHERE
		debtor_trans_type = 13 AND (";

	foreach($_SESSION['Items']->trans_no as $key=>$num) {
	    if($key!=0) $sql .= ' OR ';
	    $sql .= 'debtor_trans_no =' . $num;
	}
	$sql .= ') ORDER BY id';
	$result = db_query($sql,"while checking delivery changes");

	if (db_num_rows($result) != count($_SESSION['Items']->line_items))
	{

		/*there should be the same number of items returned from this query as there are lines on the invoice -
			if not then someone has already invoiced or credited some lines */
    	if ($debug == 1)
    	{
    		display_note($sql, 1, 0);
    		display_note("No rows returned by sql:" . db_num_rows($result), 1, 0);
    		display_note("Count of items in the session " . count($_SESSION['Items']->line_items), 1, 0);
    	}

		return false;
	}
	$line=0;
	while ($myrow = db_fetch($result)) 
	{
		$stk_itm = $myrow["stock_id"];

		if ($_SESSION['Items']->line_items[$line]->quantity != -$myrow["quantity"] ||
			$_SESSION['Items']->line_items[$line]->qty_done != -$myrow["qty_done"])
		{
			display_note(_("Original delivery for line #") . $line+1 . ' '.
				_("has a quantity of") . " " . -$myrow["quantity"] . " " . 
				_("and an delivered quantity of") . " " . -$myrow["qty_done"] . "." .
				_("Now the quantity of") . " " . 
				$_SESSION['Items']->line_items[$line]->quantity . " " . 
				_("and invoiced quantity of") . " " . 
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
	if (!isset($_POST['InvoiceDate']) || !is_date($_POST['InvoiceDate']))	
	{
		display_error(_("The entered invoice date is invalid."));
		return false;
	}
	if (!is_date_in_fiscalyear($_POST['InvoiceDate'])) 
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

function process_invoice($invoicing=false)
{
	if ($invoicing) 
	{
		read_sales_delivery($_SESSION['Items']->trans_no, $_SESSION['Items']);
		$duedate = get_invoice_duedate($_SESSION['Items']->customer_id, $_SESSION['Items']->delivery_date);
		$invoice_no = add_sales_invoice($_SESSION['Items'],
			$_SESSION['Items']->delivery_date, $duedate,
			$_SESSION['Items']->tax_group_id, $_SESSION['Items']->freight_cost,
			$_SESSION['Items']->Location, $_SESSION['Items']->ship_via,
			$_SESSION['Items']->default_sales_type,	references::get_next(10),
 			$_SESSION['Items']->memo_);
	} 
	else 
	{
	
		if (!check_data())
			return;

		if (!check_delivery_changed())
			delivery_changed_error();

		$invoice_no = add_sales_invoice($_SESSION['Items'],
			$_POST['InvoiceDate'], $_POST['due_date'],
			$_SESSION['Items']->tax_group_id,	
			$_POST['ChargeFreightCost'], 
			$_SESSION['Items']->Location, 
			$_POST['ship_via'],	$_POST['sales_type_id'], $_POST['ref'],
			$_POST['InvoiceText']);	
		unset($_SESSION['ProcessingDelivery']);
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
//echo "<tr><td>"; // outer table

//start_table("$table_style width=100%");
start_row();
label_cells(_("Customer"), $_SESSION['Items']->customer_name, "class='tableheader2'");
label_cells(_("Branch"), get_branch_name($_SESSION['Items']->Branch), "class='tableheader2'");
label_cells(_("Currency"), $_SESSION['Items']->customer_currency, "class='tableheader2'");
end_row();
start_row();

if (!isset($_POST['ref']))
	$_POST['ref'] = references::get_next(10);

ref_cells(_("Reference"), 'ref', null, "class='tableheader2'");

label_cells(_("Delivery Notes:"), get_customer_trans_view_str(systypes::cust_dispatch(), $_SESSION['ProcessingDelivery']), "class='tableheader2'");


if (!isset($_POST['sales_type_id']))
	$_POST['sales_type_id'] = $_SESSION['Items']->default_sales_type;
label_cell(_("Sales Type"), "class='tableheader2'");	
sales_types_list_cells(null, 'sales_type_id', $_POST['sales_type_id']);

end_row();
start_row();

if (!isset($_POST['ship_via']))
	$_POST['ship_via'] = $_SESSION['Items']->ship_via;
label_cell(_("Shipping Company"), "class='tableheader2'");	
shippers_list_cells(null, 'ship_via', $_POST['ship_via']);

if (!isset($_POST['InvoiceDate']) || !is_date($_POST['InvoiceDate']))
{
	$_POST['InvoiceDate'] = Today();
	if (!is_date_in_fiscalyear($_POST['InvoiceDate']))
		$_POST['InvoiceDate'] = end_fiscalyear();
}

date_cells(_("Date"), 'InvoiceDate', $_POST['InvoiceDate'], 0, 0, 0, "class='tableheader'");
//end_table();

//echo "</td><td>";// outer table

//start_table("$table_style width=90%");

if (!isset($_POST['due_date']) || !is_date($_POST['due_date']))

	$_POST['due_date'] = get_invoice_duedate($_SESSION['Items']->customer_id, $_POST['InvoiceDate']);

date_cells(_("Due Date"), 'due_date', $_POST['due_date'], 0, 0, 0, "class='tableheader'");
//end_table();

//echo "</td></tr>";
end_row();
end_table(); // outer table

display_heading(_("Invoice Items"));

start_table("$table_style width=80%");
$th = array(_("Item Code"), _("Item Description"), _("Delivered"), _("Units"), _("Invoiced"),
	_("This Invoice"), _("Price"), _("Tax Type"), _("Discount"), _("Total"));
table_header($th);
$k = 0;
$has_marked = false;
$show_qoh = true;

foreach ($_SESSION['Items']->line_items as $line=>$ln_itm) 
{
	if($ln_itm->quantity==$ln_itm->qty_done) continue; //this line is fully delivered
	alt_table_row_color($k);

	view_stock_status_cell($ln_itm->stock_id); // ?

	text_cells(null, 'Line'.$line.'Desc', $ln_itm->item_description, 30, 50);
	qty_cell($ln_itm->quantity);
	label_cell($ln_itm->units);
	qty_cell($ln_itm->qty_done);

	if(count($_SESSION['Items']->trans_no)>1) {
	    // for batch invoices we can process only whole deliveries
	    qty_cell($ln_itm->qty_dispatched);
	    hidden('Line'.$line,$ln_itm->qty_dispatched );
	} else
	    text_cells(null, 'Line'.$line, $ln_itm->qty_dispatched, 10, 10);

	$display_discount_percent = number_format2($ln_itm->discount_percent*100,user_percent_dec()) . "%";

	$line_total = ($ln_itm->qty_dispatched * $ln_itm->price * (1 - $ln_itm->discount_percent));

	amount_cell($ln_itm->price);
	label_cell($ln_itm->tax_type_name);
	label_cell($display_discount_percent, "nowrap align=right");
	amount_cell($line_total);

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

$taxes = $_SESSION['Items']->get_taxes($_SESSION['Items']->tax_group_id, $_POST['ChargeFreightCost']);
$tax_total = display_edit_tax_items($taxes, 9);

$display_total = number_format2(($inv_items_total + $_POST['ChargeFreightCost'] + $tax_total), user_price_dec());

label_row(_("Invoice Total"), $display_total, "colspan=9 align=right","align=right");

end_table(1);

//if ($has_marked)
//	display_note(_("Marked items have insufficient quantities in stock."), 0, 1, "class='red'");

start_table($table_style2);

textarea_row(_("Memo"), 'InvoiceText', null, 50, 4);

end_table(1);

submit_center_first('Update', _("Update"));
submit_center_last('process_invoice', _("Process Invoice"));

end_form();

//---------------------------------------------------------------------------------------------

end_page();

?>
