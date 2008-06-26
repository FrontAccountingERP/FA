<?php

$page_security = 11;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("Items"));

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/inventory/includes/inventory_db.inc");

$user_comp = user_company();
$new_item = (!isset($_POST['stock_id']) || $_POST['stock_id'] == ""); 
//------------------------------------------------------------------------------------

if (isset($_GET['stock_id']))
{
	$stock_id = strtoupper($_GET['stock_id']);
}
else if (isset($_POST['stock_id']))
{
	$stock_id = strtoupper($_POST['stock_id']);
}

if(get_post('_stock_id_update')) {
	$_POST['NewStockID'] = $_POST['stock_id'];
	if ($new_item) {
	  clear_data();
	  set_focus('NewStockID');
	} else {
	  set_focus('description');
	}
	$Ajax->activate('details');
	$Ajax->activate('controls');
//	unset($_POST['New']);
}
$upload_file = "";
if (isset($_FILES['pic']) && $_FILES['pic']['name'] != '') 
{
	$result = $_FILES['pic']['error'];
 	$upload_file = 'Yes'; //Assume all is well to start off with
	$filename = $comp_path . "/$user_comp/images";
	if (!file_exists($filename))
	{
		mkdir($filename);
	}	
	$filename .= "/$stock_id.jpg";
	
	 //But check for the worst 
	if (strtoupper(substr(trim($_FILES['pic']['name']), strlen($_FILES['pic']['name']) - 3)) != 'JPG')
	{
		display_notification(_('Only jpg files are supported - a file extension of .jpg is expected'));
		$upload_file ='No';
	} 
	elseif ( $_FILES['pic']['size'] > ($max_image_size * 1024)) 
	{ //File Size Check
		display_notification(_('The file size is over the maximum allowed. The maximum size allowed in KB is') . ' ' . $max_image_size);
		$upload_file ='No';
	} 
	elseif ( $_FILES['pic']['type'] == "text/plain" ) 
	{  //File type Check
		display_notification( _('Only graphics files can be uploaded'));
         	$upload_file ='No';
	} 
	elseif (file_exists($filename))
	{
		display_notification(_('Attempting to overwrite an existing item image'));
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
		$message = ($result)?_('File url') ."<a href='$filename'>$filename</a>" : "Somthing is wrong with uploading a file.";
	}
 /* EOF Add Image upload for New Item  - by Ori */
}


check_db_has_stock_categories(_("There are no item categories defined in the system. At least one item category is required to add a item."));

check_db_has_item_tax_types(_("There are no item tax types defined in the system. At least one item tax type is required to add a item."));

function clear_data()
{
	global $new_item;
	
	unset($_POST['long_description']);
	unset($_POST['description']);
	unset($_POST['category_id']);
	unset($_POST['tax_type_id']);
	unset($_POST['units']);
	unset($_POST['mb_flag']);
	unset($_POST['NewStockID']);
	unset($_POST['dimension_id']);
	unset($_POST['dimension2_id']);
	$new_item = true;
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
		strstr($_POST['NewStockID'], "&")) 
	{
		$input_error = 1;
		display_error( _('The item code cannot contain any of the following characters -  & + OR a space OR quotes'));
		set_focus('NewStockID');

	}

	if ($input_error != 1)
	{

		if (!$new_item) 
		{ /*so its an existing one */

			update_item($_POST['NewStockID'], $_POST['description'],
				$_POST['long_description'], $_POST['category_id'], $_POST['tax_type_id'],
				$_POST['sales_account'], $_POST['inventory_account'], $_POST['cogs_account'],
				$_POST['adjustment_account'], $_POST['assembly_account'], 
				$_POST['dimension_id'], $_POST['dimension2_id']);

			display_notification(_("Item has been updated."));
			set_focus('stock_id');
			$Ajax->activate('details'); //update image if any
		} 
		else 
		{ //it is a NEW part

			add_item($_POST['NewStockID'], $_POST['description'],
				$_POST['long_description'], $_POST['category_id'], $_POST['tax_type_id'],
				$_POST['units'], $_POST['mb_flag'], $_POST['sales_account'],
				$_POST['inventory_account'], $_POST['cogs_account'],
				$_POST['adjustment_account'], $_POST['assembly_account'], 
				$_POST['dimension_id'], $_POST['dimension2_id']);

		display_notification(_("A new item has been added."));
		$_POST['stock_id'] = $_POST['NewStockID'];
		$new_item = false;
		set_focus('stock_id');
		$Ajax->activate('_page_body');
		}
	}
}

//------------------------------------------------------------------------------------

function can_delete($stock_id)
{
	$sql= "SELECT COUNT(*) FROM ".TB_PREF."stock_moves WHERE stock_id='$stock_id'";
	$result = db_query($sql, "could not query stock moves");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_('Cannot delete this item because there are stock movements that refer to this item.'));
		return false;
	}

	$sql= "SELECT COUNT(*) FROM ".TB_PREF."bom WHERE component='$stock_id'";
	$result = db_query($sql, "could not query boms");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_('Cannot delete this item record because there are bills of material that require this part as a component.'));
		return false;
	}

	$sql= "SELECT COUNT(*) FROM ".TB_PREF."sales_order_details WHERE stk_code='$stock_id'";
	$result = db_query($sql, "could not query sales orders");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_('Cannot delete this item record because there are existing sales orders for this part.'));
		return false;
	}

	$sql= "SELECT COUNT(*) FROM ".TB_PREF."purch_order_details WHERE item_code='$stock_id'";
	$result = db_query($sql, "could not query purchase orders");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_('Cannot delete this item because there are existing purchase order items for it.'));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------

