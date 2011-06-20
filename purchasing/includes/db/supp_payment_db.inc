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
function add_supp_payment($supplier_id, $date_, $bank_account,
	$amount, $discount, $ref, $memo_, $rate=0, $charge=0)
{
	global $Refs;

	begin_transaction();
	$args = func_get_args(); while (count($args) < 9) $args[] = 0;
	$args = (object)array_combine(array('supplier_id', 'date_', 'bank_account', 
		'amount', 'discount', 'ref', 'memo_', 'rate', 'charge'), $args);
	$args->trans_no = 0;
	hook_db_prewrite( $args, ST_SUPPAYMENT);

   	$supplier_currency = get_supplier_currency($supplier_id);
    $bank_account_currency = get_bank_account_currency($bank_account);
	$bank_gl_account = get_bank_gl_account($bank_account);

	if ($rate == 0)
	{
		$supp_amount = exchange_from_to($amount, $bank_account_currency, $supplier_currency, $date_);
		$supp_discount = exchange_from_to($discount, $bank_account_currency, $supplier_currency, $date_);
		$supp_charge = exchange_from_to($charge, $bank_account_currency, $supplier_currency, $date_);
	}
	else
	{
		$supp_amount = round($amount / $rate, user_price_dec());
		$supp_discount = round($discount / $rate, user_price_dec());
		$supp_charge = round($charge / $rate, user_price_dec());
	}
	

	// it's a supplier payment
	$trans_type = ST_SUPPAYMENT;

	/* Create a supp_trans entry for the supplier payment */
	$payment_id = write_supp_trans($trans_type, 0, $supplier_id, $date_, $date_,
		$ref, "", -$supp_amount, 0, -$supp_discount, "", $rate);

	// Now debit creditors account with payment + discount

	$total = 0;
    $supplier_accounts = get_supplier_accounts($supplier_id);
	$total += add_gl_trans_supplier($trans_type, $payment_id, $date_, $supplier_accounts["payable_account"], 0, 0,
		$supp_amount + $supp_discount, $supplier_id, "", $rate);

	// Now credit discount received account with discounts
	if ($supp_discount != 0)
	{
		$total += add_gl_trans_supplier($trans_type, $payment_id, $date_, $supplier_accounts["payment_discount_account"], 0, 0,
			-$supp_discount, $supplier_id, "", $rate);
	}

	if ($supp_charge != 0)
	{
		$charge_act = get_company_pref('bank_charge_act');
		$total += add_gl_trans_supplier($trans_type, $payment_id, $date_, $charge_act, 0, 0,
			$supp_charge, $supplier_id, "", $rate);
	}

	if ($supp_amount != 0)
	{
		$total += add_gl_trans_supplier($trans_type, $payment_id, $date_, $bank_gl_account, 0, 0,
			-($supp_amount + $supp_charge), $supplier_id, "", $rate);
	}

	/*Post a balance post if $total != 0 */
	add_gl_balance($trans_type, $payment_id, $date_, -$total, PT_SUPPLIER, $supplier_id);	

   /*now enter the bank_trans entry */
	add_bank_trans($trans_type, $payment_id, $bank_account, $ref,
		$date_, -($amount + $charge), PT_SUPPLIER,
		$supplier_id, $bank_account_currency,
		"Could not add the supplier payment bank transaction");

	add_comments($trans_type, $payment_id, $date_, $memo_);

	$Refs->save($trans_type, $payment_id, $ref);

	$args->trans_no = $payment_id;
	hook_db_postwrite($args, ST_SUPPAYMENT);
	commit_transaction();

	return $payment_id;
}

//------------------------------------------------------------------------------------------------

function void_supp_payment($type, $type_no)
{
	begin_transaction();
	hook_db_prevoid($type, $type_no);

	void_bank_trans($type, $type_no, true);
	void_gl_trans($type, $type_no, true);
	void_supp_allocations($type, $type_no);
	void_supp_trans($type, $type_no);

	commit_transaction();
}


?>