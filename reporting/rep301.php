<?php

$page_security = 2;
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	Supplier Balances
// ----------------------------------------------------------------
$path_to_root="../";

include_once($path_to_root . "includes/session.inc");
include_once($path_to_root . "includes/date_functions.inc");
include_once($path_to_root . "includes/data_checks.inc");
include_once($path_to_root . "gl/includes/gl_db.inc");
include_once($path_to_root . "inventory/includes/db/items_category_db.inc");

//----------------------------------------------------------------------------------------------------

// trial_inquiry_controls();
print_inventory_valuation_report();

function getTransactions($category, $location)
{
	$sql = "SELECT ".TB_PREF."stock_master.category_id,
			".TB_PREF."stock_category.description AS cat_description,
			".TB_PREF."stock_master.stock_id,
			".TB_PREF."stock_master.description,
			".TB_PREF."stock_moves.loc_code,
			SUM(".TB_PREF."stock_moves.qty) AS QtyOnHand,
			".TB_PREF."stock_master.material_cost + ".TB_PREF."stock_master.labour_cost + ".TB_PREF."stock_master.overhead_cost AS UnitCost,
			SUM(".TB_PREF."stock_moves.qty) *(".TB_PREF."stock_master.material_cost + ".TB_PREF."stock_master.labour_cost + ".TB_PREF."stock_master.overhead_cost) AS ItemTotal
		FROM ".TB_PREF."stock_master,
			".TB_PREF."stock_category,
			".TB_PREF."stock_moves
		WHERE ".TB_PREF."stock_master.stock_id=".TB_PREF."stock_moves.stock_id
		AND ".TB_PREF."stock_master.category_id=".TB_PREF."stock_category.category_id
		GROUP BY ".TB_PREF."stock_master.category_id,
			".TB_PREF."stock_category.description,
			UnitCost,
			".TB_PREF."stock_master.stock_id,
			".TB_PREF."stock_master.description
		HAVING SUM(".TB_PREF."stock_moves.qty) != 0";
		if ($category != 0)
			$sql .= " AND ".TB_PREF."stock_master.category_id = '$category'";
		if ($location != 'all')
			$sql .= " AND ".TB_PREF."stock_moves.loc_code = '$location'";
		$sql .= " ORDER BY ".TB_PREF."stock_master.category_id,
			".TB_PREF."stock_master.stock_id";

    return db_query($sql,"No transactions were returned");
}

//----------------------------------------------------------------------------------------------------

function print_inventory_valuation_report()
{
    global $path_to_root;

    include_once($path_to_root . "reporting/includes/pdf_report.inc");

    $category = $_POST['PARAM_0'];
    $location = $_POST['PARAM_1'];
    $detail = $_POST['PARAM_2'];
    $comments = $_POST['PARAM_3'];
    
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
		$loc = $location;

	$cols = array(0, 100, 250, 350, 450,	515);

	$headers = array(_('Category'), '', _('Quantity'), _('Unit Cost'), _('Value'));

	$aligns = array('left',	'left',	'right', 'right', 'right');

    $params =   array( 	0 => $comments,
    				    1 => array('text' => _('Category'), 'from' => $cat, 'to' => ''),
    				    2 => array('text' => _('Location'), 'from' => $loc, 'to' => ''));

    $rep = new FrontReport(_('Inventory Valuation Report'), "InventoryValReport.pdf", user_pagesize());

    $rep->Font();
    $rep->Info($params, $cols, $headers, $aligns);
    $rep->Header();

	$res = getTransactions($category, $location);
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
				$rep->Textcol(4, 5, number_format2($total, $dec));
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
		if ($detail)
		{
			$rep->NewLine();
			$rep->fontsize -= 2;
			$rep->TextCol(0, 1, $trans['stock_id']);
			$rep->TextCol(1, 2, $trans['description']);
			$rep->TextCol(2, 3, number_format2($trans['QtyOnHand'], user_qty_dec()));
			$rep->TextCol(3, 4, number_format2($trans['UnitCost'], $dec));
			$rep->TextCol(4, 5, number_format2($trans['ItemTotal'], $dec));
			$rep->fontsize += 2;
		}
		$total += $trans['ItemTotal'];
		$grandtotal += $trans['ItemTotal'];
	}
	if ($detail)
	{
		$rep->NewLine(2, 3);
		$rep->TextCol(0, 4, _('Total'));
	}	
	$rep->Textcol(4, 5, number_format2($total, $dec));
	if ($detail)
	{
		$rep->Line($rep->row - 2);
		$rep->NewLine();
	}
	$rep->NewLine(2, 1);
	$rep->TextCol(0, 4, _('Grand Total'));
	$rep->TextCol(4, 5, number_format2($grandtotal, $dec));
	$rep->Line($rep->row  - 4);
    $rep->End();
}

?>