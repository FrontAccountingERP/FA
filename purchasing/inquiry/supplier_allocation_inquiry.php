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
$page_security = 'SA_SUPPLIERALLOC';
$path_to_root = "../..";
include($path_to_root . "/includes/db_pager.inc");
include($path_to_root . "/includes/session.inc");

include($path_to_root . "/purchasing/includes/purchasing_ui.inc");
$js = "";
if ($SysPrefs->use_popup_windows)
	$js .= get_js_open_window(900, 500);
if (user_use_date_picker())
	$js .= get_js_date_picker();
page(_($help_context = "Supplier Allocation Inquiry"), false, false, "", $js);

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

start_form();

if (!isset($_POST['supplier_id']))
	$_POST['supplier_id'] = get_global_supplier();

start_table(TABLESTYLE_NOBORDER);
start_row();

supplier_list_cells(_("Select a supplier: "), 'supplier_id', $_POST['supplier_id'], true);

date_cells(_("From:"), 'TransAfterDate', '', null, -user_transaction_days());
date_cells(_("To:"), 'TransToDate', '', null, 1);

supp_allocations_list_cell("filterType", null);

check_cells(_("show settled:"), 'showSettled', null);

submit_cells('RefreshInquiry', _("Search"),'',_('Refresh Inquiry'), 'default');

set_global_supplier($_POST['supplier_id']);

end_row();
end_table();
//------------------------------------------------------------------------------------------------
function check_overdue($row)
{
	return ($row['TotalAmount']>$row['Allocated']) && 
		$row['OverDue'] == 1;
}

function systype_name($dummy, $type)
{
	global $systypes_array;
	
	return $systypes_array[$type];
}

function view_link($trans)
{
	return get_trans_view_str($trans["type"], $trans["trans_no"]);
}

function due_date($row)
{
	return (($row["type"] == ST_SUPPINVOICE) || ($row["type"]== ST_SUPPCREDIT))
		? $row["due_date"] : "";
}

function fmt_balance($row)
{
	$value = ($row["type"] == ST_BANKPAYMENT || $row["type"] == ST_SUPPCREDIT || $row["type"] == ST_SUPPAYMENT)	? -$row["TotalAmount"] - $row["Allocated"]
		: ($row["type"] == ST_JOURNAL ? abs($row["TotalAmount"]) - $row["Allocated"] :
			$row["TotalAmount"] - $row["Allocated"]);
	return $value;
}

function alloc_link($row)
{
	$link = 
	pager_link(_("Allocations"),
		"/purchasing/allocations/supplier_allocate.php?trans_no=" .
			$row["trans_no"]. "&trans_type=" . $row["type"]. "&supplier_id=" . $row["supplier_id"], ICON_ALLOC );

	if ($row["type"] == ST_BANKPAYMENT || $row["type"] == ST_SUPPAYMENT ||
		(($row["type"] == ST_SUPPCREDIT || $row["type"] == ST_JOURNAL) && $row["TotalAmount"] < 0))
		return floatcmp(-$row["TotalAmount"], $row["Allocated"]) ? $link : '';

	$link = 
	pager_link(_("Payment"),
		"/purchasing/supplier_payment.php?supplier_id=".$row["supplier_id"]."&PInvoice=" 
			. $row["trans_no"]."&trans_type=" . $row["type"], ICON_MONEY);

	if ($row["type"] == ST_SUPPINVOICE || (($row["type"] == ST_SUPPCREDIT || $row["type"] == ST_JOURNAL) && $row["TotalAmount"] > 0))
		return floatcmp($row["TotalAmount"], $row["Allocated"]) ? $link : '';


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

$sql = get_sql_for_supplier_allocation_inquiry(get_post('TransAfterDate'),get_post('TransToDate'),
	get_post('filterType'), get_post('supplier_id'), check_value('showSettled'));

$cols = array(
	_("Type") => array('fun'=>'systype_name'),
	_("#") => array('fun'=>'view_link', 'ord'=>'', 'align'=>'right'),
	_("Reference"), 
	_("Supplier") => array('ord'=>''), 
	_("Supp Reference"),
	_("Date") => array('name'=>'tran_date', 'type'=>'date', 'ord'=>'asc'),
	_("Due Date") => array('type'=>'date', 'fun'=>'due_date'),
	_("Currency") => array('align'=>'center'),
	_("Debit") => array('align'=>'right', 'fun'=>'fmt_debit'), 
	_("Credit") => array('align'=>'right', 'insert'=>true, 'fun'=>'fmt_credit'), 
	_("Allocated") => 'amount', 
	_("Balance") => array('type'=>'amount', 'insert'=>true, 'fun'=>'fmt_balance'),
	array('insert'=>true, 'fun'=>'alloc_link')
	);

if ($_POST['supplier_id'] != ALL_TEXT) {
	$cols[_("Supplier")] = 'skip';
	$cols[_("Currency")] = 'skip';
}
//------------------------------------------------------------------------------------------------

$table =& new_db_pager('doc_tbl', $sql, $cols);
$table->set_marker('check_overdue', _("Marked items are overdue."));

$table->width = "90%";

display_db_pager($table);

end_form();
end_page();
