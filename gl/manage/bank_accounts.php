<?php

$page_security = 10;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("Bank Accounts"));

include($path_to_root . "/includes/ui.inc");
include($path_to_root . "/includes/data_checks.inc");

if (isset($_GET['selected_id'])) 
{
	$selected_id = $_GET['selected_id'];
} 
elseif (isset($_POST['selected_id'])) 
{
	$selected_id = $_POST['selected_id'];
}

if (isset($_POST['ADD_ITEM']) || isset($_POST['UPDATE_ITEM'])) 
{

	//initialise no input errors assumed initially before we test
	$input_error = 0;

	//first off validate inputs sensible
	if (strlen($_POST['bank_account_name']) == 0) 
	{
		$input_error = 1;
		display_error(_("The bank account name cannot be empty."));
	} 
	
	if ($input_error != 1)
	{
    	if (isset($selected_id)) 
    	{
    		
    		update_bank_account($selected_id, $_POST['account_type'], $_POST['bank_account_name'], $_POST['bank_name'], 
    			$_POST['bank_account_number'], 
    			$_POST['bank_address'], $_POST['BankAccountCurrency']);		
    	} 
    	else 
    	{
    
    		add_bank_account($_POST['account_code'], $_POST['account_type'], $_POST['bank_account_name'], $_POST['bank_name'], 
    			$_POST['bank_account_number'], 
    			$_POST['bank_address'], $_POST['BankAccountCurrency']);
    	}
		
		meta_forward($_SERVER['PHP_SELF']); 
	}

} 
elseif (isset($_GET['delete'])) 
{
	//the link to delete a selected record was clicked instead of the submit button

	$cancel_delete = 0;

	// PREVENT DELETES IF DEPENDENT RECORDS IN 'bank_trans'

	$sql= "SELECT COUNT(*) FROM ".TB_PREF."bank_trans WHERE bank_act='$selected_id'";
	$result = db_query($sql,"check failed");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		$cancel_delete = 1;
		display_error(_("Cannot delete this bank account because transactions have been created using this account."));
	}
	if (!$cancel_delete) 
	{
		delete_bank_account($selected_id);
		meta_forward($_SERVER['PHP_SELF']);
	} //end if Delete bank account
}

/* Always show the list of accounts */

$sql = "SELECT ".TB_PREF."bank_accounts.*, ".TB_PREF."chart_master.account_name FROM ".TB_PREF."bank_accounts, ".TB_PREF."chart_master 
	WHERE ".TB_PREF."bank_accounts.account_code = ".TB_PREF."chart_master.account_code";
$result = db_query($sql,"could not get bank accounts");

check_db_error("The bank accounts set up could not be retreived", $sql);

start_table("$table_style width='80%'");

$th = array(_("GL Account"), _("Bank"), _("Account Name"),
	_("Type"), _("Number"), _("Currency"), _("Bank Address"));
table_header($th);	

$k = 0; 
while ($myrow = db_fetch($result)) 
{
	
	alt_table_row_color($k);

    label_cell($myrow["account_code"] . " " . $myrow["account_name"], "nowrap");
    label_cell($myrow["bank_name"], "nowrap");
    label_cell($myrow["bank_account_name"], "nowrap");
	label_cell(bank_account_types::name($myrow["account_type"]), "nowrap");
    label_cell($myrow["bank_account_number"], "nowrap");
    label_cell($myrow["bank_curr_code"], "nowrap");
    label_cell($myrow["bank_address"]);
    edit_link_cell("selected_id=" . $myrow["account_code"]);
    delete_link_cell("selected_id=" . $myrow["account_code"]. "&delete=1");
    end_row(); 
}
//END WHILE LIST LOOP


end_table();

hyperlink_no_params($_SERVER['PHP_SELF'], _("New Bank Account"));

start_form();

$is_editing = (isset($selected_id) && !isset($_GET['delete'])); 

start_table($table_style2);

if ($is_editing) 
{
	
	$myrow = get_bank_account($selected_id);

	$_POST['account_code'] = $myrow["account_code"];
	$_POST['account_type'] = $myrow["account_type"];
	$_POST['bank_name']  = $myrow["bank_name"];
	$_POST['bank_account_name']  = $myrow["bank_account_name"];
	$_POST['bank_account_number'] = $myrow["bank_account_number"];
	$_POST['bank_address'] = $myrow["bank_address"];
	$_POST['BankAccountCurrency'] = $myrow["bank_curr_code"];

	hidden('selected_id', $selected_id);
	hidden('account_code', $_POST['account_code']);
	hidden('BankAccountCurrency', $_POST['BankAccountCurrency']);	
	label_row(_("Bank Account GL Code:"), $_POST['account_code']);
} 
else 
{
	gl_all_accounts_list_row(_("Bank Account GL Code:"), 'account_code', null, true);	
}

bank_account_types_list_row(_("Account Type:"), 'account_type', null); 

text_row(_("Bank Name:"), 'bank_name', null, 50, 50);
text_row(_("Bank Account Name:"), 'bank_account_name', null, 50, 50);
text_row(_("Bank Account Number:"), 'bank_account_number', null, 30, 30);

if ($is_editing) 
{
	label_row(_("Bank Account Currency:"), $_POST['BankAccountCurrency']);
} 
else 
{
	currencies_list_row(_("Bank Account Currency:"), 'BankAccountCurrency', null);
}	

textarea_row(_("Bank Address:"), 'bank_address', null, 40, 5);
//text_row(_("Bank Address:"), 'bank_address', null, 70, 70);

end_table(1);

submit_add_or_update_center(!isset($selected_id));

end_form();

end_page();
?>
