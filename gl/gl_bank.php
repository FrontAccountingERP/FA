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
$path_to_root = "..";
include_once($path_to_root . "/includes/ui/items_cart.inc");
include_once($path_to_root . "/includes/session.inc");
$page_security = isset($_GET['NewPayment']) || 
	@($_SESSION['pay_items']->trans_type==ST_BANKPAYMENT)
 ? 'SA_PAYMENT' : 'SA_DEPOSIT';

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/gl/includes/ui/gl_bank_ui.inc");
include_once($path_to_root . "/gl/includes/gl_db.inc");
include_once($path_to_root . "/gl/includes/gl_ui.inc");

$js = '';
if ($use_popup_windows)
	$js .= get_js_open_window(800, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();

if (isset($_GET['NewPayment'])) {
	$_SESSION['page_title'] = _($help_context = "Bank Account Payment Entry");
	handle_new_order(ST_BANKPAYMENT);

} else if(isset($_GET['NewDeposit'])) {
	$_SESSION['page_title'] = _($help_context = "Bank Account Deposit Entry");
	handle_new_order(ST_BANKDEPOSIT);
}
page($_SESSION['page_title'], false, false, '', $js);

//-----------------------------------------------------------------------------------------------
check_db_has_bank_accounts(_("There are no bank accounts defined in the system."));

//----------------------------------------------------------------------------------------
if (list_updated('PersonDetailID')) {
	$br = get_branch(get_post('PersonDetailID'));
	$_POST['person_id'] = $br['debtor_no'];
	$Ajax->activate('person_id');
}

//--------------------------------------------------------------------------------------------------
function line_start_focus() {
  global 	$Ajax;

  $Ajax->activate('items_table');
  set_focus('_code_id_edit');
}

//-----------------------------------------------------------------------------------------------

if (isset($_GET['AddedID']))
{
	$trans_no = $_GET['AddedID'];
	$trans_type = ST_BANKPAYMENT;

   	display_notification_centered(_("Payment $trans_no has been entered"));

	display_note(get_gl_view_str($trans_type, $trans_no, _("&View the GL Postings for this Payment")));

	hyperlink_params($_SERVER['PHP_SELF'], _("Enter Another &Payment"), "NewPayment=yes");

	hyperlink_params($_SERVER['PHP_SELF'], _("Enter A &Deposit"), "NewDeposit=yes");

	display_footer_exit();
}

if (isset($_GET['AddedDep']))
{
	$trans_no = $_GET['AddedDep'];
	$trans_type = ST_BANKDEPOSIT;

   	display_notification_centered(_("Deposit $trans_no has been entered"));

	display_note(get_gl_view_str($trans_type, $trans_no, _("View the GL Postings for this Deposit")));

	hyperlink_params($_SERVER['PHP_SELF'], _("Enter Another Deposit"), "NewDeposit=yes");

	hyperlink_params($_SERVER['PHP_SELF'], _("Enter A Payment"), "NewPayment=yes");

	display_footer_exit();
}
if (isset($_POST['_date__changed'])) {
	$Ajax->activate('_ex_rate');
}
//--------------------------------------------------------------------------------------------------

function handle_new_order($type)
{
	if (isset($_SESSION['pay_items']))
	{
		unset ($_SESSION['pay_items']);
	}

	//session_register("pay_items");

	$_SESSION['pay_items'] = new items_cart($type);

	$_POST['date_'] = new_doc_date();
	if (!is_date_in_fiscalyear($_POST['date_']))
		$_POST['date_'] = end_fiscalyear();
	$_SESSION['pay_items']->tran_date = $_POST['date_'];
}

//-----------------------------------------------------------------------------------------------

if (isset($_POST['Process']))
{

	$input_error = 0;

	if ($_SESSION['pay_items']->count_gl_items() < 1) {
		display_error(_("You must enter at least one payment line."));
		set_focus('code_id');
		$input_error = 1;
	}

	if ($_SESSION['pay_items']->gl_items_total() == 0.0) {
		display_error(_("The total bank amount cannot be 0."));
		set_focus('code_id');
		$input_error = 1;
	}

	if (!$Refs->is_valid($_POST['ref']))
	{
		display_error( _("You must enter a reference."));
		set_focus('ref');
		$input_error = 1;
	}
	elseif (!is_new_reference($_POST['ref'], $_SESSION['pay_items']->trans_type))
	{
		display_error( _("The entered reference is already in use."));
		set_focus('ref');
		$input_error = 1;
	}
	if (!is_date($_POST['date_']))
	{
		display_error(_("The entered date for the payment is invalid."));
		set_focus('date_');
		$input_error = 1;
	}
	elseif (!is_date_in_fiscalyear($_POST['date_']))
	{
		display_error(_("The entered date is not in fiscal year."));
		set_focus('date_');
		$input_error = 1;
	}

	if ($input_error == 1)
		unset($_POST['Process']);
}

if (isset($_POST['Process']))
{

	$trans = add_bank_transaction(
		$_SESSION['pay_items']->trans_type, $_POST['bank_account'],
		$_SESSION['pay_items'], $_POST['date_'],
		$_POST['PayType'], $_POST['person_id'], get_post('PersonDetailID'),
		$_POST['ref'], $_POST['memo_']);

	$trans_type = $trans[0];
   	$trans_no = $trans[1];
	new_doc_date($_POST['date_']);

	$_SESSION['pay_items']->clear_items();
	unset($_SESSION['pay_items']);

	meta_forward($_SERVER['PHP_SELF'], $trans_type==ST_BANKPAYMENT ?
		"AddedID=$trans_no" : "AddedDep=$trans_no");

} /*end of process credit note */

//-----------------------------------------------------------------------------------------------

function check_item_data()
{
	//if (!check_num('amount', 0))
	//{
	//	display_error( _("The amount entered is not a valid number or is less than zero."));
	//	set_focus('amount');
	//	return false;
	//}

	if ($_POST['code_id'] == $_POST['bank_account'])
	{
		display_error( _("The source and destination accouts cannot be the same."));
		set_focus('code_id');
		return false;
	}

	//if (is_bank_account($_POST['code_id']))
	//{
	//	if ($_SESSION['pay_items']->trans_type == ST_BANKPAYMENT)
	//		display_error( _("You cannot make a payment to a bank account. Please use the transfer funds facility for this."));
	//	else
 	//		display_error( _("You cannot make a deposit from a bank account. Please use the transfer funds facility for this."));
	//	set_focus('code_id');
	//	return false;
	//}

   	return true;
}

//-----------------------------------------------------------------------------------------------

function handle_update_item()
{
	$amount = ($_SESSION['pay_items']->trans_type==ST_BANKPAYMENT ? 1:-1) * input_num('amount');
    if($_POST['UpdateItem'] != "" && check_item_data())
    {
    	$_SESSION['pay_items']->update_gl_item($_POST['Index'], $_POST['code_id'], 
    	    $_POST['dimension_id'], $_POST['dimension2_id'], $amount , $_POST['LineMemo']);
    }
	line_start_focus();
}

//-----------------------------------------------------------------------------------------------

function handle_delete_item($id)
{
	$_SESSION['pay_items']->remove_gl_item($id);
	line_start_focus();
}

//-----------------------------------------------------------------------------------------------

function handle_new_item()
{
	if (!check_item_data())
		return;
	$amount = ($_SESSION['pay_items']->trans_type==ST_BANKPAYMENT ? 1:-1) * input_num('amount');

	$_SESSION['pay_items']->add_gl_item($_POST['code_id'], $_POST['dimension_id'],
		$_POST['dimension2_id'], $amount, $_POST['LineMemo']);
	line_start_focus();
}
//-----------------------------------------------------------------------------------------------
$id = find_submit('Delete');
if ($id != -1)
	handle_delete_item($id);

if (isset($_POST['AddItem']))
	handle_new_item();

if (isset($_POST['UpdateItem']))
	handle_update_item();

if (isset($_POST['CancelItemChanges']))
	line_start_focus();

if (isset($_POST['go']))
{
	display_quick_entries($_SESSION['pay_items'], $_POST['person_id'], input_num('totamount'), 
		$_SESSION['pay_items']->trans_type==ST_BANKPAYMENT ? QE_PAYMENT : QE_DEPOSIT);
	$_POST['totamount'] = price_format(0); $Ajax->activate('totamount');
	line_start_focus();
}
//-----------------------------------------------------------------------------------------------

start_form();

display_bank_header($_SESSION['pay_items']);

start_table("$table_style2 width=90%", 10);
start_row();
echo "<td>";
display_gl_items($_SESSION['pay_items']->trans_type==ST_BANKPAYMENT ?
	_("Payment Items"):_("Deposit Items"), $_SESSION['pay_items']);
gl_options_controls();
echo "</td>";
end_row();
end_table(1);

submit_center_first('Update', _("Update"), '', null);
submit_center_last('Process', $_SESSION['pay_items']->trans_type==ST_BANKPAYMENT ?
	_("Process Payment"):_("Process Deposit"), '', 'default');

end_form();

//------------------------------------------------------------------------------------------------

end_page();

?>
