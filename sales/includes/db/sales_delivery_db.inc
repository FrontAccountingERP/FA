<?php

// $bo_policy = 0, cancel remaining quantities on order
// = 1, backorder remaining quantities 

function add_sales_delivery(&$delivery, $date_, $due_date, $sales_order, $tax_group_id, 
	$charge_shipping, $location, $ship_via, $sales_type, $reference, $memo_, $bo_policy)
{
	begin_transaction();
	
	$company_data = get_company_prefs();
	
	$branch_data = get_branch_accounts($delivery->Branch);	
	
	$delivery_items_total = $delivery->get_items_total_dispatch($tax_group_id);
	
    $tax_total = 0;
    $taxes = $delivery->get_taxes($tax_group_id, $charge_shipping);
    foreach ($taxes as $taxitem)
    	$tax_total +=  $taxitem['Value'];

	/*Now insert the debtor_trans */
    $delivery_no = add_customer_trans(13, $delivery->customer_id, $delivery->Branch, $date_, 
		$reference, $delivery_items_total, 0, $tax_total, $charge_shipping, 
    $sales_type, $sales_order, 0, $ship_via, $due_date);
    	
    // If balance of this delivery cancelled update sales order details quantity. 
    foreach ($delivery->line_items as $delivery_line) 
    {
			$line_taxfree_price = $delivery_line->taxfree_price($tax_group_id);
			/* add delivery details for _all_ lines */
			add_customer_trans_detail_item(13, $delivery_no, $delivery_line->stock_id,
				$delivery_line->item_description, $location, $date_, 
				-$delivery_line->qty_dispatched, $line_taxfree_price, 0,
				$delivery_line->discount_percent, $reference, $delivery_line->standard_cost);

		if ($delivery_line->qty_dispatched != 0) 
		{
			// Now update sales_order_details for the quantity delivered 
			dispatch_sales_order_item($sales_order, $delivery_line->stock_id, 
				$delivery_line->qty_dispatched, $delivery_line->id);
			
			$stock_gl_code = get_stock_gl_code($delivery_line->stock_id);

			/* insert gl_trans to credit stock and debit cost of sales at standard cost*/
			if ($delivery_line->standard_cost != 0)
			{

				/*first the cost of sales entry*/
                add_gl_trans_std_cost(13, $delivery_no, $date_, $stock_gl_code["cogs_account"], 
                	$stock_gl_code["dimension_id"], $stock_gl_code["dimension2_id"], "", 
                	$delivery_line->standard_cost * $delivery_line->qty_dispatched, 
                	payment_person_types::customer(), $delivery->customer_id, 
					"The cost of sales GL posting could not be inserted");
										
				/*now the stock entry*/
				//$stock_gl_code = get_stock_gl_code($order_line->stock_id);
				
                add_gl_trans_std_cost(13, $delivery_no, $date_, $stock_gl_code["inventory_account"], 0, 0, "", 
                	(-$delivery_line->standard_cost * $delivery_line->qty_dispatched), 
                	payment_person_types::customer(), $delivery->customer_id, 
					"The stock side of the cost of sales GL posting could not be inserted");				

			} /* end of if GL and stock integrated and standard cost !=0 */

		} /*quantity dispatched is more than 0 */
	} /*end of order_line loop */

	if ($bo_policy == 0)
	{
		// if cancelling any remaining quantities
		close_sales_order($sales_order);
	}
    
	add_comments(13, $delivery_no, $date_, $memo_);    
	
	add_forms_for_sys_type(13, $delivery_no, $location);
	
	references::save_last($reference, 13);	
	
	commit_transaction();	

	return $delivery_no;
}

//--------------------------------------------------------------------------------------------------

function void_sales_delivery($type, $type_no)
{
	begin_transaction();
	
	void_gl_trans($type, $type_no, true);
	
	// for invoices and credits related to invoices, 
	// reverse all the changes in the sales order
	$items_result = get_customer_trans_details($type, $type_no);
	
	$order = get_customer_trans_order($type, $type_no);
	
	if ($order) 
	{
		while ($row = db_fetch($items_result))
		{
			dispatch_sales_order_item($order, $row["stock_id"], $row["quantity"]);
		}
	}
	
	// clear details after they've been reversed in the sales order
	void_customer_trans_details($type, $type_no);
	
	void_customer_trans_tax_details($type, $type_no);	
	
	void_cust_allocations($type, $type_no);
	
	// do this last because other voidings can depend on it
	// DO NOT MOVE THIS ABOVE VOIDING or we can end up with trans with alloc < 0
	void_customer_trans($type, $type_no);	
	
	commit_transaction();			
}

//----------------------------------------------------------------------------------------

