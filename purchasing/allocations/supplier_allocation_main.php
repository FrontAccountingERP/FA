<?php

$path_to_root="../..";
$page_security = 3;
include($path_to_root . "/includes/db_pager.inc");
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");

include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/banking.inc");

include_once($path_to_root . "/sales/includes/sales_db.inc");
$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
page(_("Supplier Allocations"), false, false, "", $js);

//--------------------------------------------------------------------------------
if ($ret = context_restore()) {
	if(isset($ret['supplier_id']))
		$_POST['supplier_id'] = $ret['supplier_id'];
}
if (isset($_POST['_supplier_id_editor'])) {
	context_call($path_to_root.'/purchasing/manage/suppliers.php?supplier_id='.$_POST['supplier_id'] );
}

//--------------------------------------------------------------------------------

start_form();

	/* show all outstanding receipts and credits to be allocated */
	/*Clear any previous allocation records */
	if (isset($_SESSION['alloc']))
	{
		unset($_SESSION['alloc']->allocs);
		unset($_SESSION['alloc']);
	}
    if (!isset($_POST['supplier_id']))
    	$_POST['supplier_id'] = get_global_supplier();

    echo "<center>" . _("Select a Supplier: ") . "&nbsp;&nbsp;";
	supplier_list('supplier_id', $_POST['supplier_id'], true, true);
    echo "<br>";
    check(_("Show Settled Items:"), 'ShowSettled', null, true);
	echo "</center><br><br>";

end_form();
	set_global_supplier($_POST['supplier_id']);

	if (isset($_POST['supplier_id']) && ($_POST['supplier_id'] == reserved_words::get_all())) 
	{
		unset($_POST['supplier_id']);
	}

	$settled = false;
	if (check_value('ShowSettled'))
		$settled = true;
	$supplier_id = null;
	if (isset($_POST['supplier_id']))
		$supplier_id = $_POST['supplier_id'];

//--------------------------------------------------------------------------------
function systype_name($dummy, $type)
{
	return systypes::name($type);
}

function trans_view($trans)
{
	return get_trans_view_str($trans["type"], $trans["trans_no"]);
}

function alloc_link($row)
{
	return pager_link(_("Allocate"),
		"/purchasing/allocations/supplier_allocate.php?trans_no="
			.$row["trans_no"] . "&trans_type=" . $row["type"] );
}

function amount_left($row)
{
	return -$row["Total"]-$row["alloc"];
}

function amount_total($row)
{
	return -$row["Total"];
}

function check_settled($row)
{
	return $row['settled'] == 1;
}


$sql = get_allocatable_from_supp_sql($supplier_id, $settled);

$cols = array(
	_("Transaction Type") => array('fun'=>'systype_name'),
	_("#") => array('fun'=>'trans_view'),
	_("Reference"), 
	_("Date") => array('type'=>'date', 'ord'=>'asc'),
	_("Supplier") => array('ord'=>''),
	_("Currency") => array('align'=>'center'),
	_("Total") => 'amount', 
	_("Left to Allocate") => array('align'=>'right','insert'=>true, 'fun'=>'amount_left'), 
	array('insert'=>true, 'fun'=>'alloc_link')
	);

if (isset($_POST['customer_id'])) {
	$cols[_("Supplier")] = 'skip';
	$cols[_("Currency")] = 'skip';
}

$table =& new_db_pager('alloc_tbl', $sql, $cols);
$table->set_marker('check_settled', _("Marked items are settled."), 'settledbg', 'settledfg');

if (get_post('_ShowSettled_update') || get_post('_supplier_id_update') ) {
	$table->set_sql($sql);
	$table->set_columns($cols);
	$Ajax->activate('alloc_tbl');
}

	start_form();
	display_db_pager($table);
	end_form();
end_page();


?>