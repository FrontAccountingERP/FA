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
include_once($path_to_root."/includes/packages.inc");

if ($use_popup_windows) {
	$js = get_js_open_window(900, 500);
}
page(_($help_context = "Install/Activate extensions"));

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/admin/db/company_db.inc");
include_once($path_to_root . "/admin/db/maintenance_db.inc");
include_once($path_to_root . "/includes/ui.inc");

simple_page_mode(true);
//---------------------------------------------------------------------------------------------
// Check third-party extension parameters
//
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
		display_error(_("You have to select extension file to upload"));
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

	if ($selected_id != -1 && $extensions[$id]['type'] != 'extension'
		|| (isset($extensions[$id]['tabs']) && count($extensions[$id]['tabs']))) {
		display_error(_('Module installation support is not implemented.'));
		return;
	}

	$extensions[$id]['name'] = $_POST['name'];
	$extensions[$id]['package'] = '';
	$extensions[$id]['version'] = '';
	$extensions[$id]['active'] = check_value('active');
	$entry = $selected_id == -1 ? array() : $extensions[$id]['entries'][0];
	
	$entry['tab_id'] = $_POST['tab'];
	$entry['title'] = $_POST['title'];
	$entry['section'] = 2; // menu section aka module

	// Only simple plugin type extensions can be installed manually.
	$extensions[$id]['type'] = 'extension';
	$extensions[$id]['path'] = 'modules/'.$_POST['path'];
	$directory = $path_to_root . "/modules/" . $_POST['path'];
	if (!file_exists($directory))
	{
		mkdir($directory);
	}
	if (is_uploaded_file($_FILES['uploadfile']['tmp_name']))
	{
		$entry['url'] = $_FILES['uploadfile']['name'];
		$file1 = $_FILES['uploadfile']['tmp_name'];
		$file2 = $directory . "/".$_FILES['uploadfile']['name'];
		if (file_exists($file2))
			unlink($file2);
		move_uploaded_file($file1, $file2);
	}
	else
		$entry['url'] = get_post('filename');

	if (is_uploaded_file($_FILES['uploadfile2']['tmp_name']))
	{
		$file1 = $_FILES['uploadfile2']['tmp_name'];
		$db_name = $_SESSION["wa_current_user"]->company;
		db_import($file1, $db_connections[$db_name]);
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
	$exttext = file_get_contents($path_to_root.'/'.$extensions[$id]['path'].'/'.$entry['url']);
	$area = 'SA_OPEN';
	if (preg_match('/.*\$page_security\s*=\s*[\'"]([^\'"]*)/', $exttext, $match)) {
		$area = trim($match[1]);
	}
	$entry['access'] = $area;

	$extensions[$id]['entries'] = array($entry);
	
	if ($selected_id == -1) 
	{
		$next_extension_id++;
	}
	if (!update_extensions($extensions))
		return false;
	return true;
}

function handle_delete($id)
{
	global $path_to_root;
	
	$extensions = get_company_extensions();

	if ($extensions[$id]['package'] != '') {
		if (!uninstall_package($extensions[$id]['package']))
			return false;
	} else {

		$dirname = $extensions[$id]['path'];
		if ($dirname) {
			$dirname = $path_to_root.'/'.$dirname;
			flush_dir($dirname, true);
			rmdir($dirname);
		}
	}
	unset($extensions[$id]);
	if (update_extensions($extensions)) {
		display_notification(_("Selected extension has been successfully deleted"));
	}
	return true;
}
//
// Helper for formating menu tabs/entries to be displayed in extension table
//
function fmt_titles($defs)
{
		if (!$defs) return '';
		foreach($defs as $def) {
			$str[] = access_string($def['title'], true);
		}
		return implode('<br>', array_values($str));
}
//---------------------------------------------------------------------------------------------
//
// Display list of all extensions - installed and available from repository
//
function display_extensions()
{

	div_start('ext_tbl');
	start_table(TABLESTYLE);

	$th = array(_("Extension"),_("Modules provided"), _("Options provided"),
		 _("Installed"), _("Available"),  "", "");
	table_header($th);

	$k = 0;
	$mods = get_extensions_list('extension');

	foreach($mods as $pkg_name => $ext)
	{
		$available = @$ext['available'];
		$installed = @$ext['version'];
		$id = @$ext['local_id'];
		$is_mod = $ext['type'] == 'module';

		$entries = fmt_titles(@$ext['entries']);
		$tabs = fmt_titles(@$ext['tabs']);

		alt_table_row_color($k);
//		label_cell(is_array($ext['Descr']) ? $ext['Descr'][0] : $ext['Descr']);
		label_cell($available ? get_package_view_str($pkg_name, $ext['name']) : $ext['name']);
		label_cell($tabs);
		label_cell($entries);

		label_cell($id === null ? _("None") :
			($available && $installed ? $installed : _("Unknown")));
		label_cell($available ? $available : _("None"));

		if (!$available && $ext['type'] == 'extension' && !count(@$ext['tabs']))	// third-party plugin
			button_cell('Edit'.$id, _("Edit"), _('Edit third-party extension parameters.'), 
				ICON_EDIT);
		elseif (check_pkg_upgrade($installed, $available)) // outdated or not installed extension in repo
			button_cell('Update'.$pkg_name, $installed ? _("Update") : _("Install"),
				_('Upload and install latest extension package'), ICON_DOWN);
		else
			label_cell('');

		if ($id !== null) {
			delete_button_cell('Delete'.$id, _('Delete'));
			submit_js_confirm('Delete'.$id, 
				sprintf(_("You are about to remove package \'%s\'.\nDo you want to continue ?"), 
					$ext['name']));
		} else
			label_cell('');

		end_row();
	}

	end_table(1);

	submit_center_first('Refresh', _("Update"), '', null);
	submit_center_last('Add', _("Add third-party extension"), '', false);

	div_end();
}
//---------------------------------------------------------------------------------
//
// Get all installed extensions and display
// with current status stored in company directory.
//
function company_extensions($id)
{
	start_table(TABLESTYLE);
	
	$th = array(_("Extension"),_("Modules provided"), _("Options provided"), _("Active"));
	
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
		if ($mod['type'] != 'extension') continue;
   		alt_table_row_color($k);
		label_cell($mod['name']);
		$entries = fmt_titles(@$mod['entries']);
		$tabs = fmt_titles(@$mod['tabs']);
		label_cell($tabs);
		label_cell($entries);

		check_cells(null, 'Active'.$i, @$mod['active'] ? 1:0, 
			false, false, "align='center'");
		end_row();
	}

	end_table(1);
	submit_center('Refresh', _('Update'), true, false, 'default');
}

