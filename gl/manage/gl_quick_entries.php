<?php

$page_security = 3;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("Quick Entries"));

include($path_to_root . "/gl/includes/gl_db.inc");

include($path_to_root . "/includes/ui.inc");

simple_page_mode(true);
//-----------------------------------------------------------------------------------

function can_process() 
{

	if (strlen($_POST['description']) == 0) 
	{
		display_error( _("The Quick Entry description cannot be empty."));
		set_focus('description');
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------------

if ($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM') 
{

	if (can_process()) 
	{

    	if ($selected_id != -1) 
    	{
    		update_quick_entry($selected_id, $_POST['description'], $_POST['account'], $_POST['deposit']);
			display_notification(_('Selected quick entry has been updated'));
    	} 
    	else 
    	{
    		add_quick_entry($_POST['description'], $_POST['account'], $_POST['deposit']);
			display_notification(_('New account class has been added'));
    	}
		$Mode = 'RESET';
	}
}

//-----------------------------------------------------------------------------------

if ($Mode == 'Delete')
{

	delete_quick_entry($selected_id);
	display_notification(_('Selected quick entry has been deleted'));
	$Mode = 'RESET';
}

//-----------------------------------------------------------------------------------
if ($Mode == 'RESET')
{
	$selected_id = -1;
	$_POST['description'] = $_POST['account'] = $_POST['deposit'] = '';
}
//-----------------------------------------------------------------------------------

$result = get_quick_entries();
start_form();
start_table($table_style);
$th = array(_("Description"), _("Account"), _("Deposit"), "", "");
table_header($th);

$k = 0;
while ($myrow = db_fetch($result)) 
{

	alt_table_row_color($k);

	if ($myrow["deposit"] == 0) 
	{
		$bs_text = _("No");
	} 
	else 
	{
		$bs_text = _("Yes");
	}
	label_cell($myrow['description']);
	label_cell($myrow['account']." ".$myrow['account_name']);
	label_cell($bs_text);
	edit_button_cell("Edit".$myrow["id"], _("Edit"));
	edit_button_cell("Delete".$myrow["id"], _("Delete"));
	end_row();
}

end_table();
end_form();
echo '<br>';
//-----------------------------------------------------------------------------------

start_form();

start_table($table_style2);

if ($selected_id != -1) 
{
 	if ($Mode == 'Edit') 
 	{
		//editing an existing status code
		$myrow = get_quick_entry($selected_id);

		$_POST['id']  = $myrow["id"];
		$_POST['description']  = $myrow["description"];
		$_POST['account']  = $myrow["account"];
		$_POST['deposit']  = $myrow["deposit"];
		hidden('selected_id', $selected_id);
 	}
} 

text_row_ex(_("Description:"), 'description', 50, 60);

gl_all_accounts_list_row(_("Account"), 'account', null, true);

yesno_list_row(_("Deposit:"), 'deposit', null, "", "", false);

end_table(1);

submit_add_or_update_center($selected_id == -1, '', true);

end_form();

//------------------------------------------------------------------------------------

end_page();

?>
