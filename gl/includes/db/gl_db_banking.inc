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
function add_exchange_variation($trans_type, $trans_no, $date_, $acc_id, $account,
    $currency, $person_type_id=null, $person_id = "")
{
	if (is_company_currency($currency))
		return;
	if ($date_ == null)
		$date_ = Today();
	$for_amount = 0;

	// We have to calculate all the currency accounts belonging to the GL account
	// upto $date_ and calculate with the exchange rates. And then compare with the GL account balance.
	// 2010-02-23 Joe Hunt with help of Ary Wibowo
	$sql = "SELECT SUM(bt.amount) AS for_amount, ba.bank_curr_code 		
		FROM ".TB_PREF."bank_trans bt, ".TB_PREF."bank_accounts ba
		WHERE ba.id = bt.bank_act AND ba.account_code = ".db_escape($account)." AND bt.trans_date<='".date2sql($date_)."'
		GROUP BY ba.bank_curr_code";	
	$result = db_query($sql, "Transactions for bank account $acc_id could not be calculated");
	while ($row = db_fetch($result))
	{
		if ($row['for_amount'] == 0)
			continue;
		$rate = get_exchange_rate_from_home_currency($row['bank_curr_code'], $date_);
		$for_amount += round2($row['for_amount'] * $rate, user_price_dec());
	}	
	$amount = get_gl_trans_from_to("", $date_, $account);
	$diff = $amount - $for_amount;
	if ($diff != 0)
	{
		if ($trans_type == null)
			$trans_type = ST_JOURNAL;
		if ($trans_no == null)
			$trans_no = get_next_trans_no($trans_type);
		if ($person_type_id == null)
			$person_type_id = PT_MISC;
		add_gl_trans($trans_type, $trans_no, $date_, $account, 0, 0, _("Exchange Variance"),
    		-$diff, null, $person_type_id, $person_id);
    	add_gl_trans($trans_type, $trans_no, $date_, get_company_pref('exchange_diff_act'), 0, 0, 
    		_("Exchange Variance"),	$diff, null, $person_type_id, $person_id);
	}	
}

function add_exchange_variation_all()
{
	$trans_no = get_next_trans_no(ST_JOURNAL);
	$sql = "SELECT * FROM ".TB_PREF."bank_accounts";
	$result = db_query($sql, "could not retreive bank accounts");
	while ($myrow = db_fetch($result))
		add_exchange_variation(ST_JOURNAL, $trans_no, null, $myrow['id'], $myrow['account_code'],
			$myrow['currency_code']);
}
//----------------------------------------------------------------------------------
//	Add bank tranfer to database.
//
//	$from_account - source bank account id
//	$to_account   -	target bank account id
//	

function add_bank_transfer($from_account, $to_account, $date_,
	$amount, $ref, $memo_, $charge=0)
{
	global $Refs;
	
	begin_transaction();

	$trans_type = ST_BANKTRANSFER;

	$currency = get_bank_account_currency($from_account);

	$trans_no = get_next_trans_no($trans_type);

	$from_gl_account = get_bank_gl_account($from_account);
	$to_gl_account = get_bank_gl_account($to_account);

	$total = 0;
	// do the source account postings
    $total += add_gl_trans($trans_type, $trans_no, $date_, $from_gl_account, 0, 0, "",
		-($amount + $charge), $currency);

    add_bank_trans($trans_type, $trans_no, $from_account, $ref,
		$date_, -($amount + $charge),
		PT_MISC, "", $currency,
		"Cannot insert a source bank transaction");

	add_exchange_variation($trans_type, $trans_no, $date_, $from_account, $from_gl_account, 
		$currency, PT_MISC, "");

	if ($charge != 0)
	{
		/* Now Debit bank charge account with charges */
		$charge_act = get_company_pref('bank_charge_act');
		$total += add_gl_trans($trans_type, $trans_no, $date_,
			$charge_act, 0, 0, "", $charge, $currency);
	}
	// do the destination account postings
	$total += add_gl_trans($trans_type, $trans_no, $date_, $to_gl_account, 0, 0, "",
		$amount, $currency);
		
	/*Post a balance post if $total != 0 */
	add_gl_balance($trans_type, $trans_no, $date_, -$total);	
	
	add_bank_trans($trans_type, $trans_no, $to_account, $ref,
		$date_, $amount, PT_MISC, "",
		$currency, "Cannot insert a destination bank transaction");

	$currency = get_bank_account_currency($to_account);
	
	add_exchange_variation($trans_type, $trans_no, $date_, $to_account, $to_gl_account, 
		$currency, PT_MISC, "");
	
	add_comments($trans_type, $trans_no, $date_, $memo_);

	$Refs->save($trans_type, $trans_no, $ref);
	add_audit_trail($trans_type, $trans_no, $date_);

	commit_transaction();

	return $trans_no;
}

