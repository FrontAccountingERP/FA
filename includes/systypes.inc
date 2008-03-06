<?php

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
        case      0 : return array("".TB_PREF."gl_trans", "type", "type_no", null, "tran_date");
        case      1 : return array("".TB_PREF."bank_trans", "type", "trans_no", "ref", "trans_date");
        case      2 : return array("".TB_PREF."bank_trans", "type", "trans_no", "ref", "trans_date");
        case      3 : return null;
        case      4 : return array("".TB_PREF."bank_trans", "type", "trans_no", "ref", "trans_date");
        case     10 : return array("".TB_PREF."debtor_trans", "type", "trans_no", "reference", "tran_date");
        case     11 : return array("".TB_PREF."debtor_trans", "type", "trans_no", "reference", "tran_date");
        case     12 : return array("".TB_PREF."debtor_trans", "type", "trans_no", "reference", "tran_date");
        case     13 : return array("".TB_PREF."debtor_trans", "type", "trans_no", "reference", "tran_date");
        case     16 : return array("".TB_PREF."stock_moves", "type", "trans_no", "reference", "tran_date");
        case     17 : return array("".TB_PREF."stock_moves", "type", "trans_no", "reference", "tran_date");
        case     18 : return array("".TB_PREF."purch_orders", null, "order_no", "reference", "tran_date");
        case     20 : return array("".TB_PREF."supp_trans", "type", "trans_no", "reference", "tran_date");
        case     21 : return array("".TB_PREF."supp_trans", "type", "trans_no", "reference", "tran_date");
        case     22 : return array("".TB_PREF."supp_trans", "type", "trans_no", "reference", "tran_date");
        case     25 : return array("".TB_PREF."grn_batch", null, "id", "reference", "delivery_date");
        case     26 : return array("".TB_PREF."workorders", null, "id", "wo_ref", "released_date");
        case     28 : return array("".TB_PREF."wo_issues", null, "issue_no", "reference", "issue_date");
        case     29 : return array("".TB_PREF."wo_manufacture", null, "id", "reference", "date_");
        case     30 : return array("".TB_PREF."sales_orders", null, "order_no", "customer_ref", "ord_date");
        case	 40 : return array("".TB_PREF."dimensions", null, "id", "reference", "date_");
        case     35 : return null;
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