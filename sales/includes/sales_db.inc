<?php

include_once($path_to_root . "/includes/banking.inc");
include_once($path_to_root . "/includes/db/inventory_db.inc");
include_once($path_to_root . "/sales/includes/db/sales_order_db.inc");
include_once($path_to_root . "/sales/includes/db/sales_credit_db.inc");
include_once($path_to_root . "/sales/includes/db/sales_invoice_db.inc");
include_once($path_to_root . "/sales/includes/db/sales_delivery_db.inc");
include_once($path_to_root . "/sales/includes/db/sales_types_db.inc");
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
	$amount, $customer_id, $err_msg="", $rate=0)
{
	if ($err_msg == "")
		$err_msg = "The customer GL transaction could not be inserted";

	return add_gl_trans($type, $type_no, $date_, $account, $dimension, $dimension2, "", $amount,
		get_customer_currency($customer_id),
		payment_person_types::customer(), $customer_id, $err_msg, $rate);
}

//----------------------------------------------------------------------------------------

function get_price ($stock_id, $currency, $sales_type_id, $factor=null, $date=null)
{
	if ($date == null)
	    $date = Today();

	if ($factor === null) 
	{
		$myrow = get_sales_type($sales_type_id);
		$factor = $myrow['factor'];
	}
	    
	$sql = "SELECT ".TB_PREF."prices.price
		FROM ".TB_PREF."prices
		WHERE ".TB_PREF."prices.stock_id = '" . $stock_id . "' "
		." AND ".TB_PREF."prices.sales_type_id = " . $sales_type_id
		." AND ".TB_PREF."prices.curr_abrev = '$currency'";

	$msg = "There was a problem retrieving the pricing information for the part $stock_id for customer";
	$result = db_query($sql, $msg);

	if (db_num_rows($result) != 0) 
	{
		$myrow = db_fetch_row($result);
		return $myrow[0];
	}
	if ($factor == 0) return 0; // auto price calculations off

	$base_id = get_base_sales_type();
        $home_curr = get_company_currency();

    // get all prices which we can use to guess the price.
    // alternative is make up to 2 additional sql queries
	$sql = "SELECT ".TB_PREF."prices.price,".TB_PREF."prices.curr_abrev,
		".TB_PREF."prices.sales_type_id
		FROM ".TB_PREF."prices
		WHERE ".TB_PREF."prices.stock_id = '" . $stock_id . "' "
		." AND (".TB_PREF."prices.sales_type_id = " . $sales_type_id
		." OR ".TB_PREF."prices.sales_type_id = " . $base_id.")"
		." AND (".TB_PREF."prices.curr_abrev = '$currency'"
		." OR ".TB_PREF."prices.curr_abrev = '$home_curr')";

	$result = db_query($sql, $msg);

	$prices = array();
	while($myrow = db_fetch($result)) 
	{
	    $prices[$myrow['sales_type_id']][$myrow['curr_abrev']] = $myrow['price'];
	}
	
	$rate = round(get_exchange_rate_from_home_currency($currency, $date),
	    user_exrate_dec());
	$price = 0.00;
	
	if (isset($prices[$sales_type_id][$home_curr])) 
	{
	    $price = $prices[$sales_type_id][$home_curr] / $rate;
	}
	if (isset($prices[$base_id][$currency])) 
	{
	    $price =$prices[$base_id][$currency] * $factor;
	}
	if (isset($prices[$base_id][$home_curr])) 
	{
	    $price =$prices[$base_id][$home_curr] * $factor / $rate;
	}
	
	return round($price, user_price_dec());
}

//-----------------------------------------------------------------------------

function set_document_parent($cart)
{
	$inv_no = key($cart->trans_no);

	if (count($cart->src_docs) == 1) {

	// if this child document has only one parent - update child link
	$del_no = reset(array_keys($cart->src_docs));

	$sql = 'UPDATE '.TB_PREF.'debtor_trans SET trans_link = ' . $del_no .
		' WHERE type='.$cart->trans_type.' AND trans_no='. $inv_no ;
	db_query($sql, 'Child document link cannot be updated');

	}
	if ($cart->trans_type != 10)
		return 0;

	// the rest is batch invoice specific

	foreach ($cart->line_items as $line) {
		if ($line->quantity != $line->qty_dispatched) {
			return 1;	// this is partial invoice
		}
	}

	$sql = 'UPDATE '.TB_PREF.'debtor_trans SET trans_link = ' . $inv_no .
	' WHERE type='.get_parent_type($cart->trans_type).' AND (';

	$deliveries = array_keys($cart->src_docs);

	foreach ($deliveries as $key=>$del)
		$deliveries[$key] = 'trans_no='.$del;

	$sql .= implode(' OR ', $deliveries) . ')';
	db_query($sql, 'Delivery links cannot be updated');

	return 0; // batch or complete invoice
}

