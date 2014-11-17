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
include_once($path_to_root . "/includes/remote_url.inc");
//---------------------------------------------------------------------------------------------
function get_exchange_rate($rate_id)
{
	$sql = "SELECT * FROM ".TB_PREF."exchange_rates WHERE id=".db_escape($rate_id);
	$result = db_query($sql, "could not get exchange rate for $rate_id");	

	return db_fetch($result);
}

// Retrieves buy exchange rate for given currency/date, zero if no result
function get_date_exchange_rate($curr_code, $date_)
{
	$date = date2sql($date_);
	$sql = "SELECT rate_buy FROM ".TB_PREF."exchange_rates WHERE curr_code=".db_escape($curr_code)
	." AND date_='$date'";
	$result = db_query($sql, "could not get exchange rate for $curr_code - $date_");	

	if(db_num_rows($result) == 0) 
		return 0;
	$row = db_fetch($result);
		return $row[0];
}

//---------------------------------------------------------------------------------------------

function update_exchange_rate($curr_code, $date_, $buy_rate, $sell_rate)
{
	if (is_company_currency($curr_code))
		display_db_error("Exchange rates cannot be set for company currency", "", true);
			
	$date = date2sql($date_);
		
	$sql = "UPDATE ".TB_PREF."exchange_rates SET rate_buy=$buy_rate, rate_sell=".db_escape($sell_rate)
	." WHERE curr_code=".db_escape($curr_code)." AND date_='$date'";
				
	db_query($sql, "could not add exchange rate for $curr_code");
}

//---------------------------------------------------------------------------------------------

