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
// Title:	Profit and Loss Statement
// ----------------------------------------------------------------
$path_to_root="..";

include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/data_checks.inc");
include_once($path_to_root . "/gl/includes/gl_db.inc");

//----------------------------------------------------------------------------------------------------

print_profit_and_loss_statement();

//----------------------------------------------------------------------------------------------------

function Achieve($d1, $d2)
{
	if ($d1 == 0 && $d2 == 0)
		return 0;
	elseif ($d2 == 0)
		return 999;
	$ret = ($d1 / $d2 * 100.0);
	if ($ret > 999)
		$ret = 999;
	return $ret;
}

//----------------------------------------------------------------------------------------------------

function print_profit_and_loss_statement()
{
	global $comp_path, $path_to_root;

	$dim = get_company_pref('use_dimension');
	$dimension = $dimension2 = 0;

	$from = $_POST['PARAM_0'];
	$to = $_POST['PARAM_1'];
	$compare = $_POST['PARAM_2'];
	if ($dim == 2)
	{
		$dimension = $_POST['PARAM_3'];
		$dimension2 = $_POST['PARAM_4'];
		$graphics = $_POST['PARAM_5'];
		$comments = $_POST['PARAM_6'];
		$destination = $_POST['PARAM_7'];
	}
	else if ($dim == 1)
	{
		$dimension = $_POST['PARAM_3'];
		$graphics = $_POST['PARAM_4'];
		$comments = $_POST['PARAM_5'];
		$destination = $_POST['PARAM_6'];
	}
	else
	{
		$graphics = $_POST['PARAM_3'];
		$comments = $_POST['PARAM_4'];
		$destination = $_POST['PARAM_5'];
	}
	if ($destination)
		include_once($path_to_root . "/reporting/includes/excel_report.inc");
	else
		include_once($path_to_root . "/reporting/includes/pdf_report.inc");
	if ($graphics)
	{
		include_once($path_to_root . "/reporting/includes/class.graphic.inc");
		$pg = new graph();
	}
	$dec = 0;
	$pdec = user_percent_dec();

	$cols = array(0, 50, 200, 350, 425,	500);
	//------------0--1---2----3----4----5--

	$headers = array(_('Account'), _('Account Name'), _('Period'), _('Accumulated'), _('Achieved %'));

	$aligns = array('left',	'left',	'right', 'right', 'right');

    if ($dim == 2)
    {
    	$params =   array( 	0 => $comments,
    				    1 => array('text' => _('Period'),'from' => $from, 'to' => $to),
                    	2 => array('text' => _('Dimension')." 1",
                            'from' => get_dimension_string($dimension), 'to' => ''),
                    	3 => array('text' => _('Dimension')." 2",
                            'from' => get_dimension_string($dimension2), 'to' => ''));
    }
    else if ($dim == 1)
    {
    	$params =   array( 	0 => $comments,
    				    1 => array('text' => _('Period'),'from' => $from, 'to' => $to),
                    	2 => array('text' => _('Dimension'),
                            'from' => get_dimension_string($dimension), 'to' => ''));
    }
    else
    {
    	$params =   array( 	0 => $comments,
    				    1 => array('text' => _('Period'),'from' => $from, 'to' => $to));
    }


	if ($compare == 0 || $compare == 2)
	{
		$end = $to;
		if ($compare == 2)
		{
			$begin = $from;
			$headers[3] = _('Budget');
		}
		else
			$begin = begin_fiscalyear();
	}
	elseif ($compare == 1)
	{
		$begin = add_months($from, -12);
		$end = add_months($to, -12);
		$headers[3] = _('Period Y-1');
	}

	$rep = new FrontReport(_('Profit and Loss Statement'), "ProfitAndLoss", user_pagesize());

	$rep->Font();
	$rep->Info($params, $cols, $headers, $aligns);
	$rep->Header();

	$classname = '';
	
	$typeper = array(0,0,0,0,0,0,0,0,0,0);
	$typeacc = array(0,0,0,0,0,0,0,0,0,0);
	$typename = array('','','','','','','','','','');
	$closing = array(-1,-1,-1,-1,-1,-1,-1,-1,-1,-1);
	$level = 0;

	$classper = 0.0;
	$classacc = 0.0;
	$salesper = 0.0;
	$salesacc = 0.0;
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
			$per_balance = get_gl_trans_from_to($from, $to, $account["account_code"], $dimension, $dimension2);

			if ($compare == 2)
				$acc_balance = get_budget_trans_from_to($begin, $end, $account["account_code"], $dimension, $dimension2);
			else
				$acc_balance = get_gl_trans_from_to($begin, $end, $account["account_code"], $dimension, $dimension2);
			if (!$per_balance && !$acc_balance)
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
						$rep->AmountCol(2, 3, $typeper[$level] * $convert, $dec);
						$rep->AmountCol(3, 4, $typeacc[$level] * $convert, $dec);
						$rep->AmountCol(4, 5, Achieve($typeper[$level], $typeacc[$level]), $pdec);
						if ($graphics)
						{
							$pg->x[] = $typename[$level];
							$pg->y[] = abs($typeper[$level]);
							$pg->z[] = abs($typeacc[$level]);
						}
						$typeper[$level] = $typeacc[$level] = 0.0;
					}
					else
						break;
					$rep->NewLine();
				}
				//$rep->NewLine();
				if ($closeclass)
				{
					$rep->row += 6;
					$rep->Line($rep->row);
					$rep->NewLine();
					$rep->Font('bold');
					$rep->TextCol(0, 2,	_('Total') . " " . $classname);
					$rep->AmountCol(2, 3, $classper * $convert, $dec);
					$rep->AmountCol(3, 4, $classacc * $convert, $dec);
					$rep->AmountCol(4, 5, Achieve($classper, $classacc), $pdec);
					$rep->Font();
					$salesper += $classper;
					$salesacc += $classacc;
					$classper = $classacc = 0.0;
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
			//$per_balance *= -1;
			//$acc_balance *= -1;
		
			for ($i = 0; $i <= $level; $i++)
			{
				$typeper[$i] += $per_balance;
				$typeacc[$i] += $acc_balance;
			}
			$classper += $per_balance;
			$classacc += $acc_balance;
			$rep->TextCol(0, 1,	$account['account_code']);
			$rep->TextCol(1, 2,	$account['account_name']);

			$rep->AmountCol(2, 3, $per_balance * $convert, $dec);
			$rep->AmountCol(3, 4, $acc_balance * $convert, $dec);
			$rep->AmountCol(4, 5, Achieve($per_balance, $acc_balance), $pdec);

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
					$rep->AmountCol(2, 3, $typeper[$level] * $convert, $dec);
					$rep->AmountCol(3, 4, $typeacc[$level] * $convert, $dec);
					$rep->AmountCol(4, 5, Achieve($typeper[$level], $typeacc[$level]), $pdec);
					if ($graphics)
					{
						$pg->x[] = $typename[$level];
						$pg->y[] = abs($typeper[$level]);
						$pg->z[] = abs($typeacc[$level]);
					}
					$typeper[$level] = $typeacc[$level] = 0.0;
				}
				else
					break;
				$rep->NewLine();
			}
			//$rep->NewLine();
			if ($closeclass)
			{
				$rep->Line($rep->row + 6);
				$calculateper = $salesper + $classper;
				$calculateacc = $salesacc + $classacc;
				$rep->row += 6;
				$rep->Line($rep->row);
				$rep->NewLine();

				$rep->Font('bold');
				$rep->TextCol(0, 2,	_('Total') . " " . $classname);
				$rep->AmountCol(2, 3, $classper * $convert, $dec);
				$rep->AmountCol(3, 4, $classacc * $convert, $dec);
				$rep->AmountCol(4, 5, Achieve($classper, $classacc), $pdec);

				$rep->NewLine(2);
				$rep->TextCol(0, 2,	_('Calculated Return'));
				$rep->AmountCol(2, 3, $calculateper *-1, $dec); // always convert
				$rep->AmountCol(3, 4, $calculateacc * -1, $dec);
				$rep->AmountCol(4, 5, Achieve($calculateper, $calculateacc), $pdec);
				if ($graphics)
				{
					$pg->x[] = _('Calculated Return');
					$pg->y[] = abs($calculateper);
					$pg->z[] = abs($calculateacc);
				}

				$rep->Font();

				$rep->NewLine();
			}
		}
	}
	$rep->Line($rep->row);
	if ($graphics)
	{
		global $decseps, $graph_skin;
		$pg->title     = $rep->title;
		$pg->axis_x    = _("Group");
		$pg->axis_y    = _("Amount");
		$pg->graphic_1 = $headers[2];
		$pg->graphic_2 = $headers[3];
		$pg->type      = $graphics;
		$pg->skin      = $graph_skin;
		$pg->built_in  = false;
		$pg->fontfile  = $path_to_root . "/reporting/fonts/Vera.ttf";
		$pg->latin_notation = ($decseps[$_SESSION["wa_current_user"]->prefs->dec_sep()] != ".");
		$filename = $comp_path.'/'.user_company(). "/pdf_files/test.png";
		$pg->display($filename, true);
		$w = $pg->width / 1.5;
		$h = $pg->height / 1.5;
		$x = ($rep->pageWidth - $w) / 2;
		$rep->NewLine(2);
		if ($rep->row - $h < $rep->bottomMargin)
			$rep->Header();
		$rep->AddImage($filename, $x, $rep->row - $h, $w, $h);
	}
	$rep->End();
}

?>