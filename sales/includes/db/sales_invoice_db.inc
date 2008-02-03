<?php

// $bo_policy = 0, cancel remaining quantities on order
// = 1, backorder remaining quantities

function add_sales_invoice(&$invoice, $date_, $due_date, $sales_order, $tax_group_id,
	$charge_shipping, $location, $ship_via, $sales_type, $reference, $memo_, $bo_policy)
{
	begin_transaction();

	$company_data = get_company_prefs();

	$branch_data = get_branch_accounts($invoice->Branch);

	$invoice_items_total = $invoice->get_items_total_dispatch($tax_group_id);

    $tax_total = 0;
    $taxes = $invoice->get_taxes($tax_group_id, $charge_shipping);
    foreach ($taxes as $taxitem)
    	$tax_total +=  $taxitem['Value'];

	/*Now insert the debtor_trans */

	$invoice_no = add_customer_trans(10, $invoice->customer_id, $invoice->Branch, $date_,
		$reference, $invoice_items_total, 0, $tax_total, $charge_shipping,
    	$sales_type, $sales_order, $ship_via, $due_date);

	// If balance of the order cancelled update sales order details quantity.
	foreach ($invoice->line_items as $order_line)
	{

		if ($order_line->qty_dispatched != 0)
		{

			$line_taxfree_price = $order_line->taxfree_price($tax_group_id);
			$line_tax = $order_line->full_price() - $line_taxfree_price;

			// Now update sales_order_details for the quantity invoiced
			dispatch_sales_order_item($sales_order, $order_line->id,
				$order_line->qty_dispatched);

			/* add invoice details and stock movements */
			$memo_ = "";

			add_customer_trans_detail_item(10, $invoice_no, $order_line->stock_id,
				$order_line->item_description, $location, $date_,
				-$order_line->qty_dispatched, $line_taxfree_price, $line_tax,
				$order_line->discount_percent, $memo_, $order_line->standard_cost);

			$stock_gl_code = get_stock_gl_code($order_line->stock_id);

			/* insert gl_trans to credit stock and debit cost of sales at standard cost*/
			if ($order_line->standard_cost != 0)
			{

				/*first the cost of sales entry*/
                add_gl_trans_std_cost(10, $invoice_no, $date_, $stock_gl_code["cogs_account"],
                	$stock_gl_code["dimension_id"], $stock_gl_code["dimension2_id"], "",
                	$order_line->standard_cost * $order_line->qty_dispatched,
                	payment_person_types::customer(), $invoice->customer_id,
					"The cost of sales GL posting could not be inserted");

				/*now the stock entry*/
				//$stock_gl_code = get_stock_gl_code($order_line->stock_id);

                add_gl_trans_std_cost(10, $invoice_no, $date_, $stock_gl_code["inventory_account"], 0, 0, "",
                	(-$order_line->standard_cost * $order_line->qty_dispatched),
                	payment_person_types::customer(), $invoice->customer_id,
					"The stock side of the cost of sales GL posting could not be inserted");

			} /* end of if GL and stock integrated and standard cost !=0 */

			if ($order_line->price != 0)
			{

				//Post sales transaction to GL credit sales

                add_gl_trans_customer(10, $invoice_no, $date_, $stock_gl_code["sales_account"],
                	$stock_gl_code["dimension_id"], $stock_gl_code["dimension2_id"],
    				(-$line_taxfree_price * $order_line->qty_dispatched),
    				$invoice->customer_id, "The sales price GL posting could not be inserted");

				if ($order_line->discount_percent != 0)
				{

                	add_gl_trans_customer(10, $invoice_no, $date_,
    					$branch_data["sales_discount_account"], $stock_gl_code["dimension_id"],
    					$stock_gl_code["dimension2_id"],
    					($line_taxfree_price * $order_line->qty_dispatched * $order_line->discount_percent),
    					$invoice->customer_id, "The sales discount GL posting could not be inserted");
				} /*end of if discount !=0 */
			} /*end of if sales integrated with debtors */

		} /*quantity dispatched is more than 0 */
	} /*end of order_line loop */


	if ($bo_policy == 0)
	{
		// if cancelling any remaining quantities
		close_sales_order($sales_order);
	}

	/*Post debtors transaction to GL debit debtors, credit freight re-charged and credit sales */
	if (($invoice_items_total + $charge_shipping + $tax_total) != 0)
	{

    	add_gl_trans_customer(10, $invoice_no, $date_, $branch_data["receivables_account"], 0, 0,
			($invoice_items_total + $charge_shipping + $tax_total),
			$invoice->customer_id, "The total debtor GL posting could not be inserted");
	}

	if ($charge_shipping != 0)
	{

    	add_gl_trans_customer(10, $invoice_no, $date_, $company_data["freight_act"], 0, 0,
			(-$charge_shipping), $invoice->customer_id,
			"The freight GL posting could not be inserted");
	}

    foreach ($taxes as $taxitem)
    {
    	if ($taxitem['Value'] != 0)
    	{
			add_customer_trans_tax_detail_item(10, $invoice_no, $taxitem['tax_type_id'],
				$taxitem['rate'], $taxitem['included_in_price'], $taxitem['Value']);

        	add_gl_trans_customer(10, $invoice_no, $date_, $taxitem['sales_gl_code'], 0, 0,
    			(-$taxitem['Value']), $invoice->customer_id,
    			"A tax GL posting could not be inserted");
    	}
    }

	add_comments(10, $invoice_no, $date_, $memo_);

	add_forms_for_sys_type(10, $invoice_no, $location);

	references::save_last($reference, 10);

	commit_transaction();

	return $invoice_no;
}

//--------------------------------------------------------------------------------------------------

function void_sales_invoice($type, $type_no)
{
	begin_transaction();

	void_bank_trans($type, $type_no, true);
	void_gl_trans($type, $type_no, true);

	// for invoices and credits related to invoices,
	// reverse all the changes in the sales order
	$items_result = get_customer_trans_details($type, $type_no);

	$order = get_customer_trans_order($type, $type_no);

	if ($order)
	{
		while ($row = db_fetch($items_result))
		{
			dispatch_sales_order_item($order, $row["id"], $row["quantity"]);
		}
	}

	// clear details after they've been reversed in the sales order
	void_customer_trans_details($type, $type_no);

	void_customer_trans_tax_details($type, $type_no);

	void_cust_allocations($type, $type_no);

	// do this last because other voidings can depend on it - especially voiding
	// DO NOT MOVE THIS ABOVE VOIDING or we can end up with trans with alloc < 0
	void_customer_trans($type, $type_no);

	commit_transaction();
}

//--------------------------------------------------------------------------------------------------





?>