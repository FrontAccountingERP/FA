<?php

$page_security = 3;
$path_to_root="../..";

include($path_to_root . "/includes/session.inc");

page(_("Bank Transaction Types"));

include($path_to_root . "/gl/includes/gl_db.inc");

include($path_to_root . "/includes/ui.inc");

simple_page_mode();
//-----------------------------------------------------------------------------------

if ($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM') 
{

	//initialise no input errors assumed initially before we test
	$input_error = 0;

	if (strlen($_POST['name']) == 0) 
	{
		$input_error = 1;
		display_error( _("The bank transaction type name cannot be empty."));
		set_focus('name');
	}

	if ($input_error != 1) 
	{
		
    	if ($selected_id != -1) 
    	{
    		update_bank_trans_type($selected_id, $_POST['name']);
			display_notification('Selected bank account settings has been updated');
    	} 
    	else 
    	{
       		add_bank_trans_type($_POST['name']);
			display_notification('New bank account has been added');
    	}
 		$Mode = 'RESET';
	}
} 

//-----------------------------------------------------------------------------------

function can_delete($selected_id)
{
	if ($selected_id == -1)
		return false;
	$sql= "SELECT COUNT(*) FROM ".TB_PREF."bank_trans WHERE bank_trans_type_id=$selected_id";
	$result = db_query($sql, "could not query bank transactions");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_("Cannot delete this bank transaction type because bank transactions have been created referring to it."));
		return false;
	}
	
	return true;
}


//-----------------------------------------------------------------------------------

if( $Mode == 'Delete')
{
	if (can_delete($selected_id))
	{
		delete_bank_trans_type($selected_id);
		display_notification('Selected bank account has been deleted');
	}
	$Mode = 'RESET';
}

if ($Mode == 'RESET')
{
	$selected_id = -1;
	$_POST['name']  = '';
}
//-----------------------------------------------------------------------------------

$result = get_all_bank_trans_type();

start_form();
start_table($table_style);

$th = array(_("Description"), "", "");
table_header($th);
$k = 0;
while ($myrow = db_fetch($result)) 
{
	
	alt_table_row_color($k);	

	label_cell($myrow["name"]);

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
	//editing an existing status code
	if ($Mode == 'Edit') {
		$myrow = get_bank_trans_type($selected_id);
		$_POST['name']  = $myrow["name"];
	}
	hidden('selected_id', $selected_id);
} 

set_focus('name');
text_row_ex(_("Description:"), 'name', 40);

end_table(1);

submit_add_or_update_center($selected_id == -1, '', true);

end_form();

//------------------------------------------------------------------------------------

end_page();

?>
