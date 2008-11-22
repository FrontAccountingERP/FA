<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU Affero General Public License,
	AGPL, as published by the Free Software Foundation, either version 
	3 of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/agpl-3.0.html>.
***********************************************************************/
$page_security = 20;
$path_to_root="..";
include_once($path_to_root . "/includes/session.inc");

page(_("Install/Update Languages"));

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
	if ($_POST['code'] == "" || $_POST['name'] == "" || $_POST['encoding'] == "")
		return false;
	return true;
}

/**
 * @return Returns the array sorted as required
 * @param $aryData Array containing data to sort
 * @param $strIndex name of column to use as an index
 * @param $strSortBy Column to sort the array by
 * @param $strSortType String containing either asc or desc [default to asc]
 * @desc Naturally sorts an array using by the column $strSortBy
 */
function array_natsort($aryData, $strIndex, $strSortBy, $strSortType=false)
{
   //    if the parameters are invalid
   if (!is_array($aryData) || !$strIndex || !$strSortBy)
       //    return the array
       return $aryData;

   //    create our temporary arrays
   $arySort = $aryResult = array();

   //    loop through the array
   foreach ($aryData as $aryRow)
       //    set up the value in the array
       $arySort[$aryRow[$strIndex]] = $aryRow[$strSortBy];

   //    apply the natural sort
   natsort($arySort);

   //    if the sort type is descending
   if ($strSortType=="desc")
       //    reverse the array
       arsort($arySort);

   //    loop through the sorted and original data
   foreach ($arySort as $arySortKey => $arySorted)
       foreach ($aryData as $aryOriginal)
           //    if the key matches
           if ($aryOriginal[$strIndex]==$arySortKey)
               //    add it to the output array
               array_push($aryResult, $aryOriginal);

   //    return the return
   return $aryResult;
}

function write_lang()
{
	global $path_to_root, $installed_languages;
	include_once($path_to_root . "/lang/installed_languages.inc");

	$conn = array_natsort($installed_languages, 'code', 'code');
	$installed_languages = $conn;
	//reset($installed_languages);
	$n = count($installed_languages);
	$msg = "<?php\n\n";

	$msg .= "/* How to make new entries here\n\n";
	$msg .= "-- if adding languages at the beginning of the list, make sure it's index is set to 0 (it has ' 0 => ')\n";
	$msg .= "-- 'code' should match the name of the directory for the language under \\lang\n";
	$msg .= "-- 'name' is the name that will be displayed in the language selection list (in Users and Display Setup)\n";
	$msg .= "-- 'rtl' only needs to be set for right-to-left languages like Arabic and Hebrew\n\n";
	$msg .= "*/\n\n\n";

	$msg .= "\$installed_languages = array (\n";
	if ($n > 0)
	    $msg .= "\t0 => ";
	for ($i = 0; $i < $n; $i++)
	{
		if ($i > 0)
			$msg .= "\t\tarray ";
		else
			$msg .= "array ";
		$msg .= "('code' => '" . $installed_languages[$i]['code'] . "', ";
		$msg .= "'name' => '" . $installed_languages[$i]['name'] . "', ";
		$msg .= "'encoding' => '" . $installed_languages[$i]['encoding'] . "'";
		if (isset($installed_languages[$i]['rtl']) && $installed_languages[$i]['rtl'])
			$msg .= ", 'rtl' => true),\n";
		else
			$msg .= "),\n";
	}
	$msg .= "\t);\n?>";

	$filename = $path_to_root . "/lang/installed_languages.inc";
	// Check if the file exists and is writable first.
	if (file_exists($filename) && is_writable($filename))
	{
		if (!$zp = fopen($filename, 'w'))
		{
			display_error(_("Cannot open the languages file - ") . $filename);
			return false;
		}
		else
		{
			if (!fwrite($zp, $msg))
			{
				display_error(_("Cannot write to the language file - ") . $filename);
				fclose($zp);
				return false;
			}
			// Close file
			fclose($zp);
		}
	}
	else
	{
		display_error(_("The language file ") . $filename . _(" is not writable. Change its permissions so it is, then re-run the operation."));
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------------------------

function handle_submit()
{
	global $path_to_root, $installed_languages;

	if (!check_data())
		return false;

	$id = $_GET['id'];

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
	global  $path_to_root, $installed_languages;

	$id = $_GET['id'];

	$lang = $installed_languages[$id]['code'];
	$filename = "$path_to_root/lang/$lang/LC_MESSAGES";
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
	$filename = "$path_to_root/lang/$lang";
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

	unset($installed_languages[$id]);
	$conn = array_values($installed_languages);
	$installed_languages = $conn;

	//$$db_connections = array_values($db_connections);

	if (!write_lang())
		return;
	meta_forward($_SERVER['PHP_SELF']);
}

//---------------------------------------------------------------------------------------------

function display_languages()
{
	global $table_style, $installed_languages;

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
	$th = array(_("Language"), _("Name"), _("Encoding"), _("Right To Left"), "", "");
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
	global $installed_languages, $table_style2;

	if ($selected_id != -1)
		$n = $selected_id;
	else
		$n = count($installed_languages);

	start_form(true, true);

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
		hidden('selected_id', $selected_id);
	}
	text_row_ex(_("Language"), 'code', 20);
	text_row_ex(_("Name"), 'name', 20);
	text_row_ex(_("Encoding"), 'encoding', 20);

	yesno_list_row(_("Right To Left"), 'rtl', null, "", "", false);

	label_row(_("Language File") . " (PO)", "<input name='uploadfile' type='file'>");
	label_row(_("Language File") . " (MO)", "<input name='uploadfile2' type='file'>");

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