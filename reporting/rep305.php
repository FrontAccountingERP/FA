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
$page_security = 'SA_SUPPLIERANALYTIC';
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	GRN Valuation Report
// ----------------------------------------------------------------
$path_to_root="..";

include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/data_checks.inc");
include_once($path_to_root . "/includes/banking.inc");
include_once($path_to_root . "/gl/includes/gl_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_category_db.inc");

//----------------------------------------------------------------------------------------------------

print_grn_valuation();

function getTransactions($from, $to)
{
	$from = date2sql($from);
	$to = date2sql($to);
	
	$sql = "SELECT ".TB_PREF."grn_batch.id batch_no,
			".TB_PREF."grn_batch.supplier_id, 
            ".TB_PREF."purch_order_details.*,
            ".TB_PREF."stock_master.description,
			".TB_PREF."grn_items.qty_recd,
			".TB_PREF."grn_items.quantity_inv,
			".TB_PREF."grn_items.id grn_item_id
        FROM ".TB_PREF."stock_master,
            ".TB_PREF."purch_order_details,
            ".TB_PREF."grn_batch,
			".TB_PREF."grn_items 
        WHERE ".TB_PREF."stock_master.stock_id=".TB_PREF."purch_order_details.item_code
        AND ".TB_PREF."grn_batch.purch_order_no=".TB_PREF."purch_order_details.order_no
		AND ".TB_PREF."grn_batch.id = ".TB_PREF."grn_items.grn_batch_id 
		AND ".TB_PREF."grn_items.po_detail_item = ".TB_PREF."purch_order_details.po_detail_item
        AND ".TB_PREF."grn_items.qty_recd>0
        AND ".TB_PREF."grn_batch.delivery_date>='$from'
        AND ".TB_PREF."grn_batch.delivery_date<='$to'
        ORDER BY ".TB_PREF."stock_master.stock_id, ".TB_PREF."grn_batch.delivery_date"; 	
	
    return db_query($sql,"No transactions were returned");

}

function getSuppInvDetails($grn_item_id)
{
	$sql = "SELECT
			".TB_PREF."supp_invoice_items.supp_trans_no inv_no,
			".TB_PREF."supp_invoice_items.quantity inv_qty,
			".TB_PREF."supp_trans.rate,
			IF (".TB_PREF."supp_trans.tax_included = 1, ".TB_PREF."supp_invoice_items.unit_price - ".TB_PREF."supp_invoice_items.unit_tax, ".TB_PREF."supp_invoice_items.unit_price) inv_price
			FROM ".TB_PREF."grn_items, ".TB_PREF."supp_trans, ".TB_PREF."supp_invoice_items
			WHERE ".TB_PREF."grn_items.id = ".TB_PREF."supp_invoice_items.grn_item_id
			AND ".TB_PREF."grn_items.po_detail_item = ".TB_PREF."supp_invoice_items.po_detail_item_id
			AND ".TB_PREF."grn_items.item_code = ".TB_PREF."supp_invoice_items.stock_id
			AND ".TB_PREF."supp_trans.type = ".TB_PREF."supp_invoice_items.supp_trans_type
			AND ".TB_PREF."supp_trans.trans_no = ".TB_PREF."supp_invoice_items.supp_trans_no
			AND ".TB_PREF."supp_invoice_items.supp_trans_type = 20
			AND ".TB_PREF."supp_invoice_items.grn_item_id = ".$grn_item_id."
			ORDER BY ".TB_PREF."supp_invoice_items.id asc";

	return db_query($sql,"No transactions were returned");
}

//----------------------------------------------------------------------------------------------------

