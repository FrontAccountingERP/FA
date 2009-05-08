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
//--------------------------------------------------------------------------------------------------

function get_reference($type, $id)
{
	$sql = "SELECT * FROM ".TB_PREF."refs WHERE type=$type AND id=$id";

	$result = db_query($sql, "could not query reference table");
    $row = db_fetch($result);
    return $row['reference'];
}

//--------------------------------------------------------------------------------------------------

function add_reference($type, $id, $reference)
{
	$sql = "INSERT INTO ".TB_PREF."refs (type, id, reference)
		VALUES ($type, $id, " . db_escape(trim($reference)) . ")";

	db_query($sql, "could not add reference entry");
}

//--------------------------------------------------------------------------------------------------

function delete_reference($type, $id)
{
	$sql = "DELETE FROM ".TB_PREF."refs WHERE type=$type AND id=$id";

	return db_query($sql, "could not delete from reference table");
}

//--------------------------------------------------------------------------------------------------

function find_reference($type, $reference)
{
	$sql = "SELECT id FROM ".TB_PREF."refs WHERE type=$type AND reference='$reference'";

	$result = db_query($sql, "could not query reference table");

    return (db_num_rows($result) > 0);
}

//--------------------------------------------------------------------------------------------------

function save_next_reference($type, $reference)
{
    $sql = "UPDATE ".TB_PREF."sys_types SET next_reference=" . db_escape(trim($reference)) . " WHERE type_id = $type";

	db_query($sql, "The next transaction ref for $type could not be updated");
}

//--------------------------------------------------------------------------------------------------

function get_next_reference($type)
{
    $sql = "SELECT next_reference FROM ".TB_PREF."sys_types WHERE type_id = $type";

    $result = db_query($sql,"The last transaction ref for $type could not be retreived");

    $row = db_fetch_row($result);
    return $row[0];
}

?>