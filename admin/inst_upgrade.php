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
//	Checks $field existence in $table with given field $properties
//	$table - table name without prefix
//  $field -  optional field name
//  $properties - optional properties of field defined by MySQL:
//		'Type', 'Null', 'Key', 'Default', 'Extra'
//
function check_table($pref, $table, $field=null, $properties=null)
{
	$tables = @db_query("SHOW TABLES LIKE '".$pref.$table."'");
	if (!db_num_rows($tables))
		return 1;		// no such table or error

	$fields = @db_query("SHOW COLUMNS FROM ".$pref.$table);
	if (!isset($field)) 
		return 0;		// table exists

	while( $row = db_fetch_assoc($fields)) 
	{
		if ($row['Field'] == $field) 
		{
			if (!isset($properties)) 
				return 0;
			foreach($properties as $property => $value) 
			{
				if ($row[$property] != $value) 
					return 3;	// failed type/length check
			}
			return 0; // property check ok.
		}
	}
	return 2; // field not found
}
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
				&& stristr($fname, '.php') != false)
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

			if ($sql != '')
				$ret &= db_import($path_to_root.'/sql/'.$sql, $conn, $force);

			$ret &= $inst->install($pref, $force);
		} else
			if ($state!==true) {
				display_error(_("Upgrade cannot be done because database has been already partially upgraded. Please downgrade database to clean previous version or try forced upgrade."));
				$ret = false;
			}
	}
	return $ret;
}

function db_open($conn)
{
	$db = mysql_connect($conn["host"] ,$conn["dbuser"], $conn["dbpassword"]);
	if (!$db)
		return false;
	if (!mysql_select_db($conn["dbname"], $db))
		return false;
	return $db;
}

$installers = get_installers();

if (get_post('Upgrade')) 
{

	$ret = true;
	foreach ($db_connections as $conn) 
	{
	// connect to database
		if (!($db = db_open($conn))) 
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
	if($ret)
	{	// re-read the prefs
		global $path_to_root;
		include_once($path_to_root . "/admin/db/users_db.inc");
		$user = get_user_by_login($_SESSION["wa_current_user"]->username);
		$_SESSION["wa_current_user"]->prefs = new user_prefs($user);
		display_notification(_('All companies data has been successfully updated'));
	}	
	$Ajax->activate('_page_body');
}

start_form();
start_table($table_style);
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
			label_cell("<span class=redfg>"
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