<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU General Public License, GPL, 
	as published by the Free Software Foundation, either version 3 
	of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/gpl-3.0.html>.
***********************************************************************/
$page_security = 'SA_CREATECOMPANY';
$path_to_root="..";
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/admin/db/company_db.inc");
include_once($path_to_root . "/admin/db/maintenance_db.inc");
include_once($path_to_root . "/includes/ui.inc");

page(_($help_context = "Create/Update Company"));

$comp_subdirs = array('images', 'pdf_files', 'backup','js_cache', 'reporting', 'attachments');

//---------------------------------------------------------------------------------------------
if (isset($_GET['selected_id']))
{
	$selected_id = $_GET['selected_id'];
}
elseif (isset($_POST['selected_id']))
{
	$selected_id = $_POST['selected_id'];
}
else
	$selected_id = -1;

//---------------------------------------------------------------------------------------------

function check_data()
{
	global $db_connections, $tb_pref_counter, $selected_id;

	if($selected_id != -1) {
		if ($_POST['name'] == "")
		{
			display_error(_("Database settings are not specified."));
	 		return false;
		}
	} else {
		if ($_POST['name'] == "" || $_POST['host'] == "" || $_POST['dbuser'] == "" || $_POST['dbname'] == "")
		{
			display_error(_("Database settings are not specified."));
	 		return false;
		}
		foreach($db_connections as $id=>$con)
		{
		 if($id != $selected_id && $_POST['host'] == $con['host'] 
		 	&& $_POST['dbname'] == $con['dbname'])
	  		{
				if ($_POST['tbpref'] == $con['tbpref'])
				{
					display_error(_("This database settings are already used by another company."));
					return false;
				}
				if (($_POST['tbpref'] == 0) ^ ($con['tbpref'] == ''))
				{
					display_error(_("You cannot have table set without prefix together with prefixed sets in the same database."));
					return false;
				}
		  	}
		}
	}
	return true;
}

//---------------------------------------------------------------------------------------------

function remove_connection($id) {
	global $db_connections;

	$err = db_drop_db($db_connections[$id]);

	unset($db_connections[$id]);
	$conn = array_values($db_connections);
	$db_connections = $conn;
	//$$db_connections = array_values($db_connections);
    return $err;
}
//---------------------------------------------------------------------------------------------

function handle_submit()
{
	global $db_connections, $def_coy, $tb_pref_counter, $db,
	    $comp_subdirs, $path_to_root, $selected_id;

	$error = false;
	if (!check_data())
		return false;

	if ($selected_id==-1)
		$selected_id = count($db_connections);

	$new = !isset($db_connections[$selected_id]);

	if ((bool)$_POST['def'] == true)
		$def_coy = $selected_id;

	$db_connections[$selected_id]['name'] = $_POST['name'];
	if($new) {
		$db_connections[$selected_id]['host'] = $_POST['host'];
		$db_connections[$selected_id]['dbuser'] = $_POST['dbuser'];
		$db_connections[$selected_id]['dbpassword'] = $_POST['dbpassword'];
		$db_connections[$selected_id]['dbname'] = $_POST['dbname'];
		if (is_numeric($_POST['tbpref']))
		{
			$db_connections[$selected_id]['tbpref'] = $_POST['tbpref'] == 1 ?
			  $tb_pref_counter."_" : '';
		}
		else if ($_POST['tbpref'] != "")
			$db_connections[$selected_id]['tbpref'] = $_POST['tbpref'];
		else
			$db_connections[$selected_id]['tbpref'] = "";

		$conn = $db_connections[$selected_id];
		if (($db = db_create_db($conn)) == 0)
		{
			display_error(_("Error creating Database: ") . $conn['dbname'] . _(", Please create it manually"));
			$error = true;
		} else {
			if (!db_import($path_to_root.'/sql/'.get_post('coa'), $conn, $selected_id)) {
				display_error(_('Cannot create new company due to bugs in sql file.'));
				$error = true;
			} 
			else
			{
				if (!isset($_POST['admpassword']) || $_POST['admpassword'] == "")
					$_POST['admpassword'] = "password";
				update_admin_password($conn, md5($_POST['admpassword']));
			}	
		}
		set_global_connection();
		if ($error) {
			remove_connection($selected_id);
			return false;
		}
	}
	$error = write_config_db($new);
	if ($error == -1)
		display_error(_("Cannot open the configuration file - ") . $path_to_root . "/config_db.php");
	else if ($error == -2)
		display_error(_("Cannot write to the configuration file - ") . $path_to_root . "/config_db.php");
	else if ($error == -3)
		display_error(_("The configuration file ") . $path_to_root . "/config_db.php" . _(" is not writable. Change its permissions so it is, then re-run the operation."));
	if ($error != 0)
	{
		return false;
	}

	if ($new)
	{
		create_comp_dirs(company_path($selected_id), $comp_subdirs);
	}
	$exts = get_company_extensions();
	write_extensions($exts, $selected_id);
	display_notification($new ? _('New company has been created.') : _('Company has been updated.'));
	return true;
}

