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
// Link to printing single document with bulk report template file.
// Ex. label_cell(print_document_link($myrow['order_no'], _("Print")), $type);
// or display_note(print_document_link($order_no, _("Print this order")));
// You only need full parameter list for invoices/credit notes

function print_document_link($doc_no, $link_text, $link=true, $type_no, 
	$icon=false, $class='printlink', $id='', $email=0)
{
	global $path_to_root;
	include_once($path_to_root . "/includes/types.inc");

	$url = $path_to_root.'/reporting/prn_redirect.php?';

	switch ($type_no)
	{
		case systypes::sales_order() :
			$rep = 109;
			// from, to, currency, bank acc, email, quote, comments
			$ar = array(
				'PARAM_0' => $doc_no, 
				'PARAM_1' => $doc_no, 
				'PARAM_2' => "", 
				'PARAM_3' => get_first_bank_account(), 
				'PARAM_4' => $email, 
				'PARAM_5' => 0, 
				'PARAM_6' => "");
			break;
		case systypes::cust_dispatch() :
			$rep = 110;
			// from, to, email, comments
			$ar = array(
				'PARAM_0' => $doc_no, 
				'PARAM_1' => $doc_no, 
				'PARAM_2' => $email, 
				'PARAM_3' => "");
			break;
		case 10 : // Sales Invoice
		case 11 : // Customer Credit Note
			$rep = 107;
			// from, to, currency, bank acc, email, paylink, comments, type
			$ar = array(
				'PARAM_0' => $doc_no, 
				'PARAM_1' => $doc_no, 
				'PARAM_2' => "", 
				'PARAM_3' => get_first_bank_account(), 
				'PARAM_4' => $email, 
				'PARAM_5' => "",
				'PARAM_6' => "", 
				'PARAM_7' => $type_no);
			break;
		case systypes::po() :
			$rep = 209;
			// from, to, currency, bank acc, email, comments
			$ar = array(
				'PARAM_0' => $doc_no, 
				'PARAM_1' => $doc_no, 
				'PARAM_2' => "", 
				'PARAM_3' => get_first_bank_account(), 
				'PARAM_4' => $email, 
				'PARAM_5' => "");
			break;
//		default: $ar = array();
	}
	
	return print_link($link_text, $rep, $ar, "", $icon, $class, $id);
}
//
//	Universal link to any kind of report.
//
function print_link($link_text, $rep, $pars = array(), $dir = '', 
	$icon=false, $class='printlink', $id='')
{
	global $path_to_root, $pdf_debug;

	$url = $dir == '' ?  $path_to_root.'/reporting/prn_redirect.php?' : $dir;

	$id = default_focus($id);
	foreach($pars as $par => $val) {
		$pars[$par] = "$par=".urlencode($val);
	}
	$pars[] = 'REP_ID='.urlencode($rep);
	$url .= implode ('&', $pars);

	if ($class != '')
		$class = $pdf_debug ? '' : " class='$class'";
	if ($id != '')
		$id = " id='$id'";
	$pars = access_string($link_text);
	if (user_graphic_links() && $icon)
		$pars[0] = set_icon($icon, $pars[0]);
	return "<a target='_blank' href='$url'$id$class $pars[1]>$pars[0]</a>";
}

function get_first_bank_account()
{
	$sql = "SELECT ".TB_PREF."bank_accounts.id FROM ".TB_PREF."bank_accounts, ".TB_PREF."company
		WHERE bank_curr_code=curr_default LIMIT 0, 1";
	$result = db_query($sql);
	$row = db_fetch_row($result);
	return $row[0];
}
?>