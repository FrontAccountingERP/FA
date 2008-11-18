<?php

$path_to_root="../..";
$page_security = 3;
include($path_to_root . "/includes/db_pager.inc");
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/sales/includes/sales_ui.inc");
include_once($path_to_root . "/sales/includes/sales_db.inc");
$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
page(_("Customer Allocations"), false, false, "", $js);

//--------------------------------------------------------------------------------
if ($ret = context_restore()) {
	if(isset($ret['customer_id']))
		$_POST['customer_id'] = $ret['customer_id'];
}
if (isset($_POST['_customer_id_editor'])) {
	context_call($path_to_root.'/sales/manage/customers.php?debtor_no='.$_POST['customer_id'] );
}

start_form();
	/* show all outstanding receipts and credits to be allocated */
	/*Clear any previous allocation records */
	if (isset($_SESSION['alloc']))
	{
		unset($_SESSION['alloc']->allocs);
		unset($_SESSION['alloc']);
	}
    if (!isset($_POST['customer_id']))
    	$_POST['customer_id'] = get_global_customer();

    echo "<center>" . _("Select a customer: ") . "&nbsp;&nbsp;";
	customer_list('customer_id', $_POST['customer_id'], true, true);
    echo "<br>";
    check(_("Show Settled Items:"), 'ShowSettled', null, true);
	echo "</center><br><br>";

	set_global_customer($_POST['customer_id']);

	if (isset($_POST['customer_id']) && ($_POST['customer_id'] == reserved_words::get_all()))
	{
		unset($_POST['customer_id']);
	}

	/*if (isset($_POST['customer_id'])) {
		$custCurr = get_customer_currency($_POST['customer_id']);
		if (!is_company_currency($custCurr))
			echo _("Customer Currency:") . $custCurr;
	}*/

	$settled = false;
	if (check_value('ShowSettled'))
		$settled = true;

	$customer_id = null;
	if (isset($_POST['customer_id']))
		$customer_id = $_POST['customer_id'];

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
		"/sales/allocations/customer_allocate.php?trans_no="
			.$row["trans_no"] . "&trans_type=" . $row["type"]		
	);
}

function amount_left($row)
{
	return $row["Total"]-$row["alloc"];
}

function check_settled($row)
{
	return $row['settled'] == 1;
}


$sql = get_allocatable_from_cust_sql($customer_id, $settled);

$cols = array(
	_("Transaction Type") => array('fun'=>'systype_name'),
	_("#") => array('fun'=>'trans_view'),
	_("Reference"), 
	_("Date") => array('type'=>'date', 'ord'=>'asc'),
	_("Customer") => array('ord'=>''),
	_("Currency") => array('align'=>'center'),
	_("Total") => 'amount', 
	_("Left to Allocate") => array('align'=>'right','insert'=>true, 'fun'=>'amount_left'), 
	array('insert'=>true, 'fun'=>'alloc_link')
	);

if (isset($_POST['customer_id'])) {
	$cols[_("Customer")] = 'skip';
	$cols[_("Currency")] = 'skip';
}

$table =& new_db_pager('alloc_tbl', $sql, $cols);
$table->set_marker('check_settled', _("Marked items are settled."), 'settledbg', 'settledfg');

if (get_post('_ShowSettled_update') || get_post('_customer_id_update')) {
	$table->set_sql($sql);
	$table->set_columns($cols);
}

display_db_pager($table);
end_form();

end_page();
?>