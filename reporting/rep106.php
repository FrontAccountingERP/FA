<?php

$page_security = 2;
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	Order Status List
// ----------------------------------------------------------------
$path_to_root="../";

include_once($path_to_root . "includes/session.inc");
include_once($path_to_root . "includes/date_functions.inc");
include_once($path_to_root . "includes/data_checks.inc");
include_once($path_to_root . "sales/includes/sales_db.inc");
include_once($path_to_root . "inventory/includes/db/items_category_db.inc");

//----------------------------------------------------------------------------------------------------

// trial_inquiry_controls();
print_salesman_list();

//----------------------------------------------------------------------------------------------------

function GetSalesmanTrans($from, $to)
{
	$fromdate = date2sql($from);
	$todate = date2sql($to);

	$sql = "SELECT DISTINCT ".TB_PREF."debtor_trans.*,
		ov_amount+ov_discount AS InvoiceTotal,
		".TB_PREF."debtors_master.name AS DebtorName, ".TB_PREF."debtors_master.curr_code, ".TB_PREF."cust_branch.br_name,
		".TB_PREF."cust_branch.contact_name, ".TB_PREF."salesman.*
		FROM ".TB_PREF."debtor_trans, ".TB_PREF."debtors_master, ".TB_PREF."sales_orders, ".TB_PREF."cust_branch,
			".TB_PREF."salesman
		WHERE ".TB_PREF."sales_orders.order_no=".TB_PREF."debtor_trans.order_
		    AND ".TB_PREF."sales_orders.branch_code=".TB_PREF."cust_branch.branch_code
		    AND ".TB_PREF."cust_branch.salesman=".TB_PREF."salesman.salesman_code
		    AND ".TB_PREF."debtor_trans.debtor_no=".TB_PREF."debtors_master.debtor_no
		    AND (".TB_PREF."debtor_trans.type=10 OR ".TB_PREF."debtor_trans.type=11)
		    AND ".TB_PREF."debtor_trans.tran_date>='$fromdate'
		    AND ".TB_PREF."debtor_trans.tran_date<='$todate'
		ORDER BY ".TB_PREF."salesman.salesman_code, ".TB_PREF."debtor_trans.tran_date";

	return db_query($sql, "Error getting order details");
}

//----------------------------------------------------------------------------------------------------

function print_salesman_list()
{
	global $path_to_root;

	include_once($path_to_root . "reporting/includes/pdf_report.inc");

	$from = $_POST['PARAM_0'];
	$to = $_POST['PARAM_1'];
	$summary = $_POST['PARAM_2'];
	$comments = $_POST['PARAM_3'];

	if ($summary == 0)
		$sum = _("No");
	else
		$sum = _("Yes");

	$dec = user_qty_dec();

	$cols = array(0, 60, 150, 220, 325,	385, 450, 515);

	$headers = array(_('Invoice'), _('Customer'), _('Branch'), _('Customer Ref'),
		_('Inv Date'),	_('Total'),	_('Provision'));

	$aligns = array('left',	'left',	'left', 'left', 'left', 'right',	'right');

	$headers2 = array(_('Salesman'), " ",	_('Phone'), _('Email'),	_('Provision'),
		_('Break Pt.'), _('Provision')." 2");

    $params =   array( 	0 => $comments,
	    				1 => array(  'text' => _('Period'), 'from' => $from, 'to' => $to),
	    				2 => array(  'text' => _('Summary Only'),'from' => $sum,'to' => ''));

	$cols2 = $cols;
	$aligns2 = $aligns;

	$rep = new FrontReport(_('Salesman Listing'), "SalesmanListing.pdf", user_pagesize());
	$rep->Font();
	$rep->Info($params, $cols, $headers, $aligns, $cols2, $headers2, $aligns2);

	$rep->Header();
	$salesman = 0;
	$subtotal = $total = $subprov = $provtotal = 0;

	$result = GetSalesmanTrans($from, $to);

	while ($myrow=db_fetch($result))
	{
		if ($rep->row < $rep->bottomMargin + (2 * $rep->lineHeight))
		{
			$salesman = 0;
			$rep->Header();
		}
		$rep->NewLine(0, 2, false, $salesman);
		if ($salesman != $myrow['salesman_code'])
		{
			if ($salesman != 0)
			{
				$rep->Line($rep->row - 8);
				$rep->NewLine(2);
				$rep->TextCol(0, 3, _('Total'));
				$rep->TextCol(5, 6, number_format2($subtotal, $dec));
				$rep->TextCol(6, 7, number_format2($subprov, $dec));
    			$rep->Line($rep->row  - 4);
    			$rep->NewLine(2);
				$rep->Line($rep->row);
			}
			$rep->TextCol(0, 2,	$myrow['salesman_code']." ".$myrow['salesman_name']);
			$rep->TextCol(2, 3,	$myrow['salesman_phone']);
			$rep->TextCol(3, 4,	$myrow['salesman_email']);
			$rep->TextCol(4, 5,	number_format2($myrow['provision'], user_percent_dec()) ." %");
			$rep->TextCol(5, 6,	number_format2($myrow['break_pt'], $dec));
			$rep->TextCol(6, 7,	number_format2($myrow['provision2'], user_percent_dec()) ." %");
			$rep->NewLine(2);
			$salesman = $myrow['salesman_code'];
			$total += $subtotal;
			$provtotal += $subprov;
			$subtotal = 0;
			$subprov = 0;
		}
		$date = sql2date($myrow['tran_date']);
		$rate = get_exchange_rate_from_home_currency($myrow['curr_code'], $date);
		$amt = $myrow['InvoiceTotal'] * $rate;
		if ($subprov > $myrow['break_pt'] && $myrow['provision2'] != 0)
			$prov = $myrow['provision2'] * $amt / 100;
		else
			$prov = $myrow['provision'] * $amt / 100;
		if (!$summary)
		{
			$rep->TextCol(0, 1,	$myrow['trans_no']);
			$rep->TextCol(1, 2,	$myrow['DebtorName']);
			$rep->TextCol(2, 3,	$myrow['br_name']);
			$rep->TextCol(3, 4,	$myrow['contact_name']);
			$rep->TextCol(4, 5,	$date);
			$rep->TextCol(5, 6,	number_format2($amt, $dec));
			$rep->TextCol(6, 7,	number_format2($prov, $dec));
			$rep->NewLine();
			if ($rep->row < $rep->bottomMargin + (2 * $rep->lineHeight))
			{
				$salesman = 0;
				$rep->Header();
			}
		}
		$subtotal += $amt;
		$subprov += $prov;
	}
	if ($salesman != 0)
	{
		$rep->Line($rep->row - 4);
		$rep->NewLine(2);
		$rep->TextCol(0, 3, _('Total'));
		$rep->TextCol(5, 6, number_format2($subtotal, $dec));
		$rep->TextCol(6, 7, number_format2($subprov, $dec));
		$rep->Line($rep->row  - 4);
		$rep->NewLine(2);
		//$rep->Line($rep->row);
		$total += $subtotal;
		$provtotal += $subprov;
	}
	$rep->fontSize += 2;
	$rep->TextCol(0, 3, _('Grand Total'));
	$rep->fontSize -= 2;
	$rep->TextCol(5, 6, number_format2($total, $dec));
	$rep->TextCol(6, 7, number_format2($provtotal, $dec));
	$rep->Line($rep->row  - 4);
	$rep->End();
}

?>