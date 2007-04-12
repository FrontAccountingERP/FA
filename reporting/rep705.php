<?php

$page_security = 2;
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	Annual expense breakdown
// ----------------------------------------------------------------
$path_to_root="../";

include_once($path_to_root . "includes/session.inc");
include_once($path_to_root . "includes/date_functions.inc");
include_once($path_to_root . "includes/data_checks.inc");
include_once($path_to_root . "gl/includes/gl_db.inc");

//----------------------------------------------------------------------------------------------------

// trial_inquiry_controls();
print_annual_expense_breakdown();

//----------------------------------------------------------------------------------------------------

function getPeriods($year, $account, $dimension, $dimension2)
{
	//$yr = date('Y');
	//$mo = date('m'):
	// from now
	$yr = $year;
	$mo = 12;
	$date13 = date('Y-m-d',mktime(0,0,0,$mo+1,1,$yr));
	$date12 = date('Y-m-d',mktime(0,0,0,$mo,1,$yr));
	$date11 = date('Y-m-d',mktime(0,0,0,$mo-1,1,$yr));
	$date10 = date('Y-m-d',mktime(0,0,0,$mo-2,1,$yr));
	$date09 = date('Y-m-d',mktime(0,0,0,$mo-3,1,$yr));
	$date08 = date('Y-m-d',mktime(0,0,0,$mo-4,1,$yr));
	$date07 = date('Y-m-d',mktime(0,0,0,$mo-5,1,$yr));
	$date06 = date('Y-m-d',mktime(0,0,0,$mo-6,1,$yr));
	$date05 = date('Y-m-d',mktime(0,0,0,$mo-7,1,$yr));
	$date04 = date('Y-m-d',mktime(0,0,0,$mo-8,1,$yr));
	$date03 = date('Y-m-d',mktime(0,0,0,$mo-9,1,$yr));
	$date02 = date('Y-m-d',mktime(0,0,0,$mo-10,1,$yr));
	$date01 = date('Y-m-d',mktime(0,0,0,$mo-11,1,$yr));

    $sql = "SELECT SUM(CASE WHEN tran_date >= '$date01' AND tran_date < '$date02' THEN -amount / 1000 ELSE 0 END) AS per01,
		   		SUM(CASE WHEN tran_date >= '$date02' AND tran_date < '$date03' THEN -amount / 1000 ELSE 0 END) AS per02,
		   		SUM(CASE WHEN tran_date >= '$date03' AND tran_date < '$date04' THEN -amount / 1000 ELSE 0 END) AS per03,
		   		SUM(CASE WHEN tran_date >= '$date04' AND tran_date < '$date05' THEN -amount / 1000 ELSE 0 END) AS per04,
		   		SUM(CASE WHEN tran_date >= '$date05' AND tran_date < '$date06' THEN -amount / 1000 ELSE 0 END) AS per05,
		   		SUM(CASE WHEN tran_date >= '$date06' AND tran_date < '$date07' THEN -amount / 1000 ELSE 0 END) AS per06,
		   		SUM(CASE WHEN tran_date >= '$date07' AND tran_date < '$date08' THEN -amount / 1000 ELSE 0 END) AS per07,
		   		SUM(CASE WHEN tran_date >= '$date08' AND tran_date < '$date09' THEN -amount / 1000 ELSE 0 END) AS per08,
		   		SUM(CASE WHEN tran_date >= '$date09' AND tran_date < '$date10' THEN -amount / 1000 ELSE 0 END) AS per09,
		   		SUM(CASE WHEN tran_date >= '$date10' AND tran_date < '$date11' THEN -amount / 1000 ELSE 0 END) AS per10,
		   		SUM(CASE WHEN tran_date >= '$date11' AND tran_date < '$date12' THEN -amount / 1000 ELSE 0 END) AS per11,
		   		SUM(CASE WHEN tran_date >= '$date12' AND tran_date < '$date13' THEN -amount / 1000 ELSE 0 END) AS per12
    			FROM ".TB_PREF."gl_trans
				WHERE account='$account'";
	if ($dimension > 0)
		$sql .= " AND dimension_id = $dimension";
	if ($dimension2 > 0)
		$sql .= " AND dimension2_id = $dimension2";

	$result = db_query($sql, "Transactions for account $account could not be calculated");

	return db_fetch($result);
}

//----------------------------------------------------------------------------------------------------

