<?php

$page_security = 3;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("GL Account Classes"));

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
	$selected_id = -1;
//-----------------------------------------------------------------------------------

function can_process() 
{

	if (strlen($_POST['name']) == 0) 
	{
		display_error( _("The account class name cannot be empty."));
		set_focus('name');
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------------

if (isset($_POST['ADD_ITEM']) || isset($_POST['UPDATE_ITEM'])) 
{

	if (can_process()) 
	{

    	if ($selected_id != -1) 
    	{

    		update_account_class($selected_id, $_POST['name'], $_POST['Balance']);

    	} 
    	else 
    	{

    		add_account_class($_POST['id'], $_POST['name'], $_POST['Balance']);
    	}
		meta_forward($_SERVER['PHP_SELF']);
	}
}

//-----------------------------------------------------------------------------------

function can_delete($selected_id)
{
	if ($selected_id == -1)
		return false;
	$sql= "SELECT COUNT(*) FROM ".TB_PREF."chart_types
		WHERE class_id=$selected_id";
	$result = db_query($sql, "could not query chart master");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_("Cannot delete this account class because GL account types have been created referring to it."));
		return false;
	}

	return true;
}


//-----------------------------------------------------------------------------------

if (isset($_GET['delete'])) 
{

	if (can_delete($selected_id))
	{
		delete_account_class($selected_id);
		meta_forward($_SERVER['PHP_SELF']);
	}
}

//-----------------------------------------------------------------------------------

$result = get_account_classes();

start_table($table_style);
$th = array(_("Class ID"), _("Class Name"), _("Balance Sheet"), "", "");
table_header($th);

$k = 0;
while ($myrow = db_fetch($result)) 
{

	alt_table_row_color($k);

	if ($myrow["balance_sheet"] == 0) 
	{
		$bs_text = _("No");
	} 
	else 
	{
		$bs_text = _("Yes");
	}
	label_cell($myrow["cid"]);
	label_cell($myrow['class_name']);
	label_cell($bs_text);
	edit_link_cell("selected_id=" . $myrow["cid"]);
	delete_link_cell("selected_id=" . $myrow["cid"]. "&delete=1");
	end_row();
}

end_table();

//-----------------------------------------------------------------------------------

hyperlink_no_params($_SERVER['PHP_SELF'], _("New Account Class"));

start_form();

start_table($table_style2);

if ($selected_id != -1) 
{
	//editing an existing status code

	$myrow = get_account_class($selected_id);

	$_POST['id']  = $myrow["cid"];
	$_POST['name']  = $myrow["class_name"];
	$_POST['Balance']  = $myrow["balance_sheet"];
	hidden('selected_id', $selected_id);
	label_row(_("Class ID:"), $_POST['id']);

} 
else 
{

	text_row_ex(_("Class ID:"), 'id', 3);
}

text_row_ex(_("Class Name:"), 'name', 50);

yesno_list_row(_("Balance Sheet:"), 'Balance', null, "", "", false);

end_table(1);

submit_add_or_update_center($selected_id == -1);

end_form();

//------------------------------------------------------------------------------------

end_page();

?>
