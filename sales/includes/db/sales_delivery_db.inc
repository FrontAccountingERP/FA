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
//-----------------------------------------------------------------------------
// insert/update sales delivery
//
function write_sales_delivery(&$delivery,$bo_policy)
{
	global $Refs;

	$trans_no = $delivery->trans_no;
	if (is_array($trans_no)) $trans_no = key($trans_no);

	begin_transaction();
	$delivery->bo_policy = $bo_policy;
	hook_db_prewrite($delivery, ST_CUSTDELIVERY);

	$customer = get_customer($delivery->customer_id);

	add_new_exchange_rate($customer['curr_code'], $delivery->document_date, $delivery->ex_rate);

	$delivery_items_total = $delivery->get_items_total_dispatch();
	$freight_tax = $delivery->get_shipping_tax();

// mark sales order for concurrency conflicts check
	update_sales_order_version($delivery->src_docs);

	$tax_total = 0;
	$taxes = $delivery->get_taxes(); // all taxes with freight_tax

	foreach ($taxes as $taxitem) {
		$taxitem['Value'] =  round2($taxitem['Value'], user_price_dec());
		$tax_total +=  $taxitem['Value'];
	}
	/* Insert/update the debtor_trans */
	$delivery_no = write_customer_trans(ST_CUSTDELIVERY, $trans_no, $delivery->customer_id,
		$delivery->Branch, $delivery->document_date, $delivery->reference,
		$delivery_items_total, 0,
		$delivery->tax_included ? 0 : $tax_total-$freight_tax,
		$delivery->freight_cost,
		$delivery->tax_included ? 0 : $freight_tax,
		$delivery->sales_type, $delivery->order_no,
		$delivery->ship_via, $delivery->due_date, 0, 0, $delivery->dimension_id, 
		$delivery->dimension2_id, $delivery->payment);

	if ($trans_no == 0) {
		$delivery->trans_no = array($delivery_no=>0);
	} else {
		void_gl_trans(ST_CUSTDELIVERY, $delivery_no, true);
		void_stock_move(ST_CUSTDELIVERY, $delivery_no);
		void_trans_tax_details(ST_CUSTDELIVERY, $delivery_no);
		delete_comments(ST_CUSTDELIVERY, $delivery_no);
	}

	foreach ($delivery->line_items as $line_no => $delivery_line) {

		$line_price = $delivery_line->line_price();
		$line_taxfree_price = get_tax_free_price_for_item($delivery_line->stock_id,
			$delivery_line->price, 0, $delivery->tax_included,
			$delivery->tax_group_array);

		$line_tax = get_full_price_for_item($delivery_line->stock_id, $delivery_line->price,
			0, $delivery->tax_included, $delivery->tax_group_array) - $line_taxfree_price;

		//if ($trans_no != 0) // Inserted 2008-09-25 Joe Hunt. This condition is removed after experience by Chaitanya
			$delivery_line->standard_cost = get_standard_cost($delivery_line->stock_id);

		/* add delivery details for all lines */
		write_customer_trans_detail_item(ST_CUSTDELIVERY, $delivery_no, $delivery_line->stock_id,
			$delivery_line->item_description, $delivery_line->qty_dispatched,
			$delivery_line->line_price(), $line_tax,
			$delivery_line->discount_percent, $delivery_line->standard_cost, $delivery_line->src_id,
			$trans_no ? $delivery_line->id : 0);

	// Now update sales_order_details for the quantity delivered
		if ($delivery_line->qty_old != $delivery_line->qty_dispatched)
			update_parent_line(ST_CUSTDELIVERY, $delivery_line->src_id,
				$delivery_line->qty_dispatched-$delivery_line->qty_old);

		if ($delivery_line->qty_dispatched != 0) {
			add_stock_move_customer(ST_CUSTDELIVERY, $delivery_line->stock_id, $delivery_no,
				$delivery->Location, $delivery->document_date, $delivery->reference,
				-$delivery_line->qty_dispatched, $delivery_line->standard_cost,1,
				$line_price, $delivery_line->discount_percent);


			$stock_gl_code = get_stock_gl_code($delivery_line->stock_id);

			/* insert gl_trans to credit stock and debit cost of sales at standard cost*/
			if (is_inventory_item($delivery_line->stock_id) && $delivery_line->standard_cost != 0) {

				/*first the cost of sales entry*/
				// 2008-08-01. If there is a Customer Dimension, then override with this,
				// else take the Item Dimension (if any)
				$dim = ($delivery->dimension_id != $customer['dimension_id'] ? $delivery->dimension_id : 
					($customer['dimension_id'] != 0 ? $customer["dimension_id"] : $stock_gl_code["dimension_id"]));
				$dim2 = ($delivery->dimension2_id != $customer['dimension2_id'] ? $delivery->dimension2_id : 
					($customer['dimension2_id'] != 0 ? $customer["dimension2_id"] : $stock_gl_code["dimension2_id"]));

				add_gl_trans_std_cost(ST_CUSTDELIVERY, $delivery_no,
					$delivery->document_date, $stock_gl_code["cogs_account"], $dim, $dim2, "",
					$delivery_line->standard_cost * $delivery_line->qty_dispatched,
					PT_CUSTOMER, $delivery->customer_id,
					"The cost of sales GL posting could not be inserted");

				/*now the stock entry*/

				add_gl_trans_std_cost(ST_CUSTDELIVERY, $delivery_no, $delivery->document_date,
					$stock_gl_code["inventory_account"], 0, 0, "",
					(-$delivery_line->standard_cost * $delivery_line->qty_dispatched),
					PT_CUSTOMER, $delivery->customer_id,
					"The stock side of the cost of sales GL posting could not be inserted");

			} /* end of if GL and stock integrated and standard cost !=0 */

		} /*quantity dispatched is more than 0 */
	} /*end of order_line loop */

	if ($bo_policy == 0) {
		// if cancelling any remaining quantities
		close_sales_order($delivery->order_no);
	}

	// taxes - this is for printing purposes
	foreach ($taxes as $taxitem) {
		if ($taxitem['Net'] != 0) {
				$ex_rate = get_exchange_rate_from_home_currency(get_customer_currency($delivery->customer_id), $delivery->document_date);
    		    add_trans_tax_details(ST_CUSTDELIVERY, $delivery_no, $taxitem['tax_type_id'],
				$taxitem['rate'], $delivery->tax_included, $taxitem['Value'],
				$taxitem['Net'], $ex_rate, $delivery->document_date, $delivery->reference );
		}
	}

	add_comments(ST_CUSTDELIVERY, $delivery_no, $delivery->document_date, $delivery->Comments);

	if ($trans_no == 0) {
			$Refs->save(ST_CUSTDELIVERY, $delivery_no, $delivery->reference);
	}

	hook_db_postwrite($delivery, ST_CUSTDELIVERY);
	commit_transaction();

	return $delivery_no;
}

