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

$page_security = $_POST['PARAM_0'] == $_POST['PARAM_1'] ?
	'SA_SALESTRANSVIEW' : 'SA_SALESBULKREP';
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	Purchase Orders
// ----------------------------------------------------------------
$path_to_root="..";

include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/data_checks.inc");

//----------------------------------------------------------------------------------------------------

print_receipts();

//----------------------------------------------------------------------------------------------------
function get_receipt($type, $trans_no)
{
    $sql = "SELECT ".TB_PREF."debtor_trans.*,
				(".TB_PREF."debtor_trans.ov_amount + ".TB_PREF."debtor_trans.ov_gst + ".TB_PREF."debtor_trans.ov_freight + 
				".TB_PREF."debtor_trans.ov_freight_tax + ".TB_PREF."debtor_trans.ov_discount) AS Total, 
   				".TB_PREF."debtors_master.name AS DebtorName,  ".TB_PREF."debtors_master.debtor_ref,
   				".TB_PREF."debtors_master.curr_code, ".TB_PREF."debtors_master.payment_terms, ".TB_PREF."debtors_master.tax_id AS tax_id, 
   				".TB_PREF."debtors_master.email, ".TB_PREF."debtors_master.address
    			FROM ".TB_PREF."debtor_trans, ".TB_PREF."debtors_master 
				WHERE ".TB_PREF."debtor_trans.debtor_no = ".TB_PREF."debtors_master.debtor_no
				AND ".TB_PREF."debtor_trans.type = ".db_escape($type)."
				AND ".TB_PREF."debtor_trans.trans_no = ".db_escape($trans_no);
   	$result = db_query($sql, "The remittance cannot be retrieved");
   	if (db_num_rows($result) == 0)
   		return false;
    return db_fetch($result);
}

function get_allocations_for_receipt($debtor_id, $type, $trans_no)
{
	$sql = get_alloc_trans_sql("amt, trans.reference, trans.alloc", "trans.trans_no = alloc.trans_no_to
		AND trans.type = alloc.trans_type_to
		AND alloc.trans_no_from=$trans_no
		AND alloc.trans_type_from=$type
		AND trans.debtor_no=".db_escape($debtor_id),
		TB_PREF."cust_allocations as alloc");
	$sql .= " ORDER BY trans_no";
	return db_query($sql, "Cannot retreive alloc to transactions");
}

function print_receipts()
{
	global $path_to_root, $systypes_array;

	include_once($path_to_root . "/reporting/includes/pdf_report.inc");

	$from = $_POST['PARAM_0'];
	$to = $_POST['PARAM_1'];
	$currency = $_POST['PARAM_2'];
	$comments = $_POST['PARAM_3'];

	if ($from == null)
		$from = 0;
	if ($to == null)
		$to = 0;
	$dec = user_price_dec();

 	$fno = explode("-", $from);
	$tno = explode("-", $to);

	$cols = array(4, 85, 150, 225, 275, 360, 450, 515);

	// $headers in doctext.inc
	$aligns = array('left',	'left',	'left', 'left', 'right', 'right', 'right');

	$params = array('comments' => $comments);

	$cur = get_company_Pref('curr_default');

	$rep = new FrontReport(_('RECEIPT'), "ReceiptBulk", user_pagesize());
	$rep->currency = $cur;
	$rep->Font();
	$rep->Info($params, $cols, null, $aligns);

	for ($i = $fno[0]; $i <= $tno[0]; $i++)
	{
		if ($fno[0] == $tno[0])
			$types = array($fno[1]);
		else
			$types = array(ST_BANKDEPOSIT, ST_CUSTPAYMENT, ST_CUSTCREDIT);
		foreach ($types as $j)
		{
			$myrow = get_receipt($j, $i);
			if (!$myrow)
				continue;			
			$baccount = get_default_bank_account($myrow['curr_code']);
			$params['bankaccount'] = $baccount['id'];

			$rep->title = _('RECEIPT');
			$rep->Header2($myrow, null, $myrow, $baccount, ST_CUSTPAYMENT);
			$result = get_allocations_for_receipt($myrow['debtor_no'], $myrow['type'], $myrow['trans_no']);

			$linetype = true;
			$doctype = ST_CUSTPAYMENT;
			if ($rep->currency != $myrow['curr_code'])
			{
				include($path_to_root . "/reporting/includes/doctext2.inc");
			}
			else
			{
				include($path_to_root . "/reporting/includes/doctext.inc");
			}

			$total_allocated = 0;
			$rep->TextCol(0, 4,	$doc_Towards, -2);
			$rep->NewLine(2);
			
			while ($myrow2=db_fetch($result))
			{
				$rep->TextCol(0, 1,	$systypes_array[$myrow2['type']], -2);
				$rep->TextCol(1, 2,	$myrow2['reference'], -2);
				$rep->TextCol(2, 3,	sql2date($myrow2['tran_date']), -2);
				$rep->TextCol(3, 4,	sql2date($myrow2['due_date']), -2);
				$rep->AmountCol(4, 5, $myrow2['Total'], $dec, -2);
				$rep->AmountCol(5, 6, $myrow2['Total'] - $myrow2['alloc'], $dec, -2);
				$rep->AmountCol(6, 7, $myrow2['amt'], $dec, -2);

				$total_allocated += $myrow2['amt'];
				$rep->NewLine(1);
				if ($rep->row < $rep->bottomMargin + (15 * $rep->lineHeight))
					$rep->Header2($myrow, null, $myrow, $baccount, ST_CUSTPAYMENT);
			}

			$rep->row = $rep->bottomMargin + (15 * $rep->lineHeight);

			$rep->TextCol(3, 6, $doc_Total_Allocated, -2);
			$rep->AmountCol(6, 7, $total_allocated, $dec, -2);
			$rep->NewLine();
			$rep->TextCol(3, 6, $doc_Left_To_Allocate, -2);
			$rep->AmountCol(6, 7, $myrow['Total'] - $total_allocated, $dec, -2);
			$rep->NewLine();
			$rep->Font('bold');
			$rep->TextCol(3, 6, $doc_Total_Payment, - 2);
			$rep->AmountCol(6, 7, $myrow['Total'], $dec, -2);
			$words = price_in_words($myrow['Total'], ST_CUSTPAYMENT);
			if ($words != "")
			{
				$rep->NewLine(1);
				$rep->TextCol(0, 7, $myrow['curr_code'] . ": " . $words, - 2);
			}	
			$rep->Font();
			$rep->NewLine();
			$rep->TextCol(6, 7, $doc_Received, - 2);
			$rep->NewLine();
			$rep->TextCol(0, 2, $doc_by_Cheque, - 2);
			$rep->TextCol(2, 4, "______________________________", - 2);
			$rep->TextCol(4, 5, $doc_Dated, - 2);
			$rep->TextCol(5, 6, "__________________", - 2);
			$rep->NewLine(1);
			$rep->TextCol(0, 2, $doc_Drawn, - 2);
			$rep->TextCol(2, 4, "______________________________", - 2);
			$rep->TextCol(4, 5, $doc_Drawn_Branch, - 2);
			$rep->TextCol(5, 6, "__________________", - 2);
			$rep->TextCol(6, 7, "__________________");
		}	
	}
	$rep->End();
}

?>