function add_exchange_rate($curr_code, $date_, $buy_rate, $sell_rate)
{
	if (is_company_currency($curr_code))
		display_db_error("Exchange rates cannot be set for company currency", "", true);

	$date = date2sql($date_);
		
	$sql = "INSERT INTO ".TB_PREF."exchange_rates (curr_code, date_, rate_buy, rate_sell)
		VALUES (".db_escape($curr_code).", '$date', ".db_escape($buy_rate)
		.", ".db_escape($sell_rate).")";
	db_query($sql, "could not add exchange rate for $curr_code");
}

//---------------------------------------------------------------------------------------------

function add_new_exchange_rate($curr_code, $date_, $ex_rate)
{
	if (is_company_currency($curr_code) || !$ex_rate)
		return;

	if (!get_date_exchange_rate($curr_code, $date_))
		add_exchange_rate($curr_code, $date_, $ex_rate, $ex_rate);
}

//---------------------------------------------------------------------------------------------

function delete_exchange_rate($rate_id)
{
	$sql = "DELETE FROM ".TB_PREF."exchange_rates WHERE id=".db_escape($rate_id);
	db_query($sql, "could not delete exchange rate $rate_id");
}

//-----------------------------------------------------------------------------
//	Retrieve exchange rate as of date $date from external source (usually inet)
//
//	Exchange rate for currency revaluation purposes is defined in FA as home_currency/curr_b ratio i.e.
//
//	amount [home] = amount [curr] * ex_rate
//
function retrieve_exrate($curr_b, $date)
{
	global $xr_providers, $dflt_xr_provider;
	$xchg_rate_provider = ((isset($xr_providers) && isset($dflt_xr_provider)) ? $xr_providers[$dflt_xr_provider] : 'ECB');
	
	$rate = hook_retrieve_exrate($curr_b, $date);
	if (is_numeric($rate))
		return $rate;
	return get_extern_rate($curr_b, $xchg_rate_provider, $date);
}
//-----------------------------------------------------------------------------

function get_extern_rate($curr_b, $provider = 'ECB', $date) 
{
	global	$path_to_root;

	if ($date != Today())	// no historical rates available
		return 0;

	$curr_a = get_company_pref('curr_default');
	if ($provider == 'ECB')
	{
		$filename = "/stats/eurofxref/eurofxref-daily.xml";
		$site = "www.ecb.europa.eu";
	}
	elseif ($provider == 'YAHOO')
	{
		$filename = "/d/quotes.csv?s={$curr_a}{$curr_b}=X&f=sl1d1t1ba&e=.csv"; // new URL's for YAHOO
		$site = "download.finance.yahoo.com";
        //$filename = "/q?s={$curr_a}{$curr_b}=X"; // Let old code be here for a while, Joe.
 		//$site = "finance.yahoo.com";
	}
	elseif ($provider == 'GOOGLE')
	{
		$filename = "/finance/converter?a=1&from={$curr_a}&to={$curr_b}";
		$site = "www.google.com";
	}
	elseif ($provider == 'BLOOMBERG')
	{
		$filename = "/quote/{$curr_b}{$curr_a}:CUR";
		$site = "www.bloomberg.com";
	}
	$contents = '';
	if (function_exists('curl_init'))
	{	// first check with curl as we can set short timeout;
		$retry = 1;
	 	do {
	       $ch = curl_init();
    	   curl_setopt ($ch, CURLOPT_URL, 'http://'.$site.$filename);
	       curl_setopt ($ch, CURLOPT_COOKIEJAR, "$path_to_root/tmp/cookie.txt");
    	   curl_setopt ($ch, CURLOPT_HEADER, 0);
	       curl_setopt ($ch, CURLOPT_RETURNTRANSFER, 1);
	       // prevent warning while safe_mode/open_basedir on (redireciton doesn't occur at least on ECB page)
		   if (!ini_get('safe_mode') && !ini_get('open_basedir'))
    	   	curl_setopt ($ch, CURLOPT_FOLLOWLOCATION, 1);
	       curl_setopt ($ch, CURLOPT_TIMEOUT, 3);
    	   $contents = curl_exec ($ch);
	       curl_close($ch);
			// due to resolver bug in some curl versions (e.g. 7.15.5) 
			// try again for constant IP.
		   $site="172.230.157.137";
	   } while( ($contents == '') && $retry--);
	   
	} else {
		$contents = url_get_contents("http://".$site.$filename);
	}
	if ($provider == 'ECB')
	{
		$contents = str_replace ("<Cube currency='USD'", " <Cube currency='EUR' rate='1'/> <Cube currency='USD'", $contents);
		$from_mask = "|<Cube\s*currency=\'" . $curr_a . "\'\s*rate=\'([\d.,]*)\'\s*/>|i";
		preg_match ( $from_mask, $contents, $out );
		$val_a = isset($out[1]) ? $out[1] : 0;
		$val_a = str_replace ( ',', '', $val_a );
		$to_mask = "|<Cube\s*currency=\'" . $curr_b . "\'\s*rate=\'([\d.,]*)\'\s*/>|i";
		preg_match ( $to_mask, $contents, $out );
		$val_b = isset($out[1]) ? $out[1] : 0;
		$val_b = str_replace ( ',', '', $val_b );
		if ($val_b) 
		{
			$val = $val_a / $val_b;
		} 
		else 
		{
			$val = 0;
		}
	}
	elseif ($provider == 'YAHOO')
	{
		$val = ''; 
		$array = explode(',',$contents); // New operations for YAHOO. Safer.
		$val = $array[1];
		if ($val != 0)
			$val = 1 / $val;
		/* Let old code be here for a while, Joe.
		//if (preg_match('/Last\sTrade:(.*?)Trade\sTime/s', $contents, $matches)) {
			$val = strip_tags($matches[1]);
			$val = str_replace(',', '', $val);
			if ($val != 0)
				$val = 1 / $val;
		}
		*/
	}
	elseif ($provider == 'GOOGLE')
	{
		$val = '';
		
		$regexp = "%([\d|.]+)\s+{$curr_a}\s+=\s+<span\sclass=(.*)>([\d|.]+)\s+{$curr_b}\s*</span>%s";
		if (preg_match($regexp, $contents, $matches)) 
		{
			$val = $matches[3];
			$val = str_replace(',', '', $val);
			if ($val != 0)
				$val = 1 / $val;
		}
    }    
	elseif ($provider == 'BLOOMBERG')
	{
		$val = '';
		$stmask = '<span class=" price">';
		$val = trim(strstr($contents, $stmask));
		$stmask = chr(10);
		$val = trim(strstr($val, $stmask));
		$val = trim(strtok($val, $stmask)); 
    }    
	return $val;
}  /* end function get_extern_rate */

//-----------------------------------------------------------------------------

function get_sql_for_exchange_rates()
{
	$sql = "SELECT date_, rate_buy, id FROM "
		.TB_PREF."exchange_rates "
		."WHERE curr_code=".db_escape($_POST['curr_abrev'])."
		 ORDER BY date_ DESC";
	return $sql;	 
}
?>