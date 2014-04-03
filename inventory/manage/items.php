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
$page_security = 'SA_ITEM';
$path_to_root = "../..";
include($path_to_root . "/includes/session.inc");
$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();
	
page(_($help_context = "Items"), @$_REQUEST['popup'], false, "", $js);

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/inventory/includes/inventory_db.inc");

$user_comp = user_company();
$new_item = get_post('stock_id')=='' || get_post('cancel') || get_post('clone'); 
//------------------------------------------------------------------------------------

if (isset($_GET['stock_id']))
{
	$_POST['stock_id'] = $_GET['stock_id'];
}
$stock_id = get_post('stock_id');
if (list_updated('stock_id')) {
	$_POST['NewStockID'] = $stock_id = get_post('stock_id');
    clear_data();
	$Ajax->activate('details');
	$Ajax->activate('controls');
}

if (get_post('cancel')) {
	$_POST['NewStockID'] = $stock_id = $_POST['stock_id'] = '';
    clear_data();
	set_focus('stock_id');
	$Ajax->activate('_page_body');
}
if (list_updated('category_id') || list_updated('mb_flag')) {
	$Ajax->activate('details');
}
$upload_file = "";
if (isset($_FILES['pic']) && $_FILES['pic']['name'] != '') 
{
	$stock_id = $_POST['NewStockID'];
	$result = $_FILES['pic']['error'];
 	$upload_file = 'Yes'; //Assume all is well to start off with
	$filename = company_path().'/images';
	if (!file_exists($filename))
	{
		mkdir($filename);
	}	
	$filename .= "/".item_img_name($stock_id).".jpg";
	
	//But check for the worst 
	if ((list($width, $height, $type, $attr) = getimagesize($_FILES['pic']['tmp_name'])) !== false)
		$imagetype = $type;
	else
		$imagetype = false;
	//$imagetype = exif_imagetype($_FILES['pic']['tmp_name']);
	if ($imagetype != IMAGETYPE_GIF && $imagetype != IMAGETYPE_JPEG && $imagetype != IMAGETYPE_PNG)
	{	//File type Check
		display_warning( _('Only graphics files can be uploaded'));
		$upload_file ='No';
	}
	elseif (!in_array(strtoupper(substr(trim($_FILES['pic']['name']), strlen($_FILES['pic']['name']) - 3)), array('JPG','PNG','GIF')))
	{
		display_warning(_('Only graphics files are supported - a file extension of .jpg, .png or .gif is expected'));
		$upload_file ='No';
	} 
	elseif ( $_FILES['pic']['size'] > ($max_image_size * 1024)) 
	{ //File Size Check
		display_warning(_('The file size is over the maximum allowed. The maximum size allowed in KB is') . ' ' . $max_image_size);
		$upload_file ='No';
	} 
	elseif (file_exists($filename))
	{
		$result = unlink($filename);
		if (!$result) 
		{
			display_error(_('The existing image could not be removed'));
			$upload_file ='No';
		}
	}
	
	if ($upload_file == 'Yes')
	{
		$result  =  move_uploaded_file($_FILES['pic']['tmp_name'], $filename);
	}
	$Ajax->activate('details');
 /* EOF Add Image upload for New Item  - by Ori */
}

check_db_has_stock_categories(_("There are no item categories defined in the system. At least one item category is required to add a item."));

check_db_has_item_tax_types(_("There are no item tax types defined in the system. At least one item tax type is required to add a item."));

function clear_data()
{
	unset($_POST['long_description']);
	unset($_POST['description']);
	unset($_POST['category_id']);
	unset($_POST['tax_type_id']);
	unset($_POST['units']);
	unset($_POST['mb_flag']);
	unset($_POST['NewStockID']);
	unset($_POST['dimension_id']);
	unset($_POST['dimension2_id']);
	unset($_POST['no_sale']);
}

//------------------------------------------------------------------------------------

