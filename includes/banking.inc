<?php

include_once($path_to_root . "/gl/includes/gl_db.inc");

//----------------------------------------------------------------------------------

function is_bank_account($account_code)
{
	$sql= "SELECT account_code FROM ".TB_PREF."bank_accounts WHERE account_code='$account_code'";
	$result = db_query($sql, "retreive bank account currency");
	
	return (db_num_rows($result) > 0);
}

//----------------------------------------------------------------------------------

function is_company_currency($currency)
{
	return (get_company_currency() == $currency);
}

//----------------------------------------------------------------------------------

function get_company_currency()
{
	$sql= "SELECT curr_default FROM ".TB_PREF."company";
	$result = db_query($sql, "retreive company currency");

	if (db_num_rows($result) == 0)	
		display_db_error("Could not find the requested currency. Fatal.", $sql);
	
	$myrow = db_fetch_row($result);
	return $myrow[0];
}

//----------------------------------------------------------------------------------

function get_bank_account_currency($bankAccount)
{
	$sql= "SELECT bank_curr_code FROM ".TB_PREF."bank_accounts WHERE account_code='$bankAccount'";
	$result = db_query($sql, "retreive bank account currency");
	
	$myrow = db_fetch_row($result);
	return $myrow[0];	
}

//----------------------------------------------------------------------------------

function get_customer_currency($customer_id)
{
    $sql = "SELECT curr_code FROM ".TB_PREF."debtors_master WHERE debtor_no = '$customer_id'";
    			
	$result = db_query($sql, "Retreive currency of customer $customer_id");
	
	$myrow=db_fetch_row($result);   
	return $myrow[0];		
}

//----------------------------------------------------------------------------------	

function get_supplier_currency($supplier_id)
{
    $sql = "SELECT curr_code FROM ".TB_PREF."suppliers WHERE supplier_id = '$supplier_id'";
    			
	$result = db_query($sql, "Retreive currency of supplier $supplier_id");
	
	$myrow=db_fetch_row($result);   
	return $myrow[0];		
}

//----------------------------------------------------------------------------------

function get_exchange_rate_from_home_currency($currency_code, $date_)
{
	if ($currency_code == get_company_currency())
		return 1.0000;
		
	$date = date2sql($date_);
		
	$sql = "SELECT rate_buy, max(date_) as date_ FROM ".TB_PREF."exchange_rates WHERE curr_code = '$currency_code' 
				AND date_ <= '$date' GROUP BY rate_buy ORDER BY date_ Desc LIMIT 1";
	
	$result = db_query($sql, "could not query exchange rates");
	
	if (db_num_rows($result) == 0) 
	{
		// no stored exchange rate, just return 1
		return 1.000;
	}
	
	$myrow = db_fetch_row($result);	
	return $myrow[0];	
}

//----------------------------------------------------------------------------------

function get_exchange_rate_to_home_currency($currency_code, $date_)
{
	return 1 / get_exchange_rate_from_home_currency($currency_code, $date_);	
}

//----------------------------------------------------------------------------------

function to_home_currency($amount, $currency_code, $date_)
{
	$ex_rate = get_exchange_rate_to_home_currency($currency_code, $date_);
	return round($amount / $ex_rate,  user_price_dec());
}

//----------------------------------------------------------------------------------		

function get_exchange_rate_from_to($from_curr_code, $to_curr_code, $date_)
{
//	echo "converting from $from_curr_code to $to_curr_code <BR>";
	if ($from_curr_code == $to_curr_code)
		return 1.0000;
		
	$home_currency = get_company_currency();
	if ($to_curr_code == $home_currency) 
	{
		return get_exchange_rate_to_home_currency($from_curr_code, $date_);
	} 
	
	if ($from_curr_code == $home_currency) 
	{
		return get_exchange_rate_from_home_currency($to_curr_code, $date_);		
	}	
	
	// neither from or to are the home currency
	 return get_exchange_rate_to_home_currency($from_curr_code, $date_) / get_exchange_rate_to_home_currency($to_curr_code, $date_);
}

//--------------------------------------------------------------------------------

function exchange_from_to($amount, $from_curr_code, $to_curr_code, $date_)
{
	$ex_rate = get_exchange_rate_from_to($from_curr_code, $to_curr_code, $date_);
	return $amount / $ex_rate;		
}

//--------------------------------------------------------------------------------

?>