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
function add_account_type($id, $name, $class_id, $parent)
{
	$sql = "INSERT INTO ".TB_PREF."chart_types (id, name, class_id, parent)
		VALUES ($id, ".db_escape($name).", $class_id, $parent)";

	db_query($sql, "could not add account type");
}

function update_account_type($id, $name, $class_id, $parent)
{
    $sql = "UPDATE ".TB_PREF."chart_types SET name=".db_escape($name).",
		class_id=$class_id,	parent=$parent WHERE id = $id";

	db_query($sql, "could not update account type");
}

function get_account_types($all=false)
{
	$sql = "SELECT * FROM ".TB_PREF."chart_types";

	if (!$all) $sql .= " WHERE !inactive";
	$sql .= " ORDER BY class_id, id";

	return db_query($sql, "could not get account types");
}

function get_account_type($id)
{
	$sql = "SELECT * FROM ".TB_PREF."chart_types WHERE id = $id";

	$result = db_query($sql, "could not get account type");

	return db_fetch($result);
}

function get_account_type_name($id)
{
	$sql = "SELECT name FROM ".TB_PREF."chart_types WHERE id = $id";

	$result = db_query($sql, "could not get account type");

	$row = db_fetch_row($result);
	return $row[0];
}


function delete_account_type($id)
{
	$sql = "DELETE FROM ".TB_PREF."chart_types WHERE id = $id";

	db_query($sql, "could not delete account type");
}

function add_account_class($id, $name, $balance, $sign_conv)
{
	$sql = "INSERT INTO ".TB_PREF."chart_class (cid, class_name, balance_sheet, sign_convert)
		VALUES ($id, ".db_escape($name).", $balance, $sign_conv)";

	db_query($sql, "could not add account type");
}

function update_account_class($id, $name, $balance, $sign_conv)
{
    $sql = "UPDATE ".TB_PREF."chart_class SET class_name=".db_escape($name).",
		balance_sheet=$balance, sign_convert=$sign_conv WHERE cid = $id";

	db_query($sql, "could not update account type");
}

function get_account_classes($all=false)
{
	$sql = "SELECT * FROM ".TB_PREF."chart_class";
	if (!$all) $sql .= " WHERE !inactive";
	$sql .= " ORDER BY cid";

	return db_query($sql, "could not get account types");
}

function get_account_class($id)
{
	$sql = "SELECT * FROM ".TB_PREF."chart_class WHERE cid = $id";

	$result = db_query($sql, "could not get account type");

	return db_fetch($result);
}

function get_account_class_name($id)
{
	$sql = "SELECT class_name FROM ".TB_PREF."chart_class WHERE cid = $id";

	$result = db_query($sql, "could not get account type");

	$row = db_fetch_row($result);
	return $row[0];
}

function get_sign_convert($account_type)
{
	$sql = "SELECT sign_convert FROM ".TB_PREF."chart_class INNER JOIN ".TB_PREF."chart_types ON 
		".TB_PREF."chart_class.cid = ".TB_PREF."chart_types.class_id WHERE ".TB_PREF."chart_types.id=$account_type";
	$result = db_query($sql, "could not get sign convert");

	$row = db_fetch_row($result);
	return ($row[0]==1);
}
		
function delete_account_class($id)
{
	$sql = "DELETE FROM ".TB_PREF."chart_class WHERE cid = $id";

	db_query($sql, "could not delete account type");
}


?>