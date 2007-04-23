<?php

$page_security = 2;
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	Payment Report
// ----------------------------------------------------------------
$path_to_root="../";

include_once($path_to_root . "includes/session.inc");
include_once($path_to_root . "includes/date_functions.inc");
include_once($path_to_root . "includes/data_checks.inc");
include_once($path_to_root . "gl/includes/gl_db.inc");

//----------------------------------------------------------------------------------------------------

// trial_inquiry_controls();
print_payment_report();

function getTransactions($supplier, $date)
{
	$date = date2sql($date);
	
	$sql = "SELECT ".TB_PREF."sys_types.type_name,
			".TB_PREF."supp_trans.supp_reference,
			".TB_PREF."supp_trans.due_date,
			".TB_PREF."supp_trans.trans_no,
			".TB_PREF."supp_trans.type,
			(".TB_PREF."supp_trans.ov_amount + ".TB_PREF."supp_trans.ov_gst - ".TB_PREF."supp_trans.alloc) AS Balance,
			(".TB_PREF."supp_trans.ov_amount + ".TB_PREF."supp_trans.ov_gst ) AS TranTotal
		FROM ".TB_PREF."supp_trans,
			".TB_PREF."sys_types
		WHERE ".TB_PREF."sys_types.type_id = ".TB_PREF."supp_trans.type
		AND ".TB_PREF."supp_trans.supplier_id = '" . $supplier . "'
		AND ".TB_PREF."supp_trans.ov_amount + ".TB_PREF."supp_trans.ov_gst - ".TB_PREF."supp_trans.alloc != 0
		AND ".TB_PREF."supp_trans.due_date <='" . $date . "'
		ORDER BY ".TB_PREF."supp_trans.type,
			".TB_PREF."supp_trans.trans_no";

    return db_query($sql, "No transactions were returned");
}

//----------------------------------------------------------------------------------------------------

function print_payment_report()
{
    global $path_to_root;

    include_once($path_to_root . "reporting/includes/pdf_report.inc");

    $to = $_POST['PARAM_0'];
    $fromsupp = $_POST['PARAM_1'];
    $currency = $_POST['PARAM_2'];
    $comments = $_POST['PARAM_3'];

	if ($fromsupp == reserved_words::get_all_numeric())
		$from = _('All');
	else
		$from = get_supplier_name($fromsupp);
	    
    $dec = user_price_dec();

	if ($currency == reserved_words::get_all())
	{
		$convert = true;
		$currency = _('Balances in Home Currency');
	}
	else
		$convert = false;

	$cols = array(0, 100, 130, 190,	250, 320, 385, 450,	515);

	$headers = array(_('Trans Type'), _('#'), _('Due Date'), '', '',
		'', _('Total'), _('Balance'));

	$aligns = array('left',	'left',	'left',	'left',	'right', 'right', 'right', 'right');

    $params =   array( 	0 => $comments,
    				    1 => array('text' => _('End Date'), 'from' => $to, 'to' => ''),
    				    2 => array('text' => _('Supplier'), 'from' => $from, 'to' => ''),
    				    3 => array(  'text' => _('Currency'),'from' => $currency, 'to' => ''));

    $rep = new FrontReport(_('Payment Report'), "PaymentReport.pdf", user_pagesize());

    $rep->Font();
    $rep->Info($params, $cols, $headers, $aligns);
    $rep->Header();

	$total = array();
	$grandtotal = array();

	$sql = "SELECT supplier_id, supp_name AS name, curr_code, ".TB_PREF."payment_terms.terms FROM ".TB_PREF."suppliers, ".TB_PREF."payment_terms
		WHERE ";
	if ($fromsupp != reserved_words::get_all_numeric())
		$sql .= "supplier_id=$fromsupp AND ";
	$sql .= "".TB_PREF."suppliers.payment_terms = ".TB_PREF."payment_terms.terms_indicator 
		ORDER BY supp_name";
	$result = db_query($sql, "The customers could not be retrieved");
	
	while ($myrow=db_fetch($result)) 
	{
		if (!$convert && $currency != $myrow['curr_code'])
			continue;
		$rep->fontSize += 2;
		$rep->TextCol(0, 6, $myrow['name'] . " - " . $myrow['terms']);
		if ($convert)
		{
			$rate = get_exchange_rate_from_home_currency($myrow['curr_code'], $to);
			$rep->TextCol(6, 7,	$myrow['curr_code']);
		}
		else
			$rate = 1.0;
		$rep->fontSize -= 2;
		$rep->NewLine(1, 2);
		$res = getTransactions($myrow['supplier_id'], $to);
		if (db_num_rows($res)==0)
			continue;
		$rep->Line($rep->row + 4);
		$total[0] = $total[1] = $total[2] = $total[3] = 0.0; 
		while ($trans=db_fetch($res))
		{
			$rep->NewLine(1, 2);
			$rep->TextCol(0, 1,	$trans['type_name']);
			$rep->TextCol(1, 2,	$trans['supp_reference']);
			$rep->TextCol(2, 3,	sql2date($trans['due_date']));
			$item[0] = Abs($trans['TranTotal']) * $rate; 		
			$rep->TextCol(6, 7,	number_format2($item[0], $dec));
			$item[1] = $trans['Balance'] * $rate;
			$rep->TextCol(7, 8,	number_format2($item[1], $dec));
			for ($i = 0; $i < 2; $i++)
			{
				$total[$i] += $item[$i];
				$grandtotal[$i] += $item[$i];
			}	
		}
		$rep->Line($rep->row - 8);
		$rep->NewLine(2);
		$rep->TextCol(0, 3,	_('Total'));
		for ($i = 0; $i < 2; $i++)
		{
			$rep->TextCol($i + 6, $i + 7, number_format2($total[$i], $dec));
			$total[$i] = 0.0;
		}	
    	$rep->Line($rep->row  - 4);
    	$rep->NewLine(2);
	}
	$rep->fontSize += 2;
	$rep->TextCol(0, 3,	_('Grand Total'));
	$rep->fontSize -= 2;
	for ($i = 0; $i < 2; $i++)
		$rep->TextCol($i + 6, $i + 7,number_format2($grandtotal[$i], $dec));
	$rep->Line($rep->row  - 4);
    $rep->End();
}

?>