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
	
	$sql = "SELECT DISTINCT ".TB_PREF."grn_batch.supplier_id, 
            ".TB_PREF."purch_order_details.*,
            ".TB_PREF."stock_master.description, ".TB_PREF."stock_master.inactive
        FROM ".TB_PREF."stock_master,
            ".TB_PREF."purch_order_details,
            ".TB_PREF."grn_batch
        WHERE ".TB_PREF."stock_master.stock_id=".TB_PREF."purch_order_details.item_code
        AND ".TB_PREF."grn_batch.purch_order_no=".TB_PREF."purch_order_details.order_no
        AND ".TB_PREF."purch_order_details.quantity_received>0
        AND ".TB_PREF."grn_batch.delivery_date>='$from'
        AND ".TB_PREF."grn_batch.delivery_date<='$to'
        ORDER BY ".TB_PREF."stock_master.stock_id, ".TB_PREF."grn_batch.delivery_date"; 	
	
    return db_query($sql,"No transactions were returned");

}

//----------------------------------------------------------------------------------------------------

function print_grn_valuation()
{
    global $path_to_root;

	$from = $_POST['PARAM_0'];
	$to = $_POST['PARAM_1'];
	$comments = $_POST['PARAM_2'];
	$destination = $_POST['PARAM_3'];
	if ($destination)
		include_once($path_to_root . "/reporting/includes/excel_report.inc");
	else
		include_once($path_to_root . "/reporting/includes/pdf_report.inc");

    $dec = user_price_dec();

	$cols = array(0, 75, 225, 275, 345, 390, 445,	515);
	$headers = array(_('Stock ID'), _('Description'), _('PO No'), _('Qty Received'), _('Unit Price'), _('Actual Price'), _('Total'));

	$aligns = array('left',	'left',	'left', 'right', 'right', 'right', 'right');

    $params =   array( 	0 => $comments,
    				    1 => array('text' => _('Period'),'from' => $from, 'to' => $to));

    $rep = new FrontReport(_('GRN Valuation Report'), "GRNValuationReport", user_pagesize());

    $rep->Font();
    $rep->Info($params, $cols, $headers, $aligns);
    $rep->Header();

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
				$rep->AmountCol(3, 4, $qtotal, $qdec);
				$rep->AmountCol(6, 7, $total, $dec);
				$rep->NewLine();
				$total = $qtotal = 0;
			}
			$stock_id = $trans['item_code'];
		}
		$curr = get_supplier_currency($trans['supplier_id']);
		$rate = get_exchange_rate_from_home_currency($curr, sql2date($trans['delivery_date']));
		$trans['unit_price'] *= $rate;
		$trans['act_price'] *= $rate;

		$rep->NewLine();
		$rep->TextCol(0, 1, $trans['item_code']);
		$rep->TextCol(1, 2, $trans['description'].($trans['inactive']==1 ? " ("._("Inactive").")" : ""), -1);
		$rep->TextCol(2, 3, $trans['order_no']);
		$qdec = get_qty_dec($trans['item_code']);
		$rep->AmountCol(3, 4, $trans['quantity_received'], $qdec);
		$rep->AmountCol(4, 5, $trans['unit_price'], $dec);
		$rep->AmountCol(5, 6, $trans['act_price'], $dec);
		$amt = round2($trans['quantity_received'] * $trans['act_price'], $dec);
		$rep->AmountCol(6, 7, $amt, $dec);
		$total += $amt;
		$qtotal += $trans['quantity_received'];
		$grandtotal += $amt;
	}
	if ($stock_id != '')
	{
		$rep->Line($rep->row  - 4);
		$rep->NewLine(2);
		$rep->TextCol(0, 3, _('Total'));
		$rep->AmountCol(3, 4, $qtotal, $qdec);
		$rep->AmountCol(6, 7, $total, $dec);
		$rep->Line($rep->row  - 4);
		$rep->NewLine(2);
		$rep->TextCol(0, 6, _('Grand Total'));
		$rep->AmountCol(6, 7, $grandtotal, $dec);
	}

	$rep->Line($rep->row  - 4);
	$rep->NewLine();
    $rep->End();
}

?>