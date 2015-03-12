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
$page_security = 'SA_SOFTWAREUPGRADE';
$path_to_root="..";
include_once($path_to_root . "/includes/session.inc");

page(_($help_context = "Software Upgrade"));

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/admin/db/company_db.inc");
include_once($path_to_root . "/admin/db/maintenance_db.inc");
include_once($path_to_root . "/includes/ui.inc");

//
//	Creates table of installer objects sorted by version.
//
function get_installers()
{
	global $path_to_root;

	$patchdir = $path_to_root."/sql/";
	$upgrades = array();	
	$datadir = @opendir($patchdir);

	if ($datadir)
	{
		while(false !== ($fname = readdir($datadir)))
		{ // check all php files but index.php
			if (!is_dir($patchdir . $fname) && ($fname != 'index.php')
				&& stristr($fname, '.php') != false && $fname[0] != '.')
			{
				unset($install);
				include_once($patchdir . $fname);
				if (isset($install)) // add installer if found
					$upgrades[$install->version] =  $install;
			}
		}
		ksort($upgrades); // sort by file name
		$upgrades = array_values($upgrades);
	}
	return $upgrades;
}
//
//	Apply one differential data set.
//
function upgrade_step($index, $conn) 
{
	global $path_to_root, $installers;

	$inst = $installers[$index];
	$pref = $conn['tbpref'];
	$ret = true;

	$force = get_post('force_'.$index);
	if ($force || get_post('install_'.$index)) 
	{
		$state = $inst->installed($pref);
		if (!$state || $force) 
		{
			if (!$inst->pre_check($pref, $force)) return false;
			$sql = $inst->sql;

			error_log(sprintf(_("Database upgrade for company '%s' (%s:%s*) started..."),
				$conn['name'], $conn['dbname'], $conn['tbpref']));
				
			if ($sql != '')
				$ret &= db_import($path_to_root.'/sql/'.$sql, $conn, $force);

			$ret &= $inst->install($pref, $force);

			error_log(_("Database upgrade finished."));

		} else
			if ($state!==true) {
				display_error(_("Upgrade cannot be done because database has been already partially upgraded. Please downgrade database to clean previous version or try forced upgrade."));
				$ret = false;
			}
	}
	return $ret;
}

$installers = get_installers();

if (get_post('Upgrade')) 
{

	$ret = true;
	foreach ($db_connections as $comp => $conn) 
	{
	// connect to database
		if (!(set_global_connection($comp))) 
		{
			display_error(_("Cannot connect to database for company")
				." '".$conn['name']."'");
			continue;
		}
	// create security backup	
		db_backup($conn, 'no', 'Security backup before upgrade', $conn['tbpref']);
	// apply all upgrade data
		foreach ($installers as $i => $inst) 
		{
			$ret = upgrade_step($i, $conn);
			if (!$ret)
				display_error(
				sprintf(_("Database upgrade to version %s failed for company '%s'."),
					$inst->version, $conn['name'])
					.'<br>'
					._('You should restore company database from latest backup file'));
		}
// 		db_close($conn); ?
		if (!$ret) break;
	}
	set_global_connection();
	if($ret)
	{	// re-read the prefs
		global $path_to_root;
		include_once($path_to_root . "/admin/db/users_db.inc");
		$user = get_user_by_login($_SESSION["wa_current_user"]->username);
		$_SESSION["wa_current_user"]->prefs = new user_prefs($user);
		display_notification(_('All companies data has been successfully updated'));
	}	
	refresh_sys_prefs(); // re-read system setup
	$Ajax->activate('_page_body');
}

start_form();
start_table(TABLESTYLE);
$th = array(_("Version"), _("Description"), _("Sql file"), _("Install"),
	_("Force upgrade"));
table_header($th);

$k = 0; //row colour counter
$partial = 0;
foreach($installers as $i => $inst)
{
	alt_table_row_color($k);
	start_row();
	label_cell($inst->version);
	label_cell($inst->description);
	label_cell($inst->sql ? $inst->sql : '<i>'._('None').'</i>', 'align=center');
// this is checked only for first (site admin) company, 
// but in fact we should always upgrade all data sets after
// source upgrade.
	$check = $inst->installed(TB_PREF);
	if ($check === true)
		label_cell(_("Installed"));
	else 
		if (!$check)
			check_cells(null,'install_'.$i, 0);
		else {
			label_cell("<span class='redfg'>"
				. sprintf(_("Partially installed (%s)"), $check) . "</span>");
			$partial++;
		}

	check_cells(null,'force_'.$i, 0);
	end_row();
}
end_table(1);
if ($partial!=0)	{
	display_note(_("Database upgrades marked as partially installed cannot be installed automatically.
You have to clean database manually to enable them, or try to perform forced upgrade."));
	br();
}
submit_center('Upgrade', _('Upgrade system'), true, _('Save database and perform upgrade'), 'process');
end_form();

end_page();

?>