<?php
/**********************************************************************
    [CUSTOM] Bank statement CSV parsing helpers (pure functions, no FA
    session/globals required — unit-testable standalone).
***********************************************************************/

/**
 * Normalize a bank-statement amount string to a float.
 * Handles: "1,234.56", "1 234,56", "R1,234.56", "-1 234.56", "(123.45)",
 * "1234.56-", unicode/nbsp spaces. Returns null when unparseable.
 */
function bank_import_amount($s)
{
	if (is_numeric($s))
		return (float)$s;

	$s = trim($s);
	if ($s === '')
		return null;

	$neg = false;
	if (substr($s, -1) === '-') {           // trailing minus sign
		$neg = true;
		$s = substr($s, 0, -1);
	}
	if (substr($s, 0, 1) === '(' && substr($s, -1) === ')') {  // parentheses
		$neg = true;
		$s = substr($s, 1, -1);
	}

	// strip currency symbols and unicode/nbsp spaces
	$s = str_replace(array("\xC2\xA0", "\xE2\x80\xAF", "\xE2\x80\x89", " "), '', $s);
	$s = preg_replace('/[^0-9.,\-+]/u', '', $s);

	if ($s === '')
		return null;

	$last_dot = strrpos($s, '.');
	$last_comma = strrpos($s, ',');
	if ($last_comma !== false && ($last_dot === false || $last_comma > $last_dot)) {
		// comma is decimal separator -> strip thousands dots, comma -> dot
		$s = str_replace('.', '', $s);
		$s = str_replace(',', '.', $s);
	} else {
		// dot is decimal separator -> strip thousands commas
		$s = str_replace(',', '', $s);
	}

	$s = preg_replace('/[^0-9.\-+]/', '', $s);
	if ($s === '' || $s === '-' || $s === '.' || !is_numeric($s))
		return null;

	$v = (float)$s;
	return $neg ? -$v : $v;
}

/**
 * Normalize a bank-statement date to an SQL date (yyyy-mm-dd).
 * Accepts dd/mm/yyyy, dd-mm-yyyy, dd.mm.yyyy, yyyy/mm/dd, yyyy-mm-dd, yyyymmdd.
 * Returns null when unparseable.
 */
function bank_import_date($s)
{
	$s = trim($s);
	if ($s === '')
		return null;

	if (preg_match('/^(\d{4})(\d{2})(\d{2})$/', $s, $m))
		return "$m[1]-$m[2]-$m[3]";

	$p = preg_split('/[\/.\-]/', $s);
	if (count($p) != 3)
		return null;

	$a = $p[0]; $b = $p[1]; $c = $p[2];
	if (strlen($a) == 4) { $y = $a; $m = $b; $d = $c; }
	elseif (strlen($c) == 4) { $d = $a; $m = $b; $y = $c; }
	else return null;

	$y = (int)$y; $m = (int)$m; $d = (int)$d;
	if ($y < 1900 || $y > 2200 || $m < 1 || $m > 12 || $d < 1 || $d > 31)
		return null;
	if (!checkdate($m, $d, $y))
		return null;
	return sprintf('%04d-%02d-%02d', $y, $m, $d);
}

/**
 * Auto-detect column roles from a header row.
 * Returns array(date, desc, debit, credit, amount, ref, balance) of int|null indices.
 */
function bank_import_detect_columns($header)
{
	$map = array('date'=>null, 'desc'=>null, 'debit'=>null, 'credit'=>null,
		'amount'=>null, 'ref'=>null, 'balance'=>null);
	foreach ($header as $i => $h) {
		$h = preg_replace('/[^a-z]/', '', strtolower(trim($h)));

		// DocuClipper FNB quirk: "DebitAmountCreditAmount" is concatenated into
		// ONE header cell, with debit values in this column and credit values in
		// the NEXT column (whose own header is empty). Map both explicitly.
		if ($map['debit'] === null && $map['credit'] === null
			&& strpos($h, 'debit') !== false && strpos($h, 'credit') !== false) {
			$map['debit'] = $i;
			$map['credit'] = $i + 1;
			continue;
		}

		if ($map['date'] === null && preg_match('/^(date|transactiondate|valuedate|posteddate|txndate|transaction)/', $h)) $map['date'] = $i;
		elseif ($map['desc'] === null && preg_match('/^(description|details|narrative|transactiondetails|memo|narration)/', $h)) $map['desc'] = $i;
		elseif ($map['debit'] === null && preg_match('/^(debit|debits|withdrawal|withdrawals|paidout|moneyout)/', $h)) $map['debit'] = $i;
		elseif ($map['credit'] === null && preg_match('/^(credit|credits|deposit|deposits|paidin|moneyin)/', $h)) $map['credit'] = $i;
		elseif ($map['amount'] === null && preg_match('/^(amount|value)/', $h)) $map['amount'] = $i;
		elseif ($map['balance'] === null && preg_match('/^(balance|runningbalance)/', $h)) $map['balance'] = $i;
		elseif ($map['ref'] === null && preg_match('/^(reference|ref|chequenumber|checkno)/', $h)) $map['ref'] = $i;
	}
	return $map;
}