if (isset($_POST['addupdate'])) 
{

	$input_error = 0;
	if ($upload_file == 'No')
		$input_error = 1;
	if (strlen($_POST['description']) == 0) 
	{
		$input_error = 1;
		display_error( _('The item name must be entered.'));
		set_focus('description');
	} 
	elseif (strlen($_POST['NewStockID']) == 0) 
	{
		$input_error = 1;
		display_error( _('The item code cannot be empty'));
		set_focus('NewStockID');
	}
	elseif (strstr($_POST['NewStockID'], " ") || strstr($_POST['NewStockID'],"'") || 
		strstr($_POST['NewStockID'], "+") || strstr($_POST['NewStockID'], "\"") || 
		strstr($_POST['NewStockID'], "&") || strstr($_POST['NewStockID'], "\t")) 
	{
		$input_error = 1;
		display_error( _('The item code cannot contain any of the following characters -  & + OR a space OR quotes'));
		set_focus('NewStockID');

	}
	elseif ($new_item && db_num_rows(get_item_kit($_POST['NewStockID'])))
	{
		  	$input_error = 1;
      		display_error( _("This item code is already assigned to stock item or sale kit."));
			set_focus('NewStockID');
	}
	
	if ($input_error != 1)
	{
		if (check_value('del_image'))
		{
			$filename = company_path().'/images/'.item_img_name($_POST['NewStockID']).".jpg";
			if (file_exists($filename))
				unlink($filename);
		}
		
		if (!$new_item) 
		{ /*so its an existing one */
			update_item($_POST['NewStockID'], $_POST['description'],
				$_POST['long_description'], $_POST['category_id'], 
				$_POST['tax_type_id'], get_post('units'),
				get_post('mb_flag'), $_POST['sales_account'],
				$_POST['inventory_account'], $_POST['cogs_account'],
				$_POST['adjustment_account'], $_POST['assembly_account'], 
				$_POST['dimension_id'], $_POST['dimension2_id'],
				check_value('no_sale'), check_value('editable'));
			update_record_status($_POST['NewStockID'], $_POST['inactive'],
				'stock_master', 'stock_id');
			update_record_status($_POST['NewStockID'], $_POST['inactive'],
				'item_codes', 'item_code');
			set_focus('stock_id');
			$Ajax->activate('stock_id'); // in case of status change
			display_notification(_("Item has been updated."));
		} 
		else 
		{ //it is a NEW part

			add_item($_POST['NewStockID'], $_POST['description'],
				$_POST['long_description'], $_POST['category_id'], $_POST['tax_type_id'],
				$_POST['units'], $_POST['mb_flag'], $_POST['sales_account'],
				$_POST['inventory_account'], $_POST['cogs_account'],
				$_POST['adjustment_account'], $_POST['assembly_account'], 
				$_POST['dimension_id'], $_POST['dimension2_id'],
				check_value('no_sale'), check_value('editable'));

			display_notification(_("A new item has been added."));
			$_POST['stock_id'] = $_POST['NewStockID'] = 
			$_POST['description'] = $_POST['long_description'] = '';
			$_POST['no_sale'] = $_POST['editable'] = 0;
			set_focus('NewStockID');
		}
		$Ajax->activate('_page_body');
	}
}

if (get_post('clone')) {
	unset($_POST['stock_id']);
	$stock_id = '';
	unset($_POST['inactive']);
	set_focus('NewStockID');
	$Ajax->activate('_page_body');
}

//------------------------------------------------------------------------------------

function check_usage($stock_id, $dispmsg=true)
{
	$msg = item_in_foreign_codes($stock_id);

	if ($msg != '')	{
		if($dispmsg) display_error($msg);
		return false;
	}
	return true;
}

//------------------------------------------------------------------------------------

if (isset($_POST['delete']) && strlen($_POST['delete']) > 1) 
{

	if (check_usage($_POST['NewStockID'])) {

		$stock_id = $_POST['NewStockID'];
		delete_item($stock_id);
		$filename = company_path().'/images/'.item_img_name($stock_id).".jpg";
		if (file_exists($filename))
			unlink($filename);
		display_notification(_("Selected item has been deleted."));
		$_POST['stock_id'] = '';
		clear_data();
		set_focus('stock_id');
		$new_item = true;
		$Ajax->activate('_page_body');
	}
}

