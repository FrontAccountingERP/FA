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
Function get_next_trans_no ($trans_type){

    // sql to get the next display transaction number
	//these are maintained in the table sys_types
    // Also updates the transaction number

    $sql = "SELECT type_no FROM ".TB_PREF."sys_types WHERE type_id = " . $trans_type;

    $result = db_query($sql,"The next transaction number for $trans_type could not be retrieved");

    $myrow = db_fetch_row($result);

    $next_number = $myrow[0] + 1;

    $sql = "UPDATE ".TB_PREF."sys_types SET type_no = $next_number WHERE type_id = $trans_type
	AND type_no = ". $myrow[0]; //concurrency paranoic protection

    db_query($sql,"The next transaction number for $trans_type could not be updated");

    return $next_number;
}

//-----------------------------------------------------------------------------

function get_systype_db_info($type)
{
	switch ($type)
	{
        case     ST_JOURNAL      : return array("".TB_PREF."gl_trans", "type", "type_no", null, "tran_date");
        case     ST_BANKPAYMENT  : return array("".TB_PREF."bank_trans", "type", "trans_no", "ref", "trans_date");
        case     ST_BANKDEPOSIT  : return array("".TB_PREF."bank_trans", "type", "trans_no", "ref", "trans_date");
        case     3               : return null;
        case     ST_BANKTRANSFER : return array("".TB_PREF."bank_trans", "type", "trans_no", "ref", "trans_date");
        case     ST_SALESINVOICE : return array("".TB_PREF."debtor_trans", "type", "trans_no", "reference", "tran_date");
        case     ST_CUSTCREDIT   : return array("".TB_PREF."debtor_trans", "type", "trans_no", "reference", "tran_date");
        case     ST_CUSTPAYMENT  : return array("".TB_PREF."debtor_trans", "type", "trans_no", "reference", "tran_date");
        case     ST_CUSTDELIVERY : return array("".TB_PREF."debtor_trans", "type", "trans_no", "reference", "tran_date");
        case     ST_LOCTRANSFER  : return array("".TB_PREF."stock_moves", "type", "trans_no", "reference", "tran_date");
        case     ST_INVADJUST    : return array("".TB_PREF."stock_moves", "type", "trans_no", "reference", "tran_date");
        case     ST_PURCHORDER   : return array("".TB_PREF."purch_orders", null, "order_no", "reference", "tran_date");
        case     ST_SUPPINVOICE  : return array("".TB_PREF."supp_trans", "type", "trans_no", "reference", "tran_date");
        case     ST_SUPPCREDIT   : return array("".TB_PREF."supp_trans", "type", "trans_no", "reference", "tran_date");
        case     ST_SUPPAYMENT   : return array("".TB_PREF."supp_trans", "type", "trans_no", "reference", "tran_date");
        case     ST_SUPPRECEIVE  : return array("".TB_PREF."grn_batch", null, "id", "reference", "delivery_date");
        case     ST_WORKORDER    : return array("".TB_PREF."workorders", null, "id", "wo_ref", "released_date");
        case     ST_MANUISSUE    : return array("".TB_PREF."wo_issues", null, "issue_no", "reference", "issue_date");
        case     ST_MANURECEIVE  : return array("".TB_PREF."wo_manufacture", null, "id", "reference", "date_");
        case     ST_SALESORDER   : return array("".TB_PREF."sales_orders", "trans_type", "order_no", "reference", "ord_date");
        case     31              : return array("".TB_PREF."service_orders", null, "order_no", "cust_ref", "date");
        case     ST_SALESQUOTE   : return array("".TB_PREF."sales_orders", "trans_type", "order_no", "reference", "ord_date");
        case	 ST_DIMENSION    : return array("".TB_PREF."dimensions", null, "id", "reference", "date_");
        case     ST_COSTUPDATE   : return null;
	}

	display_db_error("invalid type ($type) sent to get_systype_db_info", "", true);
}

function get_systypes()
{
	$sql = "SELECT * FROM ".TB_PREF."sys_types";
	$result = db_query($sql, "could not query systypes table");
	return $result;
}

?>