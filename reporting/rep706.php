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
// Title:	Balance Sheet
// ----------------------------------------------------------------
$path_to_root="..";

include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/data_checks.inc");
include_once($path_to_root . "/gl/includes/gl_db.inc");

//----------------------------------------------------------------------------------------------------

print_balance_sheet();


//----------------------------------------------------------------------------------------------------

function print_balance_sheet()
{
	global $comp_path, $path_to_root;

	$dim = get_company_pref('use_dimension');
	$dimension = $dimension2 = 0;

	$from = $_POST['PARAM_0'];
	$to = $_POST['PARAM_1'];
	if ($dim == 2)
	{
		$dimension = $_POST['PARAM_2'];
		$dimension2 = $_POST['PARAM_3'];
		$graphics = $_POST['PARAM_4'];
		$comments = $_POST['PARAM_5'];
		$destination = $_POST['PARAM_6'];
	}
	else if ($dim == 1)
	{
		$dimension = $_POST['PARAM_2'];
		$graphics = $_POST['PARAM_3'];
		$comments = $_POST['PARAM_4'];
		$destination = $_POST['PARAM_5'];
	}
	else
	{
		$graphics = $_POST['PARAM_2'];
		$comments = $_POST['PARAM_3'];
		$destination = $_POST['PARAM_4'];
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

	$cols = array(0, 50, 200, 350, 425,	500);
	//------------0--1---2----3----4----5--

	$headers = array(_('Account'), _('Account Name'), _('Open Balance'), _('Period'),
		_('Close Balance'));

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

	$rep = new FrontReport(_('Balance Sheet'), "BalanceSheet", user_pagesize());
	$rep->Font();
	$rep->Info($params, $cols, $headers, $aligns);
	$rep->Header();
	$classname = '';
	$group = '';
	$totalopen = 0.0;
	$totalperiod = 0.0;
	$totalclose = 0.0;
	$classopen = 0.0;
	$classperiod = 0.0;
	$classclose = 0.0;
	$assetsopen = 0.0;
	$assetsperiod = 0.0;
	$assetsclose = 0.0;
	$closeclass = false;
	$convert = 1;
	$rep->NewLine();

	$accounts = get_gl_accounts_all(1);

	while ($account=db_fetch($accounts))
	{
		$prev_balance = get_gl_balance_from_to("", $from, $account["account_code"], $dimension, $dimension2);

		$curr_balance = get_gl_trans_from_to($from, $to, $account["account_code"], $dimension, $dimension2);

		if (!$prev_balance && !$curr_balance)
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
				$rep->row += 6;
				$rep->Line($rep->row);
				$rep->NewLine();
				$rep->TextCol(0, 2,	_('Total') . " " . $group);
				$rep->AmountCol(2, 3, $totalopen * $convert, $dec);
				$rep->AmountCol(3, 4, $totalperiod * $convert, $dec);
				$rep->AmountCol(4, 5, $totalclose * $convert, $dec);
				if ($graphics)
				{
					$pg->x[] = $group;
					$pg->y[] = abs($totalclose);
				}
				$totalopen = $totalperiod = $totalclose = 0.0;
				$rep->NewLine();
				if ($closeclass)
				{
					$rep->row += 6;
					$rep->Line($rep->row);
					$rep->NewLine();
					$rep->Font('bold');
		 			$rep->TextCol(0, 2,	_('Total') . " " . $classname);
					$rep->AmountCol(2, 3, $classopen * $convert, $dec);
					$rep->AmountCol(3, 4, $classperiod * $convert, $dec);
					$rep->AmountCol(4, 5, $classclose * $convert, $dec);
					$rep->Font();
					$assetsopen += $classopen;
					$assetsperiod += $classperiod;
					$assetsclose += $classclose;
					$classopen = $classperiod = $classclose = 0.0;
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
			$group = $account['AccountTypeName'];
			if (get_sign_convert($account['account_type']))
				$convert = -1;
			else
				$convert = 1;
			$rep->row -= 4;
			$rep->TextCol(0, 5, $account['AccountTypeName']);
			$rep->row -= 4;
			$rep->Line($rep->row);
			$rep->NewLine();
		}
		$classname = $account['AccountClassName'];
		$totalopen += $prev_balance;
		$totalperiod += $curr_balance;
		$totalclose = $totalopen + $totalperiod;
		$classopen += $prev_balance;
		$classperiod += $curr_balance;
		$classclose = $classopen + $classperiod;
		$rep->TextCol(0, 1,	$account['account_code']);
		$rep->TextCol(1, 2,	$account['account_name']);

		$rep->AmountCol(2, 3, $prev_balance * $convert, $dec);
		$rep->AmountCol(3, 4, $curr_balance * $convert, $dec);
		$rep->AmountCol(4, 5, ($curr_balance + $prev_balance) * $convert, $dec);

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
			$rep->row += 6;
			$rep->Line($rep->row);
			$rep->NewLine();
			$rep->TextCol(0, 2,	_('Total') . " " . $group);
			$rep->AmountCol(2, 3, $totalopen * $convert, $dec);
			$rep->AmountCol(3, 4, $totalperiod * $convert, $dec);
			$rep->AmountCol(4, 5, $totalclose * $convert, $dec);
			if ($graphics)
			{
				$pg->x[] = $group;
				$pg->y[] = abs($totalclose);
			}
			$rep->NewLine();
			if ($closeclass)
			{
				$calculateopen = -$assetsopen - $classopen;
				$calculateperiod = -$assetsperiod - $classperiod;
				$calculateclose = -$assetsclose  - $classclose;
				$rep->row += 6;
				$rep->Line($rep->row);
				$rep->NewLine();
				$rep->TextCol(0, 2,	_('Calculated Return'));
				$rep->AmountCol(2, 3, $calculateopen * $convert, $dec);
				$rep->AmountCol(3, 4, $calculateperiod * $convert, $dec);
				$rep->AmountCol(4, 5, $calculateclose * $convert, $dec);
				if ($graphics)
				{
					$pg->x[] = _('Calculated Return');
					$pg->y[] = abs($calculateclose);
				}
				$rep->NewLine(2);
				$rep->Font('bold');
				$rep->TextCol(0, 2,	_('Total') . " " . $classname);
				$rep->AmountCol(2, 3, -$assetsopen * $convert, $dec);
				$rep->AmountCol(3, 4, -$assetsperiod * $convert, $dec);
				$rep->AmountCol(4, 5, -$assetsclose * $convert, $dec);
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
		$pg->graphic_1 = $to;
		$pg->type      = $graphics;
		$pg->skin      = $graph_skin;
		$pg->built_in  = false;
		$pg->fontfile  = $path_to_root . "/reporting/fonts/Vera.ttf";
		$pg->latin_notation = ($decseps[$_SESSION["wa_current_user"]->prefs->dec_sep()] != ".");
		$filename =  $comp_path.'/'.user_company()."/pdf_files/test.png";
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