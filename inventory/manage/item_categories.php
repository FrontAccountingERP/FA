<?php

$page_security = 11;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("Item Categories"));

include_once($path_to_root . "/includes/ui.inc");

include_once($path_to_root . "/inventory/includes/inventory_db.inc");

simple_page_mode(true);
//----------------------------------------------------------------------------------

if ($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM') 
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
    	if ($selected_id != -1) 
    	{
		    update_item_category($selected_id, $_POST['description']);    		
			display_notification(_('Selected item category has been updated'));
    	} 
    	else 
    	{
		    add_item_category($_POST['description']);
			display_notification(_('New item category has been added'));
    	}
		$Mode = 'RESET';
	}
}

//---------------------------------------------------------------------------------- 

if ($Mode == 'Delete')
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
		display_notification(_('Selected item category has been deleted'));
	}
	$Mode = 'RESET';
}

if ($Mode == 'RESET')
{
	$selected_id = -1;
	unset($_POST);
}
//----------------------------------------------------------------------------------

$sql = "SELECT * FROM ".TB_PREF."stock_category";
$result = db_query($sql, "could not get stock categories");

start_form();
start_table("$table_style width=30%");
$th = array(_("Name"), "", "");
table_header($th);
$k = 0; //row colour counter

while ($myrow = db_fetch($result)) 
{
	
	alt_table_row_color($k);

	label_cell($myrow["description"]);
 	edit_button_cell("Edit".$myrow[0], _("Edit"));
 	edit_button_cell("Delete".$myrow[0], _("Delete"));
	end_row();
}

end_table();
end_form();
echo '<br>';
//----------------------------------------------------------------------------------

start_form();

start_table("class='tablestyle_noborder'");

if ($selected_id != -1) 
{
 	if ($Mode == 'Edit') {
		//editing an existing item category
		$myrow = get_item_category($selected_id);

		$_POST['category_id'] = $myrow["category_id"];
		$_POST['description']  = $myrow["description"];
	}
	hidden('selected_id', $selected_id);
	hidden('category_id');
}

text_row(_("Category Name:"), 'description', null, 30, 30);  

end_table(1);

submit_add_or_update_center($selected_id == -1, '', true);

end_form();

end_page();

?>
