<?php

$page_security=1;
$path_to_root="..";
include_once($path_to_root . "/includes/session.inc");

page(_("Change password"));

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");

include_once($path_to_root . "/admin/db/users_db.inc");

$selected_id = $_SESSION["wa_current_user"]->username;


function can_process()
{

   	if (strlen($_POST['password']) < 4)
   	{
  		display_error( _("The password entered must be at least 4 characters long."));
		set_focus('password');
   		return false;
   	}

   	if (strstr($_POST['password'], $_POST['user_id']) != false)
   	{
   		display_error( _("The password cannot contain the user login."));
		set_focus('password');
   		return false;
   	}

   	if ($_POST['password'] != $_POST['passwordConfirm'])
   	{
   		display_error( _("The passwords entered are not the same."));
		set_focus('password');
   		return false;
   	}

	return true;
}

if (isset($_POST['UPDATE_ITEM']))
{

	if (can_process())
	{
		update_user_password($_POST['user_id'], md5($_POST['password']));
	    display_notification(_("Your password has been updated."));
		$Ajax->activate('_page_body');
	}
}

start_form();

start_table($table_style);

$myrow = get_user($selected_id);

$_POST['user_id'] = $myrow["user_id"];
hidden('selected_id', $selected_id);
hidden('user_id', $_POST['user_id']);

label_row(_("User login:"), $_POST['user_id']);

$_POST['password'] = "";
$_POST['passwordConfirm'] = "";

start_row();
label_cell(_("Password:"));
label_cell("<input type='password' name='password' size=22 maxlength=20 value='" . $_POST['password'] . "'>");
end_row();

start_row();
label_cell(_("Repeat password:"));
label_cell("<input type='password' name='passwordConfirm' size=22 maxlength=20 value='" . $_POST['passwordConfirm'] . "'>");
end_row();

table_section_title(_("Enter your new password in the fields."));

end_table(1);

submit_center( 'UPDATE_ITEM', _('Change password'), true, '', true);
end_form();
end_page();
?>
