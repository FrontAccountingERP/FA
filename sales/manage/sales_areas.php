<?php


$page_security = 3;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("Sales Areas"));

include($path_to_root . "/includes/ui.inc");

simple_page_mode(true);

if ($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM') 
{

	$input_error = 0;

	if (strlen($_POST['description']) == 0) 
	{
		$input_error = 1;
		display_error(_("The area description cannot be empty."));
	}

	if ($input_error != 1)
	{
    	if ($selected_id != -1) 
    	{
    		$sql = "UPDATE ".TB_PREF."areas SET description=".db_escape($_POST['description'])." WHERE area_code = '$selected_id'";
			$note = _('Selected sales area has been updated');
    	} 
    	else 
    	{
    		$sql = "INSERT INTO ".TB_PREF."areas (description) VALUES (".db_escape($_POST['description']) . ")";
			$note = _('New sales area has been added');
    	}
    
    	db_query($sql,"The sales area could not be updated or added");
		display_notification($note);    	
		$Mode = 'RESET';
	}
} 

if ($Mode == 'Delete')
{

	$cancel_delete = 0;

	// PREVENT DELETES IF DEPENDENT RECORDS IN 'debtors_master'

	$sql= "SELECT COUNT(*) FROM ".TB_PREF."cust_branch WHERE area='$selected_id'";
	$result = db_query($sql,"check failed");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		$cancel_delete = 1;
		display_error(_("Cannot delete this area because customer branches have been created using this area."));
	} 
	if ($cancel_delete == 0) 
	{
		$sql="DELETE FROM ".TB_PREF."areas WHERE area_code='" . $selected_id . "'";
		db_query($sql,"could not delete sales area");

		display_notification(_('Selected sales area has been deleted'));
		$Mode = 'RESET';
	} //end if Delete area
} 

if ($Mode == 'RESET')
{
	$selected_id = -1;
	unset($_POST);
}
//-------------------------------------------------------------------------------------------------

$sql = "SELECT * FROM ".TB_PREF."areas";
$result = db_query($sql,"could not get areas");

start_form();
start_table("$table_style width=40%");
$th = array(_("Area Name"), "", "");
table_header($th);
$k = 0; 

while ($myrow = db_fetch($result)) 
{
	
	alt_table_row_color($k);
		
	label_cell($myrow["description"]);
 	edit_button_cell("Edit".$myrow["area_code"], _("Edit"));
 	edit_button_cell("Delete".$myrow["area_code"], _("Delete"));
	end_row();
}


end_table();
end_form();
echo '<br>';

//-------------------------------------------------------------------------------------------------

start_form();

start_table("$table_style2 width=40%");

if ($selected_id != -1) 
{
 	if ($Mode == 'Edit') {
		//editing an existing area
		$sql = "SELECT * FROM ".TB_PREF."areas WHERE area_code='$selected_id'";

		$result = db_query($sql,"could not get area");
		$myrow = db_fetch($result);

		$_POST['description']  = $myrow["description"];
	}
	hidden("selected_id", $selected_id);
} 

text_row_ex(_("Area Name:"), 'description', 30); 

end_table(1);

submit_add_or_update_center($selected_id == -1, '', true);

end_form();

end_page();
?>
