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
$page_security = 'SA_GLANALYTIC';
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	Annual expense breakdown
// ----------------------------------------------------------------
$path_to_root="..";

include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/data_checks.inc");
include_once($path_to_root . "/gl/includes/gl_db.inc");

//----------------------------------------------------------------------------------------------------

print_annual_expense_breakdown();

//----------------------------------------------------------------------------------------------------

function getPeriods($row, $account, $dimension, $dimension2)
{
	$yr = $row['yr'];
	$mo = $row['mo'];
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

    $sql = "SELECT SUM(CASE WHEN tran_date >= '$date01' AND tran_date < '$date02' THEN amount / 1000 ELSE 0 END) AS per01,
		   		SUM(CASE WHEN tran_date >= '$date02' AND tran_date < '$date03' THEN amount / 1000 ELSE 0 END) AS per02,
		   		SUM(CASE WHEN tran_date >= '$date03' AND tran_date < '$date04' THEN amount / 1000 ELSE 0 END) AS per03,
		   		SUM(CASE WHEN tran_date >= '$date04' AND tran_date < '$date05' THEN amount / 1000 ELSE 0 END) AS per04,
		   		SUM(CASE WHEN tran_date >= '$date05' AND tran_date < '$date06' THEN amount / 1000 ELSE 0 END) AS per05,
		   		SUM(CASE WHEN tran_date >= '$date06' AND tran_date < '$date07' THEN amount / 1000 ELSE 0 END) AS per06,
		   		SUM(CASE WHEN tran_date >= '$date07' AND tran_date < '$date08' THEN amount / 1000 ELSE 0 END) AS per07,
		   		SUM(CASE WHEN tran_date >= '$date08' AND tran_date < '$date09' THEN amount / 1000 ELSE 0 END) AS per08,
		   		SUM(CASE WHEN tran_date >= '$date09' AND tran_date < '$date10' THEN amount / 1000 ELSE 0 END) AS per09,
		   		SUM(CASE WHEN tran_date >= '$date10' AND tran_date < '$date11' THEN amount / 1000 ELSE 0 END) AS per10,
		   		SUM(CASE WHEN tran_date >= '$date11' AND tran_date < '$date12' THEN amount / 1000 ELSE 0 END) AS per11,
		   		SUM(CASE WHEN tran_date >= '$date12' AND tran_date < '$date13' THEN amount / 1000 ELSE 0 END) AS per12
    			FROM ".TB_PREF."gl_trans
				WHERE account='$account'";
	if ($dimension != 0)
  		$sql .= " AND dimension_id = ".($dimension<0?0:db_escape($dimension));
	if ($dimension2 != 0)
  		$sql .= " AND dimension2_id = ".($dimension2<0?0:db_escape($dimension2));

	$result = db_query($sql, "Transactions for account $account could not be calculated");

	return db_fetch($result);
}

//----------------------------------------------------------------------------------------------------

