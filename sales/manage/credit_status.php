<?php

$page_security = 3;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("Credit Status")); 

include($path_to_root . "/sales/includes/db/credit_status_db.inc");

include($path_to_root . "/includes/ui.inc");

simple_page_mode(true);
//-----------------------------------------------------------------------------------

function can_process() 
{
	
	if (strlen($_POST['reason_description']) == 0) 
	{
		display_error(_("The credit status description cannot be empty."));
		return false;
	}	
	
	return true;
}

//-----------------------------------------------------------------------------------

if ($Mode=='ADD_ITEM' && can_process()) 
{

	add_credit_status($_POST['reason_description'], $_POST['DisallowInvoices']);
	display_notification(_('New credit status has been added'));
	$Mode = 'RESET';
} 

//-----------------------------------------------------------------------------------

if ($Mode=='UPDATE_ITEM' && can_process()) 
{
	display_notification(_('Selected credit status has been updated'));
	update_credit_status($selected_id, $_POST['reason_description'], $_POST['DisallowInvoices']);
	$Mode = 'RESET';
}

//-----------------------------------------------------------------------------------

function can_delete($selected_id)
{
	$sql= "SELECT COUNT(*) FROM ".TB_PREF."debtors_master 
		WHERE credit_status=$selected_id";
	$result = db_query($sql, "could not query customers");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_("Cannot delete this credit status because customer accounts have been created referring to it."));
		return false;
	}
	
	return true;
}


//-----------------------------------------------------------------------------------

if ($Mode == 'Delete')
{

	if (can_delete($selected_id))
	{
		delete_credit_status($selected_id);
		display_notification(_('Selected credit status has been deleted'));
	}
	$Mode = 'RESET';
}

if ($Mode == 'RESET')
{
	$selected_id = -1;
	unset($_POST);
}
//-----------------------------------------------------------------------------------

$result = get_all_credit_status();

start_form();
start_table("$table_style width=40%");
$th = array(_("Description"), _("Dissallow Invoices"),'','');
table_header($th);

$k = 0;
while ($myrow = db_fetch($result)) 
{
	
	alt_table_row_color($k);	

	if ($myrow["dissallow_invoices"] == 0) 
	{
		$disallow_text = _("Invoice OK");
	} 
	else 
	{
		$disallow_text = "<b>" . _("NO INVOICING") . "</b>";
	}
	
	label_cell($myrow["reason_description"]);
	label_cell($disallow_text);
 	edit_button_cell("Edit".$myrow['id'], _("Edit"));
 	edit_button_cell("Delete".$myrow['id'], _("Delete"));
	end_row();
}

end_table();
end_form();
echo '<br>';

//-----------------------------------------------------------------------------------

start_form();

start_table("$table_style2 width=40%");

if ($selected_id != -1) 
{
 	if ($Mode == 'Edit') {
		//editing an existing status code

		$myrow = get_credit_status($selected_id);

		$_POST['reason_description']  = $myrow["reason_description"];
		$_POST['DisallowInvoices']  = $myrow["dissallow_invoices"];
	}
	hidden('selected_id', $selected_id);
} 

text_row_ex(_("Description:"), 'reason_description', 50);

yesno_list_row(_("Dissallow invoicing ?"), 'DisallowInvoices', null); 

end_table(1);

submit_add_or_update_center($selected_id == -1, '', true);

end_form();

//------------------------------------------------------------------------------------

end_page();

?>
