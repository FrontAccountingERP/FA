<?php

$page_security = 2;
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	Inventory Planning
// ----------------------------------------------------------------
$path_to_root="../";

include_once($path_to_root . "includes/session.inc");
include_once($path_to_root . "includes/date_functions.inc");
include_once($path_to_root . "includes/data_checks.inc");
include_once($path_to_root . "gl/includes/gl_db.inc");
include_once($path_to_root . "inventory/includes/db/items_category_db.inc");

//----------------------------------------------------------------------------------------------------

// trial_inquiry_controls();
print_inventory_planning();

function getTransactions($category, $location)
{
	$sql = "SELECT ".TB_PREF."stock_master.category_id,
			".TB_PREF."stock_category.description AS cat_description,
			".TB_PREF."stock_master.stock_id,
			".TB_PREF."stock_master.description,
			".TB_PREF."stock_moves.loc_code,
			SUM(".TB_PREF."stock_moves.qty) AS qty_on_hand
		FROM ".TB_PREF."stock_master,
			".TB_PREF."stock_category,
			".TB_PREF."stock_moves
		WHERE ".TB_PREF."stock_master.stock_id=".TB_PREF."stock_moves.stock_id
		AND ".TB_PREF."stock_master.category_id=".TB_PREF."stock_category.category_id
		AND (".TB_PREF."stock_master.mb_flag='B' OR ".TB_PREF."stock_master.mb_flag='M')";
	if ($category != 0)
		$sql .= " AND ".TB_PREF."stock_master.category_id = '$category'";
	if ($location != 'all')
		$sql .= " AND ".TB_PREF."stock_moves.loc_code = '$location'";
	$sql .= " GROUP BY ".TB_PREF."stock_master.category_id,
		".TB_PREF."stock_master.description,
		".TB_PREF."stock_category.description,
		".TB_PREF."stock_moves.stock_id,
		".TB_PREF."stock_master.stock_id
		ORDER BY ".TB_PREF."stock_master.category_id,
		".TB_PREF."stock_master.stock_id";

    return db_query($sql,"No transactions were returned");

}

function getCustQty($stockid, $location)
{
	$sql = "SELECT SUM(".TB_PREF."sales_order_details.quantity - ".TB_PREF."sales_order_details.qty_sent) AS qty_demand
				FROM ".TB_PREF."sales_order_details,
					".TB_PREF."sales_orders
				WHERE ".TB_PREF."sales_order_details.order_no=".TB_PREF."sales_orders.order_no AND
					".TB_PREF."sales_orders.from_stk_loc ='$location' AND
					".TB_PREF."sales_order_details.stk_code = '$stockid'";

    $TransResult = db_query($sql,"No transactions were returned");
	$DemandRow = db_fetch($TransResult);
	return $DemandRow['qty_demand'];
}

function getCustAsmQty($stockid, $location)
{
	$sql = "SELECT SUM((".TB_PREF."sales_order_details.quantity-".TB_PREF."sales_order_details.qty_sent)*".TB_PREF."bom.quantity)
				   AS Dem
				   FROM ".TB_PREF."sales_order_details,
						".TB_PREF."sales_orders,
						".TB_PREF."bom,
						".TB_PREF."stock_master
				   WHERE ".TB_PREF."sales_order_details.stk_code=".TB_PREF."bom.parent AND
				   ".TB_PREF."sales_orders.order_no = ".TB_PREF."sales_order_details.order_no AND
				   ".TB_PREF."sales_orders.from_stk_loc='$location' AND
				   ".TB_PREF."sales_order_details.quantity-".TB_PREF."sales_order_details.qty_sent > 0 AND
				   ".TB_PREF."bom.component='$stockid' AND
				   ".TB_PREF."stock_master.stock_id=".TB_PREF."bom.parent AND
				   ".TB_PREF."stock_master.mb_flag='A'";

    $TransResult = db_query($sql,"No transactions were returned");
	if (db_num_rows($TransResult) == 1)
	{
		$DemandRow = db_fetch_row($TransResult);
		$DemandQty = $DemandRow[0];
	}
	else
		$DemandQty = 0.0;

    return $DemandQty;
}

function getSuppQty($stockid, $location)
{
	$sql = "SELECT SUM(".TB_PREF."purch_order_details.quantity_ordered - ".TB_PREF."purch_order_details.quantity_received) AS QtyOnOrder
				FROM ".TB_PREF."purch_order_details,
					".TB_PREF."purch_orders
				WHERE ".TB_PREF."purch_order_details.order_no = ".TB_PREF."purch_orders.order_no
				AND ".TB_PREF."purch_order_details.item_code = '$stockid'
				AND ".TB_PREF."purch_orders.into_stock_location= '$location'";

    $TransResult = db_query($sql,"No transactions were returned");
	$DemandRow = db_fetch($TransResult);
	return $DemandRow['QtyOnOrder'];
}