//---------------------------------------------------------------------------------------------

function handle_delete()
{
	global $def_coy, $db_connections, $comp_subdirs, $path_to_root;

	$id = $_GET['id'];

	// First make sure all company directories from the one under removal are writable. 
	// Without this after operation we end up with changed per-company owners!
	for($i = $id; $i < count($db_connections); $i++) {
			$comp_path = company_path($i);
		if (!is_dir($comp_path) || !is_writable($comp_path)) {
			display_error(_('Broken company subdirectories system. You have to remove this company manually.'));
			return;
		}
	}
	// make sure config file is writable
	if (!is_writeable($path_to_root . "/config_db.php"))
	{
		display_error(_("The configuration file ") . $path_to_root . "/config_db.php" . _(" is not writable. Change its permissions so it is, then re-run the operation."));
		return;
	}
	// rename directory to temporary name to ensure all
	// other subdirectories will have right owners even after
	// unsuccessfull removal.
	$cdir = company_path($id);
	$tmpname  = company_path('/old_'.$id);
	if (!@rename($cdir, $tmpname)) {
		display_error(_('Cannot rename subdirectory to temporary name.'));
		return;
	}
	// 'shift' company directories names
	for ($i = $id+1; $i < count($db_connections); $i++) {
		if (!rename(company_path($i), company_path($i-1))) {
			display_error(_("Cannot rename company subdirectory"));
			return;
		}
	}
	$err = remove_connection($id);
	if ($err == 0)
		display_error(_("Error removing Database: ") . $dbase . _(", please remove it manually"));

	if ($def_coy == $id)
		$def_coy = 0;
	$error = write_config_db();
	if ($error == -1)
		display_error(_("Cannot open the configuration file - ") . $path_to_root . "/config_db.php");
	else if ($error == -2)
		display_error(_("Cannot write to the configuration file - ") . $path_to_root . "/config_db.php");
	else if ($error == -3)
		display_error(_("The configuration file ") . $path_to_root . "/config_db.php" . _(" is not writable. Change its permissions so it is, then re-run the operation."));
	if ($error != 0) {
		@rename($tmpname, $cdir);
		return;
	}
	// finally remove renamed company directory
	@flush_dir($tmpname, true);
	if (!@rmdir($tmpname))
	{
		display_error(_("Cannot remove temporary renamed company data directory ") . $tmpname);
		return;
	}
	display_notification(_("Selected company has been deleted"));
}

//---------------------------------------------------------------------------------------------

