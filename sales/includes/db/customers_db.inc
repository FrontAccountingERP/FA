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
function get_customer_details($customer_id, $to=null)
{

	if ($to == null)
		$todate = date("Y-m-d");
	else
		$todate = date2sql($to);
	$past1 = get_company_pref('past_due_days');
	$past2 = 2 * $past1;
	// removed - debtor_trans.alloc from all summations

    $value = "IF(".TB_PREF."debtor_trans.type=11 OR ".TB_PREF."debtor_trans.type=12 OR ".TB_PREF."debtor_trans.type=2,
	-1, 1) *".
      "(".TB_PREF."debtor_trans.ov_amount + ".TB_PREF."debtor_trans.ov_gst + "
		.TB_PREF."debtor_trans.ov_freight + ".TB_PREF."debtor_trans.ov_freight_tax + "
		.TB_PREF."debtor_trans.ov_discount)";
	$due = "IF (".TB_PREF."debtor_trans.type=10,".TB_PREF."debtor_trans.due_date,".TB_PREF."debtor_trans.tran_date)";
    $sql = "SELECT ".TB_PREF."debtors_master.name, ".TB_PREF."debtors_master.curr_code, ".TB_PREF."payment_terms.terms,
		".TB_PREF."debtors_master.credit_limit, ".TB_PREF."credit_status.dissallow_invoices, ".TB_PREF."credit_status.reason_description,

		Sum(".$value.") AS Balance,

		Sum(IF ((TO_DAYS('$todate') - TO_DAYS($due)) >= 0,$value,0)) AS Due,
		Sum(IF ((TO_DAYS('$todate') - TO_DAYS($due)) >= $past1,$value,0)) AS Overdue1,
		Sum(IF ((TO_DAYS('$todate') - TO_DAYS($due)) >= $past2,$value,0)) AS Overdue2

		FROM ".TB_PREF."debtors_master,
			 ".TB_PREF."payment_terms,
			 ".TB_PREF."credit_status,
			 ".TB_PREF."debtor_trans

		WHERE
			 ".TB_PREF."debtors_master.payment_terms = ".TB_PREF."payment_terms.terms_indicator
			 AND ".TB_PREF."debtors_master.credit_status = ".TB_PREF."credit_status.id
			 AND ".TB_PREF."debtors_master.debtor_no = ".db_escape($customer_id)."
			 AND ".TB_PREF."debtor_trans.tran_date <= '$todate'
			 AND ".TB_PREF."debtor_trans.type <> 13
			 AND ".TB_PREF."debtors_master.debtor_no = ".TB_PREF."debtor_trans.debtor_no

		GROUP BY
			  ".TB_PREF."debtors_master.name,
			  ".TB_PREF."payment_terms.terms,
			  ".TB_PREF."payment_terms.days_before_due,
			  ".TB_PREF."payment_terms.day_in_following_month,
			  ".TB_PREF."debtors_master.credit_limit,
			  ".TB_PREF."credit_status.dissallow_invoices,
			  ".TB_PREF."credit_status.reason_description";
    $result = db_query($sql,"The customer details could not be retrieved");

    if (db_num_rows($result) == 0)
    {

    	/*Because there is no balance - so just retrieve the header information about the customer - the choice is do one query to get the balance and transactions for those customers who have a balance and two queries for those who don't have a balance OR always do two queries - I opted for the former */

    	$nil_balance = true;

    	$sql = "SELECT ".TB_PREF."debtors_master.name, ".TB_PREF."debtors_master.curr_code, ".TB_PREF."debtors_master.debtor_no,  ".TB_PREF."payment_terms.terms,
    		".TB_PREF."debtors_master.credit_limit, ".TB_PREF."credit_status.dissallow_invoices, ".TB_PREF."credit_status.reason_description
    		FROM ".TB_PREF."debtors_master,
    		     ".TB_PREF."payment_terms,
    		     ".TB_PREF."credit_status

    		WHERE
    		     ".TB_PREF."debtors_master.payment_terms = ".TB_PREF."payment_terms.terms_indicator
    		     AND ".TB_PREF."debtors_master.credit_status = ".TB_PREF."credit_status.id
    		     AND ".TB_PREF."debtors_master.debtor_no = ".db_escape($customer_id);

    	$result = db_query($sql,"The customer details could not be retrieved");

    }
    else
    {
    	$nil_balance = false;
    }

    $customer_record = db_fetch($result);

    if ($nil_balance == true)
    {
    	$customer_record["Balance"] = 0;
    	$customer_record["Due"] = 0;
    	$customer_record["Overdue1"] = 0;
    	$customer_record["Overdue2"] = 0;
    }

    return $customer_record;

}

function get_customer($customer_id)
{
	$sql = "SELECT * FROM ".TB_PREF."debtors_master WHERE debtor_no=".db_escape($customer_id);

	$result = db_query($sql, "could not get customer");

	return db_fetch($result);
}

function get_customer_name($customer_id)
{
	$sql = "SELECT name FROM ".TB_PREF."debtors_master WHERE debtor_no=".db_escape($customer_id);

	$result = db_query($sql, "could not get customer");

	$row = db_fetch_row($result);

	return $row[0];
}

function get_area_name($id)
{
	$sql = "SELECT description FROM ".TB_PREF."areas WHERE area_code=".db_escape($id);

	$result = db_query($sql, "could not get sales type");

	$row = db_fetch_row($result);
	return $row[0];
}

function get_salesman_name($id)
{
	$sql = "SELECT salesman_name FROM ".TB_PREF."salesman WHERE salesman_code=".db_escape($id);

	$result = db_query($sql, "could not get sales type");

	$row = db_fetch_row($result);
	return $row[0];
}




?>