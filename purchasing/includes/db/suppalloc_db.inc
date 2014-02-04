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
//----------------------------------------------------------------------------------------

function add_supp_allocation($amount, $trans_type_from, $trans_no_from,
	$trans_type_to, $trans_no_to, $date_=null)
{
	if (!$date_)
		$date = Today();

	$date = date2sql($date_);
	$sql = "INSERT INTO ".TB_PREF."supp_allocations (
		amt, date_alloc,
		trans_type_from, trans_no_from, trans_no_to, trans_type_to)
		VALUES (".db_escape($amount).", '$date', "
		.db_escape($trans_type_from).", ".db_escape($trans_no_from).", "
		.db_escape($trans_no_to).", ".db_escape($trans_type_to).")";

	db_query($sql, "A supplier allocation could not be added to the database");
}

//----------------------------------------------------------------------------------------


function delete_supp_allocation($trans_id)
{
	$sql = "DELETE FROM ".TB_PREF."supp_allocations WHERE id = ".db_escape($trans_id);
	db_query($sql, "The existing allocation $trans_id could not be deleted");
}

//----------------------------------------------------------------------------------------

function get_supp_trans_allocation_balance($trans_type, $trans_no)
{
	$sql = "SELECT (ov_amount+ov_gst-ov_discount-alloc) AS BalToAllocate
		FROM ".TB_PREF."supp_trans WHERE trans_no="
		.db_escape($trans_no)." AND type=".db_escape($trans_type);
	$result = db_query($sql,"calculate the allocation");
	$myrow = db_fetch_row($result);

	return $myrow[0];
}

//----------------------------------------------------------------------------------------
//	Update supplier trans alloc field according to current status of supp_allocations
//
function update_supp_trans_allocation($trans_type, $trans_no)
{
	$sql = "UPDATE `".TB_PREF.($trans_type==ST_PURCHORDER ? 'purch_orders' : 'supp_trans')."` trans,
			(SELECT sum(amt) amt from ".TB_PREF."supp_allocations
				WHERE (trans_type_to=".db_escape($trans_type)." AND trans_no_to=".db_escape($trans_no).")
				OR (trans_type_from=".db_escape($trans_type)." AND trans_no_from=".db_escape($trans_no).")) allocated
		SET 
			trans.alloc=IFNULL(allocated.amt, 0)
		WHERE "	. ($trans_type==ST_PURCHORDER ? 
			  "trans.order_no=".db_escape($trans_no)
			: "trans.type=".db_escape($trans_type)." AND trans.trans_no=".db_escape($trans_no));

	db_query($sql, "The supp transaction record could not be modified for the allocation against it");
}


//-------------------------------------------------------------------------------------------------------------

function void_supp_allocations($type, $type_no, $date="")
{
	return clear_supp_alloctions($type, $type_no, $date);
}

//-------------------------------------------------------------------------------------------------------------

function clear_supp_alloctions($type, $type_no, $date="")
{
	// clear any allocations for this transaction
	$sql = "SELECT * FROM ".TB_PREF."supp_allocations
		WHERE (trans_type_from=".db_escape($type)." AND trans_no_from=".db_escape($type_no).")
		OR (trans_type_to=".db_escape($type)." AND trans_no_to=".db_escape($type_no).")";
	$result = db_query($sql, "could not void supp transactions for type=$type and trans_no=$type_no");

	while ($row = db_fetch($result))
	{
		$sql = "UPDATE ".TB_PREF."supp_trans SET alloc=alloc - " . $row['amt'] . "
			WHERE (type= " . $row['trans_type_from'] . " AND trans_no=" . $row['trans_no_from'] . ")
			OR (type=" . $row['trans_type_to'] . " AND trans_no=" . $row['trans_no_to'] . ")";
		db_query($sql, "could not clear allocation");

		// 2008-09-20 Joe Hunt
		if ($date != "")
			exchange_variation($type, $type_no, $row['trans_type_to'], $row['trans_no_to'], $date,
				$row['amt'], PT_SUPPLIER, true);
		// 2014-01-16 Joe Hunt. Fix for voiding allocated currency supp_invoices.		
		elseif ($type == ST_SUPPINVOICE && $row['trans_type_to'] == $type && $row['trans_no_to'] == $type_no)		
			exchange_variation($row['trans_type_from'], $row['trans_no_from'], $row['trans_type_to'], $row['trans_no_to'], 
				sql2date($row['date_alloc']), $row['amt'], PT_SUPPLIER, true);
		//////////////////////
	}


	// remove any allocations for this transaction
	$sql = "DELETE FROM ".TB_PREF."supp_allocations
		WHERE (trans_type_from=".db_escape($type)." AND trans_no_from=".db_escape($type_no).")
		OR (trans_type_to=".db_escape($type)." AND trans_no_to=".db_escape($type_no).")";

	db_query($sql, "could not void supp transactions for type=$type and trans_no=$type_no");
}

//----------------------------------------------------------------------------------------
function get_alloc_supp_sql($extra_fields=null, $extra_conditions=null, $extra_tables=null)
{
	$sql = "SELECT
		trans.type,
		trans.trans_no,
		IF(trans.supp_reference='',trans.reference,trans.supp_reference) as reference,
 		trans.tran_date,
		supplier.supp_name, 
		supplier.curr_code, 
		ov_amount+ov_gst+ov_discount AS Total,
		trans.alloc,
		trans.due_date,
		trans.supplier_id,
		supplier.address";

	if ($extra_fields)
		$sql .= ", $extra_fields ";

	$sql .= " FROM ".TB_PREF."supp_trans as trans, ".TB_PREF."suppliers as supplier";
	if ($extra_tables)
		$sql .= " ,$extra_tables ";

	$sql .= " WHERE trans.supplier_id=supplier.supplier_id";

	if ($extra_conditions)
		$sql .= " AND $extra_conditions";
	
	return $sql;
}


