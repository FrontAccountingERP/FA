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

page(_($help_context = "Install/Activate extensions"));

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/admin/db/company_db.inc");
include_once($path_to_root . "/admin/db/maintenance_db.inc");
include_once($path_to_root . "/includes/ui.inc");

simple_page_mode(true);

//---------------------------------------------------------------------------------------------
function update_extensions($extensions) {
	global $db_connections;
	
	if (!write_extensions($extensions)) {
		display_notification(_("Cannot update system extensions list."));
		return false;
	}

	// update per company files
	$cnt = count($db_connections);
	for($i = 0; $i < $cnt; $i++) 
	{
		$newexts = $extensions;
		// update 'active' status 
		$exts = get_company_extensions($i);
		foreach ($exts as $key => $ext) 
		{
			if (isset($newexts[$key]))
				$newexts[$key]['active'] = $exts[$key]['active'];
		}
		if(!write_extensions($newexts, $i)) 
		{
			display_notification(sprintf(_("Cannot update extensions list for company '%s'."),
				$db_connections[$i]['name']));
		 return false;
		}
	}
	return true;
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
	global $path_to_root, $db_connections, $selected_id, $next_extension_id;

	$extensions = get_company_extensions();
	if (!check_data($selected_id, $extensions))
		return false;
	$id = $selected_id==-1 ? $next_extension_id : $selected_id;

	if ($selected_id != -1 && $extensions[$id]['type'] != 'plugin') {
		display_error(_('Module installation support is not implemented yet. You have to do it manually.'));
		return;
	}

	$extensions[$id]['tab'] = $_POST['tab'];
	$extensions[$id]['name'] = $_POST['name'];
	$extensions[$id]['path'] = $_POST['path'];
	$extensions[$id]['title'] = $_POST['title'];
	$extensions[$id]['active'] = check_value('active');

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

	// security area guess for plugins
	if ($extensions[$id]['type'] == 'plugin'){
		$exttext = file_get_contents($path_to_root.'/modules/'
			.$extensions[$id]['path'].'/'.$extensions[$id]['filename']);
		$area = 'SA_OPEN';
		if (preg_match('/.*\$page_security\s*=\s*[\'"]([^\'"]*)/', $exttext, $match)) {
			$area = trim($match[1]);
		} 
		$extensions[$id]['access'] = $area;
	}

	if ($selected_id == -1) 
	{
		$next_extension_id++;
	}
	if (!update_extensions($extensions))
		return false;
	return true;
}

function handle_delete()
{
	global  $path_to_root, $db_connections, $selected_id;
	
	$extensions = get_company_extensions();

	$id = $selected_id;

	$filename = $path_to_root
		. ($extensions[$id]['type']=='plugin' ? "/modules/": '/')
		. $extensions[$id]['path'];

	flush_dir($filename);
	rmdir($filename);
	unset($extensions[$id]);
	if (update_extensions($extensions))
		display_notification(_("Selected extension has been successfully deleted"));
	return true;
}

//---------------------------------------------------------------------------------------------

function display_extensions()
{
	global $table_style;

	start_table($table_style);
	$th = array(_("Name"),_("Tab"), _("Link text"), _("Folder"), _("Filename"), 
		_("Access extensions"),"", "");
	table_header($th);

	$k = 0;
	$mods = get_company_extensions();
	$mods = array_natsort($mods, null, 'name');

	foreach($mods as $i => $mod)
	{
		$is_mod = $mod['type'] == 'module';
   		alt_table_row_color($k);
		label_cell($mod['name']);
		label_cell( $is_mod ? 
			$mod['title'] : access_string($_SESSION['App']->applications[$mod['tab']]->name, true));
		$ttl = access_string($mod['title']);
		label_cell($ttl[0]);
		label_cell($mod['path']);
		label_cell($mod['filename']);
		label_cell(@$mod['acc_file']);
		if ($is_mod)
		{
			label_cell(''); // not implemented (yet)
		}
		else
		{
			edit_button_cell("Edit".$i, _("Edit"));
		}
			delete_button_cell("Delete".$i, _("Delete"));
		submit_js_confirm('Delete'.$i, _('You are about to delete this extension\nDo you want to continue?'));
		end_row();
	}

	end_table(1);
}

