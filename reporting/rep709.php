<?php

$page_security = 2;
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	Tax Report
// ----------------------------------------------------------------
$path_to_root="../";

include_once($path_to_root . "includes/session.inc");
include_once($path_to_root . "includes/date_functions.inc");
include_once($path_to_root . "includes/data_checks.inc");
include_once($path_to_root . "gl/includes/gl_db.inc");

//----------------------------------------------------------------------------------------------------

// trial_inquiry_controls();
print_tax_report();

function getCustTransactions($from, $to)
{
	$fromdate = date2sql($from);
	$todate = date2sql($to);
	
	$sql = "SELECT ".TB_PREF."debtor_trans.reference,
			".TB_PREF."debtor_trans.type,
			".TB_PREF."sys_types.type_name,
			".TB_PREF."debtor_trans.tran_date,
			".TB_PREF."debtor_trans.debtor_no,
			".TB_PREF."debtors_master.name,
			".TB_PREF."debtors_master.curr_code,
			".TB_PREF."debtor_trans.branch_code,
			".TB_PREF."debtor_trans.order_,
			(ov_amount+ov_freight)*rate AS NetAmount,
			ov_freight*rate AS FreightAmount,
			ov_gst*rate AS Tax
		FROM ".TB_PREF."debtor_trans
		INNER JOIN ".TB_PREF."debtors_master ON ".TB_PREF."debtor_trans.debtor_no=".TB_PREF."debtors_master.debtor_no
		INNER JOIN ".TB_PREF."sys_types ON ".TB_PREF."debtor_trans.type=".TB_PREF."sys_types.type_id
		WHERE ".TB_PREF."debtor_trans.tran_date >= '$fromdate'
			AND ".TB_PREF."debtor_trans.tran_date <= '$todate'
			AND (".TB_PREF."debtor_trans.type=10 OR ".TB_PREF."debtor_trans.type=11)
		ORDER BY ".TB_PREF."debtor_trans.tran_date";

    return db_query($sql,"No transactions were returned");
}

function getSuppTransactions($from, $to)
{
	$fromdate = date2sql($from);
	$todate = date2sql($to);
	
	$sql = "SELECT ".TB_PREF."supp_trans.supp_reference,
			".TB_PREF."supp_trans.type,
			".TB_PREF."sys_types.type_name,
			".TB_PREF."supp_trans.tran_date,
			".TB_PREF."supp_trans.supplier_id,
			".TB_PREF."supp_trans.rate,
			".TB_PREF."suppliers.supp_name,
			".TB_PREF."suppliers.curr_code,
			".TB_PREF."supp_trans.rate,
			ov_amount*rate AS NetAmount,
			ov_gst*rate AS Tax
		FROM ".TB_PREF."supp_trans
		INNER JOIN ".TB_PREF."suppliers ON ".TB_PREF."supp_trans.supplier_id=".TB_PREF."suppliers.supplier_id
		INNER JOIN ".TB_PREF."sys_types ON ".TB_PREF."supp_trans.type=".TB_PREF."sys_types.type_id
		WHERE ".TB_PREF."supp_trans.tran_date >= '$fromdate'
			AND ".TB_PREF."supp_trans.tran_date <= '$todate'
			AND (".TB_PREF."supp_trans.type=20 OR ".TB_PREF."supp_trans.type=21)
		ORDER BY ".TB_PREF."supp_trans.tran_date";

    return db_query($sql,"No transactions were returned");
}

function getTaxTypes()
{
	$sql = "SELECT id FROM ".TB_PREF."tax_types ORDER BY id";
    return db_query($sql,"No transactions were returned");
}

function getTaxInfo($id)
{
	$sql = "SELECT * FROM ".TB_PREF."tax_types WHERE id=$id";
    $result = db_query($sql,"No transactions were returned");
    return db_fetch($result);
}

function getCustInvTax($taxtype, $from, $to)
{
	$fromdate = date2sql($from);
	$todate = date2sql($to);
	
	$sql = "SELECT SUM(unit_price * quantity*".TB_PREF."debtor_trans.rate), SUM(amount*".TB_PREF."debtor_trans.rate)
		FROM ".TB_PREF."debtor_trans_details, ".TB_PREF."debtor_trans_tax_details, ".TB_PREF."debtor_trans
				WHERE ".TB_PREF."debtor_trans_details.debtor_trans_type>=10
					AND ".TB_PREF."debtor_trans_details.debtor_trans_type<=11
					AND ".TB_PREF."debtor_trans_details.debtor_trans_no=".TB_PREF."debtor_trans.trans_no
					AND ".TB_PREF."debtor_trans_details.debtor_trans_type=".TB_PREF."debtor_trans.type
					AND ".TB_PREF."debtor_trans_details.debtor_trans_no=".TB_PREF."debtor_trans_tax_details.debtor_trans_no
					AND ".TB_PREF."debtor_trans_details.debtor_trans_type=".TB_PREF."debtor_trans_tax_details.debtor_trans_type
					AND ".TB_PREF."debtor_trans_tax_details.tax_type_id=$taxtype
					AND ".TB_PREF."debtor_trans.tran_date >= '$fromdate'
					AND ".TB_PREF."debtor_trans.tran_date <= '$todate'";

    $result = db_query($sql,"No transactions were returned");
    return db_fetch_row($result);
}

