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
$page_security = 11;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("Inventory Locations"));

include_once($path_to_root . "/includes/ui.inc");

include_once($path_to_root . "/inventory/includes/inventory_db.inc");

simple_page_mode(true);

if ($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM') 
{

	//initialise no input errors assumed initially before we test
	$input_error = 0;

	/* actions to take once the user has clicked the submit button
	ie the page has called itself with some user input */

	//first off validate inputs sensible
	$_POST['loc_code'] = strtoupper($_POST['loc_code']);

	if (strlen($_POST['loc_code']) > 5) 
	{
		$input_error = 1;
		display_error( _("The location code must be five characters or less long."));
		set_focus('loc_code');
	} 
	elseif (strlen($_POST['location_name']) == 0) 
	{
		$input_error = 1;
		display_error( _("The location name must be entered."));		
		set_focus('location_name');
	}

	if ($input_error != 1) 
	{
    	if ($selected_id != -1) 
    	{
    
    		update_item_location($selected_id, $_POST['location_name'], $_POST['delivery_address'],
    			$_POST['phone'], $_POST['fax'], $_POST['email'], $_POST['contact']);	
			display_notification(_('Selected location has been updated'));
    	} 
    	else 
    	{
    
    	/*selected_id is null cos no item selected on first time round so must be adding a	record must be submitting new entries in the new Location form */
    	
    		add_item_location($_POST['loc_code'], $_POST['location_name'], $_POST['delivery_address'], 
    		 	$_POST['phone'], $_POST['fax'], $_POST['email'], $_POST['contact']);
			display_notification(_('New location has been added'));
    	}
		
		$Mode = 'RESET';
	}
} 

function can_delete($selected_id)
{
	$sql= "SELECT COUNT(*) FROM ".TB_PREF."stock_moves WHERE loc_code='$selected_id'";
	$result = db_query($sql, "could not query stock moves");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_("Cannot delete this location because item movements have been created using this location."));
		return false;
	}

	$sql= "SELECT COUNT(*) FROM ".TB_PREF."workorders WHERE loc_code='$selected_id'";
	$result = db_query($sql, "could not query work orders");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_("Cannot delete this location because it is used by some work orders records."));
		return false;
	}

	$sql= "SELECT COUNT(*) FROM ".TB_PREF."cust_branch WHERE default_location='$selected_id'";
	$result = db_query($sql, "could not query customer branches");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_("Cannot delete this location because it is used by some branch records as the default location to deliver from."));
		return false;
	}
	
	return true;
}

//----------------------------------------------------------------------------------

if ($Mode == 'Delete')
{

	if (can_delete($selected_id)) 
	{
		delete_item_location($selected_id);
		display_notification(_('Selected location has been deleted'));
	} //end if Delete Location
	$Mode = 'RESET';
}

if ($Mode == 'RESET')
{
	$selected_id = -1;
	unset($_POST);
}

$sql = "SELECT * FROM ".TB_PREF."locations";
$result = db_query($sql, "could not query locations");;

start_form();
start_table("$table_style width=30%");
$th = array(_("Location Code"), _("Location Name"), "", "");
table_header($th);
$k = 0; //row colour counter
while ($myrow = db_fetch_row($result)) 
{

	alt_table_row_color($k);
	
	label_cell($myrow[0]);
	label_cell($myrow[1]);
 	edit_button_cell("Edit".$myrow[0], _("Edit"));
 	delete_button_cell("Delete".$myrow[0], _("Delete"));
	end_row();
}
	//END WHILE LIST LOOP

//end of ifs and buts!

end_table();

end_form();
echo '<br>';

start_form();

start_table($table_style2);

$_POST['email'] = "";
if ($selected_id != -1) 
{
	//editing an existing Location

 	if ($Mode == 'Edit') {
		$myrow = get_item_location($selected_id);

		$_POST['loc_code'] = $myrow["loc_code"];
		$_POST['location_name']  = $myrow["location_name"];
		$_POST['delivery_address'] = $myrow["delivery_address"];
		$_POST['contact'] = $myrow["contact"];
		$_POST['phone'] = $myrow["phone"];
		$_POST['fax'] = $myrow["fax"];
		$_POST['email'] = $myrow["email"];
	}
	hidden("selected_id", $selected_id);
	hidden("loc_code");
	label_row(_("Location Code:"), $_POST['loc_code']);
} 
else 
{ //end of if $selected_id only do the else when a new record is being entered
	text_row(_("Location Code:"), 'loc_code', null, 5, 5);
}

text_row_ex(_("Location Name:"), 'location_name', 50, 50);
text_row_ex(_("Contact for deliveries:"), 'contact', 30, 30);

textarea_row(_("Address:"), 'delivery_address', null, 35, 5);	

text_row_ex(_("Telephone No:"), 'phone', 30, 30);
text_row_ex(_("Facsimile No:"), 'fax', 30, 30);
email_row_ex(_("E-mail:"), 'email', 30);

end_table(1);
submit_add_or_update_center($selected_id == -1, '', 'both');

end_form();

end_page();

?>
