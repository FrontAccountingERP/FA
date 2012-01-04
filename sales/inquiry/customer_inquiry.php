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
$page_security = 'SA_SALESTRANSVIEW';
$path_to_root = "../..";
include_once($path_to_root . "/includes/db_pager.inc");
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/sales/includes/sales_ui.inc");
include_once($path_to_root . "/sales/includes/sales_db.inc");
include_once($path_to_root . "/reporting/includes/reporting.inc");

if (!@$_GET['popup'])
{
	$js = "";
	if ($use_popup_windows)
		$js .= get_js_open_window(900, 500);
	if ($use_date_picker)
		$js .= get_js_date_picker();
	page(_($help_context = "Customer Transactions"), isset($_GET['customer_id']), false, "", $js);
}

if (isset($_GET['customer_id']))
{
	$_POST['customer_id'] = $_GET['customer_id'];
}

//------------------------------------------------------------------------------------------------

if (!@$_GET['popup'])
	start_form();

if (!isset($_POST['customer_id']))
	$_POST['customer_id'] = get_global_customer();

start_table(TABLESTYLE_NOBORDER);
start_row();

if (!@$_GET['popup'])
	customer_list_cells(_("Select a customer: "), 'customer_id', null, true, false, false, !@$_GET['popup']);

date_cells(_("From:"), 'TransAfterDate', '', null, -30);
date_cells(_("To:"), 'TransToDate', '', null, 1);

if (!isset($_POST['filterType']))
	$_POST['filterType'] = 0;

cust_allocations_list_cells(null, 'filterType', $_POST['filterType'], true);

submit_cells('RefreshInquiry', _("Search"),'',_('Refresh Inquiry'), 'default');
end_row();
end_table();

set_global_customer($_POST['customer_id']);

//------------------------------------------------------------------------------------------------

function display_customer_summary($customer_record)
{
	$past1 = get_company_pref('past_due_days');
	$past2 = 2 * $past1;
    if ($customer_record["dissallow_invoices"] != 0)
    {
    	echo "<center><font color=red size=4><b>" . _("CUSTOMER ACCOUNT IS ON HOLD") . "</font></b></center>";
    }

	$nowdue = "1-" . $past1 . " " . _('Days');
	$pastdue1 = $past1 + 1 . "-" . $past2 . " " . _('Days');
	$pastdue2 = _('Over') . " " . $past2 . " " . _('Days');

    start_table(TABLESTYLE, "width=80%");
    $th = array(_("Currency"), _("Terms"), _("Current"), $nowdue,
    	$pastdue1, $pastdue2, _("Total Balance"));
    table_header($th);

	start_row();
    label_cell($customer_record["curr_code"]);
    label_cell($customer_record["terms"]);
	amount_cell($customer_record["Balance"] - $customer_record["Due"]);
	amount_cell($customer_record["Due"] - $customer_record["Overdue1"]);
	amount_cell($customer_record["Overdue1"] - $customer_record["Overdue2"]);
	amount_cell($customer_record["Overdue2"]);
	amount_cell($customer_record["Balance"]);
	end_row();

	end_table();
}
//------------------------------------------------------------------------------------------------