function print_grn_valuation()
{
    global $path_to_root;

	$from = $_POST['PARAM_0'];
	$to = $_POST['PARAM_1'];
	$comments = $_POST['PARAM_2'];
	$orientation = $_POST['PARAM_3'];
	$destination = $_POST['PARAM_4'];
	if ($destination)
		include_once($path_to_root . "/reporting/includes/excel_report.inc");
	else
		include_once($path_to_root . "/reporting/includes/pdf_report.inc");

 	$orientation = ($orientation ? 'L' : 'P');
   	$dec = user_price_dec();

	$cols = array(0, 75, 225, 260, 295, 330, 370, 410, 455, 515);
	$headers = array(_('Stock ID'), _('Description'), _('PO No'), _('GRN')."#", _('Inv')."#", _('Qty'), _('Inv Price'), _('PO Price'), _('Total'));

	$aligns = array('left',	'left',	'left', 'left', 'left', 'right', 'right', 'right', 'right');

    $params =   array( 	0 => $comments,
    				    1 => array('text' => _('Period'),'from' => $from, 'to' => $to));

    $rep = new FrontReport(_('GRN Valuation Report'), "GRNValuationReport", user_pagesize(), 9, $orientation);
    if ($orientation == 'L')
    	recalculate_cols($cols);

    $rep->Font();
    $rep->Info($params, $cols, $headers, $aligns);
    $rep->NewPage();

	$res = getTransactions($from, $to);
	$total = $qtotal = $grandtotal = 0.0;
	$stock_id = '';
	while ($trans=db_fetch($res))
	{
		if ($stock_id != $trans['item_code'])
		{
			if ($stock_id != '')
			{
				$rep->Line($rep->row  - 4);
				$rep->NewLine(2);
				$rep->TextCol(0, 3, _('Total'));
				$rep->AmountCol(5, 6, $qtotal, $qdec);
				$rep->AmountCol(8, 9, $total, $dec);
				$rep->NewLine();
				$total = $qtotal = 0;
			}
			$stock_id = $trans['item_code'];
		}

		$rep->NewLine();
		$rep->TextCol(0, 1, $trans['item_code']);
		$rep->TextCol(1, 2, $trans['description']);
		$rep->TextCol(2, 3, $trans['order_no']);
		$qdec = get_qty_dec($trans['item_code']);
		$rep->TextCol(3, 4, $trans['batch_no']);
		
		if ($trans['quantity_inv'])
		{
			$suppinv = getSuppInvDetails($trans['grn_item_id']);
			while ($inv=db_fetch($suppinv))
			{	
				$inv['inv_price'] *= $inv['rate'];
				$rep->TextCol(4, 5, $inv['inv_no']);
				$rep->AmountCol(5, 6, $inv['inv_qty'], $qdec);
				$rep->AmountCol(6, 7, $inv['inv_price'], $dec);
				$rep->AmountCol(7, 8, $trans['std_cost_unit'], $dec);
				$amt = round2($inv['inv_qty'] * $inv['inv_price'], $dec);
				$rep->AmountCol(8, 9, $amt, $dec);
				$rep->NewLine();
				$total += $amt;
				$qtotal += $inv['inv_qty'];
				$grandtotal += $amt;
			}
		}
		
		if ($trans['qty_recd'] - $trans['quantity_inv'] !=0 )
		{
			$curr = get_supplier_currency($trans['supplier_id']);
			$rate = get_exchange_rate_from_home_currency($curr, sql2date($trans['delivery_date']));
			$trans['unit_price'] *= $rate;
			$rep->TextCol(4, 5, "--");
			$rep->AmountCol(5, 6, $trans['qty_recd'] - $trans['quantity_inv'], $qdec);
			$rep->AmountCol(7, 8, $trans['unit_price'], $dec);
			$amt = round2(($trans['qty_recd'] - $trans['quantity_inv']) * $trans['unit_price'], $dec);
			$rep->AmountCol(8, 9, $amt, $dec);
			$total += $amt;
			$qtotal += $trans['qty_recd'] - $trans['quantity_inv'];
			$grandtotal += $amt;
		}
		else
			$rep->NewLine(-1);
	}
	if ($stock_id != '')
	{
		$rep->Line($rep->row  - 4);
		$rep->NewLine(2);
		$rep->TextCol(0, 3, _('Total'));
		$rep->AmountCol(5, 6, $qtotal, $qdec);
		$rep->AmountCol(8, 9, $total, $dec);
		$rep->Line($rep->row  - 4);
		$rep->NewLine(2);
		$rep->TextCol(0, 7, _('Grand Total'));
		$rep->AmountCol(8, 9, $grandtotal, $dec);
	}

	$rep->Line($rep->row  - 4);
	$rep->NewLine();
    $rep->End();
}

?>