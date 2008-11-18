<?php

$page_security=2;
$path_to_root="../..";
include($path_to_root . "/includes/db_pager.inc");
include($path_to_root . "/includes/session.inc");

include($path_to_root . "/purchasing/includes/purchasing_ui.inc");
$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();
page(_("Supplier Allocation Inquiry"), false, false, "", $js);

if (isset($_GET['supplier_id']))
{
	$_POST['supplier_id'] = $_GET['supplier_id'];
}
if (isset($_GET['FromDate']))
{
	$_POST['TransAfterDate'] = $_GET['FromDate'];
}
if (isset($_GET['ToDate']))
{
	$_POST['TransToDate'] = $_GET['ToDate'];
}

//------------------------------------------------------------------------------------------------

start_form(false, true);

if (!isset($_POST['supplier_id']))
	$_POST['supplier_id'] = get_global_supplier();

start_table("class='tablestyle_noborder'");
start_row();

supplier_list_cells(_("Select a supplier: "), 'supplier_id', $_POST['supplier_id'], true);

date_cells(_("From:"), 'TransAfterDate', '', null, -30);
date_cells(_("To:"), 'TransToDate', '', null, 1);

supp_allocations_list_cell("filterType", null);

check_cells(_("show settled:"), 'showSettled', null);

submit_cells('RefreshInquiry', _("Search"),'',_('Refresh Inquiry'), true);

set_global_supplier($_POST['supplier_id']);

end_row();
end_table();
end_form();
//------------------------------------------------------------------------------------------------
function check_overdue($row)
{
	return ($row['TotalAmount']>$row['Allocated']) && 
		$row['OverDue'] == 1;
}

function systype_name($dummy, $type)
{
	return systypes::name($type);
}

function view_link($trans)
{
	return get_trans_view_str($trans["type"], $trans["trans_no"]);
}

function due_date($row)
{
	return (($row["type"] == 20) || ($row["type"]== 21))
		? $row["due_date"] : "";
}

function fmt_balance($row)
{
	$value = ($row["type"] == 1 || $row["type"] == 21 || $row["type"] == 22)
		? -$row["TotalAmount"] - $row["Allocated"]
		: $row["TotalAmount"] - $row["Allocated"];
	return $value;
}

function alloc_link($row)
{
	$link = 
	pager_link(_("Allocations"),
		"/purchasing/allocations/supplier_allocate.php?trans_no=" .
			$row["trans_no"]. "&trans_type=" . $row["type"] );

	return (($row["type"] == 1 || $row["type"] == 21 || $row["type"] == 22) 
		&& (-$row["TotalAmount"] - $row["Allocated"]) > 0)
		? $link : '';
}

function fmt_debit($row)
{
	$value = -$row["TotalAmount"];
	return $value>=0 ? price_format($value) : '';

}

function fmt_credit($row)
{
	$value = $row["TotalAmount"];
	return $value>0 ? price_format($value) : '';
}
//------------------------------------------------------------------------------------------------

 $date_after = date2sql($_POST['TransAfterDate']);
 $date_to = date2sql($_POST['TransToDate']);

    // Sherifoz 22.06.03 Also get the description
    $sql = "SELECT 
		trans.type, 
		trans.trans_no,
		trans.reference, 
		supplier.supp_name, 
		trans.supp_reference,
    	trans.tran_date, 
		trans.due_date,
		supplier.curr_code, 
    	(trans.ov_amount + trans.ov_gst  + trans.ov_discount) AS TotalAmount, 
		trans.alloc AS Allocated,
		((trans.type = 20 OR trans.type = 21) AND trans.due_date < '" . date2sql(Today()) . "') AS OverDue
    	FROM "
			.TB_PREF."supp_trans as trans, "
			.TB_PREF."suppliers as supplier
    	WHERE supplier.supplier_id = trans.supplier_id
     	AND trans.tran_date >= '$date_after'
    	AND trans.tran_date <= '$date_to'";
   	if ($_POST['supplier_id'] != reserved_words::get_all())
   		$sql .= " AND trans.supplier_id = '" . $_POST['supplier_id'] . "'";
   	if (isset($_POST['filterType']) && $_POST['filterType'] != reserved_words::get_all())
   	{
   		if (($_POST['filterType'] == '1') || ($_POST['filterType'] == '2'))
   		{
   			$sql .= " AND trans.type = 20 ";
   		}
   		elseif ($_POST['filterType'] == '3')
   		{
			$sql .= " AND trans.type = 22 ";
   		}
   		elseif (($_POST['filterType'] == '4') || ($_POST['filterType'] == '5'))
   		{
			$sql .= " AND trans.type = 21 ";
   		}

   		if (($_POST['filterType'] == '2') || ($_POST['filterType'] == '5'))
   		{
   			$today =  date2sql(Today());
			$sql .= " AND trans.due_date < '$today' ";
   		}
   	}

   	if (!check_value('showSettled'))
   	{
   		$sql .= " AND (round(abs(ov_amount + ov_gst + ov_discount) - alloc,6) != 0) ";
   	}

$cols = array(
	_("Type") => array('fun'=>'systype_name'),
	_("Number") => array('fun'=>'view_link', 'ord'=>''),
	_("Reference"), 
	_("Supplier") => array('ord'=>''), 
	_("Supp Reference"),
	_("Date") => array('type'=>'date', 'ord'=>'asc'),
	_("Due Date") => array('fun'=>'due_date'),
	_("Currency") => array('align'=>'center'),
	_("Debit") => array('align'=>'right', 'fun'=>'fmt_debit'), 
	_("Credit") => array('align'=>'right', 'insert'=>true, 'fun'=>'fmt_credit'), 
	_("Allocated") => 'amount', 
	_("Balance") => array('type'=>'amount', 'insert'=>true, 'fun'=>'fmt_balance'),
	array('insert'=>true, 'fun'=>'alloc_link')
	);

if ($_POST['supplier_id'] != reserved_words::get_all()) {
	$cols[_("Supplier")] = 'skip';
	$cols[_("Currency")] = 'skip';
}
//------------------------------------------------------------------------------------------------

$table =& new_db_pager('doc_tbl', $sql, $cols);
$table->set_marker('check_overdue', _("Marked items are overdue."));

if (get_post('RefreshInquiry')) {
	$table->set_sql($sql);
	$table->set_columns($cols);
}
start_form();

display_db_pager($table);

end_form();
end_page();
?>
