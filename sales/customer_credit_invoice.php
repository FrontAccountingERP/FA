<?php

$page_security =3;
$path_to_root="..";

include_once($path_to_root . "/sales/includes/cart_class.inc");
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/includes/manufacturing.inc");
include_once($path_to_root . "/sales/includes/sales_db.inc");
include_once($path_to_root . "/sales/includes/sales_ui.inc");
$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();
page(_("Credit all or part of an Invoice"), false, false, "", $js);

//---------------------------------------------------------------------------------------------------------------

if (isset($_GET['AddedID'])) 
{
	$credit_no = $_GET['AddedID'];
	$trans_type = 11;

	echo "<center>";
	display_notification_centered(_("Credit Note has been processed"));
	display_note(get_customer_trans_view_str($trans_type, $credit_no, _("View this credit note")), 0, 0);
	
 	display_note(get_gl_view_str($trans_type, $credit_no, _("View the GL Journal Entries for this Credit Note")));

	display_footer_exit();
}

//--------------------------------------------------------------------------------------

if (!isset($_GET['InvoiceNumber']) && !$_SESSION['InvoiceToCredit']) 
{
	/* This page can only be called with an invoice number for crediting*/
	die (_("This page can only be opened if an invoice has been selected for crediting."));
}

//--------------------------------------------------------------------------------------

function can_process()
{
	if (!is_date($_POST['CreditDate'])) 
	{
		display_error(_("The entered date is invalid."));;
		return false;
	} 
	elseif (!is_date_in_fiscalyear($_POST['CreditDate'])) 
	{
		display_error(_("The entered date is not in fiscal year."));
		return false;
	}

	if (!references::is_valid($_POST['ref'])) 
	{
		display_error(_("You must enter a reference."));;
		return false;
	}

	if (!is_new_reference($_POST['ref'], 11)) 
	{
		display_error(_("The entered reference is already in use."));;
		return false;
	}

	if (!is_numeric($_POST['ChargeFreightCost']) || $_POST['ChargeFreightCost'] < 0) 
	{
		display_error(_("The entered shipping cost is invalid or less than zero."));;
		return false;
	}

	return true;
}

//--------------------------------------------------------------------------------------

function clear_globals()
{
	if (isset($_SESSION['Items'])) 
	{
		unset($_SESSION['Items']->line_items);
		unset($_SESSION['Items']);
	}
	unset($_SESSION['InvoiceToCredit']);
}

//--------------------------------------------------------------------------------------

function process_credit()
{
	global $path_to_root;
	if (can_process())
	{
		$credit_no = credit_invoice($_SESSION['Items'], $_SESSION['InvoiceToCredit'],
			$_SESSION['Order'],	$_POST['CreditDate'], $_POST['CreditType'], 
			$_POST['tax_group_id'],	$_POST['ChargeFreightCost'], $_POST['ref'], 
			$_POST['CreditText'], $_POST['WriteOffGLCode']);

   		clear_globals();

   		meta_forward($_SERVER['PHP_SELF'], "AddedID=$credit_no");
	}
}

//--------------------------------------------------------------------------------------

