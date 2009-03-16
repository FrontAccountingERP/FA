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
$page_security = 20;
$path_to_root="..";
include_once($path_to_root . "/includes/session.inc");

page(_("Install/Update Modules"));

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/admin/db/company_db.inc");
include_once($path_to_root . "/admin/db/maintenance_db.inc");
include_once($path_to_root . "/modules/installed_modules.php");
include_once($path_to_root . "/includes/ui.inc");

$tabs = array('orders', 'AP', 'stock', 'manuf', 'proj', 'GL', 'system');
$names = array(_("Sales"), _("Purchases"), _("Items and Inventory"), _("Manufacturing"),
	_("Dimensions"), _("Banking and General Ledger"), _("Setup"));

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

function get_tab_title($tab)
{
	global $tabs, $names;
	for ($i = 0; $i < count($tabs); $i++)
	{
		if ($tabs[$i] == $tab)
			return $names[$i];
	}
	return "";
}

function tab_list_row($label, $name, $selected)
{
	global $tabs, $names;
	echo "<tr>\n";
	if ($label != null)
		echo "<td>$label</td>\n";
	if ($selected == null)
		$selected = (!isset($_POST[$name]) ? "orders" : $_POST[$name]);
	echo "<td><select name='$name'>";
	for ($i = 0; $i < count($tabs); $i++)
	{
		if ($selected == $tabs[$i])
			echo "<option selected value='".$tabs[$i]."'>" . $names[$i]. "</option>\n";
		else
			echo "<option value='".$tabs[$i]."'>" . $names[$i]. "</option>\n";
	}
	echo "</select></td>\n";
	echo "</tr>\n";
}

//---------------------------------------------------------------------------------------------

