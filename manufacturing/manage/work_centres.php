<?php

$page_security = 3;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("Work Centres"));

include($path_to_root . "/manufacturing/includes/manufacturing_db.inc");

include($path_to_root . "/includes/ui.inc");

if (isset($_GET['selected_id']))
{
	$selected_id = $_GET['selected_id'];
} 
elseif(isset($_POST['selected_id']))
{
	$selected_id = $_POST['selected_id'];
}

//-----------------------------------------------------------------------------------

if (isset($_POST['ADD_ITEM']) || isset($_POST['UPDATE_ITEM'])) 
{

	//initialise no input errors assumed initially before we test
	$input_error = 0;

	if (strlen($_POST['name']) == 0) 
	{
		$input_error = 1;
		display_error(_("The work centre name cannot be empty."));
		set_focus('name');
	}

	if ($input_error != 1) 
	{
		
    	if (isset($selected_id)) 
    	{
    		
    		update_work_centre($selected_id, $_POST['name'], $_POST['description']);
    
    	} 
    	else 
    	{
    
    		add_work_centre($_POST['name'], $_POST['description']);
    	}
		meta_forward($_SERVER['PHP_SELF']);    	
	}
} 

//-----------------------------------------------------------------------------------

function can_delete($selected_id)
{
	$sql= "SELECT COUNT(*) FROM ".TB_PREF."bom WHERE workcentre_added='$selected_id'";	
	$result = db_query($sql, "check can delete work centre");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_("Cannot delete this work centre because BOMs have been created referring to it."));
		return false;
	}
	
	$sql= "SELECT COUNT(*) FROM ".TB_PREF."wo_requirements WHERE workcentre='$selected_id'";
	$result = db_query($sql, "check can delete work centre");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_("Cannot delete this work centre because work order requirements have been created referring to it."));
		return false;
	}		
	
	return true;
}


//-----------------------------------------------------------------------------------

if (isset($_GET['delete'])) 
{

	if (can_delete($selected_id))
	{
		delete_work_centre($selected_id);
		meta_forward($_SERVER['PHP_SELF']);		
	}
}

//-----------------------------------------------------------------------------------

$result = get_all_work_centres();

start_table("$table_style width=50%");
$th = array(_("Name"), _("description"), "", "");
table_header($th);

$k = 0;
while ($myrow = db_fetch($result)) 
{
	
	alt_table_row_color($k);	

	label_cell($myrow["name"]);
	label_cell($myrow["description"]);
	edit_link_cell("selected_id=" . $myrow["id"]);
	delete_link_cell("selected_id=" . $myrow["id"]. "&delete=1");
	end_row();
}

end_table();

//-----------------------------------------------------------------------------------

hyperlink_no_params($_SERVER['PHP_SELF'], _("New Work Centre"));

start_form();

start_table($table_style2);

if (isset($selected_id)) 
{
	//editing an existing status code

	$myrow = get_work_centre($selected_id);

	$_POST['name']  = $myrow["name"];
	$_POST['description']  = $myrow["description"];

	hidden('selected_id', $selected_id);
} 

text_row_ex(_("Name:"), 'name', 40);
text_row_ex(_("Description:"), 'description', 50);

end_table(1);

submit_add_or_update_center(!isset($selected_id));

end_form();

//------------------------------------------------------------------------------------

end_page();

?>
