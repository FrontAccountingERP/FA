<?php

/*The credit selection screen uses the Cart class used for the making up orders
some of the variable names refer to order - please think credit when you read order */

$page_security = 3;
$path_to_root="..";
include_once($path_to_root . "/sales/includes/cart_class.inc");

include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/sales/includes/sales_db.inc");

include_once($path_to_root . "/sales/includes/sales_ui.inc");

include_once($path_to_root . "/sales/includes/ui/sales_credit_ui.inc");
include_once($path_to_root . "/sales/includes/ui/sales_order_ui.inc");

$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();
page(_("Customer Credit Note"), false, false, "", $js);

//-----------------------------------------------------------------------------------------------

check_db_has_stock_items(_("There are no items defined in the system."));

check_db_has_customer_branches(_("There are no customers, or there are no customers with branches. Please define customers and customer branches."));

//---------------------------------------------------------------------------------------------------------------

if (isset($_GET['AddedID'])) 
{
	$credit_no = $_GET['AddedID'];
	$trans_type = 11;

	display_notification_centered(_("Credit Note has been processed"));
	display_note(get_customer_trans_view_str($trans_type, $credit_no, _("View this credit note")), 0, 1);
	
 	display_note(get_gl_view_str($trans_type, $credit_no, _("View the GL Journal Entries for this Credit Note")));

	hyperlink_params($_SERVER['PHP_SELF'], _("Enter Another Credit Note"), "NewCredit=yes");

	display_footer_exit();
}

//--------------------------------------------------------------------------------------------------

function copy_to_cn()
{
	$_SESSION['credit_items']->memo_ = $_POST['CreditText'];

	$_SESSION['credit_items']->orig_order_date = $_POST['OrderDate'];
	$_SESSION['credit_items']->freight_cost = $_POST['ChargeFreightCost'];

	$_SESSION['credit_items']->Location = $_POST["Location"];

   	$_SESSION['credit_items']->default_sales_type = $_POST['sales_type_id'];
	$_SESSION['credit_items']->tax_group_id = $_POST["tax_group_id"];
}

//--------------------------------------------------------------------------------------------------

function copy_from_cn()
{
	$_POST['CreditText'] = $_SESSION['credit_items']->memo_;

	$_POST['OrderDate'] = $_SESSION['credit_items']->orig_order_date;
	$_POST['ChargeFreightCost'] = $_SESSION['credit_items']->freight_cost;

	$_POST["Location"] = $_SESSION['credit_items']->Location;

   	$_POST['sales_type_id'] = $_SESSION['credit_items']->default_sales_type;
	$_POST["tax_group_id"] = $_SESSION['credit_items']->tax_group_id;
}

//-----------------------------------------------------------------------------------------------

function handle_new_order()
{
	if (isset($_SESSION['credit_items']))
	{
		unset ($_SESSION['credit_items']->line_items);
		unset ($_SESSION['credit_items']);
	}

    session_register("credit_items");

    $_SESSION['credit_items'] = new cart;
	$_POST['OrderDate'] = Today();
	if (!is_date_in_fiscalyear($_POST['OrderDate']))
		$_POST['OrderDate'] = end_fiscalyear();
	$_SESSION['credit_items']->orig_order_date = $_POST['OrderDate'];	
    
}

//-----------------------------------------------------------------------------------------------

if (isset($_POST['ProcessCredit']))
{

	$input_error = 0;

	if (!references::is_valid($_POST['ref'])) 
	{
		display_error( _("You must enter a reference."));
		$input_error = 1;
	} 
	elseif (!is_new_reference($_POST['ref'], 11)) 
	{
		display_error( _("The entered reference is already in use."));
		$input_error = 1;
	} 
	elseif (!is_date($_POST['OrderDate'])) 
	{
		display_error(_("The entered date for the credit note is invalid."));
		$input_error = 1;
	} 
	elseif (!is_date_in_fiscalyear($_POST['OrderDate'])) 
	{
		display_error(_("The entered date is not in fiscal year."));
		$input_error = 1;
	}
	if ($input_error == 1)
		unset($_POST['ProcessCredit']);
}

//------------------------------------------------------------------------------------

