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
$page_security = 'SA_OPEN';
$path_to_root="..";

//if (file_exists($path_to_root.'/config.php'))
//	header("Location: $path_to_root/index.php");

include($path_to_root . "/install/isession.inc");

page(_($help_context = "FrontAccouting ERP Installation Wizard"), true, false, "", '', false,
	'stylesheet.css');

include($path_to_root . "/includes/ui.inc");
include($path_to_root . "/includes/system_tests.inc");
include($path_to_root . "/admin/db/maintenance_db.inc");
include($path_to_root . "/includes/packages.inc");
include($path_to_root . "/installed_extensions.php");
//-------------------------------------------------------------------------------------------------

function subpage_title($txt) 
{
	global $path_to_root;
	
	echo '<center><img src="'.$path_to_root.'/themes/default/images/logo_frontaccounting.png" width="250" height="50" alt="Logo" />
		</center>';
	$page = @$_POST['Page'] ? $_POST['Page'] : 1;

	display_heading(
		$page==5 ? $txt :
			_("FrontAccouting ERP Installation Wizard").'<br>'
			. sprintf(_('Step %d: %s'),  $page , $txt));
	br();
}

function display_coas()
{
	start_table(TABLESTYLE);
	$th = array(_("Chart of accounts"), _("Description"), _("Install"));
	table_header($th);

	$k = 0;
	$charts = get_charts_list();

	foreach($charts as $pkg_name => $coa)
	{
		$available = @$coa['available'];
		$installed = @$coa['version'];
		$id = @$coa['local_id'];

		alt_table_row_color($k);
		label_cell($coa['name']);
		label_cell(is_array($coa['Descr']) ? implode('<br>', $coa['Descr']) :  $coa['Descr']);
		if ($installed)
			label_cell(_("Installed"));
		else
			check_cells(null, 'coas['.$coa['package'].']');

		end_row();
	}
	end_table(1);
}

function install_connect_db() {
	global $db;

	$conn = $_SESSION['inst_set'];
	
	$db = mysql_connect($conn["host"] , $conn["dbuser"], $conn["dbpassword"]);
	if(!$db) {
		display_error('Cannot connect to database server. Host name, username and/or password incorrect.');
		return false;
	}
	if (!defined('TB_PREF'))
		define('TB_PREF', $conn["tbpref"]);

	if (!mysql_select_db($conn["dbname"], $db)) {
		$sql = "CREATE DATABASE " . $conn["dbname"];
		if (!mysql_query($sql)) {
			display_error('Cannot create database. Check your permissions to database creation or selct already created database.');
			return false;
		}
		return mysql_select_db($conn["dbname"], $db);
	}
	return true;
}

function do_install() {
	global $path_to_root, $db_connections, $def_coy, $installed_extensions;

	$coa = $_SESSION['inst_set']['coa'];
	if (install_connect_db() && db_import($path_to_root.'/sql/'.$coa, $_SESSION['inst_set'])) {
		$con = $_SESSION['inst_set'];
		$table_prefix = $con['tbpref'];
		update_admin_password($con, md5($con['pass']));
		update_company_prefs(array('coy_name'=>$con['name']));

		$def_coy = 0;
		$tb_pref_counter = 0;
		$db_connections = array (0=> array (
		 'name' => $con['name'],
		 'host' => $con['host'],
		 'dbuser' => $con['dbuser'],
		 'dbpassword' => $con['dbpassword'],
		 'dbname' => $con['dbname'],
		 'tbpref' => $table_prefix
		));
		$err = write_config_db($table_prefix != "");

		if ($err == -1) {
			display_error(_("Cannot open the config_db.php configuration file:"));
			return false;
		} else if ($err == -2) {
			display_error(_("Cannot write to the config_db.php configuration file"));
			return false;
		} else if ($err == -3) {
			display_error(_("The configuration file config_db.php is not writable. Change its permissions so it is, then re-run step 5."));
			return false;
		}
		if (!copy($path_to_root. "/config.default.php", $path_to_root. "/config.php")) {
			display_error(_("Cannot save system configuration file config.php"));
			return false;
		}
		if (count($installed_extensions))
		 if (!update_extensions($installed_extensions)) { // update company 0 extensions (charts)
			display_error(_("Can't update extensions configuration."));
			return false;
		 }
		return true;
	}
	return false;
}

if (!isset($_SESSION['inst_set']))  // default settings
	$_SESSION['inst_set'] = array(
		'host'=>'localhost', 
		'dbuser' => 'root',
		'dbpassword' => '',
		'username' => 'admin',
		'tbpref' => '0_',
		'admin' => 'admin',
		'coa_type' => 0
	);

if (!@$_POST['Tests'])
	$_POST['Page'] = 1; // set to start page