function print_annual_expense_breakdown()
{
	global $path_to_root, $date_system;

	$dim = get_company_pref('use_dimension');
	$dimension = $dimension2 = 0;

	if ($dim == 2)
	{
		$year = $_POST['PARAM_0'];
		$dimension = $_POST['PARAM_1'];
		$dimension2 = $_POST['PARAM_2'];
		$comments = $_POST['PARAM_3'];
		$destination = $_POST['PARAM_4'];
	}
	else if ($dim == 1)
	{
		$year = $_POST['PARAM_0'];
		$dimension = $_POST['PARAM_1'];
		$comments = $_POST['PARAM_2'];
		$destination = $_POST['PARAM_3'];
	}
	else
	{
		$year = $_POST['PARAM_0'];
		$comments = $_POST['PARAM_1'];
		$destination = $_POST['PARAM_2'];
	}
	if ($destination)
		include_once($path_to_root . "/reporting/includes/excel_report.inc");
	else
		include_once($path_to_root . "/reporting/includes/pdf_report.inc");

	$dec = 1;
	//$pdec = user_percent_dec();

	$cols = array(0, 40, 150, 180, 210, 240, 270, 300, 330, 360, 390, 420, 450, 480, 510);
	//------------0--1---2----3----4----5----6----7----8----10---11---12---13---14---15-

	//$yr = date('Y');
	//$mo = date('m'):
	// from now
	$sql = "SELECT begin, end, YEAR(end) AS yr, MONTH(end) AS mo FROM ".TB_PREF."fiscal_year WHERE id=".db_escape($year);
	$result = db_query($sql, "could not get fiscal year");
	$row = db_fetch($result);
	
	$year = sql2date($row['begin'])." - ".sql2date($row['end']);
	$yr = $row['yr'];
	$mo = $row['mo'];
	$da = 1;
	if ($date_system == 1)
		list($yr, $mo, $da) = jalali_to_gregorian($yr, $mo, $da);
	elseif ($date_system == 2)
		list($yr, $mo, $da) = islamic_to_gregorian($yr, $mo, $da);
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

	$rep = new FrontReport(_('Annual Expense Breakdown'), "AnnualBreakDown", user_pagesize());

	$rep->Font();
	$rep->Info($params, $cols, $headers, $aligns);
	$rep->Header();

	$classname = '';
	$total = Array(
		0 => Array(1 => 0,0,0,0,0,0,0,0,0,0,0,0),
			Array(1 => 0,0,0,0,0,0,0,0,0,0,0,0),
			Array(1 => 0,0,0,0,0,0,0,0,0,0,0,0),
			Array(1 => 0,0,0,0,0,0,0,0,0,0,0,0),
			Array(1 => 0,0,0,0,0,0,0,0,0,0,0,0),
			Array(1 => 0,0,0,0,0,0,0,0,0,0,0,0),
			Array(1 => 0,0,0,0,0,0,0,0,0,0,0,0),
			Array(1 => 0,0,0,0,0,0,0,0,0,0,0,0),
			Array(1 => 0,0,0,0,0,0,0,0,0,0,0,0),
			Array(1 => 0,0,0,0,0,0,0,0,0,0,0,0));
	$total2 = Array(1 => 0,0,0,0,0,0,0,0,0,0,0,0);
	$sales = Array(1 => 0,0,0,0,0,0,0,0,0,0,0,0);
	$calc = Array(1 => 0,0,0,0,0,0,0,0,0,0,0,0);
	$typename = array('','','','','','','','','','');
	$closing = array(-1,-1,-1,-1,-1,-1,-1,-1,-1,-1);
	$level = 0;
	$last = -1;

	$closeclass = false;
	$convert = 1;
	$ctype = 0;

	$accounts = get_gl_accounts_all(0);

	while ($account=db_fetch($accounts))
	{
		if ($account['account_code'] == null && $account['parent'] > 0)
			continue;

		if ($account['account_code'] != null)
		{
			$bal = getPeriods($row, $account["account_code"], $dimension, $dimension2);
			if (!$bal['per01'] && !$bal['per02'] && !$bal['per03'] && !$bal['per04'] &&
				!$bal['per05'] && !$bal['per06'] && !$bal['per07'] && !$bal['per08'] &&
				!$bal['per09'] && !$bal['per10'] && !$bal['per11'] && !$bal['per12'])
				continue;
		}
		if ($account['AccountClassName'] != $classname)
		{
			if ($classname != '')
			{
				$closeclass = true;
			}
		}

		if ($account['AccountTypeName'] != $typename[$level])
		{
			if ($typename[$level] != '')
			{
				for ( ; $level >= 0, $typename[$level] != ''; $level--) 
				{
					if ($account['parent'] == $closing[$level] || $account['parent'] < $last || $account['parent'] <= 0 || $closeclass)
					{
						$rep->row += 6;
						$rep->Line($rep->row);
						$rep->NewLine();
						$rep->TextCol(0, 2,	_('Total') . " " . $typename[$level]);
						for ($i = 1; $i <= 12; $i++)
						{
							$rep->AmountCol($i + 1, $i + 2, $total[$level][$i] * $convert, $dec);
							$total[$level][$i] = 0.0;
						}
					}
					else
						break;
					$rep->NewLine();
				}	
				if ($closeclass)
				{
					$rep->row += 6;
					$rep->Line($rep->row);
					$rep->NewLine();
					$rep->Font('bold');
					$rep->TextCol(0, 2,	_('Total') . " " . $classname);
					for ($i = 1; $i <= 12; $i++)
					{
						$rep->AmountCol($i + 1, $i + 2, $total2[$i] * $convert, $dec);
						$sales[$i] += $total2[$i];
					}
					$rep->Font();
					$total2 = Array(1 => 0,0,0,0,0,0,0,0,0,0,0,0);
					$rep->NewLine(2);
					$closeclass = false;
				}
			}
			if ($account['AccountClassName'] != $classname)
			{
				$rep->Font('bold');
				$rep->TextCol(0, 5, $account['AccountClassName']);
				$rep->Font();
				$rep->NewLine();
			}
			$level++;
			if ($account['parent'] != $last)
				$last = $account['parent'];
			$typename[$level] = $account['AccountTypeName'];
			$closing[$level] = $account['parent'];
			$rep->row -= 4;
			$rep->TextCol(0, 5, $account['AccountTypeName']);
			$rep->row -= 4;
			$rep->Line($rep->row);
			$rep->NewLine();
		}
		$classname = $account['AccountClassName'];
		$ctype = $account['ClassType'];
		$convert = get_class_type_convert($ctype); 

		if ($account['account_code'] != null)
		{
			$balance = array(1 => $bal['per01'], $bal['per02'], $bal['per03'], $bal['per04'],
				$bal['per05'], $bal['per06'], $bal['per07'], $bal['per08'],
				$bal['per09'], $bal['per10'], $bal['per11'], $bal['per12']);
			$rep->TextCol(0, 1,	$account['account_code']);
			$rep->TextCol(1, 2,	$account['account_name']);

			for ($i = 1; $i <= 12; $i++)
			{
				$rep->AmountCol($i + 1, $i + 2, $balance[$i] * $convert, $dec);
				$total2[$i] += $balance[$i];
			}
			for ($j = 0; $j <= $level; $j++)
			{
				for ($i = 1; $i <= 12; $i++)
					$total[$j][$i] += $balance[$i];
			}
			$rep->NewLine();

			if ($rep->row < $rep->bottomMargin + 3 * $rep->lineHeight)
			{
				$rep->Line($rep->row - 2);
				$rep->Header();
			}
		}	
	}
	if ($account['AccountClassName'] != $classname)
	{
		if ($classname != '')
		{
			$closeclass = true;
		}
	}
	if ($account['AccountTypeName'] != $typename[$level])
	{
		if ($typename[$level] != '')
		{
			for ( ; $level >= 0, $typename[$level] != ''; $level--) 
			{
				if ($account['parent'] == $closing[$level] || $account['parent'] < $last || $account['parent'] <= 0 || $closeclass)
				{
					$rep->row += 6;
					$rep->Line($rep->row);
					$rep->NewLine();
					$rep->TextCol(0, 2,	_('Total') . " " . $typename[$level]);
					for ($i = 1; $i <= 12; $i++)
					{
						$rep->AmountCol($i + 1, $i + 2, $total[$level][$i] * $convert, $dec);
						$total[$level][$i] = 0.0;
					}
				}
				else
					break;
				$rep->NewLine();
			}	
			if ($closeclass)
			{
				$rep->row += 6;
				$rep->Line($rep->row);
				$rep->NewLine();

				$rep->Font('bold');
				$rep->TextCol(0, 2,	_('Total') . " " . $classname);
				for ($i = 1; $i <= 12; $i++)
				{
					$rep->AmountCol($i + 1, $i + 2, $total2[$i] * $convert, $dec);
					$calc[$i] = $sales[$i] + $total2[$i];
				}

				$rep->NewLine(2);
				$rep->TextCol(0, 2,	_('Calculated Return'));
				for ($i = 1; $i <= 12; $i++)
					$rep->AmountCol($i + 1, $i + 2, $calc[$i] * -1, $dec); // always convert
				$rep->Font();

				$rep->NewLine();
			}
		}
	}
	$rep->Line($rep->row);
	$rep->End();
}

?>