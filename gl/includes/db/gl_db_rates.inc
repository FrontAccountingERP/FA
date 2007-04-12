<?php

//--------------------------------------------------------------------------------------------------

function get_exchange_rates($curr_code)
{
	$sql = "SELECT * FROM ".TB_PREF."exchange_rates WHERE curr_code='$curr_code' ORDER BY date_ DESC";	
	return db_query($sql, "could not get exchange rates for $curr_code");
}

//---------------------------------------------------------------------------------------------

function get_exchange_rate($rate_id)
{
	$sql = "SELECT * FROM ".TB_PREF."exchange_rates WHERE id=$rate_id";
	$result = db_query($sql, "could not get exchange rate for $rate_id");	

	return db_fetch($result);
}

function get_date_exchange_rate($curr_code, $date_)
{
	$date = date2sql($date_);
	$sql = "SELECT date_ FROM ".TB_PREF."exchange_rates WHERE curr_code='$curr_code' 
		AND date_='$date'";
	$result = db_query($sql, "could not get exchange rate for $curr_code - $date_");	

	return (db_num_rows($result) == 1);
}

//---------------------------------------------------------------------------------------------

function update_exchange_rate($curr_code, $date_, $buy_rate, $sell_rate)
{
	if (is_company_currency($curr_code))
		display_db_error("Exchange rates cannot be set for company currency", "", true);
			
	$date = date2sql($date_);
		
	$sql = "UPDATE ".TB_PREF."exchange_rates SET rate_buy=$buy_rate, rate_sell=$sell_rate
		WHERE curr_code='$curr_code' AND date_='$date'";
				
	db_query($sql, "could not add exchange rate for $curr_code");				
}

//---------------------------------------------------------------------------------------------

function add_exchange_rate($curr_code, $date_, $buy_rate, $sell_rate)
{
	if (is_company_currency($curr_code))
		display_db_error("Exchange rates cannot be set for company currency", "", true);

	$date = date2sql($date_);
		
	$sql = "INSERT INTO ".TB_PREF."exchange_rates (curr_code, date_, rate_buy, rate_sell)
		VALUES ('$curr_code', '$date', $buy_rate, $sell_rate)";
	db_query($sql, "could not add exchange rate for $curr_code");				
}

//---------------------------------------------------------------------------------------------

function delete_exchange_rate($rate_id)
{
	$sql = "DELETE FROM ".TB_PREF."exchange_rates WHERE id=$rate_id";
	db_query($sql, "could not delete exchange rate $rate_id");		
}

//---------------------------------------------------------------------------------------------

function get_ecb_rate($curr_b) 
{
	$curr_a = get_company_pref('curr_default');
	$ecb_filename = 'http://www.ecb.int/stats/eurofxref/eurofxref-daily.xml';
	$handle = @fopen($ecb_filename, 'rb');
	$contents = '';
	if ($handle)
	{
		do 
		{
			$data = @fread( $handle, 4096 );
			if ( strlen ( $data ) == 0 ) 
   				break;
   			$contents .= $data; // with this syntax only text will be translated, whole text with htmlspecialchars($data)
		} 
		while (true);
		@fclose( $handle );
	} // end handle
	else
	{
       $ch = curl_init();
       curl_setopt ($ch, CURLOPT_URL, $ecb_filename);
       curl_setopt ($ch, CURLOPT_COOKIEJAR, "cookie.txt");
       curl_setopt ($ch, CURLOPT_HEADER, 0);
       curl_setopt ($ch, CURLOPT_RETURNTRANSFER, 1);
       curl_setopt ($ch, CURLOPT_FOLLOWLOCATION, 1);
       curl_setopt ($ch, CURLOPT_TIMEOUT, 120);
       $contents = curl_exec ($ch);
       curl_close($ch);
	}
	$contents = str_replace ("<Cube currency='USD'", " <Cube currency='EUR' rate='1'/> <Cube currency='USD'", $contents);
	$from_mask = "|<Cube\s*currency=\'" . $curr_a . "\'\s*rate=\'([\d.,]*)\'\s*/>|i";
	preg_match ( $from_mask, $contents, $out );
	$val_a = $out[1];
	$val_a = str_replace ( ',', '', $val_a );
	$to_mask = "|<Cube\s*currency=\'" . $curr_b . "\'\s*rate=\'([\d.,]*)\'\s*/>|i";
	preg_match ( $to_mask, $contents, $out );
	$val_b = $out[1];
	$val_b = str_replace ( ',', '', $val_b );
	if ($val_b) 
	{
		$val = $val_a / $val_b;
	} 
	else 
	{
		$val = 0;
	}
	return $val;
} // end function get_ecb_rate

?>