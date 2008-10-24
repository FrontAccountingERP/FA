<?php

$page_security = 2;
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	Trial Balance
// ----------------------------------------------------------------
$path_to_root="../";

include_once($path_to_root . "includes/session.inc");
include_once($path_to_root . "includes/date_functions.inc");
include_once($path_to_root . "includes/data_checks.inc");
include_once($path_to_root . "gl/includes/gl_db.inc");

//----------------------------------------------------------------------------------------------------

// trial_inquiry_controls();
print_trial_balance();

//----------------------------------------------------------------------------------------------------

function print_trial_balance()
{
	global $path_to_root;

	include_once($path_to_root . "reporting/includes/pdf_report.inc");
	$dim = get_company_pref('use_dimension');
	$dimension = $dimension2 = 0;

	$from = $_POST['PARAM_0'];
	$to = $_POST['PARAM_1'];
	$zero = $_POST['PARAM_2'];
	if ($dim == 2)
	{
		$dimension = $_POST['PARAM_3'];
		$dimension2 = $_POST['PARAM_4'];
		$comments = $_POST['PARAM_5'];
	}
	else if ($dim == 1)
	{
		$dimension = $_POST['PARAM_3'];
		$comments = $_POST['PARAM_4'];
	}
	else
	{
		$comments = $_POST['PARAM_3'];
	}
	$dec = user_price_dec();

	$cols2 = array(0, 50, 230, 330, 430, 530);
	//-------------0--1---2----3----4----5--

	$headers2 = array('', '', _('Brought Forward'),	_('This Period'), _('Balance'));

	$aligns2 = array('left', 'left', 'left', 'left', 'left');

	$cols = array(0, 50, 200, 250, 300,	350, 400, 450, 500,	550);
	//------------0--1---2----3----4----5----6----7----8----9--

	$headers = array(_('Account'), _('Account Name'), _('Debit'), _('Credit'), _('Debit'),
		_('Credit'), _('Debit'), _('Credit'));

	$aligns = array('left',	'left',	'right', 'right', 'right', 'right',	'right', 'right');

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

	$rep = new FrontReport(_('Trial Balance'), "TrialBalance.pdf", user_pagesize());

	$rep->Font();
	$rep->Info($params, $cols, $headers, $aligns, $cols2, $headers2, $aligns2);
	$rep->Header();
	$totprevd = $totprevc = $totcurrd = $totcurrc = 0.0;

	$accounts = get_gl_accounts();

	while ($account=db_fetch($accounts))
	{

		if (is_account_balancesheet($account["account_code"]))
			$begin = "";
		else
		{
			$begin = begin_fiscalyear();
			if (date1_greater_date2($begin, $from))
				$begin = $from;
			$begin = add_days($begin, -1);
		}

		$prev_balance = get_gl_balance_from_to($begin, $from, $account["account_code"], $dimension, $dimension2);

		$curr_balance = get_gl_trans_from_to($from, $to, $account["account_code"], $dimension, $dimension2);

		if ($zero == 0 && !$prev_balance && !$curr_balance)
			continue;
		$rep->TextCol(0, 1, $account['account_code']);
		$rep->TextCol(1, 2,	$account['account_name']);

		if ($prev_balance >= 0.0)
		{
			$totprevd += $prev_balance;
			$rep->TextCol(2, 3,	number_format2(abs($prev_balance), $dec));
		}	
		else
		{
			$totprevc += $prev_balance;
			$rep->TextCol(3, 4,	number_format2(abs($prev_balance), $dec));
		}	
		if ($curr_balance >= 0.0)
		{
			$totcurrd += $curr_balance;
			$rep->TextCol(4, 5,	number_format2(abs($curr_balance), $dec));
		}	
		else
		{
			$totcurrc += $curr_balance;
			$rep->TextCol(5, 6,	number_format2(abs($curr_balance), $dec));
		}	
		if ($curr_balance + $prev_balance >= 0.0)
			$rep->TextCol(6, 7,	number_format2(abs($curr_balance + $prev_balance), $dec));
		else
			$rep->TextCol(7, 8,	number_format2(abs($curr_balance + $prev_balance), $dec));

		$rep->NewLine();

		if ($rep->row < $rep->bottomMargin + $rep->lineHeight)
		{
			$rep->Line($rep->row - 2);
			$rep->Header();
		}
	}
	$rep->Line($rep->row);
	$rep->NewLine();
	$rep->Font('bold');
	$rep->TextCol(0, 2, _("Total"));

	$rep->TextCol(2, 3,	number_format2(abs($totprevd), $dec));
	$rep->TextCol(3, 4,	number_format2(abs($totprevc), $dec));
	$rep->TextCol(4, 5,	number_format2(abs($totcurrd), $dec));
	$rep->TextCol(5, 6,	number_format2(abs($totcurrc), $dec));
	$rep->TextCol(6, 7,	number_format2(abs($totcurrd + $totprevd), $dec));
	$rep->TextCol(7, 8,	number_format2(abs($totcurrc + $totprevc), $dec));
	$rep->NewLine();
	$totprev = $totprevd + $totprevc;
	$totcurr = $totcurrd + $totcurrc;
	$rep->TextCol(0, 2, _("Ending Balance"));

	if ($totprev >= 0.0)
		$rep->TextCol(2, 3,	number_format2(abs($totprev), $dec));
	else
		$rep->TextCol(3, 4,	number_format2(abs($totprev), $dec));
	if ($totcurr >= 0.0)
		$rep->TextCol(4, 5,	number_format2(abs($totcurr), $dec));
	else
		$rep->TextCol(5, 6,	number_format2(abs($totcurr), $dec));
	if ($totcurr + $totprev >= 0.0)
		$rep->TextCol(6, 7,	number_format2(abs($totcurr + $totprev), $dec));
	else
		$rep->TextCol(7, 8,	number_format2(abs($totcurr + $totprev), $dec));
	
	$rep->Line($rep->row - 6);
	
	$rep->End();
}

?>