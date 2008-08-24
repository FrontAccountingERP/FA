<?php

$page_security=5;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("Suppliers"));

//include($path_to_root . "/includes/date_functions.inc");

include($path_to_root . "/includes/ui.inc");

check_db_has_tax_groups(_("There are no tax groups defined in the system. At least one tax group is required before proceeding."));

$new_supplier = get_post('supplier_id') == ''; 

if (isset($_POST['submit'])) 
{

	//initialise no input errors assumed initially before we test
	$input_error = 0;

	/* actions to take once the user has clicked the submit button
	ie the page has called itself with some user input */

	//first off validate inputs sensible

	if (strlen($_POST['supp_name']) == 0 || $_POST['supp_name'] == "") 
	{
		$input_error = 1;
		display_error(_("The supplier name must be entered."));
		set_focus('supp_name');
	}

	if ($input_error !=1 )
	{

		if (!$new_supplier) 
		{

			$sql = "UPDATE ".TB_PREF."suppliers SET supp_name=".db_escape($_POST['supp_name']) . ",
                address=".db_escape($_POST['address']) . ",
                email=".db_escape($_POST['email']) . ",
                bank_account=".db_escape($_POST['bank_account']) . ",
                dimension_id=".db_escape($_POST['dimension_id']) . ",
                dimension2_id=".db_escape($_POST['dimension2_id']) . ",
                curr_code=".db_escape($_POST['curr_code']).",
                payment_terms=".db_escape($_POST['payment_terms']) . ",
				payable_account=".db_escape($_POST['payable_account']) . ",
				purchase_account=".db_escape($_POST['purchase_account']) . ",
				payment_discount_account=".db_escape($_POST['payment_discount_account']) . ",
				tax_group_id=".db_escape($_POST['tax_group_id']) . " WHERE supplier_id = '" . $_POST['supplier_id'] . "'";

			db_query($sql,"The supplier could not be updated");
			display_notification(_("Supplier has been updated."));
		} 
		else 
		{

			$sql = "INSERT INTO ".TB_PREF."suppliers (supp_name, address, email, bank_account, dimension_id, dimension2_id, curr_code,
				payment_terms, payable_account, purchase_account, payment_discount_account, tax_group_id)
				VALUES (".db_escape($_POST['supp_name']). ", "
				.db_escape($_POST['address']) . ", "
				.db_escape($_POST['email']). ", "
				.db_escape($_POST['bank_account']). ", "
				.db_escape($_POST['dimension_id']). ", "
				.db_escape($_POST['dimension2_id']). ", "
				.db_escape($_POST['curr_code']). ", "
				.db_escape($_POST['payment_terms']). ", "
				.db_escape($_POST['payable_account']). ", "
				.db_escape($_POST['purchase_account']). ", "
				.db_escape($_POST['payment_discount_account']). ", "
				.db_escape($_POST['tax_group_id']). ")";

			db_query($sql,"The supplier could not be added");
			$_POST['supplier_id'] = db_insert_id();
			$new_supplier = false;
			display_notification(_("A new supplier has been added."));
			$Ajax->activate('_page_body');
		}
	}

} 
elseif (isset($_POST['delete']) && $_POST['delete'] != "") 
{
	//the link to delete a selected record was clicked instead of the submit button

	$cancel_delete = 0;

	// PREVENT DELETES IF DEPENDENT RECORDS IN 'supp_trans' , purch_orders

	$sql= "SELECT COUNT(*) FROM ".TB_PREF."supp_trans WHERE supplier_id='" . $_POST['supplier_id'] . "'";
	$result = db_query($sql,"check failed");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		$cancel_delete = 1;
		display_error(_("Cannot delete this supplier because there are transactions that refer to this supplier."));

	} 
	else 
	{
		$sql= "SELECT COUNT(*) FROM ".TB_PREF."purch_orders WHERE supplier_id='" . $_POST['supplier_id'] . "'";
		$result = db_query($sql,"check failed");
		$myrow = db_fetch_row($result);
		if ($myrow[0] > 0) 
		{
			$cancel_delete = 1;
			display_error(_("Cannot delete the supplier record because purchase orders have been created against this supplier."));
		}

	}
	if ($cancel_delete == 0) 
	{
		$sql="DELETE FROM ".TB_PREF."suppliers WHERE supplier_id='" . $_POST['supplier_id']. "'";
		db_query($sql,"check failed");

		unset($_SESSION['supplier_id']);
		$new_supplier = true;
		$Ajax->activate('_page_body');
	} //end if Delete supplier
}

