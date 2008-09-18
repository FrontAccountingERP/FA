<?php

$page_security = 2;
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	Bank Accounts Transactions
// ----------------------------------------------------------------
$path_to_root="../";

include_once($path_to_root . "includes/session.inc");
include_once($path_to_root . "includes/date_functions.inc");
include_once($path_to_root . "includes/data_checks.inc");
include_once($path_to_root . "gl/includes/gl_db.inc");

//----------------------------------------------------------------------------------------------------

// trial_inquiry_controls();
print_bank_transactions();

//----------------------------------------------------------------------------------------------------

function get_bank_balance_to($to, $account)
{
	$to = date2sql($to);
	$sql = "SELECT SUM(amount) FROM ".TB_PREF."bank_trans WHERE bank_act='$account'
	AND trans_date < '$to'";
	$result = db_query($sql, "The starting balance on hand could not be calculated");
	$row = db_fetch_row($result);
	return $row[0];
}

function get_bank_transactions($from, $to, $account)
{
	$from = date2sql($from);
	$to = date2sql($to);
	$sql = "SELECT ".TB_PREF."bank_trans.*,name AS BankTransType FROM ".TB_PREF."bank_trans, ".TB_PREF."bank_trans_types
		WHERE ".TB_PREF."bank_trans.bank_act = '$account'
		AND trans_date >= '$from'
		AND trans_date <= '$to'
		AND ".TB_PREF."bank_trans_types.id = ".TB_PREF."bank_trans.bank_trans_type_id
		ORDER BY trans_date,".TB_PREF."bank_trans.id";

	return db_query($sql,"The transactions for '$account' could not be retrieved");
}

function print_bank_transactions()
{
	global $path_to_root;

	include_once($path_to_root . "reporting/includes/pdf_report.inc");

	$rep = new FrontReport(_('Bank Statement'), "BankStatement.pdf", user_pagesize());

	$acc = $_POST['PARAM_0'];
	$from = $_POST['PARAM_1'];
	$to = $_POST['PARAM_2'];
	$comments = $_POST['PARAM_3'];

	$dec = user_price_dec();

	$cols = array(0, 90, 110, 170, 230, 300, 380, 430, 490, 550);

	$aligns = array('left',	'left',	'left',	'left',	'left',	'left',	'right', 'right', 'right');

	$headers = array(_('Type'),	_('#'),	_('Reference'), _('Date'), _('Type'), _('Person/Item'),
		_('Debit'),	_('Credit'), _('Balance'));

	$account = get_bank_account($acc);
	$act = $account['bank_account_name']." - ".$account['bank_curr_code']." - ".$account['bank_account_number'];
   	$params =   array( 	0 => $comments,
	    1 => array('text' => _('Period'), 'from' => $from, 'to' => $to),
	    2 => array('text' => _('Bank Account'),'from' => $act,'to' => ''));

	$rep->Font();
	$rep->Info($params, $cols, $headers, $aligns);
	$rep->Header();


	$prev_balance = get_bank_balance_to($from, $account["account_code"]);

	$trans = get_bank_transactions($from, $to, $account['account_code']);

	$rows = db_num_rows($trans);
	if ($prev_balance != 0.0 || $rows != 0)
	{
		$rep->Font('bold');
		$rep->TextCol(0, 3,	$act);
		$rep->TextCol(3, 5, _('Opening Balance'));
		if ($prev_balance > 0.0)
			$rep->TextCol(6, 7,	number_format2(abs($prev_balance), $dec));
		else
			$rep->TextCol(7, 8,	number_format2(abs($prev_balance), $dec));
		$rep->Font();
		$total = $prev_balance;
		$rep->NewLine(2);
		if ($rows > 0)
		{
			while ($myrow=db_fetch($trans))
			{
				$total += $myrow['amount'];

				$rep->TextCol(0, 1,	systypes::name($myrow["type"]));
				$rep->TextCol(1, 2,	$myrow['trans_no']);
				$rep->TextCol(2, 3,	$myrow['ref']);
				$rep->TextCol(3, 4,	sql2date($myrow["trans_date"]));
				$rep->TextCol(4, 5,	$myrow['BankTransType']);
				$rep->TextCol(5, 6,	payment_person_types::person_name($myrow["person_type_id"],$myrow["person_id"], false));
				if ($myrow['amount'] > 0.0)
					$rep->TextCol(6, 7,	number_format2(abs($myrow['amount']), $dec));
				else
					$rep->TextCol(7, 8,	number_format2(abs($myrow['amount']), $dec));
				$rep->TextCol(8, 9,	number_format2($total, $dec));
				$rep->NewLine();
				if ($rep->row < $rep->bottomMargin + $rep->lineHeight)
				{
					$rep->Line($rep->row - 2);
					$rep->Header();
				}
			}
			$rep->NewLine();
		}
		$rep->Font('bold');
		$rep->TextCol(3, 5,	_("Ending Balance"));
		if ($total > 0.0)
			$rep->TextCol(6, 7,	number_format2(abs($total), $dec));
		else
			$rep->TextCol(7, 8,	number_format2(abs($total), $dec));
		$rep->Font();
		$rep->Line($rep->row - $rep->lineHeight + 4);
		$rep->NewLine(2, 1);
	}
	$rep->End();
}

?>