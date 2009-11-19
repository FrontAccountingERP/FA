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
$page_security = 'SA_SALESGROUP';
$path_to_root = "../..";
include($path_to_root . "/includes/session.inc");

page(_($help_context = "Sales Groups"));

include($path_to_root . "/includes/ui.inc");

simple_page_mode(true);

if ($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM') 
{

	$input_error = 0;

	if (strlen($_POST['description']) == 0) 
	{
		$input_error = 1;
		display_error(_("The area description cannot be empty."));
		set_focus('description');
	}

	if ($input_error != 1)
	{
    	if ($selected_id != -1) 
    	{
    		$sql = "UPDATE ".TB_PREF."groups SET description=".db_escape($_POST['description'])." WHERE id = ".db_escape($selected_id);
			$note = _('Selected sales group has been updated');
    	} 
    	else 
    	{
    		$sql = "INSERT INTO ".TB_PREF."groups (description) VALUES (".db_escape($_POST['description']) . ")";
			$note = _('New sales group has been added');
    	}
    
    	db_query($sql,"The sales group could not be updated or added");
		display_notification($note);    	
		$Mode = 'RESET';
	}
} 

if ($Mode == 'Delete')
{

	$cancel_delete = 0;

	// PREVENT DELETES IF DEPENDENT RECORDS IN 'debtors_master'

	$sql= "SELECT COUNT(*) FROM ".TB_PREF."cust_branch WHERE group_no=".db_escape($selected_id);
	$result = db_query($sql,"check failed");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		$cancel_delete = 1;
		display_error(_("Cannot delete this group because customers have been created using this group."));
	} 
	if ($cancel_delete == 0) 
	{
		$sql="DELETE FROM ".TB_PREF."groups WHERE id=".db_escape($selected_id);
		db_query($sql,"could not delete sales group");

		display_notification(_('Selected sales group has been deleted'));
	} //end if Delete area
	$Mode = 'RESET';
} 

if ($Mode == 'RESET')
{
	$selected_id = -1;
	$sav = get_post('show_inactive');
	unset($_POST);
	if ($sav) $_POST['show_inactive'] = 1;
}
//-------------------------------------------------------------------------------------------------

$sql = "SELECT * FROM ".TB_PREF."groups";
if (!check_value('show_inactive')) $sql .= " WHERE !inactive";
$sql .= " ORDER BY description";
$result = db_query($sql,"could not get groups");

start_form();
start_table("$table_style width=30%");
$th = array(_("Group Name"), "", "");
inactive_control_column($th);

table_header($th);
$k = 0; 

while ($myrow = db_fetch($result)) 
{
	
	alt_table_row_color($k);
		
	label_cell($myrow["description"]);
	inactive_control_cell($myrow["id"], $myrow["inactive"], 'groups', 'id');
 	edit_button_cell("Edit".$myrow["id"], _("Edit"));
 	delete_button_cell("Delete".$myrow["id"], _("Delete"));
	end_row();
}

inactive_control_row($th);
end_table();

echo '<br>';

//-------------------------------------------------------------------------------------------------

start_table($table_style2);

if ($selected_id != -1) 
{
 	if ($Mode == 'Edit') {
		//editing an existing area
		$sql = "SELECT * FROM ".TB_PREF."groups WHERE id=".db_escape($selected_id);

		$result = db_query($sql,"could not get group");
		$myrow = db_fetch($result);

		$_POST['description']  = $myrow["description"];
	}
	hidden("selected_id", $selected_id);
} 

text_row_ex(_("Group Name:"), 'description', 30); 

end_table(1);

submit_add_or_update_center($selected_id == -1, '', 'both');

end_form();

end_page();
?>
