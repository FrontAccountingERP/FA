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
$page_security = 2;
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	Inventory Planning
// ----------------------------------------------------------------
$path_to_root="..";

include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/data_checks.inc");
include_once($path_to_root . "/includes/banking.inc");
include_once($path_to_root . "/gl/includes/gl_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_category_db.inc");

//----------------------------------------------------------------------------------------------------

print_inventory_sales();

function getTransactions($category, $location, $from, $to)
{
	$from = date2sql($from);
	$to = date2sql($to);
	$sql = "SELECT ".TB_PREF."stock_master.category_id,
			".TB_PREF."stock_category.description AS cat_description,
			".TB_PREF."stock_master.stock_id,
			".TB_PREF."stock_master.description,
			".TB_PREF."stock_moves.loc_code,
			".TB_PREF."debtor_trans.debtor_no,
			".TB_PREF."stock_moves.tran_date,
			-".TB_PREF."stock_moves.qty*".TB_PREF."stock_moves.price*(1-".TB_PREF."stock_moves.discount_percent) AS amt,
			-".TB_PREF."stock_moves.qty *(".TB_PREF."stock_master.material_cost + ".TB_PREF."stock_master.labour_cost + ".TB_PREF."stock_master.overhead_cost) AS cost
		FROM ".TB_PREF."stock_master,
			".TB_PREF."stock_category,
			".TB_PREF."debtor_trans,
			".TB_PREF."stock_moves
		WHERE ".TB_PREF."stock_master.stock_id=".TB_PREF."stock_moves.stock_id
		AND ".TB_PREF."stock_master.category_id=".TB_PREF."stock_category.category_id

		AND ".TB_PREF."stock_moves.type=".TB_PREF."debtor_trans.type
		AND ".TB_PREF."stock_moves.trans_no=".TB_PREF."debtor_trans.trans_no
		AND ".TB_PREF."stock_moves.tran_date>='$from'
		AND ".TB_PREF."stock_moves.tran_date<='$to'
		AND ((".TB_PREF."debtor_trans.type=13 AND ".TB_PREF."debtor_trans.version=1) OR ".TB_PREF."stock_moves.type=11)
		AND (".TB_PREF."stock_master.mb_flag='B' OR ".TB_PREF."stock_master.mb_flag='M')";
		if ($category != 0)
			$sql .= " AND ".TB_PREF."stock_master.category_id = '$category'";
		if ($location != 'all')
			$sql .= " AND ".TB_PREF."stock_moves.loc_code = '$location'";
		//$sql .= " AND SUM(".TB_PREF."stock_moves.qty) != 0
		$sql .= " ORDER BY ".TB_PREF."stock_master.category_id,
			".TB_PREF."stock_master.stock_id";
    return db_query($sql,"No transactions were returned");

}

//----------------------------------------------------------------------------------------------------