if (isset($_POST['ProcessCredit']))
{
	//alert("WriteOffGLCode = ".$_POST['WriteOffGLCode'].", CreditType = ".$_POST['CreditType']);
	if ($_POST['CreditType'] == "WriteOff" && (!isset($_POST['WriteOffGLCode']) || 
	 	$_POST['WriteOffGLCode'] == ''))
	{
		display_note(_("For credit notes created to write off the stock, a general ledger account is required to be selected."), 1, 0);
	  	display_note(_("Please select an account to write the cost of the stock off to, then click on Process again."), 1, 0);
		exit;
	}
	if (!isset($_POST['WriteOffGLCode']))
		$_POST['WriteOffGLCode'] = "";
	if (!isset($_POST['ShipperID']))
		$_POST['ShipperID'] = 0;
	$credit_no = add_credit_note($_SESSION['credit_items'], $_POST['OrderDate'],
		$_POST['CreditType'], $_POST['tax_group_id'],
		$_POST['ChargeFreightCost'], $_POST['sales_type_id'], $_POST['ShipperID'],
		$_POST['ref'], $_POST['CreditText'], $_POST['WriteOffGLCode']);

	unset($_SESSION['credit_items']->line_items);
	unset($_SESSION['credit_items']);

	meta_forward($_SERVER['PHP_SELF'], "AddedID=$credit_no");

} /*end of process credit note */

//---------------------------------------------------------------------------------------------------

function get_details_from_customer()
{
	return get_customer_details_to_order($_SESSION['credit_items'], $_POST['customer_id'], $_POST['branch_id']);
}

//-----------------------------------------------------------------------------------------------

function check_item_data()
{
	if ($_POST['qty'] <= 0)
	{
		display_error(_("The quantity must be greater than zero."));
		return false;
	}
	if (!is_numeric($_POST['price']) || $_POST['price'] < 0)
	{
		display_error(_("The entered price is negative or invalid."));
		return false;
	}
	if (!is_numeric($_POST['Disc']) || $_POST['Disc'] > 100 || $_POST['Disc'] < 0)
	{
		display_error(_("The entered discount percent is negative, greater than 100 or invalid."));
		return false;
	}
   	return true;
}

//-----------------------------------------------------------------------------------------------

function handle_update_item()
{
    if($_POST['UpdateItem'] != "" && check_item_data())
    {
    	$_SESSION['credit_items']->update_cart_item($_POST['stock_id'], $_POST['qty'], 
    		$_POST['price'], ($_POST['Disc'] / 100));
    }
}

//-----------------------------------------------------------------------------------------------

function handle_delete_item()
{
	$_SESSION['credit_items']->remove_from_cart($_GET['Delete']);
}

//-----------------------------------------------------------------------------------------------

function handle_new_item()
{
	if (!check_item_data())
		return;

	add_to_order($_SESSION['credit_items'], $_POST['stock_id'], $_POST['qty'],
		$_POST['price'], $_POST['Disc'] / 100);
}

//-----------------------------------------------------------------------------------------------
if (isset($_GET['Delete']) || isset($_GET['Edit']))
	copy_from_cn();

if (isset($_GET['Delete']))
	handle_delete_item();

if (isset($_POST['AddItem']) || isset($_POST['UpdateItem']))
	copy_to_cn();

if (isset($_POST['AddItem']))
	handle_new_item();

if (isset($_POST['UpdateItem']))
	handle_update_item();

//-----------------------------------------------------------------------------------------------

if (isset($_GET['NewCredit']) || !isset($_SESSION['credit_items']))
{
	handle_new_order();
} 
else 
{
	if (!isset($_POST['customer_id']))
		$_POST['customer_id'] = $_SESSION['credit_items']->customer_id;
	if (!isset($_POST['branch_id']))
		$_POST['branch_id'] = $_SESSION['credit_items']->Branch;
}

//-----------------------------------------------------------------------------------------------

start_form(false, true);

$customer_error = display_credit_header($_SESSION['credit_items']);

if ($customer_error == "")
{
	start_table("$table_style width=80%", 10);
	echo "<tr><td>";
    display_credit_items(_("Credit Note Items"), $_SESSION['credit_items']);
    credit_options_controls();
    echo "</td></tr>";
    end_table();
} 
else
{
	display_error($customer_error);
}

if (!isset($_POST['ProcessCredit']))
{
    echo "<br><center><table><tr>";
    submit_cells('Update', _("Update"));
	if ($_SESSION['credit_items']->count_items() >= 1
			/*OR $_POST['ChargeTax'] > 0*/ || (isset($_POST['ChargeFreightCost']) && $_POST['ChargeFreightCost'] > 0))
	{
	    submit_cells('ProcessCredit', _("Process Credit Note"));
	}
	echo "</tr></table>";
}

end_form();
end_page();

?>
