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
	$sql = "SELECT * FROM ".TB_PREF."refs WHERE type=".db_escape($type)." AND id=".db_escape($id);

	$result = db_query($sql, "could not query reference table");
    $row = db_fetch($result);
    return $row['reference'];
}

//--------------------------------------------------------------------------------------------------
 
function update_reference($type, $id, $reference)
{
    $sql = "REPLACE ".TB_PREF."refs SET reference=".db_escape($reference)
			.", type=".db_escape($type).", id=".db_escape($id);
    db_query($sql, "could not update reference entry");
}

//--------------------------------------------------------------------------------------------------

function delete_reference($type, $id)
{
	$sql = "DELETE FROM ".TB_PREF."refs WHERE type=".db_escape($type)." AND id=".db_escape($id);

	return db_query($sql, "could not delete from reference table");
}

//--------------------------------------------------------------------------------------------------

function find_reference($type, $reference)
{
	// ignore refs references for voided transactions
	$sql = "SELECT r.id FROM ".TB_PREF."refs r LEFT JOIN ".TB_PREF."voided v ON"
		." r.type=v.type AND r.id=v.id"
		." WHERE r.type=".db_escape($type)
		." AND reference=".db_escape($reference)
		." AND ISNULL(`memo_`)";

	$result = db_query($sql, "could not query reference table");

    return (db_num_rows($result) > 0);
}

//--------------------------------------------------------------------------------------------------

function save_next_reference($type, $reference)
{
    $sql = "UPDATE ".TB_PREF."sys_types SET next_reference=" . db_escape(trim($reference)) 
		. " WHERE type_id = ".db_escape($type);

	db_query($sql, "The next transaction ref for $type could not be updated");
}

//--------------------------------------------------------------------------------------------------

function get_next_reference($type)
{
    $sql = "SELECT next_reference FROM ".TB_PREF."sys_types WHERE type_id = ".db_escape($type);

    $result = db_query($sql,"The last transaction ref for $type could not be retreived");

    $row = db_fetch_row($result);
    return $row[0];
}

//----------------------------------------------------------------------------
//
//	Check if reference was not used so far (for other transaction than $trans_no)
//
function is_new_reference($ref, $type, $trans_no=0)
{
	$db_info = get_systype_db_info($type);
	$db_name = $db_info[0];
	$db_type = $db_info[1];
	$db_trans = $db_info[2];
	$db_ref = $db_info[3];
	
	$ref = db_escape(trim($ref));
	$type = db_escape($type);
	
	if ($db_ref == null) { // journal or bank trans store references in refs table
		$db_name = TB_PREF."refs";
		$db_type = 'type';
		$db_trans = 'id';
		$db_ref = 'reference';
	}

	if ($db_type != null) {
		$sql = "SELECT $db_ref FROM $db_name tbl
			LEFT JOIN ".TB_PREF."voided v ON 
				tbl.$db_type=v.type AND tbl.$db_trans=v.id
			WHERE $db_ref=$ref AND ISNULL(v.id)
				AND tbl.$db_type=$type";
	} else {
		$sql = "SELECT $db_ref ref FROM $db_name tbl
			LEFT JOIN ".TB_PREF."voided v ON 
				v.type=$type AND tbl.$db_trans=v.id
			WHERE $db_ref=$ref AND ISNULL(v.id)";
	}
	if ($trans_no)
			$sql .= " AND tbl.`$db_trans` != ".db_escape($trans_no);

	$result = db_query($sql, "could not test for unique reference");

	return (db_num_rows($result) == 0);

}


