<?php

$page_security = 3;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("Inventory Movement Types"));

include_once($path_to_root . "/inventory/includes/inventory_db.inc");

include_once($path_to_root . "/includes/ui.inc");

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
		display_error(_("The inventory movement type name cannot be empty."));
		set_focus('name');
	}

	if ($input_error != 1) 
	{
		
    	if (isset($selected_id)) 
    	{
    		
    		update_movement_type($selected_id, $_POST['name']);
    
    	} 
    	else 
    	{
    
    		add_movement_type($_POST['name']);
    	}
    	
		meta_forward($_SERVER['PHP_SELF']);     	
	}
} 

//-----------------------------------------------------------------------------------

function can_delete($selected_id)
{
	$sql= "SELECT COUNT(*) FROM ".TB_PREF."stock_moves 
		WHERE type=" . systypes::inventory_adjustment(). " AND person_id=$selected_id";
	$result = db_query($sql, "could not query stock moves");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_("Cannot delete this inventory movement type because item transactions have been created referring to it."));
		return false;
	}
	
	return true;
}


//-----------------------------------------------------------------------------------

if (isset($_GET['delete'])) 
{

	if (can_delete($selected_id))
	{
		delete_movement_type($selected_id);
		meta_forward($_SERVER['PHP_SELF']); 
	}
}

//-----------------------------------------------------------------------------------

$result = get_all_movement_type();

start_table("$table_style width=30%");

$th = array(_("Description"), "", "");
table_header($th);
$k = 0;
while ($myrow = db_fetch($result)) 
{
	
	alt_table_row_color($k);	

	label_cell($myrow["name"]);
	edit_link_cell("selected_id=" . $myrow["id"]);
	delete_link_cell("selected_id=" . $myrow["id"]. "&delete=1");
	end_row();
}

end_table();

//-----------------------------------------------------------------------------------

hyperlink_no_params($_SERVER['PHP_SELF'], _("New Inventory Movement Type"));

start_form();

start_table();

if (isset($selected_id)) 
{
	//editing an existing status code

	$myrow = get_movement_type($selected_id);

	$_POST['name']  = $myrow["name"];

	hidden('selected_id', $selected_id);
} 

text_row(_("Description:"), 'name', null, 50, 50);

end_table(1);

submit_add_or_update_center(!isset($selected_id));

end_form();

//------------------------------------------------------------------------------------

end_page();

?>
