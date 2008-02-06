<?php

//----------------------------------------------------------------------------------------

function add_credit_note($credit_items, $credit_date, $credit_type, $tax_group_id,
	$charge_freight, $sales_type, $shipper, $reference, $memo_, $write_off_gl_code)
{
	begin_transaction();

	$branch_data = get_branch_accounts($credit_items->Branch);

	$credit_items_total = $credit_items->get_items_total($tax_group_id);

    $tax_total = 0;
    $taxes = $credit_items->get_taxes($tax_group_id, $charge_freight);
    foreach ($taxes as $taxitem)
    	$tax_total +=  $taxitem['Value'];

	$credit_no = add_customer_trans(11, $credit_items->customer_id, $credit_items->Branch,
    	$credit_date, $reference, -($credit_items_total), 0, -$tax_total, -$charge_freight,
    	$sales_type, 0, $credit_items->ship_via);

	foreach ($credit_items->line_items as $credit_line)
	{

		if ($credit_line->quantity > 0)
		{
		  	// the functions use qty_dispatched - so until that's fixed make sure it's set
			// or KABOOOOM
		  	$credit_line->qty_dispatched = $credit_line->quantity;

   		  	add_gl_trans_credit_order_item($credit_items, $credit_line, $credit_no, $credit_date,
    			$tax_group_id, $credit_type);

    		add_gl_trans_credit_costs($credit_items, $credit_line, $credit_no, $credit_date,
    			$tax_group_id, $credit_type, $write_off_gl_code, $branch_data);

		  } /*quantity credited is more than 0 */
	 } /*end of credit_line loop */

	/*Post credit note transaction to GL credit debtors,
	debit freight re-charged and debit sales */

	add_g_trans_credit_common($credit_items, $credit_no, $credit_date,
		$credit_items_total, $charge_freight, $taxes, $tax_total, $branch_data);

	add_comments(11, $credit_no, $credit_date, $memo_);

	add_forms_for_sys_type(11, $credit_no, $credit_items->Location);

	references::save_last($reference, 11);

	commit_transaction();

	return $credit_no;
}

//----------------------------------------------------------------------------------------

function credit_invoice($credit_items, $invoice_no, $order_no,
	$default_dispatch_date, $credit_type, $tax_group_id,
	$charge_freight, $reference, $memo_, $write_off_gl_code)
{
	begin_transaction();

	$branch_data = get_branch_accounts($credit_items->Branch);

	$invoice_alloc_balance = get_DebtorTrans_allocation_balance(10, $invoice_no);

	$invoice_items_total = $credit_items->get_items_total_dispatch($tax_group_id);

    $tax_total = 0;
    $taxes = $credit_items->get_taxes($tax_group_id, $charge_freight);
    foreach ($taxes as $taxitem)
    	$tax_total +=  $taxitem['Value'];

	$allocate_amount = 0;

	if ($invoice_alloc_balance > 0)
	{ 	/*the invoice is not already fully allocated */

		if ($invoice_alloc_balance > ($invoice_items_total + $charge_freight + $tax_total))
		{

			$allocate_amount = $invoice_items_total + $charge_freight + $tax_total;
		}
		else
		{ 	/*the balance left to allocate is less than the credit note value */
			$allocate_amount = $invoice_alloc_balance;
		}

		update_debtor_trans_allocation(10, $invoice_no, $allocate_amount);
	}

	/*Now insert the Credit Note into the debtor_trans table with the allocations as calculated above*/
	// all amounts in debtor's currency

	$credit_no = add_customer_trans(11, $credit_items->customer_id, $credit_items->Branch,
    	$default_dispatch_date, $reference,	-($invoice_items_total), 0, -$tax_total,
    	-$charge_freight, $credit_items->default_sales_type,	$order_no,
    	$credit_items->ship_via, "", $allocate_amount);

	/*Now insert the allocation record if > 0 */
	if ($allocate_amount != 0)
	{
		add_cust_allocation($allocate_amount, 11, $credit_no, 10, $invoice_no);
	}

	/* Update sales order details quantity invoiced less this credit quantity. */
	foreach ($credit_items->line_items as $order_line)
	{

		if ($order_line->qty_dispatched >0)
		{

			// always deduct the credited/returned quantities from the sales order
			dispatch_sales_order_item($order_no, $order_line->stock_id, -$order_line->qty_dispatched, $order_line->id);
			
			//if (($_POST['credit_type']=="Return") OR ($_POST['ItemsAction']==1))
			/*{
				$sql = "UPDATE ".TB_PREF."sales_order_details 
							SET qty_sent = qty_sent - " . $order_line->qty_dispatched . "
						WHERE order_no = " . $order_no . " 
						AND stk_code = '" . $order_line->stock_id . "'";
				$result = db_query($sql,"The sales order detail record could not be updated for the reduced quantity invoiced");
			}*/

			/*Now update sales_order_details for the quantity invoiced and the actual dispatch dates. */

			add_gl_trans_credit_order_item($credit_items, $order_line, $credit_no,
				$default_dispatch_date, $tax_group_id, $credit_type, $invoice_no);

		add_gl_trans_credit_costs($credit_items, $order_line, $credit_no, $default_dispatch_date,
			$tax_group_id, $credit_type, $write_off_gl_code, $branch_data);

		} /*quantity dispatched is more than 0 */
	} /*end of order_line loop */

	add_g_trans_credit_common($credit_items, $credit_no, $default_dispatch_date,
		$invoice_items_total, $charge_freight, $taxes, $tax_total, $branch_data);

	add_comments(11, $credit_no, $default_dispatch_date, $memo_);

	add_forms_for_sys_type(11, $credit_no);

	references::save_last($reference, 11);

	commit_transaction();

	return $credit_no;
}

