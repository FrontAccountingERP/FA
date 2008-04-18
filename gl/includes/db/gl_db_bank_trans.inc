<?php

//----------------------------------------------------------------------------------

// add a bank transaction
// $amount is in $currency
// $date_ is display date (non-sql)

function add_bank_trans($type, $trans_no, $bank_act, $ref, $date_, $bank_trans_type_id,
	$amount, $person_type_id, $person_id, $currency="", $err_msg="")
{
	$sqlDate = date2sql($date_);

	// convert $amount to the bank's currency
	if ($currency != "")
	{
    	$bank_account_currency = get_bank_account_currency($bank_act);
    	$to_bank_currency = get_exchange_rate_from_to($currency, $bank_account_currency, $date_);
    	$amount_bank = ($amount / $to_bank_currency);
	}
	else
		$amount_bank = $amount;


	// Also store the rate to the home
	//$BankToHomeCurrencyRate = get_exchange_rate_to_home_currency($bank_account_currency, $date_);

	$sql = "INSERT INTO ".TB_PREF."bank_trans (type, trans_no, bank_act, ref,
		trans_date, bank_trans_type_id, amount, person_type_id, person_id) ";

	$sql .= "VALUES ($type, $trans_no, '$bank_act', ".db_escape($ref).", '$sqlDate', '$bank_trans_type_id',
		$amount_bank, $person_type_id, '$person_id')";

	if ($err_msg == "")
		$err_msg = "The bank transaction could not be inserted";

	db_query($sql, $err_msg);
}

//----------------------------------------------------------------------------------------

function exists_bank_trans($type, $type_no)
{
	$sql = "SELECT trans_no FROM ".TB_PREF."bank_trans WHERE type=$type
		AND trans_no=$type_no";
	$result = db_query($sql, "Cannot retreive a bank transaction");

    return (db_num_rows($result) > 0);
}

//----------------------------------------------------------------------------------------

function get_bank_trans($type, $trans_no=null, $person_type_id=null, $person_id=null)
{
	$sql = "SELECT *, bank_account_name, account_code, bank_curr_code, name AS BankTransType
		FROM ".TB_PREF."bank_trans, ".TB_PREF."bank_accounts, ".TB_PREF."bank_trans_types
		WHERE ".TB_PREF."bank_trans_types.id = ".TB_PREF."bank_trans.bank_trans_type_id
		AND ".TB_PREF."bank_accounts.account_code=".TB_PREF."bank_trans.bank_act ";
	if ($type != null)
		$sql .= " AND type=$type ";
	if ($trans_no != null)
		$sql .= " AND ".TB_PREF."bank_trans.trans_no = $trans_no ";
	if ($person_type_id != null)
		$sql .= " AND ".TB_PREF."bank_trans.person_type_id = $person_type_id ";
	if ($person_id != null)
		$sql .= " AND ".TB_PREF."bank_trans.person_id = '$person_id'";
	$sql .= " ORDER BY trans_date, ".TB_PREF."bank_trans.id";

	return db_query($sql, "query for bank transaction");
}

//----------------------------------------------------------------------------------------

function get_gl_trans_value($account, $type, $trans_no)
{
	$sql = "SELECT SUM(amount) FROM ".TB_PREF."gl_trans WHERE account='$account' AND type=$type AND type_no=$trans_no";

	$result = db_query($sql, "query for gl trans value");

	$row = db_fetch_row($result);
	return $row[0];
}

//----------------------------------------------------------------------------------------

function void_bank_trans($type, $type_no, $nested=false)
{
	if (!$nested)
		begin_transaction();

	$sql = "UPDATE ".TB_PREF."bank_trans SET amount=0
		WHERE type=$type AND trans_no=$type_no";

	$result = db_query($sql, "could not void bank transactions for type=$type and trans_no=$type_no");

	void_gl_trans($type, $type_no, true);

	// in case it's a customer trans - probably better to check first
	void_cust_allocations($type, $type_no);
	void_customer_trans($type, $type_no);

	// in case it's a supplier trans - probably better to check first
	void_supp_allocations($type, $type_no);
	void_supp_trans($type, $type_no);

	if (!$nested)
		commit_transaction();
}

//----------------------------------------------------------------------------------

?>