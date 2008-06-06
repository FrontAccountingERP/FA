<?php
//---------------------------------------------------------------------------
//
//	Entry/Modify Credit Note for selected Sales Invoice
//

$page_security = 3;
$path_to_root = "..";

include_once($path_to_root . "/sales/includes/cart_class.inc");
include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/data_checks.inc");
include_once($path_to_root . "/includes/manufacturing.inc");
include_once($path_to_root . "/sales/includes/sales_db.inc");
include_once($path_to_root . "/sales/includes/sales_ui.inc");
include_once($path_to_root . "/reporting/includes/reporting.inc");

$js = "";
if ($use_popup_windows) {
	$js .= get_js_open_window(900, 500);
}

if ($use_date_picker) {
	$js .= get_js_date_picker();
}

if (isset($_GET['ModifyCredit'])) {
	$_SESSION['page_title'] = sprintf(_("Modifying Credit Invoice # %d."), $_GET['ModifyCredit']);
	$help_page_title =_("Modifying Credit Invoice");
	processing_start();
} elseif (isset($_GET['InvoiceNumber'])) {
	$_SESSION['page_title'] = _("Credit all or part of an Invoice");
	processing_start();
}
page($_SESSION['page_title'], false, false, "", $js);

//-----------------------------------------------------------------------------

if (isset($_GET['AddedID'])) {
	$credit_no = $_GET['AddedID'];
	$trans_type = 11;
	print_hidden_script(11);

	display_notification_centered(_("Credit Note has been processed"));

	display_note(get_customer_trans_view_str($trans_type, $credit_no, _("View This Credit Note")), 0, 0);

	display_note(print_document_link($credit_no, _("Print This Credit Note"), true, 11),1);

 	display_note(get_gl_view_str($trans_type, $credit_no, _("View the GL Journal Entries for this Credit Note")),1);

	display_footer_exit();

} elseif (isset($_GET['UpdatedID'])) {
	$credit_no = $_GET['UpdatedID'];
	$trans_type = 11;
	print_hidden_script(11);

	display_notification_centered(_("Credit Note has been updated"));

	display_note(get_customer_trans_view_str($trans_type, $credit_no, _("View This Credit Note")), 0, 0);

	display_note(print_document_link($credit_no, _("Print This Credit Note"), true, 11),1);

 	display_note(get_gl_view_str($trans_type, $credit_no, _("View the GL Journal Entries for this Credit Note")),1);

	display_footer_exit();
}

//-----------------------------------------------------------------------------