function getPeriods($stockid, $location)
{
	$date5 = date('Y-m-d');
	$date4 = date('Y-m-d',mktime(0,0,0,date('m'),1,date('Y')));
	$date3 = date('Y-m-d',mktime(0,0,0,date('m')-1,1,date('Y')));
	$date2 = date('Y-m-d',mktime(0,0,0,date('m')-2,1,date('Y')));
	$date1 = date('Y-m-d',mktime(0,0,0,date('m')-3,1,date('Y')));
	$date0 = date('Y-m-d',mktime(0,0,0,date('m')-4,1,date('Y')));

	$sql = "SELECT SUM(CASE WHEN tran_date >= '$date0' AND tran_date < '$date1' THEN -qty ELSE 0 END) AS prd0,
		   		SUM(CASE WHEN tran_date >= '$date1' AND tran_date < '$date2' THEN -qty ELSE 0 END) AS prd1,
				SUM(CASE WHEN tran_date >= '$date2' AND tran_date < '$date3' THEN -qty ELSE 0 END) AS prd2,
				SUM(CASE WHEN tran_date >= '$date3' AND tran_date < '$date4' THEN -qty ELSE 0 END) AS prd3,
				SUM(CASE WHEN tran_date >= '$date4' AND tran_date <= '$date5' THEN -qty ELSE 0 END) AS prd4
			FROM ".TB_PREF."stock_moves
			WHERE stock_id='$stockid'
			AND loc_code ='$location'
			AND (type=13 OR type=11)
			AND visible=1";

    $TransResult = db_query($sql,"No transactions were returned");
	return db_fetch($TransResult);
}

//----------------------------------------------------------------------------------------------------

function print_inventory_planning()
{
    global $path_to_root;

    include_once($path_to_root . "reporting/includes/pdf_report.inc");

    $category = $_POST['PARAM_0'];
    $location = $_POST['PARAM_1'];
    $comments = $_POST['PARAM_2'];

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
		$loc = $location;

	$cols = array(0, 50, 150, 180, 210, 240, 270, 300, 330, 390, 435, 480, 525);

	$per0 = strftime('%b',mktime(0,0,0,date('m'),1,date('Y')));
	$per1 = strftime('%b',mktime(0,0,0,date('m')-1,1,date('Y')));
	$per2 = strftime('%b',mktime(0,0,0,date('m')-2,1,date('Y')));
	$per3 = strftime('%b',mktime(0,0,0,date('m')-3,1,date('Y')));
	$per4 = strftime('%b',mktime(0,0,0,date('m')-4,1,date('Y')));

	$headers = array(_('Category'), '', $per4, $per3, $per2, $per1, $per0, '3*M',
		_('QOH'), _('Cust Ord'), _('Supp Ord'), _('Sugg Ord'));

	$aligns = array('left',	'left',	'right', 'right', 'right', 'right', 'right', 'right',
		'right', 'right', 'right', 'right');

    $params =   array( 	0 => $comments,
    				    1 => array('text' => _('Category'), 'from' => $cat, 'to' => ''),
    				    2 => array('text' => _('Location'), 'from' => $loc, 'to' => ''));

    $rep = new FrontReport(_('Inventory Planning Report'), "InventoryPlanning.pdf", user_pagesize());

    $rep->Font();
    $rep->Info($params, $cols, $headers, $aligns);
    $rep->Header();

	$res = getTransactions($category, $location);
	$catt = '';
	while ($trans=db_fetch($res))
	{
		if ($catt != $trans['cat_description'])
		{
			if ($catt != '')
			{
				$rep->Line($rep->row - 2);
				$rep->NewLine(2, 3);
			}
			$rep->TextCol(0, 1, $trans['category_id']);
			$rep->TextCol(1, 2, $trans['cat_description']);
			$catt = $trans['cat_description'];
			$rep->NewLine();
		}

		$custqty = getCustQty($trans['stock_id'], $trans['loc_code']);
		$custqty += getCustAsmQty($trans['stock_id'], $trans['loc_code']);
		$suppqty = getSuppQty($trans['stock_id'], $trans['loc_code']);
		$period = getPeriods($trans['stock_id'], $trans['loc_code']);
		$rep->NewLine();
		$dec = get_qty_dec($trans['stock_id']);
		$rep->TextCol(0, 1, $trans['stock_id']);
		$rep->TextCol(1, 2, $trans['description']);
		$rep->TextCol(2, 3, number_format2($period['prd0'], $dec));
		$rep->TextCol(3, 4, number_format2($period['prd1'], $dec));
		$rep->TextCol(4, 5, number_format2($period['prd2'], $dec));
		$rep->TextCol(5, 6, number_format2($period['prd3'], $dec));
		$rep->TextCol(6, 7, number_format2($period['prd4'], $dec));

		$MaxMthSales = Max($period['prd0'], $period['prd1'], $period['prd2'], $period['prd3']);
		$IdealStockHolding = $MaxMthSales * 3;
		$rep->TextCol(7, 8, number_format2($IdealStockHolding, $dec));

		$rep->TextCol(8, 9, number_format2($trans['qty_on_hand'], $dec));
		$rep->TextCol(9, 10, number_format2($custqty, $dec));
		$rep->TextCol(10, 11, number_format2($suppqty, $dec));

		$SuggestedTopUpOrder = $IdealStockHolding - $trans['qty_on_hand'] + $custqty - $suppqty;
		if ($SuggestedTopUpOrder < 0.0)
			$SuggestedTopUpOrder = 0.0;
		$rep->TextCol(11, 12, number_format2($SuggestedTopUpOrder, $dec));
	}
	$rep->Line($rep->row - 4);
    $rep->End();
}

?>