function getSuppInvTax($taxtype, $from, $to)
{
	$fromdate = date2sql($from);
	$todate = date2sql($to);
	
	$sql = "SELECT SUM(unit_price * quantity * ".TB_PREF."supp_trans.rate), SUM(amount*".TB_PREF."supp_trans.rate)  
		FROM ".TB_PREF."supp_invoice_items, ".TB_PREF."supp_invoice_tax_items, ".TB_PREF."supp_trans
				WHERE ".TB_PREF."supp_invoice_items.supp_trans_type>=20
					AND ".TB_PREF."supp_invoice_items.supp_trans_type<=21
					AND ".TB_PREF."supp_invoice_items.supp_trans_no=".TB_PREF."supp_invoice_tax_items.supp_trans_no
					AND ".TB_PREF."supp_invoice_items.supp_trans_type=".TB_PREF."supp_invoice_tax_items.supp_trans_type
					AND ".TB_PREF."supp_invoice_items.supp_trans_no=".TB_PREF."supp_trans.trans_no
					AND ".TB_PREF."supp_invoice_items.supp_trans_type=".TB_PREF."supp_trans.type
					AND ".TB_PREF."supp_invoice_tax_items.tax_type_id=$taxtype
					AND ".TB_PREF."supp_trans.tran_date >= '$fromdate'
					AND ".TB_PREF."supp_trans.tran_date <= '$todate'";

    $result = db_query($sql,"No transactions were returned");
    return db_fetch_row($result);
}

//----------------------------------------------------------------------------------------------------

