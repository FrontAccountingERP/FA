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
$page_security = 'SA_CREATEMODULES';
$path_to_root="..";
include_once($path_to_root . "/includes/session.inc");

page(_("Install/Activate extensions"));

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/admin/db/company_db.inc");
include_once($path_to_root . "/admin/db/maintenance_db.inc");
include_once($path_to_root . "/includes/ui.inc");

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
function get_company_extensions($id = -1) {

	global $path_to_root;

	$file = $path_to_root.($id == -1 ? '' : '/company/'.$id).'/installed_extensions.php';
	$installed_extensions = array();
	if (is_file($file)) {
		include($file);
	}
	return $installed_extensions;
}

function check_data($id, $exts)
{
	if ($_POST['name'] == "") {
		display_error(_("Extension name cannot be empty."));
		return false;
	}
	foreach($exts as $n =>$ext) {
		if ($_POST['name'] == $ext['name'] && $id != $n) {
			display_error(_("Extension name have to be unique."));
			return false;
		}
	}

	if ($_POST['title'] == "") {
		display_error(_("Extension title cannot be empty."));
		return false;
	}
	if ($_POST['path'] == "") {
		display_error(_("Extension folder name cannot be empty."));
		return false;
	}
	if ($id == -1 && !is_uploaded_file($_FILES['uploadfile']['tmp_name'])) {
		display_error(_("You have to select plugin file to upload"));
		return false; 
	}
	return true;
}

//---------------------------------------------------------------------------------------------

function handle_submit()
{
	global $path_to_root, $db_connections, $selected_id;

	$extensions = get_company_extensions();
	if (!check_data($selected_id), $extensions)
		return false;

	$id = $_GET['id'];

	$extensions[$id]['tab'] = $_POST['tab'];
	$extensions[$id]['name'] = $_POST['name'];
	$extensions[$id]['path'] = $_POST['path'];
	$extensions[$id]['title'] = $_POST['title'];
	$extensions[$id]['active'] = $_POST['active'];

	// Currently we support only plugin extensions here.
	$extensions[$id]['type'] = 'plugin';
	$directory = $path_to_root . "/modules/" . $_POST['path'];
	if (!file_exists($directory))
	{
		mkdir($directory);
	}
	if (is_uploaded_file($_FILES['uploadfile']['tmp_name']))
	{
		$extensions[$id]['filename'] = $_FILES['uploadfile']['name'];
		$file1 = $_FILES['uploadfile']['tmp_name'];
		$file2 = $directory . "/".$_FILES['uploadfile']['name'];
		if (file_exists($file2))
			unlink($file2);
		move_uploaded_file($file1, $file2);
	}
	else
		$extensions[$id]['filename'] = get_post('filename');
	if (is_uploaded_file($_FILES['uploadfile2']['tmp_name']))
	{
		$file1 = $_FILES['uploadfile2']['tmp_name'];
		$file2 = $directory . "/".$_FILES['uploadfile2']['name'];
		if (file_exists($file2))
			unlink($file2);
		move_uploaded_file($file1, $file2);
		$db_name = $_SESSION["wa_current_user"]->company;
		db_import($file2, $db_connections[$db_name]);
	}
	
	if (is_uploaded_file($_FILES['uploadfile3']['tmp_name']))
	{
		$extensions[$id]['acc_file'] = $_FILES['uploadfile3']['name'];
		$file1 = $_FILES['uploadfile3']['tmp_name'];
		$file2 = $directory . "/".$_FILES['uploadfile3']['name'];
		if (file_exists($file2))
			unlink($file2);
		move_uploaded_file($file1, $file2);
	}
	else
		$extensions[$id]['acc_file'] = get_post('acc_file');
	
	if (!write_extensions($extensions))
		return false;
	return true;
}

//---------------------------------------------------------------------------------------------

function handle_delete()
{
	global  $path_to_root;
	
	$extensions = get_company_extensions();

	$id = $_GET['id'];

	$path = $extensions[$id]['path'];

	if ($extensions[$id]['type'] != 'plugin') {
		display_error(_('Module installation support is not implemented yet. You have to do it manually.'));
		return;
	}
	
	$filename = "$path_to_root/modules/$path";
	if ($h = opendir($filename))
	{
   		while (($file = readdir($h)) !== false)
   		{
   			if (is_file("$filename/$file"))
   		    	unlink("$filename/$file");
   		}
   		closedir($h);
	}
	rmdir($filename);

	unset($extensions[$id]);
	$mods = array_values($extensions);
	$extensions = $mods;

	if (!write_extensions($extensions))
		return;
	
	// should we also delete module form per company extension files?
	
	meta_forward($_SERVER['PHP_SELF']);
}

//---------------------------------------------------------------------------------------------

