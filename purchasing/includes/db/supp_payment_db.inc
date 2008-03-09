<?php

function add_supp_payment($supplier_id, $date_, $payment_type, $bank_account,
	$amount, $discount, $ref, $memo_)
{
	begin_transaction();

   	$supplier_currency = get_supplier_currency($supplier_id);
    $bank_account_currency = get_bank_account_currency($bank_account);

	$supp_amount = exchange_from_to($amount, $bank_account_currency, $supplier_currency, $date_);
	$supp_discount = exchange_from_to($discount, $bank_account_currency, $supplier_currency, $date_);

	// it's a supplier payment
	$trans_type = 22;

	/* Create a supp_trans entry for the supplier payment */
	$payment_id = add_supp_trans($trans_type, $supplier_id, $date_, $date_,
		$ref, "", -$supp_amount, 0, -$supp_discount);

	// Now debit creditors account with payment + discount

    $supplier_accounts = get_supplier_accounts($supplier_id);

	add_gl_trans_supplier($trans_type, $payment_id, $date_, $supplier_accounts["payable_account"], 0, 0,
		$supp_amount + $supp_discount, $supplier_id);

	// Now credit discount received account with discounts
	if ($supp_discount != 0)
	{
		add_gl_trans_supplier($trans_type, $payment_id, $date_, $supplier_accounts["payment_discount_account"], 0, 0,
			-$supp_discount, $supplier_id);
	}

	if ($supp_amount != 0)
	{
		add_gl_trans_supplier($trans_type, $payment_id, $date_, $bank_account, 0, 0,
			-$supp_amount, $supplier_id);
	}

   /*now enter the bank_trans entry */
	add_bank_trans($trans_type, $payment_id, $bank_account, $ref,
		$date_, $payment_type, -($amount), payment_person_types::supplier(),
		$supplier_id, $bank_account_currency,
		"Could not add the supplier payment bank transaction");

	add_comments($trans_type, $payment_id, $date_, $memo_);

	references::save_last($ref, $trans_type);

	commit_transaction();

	return $payment_id;
}

//------------------------------------------------------------------------------------------------

function void_supp_payment($type, $type_no)
{
	begin_transaction();

	void_bank_trans($type, $type_no, true);
	void_gl_trans($type, $type_no, true);
	void_supp_allocations($type, $type_no);
	void_supp_trans($type, $type_no);

	commit_transaction();
}


?>