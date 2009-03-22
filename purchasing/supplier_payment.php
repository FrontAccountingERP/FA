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
$path_to_root="..";
$page_security = 5;
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/banking.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/purchasing/includes/purchasing_db.inc");
$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();
page(_("Supplier Payment Entry"), false, false, "", $js);


if (isset($_GET['supplier_id']))
{
	$_POST['supplier_id'] = $_GET['supplier_id'];
}

//----------------------------------------------------------------------------------------

check_db_has_suppliers(_("There are no suppliers defined in the system."));

check_db_has_bank_accounts(_("There are no bank accounts defined in the system."));

//----------------------------------------------------------------------------------------
if ($ret = context_restore()) {
	if(isset($ret['supplier_id']))
		$_POST['supplier_id'] = $ret['supplier_id'];
}
if (isset($_POST['_supplier_id_editor'])) {
	context_call($path_to_root.'/purchasing/manage/suppliers.php?supplier_id='.$_POST['supplier_id'], 
		array( 'supplier_id', 'bank_account', 'DatePaid', 'ref', 'amount', 
			'discount', 'memo_') );
}
if (isset($_POST['_DatePaid_changed'])) {
  $Ajax->activate('_ex_rate');
}
//----------------------------------------------------------------------------------------

if (isset($_GET['AddedID'])) 
{
	$payment_id = $_GET['AddedID'];

   	display_notification_centered( _("Payment has been sucessfully entered"));

    display_note(get_gl_view_str(22, $payment_id, _("View the GL &Journal Entries for this Payment")));

    hyperlink_params($path_to_root . "/purchasing/allocations/supplier_allocate.php", _("&Allocate this Payment"), "trans_no=$payment_id&trans_type=22");

	hyperlink_params($_SERVER['PHP_SELF'], _("Enter another supplier &payment"), "supplier_id=" . $_POST['supplier_id']);

	display_footer_exit();
}

//----------------------------------------------------------------------------------------

function display_controls()
{
	global $table_style2;
	start_form();

	if (!isset($_POST['supplier_id']))
		$_POST['supplier_id'] = get_global_supplier(false);
	if (!isset($_POST['DatePaid']))
	{
		$_POST['DatePaid'] = new_doc_date();
		if (!is_date_in_fiscalyear($_POST['DatePaid']))
			$_POST['DatePaid'] = end_fiscalyear();
	}		
	//start_table($table_style2, 5, 7);
	//echo "<tr><td valign=top>"; // outer table
	start_outer_table($table_style2, 5);

	//echo "<table>";
	table_section(1);
	
    bank_accounts_list_row(_("From Bank Account:"), 'bank_account', null, true);

	amount_row(_("Amount of Payment:"), 'amount');
	amount_row(_("Amount of Discount:"), 'discount');

    date_row(_("Date Paid") . ":", 'DatePaid', '', true, 0, 0, 0, null, true);

	table_section(2);
	//echo "</table>";
	//echo "</td><td valign=top class='tableseparator'>"; // outer table
	//echo "<table>";

    supplier_list_row(_("Payment To:"), 'supplier_id', null, false, true);

	set_global_supplier($_POST['supplier_id']);

	$supplier_currency = get_supplier_currency($_POST['supplier_id']);
	$bank_currency = get_bank_account_currency($_POST['bank_account']);
	if ($bank_currency != $supplier_currency) 
	{
		exchange_rate_display($bank_currency, $supplier_currency, $_POST['DatePaid'], true);
	}

    ref_row(_("Reference:"), 'ref', '', references::get_next(22));

    text_row(_("Memo:"), 'memo_', null, 52,50);

	//echo "</table>";

	//echo "</td></tr>";
	end_outer_table(1); // outer table

	submit_center('ProcessSuppPayment',_("Enter Payment"), true, '', 'default');

	if ($bank_currency != $supplier_currency) 
	{
		display_note(_("The amount and discount are in the bank account's currency."), 2, 0);
	}

	end_form();
}

//----------------------------------------------------------------------------------------

function check_inputs()
{
	if ($_POST['amount'] == "") 
	{
		$_POST['amount'] = price_format(0);
	}

	if (!check_num('amount', 0))
	{
		display_error(_("The entered amount is invalid or less than zero."));
		set_focus('amount');
		return false;
	}

	if (isset($_POST['_ex_rate']) && !check_num('_ex_rate', 0.000001))
	{
		display_error(_("The exchange rate must be numeric and greater than zero."));
		set_focus('_ex_rate');
		return false;
	}

	if ($_POST['discount'] == "") 
	{
		$_POST['discount'] = 0;
	}

	if (!check_num('discount', 0))
	{
		display_error(_("The entered discount is invalid or less than zero."));
		set_focus('amount');
		return false;
	}

	if (input_num('amount') - input_num('discount') <= 0) 
	{
		display_error(_("The total of the amount and the discount is zero or negative. Please enter positive values."));
		set_focus('amount');
		return false;
	}

   	if (!is_date($_POST['DatePaid']))
   	{
		display_error(_("The entered date is invalid."));
		set_focus('DatePaid');
		return false;
	} 
	elseif (!is_date_in_fiscalyear($_POST['DatePaid'])) 
	{
		display_error(_("The entered date is not in fiscal year."));
		set_focus('DatePaid');
		return false;
	}
    if (!references::is_valid($_POST['ref'])) 
    {
		display_error(_("You must enter a reference."));
		set_focus('ref');
		return false;
	}

	if (!is_new_reference($_POST['ref'], 22)) 
	{
		display_error(_("The entered reference is already in use."));
		set_focus('ref');
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------

function handle_add_payment()
{
	$supp_currency = get_supplier_currency($_POST['supplier_id']);
	$bank_currency = get_bank_account_currency($_POST['bank_account']);
	$comp_currency = get_company_currency();
	if ($comp_currency != $bank_currency && $bank_currency != $supp_currency)
		$rate = 0;
	else
		$rate = input_num('_ex_rate');

	$payment_id = add_supp_payment($_POST['supplier_id'], $_POST['DatePaid'],
		$_POST['bank_account'],	input_num('amount'), input_num('discount'), 
		$_POST['ref'], $_POST['memo_'], $rate);
	new_doc_date($_POST['DatePaid']);
	//unset($_POST['supplier_id']);
   	unset($_POST['bank_account']);
   	unset($_POST['DatePaid']);
   	unset($_POST['currency']);
   	unset($_POST['memo_']);
   	unset($_POST['amount']);
   	unset($_POST['discount']);
   	unset($_POST['ProcessSuppPayment']);

	meta_forward($_SERVER['PHP_SELF'], "AddedID=$payment_id&supplier_id=".$_POST['supplier_id']);
}

//----------------------------------------------------------------------------------------

if (isset($_POST['ProcessSuppPayment']))
{
	 /*First off  check for valid inputs */
    if (check_inputs() == true) 
    {
    	handle_add_payment();
    	end_page();
     	exit;
    }
}

display_controls();

end_page();
?>
