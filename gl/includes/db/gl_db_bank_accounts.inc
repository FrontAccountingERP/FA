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

function update_bank_account($id, $account_code, $account_type, $bank_account_name, $bank_name, $bank_account_number,
	$bank_address, $bank_curr_code)
{
	$sql = "UPDATE ".TB_PREF."bank_accounts	SET account_type = $account_type,
		account_code=".db_escape($account_code).",
		bank_account_name=".db_escape($bank_account_name).", bank_name=".db_escape($bank_name).",
		bank_account_number=".db_escape($bank_account_number).", bank_curr_code='$bank_curr_code',
		bank_address=".db_escape($bank_address)." WHERE id = '$id'";

	db_query($sql, "could not update bank account for $account_code");
}

//---------------------------------------------------------------------------------------------

function delete_bank_account($id)
{
	$sql = "DELETE FROM ".TB_PREF."bank_accounts WHERE id='$id'";

	db_query($sql,"could not delete bank account for $id");
}


//---------------------------------------------------------------------------------------------

function get_bank_account($id)
{
	$sql = "SELECT * FROM ".TB_PREF."bank_accounts WHERE id='$id'";

	$result = db_query($sql, "could not retreive bank account for $id");

	return db_fetch($result);
}

//---------------------------------------------------------------------------------------------
function get_bank_gl_account($id)
{
	$sql = "SELECT account_code FROM ".TB_PREF."bank_accounts WHERE id='$id'";

	$result = db_query($sql, "could not retreive bank account for $id");

	$bank_account = db_fetch($result);

	return $bank_account['account_code'];
}

//---------------------------------------------------------------------------------------------

function add_quick_entry($description, $deposit, $bank_only)
{
	$sql = "INSERT INTO ".TB_PREF."quick_entries (description, deposit, bank_only) VALUES 
		(".db_escape($description).", $deposit, $bank_only)";

	db_query($sql, "could not insert quick entry for $description");
}

//---------------------------------------------------------------------------------------------

function update_quick_entry($selected_id, $description, $deposit, $bank_only)
{
	$sql = "UPDATE ".TB_PREF."quick_entries	SET description = ".db_escape($description).",
		deposit=$deposit, bank_only=$bank_only	WHERE id = $selected_id";

	db_query($sql, "could not update quick entry for $selected_id");
}

//---------------------------------------------------------------------------------------------

function delete_quick_entry($selected_id)
{
	$sql = "DELETE FROM ".TB_PREF."quick_entries WHERE id=$selected_id";

	db_query($sql,"could not delete quick entry $selected_id");
}

//---------------------------------------------------------------------------------------------

function add_quick_entry_line($qid, $account, $tax_acc, $pct, $amount, $dim, $dim2)
{
	$sql = "INSERT INTO ".TB_PREF."quick_entry_lines (qid, account, tax_acc, pct, amount, dimension_id, dimension2_id) VALUES 
		($qid, ".db_escape($account).", $tax_acc, $pct, $amount, $dim, $dim2)";

	db_query($sql, "could not insert quick entry line for $qid");
}

//---------------------------------------------------------------------------------------------

function update_quick_entry_line($selected_id, $qid, $account, $tax_acc, $pct, $amount, $dim, $dim2)
{
	$sql = "UPDATE ".TB_PREF."quick_entry_lines	SET qid = $qid, account=".db_escape($account).",
		tax_acc=$tax_acc, pct=$pct, amount=$amount, dimension_id=$dim, dimension2_id=$dim2 
		WHERE id = $selected_id";

	db_query($sql, "could not update quick entry line for $selected_id");
}

//---------------------------------------------------------------------------------------------

function delete_quick_entry_line($selected_id)
{
	$sql = "DELETE FROM ".TB_PREF."quick_entry_lines WHERE id=$selected_id";

	db_query($sql,"could not delete quick entry line $selected_id");
}
//---------------------------------------------------------------------------------------------


function has_quick_entries($deposit=-1, $bank_only=-1)
{
	$where = false;
	$sql = "SELECT id FROM ".TB_PREF."quick_entries";
	if ($deposit != -1)
	{
		$sql .= " WHERE deposit=$deposit";
		$where = true;
	}		
	if ($bank_only != -1)
	{
		if ($where)
			$sql .= " AND bank_only=$bank_only";
		else	
			$sql .= " WHERE bank_only=$bank_only";
	}		

	$result = db_query($sql, "could not retreive quick entries");
	return db_num_rows($result) > 0;
}

function get_quick_entries($deposit=-1, $bank_only=-1)
{
	$where = false;
	$sql = "SELECT * FROM ".TB_PREF."quick_entries";
	if ($deposit != -1)
	{
		$sql .= " WHERE deposit=$deposit";
		$where = true;
	}		
	if ($bank_only != -1)
	{
		if ($where)
			$sql .= " AND bank_only=$bank_only";
		else	
			$sql .= " WHERE bank_only=$bank_only";
	}		
	$sql .= " ORDER BY description";

	return db_query($sql, "could not retreive quick entries");
}

function get_quick_entry($selected_id)
{
	$sql = "SELECT * FROM ".TB_PREF."quick_entries WHERE id=$selected_id";

	$result = db_query($sql, "could not retreive quick entry $selected_id");

	return db_fetch($result);
}	

function get_quick_entry_lines($qid)
{
	$sql = "SELECT ".TB_PREF."quick_entry_lines.*, ".TB_PREF."chart_master.account_name 
		FROM ".TB_PREF."quick_entry_lines, ".TB_PREF."chart_master 
		WHERE ".TB_PREF."quick_entry_lines.account = ".TB_PREF."chart_master.account_code
		AND qid=$qid";

	return db_query($sql, "could not retreive quick entries");
}

function has_quick_entry_lines($qid)
{
	$sql = "SELECT id FROM ".TB_PREF."quick_entry_lines WHERE qid=$qid";

	$result = db_query($sql, "could not retreive quick entries");
	return db_num_rows($result) > 0;
}

//---------------------------------------------------------------------------------------------

function get_quick_entry_line($selected_id)
{
	$sql = "SELECT * FROM ".TB_PREF."quick_entry_lines WHERE id=$selected_id";

	$result = db_query($sql, "could not retreive quick entry for $selected_id");

	return db_fetch($result);
}

//---------------------------------------------------------------------------------------------

?>