div_start('totals_tbl');
if ($_POST['customer_id'] != "" && $_POST['customer_id'] != ALL_TEXT)
{
	$customer_record = get_customer_details($_POST['customer_id'], $_POST['TransToDate']);
    display_customer_summary($customer_record);
    echo "<br>";
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

function order_view($row)
{
	return $row['order_']>0 ?
		get_customer_trans_view_str(ST_SALESORDER, $row['order_'])
		: "";
}

function trans_view($trans)
{
	return get_trans_view_str($trans["type"], $trans["trans_no"]);
}

function due_date($row)
{
	return	$row["type"] == ST_SALESINVOICE	? $row["due_date"] : '';
}

function gl_view($row)
{
	return get_gl_view_str($row["type"], $row["trans_no"]);
}

function fmt_debit($row)
{
	$value =
	    $row['type']==ST_CUSTCREDIT || $row['type']==ST_CUSTPAYMENT || $row['type']==ST_BANKDEPOSIT ?
		-$row["TotalAmount"] : $row["TotalAmount"];
	return $value>=0 ? price_format($value) : '';

}

function fmt_credit($row)
{
	$value =
	    !($row['type']==ST_CUSTCREDIT || $row['type']==ST_CUSTPAYMENT || $row['type']==ST_BANKDEPOSIT) ?
		-$row["TotalAmount"] : $row["TotalAmount"];
	return $value>0 ? price_format($value) : '';
}

function credit_link($row)
{
	if (@$_GET['popup'])
		return '';
	return $row['type'] == ST_SALESINVOICE && $row["Outstanding"] > 0 ?
		pager_link(_("Credit This") ,
			"/sales/customer_credit_invoice.php?InvoiceNumber=". $row['trans_no'], ICON_CREDIT):'';
}

function edit_link($row)
{
	$str = '';

	if (@$_GET['popup'])
		return '';
	switch($row['type']) {
	case ST_SALESINVOICE:
		if (get_voided_entry(ST_SALESINVOICE, $row["trans_no"]) === false && $row['Allocated'] == 0)
			$str = "/sales/customer_invoice.php?ModifyInvoice=".$row['trans_no'];
		break;
	case ST_CUSTCREDIT:
  		if (get_voided_entry(ST_CUSTCREDIT, $row["trans_no"]) === false && $row['Allocated'] == 0) // 2008-11-19 Joe Hunt
		{	 
			if ($row['order_']==0) // free-hand credit note
			    $str = "/sales/credit_note_entry.php?ModifyCredit=".$row['trans_no'];
			else	// credit invoice
			    $str = "/sales/customer_credit_invoice.php?ModifyCredit=".$row['trans_no'];
		}	    
		break;
	case ST_CUSTDELIVERY:
  		if (get_voided_entry(ST_CUSTDELIVERY, $row["trans_no"]) === false)
   			$str = "/sales/customer_delivery.php?ModifyDelivery=".$row['trans_no'];
		break;
	case ST_CUSTPAYMENT:
  		if (get_voided_entry(ST_CUSTPAYMENT, $row["trans_no"]) === false)
   			$str = "/sales/customer_payments.php?trans_no=".$row['trans_no'];
		break;
	}		
	if ($str != "" && !is_closed_trans($row['type'], $row["trans_no"]))
		return pager_link(_('Edit'), $str, ICON_EDIT);
	return '';	
}

function prt_link($row)
{
  	if ($row['type'] == ST_CUSTPAYMENT || $row['type'] == ST_BANKDEPOSIT) 
		return print_document_link($row['trans_no']."-".$row['type'], _("Print Receipt"), true, ST_CUSTPAYMENT, ICON_PRINT);
  	elseif ($row['type'] == ST_BANKPAYMENT) // bank payment printout not defined yet.
		return '';
 	else	
 		return print_document_link($row['trans_no']."-".$row['type'], _("Print"), true, $row['type'], ICON_PRINT);
}

function check_overdue($row)
{
	return $row['OverDue'] == 1
		&& floatcmp($row["TotalAmount"], $row["Allocated"]) != 0;
}
//------------------------------------------------------------------------------------------------
$sql = get_sql_for_customer_inquiry();

//------------------------------------------------------------------------------------------------
db_query("set @bal:=0");

$cols = array(
	_("Type") => array('fun'=>'systype_name', 'ord'=>''),
	_("#") => array('fun'=>'trans_view', 'ord'=>''),
	_("Order") => array('fun'=>'order_view'), 
	_("Reference"), 
	_("Date") => array('name'=>'tran_date', 'type'=>'date', 'ord'=>'desc'),
	_("Due Date") => array('type'=>'date', 'fun'=>'due_date'),
	_("Customer") => array('ord'=>''), 
	_("Branch") => array('ord'=>''), 
	_("Currency") => array('align'=>'center'),
	_("Debit") => array('align'=>'right', 'fun'=>'fmt_debit'), 
	_("Credit") => array('align'=>'right','insert'=>true, 'fun'=>'fmt_credit'), 
	_("RB") => array('align'=>'right', 'type'=>'amount'),
		array('insert'=>true, 'fun'=>'gl_view'),
		array('insert'=>true, 'fun'=>'credit_link'),
		array('insert'=>true, 'fun'=>'edit_link'),
		array('insert'=>true, 'fun'=>'prt_link')
	);


if ($_POST['customer_id'] != ALL_TEXT) {
	$cols[_("Customer")] = 'skip';
	$cols[_("Currency")] = 'skip';
}
if ($_POST['filterType'] == ALL_TEXT)
	$cols[_("RB")] = 'skip';

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
