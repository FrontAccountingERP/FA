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

function add_quick_entry($description, $account, $deposit)
{
	$sql = "INSERT INTO ".TB_PREF."quick_entries (description, account, deposit) VALUES 
		(".db_escape($description).", ".db_escape($account).", $deposit)";

	db_query($sql, "could not insert quick entry for $description");
}

//---------------------------------------------------------------------------------------------

function update_quick_entry($selected_id, $description, $account, $deposit)
{
	$sql = "UPDATE ".TB_PREF."quick_entries	SET description = ".db_escape($description).",
		account=".db_escape($account).", deposit=$deposit 
		WHERE id = $selected_id";

	db_query($sql, "could not update quick entry for $selected_id");
}

//---------------------------------------------------------------------------------------------

function delete_quick_entry($selected_id)
{
	$sql = "DELETE FROM ".TB_PREF."quick_entries WHERE id=$selected_id";

	db_query($sql,"could not delete quick entry $selected_id");
}

//---------------------------------------------------------------------------------------------

function get_quick_entries($type=false)
{
	$sql = "SELECT ".TB_PREF."quick_entries.*, ".TB_PREF."chart_master.account_name 
		FROM ".TB_PREF."quick_entries, ".TB_PREF."chart_master WHERE
		".TB_PREF."quick_entries.account = ".TB_PREF."chart_master.account_code";
	if ($type == "deposit")
		$sql .= " AND deposit=1";
	elseif ($type == "payment")
		$sql .= " AND deposit=0";

	return db_query($sql, "could not retreive quick entries");
}

//---------------------------------------------------------------------------------------------

function get_quick_entry($selected_id)
{
	$sql = "SELECT * FROM ".TB_PREF."quick_entries WHERE id=$selected_id";

	$result = db_query($sql, "could not retreive quick entry for $selected_id");

	return db_fetch($result);
}

//---------------------------------------------------------------------------------------------

?>