//--------------------------------------------------------------------------------------------------
function get_parent_type($type)
{
	$parent_types = array( 11=>10, 10=>13, 13=>30 );
	return isset($parent_types[$type]) ?  $parent_types[$type] : 0;
}

//--------------------------------------------------------------------------------------------------
function update_parent_line($doc_type, $line_id, $qty_dispatched)
{
	$doc_type = get_parent_type($doc_type);

//	echo "update line: $line_id, $doc_type, $qty_dispatched";
	if ($doc_type==0)
		return false;
	else {
		if ($doc_type==30)
			$sql = "UPDATE ".TB_PREF."sales_order_details
				SET qty_sent = qty_sent + $qty_dispatched
				WHERE id=$line_id";
		else
			$sql = "UPDATE ".TB_PREF."debtor_trans_details
				SET qty_done = qty_done + $qty_dispatched
				WHERE id=$line_id";
	}
	db_query($sql, "The parent document detail record could not be updated");
	return true;
}

//--------------------------------------------------------------------------------------------------
// find inventory location for given transaction
//
function get_location(&$cart)
{
	$sql = "SELECT ".TB_PREF."locations.* FROM ".TB_PREF."stock_moves,"
		.TB_PREF."locations".
		" WHERE type=".$cart->trans_type.
		" AND trans_no=".key($cart->trans_no).
		" AND qty!=0 ".
		" AND ".TB_PREF."locations.loc_code=".TB_PREF."stock_moves.loc_code";
	$result = db_query($sql, 'Retreiving inventory location');


	if (db_num_rows($result)) {
		return db_fetch($result);
	}
	return null;
}
//--------------------------------------------------------------------------------------------------
// Generic read debtor transaction into cart
//
//	$trans_no - array of trans nums; special case trans_no==0 - new doc
//
function read_sales_trans($doc_type, $trans_no, &$cart)
{
	if (!is_array($trans_no) && $trans_no)
			$trans_no = array($trans_no);

	$cart->trans_type = $doc_type;
	if (!$trans_no) { // new document
		$cart->trans_no = $trans_no;
	} else {
		// read header data from first document
		$myrow = get_customer_trans($trans_no[0],$doc_type);
		if (count($trans_no)>1)
			$cart->trans_no = get_customer_trans_version($doc_type, $trans_no);
		else
			$cart->trans_no = array($trans_no[0]=>$myrow["version"]);

		$cart->set_sales_type($myrow["tpe"], $myrow["sales_type"], $myrow["tax_included"],0);

		$cart->set_customer($myrow["debtor_no"], $myrow["DebtorName"],
			$myrow["curr_code"], $myrow["discount"]);

		$cart->set_branch($myrow["branch_code"], $myrow["tax_group_id"],
			$myrow["tax_group_name"],	$myrow["phone"], $myrow["email"]);

		$cart->reference = $myrow["reference"];
		$cart->order_no = $myrow["order_"];
		$cart->trans_link = $myrow["trans_link"];
		$cart->due_date = sql2date($myrow["due_date"]);
		$cart->document_date = sql2date($myrow["tran_date"]);

		$cart->Comments = '';
		foreach ( $trans_no as $trans ) {
			$coms =  get_comments($doc_type,$trans);
			while($row=db_fetch($coms)) {
				$text = $row['memo_'];
				if ($text!='') {
					if ($cart->Comments!='')
						$cart->Comments .= "\n";
					$cart->Comments .= $text;
				}
			}
		}

		// FIX this should be calculated sum() for multiply parents

		$cart->set_delivery($myrow["ship_via"], $myrow["br_name"],
		$myrow["br_address"], $myrow["ov_freight"]);

		$location = 0;
		$myrow = get_location($cart); // find location from movement

		if($myrow!=null) {
			$cart->set_location($myrow['loc_code'], $myrow['location_name']);
		}

		$result = get_customer_trans_details($doc_type,$trans_no);
		if (db_num_rows($result) > 0) {
			for($line_no=0; $myrow = db_fetch($result); $line_no++)	{
				$cart->add_to_cart($line_no,$myrow["stock_id"],$myrow["quantity"],
					$myrow["unit_price"], $myrow["discount_percent"],
					$myrow["qty_done"], $myrow["standard_cost"],
					$myrow["StockDescription"],$myrow["id"], $myrow["debtor_trans_no"]);
			}
		}
	} // !newdoc

	return true;
}
//----------------------------------------------------------------------------------------
?>