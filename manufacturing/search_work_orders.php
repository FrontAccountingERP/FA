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
$page_security = 'SA_MANUFTRANSVIEW';
$path_to_root = "..";
include($path_to_root . "/includes/db_pager.inc");
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/manufacturing/includes/manufacturing_ui.inc");
$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(800, 500);
if (isset($_GET['outstanding_only']) && ($_GET['outstanding_only'] == true))
{
// curently outstanding simply means not closed
	$outstanding_only = 1;
	page(_($help_context = "Search Outstanding Work Orders"), false, false, "", $js);
}
else
{
	$outstanding_only = 0;
	page(_($help_context = "Search Work Orders"), false, false, "", $js);
}
//-----------------------------------------------------------------------------------
// Ajax updates
//
if (get_post('SearchOrders')) 
{
	$Ajax->activate('orders_tbl');
} elseif (get_post('_OrderNumber_changed')) 
{
	$disable = get_post('OrderNumber') !== '';

	$Ajax->addDisable(true, 'StockLocation', $disable);
	$Ajax->addDisable(true, 'OverdueOnly', $disable);
	$Ajax->addDisable(true, 'OpenOnly', $disable);
	$Ajax->addDisable(true, 'SelectedStockItem', $disable);

	if ($disable) {
		set_focus('OrderNumber');
	} else
		set_focus('StockLocation');

	$Ajax->activate('orders_tbl');
}

//--------------------------------------------------------------------------------------

if (isset($_GET["stock_id"]))
	$_POST['SelectedStockItem'] = $_GET["stock_id"];

//--------------------------------------------------------------------------------------

start_form(false, false, $_SERVER['PHP_SELF'] ."?outstanding_only=$outstanding_only");

start_table("class='tablestyle_noborder'");
start_row();
ref_cells(_("Reference:"), 'OrderNumber', '',null, '', true);

locations_list_cells(_("at Location:"), 'StockLocation', null, true);

check_cells( _("Only Overdue:"), 'OverdueOnly', null);

if ($outstanding_only==0)
	check_cells( _("Only Open:"), 'OpenOnly', null);

stock_manufactured_items_list_cells(_("for item:"), 'SelectedStockItem', null, true);

submit_cells('SearchOrders', _("Search"),'',_('Select documents'),  'default');
end_row();
end_table();

//-----------------------------------------------------------------------------
function check_overdue($row)
{
	return (!$row["closed"] 
		&& date_diff2(Today(), sql2date($row["required_by"]), "d") > 0);
}

function view_link($dummy, $order_no)
{
	return get_trans_view_str(ST_WORKORDER, $order_no);
}

function view_stock($row)
{
	return view_stock_status($row["stock_id"], $row["description"], false);
}

function wo_type_name($dummy, $type)
{
	global $wo_types_array;
	
	return $wo_types_array[$type];
}

function edit_link($row)
{
	return  $row['closed'] ? '<i>'._('Closed').'</i>' :
		pager_link(_("Edit"),
			"/manufacturing/work_order_entry.php?trans_no=" . $row["id"], ICON_EDIT);
}

function release_link($row)
{
	return $row["closed"] ? '' : 
		($row["released"]==0 ?
		pager_link(_('Release'),
			"/manufacturing/work_order_release.php?trans_no=" . $row["id"])
		: 
		pager_link(_('Issue'),
			"/manufacturing/work_order_issue.php?trans_no=" .$row["id"]));
}

function produce_link($row)
{
	return $row["closed"] || !$row["released"] ? '' :
		pager_link(_('Produce'),
			"/manufacturing/work_order_add_finished.php?trans_no=" .$row["id"]);
}

function costs_link($row)
{
/*
	return $row["closed"] || !$row["released"] ? '' :
		pager_link(_('Costs'),
			"/gl/gl_bank.php?NewPayment=1&PayType=" 
			.PT_WORKORDER. "&PayPerson=" .$row["id"]);
*/			
	return $row["closed"] || !$row["released"] ? '' :
		pager_link(_('Costs'),
			"/manufacturing/work_order_costs.php?trans_no=" .$row["id"]);
}

function view_gl_link($row)
{
	if ($row['closed'] == 0)
		return '';
	return get_gl_view_str(ST_WORKORDER, $row['id']);
}

function dec_amount($row, $amount)
{
	return number_format2($amount, $row['decimals']);
}

$sql = "SELECT
	workorder.id,
	workorder.wo_ref,
	workorder.type,
	location.location_name,
	item.description,
	workorder.units_reqd,
	workorder.units_issued,
	workorder.date_,
	workorder.required_by,
	workorder.released_date,
	workorder.closed,
	workorder.released,
	workorder.stock_id,
	unit.decimals
	FROM ".TB_PREF."workorders as workorder,"
		.TB_PREF."stock_master as item,"
		.TB_PREF."item_units as unit,"
		.TB_PREF."locations as location
	WHERE workorder.stock_id=item.stock_id 
		AND workorder.loc_code=location.loc_code
		AND item.units=unit.abbr";

if (check_value('OpenOnly') || $outstanding_only != 0)
{
	$sql .= " AND workorder.closed=0";
}

if (isset($_POST['StockLocation']) && $_POST['StockLocation'] != $all_items)
{
	$sql .= " AND workorder.loc_code=".db_escape($_POST['StockLocation']);
}

if (isset($_POST['OrderNumber']) && $_POST['OrderNumber'] != "")
{
	$sql .= " AND workorder.wo_ref LIKE ".db_escape('%'.$_POST['OrderNumber'].'%');
}

if (isset($_POST['SelectedStockItem']) && $_POST['SelectedStockItem'] != $all_items)
{
	$sql .= " AND workorder.stock_id=".db_escape($_POST['SelectedStockItem']);
}

if (check_value('OverdueOnly'))
{
	$Today = date2sql(Today());

	$sql .= " AND workorder.required_by < '$Today' ";
}

$cols = array(
	_("#") => array('fun'=>'view_link'), 
	_("Reference"), // viewlink 2 ?
	_("Type") => array('fun'=>'wo_type_name'),
	_("Location"), 
	_("Item") => array('fun'=>'view_stock'),
	_("Required") => array('fun'=>'dec_amount', 'align'=>'right'),
	_("Manufactured") => array('fun'=>'dec_amount', 'align'=>'right'),
	_("Date") => 'date', 
	_("Required By") => array('type'=>'date', 'ord'=>''),
	array('insert'=>true, 'fun'=> 'edit_link'),
	array('insert'=>true, 'fun'=> 'release_link'),
	array('insert'=>true, 'fun'=> 'produce_link'),
	array('insert'=>true, 'fun'=> 'costs_link'),
	array('insert'=>true, 'fun'=> 'view_gl_link')
);

$table =& new_db_pager('orders_tbl', $sql, $cols);
$table->set_marker('check_overdue', _("Marked orders are overdue."));

$table->width = "90%";

display_db_pager($table);

end_form();
end_page();
?>