function item_settings(&$stock_id) 
{
	global $SysPrefs, $path_to_root, $new_item, $pic_height;

	start_outer_table(TABLESTYLE2);

	table_section(1);

	table_section_title(_("Item"));

	//------------------------------------------------------------------------------------
	if ($new_item) 
	{
		text_row(_("Item Code:"), 'NewStockID', null, 21, 20);

		$_POST['inactive'] = 0;
	} 
	else 
	{ // Must be modifying an existing item
		if (get_post('NewStockID') != get_post('stock_id') || get_post('addupdate')) { // first item display

			$_POST['NewStockID'] = $_POST['stock_id'];

			$myrow = get_item($_POST['NewStockID']);

			$_POST['long_description'] = $myrow["long_description"];
			$_POST['description'] = $myrow["description"];
			$_POST['category_id']  = $myrow["category_id"];
			$_POST['tax_type_id']  = $myrow["tax_type_id"];
			$_POST['units']  = $myrow["units"];
			$_POST['mb_flag']  = $myrow["mb_flag"];

			$_POST['sales_account'] =  $myrow['sales_account'];
			$_POST['inventory_account'] = $myrow['inventory_account'];
			$_POST['cogs_account'] = $myrow['cogs_account'];
			$_POST['adjustment_account']	= $myrow['adjustment_account'];
			$_POST['assembly_account']	= $myrow['assembly_account'];
			$_POST['dimension_id']	= $myrow['dimension_id'];
			$_POST['dimension2_id']	= $myrow['dimension2_id'];
			$_POST['no_sale']	= $myrow['no_sale'];
			$_POST['del_image'] = 0;	
			$_POST['inactive'] = $myrow["inactive"];
			$_POST['editable'] = $myrow["editable"];
		}
		label_row(_("Item Code:"),$_POST['NewStockID']);
		hidden('NewStockID', $_POST['NewStockID']);
		set_focus('description');
	}

	text_row(_("Name:"), 'description', null, 52, 200);

	textarea_row(_('Description:'), 'long_description', null, 42, 3);

	stock_categories_list_row(_("Category:"), 'category_id', null, false, $new_item);

	if ($new_item && (list_updated('category_id') || !isset($_POST['units']))) {

		$category_record = get_item_category($_POST['category_id']);

		$_POST['tax_type_id'] = $category_record["dflt_tax_type"];
		$_POST['units'] = $category_record["dflt_units"];
		$_POST['mb_flag'] = $category_record["dflt_mb_flag"];
		$_POST['inventory_account'] = $category_record["dflt_inventory_act"];
		$_POST['cogs_account'] = $category_record["dflt_cogs_act"];
		$_POST['sales_account'] = $category_record["dflt_sales_act"];
		$_POST['adjustment_account'] = $category_record["dflt_adjustment_act"];
		$_POST['assembly_account'] = $category_record["dflt_assembly_act"];
		$_POST['dimension_id'] = $category_record["dflt_dim1"];
		$_POST['dimension2_id'] = $category_record["dflt_dim2"];
		$_POST['no_sale'] = $category_record["dflt_no_sale"];
		$_POST['editable'] = 0;

	}
	$fresh_item = !isset($_POST['NewStockID']) || $new_item 
		|| check_usage($_POST['stock_id'],false);

	item_tax_types_list_row(_("Item Tax Type:"), 'tax_type_id', null);

	stock_item_types_list_row(_("Item Type:"), 'mb_flag', null, $fresh_item);

	stock_units_list_row(_('Units of Measure:'), 'units', null, $fresh_item);

	check_row(_("Editable description:"), 'editable');

	check_row(_("Exclude from sales:"), 'no_sale');

	table_section(2);

	$dim = get_company_pref('use_dimension');
	if ($dim >= 1)
	{
		table_section_title(_("Dimensions"));

		dimensions_list_row(_("Dimension")." 1", 'dimension_id', null, true, " ", false, 1);
		if ($dim > 1)
			dimensions_list_row(_("Dimension")." 2", 'dimension2_id', null, true, " ", false, 2);
	}
	if ($dim < 1)
		hidden('dimension_id', 0);
	if ($dim < 2)
		hidden('dimension2_id', 0);

	table_section_title(_("GL Accounts"));

	gl_all_accounts_list_row(_("Sales Account:"), 'sales_account', $_POST['sales_account']);

	if (!is_service($_POST['mb_flag'])) 
	{
		gl_all_accounts_list_row(_("Inventory Account:"), 'inventory_account', $_POST['inventory_account']);
		gl_all_accounts_list_row(_("C.O.G.S. Account:"), 'cogs_account', $_POST['cogs_account']);
		gl_all_accounts_list_row(_("Inventory Adjustments Account:"), 'adjustment_account', $_POST['adjustment_account']);
	}
	else 
	{
		gl_all_accounts_list_row(_("C.O.G.S. Account:"), 'cogs_account', $_POST['cogs_account']);
		hidden('inventory_account', $_POST['inventory_account']);
		hidden('adjustment_account', $_POST['adjustment_account']);
	}


	if (is_manufactured($_POST['mb_flag']))
		gl_all_accounts_list_row(_("Item Assembly Costs Account:"), 'assembly_account', $_POST['assembly_account']);
	else
		hidden('assembly_account', $_POST['assembly_account']);

	table_section_title(_("Other"));

	// Add image upload for New Item  - by Joe
	file_row(_("Image File (.jpg)") . ":", 'pic', 'pic');
	// Add Image upload for New Item  - by Joe
	$stock_img_link = "";
	$check_remove_image = false;
	if (isset($_POST['NewStockID']) && file_exists(company_path().'/images/'
		.item_img_name($_POST['NewStockID']).".jpg")) 
	{
	 // 31/08/08 - rand() call is necessary here to avoid caching problems. Thanks to Peter D.
		$stock_img_link .= "<img id='item_img' alt = '[".$_POST['NewStockID'].".jpg".
			"]' src='".company_path().'/images/'.item_img_name($_POST['NewStockID']).
			".jpg?nocache=".rand()."'"." height='$pic_height' border='0'>";
		$check_remove_image = true;
	} 
	else 
	{
		$stock_img_link .= _("No image");
	}

	label_row("&nbsp;", $stock_img_link);
	if ($check_remove_image)
		check_row(_("Delete Image:"), 'del_image');

	record_status_list_row(_("Item status:"), 'inactive');
	end_outer_table(1);

	div_start('controls');
	if (!isset($_POST['NewStockID']) || $new_item) 
	{
		submit_center('addupdate', _("Insert New Item"), true, '', 'default');
	} 
	else 
	{
		submit_center_first('addupdate', _("Update Item"), '', 
			@$_REQUEST['popup'] ? true : 'default');
		submit_return('select', get_post('stock_id'), 
			_("Select this items and return to document entry."), 'default');
		submit('clone', _("Clone This Item"), true, '', true);
		submit('delete', _("Delete This Item"), true, '', true);
		submit_center_last('cancel', _("Cancel"), _("Cancel Edition"), 'cancel');
	}

	div_end();
}

