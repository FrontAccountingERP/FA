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
page(_("Supplier Inquiry"), false, false, "", $js);

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

start_form(false, true);

if (!isset($_POST['supplier_id']))
	$_POST['supplier_id'] = get_global_supplier();

start_table("class='tablestyle_noborder'");
start_row();

supplier_list_cells(_("Select a supplier:"), 'supplier_id', null, true);

date_cells(_("From:"), 'TransAfterDate', '', null, -30);
date_cells(_("To:"), 'TransToDate');

supp_allocations_list_cell("filterType", null);

submit_cells('RefreshInquiry', _("Search"),'',_('Refresh Inquiry'), true);

end_row();
end_table();
end_form();
set_global_supplier($_POST['supplier_id']);

//------------------------------------------------------------------------------------------------

function display_supplier_summary($supplier_record)
{
	global $table_style;

	$past1 = get_company_pref('past_due_days');
	$past2 = 2 * $past1;
	$nowdue = "1-" . $past1 . " " . _('Days');
	$pastdue1 = $past1 + 1 . "-" . $past2 . " " . _('Days');
	$pastdue2 = _('Over') . " " . $past2 . " " . _('Days');
	

    start_table("width=80% $table_style");
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
if (($_POST['supplier_id'] != "") && ($_POST['supplier_id'] != reserved_words::get_all()))
{
	$supplier_record = get_supplier_details($_POST['supplier_id']);
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
	return systypes::name($type);
}

function trans_view($trans)
{
	return get_trans_view_str($trans["type"], $trans["trans_no"]);
}

function due_date($row)
{
	return ($row["type"]== 20) || ($row["type"]== 21)
		 ? $row["due_date"] : '';
}

function gl_view($row)
{
	return get_gl_view_str($row["type"], $row["trans_no"]);
}

function credit_link($row)
{
	return $row['type'] == 20 && $row["TotalAmount"] - $row["Allocated"] > 0 ?
		pager_link(_("Credit This"),
			"/purchasing/supplier_credit.php?New=1&invoice_no=".
			$row['trans_no'], ICON_CREDIT)
			: '';
}

function fmt_debit($row)
{
	$value = $row["TotalAmount"];
	return $value>=0 ? price_format($value) : '';

}

function fmt_credit($row)
{
	$value = -$row["TotalAmount"];
	return $value>0 ? price_format($value) : '';
}

function prt_link($row)
{
 		return print_document_link($row['trans_no'], _("Print"), true, $row['type']);
}

function check_overdue($row)
{
	return $row['OverDue'] == 1
		&& (abs($row["TotalAmount"]) - $row["Allocated"] != 0);
}
//------------------------------------------------------------------------------------------------

    $date_after = date2sql($_POST['TransAfterDate']);
    $date_to = date2sql($_POST['TransToDate']);

    // Sherifoz 22.06.03 Also get the description
    $sql = "SELECT trans.type, 
		trans.trans_no,
		trans.reference, 
		supplier.supp_name, 
		trans.supp_reference,
    	trans.tran_date, 
		trans.due_date,
		supplier.curr_code, 
    	(trans.ov_amount + trans.ov_gst  + trans.ov_discount) AS TotalAmount, 
		trans.alloc AS Allocated,
		((trans.type = 20 OR trans.type = 21) AND trans.due_date < '" . date2sql(Today()) . "') AS OverDue,
    	(ABS(trans.ov_amount + trans.ov_gst  + trans.ov_discount - trans.alloc) <= 0.005) AS Settled
    	FROM ".TB_PREF."supp_trans as trans, ".TB_PREF."suppliers as supplier
    	WHERE supplier.supplier_id = trans.supplier_id
     	AND trans.tran_date >= '$date_after'
    	AND trans.tran_date <= '$date_to'";
   	if ($_POST['supplier_id'] != reserved_words::get_all())
   		$sql .= " AND trans.supplier_id = '" . $_POST['supplier_id'] . "'";
   	if (isset($_POST['filterType']) && $_POST['filterType'] != reserved_words::get_all())
   	{
   		if (($_POST['filterType'] == '1')) 
   		{
   			$sql .= " AND (trans.type = 20 OR trans.type = 2)";
   		} 
   		elseif (($_POST['filterType'] == '2')) 
   		{
   			$sql .= " AND trans.type = 20 ";
   		} 
   		elseif ($_POST['filterType'] == '3') 
   		{
			$sql .= " AND (trans.type = 22 OR trans.type = 1) ";
   		} 
   		elseif (($_POST['filterType'] == '4') || ($_POST['filterType'] == '5')) 
   		{
			$sql .= " AND trans.type = 21  ";
   		}

   		if (($_POST['filterType'] == '2') || ($_POST['filterType'] == '5')) 
   		{
   			$today =  date2sql(Today());
			$sql .= " AND trans.due_date < '$today' ";
   		}
   	}

$cols = array(
			_("Type") => array('fun'=>'systype_name', 'ord'=>''), 
			_("#") => array('fun'=>'trans_view', 'ord'=>''), 
			_("Reference"), 
			_("Supplier"),
			_("Supplier's Reference"), 
			_("Date") => array('type'=>'date', 'ord'=>'desc'), 
			_("Due Date") => array('type'=>'date', 'fun'=>'due_date'), 
			_("Currency") => array('align'=>'center'),
			_("Debit") => array('align'=>'right', 'fun'=>'fmt_debit'), 
			_("Credit") => array('align'=>'right', 'insert'=>true,'fun'=>'fmt_credit'), 
			array('insert'=>true, 'fun'=>'gl_view'),
			array('insert'=>true, 'fun'=>'credit_link')
			);

if ($_POST['supplier_id'] != reserved_words::get_all())
{
	$cols[_("Supplier")] = 'skip';
	$cols[_("Currency")] = 'skip';
}
//------------------------------------------------------------------------------------------------


/*show a table of the transactions returned by the sql */
$table =& new_db_pager('trans_tbl', $sql, $cols);
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
