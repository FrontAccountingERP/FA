<?php

include_once($path_to_root . "/includes/reserved.inc");

function set_global_supplier($supplier_id) 
{
	$_SESSION['wa_global_supplier_id'] = $supplier_id; 
}

function get_global_supplier($return_all=true) 
{
	if (!isset($_SESSION['wa_global_supplier_id']) || 
		($return_all == false && $_SESSION['wa_global_supplier_id'] == reserved_words::get_all()))
		return "";	
	return $_SESSION['wa_global_supplier_id'];
}

function set_global_stock_item($stock_id) 
{
	$_SESSION['wa_global_stock_id'] = $stock_id; 
}

function get_global_stock_item($return_all=true) 
{
	if (!isset($_SESSION['wa_global_stock_id']) || 
		($return_all == false && $_SESSION['wa_global_stock_id'] == reserved_words::get_all()))
		return "";		
	return $_SESSION['wa_global_stock_id'];
}

function set_global_customer($customer_id) 
{
	$_SESSION['wa_global_customer_id'] = $customer_id; 
}

function get_global_customer($return_all=true) 
{
	if (!isset($_SESSION['wa_global_customer_id']) || 
		($return_all == false && $_SESSION['wa_global_customer_id'] == reserved_words::get_all()))
		return "";
	return $_SESSION['wa_global_customer_id'];
}

function set_global_curr_code($curr_code) 
{
	$_SESSION['wa_global_curr_code'] = $curr_code;
}

function get_global_curr_code() 
{
	if (!isset($_SESSION['wa_global_curr_code']))
		return "";
	return $_SESSION['wa_global_curr_code'];
}



//--------------------------------------------------------------------------------------

?>