if (isset($_GET['InvoiceNumber']) && $_GET['InvoiceNumber'] > 0) 
{

	clear_globals();

	session_register("Items");
	session_register("InvoiceToCredit");
	session_Register("Order");

	$_SESSION['InvoiceToCredit'] = $_GET['InvoiceNumber'];
	$_SESSION['Items'] = new cart;

	/*read in all the guff from the selected invoice into the Items cart	*/

	// we need a distinct here so that it only returns 1 line - becuase there can be mutliple moves
	// per item (for assemblies, etc)
	$sql = "SELECT DISTINCT ".TB_PREF."debtor_trans.*,
		".TB_PREF."cust_branch.default_location, ".TB_PREF."cust_branch.default_ship_via,
		".TB_PREF."debtors_master.name, ".TB_PREF."debtors_master.curr_code,
    	".TB_PREF."tax_groups.name AS tax_group_name, ".TB_PREF."tax_groups.id AS tax_group_id,
    	".TB_PREF."sales_orders.from_stk_loc
    	FROM ".TB_PREF."debtor_trans, ".TB_PREF."debtors_master, ".TB_PREF."cust_branch, ".TB_PREF."tax_groups, ".TB_PREF."sales_orders
    	WHERE ".TB_PREF."debtor_trans.debtor_no = ".TB_PREF."debtors_master.debtor_no
    	AND ".TB_PREF."debtor_trans.branch_code = ".TB_PREF."cust_branch.branch_code
    	AND ".TB_PREF."debtor_trans.debtor_no = ".TB_PREF."cust_branch.debtor_no
    	AND ".TB_PREF."cust_branch.tax_group_id = ".TB_PREF."tax_groups.id
    	AND ".TB_PREF."debtor_trans.trans_no = " . $_GET['InvoiceNumber'] . "
		AND ".TB_PREF."debtor_trans.type=10
    	AND ".TB_PREF."sales_orders.order_no=".TB_PREF."debtor_trans.order_";

	$result = db_query($sql,"The invoice details cannot be retrieved");

	if (db_num_rows($result) == 1) 
	{

		$myrow = db_fetch($result);

		$_SESSION['Items']->customer_id = $myrow["debtor_no"];
		$_SESSION['Items']->customer_name = $myrow["name"];
		$_SESSION['Items']->cust_ref = $myrow["reference"];
		$_SESSION['Items']->Branch = $myrow["branch_code"];
		$_SESSION['Items']->customer_currency = $myrow["curr_code"];

		$_SESSION['Items']->Comments = "";
		$_SESSION['Items']->default_sales_type =$myrow["tpe"];
		$_SESSION['Items']->Location = $myrow["from_stk_loc"];

		$_SESSION['Items']->tax_group_name = $myrow["tax_group_name"];
		$_SESSION['Items']->tax_group_id = $myrow["tax_group_id"];

		$_SESSION['Items']->delivery_date = sql2date($myrow["tran_date"]);
		$_SESSION['Items']->freight_cost = $myrow["ov_freight"];

		$_SESSION['Items']->ship_via = $myrow["default_ship_via"];

		$_SESSION['Order'] = $myrow["order_"];

		db_free_result($result);

		/*now populate the line items array with the detail records for the invoice*/

		$result = get_customer_trans_details(10, $_GET['InvoiceNumber']);

		if (db_num_rows($result) > 0) 
		{

			while ($myrow = db_fetch($result)) 
			{

				$_SESSION['Items']->add_to_cart($myrow["stock_id"],-$myrow["quantity"],
					$myrow["FullUnitPrice"],$myrow["discount_percent"]);

				$_SESSION['Items']->line_items[$myrow["stock_id"]]->standard_cost = $myrow["standard_cost"];

			}

		} /*else { // there are no item records created for that invoice
			// it's ok there might be shipping or only taxes !!
			echo "<CENTER><A HREF='$path_to_root/index.php?" . SID . "'>" . _("Back to the main menu") . "</A>";
			echo "<P>" . _("There are no line items that were retrieved for this invoice. A credit note cannot be created from this invoice."). "<BR><BR>";
			end_page();
			exit;

		}*/ //end of checks on returned data set
		db_free_result($result);
	} 
	else 
	{
		display_error(_("This invoice can not be credited using the automatic facility."));
		display_error("Please report that a duplicate debtor_trans header record was found for invoice " . $SESSION['InvoiceToCredit']);
		echo "<br><br>";
		exit;
	} //valid invoice record returned from the entered invoice number

} 
else 
{

/* if processing, the page has been called and ${$StkItm->stock_id} would have been set from the post */
	foreach ($_SESSION['Items']->line_items as $itm) 
	{
		$_SESSION['Items']->line_items[$itm->stock_id]->qty_dispatched = $_POST[$itm->stock_id];

		if (isset($_POST[$itm->stock_id . "Desc"]) && strlen($_POST[$itm->stock_id . "Desc"]) > 0) 
		{
			$_SESSION['Items']->line_items[$itm->stock_id]->item_description = $_POST[$itm->stock_id . "Desc"];
		}
	}
}

//--------------------------------------------------------------------------------------

if (isset($_POST['ProcessCredit']))
{
	process_credit();
}

//--------------------------------------------------------------------------------------

if (isset($_POST['Location']))
{
	$_SESSION['Items']->Location = $_POST['Location'];
}

