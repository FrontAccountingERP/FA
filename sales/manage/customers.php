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
$page_security = 'SA_CUSTOMER';
$path_to_root = "../..";

include_once($path_to_root . "/includes/session.inc");
page(_($help_context = "Customers"), @$_REQUEST['popup']); 

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/banking.inc");
include_once($path_to_root . "/includes/ui.inc");

if (isset($_GET['debtor_no'])) 
{
	$_POST['customer_id'] = $_GET['debtor_no'];
}
$new_customer = (!isset($_POST['customer_id']) || $_POST['customer_id'] == ""); 
//--------------------------------------------------------------------------------------------

function can_process()
{
	if (strlen($_POST['CustName']) == 0) 
	{
		display_error(_("The customer name cannot be empty."));
		set_focus('CustName');
		return false;
	} 

	if (strlen($_POST['cust_ref']) == 0) 
	{
		display_error(_("The customer short name cannot be empty."));
		set_focus('cust_ref');
		return false;
	} 
	
	if (!check_num('credit_limit', 0))
	{
		display_error(_("The credit limit must be numeric and not less than zero."));
		set_focus('credit_limit');
		return false;		
	} 
	
	if (!check_num('pymt_discount', 0, 100)) 
	{
		display_error(_("The payment discount must be numeric and is expected to be less than 100% and greater than or equal to 0."));
		set_focus('pymt_discount');
		return false;		
	} 
	
	if (!check_num('discount', 0, 100)) 
	{
		display_error(_("The discount percentage must be numeric and is expected to be less than 100% and greater than or equal to 0."));
		set_focus('discount');
		return false;		
	} 

	return true;
}

//--------------------------------------------------------------------------------------------