function print_annual_expense_breakdown()
{
	global $path_to_root;

	include_once($path_to_root . "reporting/includes/pdf_report.inc");
	$dim = get_company_pref('use_dimension');
	$dimension = $dimension2 = 0;

	if ($dim == 2)
	{
		$year = $_POST['PARAM_0'];
		$dimension = $_POST['PARAM_1'];
		$dimension2 = $_POST['PARAM_2'];
		$comments = $_POST['PARAM_3'];
	}
	else if ($dim == 1)
	{
		$year = $_POST['PARAM_0'];
		$dimension = $_POST['PARAM_1'];
		$comments = $_POST['PARAM_2'];
	}
	else
	{
		$year = $_POST['PARAM_0'];
		$comments = $_POST['PARAM_1'];
	}
	$dec = 1;
	//$pdec = user_percent_dec();

	$cols = array(0, 40, 150, 180, 210, 240, 270, 300, 330, 360, 390, 420, 450, 480, 510);
	//------------0--1---2----3----4----5----6----7----8----10---11---12---13---14---15-
	
	//$yr = date('Y');
	//$mo = date('m'):
	// from now
	$yr = $year;
	$mo = 12;
	$da = 1;
	$per12 = strftime('%b',mktime(0,0,0,$mo,$da,$yr));
	$per11 = strftime('%b',mktime(0,0,0,$mo-1,$da,$yr));
	$per10 = strftime('%b',mktime(0,0,0,$mo-2,$da,$yr));
	$per09 = strftime('%b',mktime(0,0,0,$mo-3,$da,$yr));
	$per08 = strftime('%b',mktime(0,0,0,$mo-4,$da,$yr));
	$per07 = strftime('%b',mktime(0,0,0,$mo-5,$da,$yr));
	$per06 = strftime('%b',mktime(0,0,0,$mo-6,$da,$yr));
	$per05 = strftime('%b',mktime(0,0,0,$mo-7,$da,$yr));
	$per04 = strftime('%b',mktime(0,0,0,$mo-8,$da,$yr));
	$per03 = strftime('%b',mktime(0,0,0,$mo-9,$da,$yr));
	$per02 = strftime('%b',mktime(0,0,0,$mo-10,$da,$yr));
	$per01 = strftime('%b',mktime(0,0,0,$mo-11,$da,$yr));
	
	$headers = array(_('Account'), _('Account Name'), $per01, $per02, $per03, $per04,
		$per05, $per06, $per07, $per08, $per09, $per10, $per11, $per12);
	
	$aligns = array('left',	'left',	'right', 'right', 'right',	'right', 'right', 'right',
		'right', 'right', 'right',	'right', 'right', 'right');
    
    if ($dim == 2)
    {
    	$params =   array( 	0 => $comments,
                    	1 => array('text' => _("Year"), 
                    		'from' => $year, 'to' => ''),
                    	2 => array('text' => _("Dimension")." 1", 
                    		'from' => get_dimension_string($dimension), 'to' => ''),
                    	3 => array('text' => _("Dimension")." 2", 
                    		'from' => get_dimension_string($dimension2), 'to' => ''),
                    	4 => array('text' => _('Info'), 'from' => _('Amounts in thousands'),
                    		'to' => ''));
    }
    else if ($dim == 1)
    {
    	$params =   array( 	0 => $comments,
                    	1 => array('text' => _("Year"), 
                    		'from' => $year, 'to' => ''),
                    	2 => array('text' => _('Dimension'), 
                    		'from' => get_dimension_string($dimension), 'to' => ''),
                    	3 => array('text' => _('Info'), 'from' => _('Amounts in thousands'),
                    		'to' => ''));
    }
    else
    {
    	$params =   array( 	0 => $comments,
                    	1 => array('text' => _("Year"), 
                    		'from' => $year, 'to' => ''),
                    	2 => array('text' => _('Info'), 'from' => _('Amounts in thousands'),
                    		'to' => ''));
    }

	$rep = new FrontReport(_('Annual Expense Breakdown'), "AnnualBreakDown.pdf", user_pagesize());

	$rep->Font();
	$rep->Info($params, $cols, $headers, $aligns);
	$rep->Header();

	$classname = '';
	$group = '';
	$total = Array();
	$total2 = Array();
	$sales = Array();
	$calc = Array();
	unset($total);
	unset($total2);
	unset($sales);
	unset($calc);
	$accounts = get_gl_accounts_all(0);

	while ($account = db_fetch($accounts))
	{
		$bal = getPeriods($year, $account["account_code"], $dimension, $dimension2);
		if (!$bal['per01'] && !$bal['per02'] && !$bal['per03'] && !$bal['per04'] && 
			!$bal['per05'] && !$bal['per06'] && !$bal['per07'] && !$bal['per08'] && 
			!$bal['per09'] && !$bal['per10'] && !$bal['per11'] && !$bal['per12'])
			continue;
		//if (array_sum($bal) == 0.0)
		//$i = 1;
		//foreach ($bal as $b)
		//	$balance[$i++] = $b;
		//$balance = $bal;
		$balance = Array(1 => $bal['per01'], $bal['per02'], $bal['per03'], $bal['per04'], 
			$bal['per05'], $bal['per06'], $bal['per07'], $bal['per08'], 
			$bal['per09'], $bal['per10'], $bal['per11'], $bal['per12']);
		if ($account['AccountClassName'] != $classname)
		{
			if ($classname != '')
			{
				$closeclass = true;
			}
		}

		if ($account['AccountTypeName'] != $group)
		{
			if ($group != '')
			{
				$rep->Line($rep->row + 6);
				$rep->row -= 6;
				$rep->TextCol(0, 2,	_('Total') . " " . $group);
				for ($i = 1; $i <= 12; $i++)
					$rep->TextCol($i + 1, $i + 2, number_format2($total[$i], $dec));
				unset($total);
				$rep->row -= ($rep->lineHeight + 4);
				if ($closeclass)
				{
					$rep->Line($rep->row + 6);
					$rep->row -= 6;
					$rep->Font('bold');
					$rep->TextCol(0, 2,	_('Total') . " " . $classname);
					for ($i = 1; $i <= 12; $i++)
					{
						$rep->TextCol($i + 1, $i + 2, number_format2($total2[$i], $dec));
						$sales[$i] += $total2[$i];
					}	
					$rep->Font();
					unset($total2);
					$rep->NewLine(3);
					$closeclass = false;
				}
			}
			if ($account['AccountClassName'] != $classname)
			{
				$rep->Font('bold');
				$rep->TextCol(0, 5, $account['AccountClassName']);
				$rep->Font();
				$rep->row -= ($rep->lineHeight + 4);
			}
			$group = $account['AccountTypeName'];
			$rep->TextCol(0, 5, $account['AccountTypeName']);
			$rep->Line($rep->row - 4);
			$rep->row -= ($rep->lineHeight + 4);
		}
		$classname = $account['AccountClassName'];
		$rep->TextCol(0, 1,	$account['account_code']);
		$rep->TextCol(1, 2,	$account['account_name']);
		for ($i = 1; $i <= 12; $i++)
		{
			$rep->TextCol($i + 1, $i + 2, number_format2($balance[$i], $dec));
			$total[$i] += $balance[$i];
			$total2[$i] += $balance[$i];
		}			

		$rep->NewLine();

		if ($rep->row < $rep->bottomMargin + 3 * $rep->lineHeight)
		{
			$rep->Line($rep->row - 2);
			$rep->Header();
		}
	}
	if ($account['AccountClassName'] != $classname)
	{
		if ($classname != '')
		{
			$closeclass = true;
		}
	}
	if ($account['AccountTypeName'] != $group)
	{
		if ($group != '')
		{
			$rep->Line($rep->row + 6);
			$rep->row -= 6;
			$rep->TextCol(0, 2,	_('Total') . " " . $group);
			for ($i = 1; $i <= 12; $i++)
				$rep->TextCol($i + 1, $i + 2, number_format2($total[$i], $dec));
			$rep->row -= ($rep->lineHeight + 4);
			if ($closeclass)
			{
				$rep->Line($rep->row + 6);
				$rep->row -= 6;

				$rep->Font('bold');
				$rep->TextCol(0, 2,	_('Total') . " " . $classname);
				for ($i = 1; $i <= 12; $i++)
				{
					$rep->TextCol($i + 1, $i + 2, number_format2($total2[$i], $dec));
					$calc[$i] = $sales[$i] + $total2[$i];
				}	

				$rep->row -= ($rep->lineHeight + 8);
				$rep->TextCol(0, 2,	_('Calculated Return'));
				for ($i = 1; $i <= 12; $i++)
					$rep->TextCol($i + 1, $i + 2, number_format2($calc[$i], $dec));
				$rep->Font();

				$rep->NewLine();
			}
		}
	}
	$rep->Line($rep->row);
	$rep->End();
}

?>