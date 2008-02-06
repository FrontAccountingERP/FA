<?php
function set_invoice_links(&$cart, $invoice) {
    if(count($cart->trans_no)==1) {
	  
	  $del = $cart->trans_no;
	  if(is_array($del)) $del = $del[0]; 
	  $sql = 'UPDATE debtor_trans SET trans_link = ' . $del .
		  ' WHERE type=10 AND trans_no='.$invoice;
  	  db_query($sql, 'Invoice link cannot be updated');    

	  foreach($cart->line_items as $line) {
	    if($line->quantity!=$line->qty_dispatched) {
		// this is partial invoice
		return 1;
	    }
	  }
	
	}

    $sql = 'UPDATE debtor_trans SET trans_link = ' . $invoice .
	' WHERE type=13 AND (';

    $deliveries = $cart->trans_no;
	
    foreach($deliveries as $key=>$del) 
	    $deliveries[$key] = 'trans_no='.$del;
    $sql .= implode(' OR ', $deliveries) . ')';
    
    db_query($sql, 'Delivery links cannot be updated');
 return 0; // batch or complete invoice
}

function add_sales_invoice(&$cart, $date_, $due_date, $tax_group_id, 
	$charge_shipping, $location, $ship_via, $sales_type, $reference, $memo_)
{
	begin_transaction();
	
	$company_data = get_company_prefs();
	
	$branch_data = get_branch_accounts($cart->Branch);	
	
	$cart_items_total = $cart->get_items_total_dispatch($tax_group_id);

	$delivery_no = $cart->trans_no;
	if(is_array($delivery_no)) $delivery_no = 0;
    
    $tax_total = 0;
    $taxes = $cart->get_taxes($tax_group_id, $charge_shipping);
    foreach ($taxes as $taxitem)
    	$tax_total +=  $taxitem['Value'];

	/*Now insert the debtor_trans */
	$sales_order=$cart->order_no;
	if(is_array($sales_order)) $sales_order = $sales_order[0]; // assume all crucial SO data are same for every delivery

	$invoice_no = add_customer_trans(10, $cart->customer_id, $cart->Branch, $date_, 
		$reference, $cart_items_total, 0, $tax_total, $charge_shipping, 
    	$sales_type, $sales_order, $delivery_no, $ship_via, $due_date);

	set_invoice_links($cart,$invoice_no);
    	
	// If balance of the order cancelled update sales order details quantity. 
	foreach ($cart->line_items as $delivery_line) 
	{

		if ($delivery_line->qty_dispatched != 0) 
		{
			
			$line_taxfree_price = $delivery_line->taxfree_price($tax_group_id);
			$line_tax = $delivery_line->full_price() - $line_taxfree_price;
		
			// Now update delivery items for the quantity invoiced 
			invoice_delivery_item($delivery_line->id,$delivery_line->qty_dispatched);
			
			/* add invoice details and stock movements */

			add_customer_trans_detail_item(10, $invoice_no, $delivery_line->stock_id,
				$delivery_line->item_description, $location, $date_, 
				-$delivery_line->qty_dispatched, $line_taxfree_price, $line_tax,
				$delivery_line->discount_percent, $reference, $delivery_line->standard_cost);
												
			$stock_gl_code = get_stock_gl_code($delivery_line->stock_id);												

			if ($delivery_line->price != 0)
			{

				//Post sales transaction to GL credit sales
				
         add_gl_trans_customer(10, $invoice_no, $date_, $stock_gl_code["sales_account"], 
           	$stock_gl_code["dimension_id"], $stock_gl_code["dimension2_id"], 
    				(-$line_taxfree_price * $delivery_line->qty_dispatched), 
    				$cart->customer_id, "The sales price GL posting could not be inserted");
				
				if ($delivery_line->discount_percent != 0)
				{
					
         	add_gl_trans_customer(10, $invoice_no, $date_, 
    					$branch_data["sales_discount_account"], $stock_gl_code["dimension_id"], 
    					$stock_gl_code["dimension2_id"],
    					($line_taxfree_price * $delivery_line->qty_dispatched * $delivery_line->discount_percent), 
    					$cart->customer_id, "The sales discount GL posting could not be inserted");							
				} /*end of if discount !=0 */
			} /*end of if sales integrated with debtors */

		} /*quantity dispatched is more than 0 */
	} /*end of delivery_line loop */

	/*Post debtors transaction to GL debit debtors, credit freight re-charged and credit sales */
	if (($cart_items_total + $charge_shipping + $tax_total) != 0) 
	{
    	add_gl_trans_customer(10, $invoice_no, $date_, $branch_data["receivables_account"], 0, 0,
			($cart_items_total + $charge_shipping + $tax_total), 
			$cart->customer_id, "The total debtor GL posting could not be inserted");				
	}

	if ($charge_shipping != 0) 
	{
    	add_gl_trans_customer(10, $invoice_no, $date_, $company_data["freight_act"], 0, 0,
			(-$charge_shipping), $cart->customer_id, 
			"The freight GL posting could not be inserted");				
	}
	
    foreach ($taxes as $taxitem) 
    {
    	if ($taxitem['Value'] != 0) 
    	{
			add_customer_trans_tax_detail_item(10, $invoice_no, $taxitem['tax_type_id'],
				$taxitem['rate'], $taxitem['included_in_price'], $taxitem['Value']);    		
    		
        	add_gl_trans_customer(10, $invoice_no, $date_, $taxitem['sales_gl_code'], 0, 0,
    			(-$taxitem['Value']), $cart->customer_id, 
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
	
	$delivery = get_customer_trans_link($type, $type_no);
	
	if ($delivery) 
	{
		while ($row = db_fetch($items_result))
		{
			invoice_delivery_item($row['id'], $row['quantity']);
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
function invoice_delivery_item($id, $qty_dispatched) 
{
	$sql = "UPDATE ".TB_PREF."debtor_trans_details ";

	$sql .= "SET qty_done = qty_done - $qty_dispatched ";

	$sql .= " WHERE id=$id";
//		AND debtor_trans_type = 13
//		AND debtor_trans_no = $delivery_no
//		AND stock_id = '$stock_id'";

	db_query($sql, "The sales delivery detail record could not be updated");
}

?>