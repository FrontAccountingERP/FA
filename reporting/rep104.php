<?php

$page_security = 2;
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	price Listing
// ----------------------------------------------------------------
$path_to_root="../";

include_once($path_to_root . "includes/session.inc");
include_once($path_to_root . "includes/date_functions.inc");
include_once($path_to_root . "includes/data_checks.inc");
include_once($path_to_root . "gl/includes/gl_db.inc");
include_once($path_to_root . "sales/includes/db/sales_types_db.inc");
include_once($path_to_root . "inventory/includes/db/items_category_db.inc");

//----------------------------------------------------------------------------------------------------

// trial_inquiry_controls();
print_price_listing();

function get_prices($category=0, $salestype=0)
{
		$sql = "SELECT ".TB_PREF."prices.sales_type_id,
				".TB_PREF."prices.stock_id,
				".TB_PREF."stock_master.description AS name,
				".TB_PREF."prices.curr_abrev,
				".TB_PREF."prices.price,
				".TB_PREF."sales_types.sales_type,
				".TB_PREF."stock_master.material_cost+".TB_PREF."stock_master.labour_cost+".TB_PREF."stock_master.overhead_cost AS Standardcost,
				".TB_PREF."stock_master.category_id,
				".TB_PREF."stock_category.description
			FROM ".TB_PREF."stock_master, 
				".TB_PREF."stock_category,
				".TB_PREF."sales_types,
				".TB_PREF."prices
			WHERE ".TB_PREF."stock_master.stock_id=".TB_PREF."prices.stock_id
				AND ".TB_PREF."prices.sales_type_id=".TB_PREF."sales_types.id
				AND ".TB_PREF."stock_master.category_id=".TB_PREF."stock_category.category_id";
		if ($salestype != null)
			$sql .= " AND ".TB_PREF."sales_types.id = '$salestype'";
		if ($category != null)
			$sql .= " AND ".TB_PREF."stock_category.category_id = '$category'";
		$sql .= " ORDER BY ".TB_PREF."prices.curr_abrev,
				".TB_PREF."stock_master.category_id,
				".TB_PREF."stock_master.stock_id";

    return db_query($sql,"No transactions were returned");
}

//----------------------------------------------------------------------------------------------------

function print_price_listing()
{
    global $path_to_root, $pic_height, $pic_width;

    include_once($path_to_root . "reporting/includes/pdf_report.inc");

    $category = $_POST['PARAM_0'];
    $salestype = $_POST['PARAM_1'];
    $pictures = $_POST['PARAM_2'];
    $showGP = $_POST['PARAM_3'];
    $comments = $_POST['PARAM_4'];
    
    $dec = user_price_dec();

	if ($category == reserved_words::get_all_numeric())
		$category = 0;
	if ($salestype == reserved_words::get_all_numeric())
		$salestype = 0;
	if ($category == 0)
		$cat = _('All');
	else
		$cat = get_category_name($category);
	if ($salestype == 0)
		$stype = _('All');
	else
		$stype = get_sales_type_name($salestype);
	if ($showGP == 0)
		$GP = _('No');
	else
		$GP = _('Yes');

	$cols = array(0, 100, 385, 450, 515);
	
	$headers = array(_('Category/Items'), _('Description'),	_('Price'),	_('GP %'));
	
	$aligns = array('left',	'left',	'right', 'right');
    
    $params =   array( 	0 => $comments,
    				    1 => array('text' => _('Category'), 'from' => $cat, 'to' => ''),
    				    2 => array('text' => _('Sales Type'), 'from' => $stype, 'to' => ''),
    				    3 => array(  'text' => _('Show GP %'),'from' => $GP,'to' => ''));

	if ($pictures)
		$user_comp = user_company();
	else
		$user_comp = "";
		
    $rep = new FrontReport(_('Price Listing'), "PriceListing.pdf", user_pagesize());

    $rep->Font();
    $rep->Info($params, $cols, $headers, $aligns);
    $rep->Header();

	$result = get_prices($category, $salestype);
	
	$currcode = '';
	$catgor = '';

	while ($myrow=db_fetch($result)) 
	{
		if ($currcode != $myrow['curr_abrev'])
		{
			$rep->NewLine(2);
			$rep->fontSize += 2;
			$rep->TextCol(0, 3,	$myrow['curr_abrev'] . " " . _('Prices'));
			$currcode = $myrow['curr_abrev'];
			$rep->fontSize -= 2;
			$rep->NewLine();
		}
		if ($catgor != $myrow['description'])
		{
			$rep->Line($rep->row  - $rep->lineHeight);
			$rep->NewLine(2);
			$rep->fontSize += 2;
			$rep->TextCol(0, 3, $myrow['category_id'] . " - " . $myrow['description']);
			$catgor = $myrow['description'];
			$rep->fontSize -= 2;
			$rep->NewLine();
		}
		$rep->NewLine();
		$rep->TextCol(0, 1,	$myrow['stock_id']);
		$rep->TextCol(1, 2, $myrow['name']);
		$rep->TextCol(2, 3,	number_format2($myrow['price'], $dec));
		if ($showGP)
		{
			if ($myrow['price'] != 0.0)
				$disp = ($myrow['price'] - $myrow['Standardcost']) * 100 / $myrow['price'];
			else
				$disp = 0.0;
			$rep->TextCol(3, 4,	number_format2($disp, user_percent_dec()) . " %");
		}	
		if ($pictures)
		{
			$image = $path_to_root . "inventory/manage/image/" . $user_comp . "/" . $myrow['stock_id'] . ".jpg";
			if (file_exists($image))
			{
				$rep->NewLine();
				if ($rep->row - $pic_height < $rep->bottomMargin)
					$rep->Header();
				$rep->AddImage($image, $rep->cols[1], $rep->row - $pic_height, $pic_width, $pic_height);
				$rep->row -= $pic_height;
				$rep->NewLine();
			}
		}	
		else
			$rep->NewLine(0, 1);
	}
	$rep->Line($rep->row  - 4);
    $rep->End();
}

?>