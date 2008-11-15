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

page(_("Journal Entry"), false, false,'', $js);

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
	$trans_type = systypes::journal_entry();

   	display_notification_centered( _("Journal entry has been entered") . " #$trans_no");

    display_note(get_gl_view_str($trans_type, $trans_no, _("&View this Journal Entry")));

   	hyperlink_no_params($_SERVER['PHP_SELF'], _("Enter &Another Journal Entry"));

	display_footer_exit();
}
//--------------------------------------------------------------------------------------------------

function handle_new_order()
{
	if (isset($_SESSION['journal_items']))
	{
		$_SESSION['journal_items']->clear_items();
		unset ($_SESSION['journal_items']);
	}

    session_register("journal_items");

    $_SESSION['journal_items'] = new items_cart(systypes::journal_entry());

	$_POST['date_'] = Today();
	if (!is_date_in_fiscalyear($_POST['date_']))
		$_POST['date_'] = end_fiscalyear();
	$_SESSION['journal_items']->tran_date = $_POST['date_'];	
}

//-----------------------------------------------------------------------------------------------

if (isset($_POST['Process']))
{

	$input_error = 0;

	if ($_SESSION['journal_items']->count_gl_items() < 1) {
		display_error(_("You must enter at least one journal line."));
		set_focus('code_id');
		$input_error = 1;
	}
	if (abs($_SESSION['journal_items']->gl_items_total()) > 0.0001)
	{
		display_error(_("The journal must balance (debits equal to credits) before it can be processed."));
		set_focus('code_id');
		$input_error = 1;
	}


	if (!is_date($_POST['date_'])) 
	{
		display_error(_("The entered date is invalid."));
		set_focus('date_');
		$input_error = 1;
	} 
	elseif (!is_date_in_fiscalyear($_POST['date_'])) 
	{
		display_error(_("The entered date is not in fiscal year."));
		set_focus('date_');
		$input_error = 1;
	} 
	if (!references::is_valid($_POST['ref'])) 
	{
		display_error( _("You must enter a reference."));
		set_focus('ref');
		$input_error = 1;
	} 
	elseif (references::exists(systypes::journal_entry(), $_POST['ref'])) 
	{
		display_error( _("The entered reference is already in use."));
		set_focus('ref');
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
		set_focus('dimension_id');
		return false;
	}

	if (isset($_POST['dimension2_id']) && $_POST['dimension2_id'] != 0 && dimension_is_closed($_POST['dimension2_id'])) 
	{
		display_error(_("Dimension is closed."));
		set_focus('dimension2_id');
		return false;
	}

	if (!(input_num('AmountDebit')!=0 ^ input_num('AmountCredit')!=0) )
	{
		display_error(_("You must enter either a debit amount or a credit amount."));
		set_focus('AmountDebit');
    		return false;
  	}

	if (strlen($_POST['AmountDebit']) && !check_num('AmountDebit', 0)) 
	{
    		display_error(_("The debit amount entered is not a valid number or is less than zero."));
		set_focus('AmountDebit');
    		return false;
  	} elseif (strlen($_POST['AmountCredit']) && !check_num('AmountCredit', 0))
	{
    		display_error(_("The credit amount entered is not a valid number or is less than zero."));
		set_focus('AmountCredit');
    		return false;
  	}


	if ($_SESSION["wa_current_user"]->access != 2 && is_bank_account($_POST['code_id'])) 
	{
		display_error(_("You cannot make a journal entry for a bank account. Please use one of the banking functions for bank transactions."));
		set_focus('code_id');
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
	line_start_focus();
}

//-----------------------------------------------------------------------------------------------

function handle_delete_item($id)
{
	$_SESSION['journal_items']->remove_gl_item($id);
	line_start_focus();
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
	line_start_focus();
}

function display_quick_entries(&$cart)
{
	if (!get_post('person_id'))
	{
		display_error( _("No Quick Entries are defined."));
		set_focus('totamount');
	}
	else
	{
		$rate = 0;
		$totamount = input_num('totamount');
		//$totamount = ($cart->trans_type==systypes::bank_payment() ? 1:-1) * $totamount;
		$qe = get_quick_entry($_POST['person_id']);
		$qe_lines = get_quick_entry_lines($_POST['person_id']);
		while ($qe_line = db_fetch($qe_lines))
		{
			if ($qe_line['tax_acc'])
			{
				$account = get_gl_account($qe_line['account']);
				$tax_group = $account['tax_code'];
				$items = get_tax_group_items($tax_group);
				while ($item = db_fetch($items))
					$rate += $item['rate'];
				if ($rate != 0)
					$totamount = $totamount * 100 / ($rate + 100);
				$cart->clear_items();

				$cart->add_gl_item($qe_line['account'], $qe_line['dimension_id'], $qe_line['dimension2_id'], 
					$totamount, $qe['description']);
				$items = get_tax_group_items($tax_group);
				while ($item = db_fetch($items))
				{
					if ($item['rate'] != 0)
					{
						$amount = $totamount * $item['rate'] / 100;
						$code = ($amount < 0 ? $item['purchasing_gl_code'] : 
							$item['sales_gl_code']);
						$cart->add_gl_item($code, 0, 0, $amount, $qe['description']);
					}
				}
			}
			else
			{
				if ($qe_line['pct'])
					$amount = $totamount * $qe_line['amount'] / 100;
				else
					$amount = $qe_line['amount'];
				$cart->add_gl_item($qe_line['account'], $qe_line['dimension_id'], $qe_line['dimension2_id'], 
					$amount, $qe['description']);
			}		
		}
		line_start_focus();
	}	
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
	display_quick_entries($_SESSION['journal_items']);
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

submit_center('Process', _("Process Journal Entry"), true , 
	_('Process journal entry only if debits equal to credits'), true);

end_form();
//------------------------------------------------------------------------------------------------

end_page();

?>
