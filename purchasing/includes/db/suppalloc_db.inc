<?php

//----------------------------------------------------------------------------------------

function add_supp_allocation($amount, $trans_type_from, $trans_no_from, 
	$trans_type_to, $trans_no_to, $date_)
{
	$date = date2sql($date_);
	$sql = "INSERT INTO ".TB_PREF."supp_allocations (
		amt, date_alloc, 
		trans_type_from, trans_no_from, trans_no_to, trans_type_to) 
		VALUES ($amount, '$date', $trans_type_from, $trans_no_from, $trans_no_to, $trans_type_to)";
			
	db_query($sql, "A supplier allocation could not be added to the database");
}

//----------------------------------------------------------------------------------------


function delete_supp_allocation($trans_id)
{
	$sql = "DELETE FROM ".TB_PREF."supp_allocations WHERE id = " . $trans_id;
	db_query($sql, "The existing allocation $trans_id could not be deleted");
}

//----------------------------------------------------------------------------------------

function get_supp_trans_allocation_balance($trans_type, $trans_no)
{
	$sql = "SELECT (ov_amount+ov_gst-ov_discount-alloc) AS BalToAllocate 
		FROM ".TB_PREF."supp_trans WHERE trans_no=$trans_no AND type=$trans_type";
	$result = db_query($sql,"calculate the allocation");
	$myrow = db_fetch_row($result);	
	
	return $myrow[0];
}

//----------------------------------------------------------------------------------------

function update_supp_trans_allocation($trans_type, $trans_no, $alloc)
{
	$sql = "UPDATE ".TB_PREF."supp_trans SET alloc = alloc + $alloc
		WHERE type=$trans_type AND trans_no = $trans_no";
	db_query($sql, "The supp transaction record could not be modified for the allocation against it");
}

//-------------------------------------------------------------------------------------------------------------

function void_supp_allocations($type, $type_no)
{
	return clear_supp_alloctions($type, $type_no);
}

//-------------------------------------------------------------------------------------------------------------

function clear_supp_alloctions($type, $type_no)
{
	// clear any allocations for this transaction
	$sql = "SELECT * FROM ".TB_PREF."supp_allocations 
		WHERE (trans_type_from=$type AND trans_no_from=$type_no) 
		OR (trans_type_to=$type AND trans_no_to=$type_no)";
	$result = db_query($sql, "could not void supp transactions for type=$type and trans_no=$type_no");
	
	while ($row = db_fetch($result))
	{
		$sql = "UPDATE ".TB_PREF."supp_trans SET alloc=alloc - " . $row['amt'] . "
			WHERE (type= " . $row['trans_type_from'] . " AND trans_no=" . $row['trans_no_from'] . ") 
			OR (type=" . $row['trans_type_to'] . " AND trans_no=" . $row['trans_no_to'] . ")";
		//$sql = "UPDATE ".TB_PREF."supp_trans SET alloc=alloc - " . $row['amt'] . "
		//	WHERE type=" . $row['trans_type_to'] . " AND trans_no=" . $row['trans_no_to'];
		db_query($sql, "could not clear allocation");						
	}
						

	// remove any allocations for this transaction
	$sql = "DELETE FROM ".TB_PREF."supp_allocations 
		WHERE (trans_type_from=$type AND trans_no_from=$type_no) 
		OR (trans_type_to=$type AND trans_no_to=$type_no)";
				
	db_query($sql, "could not void supp transactions for type=$type and trans_no=$type_no");					
}

//-------------------------------------------------------------------------------------------------------------

function get_allocatable_from_supp_transactions($supplier_id, $settled)
{
	$settled_sql = "";
	if (!$settled) 
	{
		$settled_sql = "AND round(ABS(ov_amount+ov_gst+ov_discount)-alloc,6) > 0";
	}
	
	$supp_sql = "";
	if ($supplier_id != null)
		$supp_sql = " AND ".TB_PREF."supp_trans.supplier_id = $supplier_id";		
	
	return get_supplier_transactions("round(ABS(ov_amount+ov_gst+ov_discount)-alloc,6) <= 0 AS settled", 
		"(type=22 OR type=21 OR type=1) AND (ov_amount < 0) " . $settled_sql . $supp_sql);	
}

//-------------------------------------------------------------------------------------------------------------

function get_allocatable_to_supp_transactions($supplier_id, $trans_no=null, $type=null)
{	
	if ($trans_no != null && $type!= null) 
	{
		return get_supplier_transactions("amt", "".TB_PREF."supp_trans.trans_no = ".TB_PREF."supp_allocations.trans_no_to 
			AND ".TB_PREF."supp_trans.type = ".TB_PREF."supp_allocations.trans_type_to
			AND ".TB_PREF."supp_allocations.trans_no_from=$trans_no
			AND ".TB_PREF."supp_allocations.trans_type_from=$type 
			AND ".TB_PREF."supp_trans.supplier_id=$supplier_id", 
			"".TB_PREF."supp_allocations");						
	} 
	else 
	{
		return get_supplier_transactions(null, "round(ABS(ov_amount+ov_gst+ov_discount)-alloc,6) > 0
			AND ".TB_PREF."supp_trans.type != 22
			AND ".TB_PREF."supp_trans.supplier_id=$supplier_id");
	}															
}


?>