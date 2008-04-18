<?php


$page_security = 3;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("Sales Areas"));

include($path_to_root . "/includes/ui.inc");

if (isset($_GET['selected_id']))
{
	$selected_id = strtoupper($_GET['selected_id']);
} 
elseif (isset($_POST['selected_id']))
{
	$selected_id = strtoupper($_POST['selected_id']);
}

if (isset($_POST['ADD_ITEM']) || isset($_POST['UPDATE_ITEM'])) 
{

	$input_error = 0;

	if (strlen($_POST['description']) == 0) 
	{
		$input_error = 1;
		display_error(_("The area description cannot be empty."));
	}

	if ($input_error != 1)
	{
    	if (isset($selected_id)) 
    	{
    		$sql = "UPDATE ".TB_PREF."areas SET description=".db_escape($_POST['description'])." WHERE area_code = '$selected_id'";
    	} 
    	else 
    	{
    
    		$sql = "INSERT INTO ".TB_PREF."areas (description) VALUES (".db_escape($_POST['description']) . ")";
    	}
    
    	db_query($sql,"The sales area could not be updated or added");
    	
		meta_forward($_SERVER['PHP_SELF']);		 	
	}
} 

if (isset($_GET['delete'])) 
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

		meta_forward($_SERVER['PHP_SELF']);			
	} //end if Delete area
} 

//-------------------------------------------------------------------------------------------------

$sql = "SELECT * FROM ".TB_PREF."areas";
$result = db_query($sql,"could not get areas");

start_table("$table_style width=40%");
$th = array(_("Area Name"), "", "");
table_header($th);
$k = 0; 

while ($myrow = db_fetch($result)) 
{
	
	alt_table_row_color($k);
		
	label_cell($myrow["description"]);
	edit_link_cell("selected_id=" . $myrow["area_code"]);
	delete_link_cell("selected_id=" . $myrow["area_code"]. "&delete=1");
	end_row();
}


end_table();
hyperlink_no_params($_SERVER['PHP_SELF'], _("New Sales Area"));

//-------------------------------------------------------------------------------------------------

start_form();

start_table("$table_style2 width=40%");

if (isset($selected_id)) 
{
	//editing an existing area
	$sql = "SELECT * FROM ".TB_PREF."areas WHERE area_code='$selected_id'";

	$result = db_query($sql,"could not get area");
	$myrow = db_fetch($result);

	$_POST['description']  = $myrow["description"];
	hidden("selected_id", $selected_id);
} 

text_row_ex(_("Area Name:"), 'description', 30); 

end_table(1);

submit_add_or_update_center(!isset($selected_id));

end_form();

end_page();
?>
