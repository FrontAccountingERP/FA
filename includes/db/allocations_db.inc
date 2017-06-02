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
//
// Functions below are unified interface to allocations (should supersede currently separate cust/supp allocations in a future)
//
//----------------------------------------------------------------------------------------
//
//	Returns table of payments currently allocated to selected transaction.
//
function get_payments_for($trans_no, $trans_type)
{
	$allocations = array();

	$sql = "(SELECT * FROM ".TB_PREF."cust_allocations
			WHERE (trans_type_to=".db_escape($trans_type)." AND trans_no_to=".db_escape($trans_no)."))
		UNION
			(SELECT * FROM ".TB_PREF."supp_allocations
			WHERE (trans_type_to=".db_escape($trans_type)." AND trans_no_to=".db_escape($trans_no)."))";
	$result = db_query($sql, "error reading current allocations");

	while($dat = db_fetch($result))
	{
		$allocations[] = $dat;
	}

	return $allocations;
}

//
//	Unified inteface to (re)allocate payments to supp/cust/transaction
//
function allocate_payment($type_from, $no_from, $type_to, $no_to, $amount, $date)
{
	$date = date2sql($date); // FIXME types
	if (in_array($type_to, array(ST_SALESORDER, ST_CUSTCREDIT, ST_CUSTDELIVERY, ST_SALESINVOICE)))
	{
		$allocations = 'cust_allocations';
		$payments='debtor_trans';
		$transactions = $type_to == ST_SALESORDER ? 'sales_orders' : 'debtor_trans';
		$where = $type_to == ST_SALESORDER ? "order_no=".db_escape($no_to) : "type=".db_escape($type_to)." AND trans_no=".db_escape($no_to);
	}
	else
	{
		$allocations = 'supp_allocations';
		$payments='supp_trans';
		$transactions = $type_to == ST_PURCHORDER ? 'purch_orders' : 'supp_trans';
		$where = $type_to == ST_PURCHORDER ? "order_no=".db_escape($no_to) : "type=".db_escape($type_to)." AND trans_no=".db_escape($no_to);
	}

	// add/update/delete allocation
	if (floatcmp($amount, 0) != 0)
	{
		$sql = "REPLACE ".TB_PREF.$allocations." SET amt = ". db_escape($amount).",date_alloc='$date'"
			.",trans_type_from=".db_escape($type_from).",trans_no_from=".db_escape($no_from)
		   	.",trans_type_to=".db_escape($type_to).",trans_no_to=".db_escape($no_to);
	}
	else {
		$sql = "DELETE FROM ".TB_PREF.$allocations."
			WHERE trans_type_from=".db_escape($type_from)." AND trans_no_from=".db_escape($no_from)
	   	." AND trans_type_to=".db_escape($type_to)." AND trans_no_to=".db_escape($no_to);
	}
	db_query($sql, "The existing allocations could not be updated");

	// referesh allocation summaries
	$sql = "UPDATE ".TB_PREF.$transactions." trans,
				(SELECT SUM(amt) amt FROM ".TB_PREF.$allocations." WHERE trans_type_to=".db_escape($type_to)." AND trans_no_to=".db_escape($no_to).") paym
	 		 SET alloc = paym.amt
			 WHERE $where";
	db_query($sql, "The transaction allocations could not be updated");

	$sql = "UPDATE ".TB_PREF.$payments." trans,
				(SELECT SUM(amt) amt FROM ".TB_PREF.$allocations." WHERE trans_type_from=".db_escape($type_from)." AND trans_no_from=".db_escape($no_from).") paym
	 		 SET alloc = paym.amt
			 WHERE type=".db_escape($type_from)." AND trans_no=".db_escape($no_from);

	return db_query($sql, "The payment allocations could not be updated");
}

//----------------------------------------------------------------------------------------
//
//	Reallocates allocations to selected transaction.
//  $allocs is table of payments which should be reallocated
//  $free_remainder should be true if old allacations should be freed when not allocated to new transaction.
//
function reallocate_payments($trans_no, $trans_type, $alloc_date, $max_alloc, $allocs, $free_remainder=true)
{

	foreach($allocs as $alloc)
	{
		$amount = min($alloc['amt'], $max_alloc);
		$remainder = $alloc['amt'] - $amount;

		$max_alloc = floatcmp($max_alloc, $amount) > 0 ? $max_alloc-$amount : 0;

		$same_to =  $trans_type == $alloc['trans_type_to'] && $trans_no == $alloc['trans_no_to'];
		if (!$same_to || ($remainder > 0))
		{
			allocate_payment($alloc['trans_type_from'], $alloc['trans_no_from'], $trans_type, $trans_no, $amount, $alloc_date);
		}
		if (!$same_to && ($remainder > 0 || $free_remainder))
		{
			allocate_payment($alloc['trans_type_from'], $alloc['trans_no_from'], 
				$alloc['trans_type_to'], $alloc['trans_no_to'], $free_remainder ? 0 : $remainder, $alloc_date);
		}
		if (!$free_remainder && $max_alloc==0)
			break;
	}
}
