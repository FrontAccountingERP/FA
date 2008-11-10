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
function get_balance($account, $dimension, $dimension2, $from, $to, $from_incl=true, $to_incl=true) 
{
	$sql = "SELECT SUM(IF(amount >= 0, amount, 0)) as debit, SUM(IF(amount < 0, -amount, 0)) as credit, SUM(amount) as balance 
		FROM ".TB_PREF."gl_trans,".TB_PREF."chart_master,".TB_PREF."chart_types, ".TB_PREF."chart_class 
		WHERE ".TB_PREF."gl_trans.account=".TB_PREF."chart_master.account_code AND ".TB_PREF."chart_master.account_type=".TB_PREF."chart_types.id 
		AND ".TB_PREF."chart_types.class_id=".TB_PREF."chart_class.cid AND";
		
	if ($account != null)
		$sql .= " account='$account' AND";
	if ($dimension > 0)
		$sql .= " dimension_id=$dimension AND";
	if ($dimension2 > 0)
		$sql .= " dimension2_id=$dimension2 AND";
	$from_date = date2sql($from);
	if ($from_incl)
		$sql .= " tran_date >= '$from_date'  AND";
	else
		$sql .= " tran_date > IF(".TB_PREF."chart_class.balance_sheet=1, '0000-00-00', '$from_date') AND";
	$to_date = date2sql($to);
	if ($to_incl)
		$sql .= " tran_date <= '$to_date' ";
	else
		$sql .= " tran_date < '$to_date' ";

	$result = db_query($sql,"No general ledger accounts were returned");

	return db_fetch($result);
}

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
	$balances = $_POST['PARAM_3'];
	if ($dim == 2)
	{
		$dimension = $_POST['PARAM_4'];
		$dimension2 = $_POST['PARAM_5'];
		$comments = $_POST['PARAM_6'];
	}
	else if ($dim == 1)
	{
		$dimension = $_POST['PARAM_4'];
		$comments = $_POST['PARAM_5'];
	}
	else
	{
		$comments = $_POST['PARAM_4'];
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

	$accounts = get_gl_accounts();

	$pdeb = $pcre = $cdeb = $ccre = $tdeb = $tcre = $pbal = $cbal = $tbal = 0;
	$begin = begin_fiscalyear();
	if (date1_greater_date2($begin, $from))
		$begin = $from;
	$begin = add_days($begin, -1);
	while ($account=db_fetch($accounts))
	{
		$prev = get_balance($account["account_code"], $dimension, $dimension2, $begin, $from, false, false);
		$curr = get_balance($account["account_code"], $dimension, $dimension2, $from, $to, true, true);
		$tot = get_balance($account["account_code"], $dimension, $dimension2, $begin, $to, false, true);

		if ($zero == 0 && !$prev['balance'] && !$curr['balance'] && !$tot['balance'])
			continue;
		$rep->TextCol(0, 1, $account['account_code']);
		$rep->TextCol(1, 2,	$account['account_name']);
		if ($balances != 0)
		{
			if ($prev['balance'] >= 0.0)
				$rep->TextCol(2, 3,	number_format2($prev['balance'], $dec));
			else
				$rep->TextCol(3, 4,	number_format2(abs($prev['balance']), $dec));
			if ($curr['balance'] >= 0.0)
				$rep->TextCol(4, 5,	number_format2($curr['balance'], $dec));
			else
				$rep->TextCol(5, 6,	number_format2(abs($curr['balance']), $dec));
			if ($tot['balance'] >= 0.0)
				$rep->TextCol(6, 7,	number_format2($tot['balance'], $dec));
			else
				$rep->TextCol(7, 8,	number_format2(abs($tot['balance']), $dec));
		}
		else
		{
			$rep->TextCol(2, 3,	number_format2($prev['debit'], $dec));
			$rep->TextCol(3, 4,	number_format2($prev['credit'], $dec));
			$rep->TextCol(4, 5,	number_format2($curr['debit'], $dec));
			$rep->TextCol(5, 6,	number_format2($curr['credit'], $dec));
			$rep->TextCol(6, 7,	number_format2($tot['debit'], $dec));
			$rep->TextCol(7, 8,	number_format2($tot['credit'], $dec));
			$pdeb += $prev['debit'];
			$pcre += $prev['credit'];
			$cdeb += $curr['debit'];
			$ccre += $curr['credit'];
			$tdeb += $tot['debit'];
			$tcre += $tot['credit'];
			
		}	
		$pbal += $prev['balance'];
		$cbal += $curr['balance'];
		$tbal += $tot['balance'];
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

	//$prev = get_balance(null, $dimension, $dimension2, $begin, $from, false, false);
	//$curr = get_balance(null, $dimension, $dimension2, $from, $to, true, true);
	//$tot = get_balance(null, $dimension, $dimension2, $begin, $to, false, true);

	if ($balances == 0)
	{
		$rep->TextCol(0, 2, _("Total"));
		$rep->TextCol(2, 3,	number_format2($pdeb, $dec));
		$rep->TextCol(3, 4,	number_format2($pcre, $dec));
		$rep->TextCol(4, 5,	number_format2($cdeb, $dec));
		$rep->TextCol(5, 6,	number_format2($ccre, $dec));
		$rep->TextCol(6, 7,	number_format2($tdeb, $dec));
		$rep->TextCol(7, 8,	number_format2($tcre, $dec));
		$rep->NewLine();
	}	
	$rep->TextCol(0, 2, _("Ending Balance"));

	if ($prev['balance'] >= 0.0)
		$rep->TextCol(2, 3,	number_format2($pbal, $dec));
	else
		$rep->TextCol(3, 4,	number_format2(abs($pbal), $dec));
	if ($curr['balance'] >= 0.0)
		$rep->TextCol(4, 5,	number_format2($cbal, $dec));
	else
		$rep->TextCol(5, 6,	number_format2(abs($cbal), $dec));
	if ($tot['balance'] >= 0.0)
		$rep->TextCol(6, 7,	number_format2($tbal, $dec));
	else
		$rep->TextCol(7, 8,	number_format2(abs($tbal), $dec));
	
	$rep->Line($rep->row - 6);
	
	$rep->End();
}

?>