//--------------------------------------------------------------------------------------------------
function adjust_shipping_charge(&$delivery, $trans_no)
{
  	$sql = "SELECT sum(ov_freight) as freight FROM ".TB_PREF."debtor_trans WHERE order_ = $trans_no AND type = " . ST_CUSTDELIVERY . " AND debtor_no = " . $delivery->customer_id;
  	$result = db_query($sql, "Can not find delivery notes");
  	$row = db_fetch_row($result);
  	if (!$row[0]) $freight = 0;
  	else $freight = $row[0];
  	if ($freight < $delivery->freight_cost) $delivery->freight_cost = $delivery->freight_cost - $freight;
  	else $delivery->freight_cost = 0;
}

//--------------------------------------------------------------------------------------------------

function void_sales_delivery($type, $type_no, $transactions=true)
{
	if ($transactions)
		begin_transaction();

	hook_db_prevoid($type, $type_no);
	
	void_gl_trans($type, $type_no, true);

	// reverse all the changes in the sales order
	$items_result = get_customer_trans_details($type, $type_no);

	$order = get_customer_trans_order($type, $type_no);

	if ($order) {
		$auto = (get_reference(ST_SALESORDER, $order) == "auto");
		while ($row = db_fetch($items_result)) {
			update_parent_line(ST_CUSTDELIVERY, $row['src_id'], -$row['quantity'], $auto);
		}
	}

	// clear details after they've been reversed in the sales order
	void_customer_trans_details($type, $type_no);

	void_trans_tax_details($type, $type_no);

	void_cust_allocations($type, $type_no);

	// do this last because other voidings can depend on it
	// DO NOT MOVE THIS ABOVE VOIDING or we can end up with trans with alloc < 0
	void_customer_trans($type, $type_no);

	if ($transactions)
		commit_transaction();
}
?>