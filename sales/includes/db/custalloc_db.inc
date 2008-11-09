<?php

//----------------------------------------------------------------------------------------

function add_cust_allocation($amount, $trans_type_from, $trans_no_from,
	$trans_type_to, $trans_no_to)
{
	$sql = "INSERT INTO ".TB_PREF."cust_allocations (
		amt, date_alloc,
		trans_type_from, trans_no_from, trans_no_to, trans_type_to)
		VALUES ($amount, Now(), $trans_type_from, $trans_no_from, $trans_no_to, $trans_type_to)";

	db_query($sql, "A customer allocation could not be added to the database");
}

//----------------------------------------------------------------------------------------


function delete_cust_allocation($trans_id)
{
	$sql = "DELETE FROM ".TB_PREF."cust_allocations WHERE id = " . $trans_id;
	return db_query($sql, "The existing allocation $trans_id could not be deleted");
}

//----------------------------------------------------------------------------------------

function get_DebtorTrans_allocation_balance($trans_type, $trans_no)
{

	$sql = "SELECT (ov_amount+ov_gst+ov_freight+ov_freight_tax-ov_discount-alloc) AS BalToAllocate
		FROM ".TB_PREF."debtor_trans WHERE trans_no=$trans_no AND type=$trans_type";
	$result = db_query($sql,"calculate the allocation");
	$myrow = db_fetch_row($result);

	return $myrow[0];
}

//----------------------------------------------------------------------------------------

function update_debtor_trans_allocation($trans_type, $trans_no, $alloc)
{
	$sql = "UPDATE ".TB_PREF."debtor_trans SET alloc = alloc + $alloc
		WHERE type=$trans_type AND trans_no = $trans_no";
	db_query($sql, "The debtor transaction record could not be modified for the allocation against it");
}

//-------------------------------------------------------------------------------------------------------------

function void_cust_allocations($type, $type_no, $date="")
{
	return clear_cust_alloctions($type, $type_no, $date);
}

//-------------------------------------------------------------------------------------------------------------

function clear_cust_alloctions($type, $type_no, $date="")
{
	// clear any allocations for this transaction
	$sql = "SELECT * FROM ".TB_PREF."cust_allocations
		WHERE (trans_type_from=$type AND trans_no_from=$type_no)
		OR (trans_type_to=$type AND trans_no_to=$type_no)";
	$result = db_query($sql, "could not void debtor transactions for type=$type and trans_no=$type_no");

	while ($row = db_fetch($result))
	{
		$sql = "UPDATE ".TB_PREF."debtor_trans SET alloc=alloc - " . $row['amt'] . "
			WHERE (type= " . $row['trans_type_from'] . " AND trans_no=" . $row['trans_no_from'] . ")
			OR (type=" . $row['trans_type_to'] . " AND trans_no=" . $row['trans_no_to'] . ")";
		db_query($sql, "could not clear allocation");
		// 2008-09-20 Joe Hunt
		if ($date != "")
			exchange_variation($type, $type_no, $row['trans_type_to'], $row['trans_no_to'], $date,
				$row['amt'], payment_person_types::customer(), true);
		//////////////////////
	}


	// remove any allocations for this transaction
	$sql = "DELETE FROM ".TB_PREF."cust_allocations
		WHERE (trans_type_from=$type AND trans_no_from=$type_no)
		OR (trans_type_to=$type AND trans_no_to=$type_no)";

	db_query($sql, "could not void debtor transactions for type=$type and trans_no=$type_no");
}
//----------------------------------------------------------------------------------------

function get_customer_transactions($extra_fields=null, $extra_conditions=null, $extra_tables=null)
{
	$sql = "SELECT trans.trans_no,
		trans.type,
		trans.tran_date,
		trans.due_date,
		trans.reference,
		trans.alloc,
		ov_amount+ov_gst+ov_freight+ov_freight_tax+ov_discount AS Total,
		debtor.name AS DebtorName, debtor.address,
		debtor.curr_code, 
		trans.version ";

	if ($extra_fields)
		$sql .= ", $extra_fields ";

	$sql .= " FROM ".TB_PREF."debtor_trans as trans, "
				.TB_PREF."debtors_master as debtor";
	if ($extra_tables)
		$sql .= ",$extra_tables ";

	$sql .= " WHERE trans.debtor_no=debtor.debtor_no";

	if ($extra_conditions)
		$sql .= " AND $extra_conditions ";

	$sql .= " ORDER BY trans_no";

	return db_query($sql, "Cannot retreive debtor transactions");
}


//-------------------------------------------------------------------------------------------------------------

function get_allocatable_from_cust_transactions($customer_id, $settled)
{
	$settled_sql = "";
	if (!$settled)
	{
		$settled_sql = " AND (round(ov_amount+ov_gst+ov_freight+ov_freight_tax-ov_discount-alloc,6) > 0)";
	}
	$cust_sql = "";
	if ($customer_id != null)
		$cust_sql = " AND trans.debtor_no = $customer_id";

	return get_customer_transactions("round(ov_amount+ov_gst+ov_freight+ov_freight_tax+ov_discount-alloc,6) <= 0 AS settled",
		"(type=12 OR type=11 OR type=2) AND (trans.ov_amount > 0) " . $settled_sql . $cust_sql);
}

//-------------------------------------------------------------------------------------------------------------

function get_allocatable_to_cust_transactions($customer_id, $trans_no=null, $type=null)
{
	if ($trans_no != null and $type != null)
	{
		return get_customer_transactions("amt", "trans.trans_no = alloc.trans_no_to
			AND trans.type = alloc.trans_type_to
			AND alloc.trans_no_from=$trans_no
			AND alloc.trans_type_from=$type
			AND trans.debtor_no=$customer_id",
			"".TB_PREF."cust_allocations as alloc");
	}
	else
	{
		return get_customer_transactions(null, "round(ov_amount+ov_gst+ov_freight+ov_freight_tax+ov_discount-alloc,6) > 0
			AND trans.type != " . systypes::cust_payment() . "
			AND trans.type != " . systypes::bank_deposit() . "
			AND trans.type != 11
			AND trans.type != 13
			AND trans.debtor_no=$customer_id");
	}
}


?>