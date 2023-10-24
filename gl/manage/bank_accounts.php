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
$page_security = 'SA_BANKACCOUNT';
$path_to_root = "../..";
include($path_to_root . "/includes/db_pager.inc");
include($path_to_root . "/includes/session.inc");

$js = "";
if (user_use_date_picker())
	$js .= get_js_date_picker();

page(_($help_context = "Bank Accounts"), isset($_GET['bank_id']), false, "", $js);

include($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/ui/attachment.inc");

simple_page_mode();

if (isset($_GET['bank_id'])) 
{
	$_POST['bank_id'] = $_GET['bank_id'];
}

$bank_id = get_post('bank_id', ''); 
if ($selected_id != -1)
	$bank_id = $selected_id;

//-----------------------------------------------------------------------------------

if ($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM') 
{

	//initialise no input errors assumed initially before we test
	$input_error = 0;

	//first off validate inputs sensible
	if (strlen($_POST['bank_account_name']) == 0) 
	{
		$input_error = 1;
		display_error(_("The bank account name cannot be empty."));
		set_focus('bank_account_name');
	} 
	if ($Mode=='ADD_ITEM' && (gl_account_in_bank_accounts(get_post('account_code')) 
			|| key_in_foreign_table(get_post('account_code'), 'gl_trans', 'account'))) {
		$input_error = 1;
		display_error(_("The GL account selected is already in use or has transactions. Select another empty GL account."));
		set_focus('account_code');
	}
	if ($input_error != 1)
	{
    	if ($bank_id) 
    	{
    		
    		update_bank_account($bank_id, $_POST['account_code'],
				$_POST['account_type'], $_POST['bank_account_name'], 
				$_POST['bank_name'], $_POST['bank_account_number'], 
    			$_POST['bank_address'], $_POST['BankAccountCurrency'],
    			$_POST['dflt_curr_act'], $_POST['bank_charge_act']);
			$Ajax->activate('bank_id'); // in case of status change
			display_notification(_('Bank account has been updated'));
    	} 
    	else 
    	{
    
    		add_bank_account($_POST['account_code'], $_POST['account_type'], 
				$_POST['bank_account_name'], $_POST['bank_name'], 
    			$_POST['bank_account_number'], $_POST['bank_address'], 
				$_POST['BankAccountCurrency'], $_POST['dflt_curr_act'], $_POST['bank_charge_act']);
			$bank_id = $_POST['bank_id'] = db_insert_id();
			display_notification(_('New bank account has been added'));
  			$Ajax->activate('_page_body');
  		}
 		$Mode = 'RESET';
	}
} 
elseif( $Mode == 'Delete')
{
	//the link to delete a selected record was clicked instead of the submit button

	$cancel_delete = 0;
	// PREVENT DELETES IF DEPENDENT RECORDS IN 'bank_trans'

	if (key_in_foreign_table($bank_id, 'bank_trans', 'bank_act') || key_in_foreign_table(get_post('account_code'), 'gl_trans', 'account'))
	{
		$cancel_delete = 1;
		display_error(_("Cannot delete this bank account because transactions have been created using this account."));
	}

	if (key_in_foreign_table($bank_id, 'sales_pos', 'pos_account'))
	{
		$cancel_delete = 1;
		display_error(_("Cannot delete this bank account because POS definitions have been created using this account."));
	}
	if (!$cancel_delete) 
	{
		delete_bank_account($bank_id);
		display_notification(_('Selected bank account has been deleted'));
	} //end if Delete bank account
	$Mode = 'RESET';
} 

if ($Mode == 'RESET')
{
 	$bank_id = '';
	$_POST['bank_name']  = 	$_POST['bank_account_name']  = '';
	$_POST['bank_account_number'] = $_POST['bank_address'] = '';
	$_POST['bank_charge_act'] = get_company_pref('bank_charge_act');
	$Ajax->activate('_page_body');
}
if (!isset($_POST['bank_charge_act']))
	$_POST['bank_charge_act'] = get_company_pref('bank_charge_act');

/* Always show the list of accounts */

$result = get_bank_accounts(check_value('show_inactive'));

start_form(true);
start_table(TABLESTYLE, "width='80%'");

$th = array(_("Account Name"), _("Type"), _("Currency"), _("GL Account"), 
	_("Bank"), _("Number"), _("Bank Address"), _("Dflt"), '','');
inactive_control_column($th);
table_header($th);	

$k = 0; 
while ($myrow = db_fetch($result)) 
{
	
	alt_table_row_color($k);

    label_cell($myrow["bank_account_name"], "nowrap");
	label_cell($bank_account_types[$myrow["account_type"]], "nowrap");
    label_cell($myrow["bank_curr_code"], "nowrap");
    label_cell($myrow["account_code"] . " " . $myrow["account_name"], "nowrap");
    label_cell($myrow["bank_name"], "nowrap");
    label_cell($myrow["bank_account_number"], "nowrap");
    label_cell($myrow["bank_address"]);
    if ($myrow["dflt_curr_act"])
		label_cell(_("Yes"));
	else
		label_cell(_("No"));

	inactive_control_cell($myrow["id"], $myrow["inactive"], 'bank_accounts', 'id');
 	edit_button_cell("Edit".$myrow["id"], _("Edit"));
 	delete_button_cell("Delete".$myrow["id"], _("Delete"));
    end_row(); 
}

inactive_control_row($th);
end_table(1);


function bank_account_settings($bank_id)
{
	global $Mode, $bank_account_types, $page_nested;

	$is_used = $bank_id && key_in_foreign_table($bank_id, 'bank_trans', 'bank_act');
	
	start_table(TABLESTYLE2);

	if ($bank_id) 
	{
	  	if ($Mode == 'Edit') {	
			$myrow = get_bank_account($bank_id);

			$_POST['account_code'] = $myrow["account_code"];
			$_POST['account_type'] = $myrow["account_type"];
			$_POST['bank_name']  = $myrow["bank_name"];
			$_POST['bank_account_name']  = $myrow["bank_account_name"];
			$_POST['bank_account_number'] = $myrow["bank_account_number"];
			$_POST['bank_address'] = $myrow["bank_address"];
			$_POST['BankAccountCurrency'] = $myrow["bank_curr_code"];
			$_POST['dflt_curr_act'] = $myrow["dflt_curr_act"];
			$_POST['bank_charge_act'] = $myrow["bank_charge_act"];
	  	}
		hidden('bank_id', $bank_id);
		set_focus('bank_account_name');
	} 

	text_row(_("Bank Account Name:"), 'bank_account_name', null, 50, 100);

	if ($is_used) 
	{
		label_row(_("Account Type:"), $bank_account_types[$_POST['account_type']]);
		hidden('account_type');
	} 
	else 
	{
		bank_account_types_list_row(_("Account Type:"), 'account_type', null); 
	}
	if ($is_used) 
	{
		label_row(_("Bank Account Currency:"), $_POST['BankAccountCurrency']);
		hidden('BankAccountCurrency', $_POST['BankAccountCurrency']);
	} 
	else 
	{
		currencies_list_row(_("Bank Account Currency:"), 'BankAccountCurrency', null);
	}	

	yesno_list_row(_("Default currency account:"), 'dflt_curr_act');

	if($is_used)
	{
		label_row(_("Bank Account GL Code:"), $_POST['account_code']);
		hidden('account_code');
	} else 
		gl_all_accounts_list_row(_("Bank Account GL Code:"), 'account_code', null);

	gl_all_accounts_list_row(_("Bank Charges Account:"), 'bank_charge_act', null, true);
	text_row(_("Bank Name:"), 'bank_name', null, 50, 60);
	text_row(_("Bank Account Number:"), 'bank_account_number', null, 30, 60);
	textarea_row(_("Bank Address:"), 'bank_address', null, 40, 5);

	end_table(1);

	submit_add_or_update_center(!$bank_id, '', 'both');
}

if (!$bank_id)
{
	unset($_POST['_tabs_sel']); // force settings tab for new customer
	display_heading("");
}
else
{
	$act = get_bank_account($bank_id);
	if ($act)
		display_heading($act['bank_account_name']." - ".$act['bank_curr_code']);
}
if ($bank_id)
	hidden('bank_id', $bank_id);

tabbed_content_start('tabs', array(
		'settings' => array(_('&General settings'), $bank_id),
		'transactions' => array(_('&Transactions'), (user_check_access('SA_BANKTRANSVIEW') ? $bank_id : null)),
		'attachments' => array(_('Attachments'), (user_check_access('SA_ATTACHDOCUMENT') ? $bank_id : null)),
	));
	
	switch (get_post('_tabs_sel')) {
		default:
		case 'settings':
			$Mode = "Edit";
			bank_account_settings($bank_id); 
			break;
		case 'transactions':
			$_GET['bank_account'] = $bank_id;
			include_once($path_to_root."/gl/inquiry/bank_inquiry.php");
			break;
		case 'attachments':
			$_GET['trans_no'] = $bank_id;
			$_GET['type_no']= ST_BANKACCOUNT;
			$attachments = new attachments('attachment', $bank_id, 'bank_accounts');
			$attachments->show();
	};
br();
tabbed_content_end();

end_form();

end_page(@$_REQUEST['popup']);
