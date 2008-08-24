<?php

//---------------------------------------------------------------------------------------------

function add_bank_account($account_code, $account_type, $bank_account_name, $bank_name, $bank_account_number,
	$bank_address, $bank_curr_code)
{
	$sql = "INSERT INTO ".TB_PREF."bank_accounts (account_code, account_type, bank_account_name, bank_name, bank_account_number, bank_address, bank_curr_code)
		VALUES (".db_escape($account_code).", $account_type, ".db_escape($bank_account_name).", ".db_escape($bank_name).", ".db_escape($bank_account_number).",
		".db_escape($bank_address).", '$bank_curr_code')";

	db_query($sql, "could not add a bank account for $account_code");
}

//---------------------------------------------------------------------------------------------

function update_bank_account($account_code, $account_type, $bank_account_name, $bank_name, $bank_account_number,
	$bank_address, $bank_curr_code)
{
	$sql = "UPDATE ".TB_PREF."bank_accounts	SET account_type = $account_type,
		bank_account_name=".db_escape($bank_account_name).", bank_name=".db_escape($bank_name).",
		bank_account_number=".db_escape($bank_account_number).", bank_curr_code='$bank_curr_code',
		bank_address=".db_escape($bank_address)." WHERE account_code = '$account_code'";

	db_query($sql, "could not update bank account for $account_code");
}

//---------------------------------------------------------------------------------------------

function delete_bank_account($account_code)
{
	$sql = "DELETE FROM ".TB_PREF."bank_accounts WHERE account_code='$account_code'";

	db_query($sql,"could not delete bank account for $account_code");
}


//---------------------------------------------------------------------------------------------

function get_bank_account($account_code)
{
	$sql = "SELECT * FROM ".TB_PREF."bank_accounts WHERE account_code='$account_code'";

	$result = db_query($sql, "could not retreive bank account for $account_code");

	return db_fetch($result);
}

//---------------------------------------------------------------------------------------------

?>