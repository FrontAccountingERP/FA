<?php

$page_security = 15;

$path_to_root="..";
include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/admin/db/maintenance_db.inc");

$valid_paths = valid_paths();
//$valid_paths = '';
if ($valid_paths != "")
{
	page(_("Backup and Restore Database - Error"));
	display_error (_("Backup paths have not been set correctly.") ."&nbsp;&nbsp;&nbsp;" . _("Please contact System Administrator.") . "<br>" .$valid_paths);
	end_page();
	exit;
}

$db_name = $_SESSION["wa_current_user"]->company;
handle_form($db_connections[$db_name]);

page(_("Backup and Restore Database"), false, false, '', '', true);

//-------------------------------------------------------------------------------
start_form(true, true);

$cmb = get_backup_file_combo();
$compr = get_compr_combo();

echo "
	<script language='javascript'>
	function createBackup() {
		ext = document.forms[0].cmb_comp.options[document.forms[0].cmb_comp.selectedIndex].value
		comm = document.forms[0].comments.value
		document.location.replace('backups.php?c=g&comp='+ext+'&comm='+comm)
	}
	function restoreBackup() {
		pFilename = document.forms[0].cmb_backups.options[document.forms[0].cmb_backups.selectedIndex].value
		document.location.replace('backups.php?c=r&fn='+pFilename)
	}
	function viewBackup() {
		pFilename = document.forms[0].cmb_backups.options[document.forms[0].cmb_backups.selectedIndex].value
		var ext = pFilename.substr(pFilename.lastIndexOf('.') + 1)
		if (ext != 'sql') {
			alert('" . _('This extension can not be be viewed: ') . "' + ext)
			return
		}
		window.open('" . BACKUP_PATH . "'+pFilename, '', 'toolbar=no,scrollbars=yes')
	}
	function deleteBackup() {
		pFilename = document.forms[0].cmb_backups.options[document.forms[0].cmb_backups.selectedIndex].value
		if (!confirm('" . _("Are you sure you want to delete the backup file - ") . "'+pFilename+'?'))
			return
		document.location.replace('backups.php?c=df&fn='+pFilename)
	}
	function downloadBackup() {
		pFilename = document.forms[0].cmb_backups.options[document.forms[0].cmb_backups.selectedIndex].value
		document.location.replace('backups.php?c=ds&fn='+pFilename)
		Behaviour.apply();
	}
	function uploadBackup() {
		if (document.forms[0].uploadfile.value=='') {
			alert('" . _("Please select a file to upload.") . "')
			return
		}
		document.forms[0].action='backups.php?c=u&fn=' + document.forms[0].uploadfile.value
		document.forms[0].submit()
	}
	</script>
	<center>
	<table cellpadding=2 cellspacing=2 border=0>
	<tr>
		<td style='padding-right:30px'>" . _("Backup scripts") . "</td>
	</tr>
	<tr>
		<td style='padding-right:30px'>$cmb</td>
		<script language='javascript'>
			if (document.forms[0].cmb_backups.options.length!=0) document.forms[0].cmb_backups.selectedIndex=0
		</script>
		<td>
			<table height=160 cellpadding=0 cellspacing=0 border=0>
			<tr><td><input onclick='javascript:createBackup()' type='button' style='width:150px' value='". _("Create Backup") . "'></td><td style='padding-left:20px'>" . _("Compression") . "&nbsp;&nbsp;$compr</td></tr>
			<tr><td><input onclick='javascript:restoreBackup()' type='button' style='width:150px' value='". _("Restore Backup") . "'></td><td>&nbsp;</td></tr>
			<tr><td><input onclick='javascript:viewBackup()' type='button' style='width:150px' value='". _("View Backup") . "'></td><td>&nbsp;</td></tr>
			<tr><td><input onclick='javascript:deleteBackup()' type='button' style='width:150px' value='". _("Delete Backup") . "'></td><td>&nbsp;</td></tr>
			<tr><td><input onclick='javascript:downloadBackup()' type='button' style='width:150px' value='". _("Download Backup") . "'></td><td>&nbsp;</td></tr>
			<tr><td><input onclick='javascript:uploadBackup()' type='button' style='width:150px' value='". _("Upload Backup"). "'></td>
				<td style='padding-left:20px'><input name='uploadfile' type='file'></td>
			</tr>
			</table>
		</td>
	</tr>
	<tr>
		<td style='padding-right:30px'>" . _("Comments") . " (" . _("Create Backup") . ")</td>
	</tr>
	<tr>
		<td style='padding-right:30px'><textarea rows=4 cols=30 name='comments'></textarea></td>
	</tr>
	</table></center>";

