<?php

//----------------------------------------------------------------------------------
function add_exchange_variation($trans_type, $trans_no, $date_, $account,
    $currency, $person_type_id=null, $person_id = "")
{
	if (is_company_currency($currency))
		return;
	if ($date_ == null)
		$date_ = Today();
	$rate = get_exchange_rate_from_home_currency($currency, $date_);	
	$result = db_query("SELECT SUM(amount) FROM ".TB_PREF."bank_trans WHERE 
		bank_act='$account' AND trans_date<='".date2sql($date_)."'", 
		"Transactions for account $account could not be calculated");
	$row = db_fetch_row($result);
	$foreign_amount = $row[0];
	$amount = get_gl_trans_from_to("", $date_, $account);
	$diff = $amount - (round2($foreign_amount * $rate, user_price_dec()));
	if ($diff != 0)
	{
		if ($trans_type == null)
			$trans_type = systypes::journal_entry();
		if ($trans_no == null)
			$trans_no = get_next_trans_no($trans_type);
		if ($person_type_id == null)
			$person_type_id = payment_person_types::misc();
		add_gl_trans($trans_type, $trans_no, $date_, $account, 0, 0, "",
    		-$diff, null, $person_type_id, $person_id);
    	add_gl_trans($trans_type, $trans_no, $date_, get_company_pref('exchange_diff_act'), 0, 0, "",
    		$diff, null, $person_type_id, $person_id);
	}	
}

function add_exchange_variation_all()
{
	$trans_no = get_next_trans_no(0);
	$sql = "SELECT * FROM ".TB_PREF."bank_accounts";
	$result = db_query($sql, "could not retreive bank accounts");
	while ($myrow = db_fetch($result))
		add_exchange_variation(0, $trans_no, null, $myrow['account_code'],
			$myrow['currency_code']);
}
//----------------------------------------------------------------------------------

function add_bank_transfer($from_account, $to_account, $date_,
	$amount, $payment_type, $ref, $memo_)
{
	begin_transaction();

	$trans_type = systypes::bank_transfer();

	$currency = get_bank_account_currency($from_account);

	$trans_no = get_next_trans_no($trans_type);

	$total = 0;
	// do the source account postings
    $total += add_gl_trans($trans_type, $trans_no, $date_, $from_account, 0, 0, "",
		-$amount, $currency);

    add_bank_trans($trans_type, $trans_no, $from_account, $ref,
		$date_, $payment_type, -$amount,
		payment_person_types::misc(), "", $currency,
		"Cannot insert a source bank transaction");

	add_exchange_variation($trans_type, $trans_no, $date_, $from_account, 
		$currency, payment_person_types::misc(), "");

	// do the destination account postings
	$total += add_gl_trans($trans_type, $trans_no, $date_, $to_account, 0, 0, "",
		$amount, $currency);
		
	/*Post a balance post if $total != 0 */
	add_gl_balance($trans_type, $trans_no, $date_, -$total);	
	
	add_bank_trans($trans_type, $trans_no, $to_account, $ref,
		$date_, $payment_type, $amount,
		payment_person_types::misc(), "",
		$currency,
		"Cannot insert a destination bank transaction");

	add_exchange_variation($trans_type, $trans_no, $date_, $from_account, 
		$currency, payment_person_types::misc(), "");
	
	add_comments($trans_type, $trans_no, $date_, $memo_);

	references::save_last($ref, $trans_type);

	commit_transaction();

	return $trans_no;
}

//----------------------------------------------------------------------------------

// $amount is in $from_account's currency

// returns an array of (inserted trans type, trans no)

function add_bank_transaction($trans_type, $from_account, $items, $date_,
	$person_type_id, $person_id, $person_detail_id,	$type, $ref, $memo_)
{
	// we can only handle type 1 (payment)and type 2 (deposit)
	if ($trans_type != systypes::bank_payment() && $trans_type != systypes::bank_deposit())
		display_db_error("Invalid type ($trans_type) sent to add_bank_transaction");

	$do_exchange_variance = false;
	
	begin_transaction();

	$currency = get_bank_account_currency($from_account);

	// the gl items are already inversed/negated for type 2 (deposit)
	$total_amount = $items->gl_items_total();

    if ($person_type_id == payment_person_types::customer())
    {
    	// we need to add a customer transaction record

		// convert to customer currency
		$cust_amount = exchange_from_to($total_amount, $currency, get_customer_currency($person_id), $date_);
		// we need to negate it too
		$cust_amount = -$cust_amount;

		$trans_no = write_customer_trans($trans_type, 0, $person_id, $person_detail_id, $date_,
        	$ref, $cust_amount);

    }
    elseif ($person_type_id == payment_person_types::supplier())
    {
    	// we need to add a supplier transaction record
		// convert to supp currency
		$supp_amount = exchange_from_to($total_amount, $currency, get_supplier_currency($person_id), $date_);

		// we need to negate it too
		$supp_amount = -$supp_amount;

		$trans_no = add_supp_trans($trans_type, $person_id, $date_, '',
    		$ref, "", $supp_amount, 0, 0);

    }
    else
    {
    	$trans_no = get_next_trans_no($trans_type);
    	$do_exchange_variance = true;
    }

    add_bank_trans($trans_type, $trans_no, $from_account, $ref,
    	$date_, $type, -$total_amount,
    	$person_type_id, $person_id,
    	$currency,
    	"Cannot insert a source bank transaction");
	$total = 0;
	foreach ($items->gl_items as $gl_item)
	{
		$is_bank_to = is_bank_account($gl_item->code_id);

		if ($trans_type == 1 AND $is_bank_to)
		{
			// we don't allow payments to go to a bank account. use transfer for this !
			display_db_error("invalid payment entered. Cannot pay to another bank account", "");
		}

    	// do the destination account postings
    	$total += add_gl_trans($trans_type, $trans_no, $date_, $gl_item->code_id,
    		$gl_item->dimension_id, $gl_item->dimension2_id, $gl_item->reference,
    		$gl_item->amount, $currency, $person_type_id, $person_id);

    	if ($is_bank_to)
    	{
    		add_bank_trans($trans_type, $trans_no, $gl_item->code_id, $ref,
    			$date_, $type, $gl_item->amount,
    			$person_type_id, $person_id, $currency,
    			"Cannot insert a destination bank transaction");
    		if ($do_exchange_variance)
    			add_exchange_variation($trans_type, $trans_no, $date_, $gl_item->code_id, 
    				$currency, $person_type_id, $person_id);
    	}
	}

	// do the source account postings
    add_gl_trans($trans_type, $trans_no, $date_, $from_account, 0, 0, "",
    	-$total, null, $person_type_id, $person_id);

    if ($do_exchange_variance)
    	add_exchange_variation($trans_type, $trans_no, $date_, $from_account, 
    		$currency, $person_type_id, $person_id);

	add_comments($trans_type, $trans_no, $date_, $memo_);

	references::save_last($ref, $trans_type);

	commit_transaction();

	return array($trans_type, $trans_no);
}

//----------------------------------------------------------------------------------------

?>