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
$page_security = 'SA_TAXGROUPS';
$path_to_root = "..";

include($path_to_root . "/includes/session.inc");

page(_($help_context = "Tax Groups"));

include_once($path_to_root . "/includes/data_checks.inc");
include_once($path_to_root . "/includes/ui.inc");

include_once($path_to_root . "/taxes/db/tax_groups_db.inc");
include_once($path_to_root . "/taxes/db/tax_types_db.inc");

simple_page_mode(true);
	
check_db_has_tax_types(_("There are no tax types defined. Define tax types before defining tax groups."));

//-----------------------------------------------------------------------------------

if ($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM') 
{

	//initialise no input errors assumed initially before we test
	$input_error = 0;

	if (strlen($_POST['name']) == 0) 
	{
		$input_error = 1;
		display_error(_("The tax group name cannot be empty."));
		set_focus('name');
	} 

	if ($input_error != 1) 
	{

		// create an array of the taxes and array of rates
    	$taxes = array();
    	$rates = array();

		while (($id = find_submit('tax_type_id'))!=-1)
		{
       		$taxes[] = $id;
			$rates[] = get_tax_type_default_rate($id);
			unset($_POST['tax_type_id' . $id]);
		}
    	if ($selected_id != -1) 
    	{
	   		update_tax_group($selected_id, $_POST['name'], $_POST['tax_shipping'], $taxes, 
    			$rates);
			display_notification(_('Selected tax group has been updated'));
    	} 
    	else 
    	{
	   		add_tax_group($_POST['name'], $_POST['tax_shipping'], $taxes, $rates);
			display_notification(_('New tax group has been added'));
    	}

		$Mode = 'RESET';
	}
}

//-----------------------------------------------------------------------------------

function can_delete($selected_id)
{
	if ($selected_id == -1)
		return false;
	if (key_in_foreign_table($selected_id, 'cust_branch', 'tax_group_id'))	
	{
		display_error(_("Cannot delete this tax group because customer branches been created referring to it."));
		return false;
	}

	if (key_in_foreign_table($selected_id, 'suppliers', 'tax_group_id'))
	{
		display_error(_("Cannot delete this tax group because suppliers been created referring to it."));
		return false;
	}


	return true;
}


//-----------------------------------------------------------------------------------

if ($Mode == 'Delete')
{

	if (can_delete($selected_id))
	{
		delete_tax_group($selected_id);
		display_notification(_('Selected tax group has been deleted'));
	}
	$Mode = 'RESET';
}

if ($Mode == 'RESET')
{
	$selected_id = -1;
	$sav = get_post('show_inactive');
	unset($_POST);
	if($sav)
		$_POST['show_inactive'] = $sav;
}
//-----------------------------------------------------------------------------------

$result = get_all_tax_groups(check_value('show_inactive'));

start_form();

start_table(TABLESTYLE);
$th = array(_("Description"), _("Shipping Tax"), "", "");
inactive_control_column($th);

table_header($th);

$k = 0;
while ($myrow = db_fetch($result)) 
{

	alt_table_row_color($k);

	label_cell($myrow["name"]);
	if ($myrow["tax_shipping"])
		label_cell(_("Yes"));
	else
		label_cell(_("No"));

	inactive_control_cell($myrow["id"], $myrow["inactive"], 'tax_groups', 'id');
 	edit_button_cell("Edit".$myrow["id"], _("Edit"));
 	delete_button_cell("Delete".$myrow["id"], _("Delete"));
	end_row();;
}

inactive_control_row($th);
end_table(1);

//-----------------------------------------------------------------------------------

start_table(TABLESTYLE2);

if ($selected_id != -1) 
{
	//editing an existing status code

 	if ($Mode == 'Edit') {
    	$group = get_tax_group($selected_id);

    	$_POST['name']  = $group["name"];
    	$_POST['tax_shipping'] = $group["tax_shipping"];

	}
	hidden('selected_id', $selected_id);
}
text_row_ex(_("Description:"), 'name', 40);
yesno_list_row(_("Tax applied to Shipping:"), 'tax_shipping', null, "", "", true);

end_table();

display_note(_("Select the taxes that are included in this group."), 1, 1);

// null means transport tax group, but for new we do not use real rates
$items = get_tax_group_rates($selected_id!=-1 ? $selected_id : null);

$th = array(_("Tax"), "");

start_table(TABLESTYLE2);
table_header($th);

while($item = db_fetch_assoc($items)) 
{
	check_row($item['tax_type_name'], 'tax_type_id' . $item['tax_type_id'], 
		$selected_id!=-1 && isset($item['rate']), "align='center'");
}

end_table(1);

submit_add_or_update_center($selected_id == -1, '', 'both');

end_form();

//------------------------------------------------------------------------------------

end_page();

?>
