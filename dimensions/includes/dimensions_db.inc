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
function add_dimension($reference, $name, $type_, $date_, $due_date, $memo_)
{
	global $Refs;

	begin_transaction();

	$date = date2sql($date_);
	$duedate = date2sql($due_date);

	$sql = "INSERT INTO ".TB_PREF."dimensions (reference, name, type_, date_, due_date)
		VALUES (".db_escape($reference).", ".db_escape($name).", ".db_escape($type_)
		.", '$date', '$duedate')";
	db_query($sql, "could not add dimension");
	$id = db_insert_id();

	add_comments(ST_DIMENSION, $id, $date_, $memo_);

	$Refs->save(ST_DIMENSION, $id, $reference);

	commit_transaction();

	return $id;
}

function update_dimension($id, $name, $type_, $date_, $due_date, $memo_)
{
	begin_transaction();

	$date = date2sql($date_);
	$duedate = date2sql($due_date);

	$sql = "UPDATE ".TB_PREF."dimensions SET name=".db_escape($name).",
		type_ = ".db_escape($type_).",
		date_='$date',
		due_date='$duedate'
		WHERE id = ".db_escape($id);

	db_query($sql, "could not update dimension");

	update_comments(ST_DIMENSION, $id, null, $memo_);

	commit_transaction();

	return $id;
}

function delete_dimension($id)
{
	begin_transaction();

	// delete the actual dimension
	$sql="DELETE FROM ".TB_PREF."dimensions WHERE id=".db_escape($id);
	db_query($sql,"The dimension could not be deleted");

	delete_comments(ST_DIMENSION, $id);

	commit_transaction();
}

//--------------------------------------------------------------------------------------

function get_dimension($id, $allow_null=false)
{
    $sql = "SELECT * FROM ".TB_PREF."dimensions	WHERE id=".db_escape($id);

	$result = db_query($sql, "The dimension could not be retrieved");

	if (!$allow_null && db_num_rows($result) == 0)
		display_db_error("Could not find dimension $id", $sql);

	return db_fetch($result);
}

//--------------------------------------------------------------------------------------

function get_dimension_string($id, $html=false, $space=' ')
{
	if ($id <= 0)
	{
		if ($html)
			$dim = "&nbsp;";
		else
			$dim = "";
	}
	else
	{
		$row = get_dimension($id, true);
		$dim = $row['reference'] . $space . $row['name'];
	}

	return $dim;
}

//--------------------------------------------------------------------------------------

function get_dimensions()
{
	$sql = "SELECT * FROM ".TB_PREF."dimensions ORDER BY date_";

	return db_query($sql, "The dimensions could not be retrieved");
}

//--------------------------------------------------------------------------------------

function dimension_has_deposits($id)
{
	return dimension_has_payments($id);
}

//--------------------------------------------------------------------------------------

function dimension_has_payments($id)
{
	$sql = "SELECT SUM(amount) FROM ".TB_PREF."gl_trans WHERE dimension_id = ".db_escape($id);
	$res = db_query($sql, "Transactions could not be calculated");
	$row = db_fetch_row($res);
	return ($row[0] != 0.0);
}

function dimension_is_closed($id)
{
	$result = get_dimension($id);
	return ($result['closed'] == '1');
}

//--------------------------------------------------------------------------------------

function close_dimension($id)
{
	$sql = "UPDATE ".TB_PREF."dimensions SET closed='1' WHERE id = ".db_escape($id);
	db_query($sql, "could not close dimension");
}

//--------------------------------------------------------------------------------------

function reopen_dimension($id)
{
	$sql = "UPDATE ".TB_PREF."dimensions SET closed='0' WHERE id = $id";
	db_query($sql, "could not reopen dimension");
}

?>