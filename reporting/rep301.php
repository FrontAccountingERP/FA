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
$page_security = 'SA_ITEMSVALREP';
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	Inventory Valuation
// ----------------------------------------------------------------
$path_to_root="..";

include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/data_checks.inc");
include_once($path_to_root . "/gl/includes/gl_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_category_db.inc");

//----------------------------------------------------------------------------------------------------

print_inventory_valuation_report();

function get_domestic_price($myrow, $stock_id, $qty, $old_std_cost, $old_qty)
{
	if ($myrow['type'] == ST_SUPPRECEIVE || $myrow['type'] == ST_SUPPCREDIT)
	{
		$price = $myrow['price'];
		if ($myrow['type'] == ST_SUPPRECEIVE)
		{
			// Has the supplier invoice increased the receival price?
			$sql = "SELECT DISTINCT act_price FROM ".TB_PREF."purch_order_details pod INNER JOIN ".TB_PREF."grn_batch grn ON pod.order_no =
				grn.purch_order_no WHERE grn.id = ".$myrow['trans_no']." AND pod.item_code = '$stock_id'";
			$result = db_query($sql, "Could not retrieve act_price from purch_order_details");
			$row = db_fetch_row($result);
			if ($row[0] > 0 AND $row[0] <> $myrow['price'])
				$price = $row[0];
		}
		if ($myrow['person_id'] > 0)
		{
			// Do we have foreign currency?
			$supp = get_supplier($myrow['person_id']);
			$currency = $supp['curr_code'];
			$ex_rate = get_exchange_rate_to_home_currency($currency, sql2date($myrow['tran_date']));
			$price /= $ex_rate;
		}	
	}
	elseif ($myrow['type'] != ST_INVADJUST) // calcutale the price from avg. price
		$price = ($myrow['standard_cost'] * $qty - $old_std_cost * $old_qty) / $myrow['qty'];
	else
		$price = $myrow['standard_cost']; // Item Adjustments just have the real cost
	return $price;
}	

function getAverageCost($stock_id, $to_date)
{
	if ($to_date == null)
		$to_date = Today();

	$to_date = date2sql($to_date);

	$sql = "SELECT standard_cost, price, tran_date, type, trans_no, qty, person_id FROM ".TB_PREF."stock_moves
		WHERE stock_id=".db_escape($stock_id)."
		AND tran_date <= '$to_date' AND standard_cost > 0.001 AND qty <> 0 AND type <> ".ST_LOCTRANSFER;

	$sql .= " ORDER BY tran_date";	

	$result = db_query($sql, "No standard cost transactions were returned");
    if ($result == false)
    	return 0;
	$qty = $old_qty = $count = $old_std_cost = $tot_cost = 0;
	while ($row=db_fetch($result))
	{
		$qty += $row['qty'];	

		$price = get_domestic_price($row, $stock_id, $qty, $old_std_cost, $old_qty);

		$old_std_cost = $row['standard_cost'];
		$tot_cost += $price;
		$count++;
		$old_qty = $qty;
	}
	if ($count == 0)
		return 0;
	return $tot_cost / $count;
}
    
function getTransactions($category, $location, $date)
{
	$date = date2sql($date);
	
	$sql = "SELECT ".TB_PREF."stock_master.category_id,
			".TB_PREF."stock_category.description AS cat_description,
			".TB_PREF."stock_master.stock_id,
			".TB_PREF."stock_master.units,
			".TB_PREF."stock_master.description, ".TB_PREF."stock_master.inactive,
			".TB_PREF."stock_moves.loc_code,
			SUM(".TB_PREF."stock_moves.qty) AS QtyOnHand, 
			".TB_PREF."stock_master.material_cost + ".TB_PREF."stock_master.labour_cost + ".TB_PREF."stock_master.overhead_cost AS UnitCost,
			SUM(".TB_PREF."stock_moves.qty) *(".TB_PREF."stock_master.material_cost + ".TB_PREF."stock_master.labour_cost + ".TB_PREF."stock_master.overhead_cost) AS ItemTotal 
			FROM ".TB_PREF."stock_master,
			".TB_PREF."stock_category,
			".TB_PREF."stock_moves
		WHERE ".TB_PREF."stock_master.stock_id=".TB_PREF."stock_moves.stock_id
		AND ".TB_PREF."stock_master.category_id=".TB_PREF."stock_category.category_id
		AND ".TB_PREF."stock_master.mb_flag<>'D' 
		AND ".TB_PREF."stock_moves.tran_date <= '$date'
		GROUP BY ".TB_PREF."stock_master.category_id,
			".TB_PREF."stock_category.description, ";
		if ($location != 'all')
			$sql .= TB_PREF."stock_moves.loc_code, ";
		$sql .= TB_PREF."stock_master.stock_id,
			".TB_PREF."stock_master.description
		HAVING SUM(".TB_PREF."stock_moves.qty) != 0";
		if ($category != 0)
			$sql .= " AND ".TB_PREF."stock_master.category_id = ".db_escape($category);
		if ($location != 'all')
			$sql .= " AND ".TB_PREF."stock_moves.loc_code = ".db_escape($location);
		$sql .= " ORDER BY ".TB_PREF."stock_master.category_id,
			".TB_PREF."stock_master.stock_id";

    return db_query($sql,"No transactions were returned");
}