start_form();

if (db_has_suppliers()) 
{
	start_table("", 3);
//	start_table("class = 'tablestyle_noborder'");
	supplier_list_row(_("Select a supplier: "), 'supplier_id', null,
		  _('New supplier'), true);
	end_table();
} 
else 
{
	hidden('supplier_id', get_post('supplier_id'));
}

//start_table("class='tablestyle2'", 0, 3);
start_table("class='tablestyle'", 3);

table_section_title(_("Supplier"));

if (!$new_supplier) 
{
	//SupplierID exists - either passed when calling the form or from the form itself
	$myrow = get_supplier($_POST['supplier_id']);

	$_POST['supp_name'] = $myrow["supp_name"];
	$_POST['address']  = $myrow["address"];
	$_POST['email']  = $myrow["email"];
	$_POST['bank_account']  = $myrow["bank_account"];
	$_POST['dimension_id']  = $myrow["dimension_id"];
	$_POST['dimension2_id']  = $myrow["dimension2_id"];
	$_POST['curr_code']  = $myrow["curr_code"];
	$_POST['payment_terms']  = $myrow["payment_terms"];
	$_POST['tax_group_id'] = $myrow["tax_group_id"];
	$_POST['payable_account']  = $myrow["payable_account"];
	$_POST['purchase_account']  = $myrow["purchase_account"];
	$_POST['payment_discount_account'] = $myrow["payment_discount_account"];

} 
else 
{
	$_POST['supp_name'] = $_POST['address'] = $_POST['tax_group_id']  = '';
	$_POST['dimension_id'] = 0;
	$_POST['dimension2_id'] = 0;
	$_POST['sales_type'] = -1;
	$_POST['email'] = $_POST['bank_account'] = '';
	$_POST['payment_terms']  = '';
	$_POST['credit_limit']	= price_format(sys_prefs::default_credit_limit());

	$company_record = get_company_prefs();
	$_POST['curr_code']  = $company_record["curr_default"];
	$_POST['payable_account'] = $company_record["creditors_act"];
	$_POST['purchase_account'] = $company_record["default_cogs_act"];
	$_POST['payment_discount_account'] = $company_record['pyt_discount_act'];
}

text_row(_("Supplier Name:"), 'supp_name', null, 42, 40);
textarea_row(_("Address:"), 'address', null, 35, 5);
text_row(_("Email:"), 'email', null, 42, 40);
text_row(_("Bank Account:"), 'bank_account', null, 42, 40);

// Sherifoz 23.09.03 currency can't be changed if editing
if (!$new_supplier) 
{
	label_row(_("Supplier's Currency:"), $_POST['curr_code']);
	hidden('curr_code', $_POST['curr_code']);
} 
else 
{
	currencies_list_row(_("Supplier's Currency:"), 'curr_code', null);
}

tax_groups_list_row(_("Tax Group:"), 'tax_group_id', null);

payment_terms_list_row(_("Payment Terms:"), 'payment_terms', null);

table_section_title(_("Accounts"));

gl_all_accounts_list_row(_("Accounts Payable Account:"), 'payable_account', $_POST['payable_account']);

gl_all_accounts_list_row(_("Purchase Account:"), 'purchase_account', $_POST['purchase_account']);

gl_all_accounts_list_row(_("Purchase Discount Account:"), 'payment_discount_account', $_POST['payment_discount_account']);

$dim = get_company_pref('use_dimension');
if ($dim >= 1)
{
	table_section_title(_("Dimension"));

	dimensions_list_row(_("Dimension")." 1:", 'dimension_id', null, true, " ", false, 1);
	if ($dim > 1)
		dimensions_list_row(_("Dimension")." 2:", 'dimension2_id', null, true, " ", false, 2);
}
if ($dim < 1)
	hidden('dimension_id', 0);
if ($dim < 2)
	hidden('dimension2_id', 0);

end_table(1);
div_start('controls');
if (!$new_supplier) 
{
	submit_center_first('submit', _("Update Supplier"), 
	  _('Update supplier data'), true);
	submit_center_last('delete', _("Delete Supplier"), 
	  _('Delete supplier data if have been never used'), true);
}
else 
{
	submit_center('submit', _("Add New Supplier Details"), true, '', true);
}
div_end();
end_form();

end_page();

?>
