<?php

//----------------------------------------------------------------------------------------

// $Total, $Tax, $Freight, $discount all in customer's currency
// date_ is display date (non-sql)

function add_customer_trans($trans_type, $debtor_no, $BranchNo, 
	$date_, $reference, $Total, $discount=0, $Tax=0, $Freight=0, 
	$sales_type=0, $order_no=0, $trans_link=0, $ship_via="", $due_date=null,  
	$AllocAmt=0)
{
	$trans_no = get_next_trans_no($trans_type);
	
	$curr = get_customer_currency($debtor_no);
	$rate = get_exchange_rate_from_home_currency($curr, $date_);	

	$SQLDate = date2sql($date_);
	$SQLDueDate = date2sql($due_date);

	$sql = "INSERT INTO ".TB_PREF."debtor_trans (
		trans_no, type, 
		debtor_no, branch_code, 
		tran_date, due_date, 
		reference, tpe, 
		order_, ov_amount, ov_discount,
		ov_gst, ov_freight, rate,
		ship_via, alloc, trans_link
		) VALUES ($trans_no, $trans_type, '$debtor_no', '$BranchNo',
		'$SQLDate', '$SQLDueDate', '$reference',
		'$sales_type', $order_no, $Total, $discount, $Tax, $Freight, $rate, 
		'$ship_via', $AllocAmt, $trans_link)";

	db_query($sql, "The debtor transaction record could not be inserted");
	
	return $trans_no;	
}

//----------------------------------------------------------------------------------------

function get_customer_trans($trans_id, $trans_type)
{
	$sql = "SELECT ".TB_PREF."debtor_trans.*,
		ov_amount+ov_gst+ov_freight+ov_discount AS Total,   
		".TB_PREF."debtors_master.name AS DebtorName, ".TB_PREF."debtors_master.address, ".TB_PREF."debtors_master.email, 
		".TB_PREF."debtors_master.curr_code, ".TB_PREF."debtors_master.tax_id, ".TB_PREF."debtors_master.payment_terms ";
	
	if ($trans_type == systypes::cust_payment()) 
	{
		// it's a payment so also get the bank account
		$sql .= ", ".TB_PREF."bank_accounts.bank_name, ".TB_PREF."bank_accounts.bank_account_name,
			".TB_PREF."bank_trans_types.name AS BankTransType ";  
	}  
	
	if ($trans_type == 10 OR $trans_type == 11 OR $trans_type == 13) 
	{
		// it's an invoice so also get the shipper and salestype
		$sql .= ", ".TB_PREF."shippers.shipper_name, ".TB_PREF."sales_types.sales_type ";  
	}  	
	
	
	$sql .= " FROM ".TB_PREF."debtor_trans, ".TB_PREF."debtors_master "; 

	if ($trans_type == systypes::cust_payment()) 
	{
		// it's a payment so also get the bank account
		$sql .= ", ".TB_PREF."bank_trans, ".TB_PREF."bank_accounts, ".TB_PREF."bank_trans_types ";  
	}	
	
	if ($trans_type == 10 OR $trans_type == 11 OR $trans_type == 13) 
	{
		// it's an invoice so also get the shipper, salestypes
		$sql .= ", ".TB_PREF."shippers, ".TB_PREF."sales_types ";  
	}  		
	
	$sql .= " WHERE ".TB_PREF."debtor_trans.trans_no=$trans_id 
		AND ".TB_PREF."debtor_trans.type=$trans_type
		AND ".TB_PREF."debtor_trans.debtor_no=".TB_PREF."debtors_master.debtor_no";
					
	if ($trans_type == systypes::cust_payment()) 
	{
		// it's a payment so also get the bank account
		$sql .= " AND ".TB_PREF."bank_trans.trans_no =$trans_id
			AND ".TB_PREF."bank_trans.type=$trans_type	
			AND ".TB_PREF."bank_trans_types.id = ".TB_PREF."bank_trans.bank_trans_type_id
			AND ".TB_PREF."bank_accounts.account_code=".TB_PREF."bank_trans.bank_act ";  
	}			
	if ($trans_type == 10 OR $trans_type == 11 OR $trans_type == 13) 
	{
		// it's an invoice so also get the shipper
		$sql .= " AND ".TB_PREF."shippers.shipper_id=".TB_PREF."debtor_trans.ship_via 
			AND ".TB_PREF."sales_types.id = ".TB_PREF."debtor_trans.tpe ";  
	}  					
	
	$result = db_query($sql, "Cannot retreive a debtor transaction");	
	
    if (db_num_rows($result) == 0) 
    {
       // can't return nothing
       display_db_error("no debtor trans found for given params", $sql, true);
       exit;
    }
    
    if (db_num_rows($result) > 1) 
    {
       // can't return multiple
       display_db_error("duplicate debtor transactions found for given params", $sql, true);
       exit;
    }	
    
    return db_fetch($result);   	
}

