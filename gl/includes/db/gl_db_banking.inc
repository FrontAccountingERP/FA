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
//----------------------------------------------------------------------------------
//	Add bank tranfer to database.
//
//	$from_account - source bank account id
//	$to_account   -	target bank account id
//	
function add_bank_transfer($from_account, $to_account, $date_,
	$amount, $ref, $memo_)
{
	begin_transaction();

	$trans_type = systypes::bank_transfer();

	$currency = get_bank_account_currency($from_account);

	$trans_no = get_next_trans_no($trans_type);

	$from_gl_account = get_bank_gl_account($from_account);
	$to_gl_account = get_bank_gl_account($to_account);

	$total = 0;
	// do the source account postings
    $total += add_gl_trans($trans_type, $trans_no, $date_, $from_gl_account, 0, 0, "",
		-$amount, $currency);

    add_bank_trans($trans_type, $trans_no, $from_account, $ref,
		$date_, -$amount,
		payment_person_types::misc(), "", $currency,
		"Cannot insert a source bank transaction");

	// do the destination account postings
	$total += add_gl_trans($trans_type, $trans_no, $date_, $to_gl_account, 0, 0, "",
		$amount, $currency);
	/*Post a balance post if $total != 0 */
	add_gl_balance($trans_type, $trans_no, $date_, -$total);	
	
	add_bank_trans($trans_type, $trans_no, $to_account, $ref,
		$date_, $amount, payment_person_types::misc(), "",
		$currency, "Cannot insert a destination bank transaction");

	add_comments($trans_type, $trans_no, $date_, $memo_);

	references::save_last($ref, $trans_type);

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
	$person_type_id, $person_id, $person_detail_id,	$ref, $memo_)
{
	// we can only handle type 1 (payment)and type 2 (deposit)
	if ($trans_type != systypes::bank_payment() && $trans_type != systypes::bank_deposit())
		display_db_error("Invalid type ($trans_type) sent to add_bank_transaction");

	begin_transaction();

	$currency = get_bank_account_currency($from_account);
	$bank_gl_account = get_bank_gl_account($from_account);

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
    			$date_, $gl_item->amount, $person_type_id, $person_id, 
				$currency, "Cannot insert a destination bank transaction");
    	}
		// store tax details if the gl account is a tax account

		$amount = $gl_item->amount;
		add_gl_tax_details($gl_item->code_id, $trans_type, $trans_no, $amount,
			$date_, $memo);
	}

	// do the source account postings
    add_gl_trans($trans_type, $trans_no, $date_, $bank_gl_account, 0, 0, $memo_,
    	-$total, null, $person_type_id, $person_id);

	add_comments($trans_type, $trans_no, $date_, $memo_);

	references::save_last($ref, $trans_type);

	commit_transaction();

	return array($trans_type, $trans_no);
}

//----------------------------------------------------------------------------------------

?>