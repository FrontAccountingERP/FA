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
// Title:	Receipts
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
				".TB_PREF."debtor_trans.ov_freight_tax) AS Total,
				".TB_PREF."debtor_trans.ov_discount,
   				".TB_PREF."debtors_master.name AS DebtorName,  ".TB_PREF."debtors_master.debtor_ref,
   				".TB_PREF."debtors_master.curr_code, ".TB_PREF."debtors_master.payment_terms, "
   				.TB_PREF."debtors_master.tax_id AS tax_id,
   				".TB_PREF."debtors_master.address
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
	$orientation = $_POST['PARAM_4'];

	if (!$from || !$to) return;

	$orientation = ($orientation ? 'L' : 'P');
	$dec = user_price_dec();

 	$fno = explode("-", $from);
	$tno = explode("-", $to);
	$from = min($fno[0], $tno[0]);
	$to = max($fno[0], $tno[0]);

	$cols = array(4, 85, 150, 225, 275, 360, 450, 515);

	// $headers in doctext.inc
	$aligns = array('left',	'left',	'left', 'left', 'right', 'right', 'right');

	$params = array('comments' => $comments);

	$cur = get_company_Pref('curr_default');

	$rep = new FrontReport(_('RECEIPT'), "ReceiptBulk", user_pagesize(), 9, $orientation);
   	if ($orientation == 'L')
    	recalculate_cols($cols);
 	$rep->SetHeaderType('Header2');
	$rep->currency = $cur;
	$rep->Font();
	$rep->Info($params, $cols, null, $aligns);

	for ($i = $from; $i <= $to; $i++)
	{
		if ($fno[0] == $tno[0])
			$types = array($fno[1]);
		else
			$types = array(ST_BANKDEPOSIT, ST_CUSTPAYMENT);
		foreach ($types as $j)
		{
			$myrow = get_receipt($j, $i);
			if (!$myrow)
				continue;			
			if ($currency != ALL_TEXT && $myrow['curr_code'] != $currency) {
				continue;
			}
			$res = get_bank_trans($j, $i);
			$baccount = db_fetch($res);
			$params['bankaccount'] = $baccount['bank_act'];

			$contacts = get_branch_contacts($myrow['branch_code'], 'invoice', $myrow['debtor_no']);
			$rep->SetCommonData($myrow, null, $myrow, $baccount, ST_CUSTPAYMENT, $contacts);
			$rep->NewPage();
			$result = get_allocations_for_receipt($myrow['debtor_no'], $myrow['type'], $myrow['trans_no']);

			$doctype = ST_CUSTPAYMENT;

			$total_allocated = 0;
			$rep->TextCol(0, 4,	_("As advance / full / part / payment towards:"), -2);
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
					$rep->NewPage();
			}

			$memo = get_comments_string($j, $i);
			if ($memo != "")
			{
				$rep->NewLine();
				$rep->TextColLines(1, 5, $memo, -2);
			}

			$rep->row = $rep->bottomMargin + (16 * $rep->lineHeight);

			$rep->TextCol(3, 6, _("Total Allocated"), -2);
			$rep->AmountCol(6, 7, $total_allocated, $dec, -2);
			$rep->NewLine();
			$rep->TextCol(3, 6, _("Left to Allocate"), -2);
			$rep->AmountCol(6, 7, $myrow['Total'] + $myrow['ov_discount'] - $total_allocated, $dec, -2);
			if (floatcmp($myrow['ov_discount'], 0))
			{
				$rep->NewLine();
				$rep->TextCol(3, 6, _("Discount"), - 2);
				$rep->AmountCol(6, 7, -$myrow['ov_discount'], $dec, -2);
			}	
			$rep->NewLine();
			$rep->Font('bold');
			$rep->TextCol(3, 6, _("TOTAL RECEIPT"), - 2);
			$rep->AmountCol(6, 7, $myrow['Total'], $dec, -2);

			$words = price_in_words($myrow['Total'], ST_CUSTPAYMENT);
			if ($words != "")
			{
				$rep->NewLine(1);
				$rep->TextCol(0, 7, $myrow['curr_code'] . ": " . $words, - 2);
			}	
			$rep->Font();
			$rep->NewLine();
			$rep->TextCol(6, 7, _("Received / Sign"), - 2);
			$rep->NewLine();
			$rep->TextCol(0, 2, _("By Cash / Cheque* / Draft No."), - 2);
			$rep->TextCol(2, 4, "______________________________", - 2);
			$rep->TextCol(4, 5, _("Dated"), - 2);
			$rep->TextCol(5, 6, "__________________", - 2);
			$rep->NewLine(1);
			$rep->TextCol(0, 2, _("Drawn on Bank"), - 2);
			$rep->TextCol(2, 4, "______________________________", - 2);
			$rep->TextCol(4, 5, _("Branch"), - 2);
			$rep->TextCol(5, 6, "__________________", - 2);
			$rep->TextCol(6, 7, "__________________");
		}	
	}
	$rep->End();
}

?>