function get_sales_delivery($trans_no)
{
	$sql = "SELECT ".TB_PREF."debtor_trans.*,
	".TB_PREF."cust_branch.*,
	".TB_PREF."debtors_master.name, 
	".TB_PREF."debtors_master.curr_code, 
	".TB_PREF."debtors_master.payment_terms, 
	".TB_PREF."debtors_master.discount, 
	".TB_PREF."sales_types.sales_type, 
	".TB_PREF."shippers.shipper_name,
	".TB_PREF."tax_groups.name AS tax_group_name , 
	".TB_PREF."tax_groups.id AS tax_group_id
		FROM ".TB_PREF."debtor_trans, 
			".TB_PREF."debtors_master, 
			".TB_PREF."sales_types, 
			".TB_PREF."tax_groups, 
			".TB_PREF."cust_branch, 
			".TB_PREF."shippers
		WHERE	".TB_PREF."debtor_trans.type=13
			  AND ".TB_PREF."debtor_trans.tpe=".TB_PREF."sales_types.id
				AND ".TB_PREF."cust_branch.branch_code = ".TB_PREF."debtor_trans.branch_code
				AND ".TB_PREF."cust_branch.tax_group_id = ".TB_PREF."tax_groups.id
				AND ".TB_PREF."debtor_trans.debtor_no = ".TB_PREF."debtors_master.debtor_no
			  AND ".TB_PREF."shippers.shipper_id = ".TB_PREF."debtor_trans.ship_via
				AND ".TB_PREF."debtor_trans.trans_no = ";
				
	if(is_array($trans_no)) { //assume all transactions are for the same customer/branch
	    $sql .= $trans_no[0];
	} else {
	    $sql .= $trans_no;
	}

	$result = db_query($sql, "delivery Retreival");

	$num = db_num_rows($result);
	if ($num > 1)
	{
		display_db_error("FATAL : delivery note query returned a duplicate - " . db_num_rows($result), $sql, true);
	}
	else if ($num == 1)
	{
		return db_fetch($result);
	}
	else
		display_db_error("FATAL : delivery note search return nothing - " . db_num_rows($result), $sql, true);

}

//function get_sales_delivery_details($delivery) {
//    if(!is_array($delivery)) {
//
//    }
//}
//--------------------------------------------------------------------------------------------------

function read_sales_delivery($trans_no, &$cart)
{
	$myrow = get_sales_delivery($trans_no);

	if(!is_array($trans_no)) $trans_no = array($trans_no);

	$cart->customer_id = $myrow["debtor_no"];
	$cart->Branch = $myrow["branch_code"];
	$cart->customer_name = $myrow["name"];
	$cart->cust_ref = $myrow["reference"];
	$cart->default_sales_type =$myrow["tpe"];
	$cart->sales_type_name =$myrow["sales_type"];
	$cart->customer_currency = $myrow["curr_code"];
	$cart->default_discount = $myrow["discount"];
	$cart->order_no = $myrow["order_"];

//	$cart->Comments = get_comments(13,$trans_no);

	$cart->ship_via = $myrow["ship_via"];
	$cart->deliver_to = $myrow["br_name"];
	$cart->delivery_date = sql2date($myrow["due_date"]);
	$cart->freight_cost = $myrow["ov_freight"];
	$cart->delivery_address = $myrow["br_address"];
	$cart->phone = $myrow["phone"];
	$cart->email = $myrow["email"];

//	$cart->Location = $myrow["location"]; 
//	$cart->location_name = $myrow["location_name"];

	$cart->orig_order_date = sql2date($myrow["tran_date"]);

	$cart->tax_group_name = $myrow["tax_group_name"];
	$cart->tax_group_id = $myrow["tax_group_id"];

	$sql = "SELECT ".TB_PREF."debtor_trans_details.stock_id, unit_price, 
		debtor_trans_no,
		".TB_PREF."debtor_trans_details.description,
		".TB_PREF."debtor_trans_details.id,
		".TB_PREF."debtor_trans_details.quantity, discount_percent,
		qty_done, ".TB_PREF."stock_master.units,
		".TB_PREF."stock_master.material_cost + ".TB_PREF."stock_master.labour_cost + ".TB_PREF."stock_master.overhead_cost AS standard_cost
		FROM ".TB_PREF."debtor_trans_details, ".TB_PREF."stock_master
			WHERE ".TB_PREF."debtor_trans_details.debtor_trans_type=13 AND
			" .TB_PREF."debtor_trans_details.stock_id = ".TB_PREF."stock_master.stock_id";

	    $sql .= ' AND (';
	    foreach($trans_no as $key=>$num) {
		if($key!=0) $sql .= ' OR ';
		$sql .= 'debtor_trans_no =' . $num;
	    }
	    $sql .= ') ORDER BY id';

	$result = db_query($sql, "Retreive Delivery Note Items");

	if (db_num_rows($result) > 0)
	{

		while ($myrow = db_fetch($result))
		{
			$cart->add_to_cart($myrow["stock_id"],-$myrow["quantity"],
				$myrow["unit_price"], $myrow["discount_percent"],
				 -$myrow["qty_done"], $myrow["standard_cost"], $myrow["description"],$myrow["id"]);
		}
	}

	foreach($cart->line_items as $key=>$line) 
	  $cart->line_items[$key]->qty_dispatched = 
	  $cart->line_items[$key]->quantity - $cart->line_items[$key]->qty_done; 

	return true;
}

?>