//----------------------------------------------------------------------------------------------------

function print_inventory_valuation_report()
{
    global $path_to_root, $use_costed_values;;

	$date = $_POST['PARAM_0'];
    $category = $_POST['PARAM_1'];
    $location = $_POST['PARAM_2'];
    $detail = $_POST['PARAM_3'];
    $comments = $_POST['PARAM_4'];
	$orientation = $_POST['PARAM_5'];
	$destination = $_POST['PARAM_6'];
	if ($destination)
		include_once($path_to_root . "/reporting/includes/excel_report.inc");
	else
		include_once($path_to_root . "/reporting/includes/pdf_report.inc");
	$detail = !$detail;
    $dec = user_price_dec();

	$orientation = ($orientation ? 'L' : 'P');
	if ($category == ALL_NUMERIC)
		$category = 0;
	if ($category == 0)
		$cat = _('All');
	else
		$cat = get_category_name($category);

	if ($location == ALL_TEXT)
		$location = 'all';
	if ($location == 'all')
		$loc = _('All');
	else
		$loc = get_location_name($location);

	$cols = array(0, 75, 225, 250, 350, 450,	515);

	$headers = array(_('Category'), '', _('UOM'), _('Quantity'), _('Unit Cost'), _('Value'));

	$aligns = array('left',	'left',	'left', 'right', 'right', 'right');

    $params =   array( 	0 => $comments,
    					1 => array('text' => _('End Date'), 'from' => $date, 		'to' => ''),
    				    2 => array('text' => _('Category'), 'from' => $cat, 'to' => ''),
    				    3 => array('text' => _('Location'), 'from' => $loc, 'to' => ''));

    $rep = new FrontReport(_('Inventory Valuation Report'), "InventoryValReport", user_pagesize(), 9, $orientation);
    if ($orientation == 'L')
    	recalculate_cols($cols);
    $rep->Font();
    $rep->Info($params, $cols, $headers, $aligns);
    $rep->NewPage();

	$res = getTransactions($category, $location, $date);
	$total = $grandtotal = 0.0;
	$catt = '';
	while ($trans=db_fetch($res))
	{
		if ($catt != $trans['cat_description'])
		{
			if ($catt != '')
			{
				if ($detail)
				{
					$rep->NewLine(2, 3);
					$rep->TextCol(0, 4, _('Total'));
				}
				$rep->AmountCol(5, 6, $total, $dec);
				if ($detail)
				{
					$rep->Line($rep->row - 2);
					$rep->NewLine();
				}
				$rep->NewLine();
				$total = 0.0;
			}
			$rep->TextCol(0, 1, $trans['category_id']);
			$rep->TextCol(1, 2, $trans['cat_description']);
			$catt = $trans['cat_description'];
			if ($detail)
				$rep->NewLine();
		}
		if (isset($use_costed_values) && $use_costed_values==1)
		{
			$UnitCost = getAverageCost($trans['stock_id'], $date);
			$ItemTotal = $trans['QtyOnHand'] * $UnitCost;
		}	
		else
		{
			$UnitCost = $trans['UnitCost'];
			$ItemTotal = $trans['ItemTotal'];
		}	
		if ($detail)
		{
			$rep->NewLine();
			$rep->fontSize -= 2;
			$rep->TextCol(0, 1, $trans['stock_id']);
			$rep->TextCol(1, 2, $trans['description'].($trans['inactive']==1 ? " ("._("Inactive").")" : ""), -1);
			$rep->TextCol(2, 3, $trans['units']);
			$rep->AmountCol(3, 4, $trans['QtyOnHand'], get_qty_dec($trans['stock_id']));
			
			$dec2 = 0;
			price_decimal_format($UnitCost, $dec2);
			$rep->AmountCol(4, 5, $UnitCost, $dec2);
			$rep->AmountCol(5, 6, $ItemTotal, $dec);
			$rep->fontSize += 2;
		}
		$total += $ItemTotal;
		$grandtotal += $ItemTotal;
	}
	if ($detail)
	{
		$rep->NewLine(2, 3);
		$rep->TextCol(0, 4, _('Total'));
	}
	$rep->Amountcol(5, 6, $total, $dec);
	if ($detail)
	{
		$rep->Line($rep->row - 2);
		$rep->NewLine();
	}
	$rep->NewLine(2, 1);
	$rep->TextCol(0, 4, _('Grand Total'));
	$rep->AmountCol(5, 6, $grandtotal, $dec);
	$rep->Line($rep->row  - 4);
	$rep->NewLine();
    $rep->End();
}

?>