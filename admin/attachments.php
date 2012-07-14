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
$path_to_root="..";
$page_security = 'SA_ATTACHDOCUMENT';

include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/data_checks.inc");
include_once($path_to_root . "/admin/db/attachments_db.inc");

if (isset($_GET['vw']))
	$view_id = $_GET['vw'];
else
	$view_id = find_submit('view');
if ($view_id != -1)
{
	$row = get_attachment($view_id);
	if ($row['filename'] != "")
	{
		if(in_ajax()) {
			$Ajax->popup($_SERVER['PHP_SELF'].'?vw='.$view_id);
		} else {
			$type = ($row['filetype']) ? $row['filetype'] : 'application/octet-stream';	
    		header("Content-type: ".$type);
    		header('Content-Length: '.$row['filesize']);
	    	//if ($type == 'application/octet-stream')
    		//	header('Content-Disposition: attachment; filename='.$row['filename']);
    		//else
	 			header("Content-Disposition: inline");
	    	echo file_get_contents(company_path(). "/attachments/".$row['unique_name']);
    		exit();
		}
	}	
}
if (isset($_GET['dl']))
	$download_id = $_GET['dl'];
else
	$download_id = find_submit('download');

if ($download_id != -1)
{
	$row = get_attachment($download_id);
	if ($row['filename'] != "")
	{
		if(in_ajax()) {
			$Ajax->redirect($_SERVER['PHP_SELF'].'?dl='.$download_id);
		} else {
			$type = ($row['filetype']) ? $row['filetype'] : 'application/octet-stream';	
    		header("Content-type: ".$type);
	    	header('Content-Length: '.$row['filesize']);
    		header('Content-Disposition: attachment; filename='.$row['filename']);
    		echo file_get_contents(company_path()."/attachments/".$row['unique_name']);
	    	exit();
		}
	}	
}

$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(800, 500);
page(_($help_context = "Attach Documents"), false, false, "", $js);

simple_page_mode(true);
//----------------------------------------------------------------------------------------
if (isset($_GET['filterType'])) // catch up external links
	$_POST['filterType'] = $_GET['filterType'];
if (isset($_GET['trans_no']))
	$_POST['trans_no'] = $_GET['trans_no'];

if ($Mode == 'ADD_ITEM' || $Mode == 'UPDATE_ITEM')
{
	if (!$_POST['trans_no'])
		display_error(_("No transaction has been selected."));
	elseif ($Mode == 'ADD_ITEM' && (!isset($_FILES['filename']) || $_FILES['filename']['size'] == 0))
		display_error(_("Select attachment file."));
	else {
		//$content = base64_encode(file_get_contents($_FILES['filename']['tmp_name']));
		$tmpname = $_FILES['filename']['tmp_name'];

		$dir =  company_path()."/attachments";
		if (!file_exists($dir))
		{
			mkdir ($dir,0777);
			$index_file = "<?php\nheader(\"Location: ../index.php\");\n?>";
			$fp = fopen($dir."/index.php", "w");
			fwrite($fp, $index_file);
			fclose($fp);
		}
		// file name compatible with POSIX
		// protect against directory traversal
		if ($Mode == 'UPDATE_ITEM')
		{
			$unique_name = preg_replace('/[^a-zA-Z0-9.\-_]/', '', $_POST['unique_name']);
			if ($Mode == 'UPDATE_ITEM' && file_exists($dir."/".$unique_name))
				unlink($dir."/".$unique_name);
		}
		else
			$unique_name = uniqid('');
		move_uploaded_file($tmpname, $dir."/".$unique_name);

		//save the file
		$filename = basename($_FILES['filename']['name']);
		$filesize = $_FILES['filename']['size'];
		$filetype = $_FILES['filename']['type'];

		if ($Mode == 'ADD_ITEM')
		{
			add_attachment($_POST['filterType'], $_POST['trans_no'], $_POST['description'],
				$filename, $unique_name, $filesize, $filetype);
			display_notification(_("Attachment has been inserted.")); 
		}
		else
		{
			update_attachment($selected_id, $_POST['filterType'], $_POST['trans_no'], $_POST['description'],
				$filename, $unique_name, $filesize, $filetype); 
			display_notification(_("Attachment has been updated.")); 
		}
	}
	$Mode = 'RESET';
}

if ($Mode == 'Delete')
{
	$row = get_attachment($selected_id);
	$dir =  company_path()."/attachments";
	if (file_exists($dir."/".$row['unique_name']))
		unlink($dir."/".$row['unique_name']);
	delete_attachment($selected_id);	
	display_notification(_("Attachment has been deleted.")); 
	$Mode = 'RESET';
}

if ($Mode == 'RESET')
{
	unset($_POST['trans_no']);
	unset($_POST['description']);
	$selected_id = -1;
}

function viewing_controls()
{
	global $selected_id;
	
    start_table(TABLESTYLE_NOBORDER);

	start_row();
	systypes_list_cells(_("Type:"), 'filterType', null, true);
	if (list_updated('filterType'))
		$selected_id = -1;;

	end_row();
    end_table(1);

}

function display_rows($type)
{
	$rows = get_attached_documents($type);
	$th = array(_("#"), _("Description"), _("Filename"), _("Size"), _("Filetype"), _("Date Uploaded"), "", "", "", "");
	
	start_table(TABLESTYLE);
	table_header($th);
	$k = 0;
	while ($row = db_fetch($rows))
	{
		alt_table_row_color($k);
		
		label_cell(get_trans_view_str($type, $row['trans_no']));
		label_cell($row['description']);
		label_cell($row['filename']);
		label_cell($row['filesize']);
		label_cell($row['filetype']);
		label_cell(sql2date($row['tran_date']));
 		edit_button_cell("Edit".$row['id'], _("Edit"));
 		button_cell("view".$row['id'], _("View"), false, ICON_VIEW);
 		button_cell("download".$row['id'], _("Download"), false, ICON_DOWN);
 		delete_button_cell("Delete".$row['id'], _("Delete"));
    	end_row();
	}	
	end_table(1);
}

//----------------------------------------------------------------------------------------

start_form(true);

viewing_controls();

display_rows($_POST['filterType']);


start_table(TABLESTYLE2);

if ($selected_id != -1)
{
	if ($Mode == 'Edit')
	{
		$row = get_attachment($selected_id);
		$_POST['trans_no']  = $row["trans_no"];
		$_POST['description']  = $row["description"];
		hidden('trans_no', $row['trans_no']);
		hidden('unique_name', $row['unique_name']);
		label_row(_("Transaction #"), $row['trans_no']);
	}	
	hidden('selected_id', $selected_id);
}
else
	text_row_ex(_("Transaction #").':', 'trans_no', 10);
text_row_ex(_("Description").':', 'description', 40);
file_row(_("Attached File") . ":", 'filename', 'filename');

end_table(1);

submit_add_or_update_center($selected_id == -1, '', 'process');

end_form();

end_page();

?>
