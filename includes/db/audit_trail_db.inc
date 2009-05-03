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

function add_audit_trail($trans_type, $trans_no, $trans_date, $descr='')
{
	$sql = "INSERT INTO ".TB_PREF."audit_trail"
		. " (type, trans_no, user, fiscal_year, gl_date, description)
			VALUES($trans_type, $trans_no,"
			. $_SESSION["wa_current_user"]->user. ","
			. get_company_pref('f_year') .","
			. "'". date2sql($trans_date) ."',"
			. db_escape($descr). ")";

	db_query($sql, "Cannot add audit info");
}

function get_audit_trail_all($trans_type, $trans_no)
{
	$sql = "SELECT * FROM ".TB_PREF."audit_trail"
		." WHERE type=$trans_type AND trans_no=$trans_no";

	return db_query($sql, "Cannot get all audit info for transaction");
}

function get_audit_trail_last($trans_type, $trans_no)
{
	$sql = "SELECT * FROM ".TB_PREF."audit_trail"
		." WHERE type=$trans_type AND trans_no=$trans_no ORDER BY id desc";

	$res = db_query($sql, "Cannot get last audit info for transaction");
	if ($res)
		$row = db_fetch($res);

	return $row;
}

?>