function display_extensions()
{
	global $table_style, $tabs;

	echo "
		<script language='javascript'>
		function deleteExtension(id, name) {
			if (!confirm('" . _("Are you sure you want to delete extension: ") . "'+name))
				return
			document.location.replace('inst_module.php?c=df&id='+id)
		}
		</script>";
	start_table($table_style);
	$th = array(_("Name"),_("Tab"), _("Link text"), _("Folder"), _("Filename"), 
		_("Access extensions"),"", "");
	table_header($th);

	$k = 0;
	$mods = get_company_extensions();
	$n = count($mods);
	for ($i = 0; $i < $n; $i++)
	{
		$is_mod = $mods[$i]['type'] == 'module';
   		alt_table_row_color($k);
		label_cell($mods[$i]['name']);
		label_cell( $is_mod ? $mods[$i]['title'] : $tabs[$mods[$i]['tab']]);
		$ttl = access_string($mods[$i]['title']);
		label_cell($ttl[0]);
		label_cell($mods[$i]['path']);
		label_cell($mods[$i]['filename']);
		label_cell(@$mods[$i]['acc_file']);
		$edit = _("Edit");
		$delete = _("Delete");
		if ($is_mod)
		{
			label_cell(''); // not implemented (yet)
			label_cell('');
		}
		else
		{
			if (user_graphic_links())
			{
				$edit = set_icon(ICON_EDIT, $edit);
				$delete = set_icon(ICON_DELETE, $delete);
			}
	    	label_cell("<a href='" . $_SERVER['PHP_SELF']. "?selected_id=$i'>$edit</a>");
			label_cell("<a href='javascript:deleteExtension(".$i.", \"" . $mods[$i]['name'] . "\")'>$delete</a>");
		}
		end_row();
	}

	end_table();
}

function company_extensions($id)
{
	global $table_style, $tabs;

	start_table($table_style);
	
	$th = array(_("Name"),_("Tab"), _("Link text"), _("Active"));
	
	// get all available extensions and display
	// with current status stored in company directory.

	$mods = get_company_extensions();
	$exts = get_company_extensions($id);
	foreach($mods as $key => $ins) {
		foreach($exts as $ext)
			if ($ext['name'] == $ins['name']) {
				$mods[$key]['active'] = @$ext['active'];
				continue 2;
			}
	}
	
	table_header($th);
	$k = 0;
	$n = count($mods);
	for ($i = 0; $i < $n; $i++)
	{
   		alt_table_row_color($k);
		label_cell($mods[$i]['name']);
		label_cell($mods[$i]['type'] == 'module' ? $mods[$i]['title'] : $tabs[$mods[$i]['tab']]);
		$ttl = access_string($mods[$i]['title']);
		label_cell($ttl[0]);
		check_cells(null, 'Active'.$i, @$mods[$i]['active'] ? 1:0, 
			false, false, "align='center'");
		end_row();
	}

	end_table(1);
	submit_center('Update', _('Update'), true, false, 'default');
}

//---------------------------------------------------------------------------------------------

function display_ext_edit($selected_id)
{
	global $table_style2;

	$extensions = get_company_extensions();
	if ($selected_id != -1)
		$n = $selected_id;
	else
		$n = count($extensions);


	echo "
		<script language='javascript'>
		function updateModule() {
			document.forms[0].action='inst_module.php?c=u&id=" . $n . "'
			document.forms[0].submit()
		}
		</script>";

	start_table($table_style2);

	if ($selected_id != -1 && $extensions[$selected_id]['type'] == 'plugin')
	{
		$mod = $extensions[$selected_id];
		$_POST['tab']  = $mod['tab'];
		$_POST['name'] = $mod['name'];
		$_POST['title'] = $mod['title'];
		$_POST['path'] = $mod['path'];
		$_POST['filename'] = $mod['filename'];
		$_POST['acc_file'] = @$mod['acc_file'];
		hidden('selected_id', $selected_id);
		hidden('filename', $_POST['filename']);
		hidden('acc_file', $_POST['acc_file']);
	}
	text_row_ex(_("Name"), 'name', 30);
	text_row_ex(_("Folder"), 'path', 20);

	tab_list_row(_("Menu Tab"), 'tab', null);
	text_row_ex(_("Menu Link Text"), 'title', 30);
	record_status_list_row(_("Default status"), 'active');

	label_row(_("Module File"), "<input name='uploadfile' type='file'>");
	label_row(_("Access Levels Extensions"), "<input name='uploadfile3' type='file'>");
	label_row(_("SQL File"), "<input name='uploadfile2' type='file'>");

	end_table(0);
	display_note(_("Select your module PHP file from your local harddisk."), 0, 1);

	echo "<center><input onclick='javascript:updateModule()' type='button' style='width:150px' value='". _("Save"). "'></center>";

}

//---------------------------------------------------------------------------------------------
if (get_post('Update')) {
	$exts = get_company_extensions();
	for($i = 0; $i < count($exts); $i++) {
		$exts[$i]['active'] = check_value('Active'.$i);
	}
	write_extensions($exts, get_post('extset'));
	if (get_post('extset') == user_company())
		$installed_extensions = $exts;
	display_notification(_('Current active extensions set has been saved.'));
}
elseif (isset($_GET['c']))
{
	if ($_GET['c'] == 'df')
	{
		handle_delete();
	}

	if ($_GET['c'] == 'u')
	{
		if (handle_submit())
		{
			if ($selected_id != -1)
				display_notification(_("Extension data has been updated."));
			else
				display_notification(_("Extension has been installed."));
		}
	}
}

//---------------------------------------------------------------------------------------------
start_form(true);
if (list_updated('extset'))
	$Ajax->activate('_page_body');

echo "<center>" . _('Extensions:') . "&nbsp;&nbsp;";
extset_list('extset', null, true);
echo "</center><br>";

$set = get_post('extset');

if ($set == -1) {
	display_extensions();

	hyperlink_no_params($_SERVER['PHP_SELF'], _("Add new extension"));

	display_ext_edit($selected_id);
} else {
	company_extensions($set);
}
//---------------------------------------------------------------------------------------------
end_form();

end_page();

?>