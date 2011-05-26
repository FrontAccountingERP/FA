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
$page_security = 'SA_JOURNALENTRY';
$path_to_root = "..";
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

if (isset($_GET['ModifyGL'])) {
	$_SESSION['page_title'] = sprintf(_("Modifying Journal Transaction # %d."), 
		$_GET['trans_no']);
	$help_context = "Modifying Journal Entry";
} else
	$_SESSION['page_title'] = _($help_context = "Journal Entry");

page($_SESSION['page_title'], false, false,'', $js);
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
	$trans_type = ST_JOURNAL;

   	display_notification_centered( _("Journal entry has been entered") . " #$trans_no");

    display_note(get_gl_view_str($trans_type, $trans_no, _("&View this Journal Entry")));

	reset_focus();
	hyperlink_params($_SERVER['PHP_SELF'], _("Enter &New Journal Entry"), "NewJournal=Yes");

	hyperlink_params("$path_to_root/admin/attachments.php", _("Add an Attachment"), "filterType=$trans_type&trans_no=$trans_no");

	display_footer_exit();
} elseif (isset($_GET['UpdatedID'])) 
{
	$trans_no = $_GET['UpdatedID'];
	$trans_type = ST_JOURNAL;

   	display_notification_centered( _("Journal entry has been updated") . " #$trans_no");

    display_note(get_gl_view_str($trans_type, $trans_no, _("&View this Journal Entry")));

   	hyperlink_no_params($path_to_root."/gl/inquiry/journal_inquiry.php", _("Return to Journal &Inquiry"));

	display_footer_exit();
}
//--------------------------------------------------------------------------------------------------

if (isset($_GET['NewJournal']))
{
	create_cart(0,0);
} 
elseif (isset($_GET['ModifyGL']))
{
	if (!isset($_GET['trans_type']) || $_GET['trans_type']!= 0) {
		display_error(_("You can edit directly only journal entries created via Journal Entry page."));
		hyperlink_params("$path_to_root/gl/gl_journal.php", _("Entry &New Journal Entry"), "NewJournal=Yes");
		display_footer_exit();
	}
	create_cart($_GET['trans_type'], $_GET['trans_no']);
}

function create_cart($type=0, $trans_no=0)
{
	global $Refs;

	if (isset($_SESSION['journal_items']))
	{
		unset ($_SESSION['journal_items']);
	}

	$cart = new items_cart($type);
    $cart->order_id = $trans_no;

	if ($trans_no) {
		$result = get_gl_trans($type, $trans_no);

		if ($result) {
			while ($row = db_fetch($result)) {
				if ($row['amount'] == 0) continue;
				$date = $row['tran_date'];
				$cart->add_gl_item($row['account'], $row['dimension_id'], 
					$row['dimension2_id'], $row['amount'], $row['memo_']);
			}
		}
		$cart->memo_ = get_comments_string($type, $trans_no);
		$cart->tran_date = sql2date($date);
		$cart->reference = $Refs->get($type, $trans_no);
		$_POST['ref_original'] = $cart->reference; // Store for comparison when updating
	} else {
		$cart->reference = $Refs->get_next(0);
		$cart->tran_date = new_doc_date();
		if (!is_date_in_fiscalyear($cart->tran_date))
			$cart->tran_date = end_fiscalyear();
		$_POST['ref_original'] = -1;
	}

	$_POST['memo_'] = $cart->memo_;
	$_POST['ref'] = $cart->reference;
	$_POST['date_'] = $cart->tran_date;

	$_SESSION['journal_items'] = &$cart;
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
	if (!$Refs->is_valid($_POST['ref'])) 
	{
		display_error( _("You must enter a reference."));
		set_focus('ref');
		$input_error = 1;
	} 
	elseif ($Refs->exists(ST_JOURNAL, $_POST['ref'])) 
	{
	    // The reference can exist already so long as it's the same as the original (when modifying) 
	    if ($_POST['ref'] != $_POST['ref_original']) {
    		display_error( _("The entered reference is already in use."));
    		set_focus('ref');
    		$input_error = 1;
	    }
	}
	if ($input_error == 1)
		unset($_POST['Process']);
}

if (isset($_POST['Process']))
{
	$cart = &$_SESSION['journal_items'];
	$new = $cart->order_id == 0;

	$cart->reference = $_POST['ref'];
	$cart->memo_ = $_POST['memo_'];
	$cart->tran_date = $_POST['date_'];

	$trans_no = write_journal_entries($cart, check_value('Reverse'));

	$cart->clear_items();
	new_doc_date($_POST['date_']);
	unset($_SESSION['journal_items']);
	if($new)
		meta_forward($_SERVER['PHP_SELF'], "AddedID=$trans_no");
	else
		meta_forward($_SERVER['PHP_SELF'], "UpdatedID=$trans_no");
}

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
	
	if (!is_tax_gl_unique(get_post('code_id'))) {
   		display_error(_("Cannot post to GL account used by more than one tax type."));
		set_focus('code_id');
   		return false;
	}

	if (!$_SESSION["wa_current_user"]->can_access('SA_BANKJOURNAL') && is_bank_account($_POST['code_id'])) 
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

    	$_SESSION['journal_items']->update_gl_item($_POST['Index'], $_POST['code_id'], 
    	    $_POST['dimension_id'], $_POST['dimension2_id'], $amount, $_POST['LineMemo']);
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
	display_quick_entries($_SESSION['journal_items'], $_POST['person_id'], input_num('totamount'), QE_JOURNAL);
	$_POST['totamount'] = price_format(0); $Ajax->activate('totamount');
	line_start_focus();
}	
//-----------------------------------------------------------------------------------------------

start_form();

display_order_header($_SESSION['journal_items']);

start_table(TABLESTYLE2, "width=90%", 10);
start_row();
echo "<td>";
display_gl_items(_("Rows"), $_SESSION['journal_items']);
gl_options_controls();
echo "</td>";
end_row();
end_table(1);

submit_center('Process', _("Process Journal Entry"), true , 
	_('Process journal entry only if debits equal to credits'), 'default');

end_form();
//------------------------------------------------------------------------------------------------

end_page();

?>