end_form();

//-------------------------------------------------------------------------------------------------

end_page();


function handle_form($conn)
{
if(isset($_GET['c']))
	switch($_GET['c'])
	{
		case 'g':	//Generate Only
			$filename = generate_backup($conn, $_GET['comp'], $_GET['comm']);
			header("Location: backups.php?c=gs&fn=" . urlencode($filename));
			break;

		case 'gd':	//Generate and download
			$filename = generate_backup($conn);
			header("Location: backups.php?c=ds&fn=" . urlencode($filename));
			break;

		case 'd':	//Download the file
			download_file(BACKUP_PATH . $_GET['fn']);
			exit;

		case 'df':	//Delete the file
			$filename = $_GET['fn'];
			@unlink(BACKUP_PATH . $filename);
			header("Location: backups.php?c=dff&fn=" . urlencode($filename));
			break;

		case 'dff':
			$msg = _("File successfully deleted.")." ";
			$msg .= _("Filename") . " = " . $_GET['fn'];
			display_notification($msg);
			break;

		case 'ds':	//Write JS script to open download window
			$filename = urlencode($_GET['fn']);
			display_notification(_("Backup is being downloaded..."));
			
			add_js_source("<script language='javascript'>
			function download_file() {location.href ='backups.php?c=d&fn=$filename'}; 
				Behaviour.addLoadEvent(download_file);
			</script>");
			break;

		case 'gs':	//Print backup success message
			$msg = _("Backup successfully generated."). ' ';
			$msg .= _("Filename") . " = " . $_GET['fn'];
			display_notification($msg);
			break;

		case 'r':	//Restore backup
			$filename=$_GET['fn'];
			if( restore_backup(BACKUP_PATH . $filename, $conn) )
				header("Location: backups.php?c=rs&fn=" . urlencode($filename));
			break;

		case 'rs':	//Print restore success message
			display_notification(_("Restore backup completed."));
			break;

		case 'u':
			$filename = $_FILES['uploadfile']['tmp_name'];
			if (is_uploaded_file ($filename))
			{
				if( restore_backup($filename, $conn) )
					display_notification(_("Uploaded file has been restored."));
				else
					display_error(_("Database restore failed."));
			}
			else
			{
				display_error(_("Backup was not uploaded into the system."));
			}
		}
}

function generate_backup($conn, $ext='no', $comm='')
{
	if ($conn['tbpref'] != "")
		$filename = $conn['dbname'] . "_" . $conn['tbpref'] . date("Ymd_Hi") . ".sql";
	else
		$filename = $conn['dbname'] . "_" . date("Ymd_Hi") . ".sql";

	$filename = db_export($conn, $filename, $ext, $comm);

	return $filename;
}

function restore_backup($filename, $conn)
{
	return db_import($filename, $conn);
}

function get_backup_file_combo()
{
	global $path_to_root;
	$ar_files = array();
    default_focus('cmb_backups');
    $dh = opendir(BACKUP_PATH);
	while (($file = readdir($dh)) !== false)
		$ar_files[] = $file;
	closedir($dh);

    rsort($ar_files);
	$opt_files = "";
    foreach ($ar_files as $file)
    	if (strpos($file, ".sql") || strpos($file, ".sql"))
    		$opt_files .= "<option value='$file'>$file</option>";

	return "<select name='cmb_backups' size=2 style='height:160px;width:230px'>$opt_files</select>";
}

function get_compr_combo()
{
	$ar_comps = array();

	$ar_comps[] = _("No");
    if (function_exists("gzcompress"))
    	$ar_comps[] = "zip";
    if (function_exists("gzopen"))
    	$ar_comps[] = "gzip";
	$opt_comps = "";
    foreach ($ar_comps as $file)
    	$opt_comps .= "<option value='$file'>$file</option>";

	return "<select name='cmb_comp'>$opt_comps</select>";
}

function download_file($filename)
{
    if (empty($filename) || !file_exists($filename))
    {
        return false;
    }
    $saveasname = basename($filename);
    header('Content-type: application/octet-stream');
    header('Content-Length: '.filesize($filename));
    header('Content-Disposition: attachment; filename="'.$saveasname.'"');
    readfile($filename);
    return true;
}

function valid_paths()
{
	global $path_to_root;

	$st = "";
	if (!file_exists(BACKUP_PATH))
		$st .= "&nbsp;&nbsp;&nbsp;-&nbsp;" . _("cannot find backup directory") . " - " . BACKUP_PATH . "<br>";
	return $st;
}

?>