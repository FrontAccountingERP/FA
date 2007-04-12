<?php

$page_security = 3;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("GL Account Groups"));

include($path_to_root . "/gl/includes/gl_db.inc");

include($path_to_root . "/includes/ui.inc");

if (isset($_GET['selected_id']))
{
	$selected_id = $_GET['selected_id'];
} 
elseif(isset($_POST['selected_id']))
{
	$selected_id = $_POST['selected_id'];
}
else
	$selected_id = "";
//-----------------------------------------------------------------------------------

function can_process() 
{
	global $selected_id;

	if (strlen($_POST['name']) == 0) 
	{
		display_error( _("The account group name cannot be empty."));
		return false;
	}

	if (isset($selected_id) && ($selected_id == $_POST['parent'])) 
	{
		display_error(_("You cannot set an account group to be a subgroup of itself."));
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------------

if (isset($_POST['ADD_ITEM']) || isset($_POST['UPDATE_ITEM'])) 
{

	if (can_process()) 
	{

    	if ($selected_id != "") 
    	{

    		update_account_type($selected_id, $_POST['name'], $_POST['class_id'], $_POST['parent']);

    	} 
    	else 
    	{

    		add_account_type($_POST['name'], $_POST['class_id'], $_POST['parent']);
    	}
		meta_forward($_SERVER['PHP_SELF']);
	}
}

//-----------------------------------------------------------------------------------

function can_delete($selected_id)
{
	if ($selected_id == "")
		return false;
	$sql= "SELECT COUNT(*) FROM ".TB_PREF."chart_master
		WHERE account_type=$selected_id";
	$result = db_query($sql, "could not query chart master");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_("Cannot delete this account group because GL accounts have been created referring to it."));
		return false;
	}

	$sql= "SELECT COUNT(*) FROM ".TB_PREF."chart_types
		WHERE parent=$selected_id";
	$result = db_query($sql, "could not query chart types");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_("Cannot delete this account group because GL account groups have been created referring to it."));
		return false;
	}

	return true;
}


//-----------------------------------------------------------------------------------

if (isset($_GET['delete'])) 
{

	if (can_delete($selected_id))
	{
		delete_account_type($selected_id);
		meta_forward($_SERVER['PHP_SELF']);
	}
}

//-----------------------------------------------------------------------------------

$result = get_account_types();

start_table($table_style);
$th = array(_("Name"), _("Subgroup Of"), _("Class Type"), "", "");
table_header($th);

$k = 0;
while ($myrow = db_fetch($result)) 
{

	alt_table_row_color($k);

	$bs_text = get_account_class_name($myrow["class_id"]);

	if ($myrow["parent"] == reserved_words::get_any_numeric()) 
	{
		$parent_text = "";
	} 
	else 
	{
		$parent_text = get_account_type_name($myrow["parent"]);
	}

	label_cell($myrow["name"]);
	label_cell($parent_text);
	label_cell($bs_text);
	edit_link_cell("selected_id=" . $myrow["id"]);
	delete_link_cell("selected_id=" . $myrow["id"]. "&delete=1");
	end_row();
}

end_table();

//-----------------------------------------------------------------------------------

hyperlink_no_params($_SERVER['PHP_SELF'], _("New Account Group"));

start_form();

start_table($table_style2);

if ($selected_id != "") 
{
	//editing an existing status code

	$myrow = get_account_type($selected_id);

	$_POST['name']  = $myrow["name"];
	$_POST['parent']  = $myrow["parent"];
	$_POST['class_id']  = $myrow["class_id"];

	hidden('selected_id', $selected_id);
}

text_row_ex(_("Name:"), 'name', 50);

gl_account_types_list_row(_("Subgroup Of:"), 'parent', null, true, _("None"), true);

class_list_row(_("Class Type:"), 'class_id', null);

end_table(1);

submit_add_or_update_center($selected_id == "");

end_form();

//------------------------------------------------------------------------------------

end_page();

?>