//---------------------------------------------------------------------------------------------
//
// Third-party plugin installation
//
function display_ext_edit($selected_id)
{
	global $Mode;

	$extensions = get_company_extensions();

	start_table(TABLESTYLE2);

	if ($selected_id != -1 && $extensions[$selected_id]['type'] == 'extension')
	{
		if ($Mode == 'Edit') {
			$mod = $extensions[$selected_id];
			$entry = $mod['entries'][0];

			$_POST['name'] = $mod['name'];
			$_POST['tab']  = $entry['tab_id'];
			$_POST['title'] = $entry['title'];
			$_POST['path'] = substr(dirname($mod['path']), 9); //strip '/modules/'
			$_POST['filename'] = basename($entry['url']);
			$_POST['acc_file'] = @$mod['acc_file'] ? basename($mod['acc_file']) : null;
			hidden('filename', $_POST['filename']);
			hidden('acc_file', $_POST['acc_file']);
		}
		hidden('selected_id', $selected_id);
	}
	text_row_ex(_("Name"), 'name', 30);
	text_row_ex(_("Subfolder (in modules directory)"), 'path', 20);

	tab_list_row(_("Menu Tab"), 'tab', null, true);
	text_row_ex(_("Menu Link Text"), 'title', 30);

	record_status_list_row(_("Default status"), 'active');

	file_row(_("Extension File"), 'uploadfile');
	file_row(_("Access Levels File"), 'uploadfile3');
	file_row(_("SQL File"), 'uploadfile2');

	end_table(0);
	display_note(_("Select your extension PHP files from your local harddisk."), 0, 1);
	echo '<center>';
	submit_add_or_update($selected_id == -1, '', 'both');
	echo '</center>';
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
	handle_delete($selected_id);
	$Mode = 'RESET';
}
if (get_post('Refresh')) {
	$exts = get_company_extensions(get_post('extset')); //
	$comp = get_post('extset');
	
	foreach($exts as $i => $ext) {
		if ($ext['package'] && ($ext['active'] ^ check_value('Active'.$i))) {
			$pkg = new package($ext['package'].'-'.$ext['version'].'.pkg');
			$pkg->support(check_value('Active'.$i) ? 'activate':'deactivate', $comp);
		}
		$exts[$i]['active'] = check_value('Active'.$i);
	}
	write_extensions($exts, get_post('extset'));
	if (get_post('extset') == user_company())
		$installed_extensions = $exts;
	display_notification(_('Current active extensions set has been saved.'));
}

if ($id = find_submit('Update', false))
	install_extension($id);

if ($Mode == 'RESET')
{
	$selected_id = -1;
	unset($_POST);
}

//---------------------------------------------------------------------------------------------
start_form(true);
if (list_updated('extset'))
	$Ajax->activate('_page_body');

$set = get_post('extset', -1);

if (isset($_GET['popup']) || get_post('Add') || $Mode == 'Edit' 
		|| $Mode == 'ADD_ITEM' || $Mode == 'UPDATE_ITEM') 
{
	display_ext_edit($selected_id);
}
else { 
	echo "<center>" . _('Extensions:') . "&nbsp;&nbsp;";
	echo extset_list('extset', null, true);
	echo "</center><br>";

	if ($set == -1) 
		display_extensions();
	else 
		company_extensions($set);
}

//---------------------------------------------------------------------------------------------
end_form();

end_page();
?>