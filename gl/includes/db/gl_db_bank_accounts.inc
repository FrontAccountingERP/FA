<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU General Public License, GPL, 
	as published by the Free Software Foundation, either version 3 
	of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/gpl-3.0.html>.
***********************************************************************/
//---------------------------------------------------------------------------------------------
function clear_dflt_curr_account($curr_code) {
	  $sql = "UPDATE ".TB_PREF."bank_accounts SET dflt_curr_act=0 WHERE bank_curr_code="
	  .db_escape($curr_code);
	  db_query($sql, "could not update default currency account");
}

function add_bank_account($account_code, $account_type, $bank_account_name,
	$bank_name, $bank_account_number, $bank_address, $bank_curr_code, 
	$dflt_curr_act)
{
	if($dflt_curr_act) 	// only one default account for any currency
	  clear_dflt_curr_account($bank_curr_code);

	$sql = "INSERT INTO ".TB_PREF."bank_accounts (account_code, account_type, 
		bank_account_name, bank_name, bank_account_number, bank_address, 
		bank_curr_code, dflt_curr_act)
		VALUES (".db_escape($account_code).", ".db_escape($account_type).", "
		.db_escape($bank_account_name).", ".db_escape($bank_name).", "
		.db_escape($bank_account_number).",".db_escape($bank_address).
		", ".db_escape($bank_curr_code).", ".db_escape($dflt_curr_act).")";

	db_query($sql, "could not add a bank account for $account_code");
}

//---------------------------------------------------------------------------------------------

function update_bank_account($id, $account_code, $account_type, $bank_account_name, 
	$bank_name, $bank_account_number, $bank_address, $bank_curr_code, $dflt_curr_act)
{
	if($dflt_curr_act) 	// only one default account for any currency
	  clear_dflt_curr_account($bank_curr_code);

	$sql = "UPDATE ".TB_PREF."bank_accounts	SET account_type = ".db_escape($account_type).",
		account_code=".db_escape($account_code).",
		bank_account_name=".db_escape($bank_account_name).", bank_name=".db_escape($bank_name).",
		bank_account_number=".db_escape($bank_account_number).", bank_curr_code=".db_escape($bank_curr_code).",
		bank_address=".db_escape($bank_address).",
		dflt_curr_act=".db_escape($dflt_curr_act)
		." WHERE id = ".db_escape($id);

	db_query($sql, "could not update bank account for $account_code");
}

//---------------------------------------------------------------------------------------------

function delete_bank_account($id)
{
	$sql = "DELETE FROM ".TB_PREF."bank_accounts WHERE id=".db_escape($id);

	db_query($sql,"could not delete bank account for $id");
}


//---------------------------------------------------------------------------------------------

function get_bank_account($id)
{
	$sql = "SELECT * FROM ".TB_PREF."bank_accounts WHERE id=".db_escape($id);

	$result = db_query($sql, "could not retreive bank account for $id");

	return db_fetch($result);
}

//---------------------------------------------------------------------------------------------
function get_bank_gl_account($id)
{
	$sql = "SELECT account_code FROM ".TB_PREF."bank_accounts WHERE id=".db_escape($id);

	$result = db_query($sql, "could not retreive bank account for $id");

	$bank_account = db_fetch($result);

	return $bank_account['account_code'];
}

//---------------------------------------------------------------------------------------------

