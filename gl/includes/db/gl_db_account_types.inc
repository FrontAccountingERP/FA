<?php

function add_account_type($name, $class_id, $parent)
{
	$sql = "INSERT INTO ".TB_PREF."chart_types (name, class_id, parent)
		VALUES (".db_escape($name).", $class_id, $parent)";

	db_query($sql, "could not add account type");
}

function update_account_type($id, $name, $class_id, $parent)
{
    $sql = "UPDATE ".TB_PREF."chart_types SET name=".db_escape($name).",
		class_id=$class_id,	parent=$parent WHERE id = $id";

	db_query($sql, "could not update account type");
}

function get_account_types()
{
	$sql = "SELECT * FROM ".TB_PREF."chart_types ORDER BY class_id, id";

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

function add_account_class($id, $name, $balance)
{
	$sql = "INSERT INTO ".TB_PREF."chart_class (cid, class_name, balance_sheet)
		VALUES ($id, ".db_escape($name).", $balance)";

	db_query($sql, "could not add account type");
}

function update_account_class($id, $name, $balance)
{
    $sql = "UPDATE ".TB_PREF."chart_class SET class_name=".db_escape($name).",
		balance_sheet=$balance WHERE cid = $id";

	db_query($sql, "could not update account type");
}

function get_account_classes()
{
	$sql = "SELECT * FROM ".TB_PREF."chart_class ORDER BY cid";

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


function delete_account_class($id)
{
	$sql = "DELETE FROM ".TB_PREF."chart_class WHERE cid = $id";

	db_query($sql, "could not delete account type");
}


?>