function print_tax_report()
{
	global $path_to_root;

	include_once($path_to_root . "reporting/includes/pdf_report.inc");

	$rep = new FrontReport(_('Tax Report'), "TaxReport.pdf", user_pagesize());
	
	$from = $_POST['PARAM_0'];
	$to = $_POST['PARAM_1'];
	$summaryOnly = $_POST['PARAM_2'];
	$comments = $_POST['PARAM_3'];
	$dec = user_price_dec();
	
	if ($summaryOnly == 1)
		$summary = _('Summary Only');
	else
		$summary = _('Detailed Report');
	
	
	$res = getTaxTypes();

	$taxes = array();
	$i = 0;
	while ($tax=db_fetch($res))
		$taxes[$i++] = $tax['id'];
	$idcounter = count($taxes);
	
	$totalinvout = array();
	$totaltaxout = array();
	$totalinvin = array();
	$totaltaxin = array();
	
	if (!$summaryOnly)
	{
		$cols = array(0, 80, 130, 190, 290, 370, 435, 500, 565);
		
		$headers = array(_('Trans Type'), _('#'), _('Date'), _('Name'),	_('Branch Name'),
			_('Net'), _('Tax'));
		
		$aligns = array('left', 'left', 'left', 'left', 'left', 'right', 'right');
		
		$params =   array( 	0 => $comments,
							1 => array('text' => _('Period'), 'from' => $from, 'to' => $to),
							2 => array('text' => _('Type'), 'from' => $summary, 'to' => ''));

		$rep->Font();
		$rep->Info($params, $cols, $headers, $aligns);
		$rep->Header();
	}
	$totalnet = 0.0;
	$totaltax = 0.0;

	$transactions = getCustTransactions($from, $to);

	while ($trans=db_fetch($transactions))
	{
		if (!$summaryOnly)
		{
			$rep->TextCol(0, 1,	$trans['type_name']);
			$rep->TextCol(1, 2,	$trans['reference']);
			$rep->TextCol(2, 3,	sql2date($trans['tran_date']));
			$rep->TextCol(3, 4,	$trans['name']);
			if ($trans["branch_code"] > 0)
				$rep->TextCol(4, 5,	get_branch_name($trans["branch_code"]));

			$rep->TextCol(5, 6,	number_format2($trans['NetAmount'], $dec));
			$rep->TextCol(6, 7,	number_format2($trans['Tax'], $dec));

			$rep->NewLine();
						
			if ($rep->row < $rep->bottomMargin + $rep->lineHeight)
			{
				$rep->Line($rep->row - 2);
				$rep->Header();
			}
		}
		$totalnet += $trans['NetAmount'];
		$totaltax += $trans['Tax'];

	}
	if (!$summaryOnly)
	{
		$rep->NewLine();

		if ($rep->row < $rep->bottomMargin + $rep->lineHeight)
		{
			$rep->Line($rep->row - 2);
			$rep->Header();
		}
		$rep->Line($rep->row + $rep->lineHeight);
		$rep->TextCol(3, 5,	_('Total Outputs'));
		$rep->TextCol(5, 6,	number_format2($totalnet, $dec));
		$rep->TextCol(6, 7,	number_format2($totaltax, $dec));
		$rep->Line($rep->row - 5);
		$rep->Header();
	}
	$totalinnet = 0.0;
	$totalintax = 0.0;
	
	$transactions = getSuppTransactions($from, $to);

	while ($trans=db_fetch($transactions))
	{
		if (!$summaryOnly)
		{
			$rep->TextCol(0, 1,	$trans['type_name']);
			$rep->TextCol(1, 2,	$trans['supp_reference']);
			$rep->TextCol(2, 3,	sql2date($trans['tran_date']));
			$rep->TextCol(3, 5,	$trans['supp_name']);
			$rep->TextCol(5, 6,	number_format2($trans['NetAmount'], $dec));
			$rep->TextCol(6, 7,	number_format2($trans['Tax'], $dec));

			$rep->NewLine();
			if ($rep->row < $rep->bottomMargin + $rep->lineHeight)
			{
				$rep->Line($rep->row - 2);
				$rep->Header();
			}
		}
		$totalinnet += $trans['NetAmount'];
		$totalintax += $trans['Tax'];

	}
	if (!$summaryOnly)
	{
		$rep->NewLine();

		if ($rep->row < $rep->bottomMargin + $rep->lineHeight)
		{
			$rep->Line($rep->row - 2);
			$rep->Header();
		}
		$rep->Line($rep->row + $rep->lineHeight);
		$rep->TextCol(3, 5,	_('Total Inputs'));
		$rep->TextCol(5, 6,	number_format2($totalinnet, $dec));
		$rep->TextCol(6, 7,	number_format2($totalintax, $dec));
		$rep->Line($rep->row - 5);
	}
	$cols2 = array(0, 100, 200,	300, 400, 500, 600);
	
	$headers2 = array(_('Tax Rate'), _('Outputs'), _('Output Tax'),	_('Inputs'), _('Input Tax'));
	
	$aligns2 = array('left', 'right', 'right', 'right',	'right');
	
	$invamount = 0.0;							
	for ($i = 0; $i < $idcounter; $i++)
	{
		$amt = getCustInvTax($taxes[$i], $from, $to);
		$totalinvout[$i] += $amt[0];
		$totaltaxout[$i] += $amt[1];
		$invamount += $amt[0];
	}
	if ($totalnet != $invamount)
		$totalinvout[$idcounter] = ($invamount - $totalnet);
	for ($i = 0; $i < $idcounter; $i++)
	{
		$amt = getSuppInvTax($taxes[$i], $from, $to);
		$totalinvin[$i] += $amt[0];
		$totaltaxin[$i] += $amt[1];
		$invamount += $amt[0];
	}
	if ($totalinnet != $invamount)
		$totalinvin[$idcounter] = ($totalinnet - $invamount);

	for ($i = 0; $i < count($cols2); $i++)
	{
		$rep->cols[$i] = $rep->leftMargin + $cols2[$i];
		$rep->headers[$i] = $headers2[$i];
		$rep->aligns[$i] = $aligns2[$i];
	}
	$rep->Header();
	$counter = count($totalinvout);
	$counter = max($counter, $idcounter);
	$trow = $rep->row;
	$i = 0;
	for ($j = 0; $j < $counter; $j++)
	{
		if (isset($taxes[$j]) && $taxes[$j] > 0)
		{
			$tx = getTaxInfo($taxes[$j]);
			$str = $tx['name'] . " " . number_format2($tx['rate'], $dec) . "%";
		}
		else
			$str = _('No tax specified');
		$rep->TextCol($i, $i + 1, $str);
		$rep->NewLine();
	}
	$i++;
	$rep->row = $trow;
	for ($j = 0; $j < $counter; $j++)
	{
		$rep->TextCol($i, $i + 1, number_format2($totalinvout[$j], $dec));
		$rep->NewLine();
	}
	$i++;
	$rep->row = $trow;
	for ($j = 0; $j < $counter; $j++)
	{
		$rep->TextCol($i, $i + 1,number_format2($totaltaxout[$j], $dec));
		$rep->NewLine();
	}
	$i++;
	$rep->row = $trow;
	for ($j = 0; $j < $counter; $j++)
	{
		$rep->TextCol($i, $i + 1, number_format2($totalinvin[$j], $dec));
		$rep->NewLine();
	}
	$i++;
	$rep->row = $trow;
	for ($j = 0; $j < $counter; $j++)
	{
		$rep->TextCol($i, $i + 1, number_format2($totaltaxin[$j], $dec));
		$rep->NewLine();
	}
	$rep->Line($rep->row - 4);
	
	$locale = $path_to_root . "lang/" . $_SESSION['language']->code . "/locale.inc";
	if (file_exists($locale))
	{
		$taxinclude = true;
		include($locale);
		/*
		if (function_exists("TaxFunction"))
			TaxFunction();
		*/	
	}
	$rep->End();
}

?>