function print_inventory_sales()
{
    global $path_to_root;

	$from = $_POST['PARAM_0'];
	$to = $_POST['PARAM_1'];
    $category = $_POST['PARAM_2'];
    $location = $_POST['PARAM_3'];
    $detail = $_POST['PARAM_4'];
	$comments = $_POST['PARAM_5'];
	$destination = $_POST['PARAM_6'];
	if ($destination)
		include_once($path_to_root . "/reporting/includes/excel_report.inc");
	else
		include_once($path_to_root . "/reporting/includes/pdf_report.inc");

    $dec = user_price_dec();

	if ($category == reserved_words::get_all_numeric())
		$category = 0;
	if ($category == 0)
		$cat = _('All');
	else
		$cat = get_category_name($category);

	if ($location == reserved_words::get_all())
		$location = 'all';
	if ($location == 'all')
		$loc = _('All');
	else
		$loc = get_location_name($location);

	$cols = array(0, 100, 250, 350, 450,	515);

	$headers = array(_('Category'), '', _('Sales'), _('Cost'), _('Contribution'));

	$aligns = array('left',	'left',	'right', 'right', 'right');

    $params =   array( 	0 => $comments,
    				    1 => array('text' => _('Period'),'from' => $from, 'to' => $to),
    				    2 => array('text' => _('Category'), 'from' => $cat, 'to' => ''),
    				    3 => array('text' => _('Location'), 'from' => $loc, 'to' => ''));

    $rep = new FrontReport(_('Inventory Sales Report'), "InventorySalesReport", user_pagesize());

    $rep->Font();
    $rep->Info($params, $cols, $headers, $aligns);
    $rep->Header();

	$res = getTransactions($category, $location, $from, $to);
	$total = $grandtotal = 0.0;
	$total1 = $grandtotal1 = 0.0;
	$total2 = $grandtotal2 = 0.0;
	$amt = $cost = $cb = 0;
	$catt = $stock_id = $stock_desc = '';
	while ($trans=db_fetch($res))
	{
		if ($catt != $trans['cat_description'])
		{
			if ($catt != '')
			{
				if ($detail)
				{
					$rep->NewLine(2, 3);
					$rep->TextCol(0, 2, _('Total'));
				}
				$rep->AmountCol(2, 3, $total, $dec);
				$rep->AmountCol(3, 4, $total1, $dec);
				$rep->AmountCol(4, 5, $total2, $dec);
				if ($detail)
				{
					$rep->Line($rep->row - 2);
					$rep->NewLine();
				}
				$rep->NewLine();
				$total = $total1 = $total2 = 0.0;
			}
			$rep->TextCol(0, 1, $trans['category_id']);
			$rep->TextCol(1, 2, $trans['cat_description']);
			$catt = $trans['cat_description'];
			if ($detail)
				$rep->NewLine();
		}
		if ($stock_id != $trans['stock_id'])
		{
			if ($stock_id != '')
			{
				if ($detail)
				{
					$rep->NewLine();
					$rep->fontsize -= 2;
					$rep->TextCol(0, 1, $stock_id);
					$rep->TextCol(1, 2, $stock_desc);
					$rep->AmountCol(2, 3, $amt, $dec);
					$rep->AmountCol(3, 4, $cost, $dec);
					$rep->AmountCol(4, 5, $cb, $dec);
					$rep->fontsize += 2;
				}
				$amt = $cost = $cb = 0;
			}
			$stock_id = $trans['stock_id'];
			$stock_desc = $trans['description'];
		}
		$curr = get_customer_currency($trans['debtor_no']);
		$rate = get_exchange_rate_from_home_currency($curr, sql2date($trans['tran_date']));
		$trans['amt'] *= $rate;
		$amt += $trans['amt'];
		$cost += $trans['cost'];
		$cb1 = $trans['amt'] - $trans['cost'];
		$cb += $cb1;
		$total += $trans['amt'];
		$total1 += $trans['cost'];
		$total2 += $cb1;
		$grandtotal += $trans['amt'];
		$grandtotal1 += $trans['cost'];
		$grandtotal2 += $cb1;
	}
	if ($detail)
	{
		$rep->NewLine();
		$rep->fontsize -= 2;
		$rep->TextCol(0, 1, $stock_id);
		$rep->TextCol(1, 2, $stock_desc);
		$rep->AmountCol(2, 3, $amt, $dec);
		$rep->AmountCol(3, 4, $cost, $dec);
		$rep->AmountCol(4, 5, $cb, $dec);
		$rep->fontsize += 2;

		$rep->NewLine(2, 3);
		$rep->TextCol(0, 2, _('Total'));
	}
	$rep->AmountCol(2, 3, $total, $dec);
	$rep->AmountCol(3, 4, $total1, $dec);
	$rep->AmountCol(4, 5, $total2, $dec);
	if ($detail)
	{
		$rep->Line($rep->row - 2);
		$rep->NewLine();
	}
	$rep->NewLine(2, 1);
	$rep->TextCol(0, 2, _('Grand Total'));
	$rep->AmountCol(2, 3, $grandtotal, $dec);
	$rep->AmountCol(3, 4, $grandtotal1, $dec);
	$rep->AmountCol(4, 5, $grandtotal2, $dec);

	$rep->Line($rep->row  - 4);
	$rep->NewLine();
    $rep->End();
}

?>