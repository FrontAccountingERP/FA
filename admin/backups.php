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
$msg = handle_form($db_connections[$db_name]);

page(_("Backup and Restore Database"));

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
	<tr><td colspan=2 style='color:darkred'><b>$msg</b>&nbsp;</td></tr>
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
	global $path_to_root;
	//Generate Only
	if (isset($_GET['c']))
	{
		if ($_GET['c']=='g')
		{
			$filename = generate_backup($conn, $_GET['comp'], $_GET['comm']);
			header("Location: backups.php?c=gs&fn=" . urlencode($filename));
			return "";
		}
		//Generate and download
		if ($_GET['c']=='gd')
		{
			$filename = generate_backup($conn);
			header("Location: backups.php?c=ds&fn=" . urlencode($filename));
			return "";
		}
		//Download the file
		if ($_GET['c']=='d')
		{
			download_file(BACKUP_PATH . $_GET['fn']);
			exit;
		}
		//Delete the file
		if ($_GET['c']=='df')
		{
			$filename = $_GET['fn'];
			@unlink(BACKUP_PATH . $filename);
			header("Location: backups.php?c=dff&fn=" . urlencode($filename));
			return "";
		}
		if ($_GET['c']=='dff')
		{
			$msg = _("File successfully deleted.")."&nbsp;&nbsp;&nbsp;";
			$msg .= _("Filename") . " = " . $_GET['fn'];
			return $msg;
		}
		//Write JS script to open download window
		if ($_GET['c']=='ds')
		{
			$filename = urlencode($_GET['fn']);
			$msg = _("Backup is being downloaded...");
			$msg .= "<script language='javascript'>";
			$msg .= "function download_file() {location.href ='backups.php?c=d&fn=$filename'}; window.onload=download_file";
			$msg .= "</script>";
			return $msg;
		}
		//Print backup success message
		if ($_GET['c']=='gs')
		{
			$msg = _("Backup successfully generated.")."&nbsp;&nbsp;&nbsp;";
			$msg .= _("Filename") . " = " . $_GET['fn'];
			return $msg;
		}
		//Restore backup
		if ($_GET['c']=='r')
		{
			$filename=$_GET['fn'];
			restore_backup(BACKUP_PATH . $filename, $conn);
			header("Location: backups.php?c=rs&fn=" . urlencode($filename));
			return "";
		}
		//Print restore success message
		if ($_GET['c']=='rs')
		{
			$msg = _("Restore backup completed.")."&nbsp;&nbsp;&nbsp;";
			return $msg;
		}

		if ($_GET['c']=='u')
		{
			$filename = $_FILES['uploadfile']['tmp_name'];
			if (is_uploaded_file ($filename))
			{
				restore_backup($filename, $conn);
				$msg = _("Uploaded file has been restored.");
			}
			else
			{
				$msg = _("Backup was not uploaded into the system.");
			}
			return $msg;
		}
	}
	return "";
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
        return FALSE;
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