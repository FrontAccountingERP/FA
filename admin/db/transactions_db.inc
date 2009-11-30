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

//--------------------------------------------------------------------------------------

function get_sql_for_view_transactions($filtertype, $from, $to, &$trans_ref)
{
	$db_info = get_systype_db_info($filtertype);

	if ($db_info == null)
		return "";

	$table_name = $db_info[0];
	$type_name = $db_info[1];
	$trans_no_name = $db_info[2];
	$trans_ref = $db_info[3];

	$sql = "SELECT DISTINCT $trans_no_name as trans_no";

	if ($trans_ref)
		$sql .= " ,$trans_ref ";

	$sql .= ", ".$filtertype." as type FROM $table_name
		WHERE $trans_no_name >= ".db_escape($from). "
		AND  $trans_no_name <= ".db_escape($to);

	if ($type_name != null)
		$sql .= " AND `$type_name` = ".db_escape($filtertype);

	$sql .= " ORDER BY $trans_no_name";
	return $sql;
}

?>