if (isset($_POST['delete']) && strlen($_POST['delete']) > 1) 
{

	if (can_delete($_POST['NewStockID'])) {

		$stock_id = $_POST['NewStockID'];
		delete_item($stock_id);
		$filename = $comp_path . "/$user_comp/images/$stock_id.jpg";
		if (file_exists($filename))
			unlink($filename);
		display_notification(_("Selected item has been deleted."));
		$new_item = true;
		$_POST['stock_id'] = '';
		clear_data();
		set_focus('stock_id');
		$Ajax->activate('_page_body');
//		meta_forward($_SERVER['PHP_SELF']);
	}
}

//------------------------------------------------------------------------------------

start_form(true);

if (db_has_stock_items()) 
{
	start_table("class='tablestyle_noborder'");
	start_row();
    stock_items_list_cells(_("Select an item:"), 'stock_id', null,
	  _('New item'), true);
//    submit_cells('SelectStockItem', _("Edit Item"));
	end_row();
	end_table();
}

//hyperlink_params($_SERVER['PHP_SELF'], _("Enter a new item"), "New=1");
//echo "<br>";

div_start('details');
start_table("$table_style2 width=40%");

table_section_title(_("Item"));

//------------------------------------------------------------------------------------

if (!isset($_POST['NewStockID']) || $new_item) 
{

/*If the page was called without $_POST['NewStockID'] passed to page then assume a new item is to be entered show a form with a part Code field other wise the form showing the fields with the existing entries against the part will show for editing with only a hidden stock_id field. New is set to flag that the page may have called itself and still be entering a new part, in which case the page needs to know not to go looking up details for an existing part*/

	text_row(_("Item Code:"), 'NewStockID', null, 21, 20);

	$company_record = get_company_prefs();

    if (!isset($_POST['inventory_account']) || $_POST['inventory_account'] == "")
    	$_POST['inventory_account'] = $company_record["default_inventory_act"];

    if (!isset($_POST['cogs_account']) || $_POST['cogs_account'] == "")
    	$_POST['cogs_account'] = $company_record["default_cogs_act"];

	if (!isset($_POST['sales_account']) || $_POST['sales_account'] == "")
		$_POST['sales_account'] = $company_record["default_inv_sales_act"];

	if (!isset($_POST['adjustment_account']) || $_POST['adjustment_account'] == "")
		$_POST['adjustment_account'] = $company_record["default_adj_act"];

	if (!isset($_POST['assembly_account']) || $_POST['assembly_account'] == "")
		$_POST['assembly_account'] = $company_record["default_assembly_act"];

} 
else 
{ // Must be modifying an existing item

	if (!$new_item) 
	{
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
	
		label_row(_("Item Code:"),$_POST['NewStockID']);
		hidden('NewStockID', $_POST['NewStockID']);
	}
}

text_row(_("Name:"), 'description', null, 52, 50);

textarea_row(_('Description:'), 'long_description', null, 45, 3);

end_table();
start_table("$table_style2 width=40%");
// Add image upload for New Item  - by Joe
start_row();
label_cells(_("Image File (.jpg)") . ":", "<input type='file' id='pic' name='pic'>");
// Add Image upload for New Item  - by Joe
if (isset($_POST['NewStockID']) && file_exists("$comp_path/$user_comp/images/".$_POST['NewStockID'].".jpg")) 
{
	$stock_img_link = "<img src='$comp_path/$user_comp/images/".$_POST['NewStockID'].".jpg' width='$pic_width' height='$pic_height' border='0'>";
} 
else 
{
	$stock_img_link = "No Image";
}

label_cell($stock_img_link, "valign=top align=center rowspan=5");
end_row();

stock_categories_list_row(_("Category:"), 'category_id', null);

item_tax_types_list_row(_("Item Tax Type:"), 'tax_type_id', null);

stock_item_types_list_row(_("Item Type:"), 'mb_flag', null,
	(!isset($_POST['NewStockID']) || $new_item));

stock_units_list_row(_('Units of Measure:'), 'units', null,
	(!isset($_POST['NewStockID']) || $new_item));
end_table();
start_table("$table_style2 width=40%");

table_section_title(_("GL Accounts"));

gl_all_accounts_list_row(_("Sales Account:"), 'sales_account', $_POST['sales_account']);

gl_all_accounts_list_row(_("Inventory Account:"), 'inventory_account', $_POST['inventory_account']);

if (!is_service($_POST['mb_flag'])) 
{
	gl_all_accounts_list_row(_("C.O.G.S. Account:"), 'cogs_account', $_POST['cogs_account']);
	gl_all_accounts_list_row(_("Inventory Adjustments Account:"), 'adjustment_account', $_POST['adjustment_account']);
}
else 
{
	hidden('cogs_account', $_POST['cogs_account']);
	hidden('adjustment_account', $_POST['adjustment_account']);
}


if (is_manufactured($_POST['mb_flag']))
	gl_all_accounts_list_row(_("Item Assembly Costs Account:"), 'assembly_account', $_POST['assembly_account']);
else
	hidden('assembly_account', $_POST['assembly_account']);
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

end_table(1);
div_end();
div_start('controls');
if (!isset($_POST['NewStockID']) || $new_item) 
{
	submit_center('addupdate', _("Insert New Item"), true, '', true);
} 
else 
{
	submit_center_first('addupdate', _("Update Item"), '', true);

	submit_center_last('delete', _("Delete This Item"), '', true);
}

div_end();
end_form();

//------------------------------------------------------------------------------------

end_page();
?>