//----------------------------------------------------------------------------------------

function add_gl_trans_credit_order_item(&$order, &$order_line, $credit_no, $date_, $tax_group_id,
	$credit_type, $credited_invoice=0)
{
	$line_taxfree_price = $order_line->taxfree_price($tax_group_id);
	$line_tax = $order_line->full_price() - $line_taxfree_price;

	if ($credit_type == "Return")
	{

		/* Insert stock movements for the stock coming back in - with unit cost */

	   	$reference = "";
	   	if ($credited_invoice)
	   		$reference .= "Ex Inv: " . $credited_invoice;

		add_customer_trans_detail_item(11, $credit_no, $order_line->stock_id,
			$order_line->item_description, $order->Location, $date_,
			$order_line->qty_dispatched, $line_taxfree_price, $line_tax,
			$order_line->discount_percent, $reference, $order_line->standard_cost);

	}
	elseif ($credit_type == "WriteOff")
	{
	   /*Insert a stock movement coming back in to show the credit note and
	   	a reversing stock movement to show the write off
	   	no mods to location stock records*/

	   $reference = "";
	   if ($credited_invoice)
	   		$reference .= "Ex Inv: " . $credited_invoice;

		add_customer_trans_detail_item_writeoff(11, $credit_no, $order_line->stock_id,
			$order_line->item_description, $order->Location, $date_,
			$order_line->qty_dispatched, $line_taxfree_price, $line_tax,
			$order_line->discount_percent, $reference, $order_line->standard_cost);
	}
}

//----------------------------------------------------------------------------------------

function add_gl_trans_credit_costs($order, $order_line, $credit_no, $date_, $tax_group_id,
	$credit_type, $write_off_gl_code, &$branch_data)
{
	$stock_gl_codes = get_stock_gl_code($order_line->stock_id);

	/* insert gl_trans to credit stock and debit cost of sales at standard cost*/
	if ($order_line->standard_cost != 0)
	{

		/*first the cost of sales entry*/

		add_gl_trans_std_cost(11, $credit_no, $date_, $stock_gl_codes["cogs_account"],
			$stock_gl_codes["dimension_id"], $stock_gl_codes["dimension2_id"], "",
			-($order_line->standard_cost * $order_line->qty_dispatched),
			payment_person_types::customer(), $order->customer_id,
			"The cost of sales GL posting could not be inserted");

		/*now the stock entry*/
		if ($credit_type == "WriteOff")
		{
			$stock_entry_account = $write_off_gl_code;
		}
		else
		{
			$stock_gl_code = get_stock_gl_code($order_line->stock_id);
			$stock_entry_account = $stock_gl_code["inventory_account"];
		}

		add_gl_trans_std_cost(11, $credit_no, $date_, $stock_entry_account, 0, 0,
			"", ($order_line->standard_cost * $order_line->qty_dispatched),
			payment_person_types::customer(), $order->customer_id,
			"The stock side (or write off) of the cost of sales GL posting could not be inserted");

	} /* end of if GL and stock integrated and standard cost !=0 */

	if ($order_line->price != 0)
	{

    	$line_taxfree_price = $order_line->taxfree_price($tax_group_id);
		$line_tax = $order_line->full_price() - $line_taxfree_price;

		//Post sales transaction to GL credit sales

		add_gl_trans_customer(11, $credit_no, $date_, $stock_gl_codes["sales_account"], 0, 0,
			($line_taxfree_price * $order_line->qty_dispatched), $order->customer_id,
			"The credit note GL posting could not be inserted");

		if ($order_line->discount_percent != 0)
		{

			add_gl_trans_customer(11, $credit_no, $date_, $branch_data["sales_discount_account"], 0, 0,
				-($line_taxfree_price * $order_line->qty_dispatched * $order_line->discount_percent),
				$order->customer_id,
				"The credit note discount GL posting could not be inserted");

		} /*end of if discount !=0 */
	} /*end of if sales integrated with debtors */
}

//----------------------------------------------------------------------------------------

function add_g_trans_credit_common($order, $credit_no, $date_,
	$credit_items_total, $freight, $taxes, $tax_total, &$branch_data)
{
	$company_data = get_company_prefs();

	/*Post credit note transaction to GL credit debtors,
	debit freight re-charged and debit sales */

	if (($credit_items_total + $freight + $tax_total) != 0)
	{

		add_gl_trans_customer(11, $credit_no, $date_, $branch_data["receivables_account"], 0, 0,
			-($credit_items_total + $freight + $tax_total), $order->customer_id,
			"The total debtor GL posting for the credit note could not be inserted");
	}

	if ($freight !=0)
	{
		add_gl_trans_customer(11, $credit_no, $date_, $company_data["freight_act"], 0, 0,
			$freight, $order->customer_id,
			"The freight GL posting for this credit note could not be inserted");
	}

    foreach ($taxes as $taxitem)
    {
    	if ($taxitem['Value'] != 0)
    	{

			add_customer_trans_tax_detail_item(11, $credit_no, $taxitem['tax_type_id'],
				$taxitem['rate'], $taxitem['included_in_price'], $taxitem['Value']);

    		add_gl_trans_customer(11, $credit_no, $date_, $taxitem['sales_gl_code'], 0, 0,
    			$taxitem['Value'], $order->customer_id,
    			"A tax GL posting for this credit note could not be inserted");
    	}
    }
}

//----------------------------------------------------------------------------------------

?>