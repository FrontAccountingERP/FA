<?php

$page_security = 11;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("Inventory Locations"));

include_once($path_to_root . "/includes/ui.inc");

include_once($path_to_root . "/inventory/includes/inventory_db.inc");

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
    	if (isset($selected_id)) 
    	{
    
    		update_item_location($selected_id, $_POST['location_name'], $_POST['delivery_address'],
    			$_POST['phone'], $_POST['fax'], $_POST['email'], $_POST['contact']);	
    	} 
    	else 
    	{
    
    	/*selected_id is null cos no item selected on first time round so must be adding a	record must be submitting new entries in the new Location form */
    	
    		add_item_location($_POST['loc_code'], $_POST['location_name'], $_POST['delivery_address'], 
    		 	$_POST['phone'], $_POST['fax'], $_POST['email'], $_POST['contact']);
    	}
		
		meta_forward($_SERVER['PHP_SELF']);   	
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

if (isset($_GET['delete'])) 
{

	if (can_delete($selected_id)) 
	{
		delete_item_location($selected_id);
		meta_forward($_SERVER['PHP_SELF']); 		
	} //end if Delete Location
}

/* It could still be the second time the page has been run and a record has been selected for modification - selected_id will exist because it was sent with the new call. If its the first time the page has been displayed with no parameters
then none of the above are true and the list of locations will be displayed with
links to delete or edit each. These will call the same page again and allow update/input
or deletion of the records*/

$sql = "SELECT * FROM ".TB_PREF."locations";
$result = db_query($sql, "could not query locations");;

start_table("$table_style width=30%");
$th = array(_("Location Code"), _("Location Name"), "", "");
table_header($th);
$k = 0; //row colour counter
while ($myrow = db_fetch_row($result)) 
{

	alt_table_row_color($k);
	
	label_cell($myrow[0]);
	label_cell($myrow[1]);
	edit_link_cell("selected_id=$myrow[0]");
	delete_link_cell("selected_id=$myrow[0]&delete=1");
	end_row();
}
	//END WHILE LIST LOOP

//end of ifs and buts!

end_table();

hyperlink_no_params($_SERVER['PHP_SELF'], _("New Location"));

start_form();

start_table($table_style2);
if (isset($selected_id)) 
{
	//editing an existing Location

	$myrow = get_item_location($selected_id);

	$_POST['loc_code'] = $myrow["loc_code"];
	$_POST['location_name']  = $myrow["location_name"];
	$_POST['delivery_address'] = $myrow["delivery_address"];
	$_POST['contact'] = $myrow["contact"];
	$_POST['phone'] = $myrow["phone"];
	$_POST['fax'] = $myrow["fax"];
	$_POST['email'] = $myrow["email"];

	hidden("selected_id", $selected_id);
	hidden("loc_code", $_POST['loc_code']);
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
text_row_ex(_("Email:"), 'email', 30, 30);

end_table(1);
submit_add_or_update_center(!isset($selected_id));

end_form();

//end if record deleted no point displaying form to add record 
 
 end_page();

?>
