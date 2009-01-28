<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU Affero General Public License,
	AGPL, as published by the Free Software Foundation, either version 
	3 of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/agpl-3.0.html>.
***********************************************************************/
$page_security = 3;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("GL Account Classes"));

include($path_to_root . "/gl/includes/gl_db.inc");

include($path_to_root . "/includes/ui.inc");

simple_page_mode(true);
//-----------------------------------------------------------------------------------

function can_process() 
{

	if (strlen($_POST['name']) == 0) 
	{
		display_error( _("The account class name cannot be empty."));
		set_focus('name');
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
    		update_account_class($selected_id, $_POST['name'], $_POST['Balance']);
			display_notification(_('Selected account class settings has been updated'));
    	} 
    	else 
    	{
    		add_account_class($_POST['id'], $_POST['name'], $_POST['Balance']);
			display_notification(_('New account class has been added'));
    	}
		$Mode = 'RESET';
	}
}

//-----------------------------------------------------------------------------------

function can_delete($selected_id)
{
	if ($selected_id == -1)
		return false;
	$sql= "SELECT COUNT(*) FROM ".TB_PREF."chart_types
		WHERE class_id=$selected_id";
	$result = db_query($sql, "could not query chart master");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_("Cannot delete this account class because GL account types have been created referring to it."));
		return false;
	}

	return true;
}


//-----------------------------------------------------------------------------------

if ($Mode == 'Delete')
{

	if (can_delete($selected_id))
	{
		delete_account_class($selected_id);
		display_notification(_('Selected account class has been deleted'));
	}
	$Mode = 'RESET';
}

//-----------------------------------------------------------------------------------
if ($Mode == 'RESET')
{
	$selected_id = -1;
	$_POST['id']  = $_POST['name']  = $_POST['Balance'] = '';
}
//-----------------------------------------------------------------------------------

$result = get_account_classes();
start_form();
start_table($table_style);
$th = array(_("Class ID"), _("Class Name"), _("Balance Sheet"), "", "");
table_header($th);

$k = 0;
while ($myrow = db_fetch($result)) 
{

	alt_table_row_color($k);

	if ($myrow["balance_sheet"] == 0) 
	{
		$bs_text = _("No");
	} 
	else 
	{
		$bs_text = _("Yes");
	}
	label_cell($myrow["cid"]);
	label_cell($myrow['class_name']);
	label_cell($bs_text);
	edit_button_cell("Edit".$myrow["cid"], _("Edit"));
	delete_button_cell("Delete".$myrow["cid"], _("Delete"));
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
 if ($Mode == 'Edit') {
	//editing an existing status code
	$myrow = get_account_class($selected_id);

	$_POST['id']  = $myrow["cid"];
	$_POST['name']  = $myrow["class_name"];
	$_POST['Balance']  = $myrow["balance_sheet"];
	hidden('selected_id', $selected_id);
 }
	hidden('id');
	label_row(_("Class ID:"), $_POST['id']);

} 
else 
{

	text_row_ex(_("Class ID:"), 'id', 3);
}

text_row_ex(_("Class Name:"), 'name', 50, 60);

yesno_list_row(_("Balance Sheet:"), 'Balance', null, "", "", false);

end_table(1);

submit_add_or_update_center($selected_id == -1, '', true);

end_form();

//------------------------------------------------------------------------------------

end_page();

?>
