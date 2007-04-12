<?php

$page_security=15;
$path_to_root="..";
include_once($path_to_root . "/includes/session.inc");

page(_("Users"));

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");

include_once($path_to_root . "/admin/db/users_db.inc");

if (isset($_GET['selected_id']))
{
	$selected_id = $_GET['selected_id'];
} 
elseif (isset($_POST['selected_id']))
{
	$selected_id = $_POST['selected_id'];
}

//-------------------------------------------------------------------------------------------------

if (isset($_GET['AddedID'])) 
{
	display_notification_centered(_("A new user has been added."));
}

if (isset($_GET['UpdatedID'])) 
{
    display_notification_centered(_("The selected user has been updated."));
}

if (isset($_GET['DeletedID'])) 
{
	display_notification_centered(_("User has been deleted."));
}

//-------------------------------------------------------------------------------------------------

function can_process() 
{

	if (strlen($_POST['user_id']) < 4)
	{
		display_error( _("The user login entered must be at least 4 characters long."));
		return false;
	}

	if ($_POST['password'] != "") 
	{
    	if (strlen($_POST['password']) < 4)
    	{
    		display_error( _("The password entered must be at least 4 characters long."));
    		return false;
    	}

    	if (strstr($_POST['password'], $_POST['user_id']) != false)
    	{
    		display_error( _("The password cannot contain the user login."));
    		return false;
    	}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------

if (isset($_POST['ADD_ITEM']) || isset($_POST['UPDATE_ITEM'])) 
{

	if (can_process())
	{
    	if (isset($selected_id)) 
    	{
    		update_user($_POST['user_id'], $_POST['real_name'], $_POST['phone'],
    			$_POST['email'], $_POST['Access'], $_POST['language']);

    		if ($_POST['password'] != "")
    			update_user_password($_POST['user_id'], md5($_POST['password']));

			unset($selected_id);
    		meta_forward($_SERVER['PHP_SELF'], "UpdatedID=1");
    	} 
    	else 
    	{
    		add_user($_POST['user_id'], $_POST['real_name'], md5($_POST['password']),
				$_POST['phone'], $_POST['email'], $_POST['Access'], $_POST['language']);

			unset($selected_id);
			meta_forward($_SERVER['PHP_SELF'], "AddedID=1");
    	}
	}
}

//-------------------------------------------------------------------------------------------------

if (isset($_GET['delete'])) 
{
	delete_user($selected_id);
	unset($selected_id);

	meta_forward($_SERVER['PHP_SELF'], "DeletedID=1");
}

//-------------------------------------------------------------------------------------------------

$result = get_users();

start_table($table_style);

if ($_SESSION["wa_current_user"]->access == 2)
	$th = array(_("User login"), _("Full Name"), _("Phone"),
		_("E-mail"), _("Last Visit"), _("Access Level"), "", "");
else		
	$th = array(_("User login"), _("Full Name"), _("Phone"),
		_("E-mail"), _("Last Visit"), _("Access Level"), "");
table_header($th);	

$k = 0; //row colour counter

while ($myrow = db_fetch($result)) 
{

	alt_table_row_color($k);

	$last_visit_date = sql2date($myrow["last_visit_date"]);

	/*The security_headings array is defined in config.php */

	label_cell($myrow["user_id"]);
	label_cell($myrow["real_name"]);
	label_cell($myrow["phone"]);
	label_cell($myrow["email"]);
	label_cell($last_visit_date, "nowrap");
	label_cell($security_headings[$myrow["full_access"]]);
    edit_link_cell("selected_id=".$myrow["user_id"]);
    if (strcasecmp($myrow["user_id"], $_SESSION["wa_current_user"]->username) &&
    	$_SESSION["wa_current_user"]->access == 2)
    	delete_link_cell("selected_id=".$myrow["user_id"]."&delete=1");
	end_row();

} //END WHILE LIST LOOP

end_table();

//-------------------------------------------------------------------------------------------------

hyperlink_no_params($_SERVER['PHP_SELF'], _("New User"));

start_form();

start_table($table_style2);
if (isset($selected_id)) 
{
	//editing an existing User

	$myrow = get_user($selected_id);

	$_POST['user_id'] = $myrow["user_id"];
	$_POST['real_name'] = $myrow["real_name"];
	$_POST['phone'] = $myrow["phone"];
	$_POST['email'] = $myrow["email"];
	$_POST['Access'] = $myrow["full_access"];
	$_POST['language'] = $myrow["language"];

	hidden('selected_id', $selected_id);
	hidden('user_id', $_POST['user_id']);

	start_row();
	label_row(_("User login:"), $_POST['user_id']);

} 
else 
{ //end of if $selected_id only do the else when a new record is being entered
	text_row(_("User Login:"), "user_id",  null, 22, 20);
}
$_POST['password'] = "";
start_row();
label_cell(_("Password:"));
label_cell("<input type='password' name='password' size=22 maxlength=20 value='" . $_POST['password'] . "'>");
end_row();

if (isset($selected_id)) 
{
	table_section_title(_("Enter a new password to change, leave empty to keep current."));
}

text_row_ex(_("Full Name").":", 'real_name',  50);

text_row_ex(_("Telephone No.:"), 'phone', 30);

text_row_ex(_("Email Address:"), 'email', 50);

security_headings_list_row(_("Access Level:"), "Access", null); 

languages_list_row(_("Language:"), 'language', null);

end_table(1);

submit_add_or_update_center(!isset($selected_id));

end_form();
end_page();
?>