function check_data()
{
	if ($_POST['name'] == "" || $_POST['path'] == "")
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

function write_modules()
{
	global $path_to_root, $installed_modules;

	$mods = array_natsort($installed_modules, 'tab', 'tab');
	$installed_modules = $mods;
	//reset($installed_languages);
	$n = count($installed_modules);
	$msg = "<?php\n\n";

	$msg .= "/*****************************************************************\n";
	$msg .= "External modules for FrontAccounting\n";
	$msg .= "******************************************************************/\n";
	$msg .= "\n\n";

	$msg .= "\$installed_modules = array (\n";
	if ($n > 0)
	    $msg .= "\t0 => ";
	for ($i = 0; $i < $n; $i++)
	{
		if ($i > 0)
			$msg .= "\t\tarray ";
		else
			$msg .= "array ";
		$msg .= "('tab' => '" . $installed_modules[$i]['tab'] . "', ";
		$msg .= "'name' => '" . $installed_modules[$i]['name'] . "', ";
		$msg .= "'path' => '" . $installed_modules[$i]['path'] . "', ";
		$msg .= "'filename' => '" . $installed_modules[$i]['filename'] . "'";
		$msg .= "),\n";
	}
	$msg .= "\t);\n?>";

	$filename = $path_to_root . "/modules/installed_modules.php";
	// Check if the file exists and is writable first.
	if (file_exists($filename) && is_writable($filename))
	{
		if (!$zp = fopen($filename, 'w'))
		{
			display_error(_("Cannot open the modules file - ") . $filename);
			return false;
		}
		else
		{
			if (!fwrite($zp, $msg))
			{
				display_error(_("Cannot write to the modules file - ") . $filename);
				fclose($zp);
				return false;
			}
			// Close file
			fclose($zp);
		}
	}
	else
	{
		display_error(_("The modules file ") . $filename . _(" is not writable. Change its permissions so it is, then re-run the operation."));
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------------------------

function handle_submit()
{
	global $path_to_root, $installed_modules, $db_connections;

	if (!check_data())
		return false;

	$id = $_GET['id'];

	$installed_modules[$id]['tab'] = $_POST['tab'];
	$installed_modules[$id]['name'] = $_POST['name'];
	$installed_modules[$id]['path'] = $_POST['path'];
	$directory = $path_to_root . "/modules/" . $_POST['path'];
	if (!file_exists($directory))
	{
		mkdir($directory);
	}
	if (is_uploaded_file($_FILES['uploadfile']['tmp_name']))
	{
		$installed_modules[$id]['filename'] = $_FILES['uploadfile']['name'];
		$file1 = $_FILES['uploadfile']['tmp_name'];
		$file2 = $directory . "/".$_FILES['uploadfile']['name'];
		if (file_exists($file2))
			unlink($file2);
		move_uploaded_file($file1, $file2);
	}
	else
		$installed_modules[$id]['filename'] = $_POST['filename'];
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
	if (!write_modules())
		return false;
	return true;
}

//---------------------------------------------------------------------------------------------

function handle_delete()
{
	global  $path_to_root, $installed_modules;

	$id = $_GET['id'];

	$path = $installed_modules[$id]['path'];
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

	unset($installed_modules[$id]);
	$mods = array_values($installed_modules);
	$installed_modules = $mods;

	if (!write_modules())
		return;
	meta_forward($_SERVER['PHP_SELF']);
}

//---------------------------------------------------------------------------------------------

function display_modules()
{
	global $table_style, $installed_modules;

	echo "
		<script language='javascript'>
		function deleteModule(id, name) {
			if (!confirm('" . _("Are you sure you want to delete module: ") . "'+name))
				return
			document.location.replace('inst_module.php?c=df&id='+id)
		}
		</script>";
	start_table($table_style);
	$th = array(_("Tab"), _("Name"), _("Folder"), _("Filename"), "", "");
	table_header($th);

	$k = 0;
	$mods = $installed_modules;
	$n = count($mods);
	for ($i = 0; $i < $n; $i++)
	{
   		alt_table_row_color($k);

		label_cell(get_tab_title($mods[$i]['tab']));
		label_cell($mods[$i]['name']);
		label_cell($mods[$i]['path']);
		label_cell($mods[$i]['filename']);
		$edit = _("Edit");
		$delete = _("Delete");
		if (user_graphic_links())
		{
			$edit = set_icon(ICON_EDIT, $edit);
			$delete = set_icon(ICON_DELETE, $delete);
		}
    	label_cell("<a href='" . $_SERVER['PHP_SELF']. "?selected_id=$i'>$edit</a>");
		label_cell("<a href='javascript:deleteModule(".$i.", \"" . $mods[$i]['name'] . "\")'>$delete</a>");
		end_row();
	}

	end_table();
}

//---------------------------------------------------------------------------------------------

function display_module_edit($selected_id)
{
	global $installed_modules, $table_style2;

	if ($selected_id != -1)
		$n = $selected_id;
	else
		$n = count($installed_modules);

	start_form(true);

	echo "
		<script language='javascript'>
		function updateModule() {
			document.forms[0].action='inst_module.php?c=u&id=" . $n . "'
			document.forms[0].submit()
		}
		</script>";

	start_table($table_style2);

	if ($selected_id != -1)
	{
		$mod = $installed_modules[$selected_id];
		$_POST['tab']  = $mod['tab'];
		$_POST['name'] = $mod['name'];
		$_POST['path'] = $mod['path'];
		$_POST['filename'] = $mod['filename'];
		hidden('selected_id', $selected_id);
		hidden('filename', $_POST['filename']);
	}
	tab_list_row(_("Menu Tab"), 'tab', null);
	text_row_ex(_("Name"), 'name', 30);
	text_row_ex(_("Folder"), 'path', 20);

	label_row(_("Module File"), "<input name='uploadfile' type='file'>");
	label_row(_("SQL File"), "<input name='uploadfile2' type='file'>");

	end_table(0);
	display_note(_("Select your module PHP file from your local harddisk."), 0, 1);
	echo "<center><input onclick='javascript:updateModule()' type='button' style='width:150px' value='". _("Save"). "'></center>";


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

display_modules();

hyperlink_no_params($_SERVER['PHP_SELF'], _("Create a new module"));

display_module_edit($selected_id);

//---------------------------------------------------------------------------------------------

end_page();

?>