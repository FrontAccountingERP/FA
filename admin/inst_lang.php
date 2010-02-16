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
$page_security = 'SA_CREATELANGUAGE';
$path_to_root="..";
include_once($path_to_root . "/includes/session.inc");

page(_($help_context = "Install/Update Languages"));

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

function check_data()
{
	if ($_POST['code'] == "" || $_POST['name'] == "" || $_POST['encoding'] == "") {
		display_error(_("Language name, code nor encoding cannot be empty"));
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------------------------

function handle_submit()
{
	global $path_to_root, $installed_languages, $dflt_lang;

	if (!check_data())
		return false;

	$id = $_GET['id'];

	if ($_POST['dflt']) {
			$dflt_lang = $_POST['code'];
	}
	
	$installed_languages[$id]['code'] = $_POST['code'];
	$installed_languages[$id]['name'] = $_POST['name'];
	$installed_languages[$id]['encoding'] = $_POST['encoding'];
	$installed_languages[$id]['rtl'] = (bool)$_POST['rtl'];
	if (!write_lang())
		return false;
	$directory = $path_to_root . "/lang/" . $_POST['code'];
	if (!file_exists($directory))
	{
		mkdir($directory);
		mkdir($directory . "/LC_MESSAGES");
	}
	if (is_uploaded_file($_FILES['uploadfile']['tmp_name']))
	{
		$file1 = $_FILES['uploadfile']['tmp_name'];
		$file2 = $directory . "/LC_MESSAGES/".$_POST['code'].".po";
		if (file_exists($file2))
			unlink($file2);
		move_uploaded_file($file1, $file2);
	}
	if (is_uploaded_file($_FILES['uploadfile2']['tmp_name']))
	{
		$file1 = $_FILES['uploadfile2']['tmp_name'];
		$file2 = $directory . "/LC_MESSAGES/".$_POST['code'].".mo";
		if (file_exists($file2))
			unlink($file2);
		move_uploaded_file($file1, $file2);
	}
	return true;
}

//---------------------------------------------------------------------------------------------

function handle_delete()
{
	global  $path_to_root, $installed_languages, $dflt_lang;

	$id = $_GET['id'];

	$lang = $installed_languages[$id]['code'];
	$filename = "$path_to_root/lang/$lang/LC_MESSAGES";

	if ($lang == $dflt_lang ) { 
		// on delete set default to current.
		$dflt_lang = $_SESSION['language']->code;
	}
	
	unset($installed_languages[$id]);
	$installed_languages = array_values($installed_languages);

	if (!write_lang())
		return;

	$filename = "$path_to_root/lang/$lang";
	flush_dir($filename);
	rmdir($filename);

	meta_forward($_SERVER['PHP_SELF']);
}

//---------------------------------------------------------------------------------------------

function display_languages()
{
	global $table_style, $installed_languages, $dflt_lang;

	$lang = $_SESSION["language"]->code;

	echo "
		<script language='javascript'>
		function deleteLanguage(id) {
			if (!confirm('" . _("Are you sure you want to delete language no. ") . "'+id))
				return
			document.location.replace('inst_lang.php?c=df&id='+id)
		}
		</script>";
	start_table($table_style);
	$th = array(_("Language"), _("Name"), _("Encoding"), _("Right To Left"), _("Default"), "", "");
	table_header($th);

	$k = 0;
	$conn = $installed_languages;
	$n = count($conn);
	for ($i = 0; $i < $n; $i++)
	{
		if ($conn[$i]['code'] == $lang)
    		start_row("class='stockmankobg'");
    	else
    		alt_table_row_color($k);

		label_cell($conn[$i]['code']);
		label_cell($conn[$i]['name']);
		label_cell($conn[$i]['encoding']);
		if (isset($conn[$i]['rtl']) && $conn[$i]['rtl'])
			$rtl = _("Yes");
		else
			$rtl = _("No");
		label_cell($rtl);
		label_cell($dflt_lang == $conn[$i]['code'] ? _("Yes") :_("No"));
		$edit = _("Edit");
		$delete = _("Delete");
		if (user_graphic_links())
		{
			$edit = set_icon(ICON_EDIT, $edit);
			$delete = set_icon(ICON_DELETE, $delete);
		}
    	label_cell("<a href='" . $_SERVER['PHP_SELF']. "?selected_id=$i'>$edit</a>");
		label_cell($conn[$i]['code'] == $lang ? '' :
			"<a href='javascript:deleteLanguage(" . $i . ")'>$delete</a>");
		end_row();
	}

	end_table();
    display_note(_("The marked language is the current language which cannot be deleted."), 0, 0, "class='currentfg'");
}

//---------------------------------------------------------------------------------------------

function display_language_edit($selected_id)
{
	global $installed_languages, $table_style2, $dflt_lang;

	if ($selected_id != -1)
		$n = $selected_id;
	else
		$n = count($installed_languages);

	start_form(true);

	echo "
		<script language='javascript'>
		function updateLanguage() {
			document.forms[0].action='inst_lang.php?c=u&id=" . $n . "'
			document.forms[0].submit()
		}
		</script>";

	start_table($table_style2);

	if ($selected_id != -1)
	{
		$conn = $installed_languages[$selected_id];
		$_POST['code'] = $conn['code'];
		$_POST['name']  = $conn['name'];
		$_POST['encoding']  = $conn['encoding'];
		if (isset($conn['rtl']))
			$_POST['rtl']  = $conn['rtl'];
		else
			$_POST['rtl'] = false;
		$_POST['dflt'] = $dflt_lang == $conn['code'];
		hidden('selected_id', $selected_id);
	}
	text_row_ex(_("Language Code"), 'code', 20);
	text_row_ex(_("Language Name"), 'name', 20);
	text_row_ex(_("Encoding"), 'encoding', 20);

	yesno_list_row(_("Right To Left"), 'rtl', null, "", "", false);
	yesno_list_row(_("Default Language"), 'dflt', null, "", "", false);

	file_row(_("Language File") . " (PO)", 'uploadfile');
	file_row(_("Language File") . " (MO)", 'uploadfile2');

	end_table(0);
	display_note(_("Select your language files from your local harddisk."), 0, 1);
	echo "<center><input onclick='javascript:updateLanguage()' type='button' style='width:150px' value='". _("Save"). "'></center>";


	end_form();
}


//---------------------------------------------------------------------------------------------

if (isset($_GET['c']))
{
	if ($_GET['c'] == 'df')
	{
		handle_delete();
	}

	if ($_GET['c'] == 'u')
	{
		if (handle_submit())
		{
			//meta_forward($_SERVER['PHP_SELF']);
		}
	}
}

//---------------------------------------------------------------------------------------------

display_languages();

hyperlink_no_params($_SERVER['PHP_SELF'], _("Create a new language"));

display_language_edit($selected_id);

//---------------------------------------------------------------------------------------------

end_page();

?>