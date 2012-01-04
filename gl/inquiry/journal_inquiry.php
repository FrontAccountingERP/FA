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

$page_security = 'SA_GLANALYTIC';
$path_to_root="../..";

include($path_to_root . "/includes/db_pager.inc");
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(800, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();

page(_($help_context = "Journal Inquiry"), false, false, "", $js);

//-----------------------------------------------------------------------------------
// Ajax updates
//
if (get_post('Search'))
{
	$Ajax->activate('journal_tbl');
}
//--------------------------------------------------------------------------------------
if (!isset($_POST['filterType']))
	$_POST['filterType'] = -1;

start_form();

start_table(TABLESTYLE_NOBORDER);
start_row();

ref_cells(_("Reference:"), 'Ref', '',null, _('Enter reference fragment or leave empty'));

journal_types_list_cells(_("Type:"), "filterType");
date_cells(_("From:"), 'FromDate', '', null, 0, -1, 0);
date_cells(_("To:"), 'ToDate');

check_cells( _("Show closed:"), 'AlsoClosed', null);

submit_cells('Search', _("Search"), '', '', 'default');
end_row();
start_row();
ref_cells(_("Memo:"), 'Memo', '',null, _('Enter memo fragment or leave empty'));
end_row();
end_table();

function journal_pos($row)
{
	return $row['gl_seq'] ? $row['gl_seq'] : '-';
}

function systype_name($dummy, $type)
{
	global $systypes_array;
	
	return $systypes_array[$type];
}

function view_link($row) 
{
	return get_trans_view_str($row["type"], $row["type_no"]);
}

function gl_link($row) 
{
	return get_gl_view_str($row["type"], $row["type_no"]);
}

$editors = array(
	ST_JOURNAL => "/gl/gl_journal.php?ModifyGL=Yes&trans_no=%d&trans_type=%d",
	ST_BANKPAYMENT => "/gl/gl_bank.php?ModifyPayment=Yes&trans_no=%d&trans_type=%d",
	ST_BANKDEPOSIT => "/gl/gl_bank.php?ModifyDeposit=Yes&trans_no=%d&trans_type=%d",
//	4=> Funds Transfer,
   ST_SALESINVOICE => "/sales/customer_invoice.php?ModifyInvoice=%d",
//   11=>
// free hand (debtors_trans.order_==0)
//	"/sales/credit_note_entry.php?ModifyCredit=%d"
// credit invoice
//	"/sales/customer_credit_invoice.php?ModifyCredit=%d"
//	 12=> Customer Payment,
   ST_CUSTDELIVERY => "/sales/customer_delivery.php?ModifyDelivery=%d",
//   16=> Location Transfer,
//   17=> Inventory Adjustment,
//   20=> Supplier Invoice,
//   21=> Supplier Credit Note,
//   22=> Supplier Payment,
//   25=> Purchase Order Delivery,
//   28=> Work Order Issue,
//   29=> Work Order Production",
//   35=> Cost Update,
);

function edit_link($row)
{
	global $editors;

	$ok = true;
	if ($row['type'] == ST_SALESINVOICE)
	{
		$myrow = get_customer_trans($row["type_no"], $row["type"]);
		if ($myrow['alloc'] != 0 || get_voided_entry(ST_SALESINVOICE, $row["type_no"]) !== false)
			$ok = false;
	}		
	return isset($editors[$row["type"]]) && !is_closed_trans($row["type"], $row["type_no"]) && $ok ? 
		pager_link(_("Edit"), 
			sprintf($editors[$row["type"]], $row["type_no"], $row["type"]),
			ICON_EDIT) : '';
}

$sql = get_sql_for_journal_inquiry(get_post('filterType', -1), get_post('FromDate'),
	get_post('ToDate'), get_post('Ref'), get_post('Memo'), check_value('AlsoClosed'));

$cols = array(
	_("#") => array('fun'=>'journal_pos', 'align'=>'center'), 
	_("Date") =>array('name'=>'tran_date','type'=>'date','ord'=>'desc'),
	_("Type") => array('fun'=>'systype_name'), 
	_("Trans #") => array('fun'=>'view_link'), 
	_("Reference"), 
	_("Amount") => array('type'=>'amount'),
	_("Memo"),
	_("User"),
	_("View") => array('insert'=>true, 'fun'=>'gl_link'),
	array('insert'=>true, 'fun'=>'edit_link')
);

if (!check_value('AlsoClosed')) {
	$cols[_("#")] = 'skip';
}

$table =& new_db_pager('journal_tbl', $sql, $cols);

$table->width = "80%";

display_db_pager($table);

end_form();
end_page();

?>
