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
function add_gl_account($account_code, $account_name, $account_type, $account_code2)
{
	$sql = "INSERT INTO ".TB_PREF."chart_master (account_code, account_code2, account_name, account_type)
		VALUES (".db_escape($account_code).", ".db_escape($account_code2).", "
			.db_escape($account_name).", ".db_escape($account_type).")";

	return db_query($sql);
}

function update_gl_account($account_code, $account_name, $account_type, $account_code2)
{
    $sql = "UPDATE ".TB_PREF."chart_master SET account_name=".db_escape($account_name)
    .",account_type=".db_escape($account_type).", account_code2=".db_escape($account_code2)
		." WHERE account_code = ".db_escape($account_code);

	return db_query($sql);
}

function delete_gl_account($code)
{
	$sql = "DELETE FROM ".TB_PREF."chart_master WHERE account_code=".db_escape($code);

	db_query($sql, "could not delete gl account");
}

function get_gl_accounts($from=null, $to=null, $type=null)
{
	$sql = "SELECT ".TB_PREF."chart_master.*,".TB_PREF."chart_types.name AS AccountTypeName
		FROM ".TB_PREF."chart_master,".TB_PREF."chart_types
		WHERE ".TB_PREF."chart_master.account_type=".TB_PREF."chart_types.id";
	if ($from != null)
		$sql .= " AND ".TB_PREF."chart_master.account_code >= ".db_escape($from);
	if ($to != null)
		$sql .= " AND ".TB_PREF."chart_master.account_code <= ".db_escape($to);
	if ($type != null)
		$sql .= " AND account_type=".db_escape($type);
	$sql .= " ORDER BY account_code";

	return db_query($sql, "could not get gl accounts");
}

function get_gl_account($code)
{
	$sql = "SELECT * FROM ".TB_PREF."chart_master WHERE account_code=".db_escape($code);

	$result = db_query($sql, "could not get gl account");
	return db_fetch($result);
}

function is_account_balancesheet($code)
{
	$sql = "SELECT ".TB_PREF."chart_class.ctype FROM ".TB_PREF."chart_class, "
		.TB_PREF."chart_types, ".TB_PREF."chart_master
		WHERE ".TB_PREF."chart_master.account_type=".TB_PREF."chart_types.id AND
		".TB_PREF."chart_types.class_id=".TB_PREF."chart_class.cid
		AND ".TB_PREF."chart_master.account_code=".db_escape($code);

	$result = db_query($sql,"could not retreive the account class for $code");
	$row = db_fetch_row($result);
	return $row[0] > 0 && $row[0] < CL_INCOME;
}

function get_gl_account_name($code)
{
	$sql = "SELECT account_name from ".TB_PREF."chart_master WHERE account_code=".db_escape($code);

	$result = db_query($sql,"could not retreive the account name for $code");

	if (db_num_rows($result) == 1)
	{
		$row = db_fetch_row($result);
		return $row[0];
	}

	display_db_error("could not retreive the account name for $code", $sql, true);
}


?>