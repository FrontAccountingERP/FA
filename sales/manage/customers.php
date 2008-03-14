<?php

$page_security = 3;
$path_to_root="../..";

include_once($path_to_root . "/includes/session.inc");
page(_("Customers")); 

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/banking.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/data_checks.inc");

if (isset($_GET['New']) || !isset($_POST['customer_id']) || $_POST['customer_id'] == "") 
{
	$_POST['New'] = "1";
}

if (isset($_POST['SelectCustomer'])) 
{
	unset($_POST['New']);
}

//--------------------------------------------------------------------------------------------

function can_process()
{
	if (strlen($_POST['CustName']) == 0) 
	{
		display_error(_("The customer name cannot be empty."));
		return false;
	} 
	
	if (!check_num('credit_limit', 0))
	{
		display_error(_("The credit limit must be numeric and not less than zero."));
		return false;		
	} 
	
	if (!check_num('pymt_discount', 0, 100)) 
	{
		display_error(_("The payment discount must be numeric and is expected to be less than 100% and greater than or equal to 0."));
		return false;		
	} 
	
	if (!check_num('discount', 0, 100)) 
	{
		display_error(_("The discount percentage must be numeric and is expected to be less than 100% and greater than or equal to 0."));
		return false;		
	} 

	return true;
}

//--------------------------------------------------------------------------------------------

function handle_submit()
{
	global $path_to_root;
	if (!can_process())
		return;
		
	if (!isset($_POST['New'])) 
	{

		$sql = "UPDATE ".TB_PREF."debtors_master SET name='" . $_POST['CustName'] . "', 
			address='" . $_POST['address'] . "', 
			tax_id='" . $_POST['tax_id'] . "', 
			curr_code='" . $_POST['curr_code'] . "', 
			email='" . $_POST['email'] . "', 
			dimension_id=" . $_POST['dimension_id'] . ", 
			dimension2_id=" . $_POST['dimension2_id'] . ", 
            credit_status='" . $_POST['credit_status'] . "', 
            payment_terms='" . $_POST['payment_terms'] . "', 
            discount=" . input_num('discount') / 100 . ", 
            pymt_discount=" . input_num('pymt_discount') / 100 . ", 
            credit_limit=" . input_num('credit_limit') . ", 
            sales_type = '" . $_POST['sales_type'] . "' 
            WHERE debtor_no = '" . $_POST['customer_id'] . "'";

		db_query($sql,"The customer could not be updated");
		display_notification(_("Customer has been updated."));
		clear_fields();			

	} 
	else 
	{ 	//it is a new customer

		begin_transaction();

		$sql = "INSERT INTO ".TB_PREF."debtors_master (name, address, tax_id, email, dimension_id, dimension2_id,  
			curr_code, credit_status, payment_terms, discount, pymt_discount,credit_limit, 
			sales_type) VALUES ('" . $_POST['CustName'] ."', '" . $_POST['address'] . "', '" . $_POST['tax_id'] . "',
			'" . $_POST['email'] . "', " . $_POST['dimension_id'] . ", " . $_POST['dimension2_id'] . ", '" . $_POST['curr_code'] . "', 
			" . $_POST['credit_status'] . ", '" . $_POST['payment_terms'] . "', " . input_num('discount')/100 . ", 
			" . input_num('pymt_discount')/100 . ", " . $_POST['credit_limit'] . ", '" . $_POST['sales_type'] . "')";

		db_query($sql,"The customer could not be added");

		$new_customer_id = db_insert_id();
		
		commit_transaction();			

		display_notification(_("A new customer has been added."));

		hyperlink_params($path_to_root . "/sales/manage/customer_branches.php", _("Add branches for this customer"), "debtor_no=$new_customer_id");

		clear_fields();
	}
}

//--------------------------------------------------------------------------------------------

if (isset($_POST['submit'])) 
{
	
	handle_submit();
}

//-------------------------------------------------------------------------------------------- 

if (isset($_POST['delete'])) 
{

	//the link to delete a selected record was clicked instead of the submit button

	$cancel_delete = 0;

	// PREVENT DELETES IF DEPENDENT RECORDS IN 'debtor_trans'

	$sql= "SELECT COUNT(*) FROM ".TB_PREF."debtor_trans WHERE debtor_no='" . $_POST['customer_id'] . "'";
	$result = db_query($sql,"check failed");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		$cancel_delete = 1;
		display_error(_("This customer cannot be deleted because there are transactions that refer to it."));
	} 
	else 
	{
		$sql= "SELECT COUNT(*) FROM ".TB_PREF."sales_orders WHERE debtor_no='" . $_POST['customer_id'] . "'";
		$result = db_query($sql,"check failed");
		$myrow = db_fetch_row($result);
		if ($myrow[0] > 0) 
		{
			$cancel_delete = 1;
			display_error(_("Cannot delete the customer record because orders have been created against it."));
		} 
		else 
		{
			$sql = "SELECT COUNT(*) FROM ".TB_PREF."cust_branch WHERE debtor_no='" . $_POST['customer_id'] . "'";
			$result = db_query($sql,"check failed");
			$myrow = db_fetch_row($result);
			if ($myrow[0] > 0) 
			{
				$cancel_delete = 1;
				display_error(_("Cannot delete this customer because there are branch records set up against it."));
				//echo "<br> There are " . $myrow[0] . " branch records relating to this customer";
			}
		}
	}
	
	if ($cancel_delete == 0) 
	{ 	//ie not cancelled the delete as a result of above tests
		$sql = "DELETE FROM ".TB_PREF."debtors_master WHERE debtor_no='" . $_POST['customer_id'] . "'";
		db_query($sql,"cannot delete customer");
		
		meta_forward($_SERVER['PHP_SELF']); 
	} //end if Delete Customer
}

