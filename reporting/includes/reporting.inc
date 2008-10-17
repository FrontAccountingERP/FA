<?php
// Link to printing single document with bulk report template file.
// Ex. label_cell(print_document_link($myrow['order_no'], _("Print")), $type);
// or display_note(print_document_link($order_no, _("Print this order")));
// You only need full parameter list for invoices/credit notes

function print_document_link($doc_no, $link_text, $link=true, $type_no)
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
				'PARAM_4' => 0, 
				'PARAM_5' => 0, 
				'PARAM_6' => "");
			break;
		case systypes::cust_dispatch() :
			$rep = 110;
			// from, to, email, comments
			$ar = array(
				'PARAM_0' => $doc_no, 
				'PARAM_1' => $doc_no, 
				'PARAM_2' => 0, 
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
				'PARAM_4' => 0, 
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
				'PARAM_4' => 0, 
				'PARAM_5' => "");
			break;
//		default: $ar = array();
	}
	
	return print_link($link_text, $rep, $ar);
}
//
//	Universal link to any kind of report.
//
function print_link($link_text, $rep, $pars = array(), $dir = '')
{
	global $path_to_root, $pdf_debug;

	$url = $dir == '' ?  $path_to_root.'/reporting/prn_redirect.php?' : $dir;

	foreach($pars as $par => $val) {
		$pars[$par] = "$par=".urlencode($val);
	}
	$pars[] = 'REP_ID='.urlencode($rep);
	$url .= implode ('&', $pars);
	$class = $pdf_debug ? '' : "class='printlink'";
	return "<a $class target='_blank' href='$url'>$link_text</a>";
}

function get_first_bank_account()
{
	$sql = "SELECT ".TB_PREF."bank_accounts.account_code FROM ".TB_PREF."bank_accounts, ".TB_PREF."company
		WHERE bank_curr_code=curr_default LIMIT 0, 1";
	$result = db_query($sql);
	$row = db_fetch_row($result);
	return $row[0];
}
?>