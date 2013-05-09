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
$page_security = 'SA_SALESANALYTIC';
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	Inventory Sales Report
// ----------------------------------------------------------------
$path_to_root="..";

include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/data_checks.inc");
include_once($path_to_root . "/includes/banking.inc");
include_once($path_to_root . "/gl/includes/gl_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_category_db.inc");

//----------------------------------------------------------------------------------------------------

print_inventory_purchase();

function getTransactions($category, $location, $fromsupp, $item, $from, $to)
{
	$from = date2sql($from);
	$to = date2sql($to);
	$sql = "SELECT ".TB_PREF."stock_master.category_id,
			".TB_PREF."stock_category.description AS cat_description,
			".TB_PREF."stock_master.stock_id,
			".TB_PREF."stock_master.description, ".TB_PREF."stock_master.inactive,
			".TB_PREF."stock_moves.loc_code,
			".TB_PREF."suppliers.supplier_id,
			".TB_PREF."suppliers.supp_name AS supplier_name,
			".TB_PREF."stock_moves.tran_date,
			".TB_PREF."stock_moves.qty AS qty,
			".TB_PREF."stock_moves.price*(1-".TB_PREF."stock_moves.discount_percent) AS price
		FROM ".TB_PREF."stock_master,
			".TB_PREF."stock_category,
			".TB_PREF."suppliers,
			".TB_PREF."stock_moves
		WHERE ".TB_PREF."stock_master.stock_id=".TB_PREF."stock_moves.stock_id
		AND ".TB_PREF."stock_master.category_id=".TB_PREF."stock_category.category_id
		AND ".TB_PREF."stock_moves.person_id=".TB_PREF."suppliers.supplier_id
		AND ".TB_PREF."stock_moves.tran_date>='$from'
		AND ".TB_PREF."stock_moves.tran_date<='$to'
		AND (".TB_PREF."stock_moves.type=".ST_SUPPRECEIVE." OR ".TB_PREF."stock_moves.type=".ST_SUPPCREDIT.")
		AND (".TB_PREF."stock_master.mb_flag='B' OR ".TB_PREF."stock_master.mb_flag='M')";
		if ($category != 0)
			$sql .= " AND ".TB_PREF."stock_master.category_id = ".db_escape($category);
		if ($location != '')
			$sql .= " AND ".TB_PREF."stock_moves.loc_code = ".db_escape($location);
		if ($fromsupp != '')
			$sql .= " AND ".TB_PREF."suppliers.supplier_id = ".db_escape($fromsupp);
		if ($item != '')
			$sql .= " AND ".TB_PREF."stock_master.stock_id = ".db_escape($item);
		$sql .= " ORDER BY ".TB_PREF."stock_master.category_id,
			".TB_PREF."suppliers.supp_name, ".TB_PREF."stock_master.stock_id, ".TB_PREF."stock_moves.tran_date";
    return db_query($sql,"No transactions were returned");

}

function get_supp_inv_reference($supplier_id, $stock_id, $date)
{
	$sql = "SELECT ".TB_PREF."supp_trans.supp_reference
		FROM ".TB_PREF."supp_trans,
			".TB_PREF."supp_invoice_items,
			".TB_PREF."grn_batch,
			".TB_PREF."grn_items
		WHERE ".TB_PREF."supp_trans.type=".TB_PREF."supp_invoice_items.supp_trans_type
		AND ".TB_PREF."supp_trans.trans_no=".TB_PREF."supp_invoice_items.supp_trans_no
		AND ".TB_PREF."grn_items.grn_batch_id=".TB_PREF."grn_batch.id
		AND ".TB_PREF."grn_items.item_code=".TB_PREF."supp_invoice_items.stock_id
		AND ".TB_PREF."supp_trans.supplier_id=".db_escape($supplier_id)."
		AND ".TB_PREF."supp_invoice_items.stock_id=".db_escape($stock_id)."
		AND ".TB_PREF."supp_trans.tran_date=".db_escape($date);
    $result = db_query($sql,"No transactions were returned");
    $row = db_fetch_row($result);
    if (isset($row[0]))
    	return $row[0];
    else
    	return '';
}    
    
	
//----------------------------------------------------------------------------------------------------

