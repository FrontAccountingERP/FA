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
$page_security = 'SA_BACKUP';

$path_to_root="..";
include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/admin/db/maintenance_db.inc");

if (get_post('view')) {
	if (!get_post('backups')) {
		display_error(_('Select backup file first.'));
	} else {
		$filename = BACKUP_PATH . clean_file_name(get_post('backups'));
		if (in_ajax()) 
			$Ajax->popup( $filename );
		else {
		    header('Content-type: text/plain');
    		header('Content-Length: '.filesize($filename));
			header("Content-Disposition: inline");
    		readfile($filename);
			exit();
		}
	}
};
if (get_post('download')) {
	if (get_post('backups')) {
		download_file(BACKUP_PATH . clean_file_name(get_post('backups')));
		exit;
	} else
		display_error(_("Select backup file first."));
}

page(_($help_context = "Backup and Restore Database"), false, false, '', '');

check_paths();

function check_paths()
{
	if (!file_exists(BACKUP_PATH)) {
		display_error (_("Backup paths have not been set correctly.") 
			._("Please contact System Administrator.")."<br>" 
			. _("cannot find backup directory") . " - " . BACKUP_PATH . "<br>");
		end_page();
		exit;
	}
}

function generate_backup($conn, $ext='no', $comm='')
{
	$filename = db_backup($conn, $ext, $comm);
	if ($filename)
		display_notification(_("Backup successfully generated."). ' '
			. _("Filename") . ": " . $filename);
	else
		display_error(_("Database backup failed."));
	
	return $filename;
}


function get_backup_file_combo()
{
	global $path_to_root, $Ajax;
	
	$ar_files = array();
    default_focus('backups');
    $dh = opendir(BACKUP_PATH);
	while (($file = readdir($dh)) !== false)
		$ar_files[] = $file;
	closedir($dh);

    rsort($ar_files);
	$opt_files = "";
    foreach ($ar_files as $file)
		if (preg_match("/.sql(.zip|.gz)?$/", $file))
    		$opt_files .= "<option value='$file'>$file</option>";

	$selector = "<select name='backups' size=2 style='height:160px;min-width:230px'>$opt_files</select>";

	$Ajax->addUpdate('backups', "_backups_sel", $selector);
	$selector = "<span id='_backups_sel'>".$selector."</span>\n";

	return $selector;
}

function compress_list_row($label, $name, $value=null)
{
	$ar_comps = array('no'=>_("No"));

    if (function_exists("gzcompress"))
    	$ar_comps['zip'] = "zip";
    if (function_exists("gzopen"))
    	$ar_comps['gzip'] = "gzip";

	echo "<tr><td class='label'>$label</td><td>";
	echo array_selector('comp', $value, $ar_comps);
	echo "</td></tr>";
}

function download_file($filename)
{
    if (empty($filename) || !file_exists($filename))
    {
		display_error(_('Select backup file first.'));
        return false;
    }
    $saveasname = basename($filename);
    header('Content-type: application/octet-stream');
   	header('Content-Length: '.filesize($filename));
   	header('Content-Disposition: attachment; filename="'.$saveasname.'"');
    readfile($filename);

    return true;
}

$db_name = $_SESSION["wa_current_user"]->company;
$conn = $db_connections[$db_name];
$backup_name = clean_file_name(get_post('backups'));
$backup_path = BACKUP_PATH . $backup_name;

if (get_post('creat')) {
	generate_backup($conn, get_post('comp'), get_post('comments'));
	$Ajax->activate('backups');
};

if (get_post('restore')) {
	if ($backup_name) {
		if (db_import($backup_path, $conn))
			display_notification(_("Restore backup completed."));
		refresh_sys_prefs(); // re-read system setup
	} else
		display_error(_("Select backup file first."));
}

if (get_post('deldump')) {
	if ($backup_name) {
		if (unlink($backup_path)) {
			display_notification(_("File successfully deleted.")." "
					. _("Filename") . ": " . $backup_name);
			$Ajax->activate('backups');
		}
		else
			display_error(_("Can't delete backup file."));
	} else
		display_error(_("Select backup file first."));
}

if (get_post('upload'))
{
	$tmpname = $_FILES['uploadfile']['tmp_name'];
	$fname = trim(basename($_FILES['uploadfile']['name']));

	if ($fname) {
		if (!preg_match("/\.sql(\.zip|\.gz)?$/", $fname))
			display_error(_("You can only upload *.sql backup files"));
		elseif (is_uploaded_file($tmpname)) {
			rename($tmpname, BACKUP_PATH . $fname);
			display_notification(_("File uploaded to backup directory"));
			$Ajax->activate('backups');
		} else
			display_error(_("File was not uploaded into the system."));
	} else
		display_error(_("Select backup file first."));

}
//-------------------------------------------------------------------------------
start_form(true, true);
start_outer_table(TABLESTYLE2);
table_section(1);
table_section_title(_("Create backup"));
	textarea_row(_("Comments:"), 'comments', null, 30, 8);
	compress_list_row(_("Compression:"),'comp');
	vertical_space("height='20px'");
	submit_row('creat',_("Create Backup"), false, "colspan=2 align='center'", '', 'process');
table_section(2);
table_section_title(_("Backup scripts maintenance"));

	start_row();
	echo "<td style='padding-left:20px' align='left'>".get_backup_file_combo()."</td>";
	echo "<td style='padding-left:20px' valign='top'>";
	start_table();
	submit_row('view',_("View Backup"), false, '', '', false);
	submit_row('download',_("Download Backup"), false, '', '', false);
	submit_row('restore',_("Restore Backup"), false, '','', 'process');
	submit_js_confirm('restore',_("You are about to restore database from backup file.\nDo you want to continue?"));

	submit_row('deldump', _("Delete Backup"), false, '','', true);
	// don't use 'delete' name or IE js errors appear
	submit_js_confirm('deldump', sprintf(_("You are about to remove selected backup file.\nDo you want to continue ?")));
	end_table();
	echo "</td>";
	end_row();
start_row();
echo "<td style='padding-left:20px' align='left'><input name='uploadfile' type='file'></td>";
	submit_cells('upload',_("Upload file"),"style='padding-left:20px'", '', true);
end_row();
end_outer_table();

end_form();

end_page();
?>
