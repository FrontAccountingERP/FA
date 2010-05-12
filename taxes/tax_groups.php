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
	/* Editable rate has been removed 090920 Joe Hunt
	else 
	{
		// make sure any entered rates are valid
    	for ($i = 0; $i < 5; $i++) 
    	{
    		if (isset($_POST['tax_type_id' . $i]) && 
    			$_POST['tax_type_id' . $i] != ALL_NUMERIC	&& 
    			!check_num('rate' . $i, 0))
    		{
			display_error( _("An entered tax rate is invalid or less than zero."));
    			$input_error = 1;
			set_focus('rate');
			break;
    		}
    	}
	}
	*/
	if ($input_error != 1) 
	{

		// create an array of the taxes and array of rates
    	$taxes = array();
    	$rates = array();

    	for ($i = 0; $i < 5; $i++) 
    	{
    		if (isset($_POST['tax_type_id' . $i]) &&
   				$_POST['tax_type_id' . $i] != ANY_NUMERIC) 
   			{
        		$taxes[] = $_POST['tax_type_id' . $i];
				$rates[] = get_tax_type_default_rate($_POST['tax_type_id' . $i]);
				//Editable rate has been removed 090920 Joe Hunt
        		//$rates[] = input_num('rate' . $i);
    		}
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
	$sql = "SELECT COUNT(*) FROM ".TB_PREF."cust_branch WHERE tax_group_id=".db_escape($selected_id);
	$result = db_query($sql, "could not query customers");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_note(_("Cannot delete this tax group because customer branches been created referring to it."));
		return false;
	}

	$sql = "SELECT COUNT(*) FROM ".TB_PREF."suppliers WHERE tax_group_id=".db_escape($selected_id);
	$result = db_query($sql, "could not query suppliers");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_note(_("Cannot delete this tax group because suppliers been created referring to it."));
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
	$_POST['show_inactive'] = $sav;
}
//-----------------------------------------------------------------------------------

$result = get_all_tax_groups(check_value('show_inactive'));

start_form();
start_table($table_style);
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

	/*for ($i=0; $i< 5; $i++)
		if ($myrow["type" . $i] != ALL_NUMERIC)
			echo "<td>" . $myrow["type" . $i] . "</td>";*/

	inactive_control_cell($myrow["id"], $myrow["inactive"], 'tax_groups', 'id');
 	edit_button_cell("Edit".$myrow["id"], _("Edit"));
 	delete_button_cell("Delete".$myrow["id"], _("Delete"));
	end_row();;
}

inactive_control_row($th);
end_table(1);

//-----------------------------------------------------------------------------------

start_table($table_style2);

if ($selected_id != -1) 
{
	//editing an existing status code

 	if ($Mode == 'Edit') {
    	$group = get_tax_group($selected_id);

    	$_POST['name']  = $group["name"];
    	$_POST['tax_shipping'] = $group["tax_shipping"];

    	$items = get_tax_group_items($selected_id);

    	$i = 0;
    	while ($tax_item = db_fetch($items)) 
    	{
    		$_POST['tax_type_id' . $i]  = $tax_item["tax_type_id"];
    		$_POST['rate' . $i]  = percent_format($tax_item["rate"]);
    		$i ++;
    	}
    	while($i<5) unset($_POST['tax_type_id'.$i++]);
	}

	hidden('selected_id', $selected_id);
}
text_row_ex(_("Description:"), 'name', 40);
yesno_list_row(_("Tax applied to Shipping:"), 'tax_shipping', null, "", "", true);

end_table();

display_note(_("Select the taxes that are included in this group."), 1);

start_table($table_style2);
//$th = array(_("Tax"), _("Default Rate (%)"), _("Rate (%)"));
//Editable rate has been removed 090920 Joe Hunt
$th = array(_("Tax"), _("Rate (%)"));
table_header($th);
for ($i = 0; $i < 5; $i++) 
{
	start_row();
	if (!isset($_POST['tax_type_id' . $i]))
		$_POST['tax_type_id' . $i] = 0;
	tax_types_list_cells(null, 'tax_type_id' . $i, $_POST['tax_type_id' . $i], _("None"), true);

	if ($_POST['tax_type_id' . $i] != 0 && $_POST['tax_type_id' . $i] != ALL_NUMERIC) 
	{
		$default_rate = get_tax_type_default_rate($_POST['tax_type_id' . $i]);
		label_cell(percent_format($default_rate), "nowrap align=right");

		//Editable rate has been removed 090920 Joe Hunt
		//if (!isset($_POST['rate' . $i]) || $_POST['rate' . $i] == "")
		//	$_POST['rate' . $i] = percent_format($default_rate);
		//small_amount_cells(null, 'rate' . $i, $_POST['rate' . $i], null, null, 
		//  user_percent_dec()); 
	}
	end_row();
}

end_table(1);

submit_add_or_update_center($selected_id == -1, '', 'both');

end_form();

//------------------------------------------------------------------------------------

end_page();

?>
