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
$page_security = 'SA_SUPPTRANSVIEW';
$path_to_root = "../..";
include_once($path_to_root . "/includes/db_pager.inc");
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/purchasing/includes/purchasing_ui.inc");
include_once($path_to_root . "/reporting/includes/reporting.inc");

if (!@$_GET['popup'])
{
	$js = "";
	if ($use_popup_windows)
		$js .= get_js_open_window(900, 500);
	if ($use_date_picker)
		$js .= get_js_date_picker();
	page(_($help_context = "Supplier Inquiry"), isset($_GET['supplier_id']), false, "", $js);
}
if (isset($_GET['supplier_id'])){
	$_POST['supplier_id'] = $_GET['supplier_id'];
}
if (isset($_GET['FromDate'])){
	$_POST['TransAfterDate'] = $_GET['FromDate'];
}
if (isset($_GET['ToDate'])){
	$_POST['TransToDate'] = $_GET['ToDate'];
}

//------------------------------------------------------------------------------------------------

if (!@$_GET['popup'])
	start_form();

if (!isset($_POST['supplier_id']))
	$_POST['supplier_id'] = get_global_supplier();

start_table(TABLESTYLE_NOBORDER);
start_row();

if (!@$_GET['popup'])
	supplier_list_cells(_("Select a supplier:"), 'supplier_id', null, true, false, false, !@$_GET['popup']);

date_cells(_("From:"), 'TransAfterDate', '', null, -30);
date_cells(_("To:"), 'TransToDate');

supp_transactions_list_cell("filterType", null, true);

submit_cells('RefreshInquiry', _("Search"),'',_('Refresh Inquiry'), 'default');

end_row();
end_table();
set_global_supplier($_POST['supplier_id']);

//------------------------------------------------------------------------------------------------

function display_supplier_summary($supplier_record)
{
	$past1 = get_company_pref('past_due_days');
	$past2 = 2 * $past1;
	$nowdue = "1-" . $past1 . " " . _('Days');
	$pastdue1 = $past1 + 1 . "-" . $past2 . " " . _('Days');
	$pastdue2 = _('Over') . " " . $past2 . " " . _('Days');
	

    start_table(TABLESTYLE, "width='80%'");
    $th = array(_("Currency"), _("Terms"), _("Current"), $nowdue,
    	$pastdue1, $pastdue2, _("Total Balance"));

	table_header($th);
    start_row();
	label_cell($supplier_record["curr_code"]);
    label_cell($supplier_record["terms"]);
    amount_cell($supplier_record["Balance"] - $supplier_record["Due"]);
    amount_cell($supplier_record["Due"] - $supplier_record["Overdue1"]);
    amount_cell($supplier_record["Overdue1"] - $supplier_record["Overdue2"]);
    amount_cell($supplier_record["Overdue2"]);
    amount_cell($supplier_record["Balance"]);
    end_row();
    end_table(1);
}
//------------------------------------------------------------------------------------------------

div_start('totals_tbl');
if (($_POST['supplier_id'] != "") && ($_POST['supplier_id'] != ALL_TEXT))
{
	$supplier_record = get_supplier_details($_POST['supplier_id'], $_POST['TransToDate']);
    display_supplier_summary($supplier_record);
}
div_end();

if(get_post('RefreshInquiry'))
{
	$Ajax->activate('totals_tbl');
}

//------------------------------------------------------------------------------------------------
function systype_name($dummy, $type)
{
	global $systypes_array;
	return $systypes_array[$type];
}

function trans_view($trans)
{
	return get_trans_view_str($trans["type"], $trans["trans_no"]);
}

function due_date($row)
{
	return ($row["type"]== ST_SUPPINVOICE) || ($row["type"]== ST_SUPPCREDIT) ? $row["due_date"] : '';
}

function gl_view($row)
{
	return get_gl_view_str($row["type"], $row["trans_no"]);
}

function credit_link($row)
{
	if (@$_GET['popup'])
		return '';
	return $row['type'] == ST_SUPPINVOICE && $row["TotalAmount"] - $row["Allocated"] > 0 ?
		pager_link(_("Credit This"),
			"/purchasing/supplier_credit.php?New=1&invoice_no=".
			$row['trans_no'], ICON_CREDIT)
			: '';
}

function fmt_debit($row)
{
	$value = $row["TotalAmount"];
	return $value>0 ? price_format($value) : '';

}

function fmt_credit($row)
{
	$value = -$row["TotalAmount"];
	return $value>0 ? price_format($value) : '';
}

function prt_link($row)
{
  	if ($row['type'] == ST_SUPPAYMENT || $row['type'] == ST_BANKPAYMENT || $row['type'] == ST_SUPPCREDIT) 
 		return print_document_link($row['trans_no']."-".$row['type'], _("Print Remittance"), true, ST_SUPPAYMENT, ICON_PRINT);
}

function check_overdue($row)
{
	return $row['OverDue'] == 1
		&& (abs($row["TotalAmount"]) - $row["Allocated"] != 0);
}
//------------------------------------------------------------------------------------------------

$sql = get_sql_for_supplier_inquiry($_POST['filterType'], $_POST['TransAfterDate'], $_POST['TransToDate'], $_POST['supplier_id']);

$cols = array(
			_("Type") => array('fun'=>'systype_name', 'ord'=>''), 
			_("#") => array('fun'=>'trans_view', 'ord'=>''), 
			_("Reference"), 
			_("Supplier"),
			_("Supplier's Reference"), 
			_("Date") => array('name'=>'tran_date', 'type'=>'date', 'ord'=>'desc'), 
			_("Due Date") => array('type'=>'date', 'fun'=>'due_date'), 
			_("Currency") => array('align'=>'center'),
			_("Debit") => array('align'=>'right', 'fun'=>'fmt_debit'), 
			_("Credit") => array('align'=>'right', 'insert'=>true,'fun'=>'fmt_credit'), 
			array('insert'=>true, 'fun'=>'gl_view'),
			array('insert'=>true, 'fun'=>'credit_link'),
			array('insert'=>true, 'fun'=>'prt_link')
			);

if ($_POST['supplier_id'] != ALL_TEXT)
{
	$cols[_("Supplier")] = 'skip';
	$cols[_("Currency")] = 'skip';
}
//------------------------------------------------------------------------------------------------


/*show a table of the transactions returned by the sql */
$table =& new_db_pager('trans_tbl', $sql, $cols);
$table->set_marker('check_overdue', _("Marked items are overdue."));

$table->width = "85%";

display_db_pager($table);

if (!@$_GET['popup'])
{
	end_form();
	end_page(@$_GET['popup'], false, false);
}
?>