if (isset($_POST['back']) && (@$_POST['Page']>1)) {
	$_POST['Page']--;
	if ($_POST['Page'] == 3)
		$_POST['Page'] = 2;
}
elseif (isset($_POST['continue'])) {
	$_POST['Page'] = 2;
}
elseif (isset($_POST['db_test'])) {
	if (get_post('host')=='') {
		display_error(_('Host name cannot be empty'));
		set_focus('host');
	}
	elseif ($_POST['dbuser']=='') {
		display_error(_('Database user name cannot be empty'));
		set_focus('dbuser');
	}
	elseif ($_POST['dbname']=='') {
		display_error(_('Database name cannot be empty'));
		set_focus('dbname');
	}
	else {
		$_SESSION['inst_set'] = array_merge($_SESSION['inst_set'], array(
			'host' => $_POST['host'],
			'dbuser' => $_POST['dbuser'],
			'dbpassword' => $_POST['dbpassword'],
			'dbname' => $_POST['dbname'],
			'tbpref' => $_POST['tbpref'] ? '0_' : '',
		));
		if (install_connect_db()) {
			$_POST['Page'] = check_value('sel_coas') ? 3 : 4;
		}
	}
}
elseif(get_post('install_coas')) 
{
	$ret = true;
	if (isset($_POST['coas']))
		foreach($_POST['coas'] as $package => $ok) {
			$ret &= install_extension($package);
		}
	if ($ret) {
		$_POST['Page'] = 4;
	}
}
elseif (isset($_POST['set_admin'])) {
	// check company settings
	if (get_post('name')=='') {
		display_error(_('Company name cannot be empty.'));
		set_focus('name');
	}
	elseif (get_post('admin')=='') {
		display_error(_('Company admin name cannot be empty.'));
		set_focus('admin');
	}
	elseif (get_post('pass')=='') {
		display_error(_('Company admin password cannot be empty.'));
		set_focus('pass');
	}
	elseif (get_post('pass')!=get_post('repass')) {
		display_error(_('Company admin passwords differ.'));
		unset($_POST['pass'],$_POST['repass']);
		set_focus('pass');
	}
	else {

		$_SESSION['inst_set'] = array_merge($_SESSION['inst_set'], array(
			'coa' => $_POST['coa'],
			'pass' => $_POST['pass'],
			'name' => $_POST['name'],
			'admin' => $_POST['admin'],
		));
		if (do_install()) {
			$_POST['Page'] = 5;
		}
	}
}

start_form();
	switch(@$_POST['Page']) {
		default:
//			include ('../install.html');
//			submit_center('continue', _('Continue >>'));
//			break;
		case '1':
			subpage_title(_('System Diagnostics'));
			$_POST['Tests'] = display_system_tests(true);
			br();
			if (@$_POST['Tests']) {
				display_notification(_('All application preliminary requirements seems to be correct. Please press Continue button below.'));
				submit_center('continue', _('Continue >>'));
			} else {
				display_error(_('Application cannot be installed. Please fix problems listed below in red, and press Refresh button.'));
				submit_center('refresh', _('Refresh'));
			}
			break;

		case '2':
			if (!isset($_POST['host'])) {
				foreach($_SESSION['inst_set'] as $name => $val)
					$_POST[$name] = $val;
			}
			subpage_title(_('Database Server Settings'));
			start_table(TABLESTYLE);
			text_row_ex(_("Server Host"), 'host', 30);
			text_row_ex(_("Database User"), 'dbuser', 30);
			text_row_ex(_("Database Password"), 'dbpassword', 30);
			text_row_ex(_("Database Name"), 'dbname', 30);
			yesno_list_row(_("Use '0_' Table Prefix"), 'tbpref', 1, _('Yes'), _('No'), false);
			check_row(_("Install additional COAs form FA repository"), 'sel_coas');
			end_table(1);
			display_note(_('Use table prefix if you share selected database with another application, or you want to use it for more than one FA company.'));
			submit_center_first('back', _('<< Back'));
			submit_center_last('db_test', _('Continue >>'));
			break;

		case '3': // select COA
			subpage_title(_('Charts of accounts selection'));
			display_coas();
			submit_center_first('back', _('<< Back'));
			submit_center_last('install_coas', _('Continue >>'));
			break;

		case '4':
			if (!isset($_POST['name'])) {
				foreach($_SESSION['inst_set'] as $name => $val)
					$_POST[$name] = $val;
				set_focus('name');
			}
			subpage_title(_('Company Settings'));
			start_table(TABLESTYLE);
			text_row_ex(_("Company Name"), 'name', 30);
			text_row_ex(_("Admin Login"), 'admin', 30);
			password_row(_("Admin Password"), 'pass', @$_POST['pass']);
			password_row(_("Reenter Password"), 'repass', @$_POST['repass']);
			coa_list_row(_("Select Chart of Accounts"), 'coa');
			end_table(1);
			submit_center_first('back', _('<< Back'));
			submit_center_last('set_admin', _('Continue >>'));
			break;

		case '5': // final screen
			subpage_title(_('FrontAccounting ERP has been installed successsfully.'));
			display_note(_('Please remove install wizard folder.'));
			$install_done = true;
			hyperlink_no_params($path_to_root.'/index.php', _('Click here to start.'));
			break;

	}

	hidden('Tests');
	hidden('Page');
end_form(1);

end_page(false, false, true);

?>