//-------------------------------------------------------------------------------------------- 

start_form(true);

if (db_has_stock_items()) 
{
	start_table(TABLESTYLE_NOBORDER);
	start_row();
    stock_items_list_cells(_("Select an item:"), 'stock_id', null,
	  _('New item'), true, check_value('show_inactive'));
	$new_item = get_post('stock_id')=='';
	check_cells(_("Show inactive:"), 'show_inactive', null, true);
	end_row();
	end_table();

	if (get_post('_show_inactive_update')) {
		$Ajax->activate('stock_id');
		set_focus('stock_id');
	}
}
else
{
	hidden('stock_id', get_post('stock_id'));
}

div_start('details');

$stock_id = get_post('stock_id');
if (!$stock_id)
	unset($_POST['_tabs_sel']); // force settings tab for new customer

tabbed_content_start('tabs', array(
		'settings' => array(_('&General settings'), $stock_id),
		'sales_pricing' => array(_('S&ales Pricing'), $stock_id),
		'purchase_pricing' => array(_('&Purchasing Pricing'), $stock_id),
		'standard_cost' => array(_('Standard &Costs'), $stock_id),
		'reorder_level' => array(_('&Reorder Levels'), (is_inventory_item($stock_id) ? $stock_id : null)),
		'movement' => array(_('&Transactions'), $stock_id),
		'status' => array(_('&Status'), $stock_id),
	));
	
	switch (get_post('_tabs_sel')) {
		default:
		case 'settings':
			item_settings($stock_id); 
			break;
		case 'sales_pricing':
			$_GET['stock_id'] = $stock_id;
			$_GET['popup'] = 1;
			include_once($path_to_root."/inventory/prices.php");
			break;
		case 'purchase_pricing':
			$_GET['stock_id'] = $stock_id;
			$_GET['popup'] = 1;
			include_once($path_to_root."/inventory/purchasing_data.php");
			break;
		case 'standard_cost':
			$_GET['stock_id'] = $stock_id;
			$_GET['popup'] = 1;
			include_once($path_to_root."/inventory/cost_update.php");
			break;
		case 'reorder_level':
			if (!is_inventory_item($stock_id))
			{
				break;
			}	
			$_GET['stock_id'] = $stock_id;
			$_GET['popup'] = 1;
			include_once($path_to_root."/inventory/reorder_level.php");
			break;
		case 'movement':
			$_GET['stock_id'] = $stock_id;
			$_GET['popup'] = 1;
			include_once($path_to_root."/inventory/inquiry/stock_movements.php");
			break;
		case 'status':
			$_GET['stock_id'] = $stock_id;
			$_GET['popup'] = 1;
			include_once($path_to_root."/inventory/inquiry/stock_status.php");
			break;
	};
br();
tabbed_content_end();

div_end();


hidden('popup', @$_REQUEST['popup']);
end_form();

//------------------------------------------------------------------------------------

end_page(@$_REQUEST['popup']);
?>
