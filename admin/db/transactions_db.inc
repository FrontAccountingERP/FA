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
	$trans_date = $db_info[4];

	$sql = "SELECT t.$trans_no_name as trans_no";

	if ($trans_ref)
		$sql .= " ,t.$trans_ref as ref ";
	else
		$sql .= ", r.reference as ref";
 	$sql .= ",t.$trans_date as trans_date";
 	if ($type_name)
		$sql .= ", t.$type_name as type";
	$sql .= " FROM $table_name t LEFT JOIN ".TB_PREF."voided v ON"
		." t.$trans_no_name=v.id AND v.type=$filtertype";
	if (!$trans_ref)
		$sql .= " LEFT JOIN ".TB_PREF."refs r ON t.$trans_no_name=r.id AND r.type=$filtertype";

	$sql .= " WHERE ISNULL(v.`memo_`)";
	if ($from != null && $to != null)
	{
		$sql .= " AND t.$trans_no_name >= ".db_escape($from). "
			AND  t.$trans_no_name <= ".db_escape($to);
		if ($type_name != null)
			$sql .= " AND t.`$type_name` = ".db_escape($filtertype);
	}
	elseif ($type_name != null)
		$sql .= " AND t.`$type_name` = ".db_escape($filtertype);

	// the ugly hack below is necessary to exclude old gl_trans records lasting after edition,
	// otherwise old data transaction can be retrieved instead of current one.
	if ($table_name==TB_PREF.'gl_trans')
		$sql .= " AND t.`amount` <> 0";

	$sql .= " GROUP BY ".($type_name ? "t.$type_name," : '')." t.$trans_no_name";
	$sql .= " ORDER BY t.$trans_no_name";
	return $sql;
}

function transaction_exists($trans_type, $trans_no)
{
	return db_num_rows(db_query(get_sql_for_view_transactions($trans_type, $trans_no, $trans_no, $dummy)));
}
?>