function add_quick_entry($description, $type, $base_amount, $base_desc)
{
	$sql = "INSERT INTO ".TB_PREF."quick_entries (description, type, base_amount, base_desc) 
	VALUES (".db_escape($description).", ".db_escape($type).", "
		.db_escape($base_amount).", ".db_escape($base_desc).")";

	db_query($sql, "could not insert quick entry for $description");
}

//---------------------------------------------------------------------------------------------

function update_quick_entry($selected_id, $description, $type, $base_amount, $base_desc)
{
	$sql = "UPDATE ".TB_PREF."quick_entries	SET description = ".db_escape($description).",
		type=".db_escape($type).", base_amount=".db_escape($base_amount)
		.", base_desc=".db_escape($base_desc)."
		WHERE id = ".db_escape($selected_id);

	db_query($sql, "could not update quick entry for $selected_id");
}

//---------------------------------------------------------------------------------------------

function delete_quick_entry($selected_id)
{
	$sql = "DELETE FROM ".TB_PREF."quick_entries WHERE id=".db_escape($selected_id);

	db_query($sql,"could not delete quick entry $selected_id");
}

//---------------------------------------------------------------------------------------------

function add_quick_entry_line($qid, $action, $dest_id, $amount, $dim, $dim2)
{
	$sql = "INSERT INTO ".TB_PREF."quick_entry_lines 
		(qid, action, dest_id, amount, dimension_id, dimension2_id) 
	VALUES 
		($qid, ".db_escape($action).",".db_escape($dest_id).",
			".db_escape($amount).", ".db_escape($dim).", ".db_escape($dim2).")";

	db_query($sql, "could not insert quick entry line for $qid");
}

//---------------------------------------------------------------------------------------------

function update_quick_entry_line($selected_id, $qid, $action, $dest_id, $amount, $dim, $dim2)
{
	$sql = "UPDATE ".TB_PREF."quick_entry_lines SET qid = ".db_escape($qid)
		.", action=".db_escape($action).",
		dest_id=".db_escape($dest_id).", amount=".db_escape($amount)
		.", dimension_id=".db_escape($dim).", dimension2_id=".db_escape($dim2)." 
		WHERE id = ".db_escape($selected_id);

	db_query($sql, "could not update quick entry line for $selected_id");
}

//---------------------------------------------------------------------------------------------

function delete_quick_entry_line($selected_id)
{
	$sql = "DELETE FROM ".TB_PREF."quick_entry_lines WHERE id=".db_escape($selected_id);

	db_query($sql,"could not delete quick entry line $selected_id");
}
//---------------------------------------------------------------------------------------------


function has_quick_entries($type=null)
{
	$sql = "SELECT id FROM ".TB_PREF."quick_entries";
	if ($type != null)
		$sql .= " WHERE type=".db_escape($type);

	$result = db_query($sql, "could not retreive quick entries");
	return db_num_rows($result) > 0;
}

function get_quick_entries($type = null)
{
	$sql = "SELECT * FROM ".TB_PREF."quick_entries";
	if ($type != null)
		$sql .= " WHERE type=".db_escape($type);
	$sql .= " ORDER BY description";

	return db_query($sql, "could not retreive quick entries");
}

function get_quick_entry($selected_id)
{
	$sql = "SELECT * FROM ".TB_PREF."quick_entries WHERE id=".db_escape($selected_id);

	$result = db_query($sql, "could not retreive quick entry $selected_id");

	return db_fetch($result);
}	

function get_quick_entry_lines($qid)
{
	$sql = "SELECT ".TB_PREF."quick_entry_lines.*, ".TB_PREF."chart_master.account_name,
			".TB_PREF."tax_types.name as tax_name
		FROM ".TB_PREF."quick_entry_lines
		LEFT JOIN ".TB_PREF."chart_master ON
			".TB_PREF."quick_entry_lines.dest_id = ".TB_PREF."chart_master.account_code
		LEFT JOIN ".TB_PREF."tax_types ON
			".TB_PREF."quick_entry_lines.dest_id = ".TB_PREF."tax_types.id
		WHERE 
			qid=".db_escape($qid)." ORDER by id";

	return db_query($sql, "could not retreive quick entries");
}

function has_quick_entry_lines($qid)
{
	$sql = "SELECT id FROM ".TB_PREF."quick_entry_lines WHERE qid=".db_escape($qid);

	$result = db_query($sql, "could not retreive quick entries");
	return db_num_rows($result) > 0;
}

//---------------------------------------------------------------------------------------------

function get_quick_entry_line($selected_id)
{
	$sql = "SELECT * FROM ".TB_PREF."quick_entry_lines WHERE id=".db_escape($selected_id);

	$result = db_query($sql, "could not retreive quick entry for $selected_id");

	return db_fetch($result);
}

//---------------------------------------------------------------------------------------------

function get_default_bank_account($curr)
{
	/* default bank account is selected as first found account from:
		. default account in $curr if any
		. first defined account in $curr if any
		. default account in home currency
		. first defined account in home currency
	*/
	$home_curr = get_company_pref('curr_default');

	$sql = "SELECT b.*, b.bank_curr_code='$home_curr' as fall_back FROM "
		.TB_PREF."bank_accounts b"
		." WHERE b.bank_curr_code=".db_escape($curr)
		." OR b.bank_curr_code='$home_curr'
		ORDER BY fall_back, dflt_curr_act desc";

	$result = db_query($sql, "could not retreive default bank account");

	return db_fetch($result);
}
?>