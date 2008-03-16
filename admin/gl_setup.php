<?php

$page_security =10;
$path_to_root="..";
include($path_to_root . "/includes/session.inc");

page(_("System and General GL Setup"));

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/admin/db/company_db.inc");

//-------------------------------------------------------------------------------------------------

function can_process() 
{
	if (!check_num('po_over_receive', 0, 100)) 
	{
		display_error(_("The delivery over-receive allowance must be between 0 and 100."));
		return false;
	}

	if (!check_num('po_over_charge', 0, 100)) 
	{
		display_error(_("The invoice over-charge allowance must be between 0 and 100."));
		return false;
	}

	if (!check_num('past_due_days', 0, 100)) 
	{
		display_error(_("The past due days interval allowance must be between 0 and 100."));
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------------------------

if (isset($_POST['submit']) && can_process()) 
{
	update_company_gl_setup($_POST['debtors_act'], $_POST['pyt_discount_act'],
		$_POST['creditors_act'], $_POST['grn_act'],
		$_POST['exchange_diff_act'], $_POST['purch_exchange_diff_act'],
		$_POST['retained_earnings_act'], $_POST['freight_act'],
		$_POST['default_sales_act'],
		$_POST['default_sales_discount_act'],
		$_POST['default_prompt_payment_act'],
		$_POST['default_inventory_act'],
		$_POST['default_cogs_act'],
		$_POST['default_adj_act'],
		$_POST['default_inv_sales_act'],
		$_POST['default_assembly_act'], $_POST['payroll_act'],
		check_value('allow_negative_stock'),
		input_num('po_over_receive'),
		input_num('po_over_charge'),
		$_POST['past_due_days'],
		$_POST['default_credit_limit'],
		$_POST['default_workorder_required'],
		$_POST['default_dim_required']);

	display_notification(_("The general GL setup has been updated."));

} /* end of if submit */

//-------------------------------------------------------------------------------------------------

start_form();
start_table("class='tablestyle'");

$myrow = get_company_prefs();

$_POST['debtors_act']  = $myrow["debtors_act"];
$_POST['creditors_act']  = $myrow["creditors_act"];
$_POST['grn_act'] = $myrow["grn_act"];
$_POST['retained_earnings_act'] = $myrow["retained_earnings_act"];
$_POST['freight_act'] = $myrow["freight_act"];
$_POST['exchange_diff_act']  = $myrow["exchange_diff_act"];

$_POST['purch_exchange_diff_act']  = $myrow["purch_exchange_diff_act"];
$_POST['pyt_discount_act']  = $myrow["pyt_discount_act"];

$_POST['default_sales_act'] = $myrow["default_sales_act"];
$_POST['default_sales_discount_act']  = $myrow["default_sales_discount_act"];
$_POST['default_prompt_payment_act']  = $myrow["default_prompt_payment_act"];

$_POST['default_inventory_act'] = $myrow["default_inventory_act"];
$_POST['default_cogs_act'] = $myrow["default_cogs_act"];
$_POST['default_adj_act'] = $myrow["default_adj_act"];
$_POST['default_inv_sales_act'] = $myrow['default_inv_sales_act'];
$_POST['default_assembly_act'] = $myrow['default_assembly_act'];
$_POST['payroll_act'] = $myrow['payroll_act'];

$_POST['allow_negative_stock'] = $myrow['allow_negative_stock'];

$_POST['po_over_receive'] = percent_format($myrow['po_over_receive']);
$_POST['po_over_charge'] = percent_format($myrow['po_over_charge']);
$_POST['past_due_days'] = $myrow['past_due_days'];

$_POST['default_credit_limit'] = $myrow['default_credit_limit'];

$_POST['default_workorder_required'] = $myrow['default_workorder_required'];
$_POST['default_dim_required'] = $myrow['default_dim_required'];

//echo "<table>";

//---------------


table_section_title(_("General GL"));

gl_all_accounts_list_row(_("Retained Earning Clearing Account:"), 'retained_earnings_act', $_POST['retained_earnings_act']);
gl_all_accounts_list_row(_("Payroll Account:"), 'payroll_act', $_POST['payroll_act']);
text_row(_("Past Due Days Interval:"), 'past_due_days', $_POST['past_due_days'], 6, 6, "", _("days"));

//---------------

table_section_title(_("Customers and Sales"));

text_row(_("Default Credit Limit:"), 'default_credit_limit', $_POST['default_credit_limit'], 12, 12);

gl_all_accounts_list_row(_("Sales Exchange Variances Account:"), 'exchange_diff_act', $_POST['exchange_diff_act']);

gl_all_accounts_list_row(_("Shipping Charged Account:"), 'freight_act', $_POST['freight_act']);

//---------------

table_section_title(_("Customers and Sales Defaults"));

gl_all_accounts_list_row(_("Accounts Receivable Account:"), 'debtors_act', $_POST['debtors_act']);

gl_all_accounts_list_row(_("Sales Account:"), 'default_sales_act', $_POST['default_sales_act']);

gl_all_accounts_list_row(_("Sales Discount Account:"), 'default_sales_discount_act', $_POST['default_sales_discount_act']);

gl_all_accounts_list_row(_("Prompt Payment Discount Account:"), 'default_prompt_payment_act', $_POST['default_prompt_payment_act']);

//---------------

table_section_title(_("Suppliers and Purchasing"));

percent_row(_("Delivery Over-Receive Allowance:"), 'po_over_receive');

percent_row(_("Invoice Over-Charge Allowance:"), 'po_over_charge');

gl_all_accounts_list_row(_("Purchases Exchange Variances Account:"), 'purch_exchange_diff_act', $_POST['purch_exchange_diff_act']);

gl_all_accounts_list_row(_("Goods Received Clearing Account:"), 'grn_act', $_POST['grn_act']);

table_section_title(_("Suppliers and Purchasing Defaults"));

gl_all_accounts_list_row(_("Accounts Payable Account:"), 'creditors_act', $_POST['creditors_act']);

gl_all_accounts_list_row(_("Purchase Discount Account:"), 'pyt_discount_act', $_POST['pyt_discount_act']);

//---------------

table_section_title(_("Inventory Defaults"));

check_row(_("Allow Negative Inventory:"), 'allow_negative_stock', null);

gl_all_accounts_list_row(_("Sales Account:"), 'default_inv_sales_act', $_POST['default_inv_sales_act']);

gl_all_accounts_list_row(_("Inventory Account:"), 'default_inventory_act', $_POST['default_inventory_act']);

gl_all_accounts_list_row(_("C.O.G.S. Account:"), 'default_cogs_act', $_POST['default_cogs_act']);

gl_all_accounts_list_row(_("Inventory Adjustments Account:"), 'default_adj_act', $_POST['default_adj_act']);

gl_all_accounts_list_row(_("Item Assembly Costs Account:"), 'default_assembly_act', $_POST['default_assembly_act']);

//----------------

table_section_title(_("Manufacturing Defaults"));

text_row(_("Default Work Order Required By After:"), 'default_workorder_required', $_POST['default_workorder_required'], 6, 6, "", _("days"));

//----------------

table_section_title(_("Dimension Defaults"));

text_row(_("Default Dimension Required By After:"), 'default_dim_required', $_POST['default_dim_required'], 6, 6, "", _("days"));

//----------------

end_table(1);

submit_center('submit', _("Update"));

end_form(2);

//-------------------------------------------------------------------------------------------------

end_page();

?>
