<?php

$page_security = 3;
$path_to_root="..";
include_once($path_to_root . "/includes/ui/items_cart.inc");

include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/gl/includes/ui/gl_journal_ui.inc");
include_once($path_to_root . "/gl/includes/gl_db.inc");
include_once($path_to_root . "/gl/includes/gl_ui.inc");

$js = '';
if ($use_popup_windows)
	$js .= get_js_open_window(800, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();

set_focus('CodeID2');

page(_("Journal Entry"), false, false,'', $js);

//-----------------------------------------------------------------------------------------------

if (isset($_GET['AddedID'])) 
{
	$trans_no = $_GET['AddedID'];
	$trans_type = systypes::journal_entry();

   	display_notification_centered( _("Journal entry has been entered") . " #$trans_no");

    display_note(get_gl_view_str($trans_type, $trans_no, _("View this Journal Entry")));

   	hyperlink_no_params($_SERVER['PHP_SELF'], _("Enter Another Journal Entry"));

	display_footer_exit();
}

//--------------------------------------------------------------------------------------------------

function copy_to_je()
{
	$_SESSION['journal_items']->tran_date = $_POST['date_'];
	$_SESSION['journal_items']->transfer_type = check_value('Reverse');
	$_SESSION['journal_items']->memo_ = $_POST['memo_'];
}

//--------------------------------------------------------------------------------------------------

function copy_from_je()
{
	$_POST['date_'] = $_SESSION['journal_items']->tran_date;
	$_POST['Reverse'] = $_SESSION['journal_items']->transfer_type;
	$_POST['memo_'] = $_SESSION['journal_items']->memo_;
}

//----------------------------------------------------------------------------------------

function handle_new_order()
{
	if (isset($_SESSION['journal_items']))
	{
		$_SESSION['journal_items']->clear_items();
		unset ($_SESSION['journal_items']);
	}

    session_register("journal_items");

    $_SESSION['journal_items'] = new items_cart;

	$_POST['date_'] = Today();
	if (!is_date_in_fiscalyear($_POST['date_']))
		$_POST['date_'] = end_fiscalyear();
	$_SESSION['journal_items']->tran_date = $_POST['date_'];	
}

//-----------------------------------------------------------------------------------------------

if (isset($_POST['Process']))
{

	$input_error = 0;

	if (!is_date($_POST['date_'])) 
	{
		display_error(_("The entered date is invalid."));
		$input_error = 1;
	} 
	elseif (!is_date_in_fiscalyear($_POST['date_'])) 
	{
		display_error(_("The entered date is not in fiscal year."));
		$input_error = 1;
	} 
	elseif (!references::is_valid($_POST['ref'])) 
	{
		display_error( _("You must enter a reference."));
		$input_error = 1;
	} 
	elseif (references::exists(systypes::journal_entry(), $_POST['ref'])) 
	{
		display_error( _("The entered reference is already in use."));
		$input_error = 1;
	}

	if ($input_error == 1)
		unset($_POST['Process']);
}

if (isset($_POST['Process']))
{

	$trans_no = add_journal_entries($_SESSION['journal_items']->gl_items,
		$_POST['date_'], $_POST['ref'], check_value('Reverse'), $_POST['memo_']);

	$_SESSION['journal_items']->clear_items();
	unset($_SESSION['journal_items']);

	meta_forward($_SERVER['PHP_SELF'], "AddedID=$trans_no");
} /*end of process credit note */

//-----------------------------------------------------------------------------------------------

function check_item_data()
{
	if (isset($_POST['dimension_id']) && $_POST['dimension_id'] != 0 && dimension_is_closed($_POST['dimension_id'])) 
	{
		display_error(_("Dimension is closed."));
			return false;
	}

	if (isset($_POST['dimension2_id']) && $_POST['dimension2_id'] != 0 && dimension_is_closed($_POST['dimension2_id'])) 
	{
		display_error(_("Dimension is closed."));
			return false;
	}

//	if (!(!strlen($_POST['AmountDebit']) ^ !strlen($_POST['AmountCredit'])))
	if (input_num('AmountDebit')!=0 && input_num('AmountCredit')!=0)
	{
		display_error(_("You must enter either a debit amount or a credit amount."));
    		return false;
  	}

	if (strlen($_POST['AmountDebit']) && !check_num('AmountDebit', 0)) 
	{
    		display_error(_("The debit amount entered is not a valid number or is less than zero."));
    		return false;
  	} elseif (strlen($_POST['AmountCredit']) && !check_num('AmountCredit', 0))
	{
    		display_error(_("The credit amount entered is not a valid number or is less than zero."));
    		return false;
  	}


	if ($_SESSION["wa_current_user"]->access != 2 && is_bank_account($_POST['code_id'])) 
	{
		display_error(_("You cannot make a journal entry for a bank account. Please use one of the banking functions for bank transactions."));
		return false;
	}

   	return true;
}

//-----------------------------------------------------------------------------------------------

function handle_update_item()
{
    if($_POST['UpdateItem'] != "" && check_item_data())
    {
    	if (input_num('AmountDebit') > 0)
    		$amount = input_num('AmountDebit');
    	else
    		$amount = -input_num('AmountCredit');

    	$_SESSION['journal_items']->update_gl_item($_POST['Index'], $_POST['dimension_id'],
    		$_POST['dimension2_id'], $amount, $_POST['LineMemo']);
    }
}

//-----------------------------------------------------------------------------------------------

function handle_delete_item()
{
	$_SESSION['journal_items']->remove_gl_item($_GET['Delete']);
}

//-----------------------------------------------------------------------------------------------

function handle_new_item()
{
	if (!check_item_data())
		return;

	if (input_num('AmountDebit') > 0)
		$amount = input_num('AmountDebit');
	else
		$amount = -input_num('AmountCredit');
	
	$_SESSION['journal_items']->add_gl_item($_POST['code_id'], $_POST['dimension_id'],
		$_POST['dimension2_id'], $amount, $_POST['LineMemo']);
}

//-----------------------------------------------------------------------------------------------

if (isset($_GET['Delete']) || isset($_GET['Edit']))
	copy_from_je();

if (isset($_GET['Delete']))
	handle_delete_item();

if (isset($_POST['AddItem']) || isset($_POST['UpdateItem']))
	copy_to_je();

if (isset($_POST['AddItem']))
	handle_new_item();

if (isset($_POST['UpdateItem']))
	handle_update_item();

//-----------------------------------------------------------------------------------------------

if (isset($_GET['NewJournal']) || !isset($_SESSION['journal_items']))
{
	handle_new_order();
}

//-----------------------------------------------------------------------------------------------

start_form();

display_order_header($_SESSION['journal_items']);

start_table("$table_style2 width=90%", 10);
start_row();
echo "<td>";
display_gl_items(_("Rows"), $_SESSION['journal_items']);
gl_options_controls();
echo "</td>";
end_row();
end_table(1);

if ($_SESSION['journal_items']->count_gl_items() >= 1 &&
	abs($_SESSION['journal_items']->gl_items_total()) < 0.0001)
{
    submit_center('Process', _("Process Journal Entry"));
} 
else 
{
	display_note(_("The journal must balance (debits equal to credits) before it can be processed."), 0, 1);
}

end_form();

//------------------------------------------------------------------------------------------------

end_page();

?>