//--------------------------------------------------------------------------------------

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
    
	if (!isset($_POST['ref']))
		$_POST['ref'] = references::get_next(11);

    ref_cells(_("Reference"), 'ref', null, "class='tableheader2'");

    label_cells(_("Crediting Invoice"), get_customer_trans_view_str(10, $_SESSION['InvoiceToCredit']), "class='tableheader2'");

    if (!isset($_POST['tax_group_id']))
    	$_POST['tax_group_id'] = $_SESSION['Items']->tax_group_id;
    label_cell(_("Tax Group"), "class='tableheader2'");	
    tax_groups_list_cells(null, 'tax_group_id', $_POST['tax_group_id'], false, null, true);
	end_row();
	end_table();

    echo "</td><td>";// outer table

    start_table("$table_style width=100%");

    label_row(_("Invoice Date"), $_SESSION['Items']->delivery_date, "class='tableheader2'");

    date_row(_("Credit Note Date"), 'CreditDate', null, 0, 0, 0, "class='tableheader2'");

    end_table();

	echo "</td></tr>";
	
	end_table(1); // outer table

    start_table("$table_style width=80%");
    $th = array(_("Item Code"), _("Item Description"), _("Invoiced Quantity"), _("Units"),
    	_("Credit Quantity"), _("Price"), _("Discount %"), _("Total"));
    table_header($th);	

    $k = 0; //row colour counter

    foreach ($_SESSION['Items']->line_items as $ln_itm) 
    {

    	alt_table_row_color($k);

    	$line_total =($ln_itm->qty_dispatched * $ln_itm->price * (1 - $ln_itm->discount_percent));

    	label_cell($ln_itm->stock_id);
		text_cells(null, $ln_itm->stock_id . "Desc", $ln_itm->item_description, 30, 50);
    	qty_cell($ln_itm->quantity);
    	label_cell($ln_itm->units);

    	text_cells(null, $ln_itm->stock_id, $ln_itm->qty_dispatched, 13, 15);

    	amount_cell($ln_itm->price);
    	amount_cell($ln_itm->discount_percent*100);
    	amount_cell($line_total);
    	end_row();
    }

    if (!isset($_POST['ChargeFreightCost']) || ($_POST['ChargeFreightCost'] == ""))
    {
    	$_POST['ChargeFreightCost'] = $_SESSION['Items']->freight_cost;
    }
	start_row();
	label_cell(_("Credit Shipping Cost"), "colspan=7 align=right");
    text_cells(null, "ChargeFreightCost", $_POST['ChargeFreightCost'], 6, 6);
	end_row();

    $inv_items_total = $_SESSION['Items']->get_items_total_dispatch();

    $display_sub_total = number_format2($inv_items_total + $_POST['ChargeFreightCost'],user_price_dec());
    label_row(_("Sub-total"), $display_sub_total, "colspan=7 align=right", "align=right");

    $taxes = $_SESSION['Items']->get_taxes($_POST['tax_group_id'], $_POST['ChargeFreightCost']);

    $tax_total = display_edit_tax_items($taxes, 7);

    $display_total = number_format2(($inv_items_total + $_POST['ChargeFreightCost'] + $tax_total), user_price_dec());

    label_row(_("Credit Note Total"), $display_total, "colspan=7 align=right", "align=right");

    end_table();
}

//--------------------------------------------------------------------------------------

function display_credit_options()
{
	global $table_style2;
	
    echo "<br>";
    start_table($table_style2);

    echo "<tr><td>" . _("Credit Note Type") . "</td>";
    echo "<td><select name='CreditType' onchange='this.form.submit();'>";
    if (!isset($_POST['CreditType']) || $_POST['CreditType'] == "Return")
    {
    	echo "<option value='WriteOff'>" . _("Items Written Off") . "</option>";
    	echo "<option selected value='Return'>" . _("Items Returned to Inventory Location") . "</option>";
    } 
    else 
    {
    	echo "<option selected value='WriteOff'>" . _("Items Written Off") . "</option>";
    	echo "<option value='Return'>" . _("Items Returned to Inventory Location") . "</option>";
    }
    echo "</select>";
    echo"</td></tr>";

    if (!isset($_POST['CreditType']) || $_POST['CreditType'] == "Return")
    {

    	/*if the credit note is a return of goods then need to know which location to receive them into */
    	if (!isset($_POST['Location']))
    	{
    		$_POST['Location'] = $_SESSION['Items']->Location;
    	}

    	locations_list_row(_("Items Returned to Inventory Location"), 'Location', $_POST['Location']);

    } 
    else 
    { 	/* the goods are to be written off to somewhere */

    	gl_all_accounts_list_row(_("Write Off the Cost of the Items to"), 'WriteOffGLCode', $_POST['WriteOffGLCode']);
    }
    textarea_row(_("Memo"), "CreditText", null, 45, 3);
    end_table();
}

//--------------------------------------------------------------------------------------

display_credit_items();
display_credit_options();

echo "<br><center>";
submit('Update', _("Update"));
echo "&nbsp";
submit('ProcessCredit', _("Process Credit Note"));
echo "</center>";

end_form();

//--------------------------------------------------------------------------------------

end_page();

?>
