<?php


$page_security = 2;
$path_to_root="../..";
include($path_to_root . "/includes/db_pager.inc");
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/banking.inc");
include_once($path_to_root . "/sales/includes/sales_db.inc");

include_once($path_to_root . "/includes/ui.inc");
$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(800, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();

page(_("Inventory Item Movement"), false, false, "", $js);
//------------------------------------------------------------------------------------------------
include $path_to_root.'/sql/upgrade.php';
check_db_has_stock_items(_("There are no items defined in the system."));

if (isset($_GET['stock_id']))
{
	$_POST['stock_id'] = $_GET['stock_id'];
}

start_form(false, true);

if (!isset($_POST['stock_id']))
	$_POST['stock_id'] = get_global_stock_item();

start_table("class='tablestyle_noborder'");

stock_items_list_cells(_("Item:"), 'stock_id', $_POST['stock_id']);

locations_list_cells(_("From Location:"), 'StockLocation', null);

date_cells(_("From:"), 'AfterDate', '', null, -30);
date_cells(_("To:"), 'BeforeDate');

submit_cells('ShowMoves',_("Show Movements"),'',_('Refresh Inquiry'), true);
end_table();
end_form();

set_global_stock_item($_POST['stock_id']);
$item_dec = get_qty_dec($_POST['stock_id']);
//
//	Get summary displayed in headewr and footer.
//
function get_summary(&$table)
{
	global $Ajax, $item_dec;

	$sql = "SELECT
		Sum(qty) as sum,
		Sum(IF(qty>0, qty, 0)) as in_qty,
		Sum(IF(qty<0, -qty, 0)) as out_qty
	FROM ".TB_PREF."stock_moves
	WHERE loc_code='" . $_POST['StockLocation'] . "'
	AND tran_date >= '". date2sql($_POST['AfterDate']) . "'
	AND tran_date <= '" . date2sql($_POST['BeforeDate']) . "'
	AND stock_id = '" . $_POST['stock_id'] . "'";
	" GROUP BY rec";
	$result = db_query($sql, "cannot retrieve stock moves");

	$qty = db_fetch($result);

	$sum['beg'] = get_qoh_on_date($_POST['stock_id'], $_POST['StockLocation'],
		add_days($_POST['AfterDate'], -1));
 	$sum['in'] = $qty['in_qty'];
 	$sum['out'] = $qty['out_qty'];
 	$sum['end'] = $sum['beg'] + $qty['sum'];
	$sum['dec'] = $item_dec = get_qty_dec($_POST['stock_id']);

	$table->sum = $sum;
	$Ajax->activate('summary');
 }
//-----------------------------------------------------------------------------

function systype_name($row)
{
	return systypes::name($row["type"]);
}

function trans_view($row)
{
	return	get_trans_view_str($row["type"], $row["trans_no"]);
}

function show_details($row)
{
	$person = $row["person_id"];
	$gl_posting = "";

	if (($row["type"] == 13) || ($row["type"] == 11))
	{
		$cust_row = get_customer_details_from_trans($row["type"], $row["trans_no"]);

		if (strlen($cust_row['name']) > 0)
			$person = $cust_row['name'] . " (" . $cust_row['br_name'] . ")";

	}
	elseif ($row["type"] == 25 || $row['type'] == 21)
	{
		// get the supplier name
		$sql = "SELECT supp_name FROM ".TB_PREF."suppliers WHERE supplier_id = '" . $row["person_id"] . "'";
		$supp_result = db_query($sql,"check failed");

		$supp_row = db_fetch($supp_result);

		if (strlen($supp_row['supp_name']) > 0)
			$person = $supp_row['supp_name'];
	}
	elseif ($row["type"] == systypes::location_transfer() || $row["type"] == systypes::inventory_adjustment())
	{
		// get the adjustment type
		$movement_type = get_movement_type($row["person_id"]);
		$person = $movement_type["name"];
	}
	elseif ($row["type"]==systypes::work_order() || $row["type"] == 28  ||
		$row["type"] == 29)
	{
		$person = "";
	}
	return $person;
}

$total_out = 0;
$total_in = 0;

function qty_in($row)
{
	$q = $row["qty"];
	return $q <= 0 ? '' : $q;
}

function qty_out($row)
{
	$q = -$row["qty"];
	return $q <= 0 ? '' : $q;
}
/*
function show_qoh($row)
{
	$qoh =& $_SESSION['qoh'];
	$qoh += $row['qty'];
	return $qoh;
}
*/
function before_status($pager)
{
	$r[] =
		array( "<b>"._("Quantity on hand before") . " " . $_POST['AfterDate']
		.':'."</b>", "align='right' colspan=5");
	if($pager->sum['beg']>=0) {
		$r[] = array (number_format2($pager->sum['beg'], $pager->sum['dec']),
		"align='right'");
		$r[] = array("&nbsp;");
	} else {
		$r[] = array("&nbsp;");
		$r[] = array (number_format2($pager->sum['beg'], $pager->sum['dec']),
		"align='right'");
	}
	return $r;
}

function after_status($pager)
{
	$r[] =
		array( "<b>"._("Quantity on hand after") . " " . $_POST['BeforeDate']
		.':'."</b>", "align='right' colspan=5");
	if($pager->sum['end']>=0) {
		$r[] = array (number_format2($pager->sum['end'], $pager->sum['dec']),
		"align='right'");
		$r[] = array("&nbsp;", "colspan=2");
	} else {
		$r[] = array("&nbsp;", "colspan=2");
		$r[] = array (number_format2($pager->sum['end'], $pager->sum['dec']),
		"align='right'");
	}
	return $r;
}
//-----------------------------------------------------------------------------

$before_date = date2sql($_POST['BeforeDate']);
$after_date = date2sql($_POST['AfterDate']);

$sql = "SELECT 
	type, 
	trans_no, 
	reference,
	tran_date, 
	person_id, 
	qty
	FROM ".TB_PREF."stock_moves
	WHERE loc_code='" . $_POST['StockLocation'] . "'
	AND tran_date >= '". $after_date . "'
	AND tran_date <= '" . $before_date . "'
	AND stock_id = '" . $_POST['stock_id'] . "'";

$cols = array(
	_("Type") => array('fun'=>'systype_name' ), 
	_("#") => array('fun'=>'trans_view' ), 
	_("Reference"), 
	_("Date") => array('date', 'ord'=>'desc'), 
	_("Detail") => array('fun'=>'show_details' ), 
	_("Quantity In") => array('type'=>'amount', 'dec'=> $item_dec, 'insert'=>true,'fun'=>'qty_in' ),
	_("Quantity Out") => array('type'=>'amount', 'dec'=> $item_dec,'insert'=>true,'fun'=>'qty_out' ), 
//	_("Quantity On Hand") => array('insert'=>true,'type'=>'amount', 'fun'=>'show_qoh' )
);

$table =& new_db_pager('doc_tbl', $sql, $cols);
$table->set_header('before_status');
$table->set_footer('after_status');

if (!$table->ready)  // new sql query - update summary
	get_summary(&$table);

start_form();

display_db_pager($table);

end_form();
end_page();
?>
