<?php

$page_security = 11;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("Item Categories"));

include_once($path_to_root . "/includes/ui.inc");

include_once($path_to_root . "/inventory/includes/inventory_db.inc");

if (isset($_GET['selected_id']))
{
	$selected_id = strtoupper($_GET['selected_id']);
} 
else if (isset($_POST['selected_id']))
{
	$selected_id = strtoupper($_POST['selected_id']);
}

//----------------------------------------------------------------------------------

if (isset($_POST['ADD_ITEM']) || isset($_POST['UPDATE_ITEM'])) 
{

	//initialise no input errors assumed initially before we test
	$input_error = 0;

	if (strlen($_POST['description']) == 0) 
	{
		$input_error = 1;
		display_error(_("The item category description cannot be empty."));
		set_focus('description');
	}

	if ($input_error !=1)
	{
    	if (isset($selected_id)) 
    	{
		    update_item_category($selected_id, $_POST['description']);    		
    	} 
    	else 
    	{
		    add_item_category($_POST['description']);
    	}
		meta_forward($_SERVER['PHP_SELF']); 
	}
}

//---------------------------------------------------------------------------------- 

if (isset($_GET['delete'])) 
{

	// PREVENT DELETES IF DEPENDENT RECORDS IN 'stock_master'
	$sql= "SELECT COUNT(*) FROM ".TB_PREF."stock_master WHERE category_id='$selected_id'";
	$result = db_query($sql, "could not query stock master");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_("Cannot delete this item category because items have been created using this item category."));

	} 
	else 
	{
		delete_item_category($selected_id);
		meta_forward($_SERVER['PHP_SELF']); 		
	}
}

//----------------------------------------------------------------------------------

$sql = "SELECT * FROM ".TB_PREF."stock_category";
$result = db_query($sql, "could not get stock categories");

start_table("$table_style width=30%");
$th = array(_("Name"), "", "");
table_header($th);
$k = 0; //row colour counter

while ($myrow = db_fetch($result)) 
{
	
	alt_table_row_color($k);

	label_cell($myrow["description"]);
	edit_link_cell(SID."selected_id=$myrow[0]");
	delete_link_cell(SID."selected_id=$myrow[0]&delete=yes");
	end_row();
}

end_table();

//----------------------------------------------------------------------------------

hyperlink_no_params($_SERVER['PHP_SELF'], _("New Item Category"));

start_form();

start_table("class='tablestyle_noborder'");

if (isset($selected_id)) 
{
	//editing an existing item category

	$myrow = get_item_category($selected_id);

	$_POST['category_id'] = $myrow["category_id"];
	$_POST['description']  = $myrow["description"];

	hidden('selected_id', $selected_id);
	hidden('category_id', $_POST['category_id']);
}

text_row(_("Category Name:"), 'description', null, 30, 30);  

end_table(1);

submit_add_or_update_center(!isset($selected_id));	

end_form();

end_page();

?>