//----------------------------------------------------------------------------------
//	Add bank payment or deposit to database.
//
//	$from_account - bank account id
//  $item - transaction cart (line item's amounts in bank account's currency)
//  $person_type_id - defines type of $person_id identifiers
//  $person_id	- supplier/customer/other id
// $person_detail_id - customer branch id or not used
//
// returns an array of (inserted trans type, trans no)

function add_bank_transaction($trans_type, $from_account, $items, $date_,
	$person_type_id, $person_id, $person_detail_id,	$ref, $memo_, $use_transaction=true)
{
	global $Refs;

	// we can only handle type 1 (payment)and type 2 (deposit)
	if ($trans_type != ST_BANKPAYMENT && $trans_type != ST_BANKDEPOSIT)
		display_db_error("Invalid type ($trans_type) sent to add_bank_transaction");

	$do_exchange_variance = false;
	
	if ($use_transaction)
		begin_transaction();

	$currency = get_bank_account_currency($from_account);
	$bank_gl_account = get_bank_gl_account($from_account);

	// the gl items are already inversed/negated for type 2 (deposit)
	$total_amount = $items->gl_items_total();

    if ($person_type_id == PT_CUSTOMER)
    {
    	// we need to add a customer transaction record

		// convert to customer currency
		$cust_amount = exchange_from_to($total_amount, $currency, get_customer_currency($person_id), $date_);
		// we need to negate it too
		$cust_amount = -$cust_amount;

		$trans_no = write_customer_trans($trans_type, 0, $person_id, $person_detail_id, $date_,
        	$ref, $cust_amount);

    }
    elseif ($person_type_id == PT_SUPPLIER)
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

	// do the source account postings

    add_bank_trans($trans_type, $trans_no, $from_account, $ref,
    	$date_, -$total_amount,
    	$person_type_id, $person_id,
    	$currency,
    	"Cannot insert a source bank transaction");
	$total = 0;
	foreach ($items->gl_items as $gl_item)
	{
		$is_bank_to = is_bank_account($gl_item->code_id);

		if ($trans_type == ST_BANKPAYMENT AND $is_bank_to)
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
    		add_bank_trans($trans_type, $trans_no, $is_bank_to, $ref,
    			$date_, $gl_item->amount,
    			$person_type_id, $person_id, $currency,
    			"Cannot insert a destination bank transaction");
    		if ($do_exchange_variance)
    			add_exchange_variation($trans_type, $trans_no, $date_, $is_bank_to, $gl_item->code_id, 
    				$currency, $person_type_id, $person_id);
    	}
		// store tax details if the gl account is a tax account

		$amount = $gl_item->amount;
		$ex_rate = get_exchange_rate_from_home_currency($currency, $date_);
			
		add_gl_tax_details($gl_item->code_id, $trans_type, $trans_no, -$amount,
			$ex_rate, $date_, $memo_);
	}

	// do the source account postings
    add_gl_trans($trans_type, $trans_no, $date_, $bank_gl_account, 0, 0, $memo_,
    	-$total, null, $person_type_id, $person_id);

    if ($do_exchange_variance)
    	add_exchange_variation($trans_type, $trans_no, $date_, $from_account, $bank_gl_account, 
    		$currency, $person_type_id, $person_id);

	add_comments($trans_type, $trans_no, $date_, $memo_);

	$Refs->save($trans_type, $trans_no, $ref);
	add_audit_trail($trans_type, $trans_no, $date_);

	if ($use_transaction)
		commit_transaction();

	return array($trans_type, $trans_no);
}

//----------------------------------------------------------------------------------------

?>