function clear_fields() 
{
	unset($_POST['CustName']);
	unset($_POST['address']);
	unset($_POST['tax_id']);
	unset($_POST['email']);
	unset($_POST['dimension_id']);
	unset($_POST['dimension2_id']);
	unset($_POST['credit_status']);
	unset($_POST['payment_terms']);
	unset($_POST['discount']);
	unset($_POST['pymt_discount']);
	unset($_POST['credit_limit']);
	unset($_POST['sales_type']);
	unset($_POST['customer_id']);
	$_POST['New'] = 1;
}

check_db_has_sales_types(_("There are no sales types defined. Please define at least one sales type before adding a customer."));
 
start_form();

if (db_has_customers()) 
{
	start_table("class = 'tablestyle_noborder'");
	start_row();
	customer_list_cells(_("Select a customer: "), 'customer_id', null);
	submit_cells('SelectCustomer', _("Edit Customer"));
	end_row();
	end_table();
} 
else 
{
	hidden('customer_id', $_POST['customer_id']);
}

hyperlink_params($_SERVER['PHP_SELF'], _("Enter a new customer"), "New=1");
echo "<br>";     

start_table($table_style2, 7, 6);
echo "<tr valign=top><td>"; // outer table	

start_table("class='tablestyle_noborder'");	

if (isset($_POST['New'])) 
{

	hidden('New', 'Yes');

	$_POST['CustName'] = $_POST['address'] = $_POST['tax_id']  = '';
	$_POST['dimension_id'] = 0;
	$_POST['dimension2_id'] = 0;
	$_POST['sales_type'] = -1;
	$_POST['curr_code']  = get_company_currency();
	$_POST['credit_status']  = -1;
	$_POST['payment_terms']  = '';
	$_POST['discount']  = $_POST['pymt_discount'] = percent_format(0);
	$_POST['credit_limit']	= price_format(sys_prefs::default_credit_limit());
} 
else 
{

	$sql = "SELECT * FROM ".TB_PREF."debtors_master WHERE debtor_no = '" . $_POST['customer_id'] . "'";
	$result = db_query($sql,"check failed");

	$myrow = db_fetch($result);

	$_POST['CustName'] = $myrow["name"];
	$_POST['address']  = $myrow["address"];
	$_POST['tax_id']  = $myrow["tax_id"];
	$_POST['email']  = $myrow["email"];
	$_POST['dimension_id']  = $myrow["dimension_id"];
	$_POST['dimension2_id']  = $myrow["dimension2_id"];
	$_POST['sales_type'] = $myrow["sales_type"];
	$_POST['curr_code']  = $myrow["curr_code"];
	$_POST['credit_status']  = $myrow["credit_status"];
	$_POST['payment_terms']  = $myrow["payment_terms"];
	$_POST['discount']  = percent_format($myrow["discount"] * 100);
	$_POST['pymt_discount']  = percent_format($myrow["pymt_discount"] * 100);
	$_POST['credit_limit']	= price_format($myrow["credit_limit"]);
}

text_row(_("Customer Name:"), 'CustName', $_POST['CustName'], 40, 40);
textarea_row(_("Address:"), 'address', $_POST['address'], 35, 5);

text_row(_("Email:"), 'email', null, 40, 40);
text_row(_("GSTNo:"), 'tax_id', null, 40, 40);


// Sherifoz 23.09.03 currency can't be changed if editing
if (isset($_POST['New'])) 
{
	currencies_list_row(_("Customer's Currency:"), 'curr_code', $_POST['curr_code']);
} 
else 
{
	label_row(_("Customer's Currency:"), $_POST['curr_code']);
	hidden('curr_code', $_POST['curr_code']);				
}	
end_table();

echo "</td><td class='tableseparator'>"; // outer table

start_table("class='tablestyle_noborder'");	

sales_types_list_row(_("Sales Type/Price List:"), 'sales_type', $_POST['sales_type']);
$dim = get_company_pref('use_dimension');
if ($dim >= 1)
	dimensions_list_row(_("Dimension")." 1:", 'dimension_id', $_POST['dimension_id'], true, " ", false, 1);
if ($dim > 1)
	dimensions_list_row(_("Dimension")." 2:", 'dimension2_id', $_POST['dimension2_id'], true, " ", false, 2);
if ($dim < 1)
	hidden('dimension_id', 0);
if ($dim < 2)
	hidden('dimension2_id', 0);

percent_row(_("Discount Percent:"), 'discount', $_POST['discount']);
percent_row(_("Prompt Payment Discount Percent:"), 'pymt_discount', $_POST['pymt_discount']);
amount_row(_("Credit Limit:"), 'credit_limit', $_POST['credit_limit']);

payment_terms_list_row(_("Payment Terms:"), 'payment_terms', $_POST['payment_terms']);
credit_status_list_row(_("Credit Status:"), 'credit_status', $_POST['credit_status']); 

end_table();

end_table(1); // outer table	

if (isset($_POST['New'])) 
{
	submit_center('submit', _("Add New Customer"));
} 
else 
{
	submit_center_first('submit', _("Update Customer"));
	submit_center_last('delete', _("Delete Customer"));
}

end_form();
end_page();

?>
