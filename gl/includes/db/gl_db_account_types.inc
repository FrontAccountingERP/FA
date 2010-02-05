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
		VALUES ($id, ".db_escape($name).", ".db_escape($class_id).", ".db_escape($parent).")";

	return db_query($sql);
}

function update_account_type($id, $name, $class_id, $parent)
{
    $sql = "UPDATE ".TB_PREF."chart_types SET name=".db_escape($name).",
		class_id=".db_escape($class_id).", parent=".db_escape($parent)
		." WHERE id = ".db_escape($id);

	return db_query($sql, "could not update account type");
}

function get_account_types($all=false, $class_id=false, $parent=false)
{
	$sql = "SELECT * FROM ".TB_PREF."chart_types";

	if (!$all) 
		$sql .= " WHERE !inactive";
	if ($class_id != false) 
		$sql .= " AND class_id=".db_escape($class_id);
	if ($parent == -1) 
		$sql .= " AND parent <= 0";
	elseif ($parent != false) 
		$sql .= " AND parent=".db_escape($parent);
	$sql .= " ORDER BY class_id, id";

	return db_query($sql, "could not get account types");
}

function get_account_type($id)
{
	$sql = "SELECT * FROM ".TB_PREF."chart_types WHERE id = ".db_escape($id);

	$result = db_query($sql, "could not get account type");

	return db_fetch($result);
}

function get_account_type_name($id)
{
	$sql = "SELECT name FROM ".TB_PREF."chart_types WHERE id = ".db_escape($id);

	$result = db_query($sql, "could not get account type");

	$row = db_fetch_row($result);
	return $row[0];
}

function delete_account_type($id)
{
	$sql = "DELETE FROM ".TB_PREF."chart_types WHERE id = ".db_escape($id);

	db_query($sql, "could not delete account type");
}

function add_account_class($id, $name, $ctype)
{
	$sql = "INSERT INTO ".TB_PREF."chart_class (cid, class_name, ctype)
		VALUES (".db_escape($id).", ".db_escape($name).", ".db_escape($ctype).")";

	return db_query($sql);
}

function update_account_class($id, $name, $ctype)
{
    $sql = "UPDATE ".TB_PREF."chart_class SET class_name=".db_escape($name).",
		ctype=".db_escape($ctype)." WHERE cid = ".db_escape($id);

	return db_query($sql);
}

function get_account_classes($all=false, $balance=-1)
{
	$sql = "SELECT * FROM ".TB_PREF."chart_class";
	if (!$all) 
		$sql .= " WHERE !inactive";
	if ($balance == 0) 
		$sql .= " AND ctype>".CL_EQUITY." OR ctype=0";
	elseif ($balance == 1) 
		$sql .= " AND ctype>0 AND ctype<".CL_INCOME; 
	$sql .= " ORDER BY cid";

	return db_query($sql, "could not get account classes");
}

function get_account_class($id)
{
	$sql = "SELECT * FROM ".TB_PREF."chart_class WHERE cid = ".db_escape($id);

	$result = db_query($sql, "could not get account type");

	return db_fetch($result);
}

function get_account_class_name($id)
{
	$sql = "SELECT class_name FROM ".TB_PREF."chart_class WHERE cid =".db_escape($id);

	$result = db_query($sql, "could not get account type");

	$row = db_fetch_row($result);
	return $row[0];
}

function delete_account_class($id)
{
	$sql = "DELETE FROM ".TB_PREF."chart_class WHERE cid = ".db_escape($id);

	db_query($sql, "could not delete account type");
}

?>