function company_extensions($id)
{
	global $table_style;

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
	$mods = array_natsort($mods, null, 'name');
	table_header($th);
	$k = 0;
	foreach($mods as $i => $mod)
	{
   		alt_table_row_color($k);
		label_cell($mod['name']);
		label_cell( $mod['type'] == 'module' ? 
			$mod['title'] : access_string($_SESSION['App']->applications[$mod['tab']]->name, true));
		$ttl = access_string($mod['title']);
		label_cell($ttl[0]);
		check_cells(null, 'Active'.$i, @$mod['active'] ? 1:0, 
			false, false, "align='center'");
		end_row();
	}

	end_table(1);
	submit_center('Update', _('Update'), true, false, 'default');
}

//---------------------------------------------------------------------------------------------

function display_ext_edit($selected_id)
{
	global $table_style2, $Mode;


	$extensions = get_company_extensions();

	start_table($table_style2);

	if ($selected_id != -1 && $extensions[$selected_id]['type'] == 'plugin')
	{
		if ($Mode == 'Edit') {
			$mod = $extensions[$selected_id];
			$_POST['tab']  = $mod['tab'];
			$_POST['name'] = $mod['name'];
			$_POST['title'] = $mod['title'];
			$_POST['path'] = $mod['path'];
			$_POST['filename'] = $mod['filename'];
			$_POST['acc_file'] = @$mod['acc_file'];
			hidden('filename', $_POST['filename']);
			hidden('acc_file', $_POST['acc_file']);
		}
		hidden('selected_id', $selected_id);
	}
	text_row_ex(_("Name"), 'name', 30);
	text_row_ex(_("Folder"), 'path', 20);

	tab_list_row(_("Menu Tab"), 'tab', null, true);
	text_row_ex(_("Menu Link Text"), 'title', 30);

	record_status_list_row(_("Default status"), 'active');

	file_row(_("Module File"), 'uploadfile');
	file_row(_("Access Levels Extensions"), 'uploadfile3');
	file_row(_("SQL File"), 'uploadfile2');

	end_table(0);
	display_note(_("Select your module PHP file from your local harddisk."), 0, 1);
	submit_add_or_update_center($selected_id == -1, '', 'both');
}

//---------------------------------------------------------------------------------------------
if ($Mode=='ADD_ITEM' || $Mode == 'UPDATE_ITEM') {
	if(handle_submit()) {
		if ($selected_id != -1)
			display_notification(_("Extension data has been updated."));
		else
			display_notification(_("Extension has been installed."));
	$Mode = 'RESET';
	}
}
if ($Mode == 'Delete')
{
	handle_delete();
	$Mode = 'RESET';
}
if (get_post('Update')) {
	$exts = get_company_extensions();
	foreach($exts as $i => $ext) {
		$exts[$i]['active'] = check_value('Active'.$i);
	}
	write_extensions($exts, get_post('extset'));
	if (get_post('extset') == user_company())
		$installed_extensions = $exts;
	display_notification(_('Current active extensions set has been saved.'));
}

if ($Mode == 'RESET')
{
	$selected_id = -1;
	unset($_POST);
}

//---------------------------------------------------------------------------------------------
start_form(true);
if (list_updated('extset'))
	$Ajax->activate('_page_body');

echo "<center>" . _('Extensions:') . "&nbsp;&nbsp;";
echo extset_list('extset', null, true);
echo "</center><br>";

$set = get_post('extset', -1);
if ($set == -1) {
	display_extensions();

	display_ext_edit($selected_id);
} else {
	company_extensions($set);
}
//---------------------------------------------------------------------------------------------
end_form();

end_page();

?>