//-------------------------------------------------------------------------------------------------------------

function get_allocatable_from_supp_sql($supplier_id, $settled)
{
	$settled_sql = "";
	if (!$settled)
	{
		$settled_sql = "AND round(ABS(ov_amount+ov_gst+ov_discount)-alloc,6) > 0";
	}

	$supp_sql = "";
	if ($supplier_id != null)
		$supp_sql = " AND trans.supplier_id = ".db_escape($supplier_id);

	$sql = get_alloc_supp_sql("round(ABS(ov_amount+ov_gst+ov_discount)-alloc,6) <= 0 AS settled",
		"(type=".ST_SUPPAYMENT." OR type=".ST_SUPPCREDIT." OR type=".ST_BANKPAYMENT.") AND (ov_amount < 0) " . $settled_sql . $supp_sql);

	return $sql;
}


//-------------------------------------------------------------------------------------------------------------

function get_allocatable_to_supp_transactions($supplier_id, $trans_no=null, $type=null)
{
	if ($trans_no != null && $type!= null)
	{
		$sql = get_alloc_supp_sql("amt, supp_reference", "trans.trans_no = alloc.trans_no_to
			AND trans.type = alloc.trans_type_to
			AND alloc.trans_no_from=".db_escape($trans_no)."
			AND alloc.trans_type_from=".db_escape($type)."
			AND trans.supplier_id=".db_escape($supplier_id),
			TB_PREF."supp_allocations as alloc");
	}
	else
	{
		$sql = get_alloc_supp_sql(null, "round(ABS(ov_amount+ov_gst+ov_discount)-alloc,6) > 0
			AND trans.type NOT IN (".implode(',',array(ST_SUPPAYMENT, ST_BANKPAYMENT, ST_SUPPCREDIT)).")
			AND trans.supplier_id=".db_escape($supplier_id));
	}

	return db_query($sql." ORDER BY due_date", "Cannot retreive alloc to transactions");
}

//-------------------------------------------------------------------------------------------------------------

function get_allocatable_from_supp_transactions($supplier_id, $trans_no=null, $type=null)
{
	if ($trans_no != null && $type!= null)
	{
		$sql = get_alloc_supp_sql("amt, supp_reference", "trans.trans_no = alloc.trans_no_from
			AND trans.type = alloc.trans_type_from
			AND alloc.trans_no_to=".db_escape($trans_no)."
			AND alloc.trans_type_to=".db_escape($type)."
			AND trans.supplier_id=".db_escape($supplier_id),
			TB_PREF."supp_allocations as alloc");
	}
	else
	{
		$sql = get_alloc_supp_sql(null, "round(ABS(ov_amount+ov_gst+ov_discount)-alloc,6) > 0
			AND trans.type != ".ST_SUPPAYMENT."
			AND trans.supplier_id=".db_escape($supplier_id));
	}

	return db_query($sql." ORDER BY due_date", "Cannot retreive alloc to transactions");
}

function get_sql_for_supplier_allocation_inquiry()
{
 	$date_after = date2sql($_POST['TransAfterDate']);
 	$date_to = date2sql($_POST['TransToDate']);

    $sql = "SELECT 
		trans.type, 
		trans.trans_no,
		trans.reference, 
		supplier.supp_name, 
		trans.supp_reference,
    	trans.tran_date, 
		trans.due_date,
		supplier.curr_code, 
    	(trans.ov_amount + trans.ov_gst  + trans.ov_discount) AS TotalAmount, 
		trans.alloc AS Allocated,
		((trans.type = ".ST_SUPPINVOICE." OR trans.type = ".ST_SUPPCREDIT.") AND trans.due_date < '" . date2sql(Today()) . "') AS OverDue,
		trans.supplier_id
    	FROM "
			.TB_PREF."supp_trans as trans, "
			.TB_PREF."suppliers as supplier
    	WHERE supplier.supplier_id = trans.supplier_id
     	AND trans.tran_date >= '$date_after'
    	AND trans.tran_date <= '$date_to'";

   	if ($_POST['supplier_id'] != ALL_TEXT)
   		$sql .= " AND trans.supplier_id = ".db_escape($_POST['supplier_id']);
   	if (isset($_POST['filterType']) && $_POST['filterType'] != ALL_TEXT)
   	{
   		if (($_POST['filterType'] == '1') || ($_POST['filterType'] == '2'))
   		{
   			$sql .= " AND trans.type = ".ST_SUPPINVOICE." ";
   		}
   		elseif ($_POST['filterType'] == '3')
   		{
			$sql .= " AND trans.type = ".ST_SUPPAYMENT." ";
   		}
   		elseif (($_POST['filterType'] == '4') || ($_POST['filterType'] == '5'))
   		{
			$sql .= " AND trans.type = ".ST_SUPPCREDIT." ";
   		}

   		if (($_POST['filterType'] == '2') || ($_POST['filterType'] == '5'))
   		{
   			$today =  date2sql(Today());
			$sql .= " AND trans.due_date < '$today' ";
   		}
   	}

   	if (!check_value('showSettled'))
   	{
   		$sql .= " AND (round(abs(ov_amount + ov_gst + ov_discount) - alloc,6) != 0) ";
   	}
   	return $sql;
}
?>