<?php

//----------------------------------------------------------------------------------

function add_bank_transfer($from_account, $to_account, $date_,
	$amount, $payment_type, $ref, $memo_)
{
	begin_transaction();

	$trans_type = systypes::bank_transfer();

	$currency = get_bank_account_currency($from_account);

	$trans_no = get_next_trans_no($trans_type);

	// do the source account postings
    add_gl_trans($trans_type, $trans_no, $date_, $from_account, 0, 0, "",
		-$amount, $currency);

    add_bank_trans($trans_type, $trans_no, $from_account, $ref,
		$date_, $payment_type, -$amount,
		payment_person_types::misc(), "", $currency,
		"Cannot insert a source bank transaction");

	// do the destination account postings
	add_gl_trans($trans_type, $trans_no, $date_, $to_account, 0, 0, "",
		$amount, $currency);

	add_bank_trans($trans_type, $trans_no, $to_account, $ref,
		$date_, $payment_type, $amount,
		payment_person_types::misc(), "",
		$currency,
		"Cannot insert a destination bank transaction");

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

	begin_transaction();

	$currency = get_bank_account_currency($from_account);

	// the gl items are already inversed/negated for type 2 (deposit)
	$total_amount = $items->gl_items_total();

    if ($person_type_id == payment_person_types::customer())
    {
    	// we need to add a customer transaction record

		// convert to customer currency
		$cust_amount = exchange_from_to($total_amount, $currency, get_customer_currency($person_id), $date_);

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
    }

	// do the source account postings
    add_gl_trans($trans_type, $trans_no, $date_, $from_account, 0, 0, "",
    	-$total_amount, $currency, $person_type_id, $person_id);

    add_bank_trans($trans_type, $trans_no, $from_account, $ref,
    	$date_, $type, -$total_amount,
    	$person_type_id, $person_id,
    	$currency,
    	"Cannot insert a source bank transaction");

	foreach ($items->gl_items as $gl_item)
	{
		$is_bank_to = is_bank_account($gl_item->code_id);

		if ($trans_type == 1 AND $is_bank_to)
		{
			// we don't allow payments to go to a bank account. use transfer for this !
			display_db_error("invalid payment entered. Cannot pay to another bank account", "");
		}

    	// do the destination account postings
    	add_gl_trans($trans_type, $trans_no, $date_, $gl_item->code_id,
    		$gl_item->dimension_id, $gl_item->dimension2_id, $gl_item->reference,
    		$gl_item->amount, $currency, $person_type_id, $person_id);

    	if ($is_bank_to)
    	{
    		add_bank_trans($trans_type, $trans_no, $gl_item->code_id, $ref,
    			$date_, $type, $gl_item->amount,
    			$person_type_id, $person_id, $currency,
    			"Cannot insert a destination bank transaction");
    	}
	}

	add_comments($trans_type, $trans_no, $date_, $memo_);

	references::save_last($ref, $trans_type);

	commit_transaction();

	return array($trans_type, $trans_no);
}

//----------------------------------------------------------------------------------------

function add_bank_payment($from_account, $items, $date_,
	$person_type_id, $person_id, $person_detail_id,	$type, $ref, $memo_)
{
	return add_bank_transaction(systypes::bank_payment(), $from_account, $items, $date_,
		$person_type_id, $person_id, $person_detail_id,	$type, $ref, $memo_);
}

//---------------------------------------------------------------------------------------------

function add_bank_deposit($from_account, $items, $date_,
	$person_type_id, $person_id, $person_detail_id,	$type, $ref, $memo_)
{
	return add_bank_transaction(systypes::bank_deposit(), $from_account, $items, $date_,
		$person_type_id, $person_id, $person_detail_id,	$type, $ref, $memo_);
}

//---------------------------------------------------------------------------------------------


?>