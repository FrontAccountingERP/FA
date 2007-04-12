<?php

$page_security = 2;
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	Profit and Loss Statement
// ----------------------------------------------------------------
$path_to_root="../";

include_once($path_to_root . "includes/session.inc");
include_once($path_to_root . "includes/date_functions.inc");
include_once($path_to_root . "includes/data_checks.inc");
include_once($path_to_root . "gl/includes/gl_db.inc");

//----------------------------------------------------------------------------------------------------

// trial_inquiry_controls();
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
	global $path_to_root;

	include_once($path_to_root . "reporting/includes/pdf_report.inc");
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
	}
	else if ($dim == 1)
	{
		$dimension = $_POST['PARAM_3'];
		$graphics = $_POST['PARAM_4'];
		$comments = $_POST['PARAM_5'];
	}
	else
	{
		$graphics = $_POST['PARAM_3'];
		$comments = $_POST['PARAM_4'];
	}
	if ($graphics)
	{
		include_once($path_to_root . "reporting/includes/class.graphic.inc");
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

	$rep = new FrontReport(_('Profit and Loss Statement'), "ProfitAndLoss.pdf", user_pagesize());

	$rep->Font();
	$rep->Info($params, $cols, $headers, $aligns);
	$rep->Header();

	$classname = '';
	$group = '';
	$totalper = 0.0;
	$totalacc = 0.0;
	$classper = 0.0;
	$classacc = 0.0;
	$salesper = 0.0;
	$salesacc = 0.0;

	$accounts = get_gl_accounts_all(0);

	while ($account=db_fetch($accounts))
	{
		$per_balance = get_gl_trans_from_to($from, $to, $account["account_code"], $dimension, $dimension2);
		
		if ($compare == 2)
			$acc_balance = get_budget_trans_from_to($begin, $end, $account["account_code"], $dimension, $dimension2);
		else
			$acc_balance = get_gl_trans_from_to($begin, $end, $account["account_code"], $dimension, $dimension2);
		if (!$per_balance && !$acc_balance)
			continue;
			
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
				$rep->TextCol(2, 3,	number_format2($totalper, $dec));
				$rep->TextCol(3, 4,	number_format2($totalacc, $dec));
				$rep->TextCol(4, 5,	number_format2(Achieve($totalper, $totalacc), $pdec));
				if ($graphics)
				{
					$pg->x[] = $group;
					$pg->y[] = abs($totalper);
					$pg->z[] = abs($totalacc);
				}	
				$totalper = $totalacc = 0.0;
				$rep->row -= ($rep->lineHeight + 4);
				if ($closeclass)
				{
					$rep->Line($rep->row + 6);
					$rep->row -= 6;
					$rep->Font('bold');
					$rep->TextCol(0, 2,	_('Total') . " " . $classname);
					$rep->TextCol(2, 3,	number_format2($classper, $dec));
					$rep->TextCol(3, 4,	number_format2($classacc, $dec));
					$rep->TextCol(4, 5,	number_format2(Achieve($classper, $classacc), $pdec));
					$rep->Font();
					$salesper += $classper;
					$salesacc += $classacc;
					$classper = $classacc = 0.0;
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
			
		$per_balance *= -1;
		$acc_balance *= -1;
		$totalper += $per_balance;
		$totalacc += $acc_balance;
		$classper += $per_balance;
		$classacc += $acc_balance;
		$rep->TextCol(0, 1,	$account['account_code']);
		$rep->TextCol(1, 2,	$account['account_name']);

		$rep->TextCol(2, 3,	number_format2($per_balance, $dec));
		$rep->TextCol(3, 4,	number_format2($acc_balance, $dec));
		$rep->TextCol(4, 5,	number_format2(Achieve($per_balance, $acc_balance), $pdec));

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
			$rep->TextCol(2, 3,	number_format2($totalper, $dec));
			$rep->TextCol(3, 4,	number_format2($totalacc, $dec));
			$rep->TextCol(4, 5,	number_format2(Achieve($totalper, $totalacc), $pdec));
			if ($graphics)
			{
				$pg->x[] = $group;
				$pg->y[] = abs($totalper);
				$pg->z[] = abs($totalacc);
			}	
			$rep->row -= ($rep->lineHeight + 4);
			if ($closeclass)
			{
				$rep->Line($rep->row + 6);
				$calculateper = $salesper + $classper;
				$calculateacc = $salesacc + $classacc;
				$rep->row -= 6;

				$rep->Font('bold');
				$rep->TextCol(0, 2,	_('Total') . " " . $classname);
				$rep->TextCol(2, 3,	number_format2($classper, $dec));
				$rep->TextCol(3, 4,	number_format2($classacc, $dec));
				$rep->TextCol(4, 5,	number_format2(Achieve($classper, $classacc), $pdec));

				$rep->row -= ($rep->lineHeight + 8);
				$rep->TextCol(0, 2,	_('Calculated Return'));
				$rep->TextCol(2, 3,	number_format2($calculateper, $dec));
				$rep->TextCol(3, 4,	number_format2($calculateacc, $dec));
				$rep->TextCol(4, 5,	number_format2(Achieve($calculateper, $calculateacc), $pdec));
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
		$pg->fontfile  = $path_to_root . "reporting/fonts/Vera.ttf";
		$pg->latin_notation = ($decseps[$_SESSION["wa_current_user"]->prefs->dec_sep()] != ".");
		$filename = $path_to_root . "reporting/pdf_files/test.png";
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