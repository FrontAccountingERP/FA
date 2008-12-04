<?php
/*
  Write/update customer payment.
*/
function write_customer_payment($trans_no, $customer_id, $branch_id, $bank_account,
	$date_, $receipt_type, $ref, $amount, $discount, $memo_, $rate=0)
{
	begin_transaction();

	$company_record = get_company_prefs();

	$payment_no = write_customer_trans(12, $trans_no, $customer_id, $branch_id, 
		$date_, $ref, $amount, $discount, 0, 0, 0, 0, 0, 0, 0, "", 0, $rate);

	if ($trans_no != 0) {
	  delete_comments(12, $trans_no);
	  void_bank_trans(12, $trans_no, true);
	  void_gl_trans(12, $trans_no, true);
	  void_cust_allocations(12, $trans_no, $date_);
	}
	$total = 0;
	/* Bank account entry first */
	$total += add_gl_trans_customer(12, $payment_no, $date_,
		$bank_account, 0, 0, $amount,  $customer_id,
		"Cannot insert a GL transaction for the bank account debit", $rate);

	if ($branch_id != reserved_words::get_any_numeric()) {

		$branch_data = get_branch_accounts($branch_id);

		$debtors_account = $branch_data["receivables_account"];
		$discount_account = $branch_data["payment_discount_account"];

	} else {
		$debtors_account = $company_record["debtors_act"];
		$discount_account = $company_record["default_prompt_payment_act"];
	}

	if ($discount != 0)	{
		/* Now Debit discount account with discounts allowed*/
		$total += add_gl_trans_customer(12, $payment_no, $date_,
			$discount_account, 0, 0, $discount, $customer_id,
			"Cannot insert a GL transaction for the payment discount debit", $rate);
	}

	if (($discount + $amount) != 0)	{
		/* Now Credit Debtors account with receipts + discounts */
		add_gl_trans(12, $payment_no, $date_, $debtors_account, 0, 0, "",
			-$total, null, payment_person_types::customer(), $customer_id, 
			"Cannot insert a GL transaction for the debtors account credit");
	}

	/*now enter the bank_trans entry */
	add_bank_trans(12, $payment_no, $bank_account, $ref,
		$date_, $receipt_type, $amount, payment_person_types::customer(), $customer_id,
		get_customer_currency($customer_id));

	add_comments(12, $payment_no, $date_, $memo_);

	references::save_last($ref, 12);

	commit_transaction();

	return $payment_no;
}

//-------------------------------------------------------------------------------------------------

function void_customer_payment($type, $type_no)
{
	begin_transaction();

	void_bank_trans($type, $type_no, true);
	void_gl_trans($type, $type_no, true);
	void_cust_allocations($type, $type_no);
	void_customer_trans($type, $type_no);

	commit_transaction();
}


?>