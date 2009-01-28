<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU Affero General Public License,
	AGPL, as published by the Free Software Foundation, either version 
	3 of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/agpl-3.0.html>.
***********************************************************************/
/*
  Write/update customer payment.
*/
function write_customer_payment($trans_no, $customer_id, $branch_id, $bank_account,
	$date_, $ref, $amount, $discount, $memo_, $rate=0)
{
	begin_transaction();

	$company_record = get_company_prefs();

	$payment_no = write_customer_trans(12, $trans_no, $customer_id, $branch_id, 
		$date_, $ref, $amount, $discount, 0, 0, 0, 0, 0, 0, 0, "", 0, $rate);

	$bank_gl_account = get_bank_gl_account($bank_account);

	if ($trans_no != 0) {
	  delete_comments(12, $trans_no);
	  void_bank_trans(12, $trans_no, true);
	  void_gl_trans(12, $trans_no, true);
	  void_cust_allocations(12, $trans_no, $date_);
	}
	$total = 0;
	/* Bank account entry first */
	$total += add_gl_trans_customer(12, $payment_no, $date_,
		$bank_gl_account, 0, 0, $amount,  $customer_id,
		"Cannot insert a GL transaction for the bank account debit", $rate);

	if ($branch_id != reserved_words::get_any_numeric()) {

		$branch_data = get_branch_accounts($branch_id);

		$debtors_account = $branch_data["receivables_account"];
		$discount_account = $branch_data["payment_discount_account"];

	} else {
		$debtors_account = $company_record["debtors_act"];
		$discount_account = $company_record["default_prompt_payment_act"];
	}

	if (($discount + $amount) != 0)	{
		/* Now Credit Debtors account with receipts + discounts */
		$total += add_gl_trans_customer(12, $payment_no, $date_,
			$debtors_account, 0, 0, -($discount + $amount), $customer_id,
			"Cannot insert a GL transaction for the debtors account credit", $rate);
	}
	if ($discount != 0)	{
		/* Now Debit discount account with discounts allowed*/
		$total += add_gl_trans_customer(12, $payment_no, $date_,
			$discount_account, 0, 0, $discount, $customer_id,
			"Cannot insert a GL transaction for the payment discount debit", $rate);
	}

	/*Post a balance post if $total != 0 */
	add_gl_balance(12, $payment_no, $date_, -$total, payment_person_types::customer(), $customer_id);	

	/*now enter the bank_trans entry */
	add_bank_trans(12, $payment_no, $bank_account, $ref,
		$date_, $amount, payment_person_types::customer(), $customer_id,
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