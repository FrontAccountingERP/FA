<?php

include_once($path_to_root . "/purchasing/includes/supp_trans_class.inc");

include_once($path_to_root . "/includes/banking.inc");

include_once($path_to_root . "/includes/date_functions.inc");

include_once($path_to_root . "/includes/db/inventory_db.inc");

include_once($path_to_root . "/purchasing/includes/db/supp_trans_db.inc");
include_once($path_to_root . "/purchasing/includes/db/po_db.inc");
include_once($path_to_root . "/purchasing/includes/db/grn_db.inc");
include_once($path_to_root . "/purchasing/includes/db/invoice_db.inc");
include_once($path_to_root . "/purchasing/includes/db/suppalloc_db.inc");
include_once($path_to_root . "/purchasing/includes/db/supp_payment_db.inc");
include_once($path_to_root . "/purchasing/includes/db/suppliers_db.inc");

//-------------------------------------------------------------------------------------------------------------

// add a supplier-related gl transaction
// $date_ is display date (non-sql)
// $amount is in SUPPLIERS'S currency

function add_gl_trans_supplier($type, $type_no, $date_, $account, $dimension, $dimension2,  
	$amount, $supplier_id, $err_msg="")
{
	if ($err_msg == "")
		$err_msg = "The supplier GL transaction could not be inserted";	
		
	return add_gl_trans($type, $type_no, $date_, $account, $dimension, $dimension2, "", 
		$amount, get_supplier_currency($supplier_id), 
		payment_person_types::supplier(), $supplier_id, $err_msg);
}

//----------------------------------------------------------------------------------------

function get_purchase_price($supplier_id, $stock_id)
{
	$sql = "SELECT price, conversion_factor FROM ".TB_PREF."purch_data 
		WHERE supplier_id = '" . $supplier_id . "' 
		AND stock_id = '". $stock_id . "'";
	$result = db_query($sql, "The supplier pricing details for " . $stock_id . " could not be retrieved");    

	if (db_num_rows($result) == 1)
	{
		$myrow = db_fetch($result);
		return $myrow["price"] / $myrow['conversion_factor'];
	} 
	else 
	{
		return 0;
	}	
}

//----------------------------------------------------------------------------------------


?>