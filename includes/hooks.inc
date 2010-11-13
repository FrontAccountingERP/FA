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
class hooks {
	//
	// Price in words. $doc_type is set to document type and can be used to suppress 
	// price in words printing for selected document types.
	// Used instead of built in simple english price_in_words() function.
	//
	//	Returns: amount in words as string.

//	function price_in_words($amount, $doc_type)
//	{
//	}

	//
	// Exchange rate currency $curr as on date $date.
	// Keep in mind FA has internally implemented 3 exrate providers
	// If any of them supports your currency, you can simply use function below
	// with apprioprate provider set, otherwise implement your own.
	// Returns: $curr value in home currency units as a real number.

//	function retrieve_ex_rate($curr, $date)
//	{
//	 	$provider = 'ECB'; // 'ECB', 'YAHOO' or 'GOOGLE'
//		return get_extern_rate($curr, $provider, $date);
//	}

	// Generic function called at the end of Tax Report (report 709)
	// Can be used e.g. for special database updates on every report printing
	// or to print special tax report footer 
	//
	// Returns: nothing
//	function tax_report_done()
//	{
//	}
	// Following database transaction hooks akcepts array of parameters:
	// 'cart' => transaction data
	// 'trans_type' => transaction type

//	function db_prewrite(&$cart, $trans_type)
//	{
//	}
//	function db_postwrite(&$cart, $trans_type)
//	{
//	}
//	function db_prevoid($trans_type, $trans_no)
//	{
//	}
}

/*
	Calls hook $method defined in extension $ext (if any)
*/
function hook_invoke($ext, &$data, $opts=null) {

	global $Hooks;
	
	if (isset($Hooks[$ext]) && method_exists($Hooks[$ext], $method)) {
		$Hooks[$ext]->$method($data, $opts);
	}
}

/*
	Calls hook $methods defined in all extensions (if any)
*/
function hook_invoke_all($method, &$data, $opts=null) {

	global $Hooks;
	
	$result = array();
	foreach($Hooks as $ext => $hook)
		if (method_exists($hook, $method)) {
			$result = $Hooks[$ext]->$method($data, $opts);
			if (isset($result) && is_array($result)) {
				$return = array_merge_recursive($return, $result);
			} else if (isset($result)) {
				$return[] = $result;
				}
		}
	return $result;
}
/*
	Returns first non-null result returned from hook.
*/
function hook_invoke_first($method, &$data, $opts=null) {

	global $Hooks;
	
	$result = array();
	foreach($Hooks as $ext => $hook) {
		if (method_exists($hook, $method)) {
			$result = $Hooks[$ext]->$method($data, $opts);
			if (isset($result))
				return $result;
		}
	}
	return null;
}
//------------------------------------------------------------------------------------------
//	Database transaction hooks.
//	$type - type of transaction (simplifies cart processing)
//	$cart - transaction cart
//	$args is optional array of parameters
//
// For FA 2.3 prewrite, postwrite and prevoid hooks are implemented for following transaction types:
//
// ST_BANKPAYMENT, ST_BANKDEPOSIT, ST_BANKTRANSFER,
// ST_SALESORDER, ST_SALESQUOTE, ST_SALESINVOICE, ST_CUSTCREDIT, ST_CUSTPAYMENT, ST_CUSTDELIVERY,
// ST_LOCTRANSFER, ST_INVADJUST, 
// ST_PURCHORDER, ST_SUPPINVOICE, ST_SUPPCREDIT, ST_SUPPAYMENT, ST_SUPPRECEIVE,
// ST_WORKORDER, ST_MANUISSUE, ST_MANURECEIVE, 

/*
	Invoked after transaction has been read from database to cart.
	Not implemented yet.
*/
//function hook_db_postread(&$cart, $type)
//{
//	hook_invoke_all('db_postread', $cart, $type);
//}

/*
	Invoked before transaction is written to database.
*/
function hook_db_prewrite(&$cart, $type)
{
	hook_invoke_all('db_prewrite', $cart, $type);
}

/*
	Invoked after transaction has been written to database.
*/
function hook_db_postwrite(&$cart, $type)
{
	hook_invoke_all('db_postwrite', $cart, $type);
}
/*
	Invoked before transaction is voided
*/
function hook_db_prevoid($type, $type_no)
{
	hook_invoke_all('db_prevoid', $type, $type_no);
}

//-------------------------------------------------------------------------------------------
//
//	Various hooks
//
//	Alternative exchange rates feeds.
//
function hook_retrieve_exrate($currency, $date)
{
	return hook_invoke_first('retrieve_exrate', $currency, $date);
}
//
// Generic function called at the end of Tax Report (report 709)
//
function hook_tax_report_done()
{
	hook_invoke_all('tax_report_done', $dummy);
}
//
//	Amount in words displayed on various documents (especially sales invoice)
//
function hook_price_in_words($amount, $document)
{
	return hook_invoke_first('price_in_words', $amount, $document);
}