function handle_submit()
{
	global $path_to_root, $new_customer, $Ajax;

	if (!can_process())
		return;
		
	if ($new_customer == false) 
	{
		update_customer($_POST['customer_id'], $_POST['CustName'], $_POST['cust_ref'], $_POST['address'],
			$_POST['tax_id'], $_POST['curr_code'], $_POST['email'], $_POST['dimension_id'], $_POST['dimension2_id'],
			$_POST['credit_status'], $_POST['payment_terms'], input_num('discount') / 100, input_num('pymt_discount') / 100,
			input_num('credit_limit'), $_POST['sales_type'], $_POST['notes'], $_POST['rep_lang']);

		update_record_status($_POST['customer_id'], $_POST['inactive'],
			'debtors_master', 'debtor_no');

		$Ajax->activate('customer_id'); // in case of status change
		display_notification(_("Customer has been updated."));
	} 
	else 
	{ 	//it is a new customer

		begin_transaction();
		add_customer($_POST['CustName'], $_POST['cust_ref'], $_POST['address'],
			$_POST['tax_id'], $_POST['curr_code'], $_POST['email'], $_POST['dimension_id'], $_POST['dimension2_id'],
			$_POST['credit_status'], $_POST['payment_terms'], input_num('discount') / 100, input_num('pymt_discount') / 100,
			input_num('credit_limit'), $_POST['sales_type'], $_POST['notes'], $_POST['rep_lang']);

		$_POST['customer_id'] = db_insert_id();
		$new_customer = false;
		commit_transaction();			

		display_notification(_("A new customer has been added."));

		$Ajax->activate('_page_body');
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
	$sel_id = db_escape($_POST['customer_id']);

	if (key_in_foreign_table($sel_id, 'debtor_trans', 'debtor_no', true))
	{
		$cancel_delete = 1;
		display_error(_("This customer cannot be deleted because there are transactions that refer to it."));
	} 
	else 
	{
		if (key_in_foreign_table($sel_id, 'sales_orders', 'debtor_no', true))
		{
			$cancel_delete = 1;
			display_error(_("Cannot delete the customer record because orders have been created against it."));
		} 
		else 
		{
			if (key_in_foreign_table($sel_id, 'cust_branch', 'debtor_no', true))
			{
				$cancel_delete = 1;
				display_error(_("Cannot delete this customer because there are branch records set up against it."));
				//echo "<br> There are " . $myrow[0] . " branch records relating to this customer";
			}
		}
	}
	
	if ($cancel_delete == 0) 
	{ 	//ie not cancelled the delete as a result of above tests
	
		delete_customer($sel_id, true);

		display_notification(_("Selected customer has been deleted."));
		unset($_POST['customer_id']);
		$new_customer = true;
		$Ajax->activate('_page_body');
	} //end if Delete Customer
}

check_db_has_sales_types(_("There are no sales types defined. Please define at least one sales type before adding a customer."));
 
start_form();

if (db_has_customers()) 
{
	start_table(TABLESTYLE_NOBORDER);
	start_row();
	customer_list_cells(_("Select a customer: "), 'customer_id', null,
		_('New customer'), true, check_value('show_inactive'));
	check_cells(_("Show inactive:"), 'show_inactive', null, true);
	end_row();
	end_table();
	if (get_post('_show_inactive_update')) {
		$Ajax->activate('customer_id');
		set_focus('customer_id');
	}
} 
else 
{
	hidden('customer_id');
}

if ($new_customer) 
{
	$_POST['CustName'] = $_POST['cust_ref'] = $_POST['address'] = $_POST['tax_id']  = '';
	$_POST['dimension_id'] = 0;
	$_POST['dimension2_id'] = 0;
	$_POST['sales_type'] = -1;
	$_POST['rep_lang'] = '';
	$_POST['email'] = '';
	$_POST['curr_code']  = get_company_currency();
	$_POST['credit_status']  = -1;
	$_POST['payment_terms']  = $_POST['notes']  = '';

	$_POST['discount']  = $_POST['pymt_discount'] = percent_format(0);
	$_POST['credit_limit']	= price_format($SysPrefs->default_credit_limit());
	$_POST['inactive'] = 0;
} 
else 
{
	$myrow = get_customer($_POST['customer_id']);

	$_POST['CustName'] = $myrow["name"];
	$_POST['cust_ref'] = $myrow["debtor_ref"];
	$_POST['address']  = $myrow["address"];
	$_POST['tax_id']  = $myrow["tax_id"];
	$_POST['email']  = $myrow["email"];
	$_POST['dimension_id']  = $myrow["dimension_id"];
	$_POST['dimension2_id']  = $myrow["dimension2_id"];
	$_POST['sales_type'] = $myrow["sales_type"];
	$_POST['rep_lang'] = $myrow["rep_lang"];
	$_POST['curr_code']  = $myrow["curr_code"];
	$_POST['credit_status']  = $myrow["credit_status"];
	$_POST['payment_terms']  = $myrow["payment_terms"];
	$_POST['discount']  = percent_format($myrow["discount"] * 100);
	$_POST['pymt_discount']  = percent_format($myrow["pymt_discount"] * 100);
	$_POST['credit_limit']	= price_format($myrow["credit_limit"]);
	$_POST['notes']  = $myrow["notes"];
	$_POST['inactive'] = $myrow["inactive"];
}

start_outer_table(TABLESTYLE2);
table_section(1);
table_section_title(_("Name and Address"));

text_row(_("Customer Name:"), 'CustName', $_POST['CustName'], 40, 80);
text_row(_("Customer Short Name:"), 'cust_ref', null, 30, 30);
textarea_row(_("Address:"), 'address', $_POST['address'], 35, 5);

email_row(_("E-mail:"), 'email', null, 40, 40);
text_row(_("GSTNo:"), 'tax_id', null, 40, 40);


if ($new_customer) 
{
	currencies_list_row(_("Customer's Currency:"), 'curr_code', $_POST['curr_code']);
} 
else 
{
	label_row(_("Customer's Currency:"), $_POST['curr_code']);
	hidden('curr_code', $_POST['curr_code']);				
}	
sales_types_list_row(_("Sales Type/Price List:"), 'sales_type', $_POST['sales_type']);
languages_list_row( _("Document Language:"), 'rep_lang', $_POST['rep_lang'], _('System default'));

table_section(2);

table_section_title(_("Sales"));

percent_row(_("Discount Percent:"), 'discount', $_POST['discount']);
percent_row(_("Prompt Payment Discount Percent:"), 'pymt_discount', $_POST['pymt_discount']);
amount_row(_("Credit Limit:"), 'credit_limit', $_POST['credit_limit']);

payment_terms_list_row(_("Payment Terms:"), 'payment_terms', $_POST['payment_terms']);
credit_status_list_row(_("Credit Status:"), 'credit_status', $_POST['credit_status']); 
$dim = get_company_pref('use_dimension');
if ($dim >= 1)
	dimensions_list_row(_("Dimension")." 1:", 'dimension_id', $_POST['dimension_id'], true, " ", false, 1);
if ($dim > 1)
	dimensions_list_row(_("Dimension")." 2:", 'dimension2_id', $_POST['dimension2_id'], true, " ", false, 2);
if ($dim < 1)
	hidden('dimension_id', 0);
if ($dim < 2)
	hidden('dimension2_id', 0);

if (!$new_customer)  {
	start_row();
	echo '<td>'._('Customer branches').':</td>';
  	hyperlink_params_td($path_to_root . "/sales/manage/customer_branches.php",
		'<b>'. (@$_REQUEST['popup'] ?  _("Select or &Add") : _("&Add or Edit ")).'</b>', 
		"debtor_no=".$_POST['customer_id'].(@$_REQUEST['popup'] ? '&popup=1':''));
	end_row();

}

textarea_row(_("General Notes:"), 'notes', null, 35, 5);
record_status_list_row(_("Customer status:"), 'inactive');
end_outer_table(1);

div_start('controls');
if ($new_customer)
{
	submit_center('submit', _("Add New Customer"), true, '', 'default');
} 
else 
{
	submit_center_first('submit', _("Update Customer"), 
	  _('Update customer data'), @$_REQUEST['popup'] ? true : 'default');
	submit_return('select', get_post('customer_id'), _("Select this customer and return to document entry."));
	submit_center_last('delete', _("Delete Customer"), 
	  _('Delete customer data if have been never used'), true);
}
div_end();
hidden('popup', @$_REQUEST['popup']);
end_form();
end_page();

?>
