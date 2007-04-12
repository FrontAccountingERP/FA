<?php

include_once($path_to_root . "/includes/banking.inc");

include_once($path_to_root . "/includes/db/inventory_db.inc");

include_once($path_to_root . "/sales/includes/db/sales_order_db.inc");
include_once($path_to_root . "/sales/includes/db/sales_credit_db.inc");
include_once($path_to_root . "/sales/includes/db/sales_invoice_db.inc");
include_once($path_to_root . "/sales/includes/db/custalloc_db.inc");
include_once($path_to_root . "/sales/includes/db/cust_trans_db.inc");
include_once($path_to_root . "/sales/includes/db/cust_trans_details_db.inc");
include_once($path_to_root . "/sales/includes/db/payment_db.inc");
include_once($path_to_root . "/sales/includes/db/branches_db.inc");
include_once($path_to_root . "/sales/includes/db/customers_db.inc");


//----------------------------------------------------------------------------------------

// $price in customer's currency
// $quantity is used as is (if it's neg it's neg, if it's pos it's pos)
// $std_cost is in home currency
// $show_or_hide 1 show this item in invoice/credit views, 0 to hide it (used for write-off items)
// $type is 10 (invoice) or 11 (credit)

function add_stock_move_customer($type, $stock_id, $trans_id, $location, $date_, $reference, 
   	$quantity, $std_cost, $show_or_hide=1, $price=0, $discount_percent=0)
{
	return add_stock_move($type, $stock_id, $trans_id, $location, $date_, $reference, 
		$quantity, $std_cost, 0, $show_or_hide, $price, $discount_percent, 
		"The customer stock movement record cannot be inserted");
}	

//----------------------------------------------------------------------------------------

// add a debtor-related gl transaction
// $date_ is display date (non-sql)
// $amount is in CUSTOMER'S currency

function add_gl_trans_customer($type, $type_no, $date_, $account, $dimension, $dimension2, 
	$amount, $customer_id, $err_msg="")
{
	if ($err_msg == "")
		$err_msg = "The customer GL transaction could not be inserted";	
		
	return add_gl_trans($type, $type_no, $date_, $account, $dimension, $dimension2, "", $amount, 
		get_customer_currency($customer_id), 
		payment_person_types::customer(), $customer_id, $err_msg);
}

//----------------------------------------------------------------------------------------

function get_price ($stock_id, $debtor_no)
{
	$sql = "SELECT ".TB_PREF."prices.price 
		FROM ".TB_PREF."prices, ".TB_PREF."debtors_master 
		WHERE ".TB_PREF."debtors_master.sales_type=".TB_PREF."prices.sales_type_id
 			AND ".TB_PREF."debtors_master.debtor_no='" . $debtor_no . "' 
			AND ".TB_PREF."prices.stock_id = '" . $stock_id . "' 
			AND ".TB_PREF."prices.curr_abrev = ".TB_PREF."debtors_master.curr_code";
					
	$result = db_query($sql, "There was a problem retrieving the pricing information for the part $stock_id for customer");

	if (db_num_rows($result) != 0)
	{
		/*There is a price from one of the above so return that */
		$myrow = db_fetch_row($result);
		return $myrow[0];
	} 
	else 
	{
		return 0;
	}
			
}

//----------------------------------------------------------------------------------------

?>