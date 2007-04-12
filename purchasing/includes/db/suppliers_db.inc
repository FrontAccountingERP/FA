<?php

function get_supplier_details($supplier_id, $to=null)
{
	
	if ($to == null)
		$todate = date("Y-m-d");
	else	
		$todate = date2sql($to);
	$past1 = get_company_pref('past_due_days');
	$past2 = 2 * $past1;
	// removed - supp_trans.alloc from all summations
	
    $sql = "SELECT ".TB_PREF."suppliers.supp_name, ".TB_PREF."suppliers.curr_code, ".TB_PREF."payment_terms.terms,
    
		Sum(".TB_PREF."supp_trans.ov_amount + ".TB_PREF."supp_trans.ov_gst + ".TB_PREF."supp_trans.ov_discount) AS Balance,

		Sum(IF (".TB_PREF."payment_terms.days_before_due > 0,
			CASE WHEN (TO_DAYS('$todate') - TO_DAYS(".TB_PREF."supp_trans.tran_date)) >= ".TB_PREF."payment_terms.days_before_due 
			THEN 
				".TB_PREF."supp_trans.ov_amount + ".TB_PREF."supp_trans.ov_gst + ".TB_PREF."supp_trans.ov_discount
			ELSE 
				0 
			END,

			CASE WHEN TO_DAYS('$todate') - TO_DAYS(DATE_ADD(DATE_ADD(".TB_PREF."supp_trans.tran_date,
				INTERVAL 1 MONTH), INTERVAL (".TB_PREF."payment_terms.day_in_following_month -
				DAYOFMONTH(".TB_PREF."supp_trans.tran_date)) DAY)) >= 0 
			THEN 
				".TB_PREF."supp_trans.ov_amount + ".TB_PREF."supp_trans.ov_gst + ".TB_PREF."supp_trans.ov_discount 
			ELSE 
				0 
			END

		)) AS Due,

		Sum(IF (".TB_PREF."payment_terms.days_before_due > 0,
			CASE WHEN TO_DAYS('$todate') - TO_DAYS(".TB_PREF."supp_trans.tran_date) > ".TB_PREF."payment_terms.days_before_due
				AND TO_DAYS('$todate') - TO_DAYS(".TB_PREF."supp_trans.tran_date) >= (".TB_PREF."payment_terms.days_before_due + $past1) 
			THEN 
				".TB_PREF."supp_trans.ov_amount + ".TB_PREF."supp_trans.ov_gst + ".TB_PREF."supp_trans.ov_discount 
			ELSE 
				0 
			END,

			CASE WHEN (TO_DAYS('$todate') - TO_DAYS(DATE_ADD(DATE_ADD(".TB_PREF."supp_trans.tran_date,
				INTERVAL 1  MONTH), INTERVAL (".TB_PREF."payment_terms.day_in_following_month -
				DAYOFMONTH(".TB_PREF."supp_trans.tran_date)) DAY)) >= $past1) 
			THEN 
				".TB_PREF."supp_trans.ov_amount + ".TB_PREF."supp_trans.ov_gst + ".TB_PREF."supp_trans.ov_discount
			ELSE 
				0 
			END

		)) AS Overdue1,

		Sum(IF (".TB_PREF."payment_terms.days_before_due > 0,
			CASE WHEN TO_DAYS('$todate') - TO_DAYS(".TB_PREF."supp_trans.tran_date) > ".TB_PREF."payment_terms.days_before_due
				AND TO_DAYS('$todate') - TO_DAYS(".TB_PREF."supp_trans.tran_date) >= (".TB_PREF."payment_terms.days_before_due + $past2) 
			THEN 
				".TB_PREF."supp_trans.ov_amount + ".TB_PREF."supp_trans.ov_gst + ".TB_PREF."supp_trans.ov_discount 
			ELSE 
				0 
			END,

			CASE WHEN (TO_DAYS('$todate') - TO_DAYS(DATE_ADD(DATE_ADD(".TB_PREF."supp_trans.tran_date,
				INTERVAL 1  MONTH), INTERVAL (".TB_PREF."payment_terms.day_in_following_month - 
				DAYOFMONTH(".TB_PREF."supp_trans.tran_date)) DAY)) >= $past2) 
			THEN 
				".TB_PREF."supp_trans.ov_amount + ".TB_PREF."supp_trans.ov_gst + ".TB_PREF."supp_trans.ov_discount
			ELSE 
				0 
			END

		)) AS Overdue2

		FROM ".TB_PREF."suppliers,
			 ".TB_PREF."payment_terms,
			 ".TB_PREF."supp_trans

		WHERE
			 ".TB_PREF."suppliers.payment_terms = ".TB_PREF."payment_terms.terms_indicator
			 AND ".TB_PREF."suppliers.supplier_id = $supplier_id
			 AND ".TB_PREF."supp_trans.tran_date <= '$todate'
			 AND ".TB_PREF."suppliers.supplier_id = ".TB_PREF."supp_trans.supplier_id

		GROUP BY
			  ".TB_PREF."suppliers.supp_name,
			  ".TB_PREF."payment_terms.terms,
			  ".TB_PREF."payment_terms.days_before_due,
			  ".TB_PREF."payment_terms.day_in_following_month";
    
    $result = db_query($sql,"The customer details could not be retrieved");
    
    if (db_num_rows($result) == 0)
    {
    
    	/*Because there is no balance - so just retrieve the header information about the customer - the choice is do one query to get the balance and transactions for those customers who have a balance and two queries for those who don't have a balance OR always do two queries - I opted for the former */
    
    	$nil_balance = true;
    
    	$sql = "SELECT ".TB_PREF."suppliers.supp_name, ".TB_PREF."suppliers.curr_code, ".TB_PREF."suppliers.supplier_id,  ".TB_PREF."payment_terms.terms
			FROM ".TB_PREF."suppliers,
				 ".TB_PREF."payment_terms
			WHERE
				 ".TB_PREF."suppliers.payment_terms = ".TB_PREF."payment_terms.terms_indicator
				 AND ".TB_PREF."suppliers.supplier_id = '$supplier_id'";
    	     
    	$result = db_query($sql,"The customer details could not be retrieved");
    
    } 
    else 
    {
    	$nil_balance = false;
    }
    
    $supp = db_fetch($result);
    
    if ($nil_balance == true)
    {
    	$supp["Balance"] = 0;
    	$supp["Due"] = 0;
    	$supp["Overdue1"] = 0;
    	$supp["Overdue2"] = 0;
    }    
    
    return $supp;
	
}

function get_supplier($supplier_id)
{
	$sql = "SELECT * FROM ".TB_PREF."suppliers WHERE supplier_id=$supplier_id";
	
	$result = db_query($sql, "could not get supplier");
	
	return db_fetch($result);
}

function get_supplier_name($supplier_id)
{
	$sql = "SELECT supp_name AS name FROM ".TB_PREF."suppliers WHERE supplier_id=$supplier_id";
	
	$result = db_query($sql, "could not get supplier");
	
	$row = db_fetch_row($result);
	
	return $row[0];
}

function get_supplier_accounts($supplier_id)
{
	$sql = "SELECT payable_account,purchase_account,payment_discount_account FROM ".TB_PREF."suppliers WHERE supplier_id=$supplier_id";
	
	$result = db_query($sql, "could not get supplier");
	
	return db_fetch($result);
}


?>