<?php

$path_to_root="..";
$page_security = 5;

include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/gl/includes/gl_db.inc");
include_once($path_to_root . "/gl/includes/gl_ui.inc");

$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(800, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();
page(_("Transfer between Bank Accounts"), false, false, "", $js);

check_db_has_bank_accounts(_("There are no bank accounts defined in the system."));
check_db_has_bank_trans_types(_("There are no bank transfer types defined in the system."));

//----------------------------------------------------------------------------------------

if (isset($_GET['AddedID'])) 
{
	$trans_no = $_GET['AddedID'];
	$trans_type = systypes::bank_transfer();

   	display_notification_centered( _("Transfer has been entered"));

	display_note(get_gl_view_str($trans_type, $trans_no, _("&View the GL Journal Entries for this Transfer")));

   	hyperlink_no_params($_SERVER['PHP_SELF'], _("Enter &Another Transfer"));

	safeExit();
}

if (isset($_POST['_DatePaid_changed'])) {
	$Ajax->activate('_ex_rate');
}

//----------------------------------------------------------------------------------------

function gl_payment_controls()
{
	global $table_style2;
	$home_currency = get_company_currency();

	start_form(false, true);

	start_table($table_style2, 5, 7);
	echo "<tr><td valign=top>"; // outer table

	echo "<table>";
	bank_accounts_list_row(_("From Account:"), 'FromBankAccount', null, true);

    bank_accounts_list_row(_("To Account:"), 'ToBankAccount', null, true);

    date_row(_("Transfer Date:"), 'DatePaid', '', null, 0, 0, 0, null, true);

	$from_currency = get_bank_account_currency($_POST['FromBankAccount']);
	$to_currency = get_bank_account_currency($_POST['ToBankAccount']);
	if ($from_currency != "" && $to_currency != "" && $from_currency != $to_currency) 
	{
		amount_row(_("Amount:"), 'amount', null, null, $from_currency);

		exchange_rate_display($from_currency, $to_currency, $_POST['DatePaid']);
	} 
	else 
	{
		amount_row(_("Amount:"), 'amount');
	}

	echo "</table>";
	echo "</td><td valign=top class='tableseparator'>"; // outer table
	echo "<table>";

	bank_trans_types_list_row(_("Transfer Type:"), 'TransferType', null);

    ref_row(_("Reference:"), 'ref', '', references::get_next(systypes::bank_transfer()));

    textarea_row(_("Memo:"), 'memo_', null, 40,4);

	end_table(1);

	echo "</td></tr>";
	end_table(1); // outer table

    submit_center('AddPayment',_("Enter Transfer"), true, '', true);

	end_form();
}

//----------------------------------------------------------------------------------------

function check_valid_entries()
{
	if (!is_date($_POST['DatePaid'])) 
	{
		display_error(_("The entered date is invalid."));
		set_focus('DatePaid');
		return false;
	}
	if (!is_date_in_fiscalyear($_POST['DatePaid']))
	{
		display_error(_("The entered date is not in fiscal year."));
		set_focus('DatePaid');
		return false;
	}

	if (!check_num('amount', 0)) 
	{
		display_error(_("The entered amount is invalid or less than zero."));
		set_focus('amount');
		return false;
	}

	if (!references::is_valid($_POST['ref'])) 
	{
		display_error(_("You must enter a reference."));
		set_focus('ref');
		return false;
	}

	if (!is_new_reference($_POST['ref'], systypes::bank_transfer())) 
	{
		display_error(_("The entered reference is already in use."));
		set_focus('ref');
		return false;
	}

	if ($_POST['FromBankAccount'] == $_POST['ToBankAccount']) 
	{
		display_error(_("The source and destination bank accouts cannot be the same."));
		set_focus('ToBankAccount');
		return false;
	}

    return true;
}

//----------------------------------------------------------------------------------------

function handle_add_deposit()
{
	global $path_to_root;

	$trans_no = add_bank_transfer($_POST['FromBankAccount'], $_POST['ToBankAccount'],
		$_POST['DatePaid'], input_num('amount'),
		$_POST['TransferType'], $_POST['ref'], $_POST['memo_']);

	meta_forward($_SERVER['PHP_SELF'], "AddedID=$trans_no");
}

//----------------------------------------------------------------------------------------

function safeExit()
{
	global $path_to_root;
	echo "<br><br>";
	end_page();
	exit;
}

//----------------------------------------------------------------------------------------

if (isset($_POST['AddPayment']))
{
	if (check_valid_entries() == true) 
	{
		handle_add_deposit();
		safeExit();
	}
}

gl_payment_controls();

end_page();
?>