function print_inventory_purchase()
{
    global $path_to_root;

	$from = $_POST['PARAM_0'];
	$to = $_POST['PARAM_1'];
    $category = $_POST['PARAM_2'];
    $location = $_POST['PARAM_3'];
    $fromsupp = $_POST['PARAM_4'];
    $item = $_POST['PARAM_5'];
	$comments = $_POST['PARAM_6'];
	$orientation = $_POST['PARAM_7'];
	$destination = $_POST['PARAM_8'];
	if ($destination)
		include_once($path_to_root . "/reporting/includes/excel_report.inc");
	else
		include_once($path_to_root . "/reporting/includes/pdf_report.inc");

	$orientation = ($orientation ? 'L' : 'P');
    $dec = user_price_dec();

	if ($category == ALL_NUMERIC)
		$category = 0;
	if ($category == 0)
		$cat = _('All');
	else
		$cat = get_category_name($category);

	if ($location == '')
		$loc = _('All');
	else
		$loc = get_location_name($location);

	if ($fromsupp == '')
		$froms = _('All');
	else
		$froms = get_supplier_name($fromsupp);

	if ($item == '')
		$itm = _('All');
	else
		$itm = $item;

	$cols = array(0, 60, 180, 225, 275, 400, 420, 465,	520);

	$headers = array(_('Category'), _('Description'), _('Date'), _('#'), _('Supplier'), _('Qty'), _('Unit Price'), _('Total'));
	if ($fromsupp != '')
		$headers[4] = '';

	$aligns = array('left',	'left',	'left', 'left', 'left', 'left', 'right', 'right');

    $params =   array( 	0 => $comments,
    				    1 => array('text' => _('Period'),'from' => $from, 'to' => $to),
    				    2 => array('text' => _('Category'), 'from' => $cat, 'to' => ''),
    				    3 => array('text' => _('Location'), 'from' => $loc, 'to' => ''),
    				    4 => array('text' => _('Supplier'), 'from' => $froms, 'to' => ''),
    				    5 => array('text' => _('Item'), 'from' => $itm, 'to' => ''));

    $rep = new FrontReport(_('Inventory Purchasing Report'), "InventoryPurchasingReport", user_pagesize(), 9, $orientation);
    if ($orientation == 'L')
    	recalculate_cols($cols);

    $rep->Font();
    $rep->Info($params, $cols, $headers, $aligns);
    $rep->NewPage();

	$res = getTransactions($category, $location, $fromsupp, $item, $from, $to);

	$total = $total_supp = $grandtotal = 0.0;
	$total_qty = 0.0;
	$catt = $stock_description = $stock_id = $supplier_name = '';
	while ($trans=db_fetch($res))
	{
		if ($stock_description != $trans['description'])
		{
			if ($stock_description != '')
			{
				if ($supplier_name != '')
				{
					$rep->NewLine(2, 3);
					$rep->TextCol(0, 1, _('Total'));
					$rep->TextCol(1, 4, $stock_description);
					$rep->TextCol(4, 5, $supplier_name);
					$rep->AmountCol(5, 7, $total_qty, get_qty_dec($stock_id));
					$rep->AmountCol(7, 8, $total_supp, $dec);
					$rep->Line($rep->row - 2);
					$rep->NewLine();
					$total_supp = $total_qty = 0.0;
					$supplier_name = $trans['supplier_name'];
				}	
			}
			$stock_id = $trans['stock_id'];
			$stock_description = $trans['description'];
		}

		if ($supplier_name != $trans['supplier_name'])
		{
			if ($supplier_name != '')
			{
				$rep->NewLine(2, 3);
				$rep->TextCol(0, 1, _('Total'));
				$rep->TextCol(1, 4, $stock_description);
				$rep->TextCol(4, 5, $supplier_name);
				$rep->AmountCol(5, 7, $total_qty, get_qty_dec($stock_id));
				$rep->AmountCol(7, 8, $total_supp, $dec);
				$rep->Line($rep->row - 2);
				$rep->NewLine();
				$total_supp = $total_qty = 0.0;
			}
			$supplier_name = $trans['supplier_name'];
		}
		if ($catt != $trans['cat_description'])
		{
			if ($catt != '')
			{
				$rep->NewLine(2, 3);
				$rep->TextCol(0, 1, _('Total'));
				$rep->TextCol(1, 7, $catt);
				$rep->AmountCol(7, 8, $total, $dec);
				$rep->Line($rep->row - 2);
				$rep->NewLine();
				$rep->NewLine();
				$total = 0.0;
			}
			$rep->TextCol(0, 1, $trans['category_id']);
			$rep->TextCol(1, 6, $trans['cat_description']);
			$catt = $trans['cat_description'];
			$rep->NewLine();
		}
		
		$curr = get_supplier_currency($trans['supplier_id']);
		$rate = get_exchange_rate_from_home_currency($curr, sql2date($trans['tran_date']));
		$trans['price'] *= $rate;
		$rep->NewLine();
		$trans['supp_reference'] = get_supp_inv_reference($trans['supplier_id'], $trans['stock_id'], $trans['tran_date']);
		$rep->fontSize -= 2;
		$rep->TextCol(0, 1, $trans['stock_id']);
		if ($fromsupp == ALL_TEXT)
		{
			$rep->TextCol(1, 2, $trans['description'].($trans['inactive']==1 ? " ("._("Inactive").")" : ""), -1);
			$rep->TextCol(2, 3, sql2date($trans['tran_date']));
			$rep->TextCol(3, 4, $trans['supp_reference']);
			$rep->TextCol(4, 5, $trans['supplier_name']);
		}
		else
		{
			$rep->TextCol(1, 2, $trans['description'].($trans['inactive']==1 ? " ("._("Inactive").")" : ""), -1);
			$rep->TextCol(2, 3, sql2date($trans['tran_date']));
			$rep->TextCol(3, 4, $trans['supp_reference']);
		}	
		$rep->AmountCol(5, 6, $trans['qty'], get_qty_dec($trans['stock_id']));
		$rep->AmountCol(6, 7, $trans['price'], $dec);
		$amt = $trans['qty'] * $trans['price'];
		$rep->AmountCol(7, 8, $amt, $dec);
		$rep->fontSize += 2;
		$total += $amt;
		$total_supp += $amt;
		$grandtotal += $amt;
		$total_qty += $trans['qty'];
	}
	if ($stock_description != '')
	{
		if ($supplier_name != '')
		{
			$rep->NewLine(2, 3);
			$rep->TextCol(0, 1, _('Total'));
			$rep->TextCol(1, 4, $stock_description);
			$rep->TextCol(4, 5, $supplier_name);
			$rep->AmountCol(5, 7, $total_qty, get_qty_dec($stock_id));
			$rep->AmountCol(7, 8, $total_supp, $dec);
			$rep->Line($rep->row - 2);
			$rep->NewLine();
			$rep->NewLine();
			$total_supp = $total_qty = 0.0;
			$supplier_name = $trans['supplier_name'];
		}	
	}
	if ($supplier_name != '')
	{
		$rep->NewLine(2, 3);
		$rep->TextCol(0, 1, _('Total'));
		$rep->TextCol(1, 4, $stock_description);
		$rep->TextCol(4, 5, $supplier_name);
		$rep->AmountCol(5, 7, $total_qty, get_qty_dec($stock_id));
		$rep->AmountCol(7, 8, $total_supp, $dec);
		$rep->Line($rep->row - 2);
		$rep->NewLine();
		$rep->NewLine();
	}

	$rep->NewLine(2, 3);
	$rep->TextCol(0, 1, _('Total'));
	$rep->TextCol(1, 7, $catt);
	$rep->AmountCol(7, 8, $total, $dec);
	$rep->Line($rep->row - 2);
	$rep->NewLine();
	$rep->NewLine(2, 1);
	$rep->TextCol(0, 7, _('Grand Total'));
	$rep->AmountCol(7, 8, $grandtotal, $dec);

	$rep->Line($rep->row  - 4);
	$rep->NewLine();
    $rep->End();
}

?>