//----------------------------------------------------------------------------------------

function get_customer_transactions($extra_fields=null, $extra_conditions=null, $extra_tables=null)
{
	$sql = "SELECT ".TB_PREF."debtor_trans.*,
		ov_amount+ov_gst+ov_freight+ov_discount AS Total,   
		".TB_PREF."debtors_master.name AS DebtorName, ".TB_PREF."debtors_master.address,
		".TB_PREF."debtors_master.curr_code ";
				
	if ($extra_fields)
		$sql .= ", $extra_fields ";			
				
	$sql .= " FROM ".TB_PREF."debtor_trans, ".TB_PREF."debtors_master ";
	if ($extra_tables)
		$sql .= ",$extra_tables ";
				
	$sql .= " WHERE ".TB_PREF."debtor_trans.debtor_no=".TB_PREF."debtors_master.debtor_no";
				
	if ($extra_conditions)
		$sql .= " AND $extra_conditions ";	
		
	$sql .= " ORDER BY trans_no";	
	
	return db_query($sql, "Cannot retreive debtor transactions");
}

//----------------------------------------------------------------------------------------

function exists_customer_trans($type, $type_no)
{
	$sql = "SELECT trans_no FROM ".TB_PREF."debtor_trans WHERE type=$type
		AND trans_no=$type_no";
	$result = db_query($sql, "Cannot retreive a debtor transaction");	
	
    return (db_num_rows($result) > 0);				
}

//----------------------------------------------------------------------------------------

// retreives the related sales order for a given trans

function get_customer_trans_order($type, $type_no)
{
	$sql = "SELECT order_ FROM ".TB_PREF."debtor_trans WHERE type=$type AND trans_no=$type_no";
	
	$result = db_query($sql, "The debtor transaction could not be queried");
	
	$row = db_fetch_row($result);
	
	return $row[0];	
}

//----------------------------------------------------------------------------------------

function get_customer_details_from_trans($type, $type_no)
{
	$sql = "SELECT ".TB_PREF."debtors_master.name, ".TB_PREF."debtors_master.curr_code, ".TB_PREF."cust_branch.br_name  
		FROM ".TB_PREF."debtors_master,".TB_PREF."cust_branch,".TB_PREF."debtor_trans 
		WHERE ".TB_PREF."debtor_trans.type=$type AND ".TB_PREF."debtor_trans.trans_no=$type_no
		AND ".TB_PREF."debtors_master.debtor_no = ".TB_PREF."debtor_trans.debtor_no 
		AND	".TB_PREF."cust_branch.branch_code = ".TB_PREF."debtor_trans.branch_code";
	
	$result = db_query($sql, "could not get customer details from trans");
	return db_fetch($result);				
}

//----------------------------------------------------------------------------------------

function void_customer_trans($type, $type_no)
{
	// clear all values and mark as void
	$sql = "UPDATE ".TB_PREF."debtor_trans SET ov_amount=0, ov_discount=0, ov_gst=0, ov_freight=0, 
		alloc=0 WHERE type=$type AND trans_no=$type_no";
			
	db_query($sql, "could not void debtor transactions for type=$type and trans_no=$type_no");
}

//----------------------------------------------------------------------------------------

function post_void_customer_trans($type, $type_no)
{
	switch ($type)
	{
		case 10 :
		case 11 :
			void_sales_invoice($type, $type_no);
			break;
		case 13 :
			void_sales_delivery($type, $type_no);
			break;
		case systypes::cust_payment() :
			void_customer_payment($type, $type_no);
			break;
	}
}

//----------------------------------------------------------------------------------------

function get_customer_trans_link($type, $type_no)
{
  $row = db_query("SELECT trans_link from debtor_trans 
	WHERE type=$type AND trans_no=$type_no", "could not get transaction link for type=$type and trans_no=$type_no");
 return $row[0];
}
//----------------------------------------------------------------------------------------

?>