function can_process()
{
	if (!is_date($_POST['CreditDate'])) {
		display_error(_("The entered date is invalid."));;
		set_focus('CreditDate');
		return false;
	} elseif (!is_date_in_fiscalyear($_POST['CreditDate']))	{
		display_error(_("The entered date is not in fiscal year."));
		set_focus('CreditDate');
		return false;
	}

    if ($_SESSION['Items']->trans_no==0) {
		if (!references::is_valid($_POST['ref'])) {
			display_error(_("You must enter a reference."));;
			set_focus('ref');
			return false;
		}

		if (!is_new_reference($_POST['ref'], 11)) {
			display_error(_("The entered reference is already in use."));;
			set_focus('ref');
			return false;
		}
    }
	if (!check_num('ChargeFreightCost', 0)) {
		display_error(_("The entered shipping cost is invalid or less than zero."));;
		set_focus('ChargeFreightCost');
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------

if (isset($_GET['InvoiceNumber']) && $_GET['InvoiceNumber'] > 0) {

    $ci = new Cart(10, $_GET['InvoiceNumber'], true);

    if ($ci==null) {
		display_error(_("This invoice can not be credited using the automatic facility."));
		display_error("Please report that a duplicate debtor_trans header record was found for invoice " . key($ci->src_docs));
		echo "<br><br>";
		processing_end();
		exit;
    }
    //valid invoice record returned from the entered invoice number

    $ci->trans_type = 11;
    $ci->src_docs = $ci->trans_no;
    $ci->src_date = $ci->document_date;
    $ci->trans_no = 0;
    $ci->document_date = Today();
    $ci->reference = references::get_next(11);

    for ($line_no=0; $line_no<count($ci->line_items); $line_no++) {
	$ci->line_items[$line_no]->qty_dispatched = '0';
    }

    $_SESSION['Items'] = $ci;
	copy_from_cart();

} elseif ( isset($_GET['ModifyCredit']) && $_GET['ModifyCredit']>0) {

	$_SESSION['Items'] = new Cart(11,$_GET['ModifyCredit']);
	copy_from_cart();

} elseif (!processing_active()) {
	/* This page can only be called with an invoice number for crediting*/
	die (_("This page can only be opened if an invoice has been selected for crediting."));
} else {
	foreach ($_SESSION['Items']->line_items as $line_no=>$itm) {
		if (isset($_POST['Line'.$line_no])) {
			if (check_num('Line'.$line_no, ($itm->quantity - $itm->qty_done))) {
				$_SESSION['Items']->line_items[$line_no]->qty_dispatched = 
				  input_num('Line'.$line_no);
			}
	  	}

		if (isset($_POST['Line'.$line_no.'Desc'])) {
			$line_desc = $_POST['Line'.$line_no.'Desc'];
			if (strlen($line_desc) > 0) {
				$_SESSION['Items']->line_items[$line_no]->item_description = $line_desc;
			}
	  	}
	}
}
//-----------------------------------------------------------------------------

function copy_to_cart()
{
  $cart = &$_SESSION['Items'];
  $cart->ship_via = $_POST['ShipperID'];
  $cart->freight_cost = input_num('ChargeFreightCost');
  $cart->document_date =  $_POST['CreditDate'];
  $cart->Location = $_POST['Location'];
  $cart->Comments = $_POST['CreditText'];
}
//-----------------------------------------------------------------------------

function copy_from_cart()
{
  $cart = &$_SESSION['Items'];
  $_POST['ShipperID'] = $cart->ship_via;
  $_POST['ChargeFreightCost'] = price_format($cart->freight_cost);
  $_POST['CreditDate']= $cart->document_date;
  $_POST['Location']= $cart->Location;
  $_POST['CreditText']= $cart->Comments;
}
//-----------------------------------------------------------------------------

if (isset($_POST['ProcessCredit']) && can_process()) {

    $newcredit = ($_SESSION['Items']->trans_no == 0);

    if (!isset($_POST['WriteOffGLCode']))
		$_POST['WriteOffGLCode'] = 0;

	copy_to_cart();
    $credit_no = $_SESSION['Items']->write($_POST['WriteOffGLCode']);

	processing_end();
	if ($newcredit) {
	   	meta_forward($_SERVER['PHP_SELF'], "AddedID=$credit_no");
	} else {
	   	meta_forward($_SERVER['PHP_SELF'], "UpdatedID=$credit_no");
	}
}

//-----------------------------------------------------------------------------

if (isset($_POST['Location'])) {
	$_SESSION['Items']->Location = $_POST['Location'];
}

//-----------------------------------------------------------------------------

function display_credit_items()
{
	global $table_style, $table_style2;

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

//	if (!isset($_POST['ref']))
//		$_POST['ref'] = references::get_next(11);

    if ($_SESSION['Items']->trans_no==0) {
		ref_cells(_("Reference"), 'ref', '', $_SESSION['Items']->reference, "class='tableheader2'");
	} else {
		label_cells(_("Reference"), $_SESSION['Items']->reference, "class='tableheader2'");
	}
//    label_cells(_("Crediting Invoice"), get_customer_trans_view_str(10, $_SESSION['InvoiceToCredit']), "class='tableheader2'");
    label_cells(_("Crediting Invoice"), get_customer_trans_view_str(10, array_keys($_SESSION['Items']->src_docs)), "class='tableheader2'");

	if (!isset($_POST['ShipperID'])) {
		$_POST['ShipperID'] = $_SESSION['Items']->ship_via;
	}
	label_cell(_("Shipping Company"), "class='tableheader2'");
	shippers_list_cells(null, 'ShipperID', $_POST['ShipperID']);
//	if (!isset($_POST['sales_type_id']))
//	  $_POST['sales_type_id'] = $_SESSION['Items']->sales_type;
//	label_cell(_("Sales Type"), "class='tableheader2'");
//	sales_types_list_cells(null, 'sales_type_id', $_POST['sales_type_id']);

	end_row();
	end_table();

    echo "</td><td>";// outer table

    start_table("$table_style width=100%");

    label_row(_("Invoice Date"), $_SESSION['Items']->src_date, "class='tableheader2'");

    date_row(_("Credit Note Date"), 'CreditDate', '', null, 0, 0, 0, "class='tableheader2'");

    end_table();

	echo "</td></tr>";

	end_table(1); // outer table

    start_table("$table_style width=80%");
    $th = array(_("Item Code"), _("Item Description"), _("Invoiced Quantity"), _("Units"),
    	_("Credit Quantity"), _("Price"), _("Discount %"), _("Total"));
    table_header($th);

    $k = 0; //row colour counter

    foreach ($_SESSION['Items']->line_items as $line_no=>$ln_itm) {
		if ($ln_itm->quantity==$ln_itm->qty_done) {
			continue; // this line was fully credited
		}
		alt_table_row_color($k);


		//	view_stock_status_cell($ln_itm->stock_id); alternative view
    	label_cell($ln_itm->stock_id);

	text_cells(null, 'Line'.$line_no.'Desc', $ln_itm->item_description, 30, 50);

    	qty_cell($ln_itm->quantity);
    	label_cell($ln_itm->units);

	amount_cells(null, 'Line'.$line_no, qty_format($ln_itm->qty_dispatched));

    	$line_total =($ln_itm->qty_dispatched * $ln_itm->price * (1 - $ln_itm->discount_percent));

    	amount_cell($ln_itm->price);
    	percent_cell($ln_itm->discount_percent*100);
    	amount_cell($line_total);
    	end_row();
    }

    if (!check_num('ChargeFreightCost')) {
    	$_POST['ChargeFreightCost'] = price_format($_SESSION['Items']->freight_cost);
    }

	start_row();
	label_cell(_("Credit Shipping Cost"), "colspan=7 align=right");
    amount_cells(null, "ChargeFreightCost", $_POST['ChargeFreightCost'], 6, 6);
	end_row();

    $inv_items_total = $_SESSION['Items']->get_items_total_dispatch();

    $display_sub_total = price_format($inv_items_total + input_num($_POST['ChargeFreightCost']));
    label_row(_("Sub-total"), $display_sub_total, "colspan=7 align=right", "align=right");

    $taxes = $_SESSION['Items']->get_taxes(input_num($_POST['ChargeFreightCost']));

    $tax_total = display_edit_tax_items($taxes, 7, $_SESSION['Items']->tax_included);

    $display_total = price_format(($inv_items_total + input_num('ChargeFreightCost') + $tax_total));

    label_row(_("Credit Note Total"), $display_total, "colspan=7 align=right", "align=right");

    end_table();
}

//-----------------------------------------------------------------------------

function display_credit_options()
{
	global $table_style2;

    echo "<br>";
    start_table($table_style2);

    echo "<tr><td>" . _("Credit Note Type") . "</td>";
    echo "<td><select name='CreditType' onchange='this.form.submit();'>";
    if (!isset($_POST['CreditType']) || $_POST['CreditType'] == "Return") {
    	echo "<option value='WriteOff'>" . _("Items Written Off") . "</option>";
    	echo "<option selected value='Return'>" . _("Items Returned to Inventory Location") . "</option>";
    } else {
    	echo "<option selected value='WriteOff'>" . _("Items Written Off") . "</option>";
    	echo "<option value='Return'>" . _("Items Returned to Inventory Location") . "</option>";
    }
    echo "</select>";
    echo"</td></tr>";

    if (!isset($_POST['CreditType']) || $_POST['CreditType'] == "Return") {

    	/*if the credit note is a return of goods then need to know which location to receive them into */
    	if (!isset($_POST['Location'])) {
    		$_POST['Location'] = $_SESSION['Items']->Location;
    	}

    	locations_list_row(_("Items Returned to Inventory Location"), 'Location', $_POST['Location']);
    } else { 	/* the goods are to be written off to somewhere */
    	gl_all_accounts_list_row(_("Write Off the Cost of the Items to"), 'WriteOffGLCode', $_POST['WriteOffGLCode']);
    }
    textarea_row(_("Memo"), "CreditText", null, 45, 3);
    end_table();
}

//-----------------------------------------------------------------------------

display_credit_items();
display_credit_options();

echo "<br><center>";
submit('Update', _("Update"));
echo "&nbsp";
submit('ProcessCredit', _("Process Credit Note"));
echo "</center>";

end_form();


end_page();

?>