function display_companies()
{
	global $def_coy, $db_connections;

	$coyno = $_SESSION["wa_current_user"]->company;

	echo "
		<script language='javascript'>
		function deleteCompany(id, name) {
			if (!confirm('" . _("Are you sure you want to delete company no. ") . "'+id+' '+name+'?'))
				return
			document.location.replace('create_coy.php?c=df&id='+id)
		}
		</script>";
	start_table(TABLESTYLE);

	$th = array(_("Company"), _("Database Host"), _("Database User"),
		_("Database Name"), _("Table Pref"), _("Default"), "", "");
	table_header($th);

	$k=0;
	$conn = $db_connections;
	$n = count($conn);
	for ($i = 0; $i < $n; $i++)
	{
		if ($i == $def_coy)
			$what = _("Yes");
		else
			$what = _("No");
		if ($i == $coyno)
    		start_row("class='stockmankobg'");
    	else
    		alt_table_row_color($k);

		label_cell($conn[$i]['name']);
		label_cell($conn[$i]['host']);
		label_cell($conn[$i]['dbuser']);
		label_cell($conn[$i]['dbname']);
		label_cell($conn[$i]['tbpref']);
		label_cell($what);
		$edit = _("Edit");
		$delete = _("Delete");
		if (user_graphic_links())
		{
			$edit = set_icon(ICON_EDIT, $edit);
			$delete = set_icon(ICON_DELETE, $delete);
		}
    	label_cell("<a href='" . $_SERVER['PHP_SELF']. "?selected_id=$i'>$edit</a>");
    	$name = "\"".$conn[$i]['name']."\"";
		label_cell( $i == $coyno ? '' :
			"<a href='javascript:deleteCompany($i, $name)'>$delete</a>");
		end_row();
	}

	end_table();
    display_note(_("The marked company is the current company which cannot be deleted."), 0, 0, "class='currentfg'");
    display_note(_("If no Admin Password is entered, the new Admin Password will be '<b>password</b>' by default "), 1, 0, "class='currentfg'");
}

//---------------------------------------------------------------------------------------------

function display_company_edit($selected_id)
{
	global $def_coy, $db_connections, $tb_pref_counter;

	start_form();

	start_table(TABLESTYLE2);

	if ($selected_id != -1)
	{
		$conn = $db_connections[$selected_id];
		$_POST['name'] = $conn['name'];
		$_POST['host']  = $conn['host'];
		$_POST['dbuser']  = $conn['dbuser'];
		$_POST['dbpassword']  = $conn['dbpassword'];
		$_POST['dbname']  = $conn['dbname'];
		$_POST['tbpref']  = $conn['tbpref'];
		if ($selected_id == $def_coy)
			$_POST['def'] = true;
		else
			$_POST['def'] = false;
		$_POST['dbcreate']  = false;
		hidden('selected_id', $selected_id);
		hidden('tbpref', $_POST['tbpref']);
		hidden('dbpassword', $_POST['dbpassword']);
	}
	else
	{
		$_POST['tbpref'] = $tb_pref_counter."_";
		// Insert the current settings as default
		$conn = $db_connections[user_company()];
		$_POST['name'] = '';
		$_POST['host']  = $conn['host'];
		$_POST['dbuser']  = $conn['dbuser'];
		$_POST['dbpassword']  = $conn['dbpassword'];
		$_POST['dbname']  = $conn['dbname'];
	}

	text_row_ex(_("Company"), 'name', 50);

	if ($selected_id == -1)
	{
		text_row_ex(_("Host"), 'host', 30, 60);
		text_row_ex(_("Database User"), 'dbuser', 30);
		text_row_ex(_("Database Password"), 'dbpassword', 30);
		text_row_ex(_("Database Name"), 'dbname', 30);
		yesno_list_row(_("Table Pref"), 'tbpref', 1, $_POST['tbpref'], _("None"), false);
	} else {
		label_row(_("Host"), $_POST['host']);
		label_row(_("Database User"), $_POST['dbuser']);
		label_row(_("Database Name"), $_POST['dbname']);
		label_row(_("Table Pref"), $_POST['tbpref']);
	}
	yesno_list_row(_("Default"), 'def', null, "", "", false);

	if ($selected_id == -1)
	{
		coa_list_row(_("Database Script"), 'coa');
		text_row_ex(_("New script Admin Password"), 'admpassword', 20);
	}
	end_table(1);

	submit_center('save', _("Save"));

	end_form();
}


//---------------------------------------------------------------------------------------------

if (isset($_GET['c']) && $_GET['c'] == 'df') {
	handle_delete();
	$selected_id = -1;
}

if (get_post('save')) {
	if (handle_submit())
		$selected_id = -1;
}

//---------------------------------------------------------------------------------------------

display_companies();

hyperlink_no_params($_SERVER['PHP_SELF'], _("Create a new company"));

display_company_edit($selected_id);

//---------------------------------------------------------------------------------------------
end_page();

?>