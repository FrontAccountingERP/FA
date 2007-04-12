<?php

/* Definition of the cart class
this class can hold all the information for:

i)   a sales order
ii)  an invoice
iii) a credit note

*/

include_once($path_to_root . "/inventory/includes/inventory_db.inc");
include_once($path_to_root . "/taxes/tax_calc.inc");

class cart 
{

	var $line_items; /*array of objects of class line_details using the product id as the pointer */

	var $delivery_date;
	var $default_sales_type; // set to the customer's sales type
	var $sales_type_name; // set to customer's sales type name
	var $customer_currency; // set to the customer's currency
	var $default_discount; // set to the customer's discount %
	var $direct_invoice;  // direct invoicing
	var $memo_; // memo_ on direct invoicing
	
	var $deliver_to;
	var $delivery_address;
	var $phone;
	
	var $email;
	var $cust_ref;
	var $Comments;
	var $Location;
	var $location_name;
	
	var $order_no; // the order number 

	var $customer_name;
	var $customer_id;
	var $Branch;
	
	var $orig_order_date;
	
	var $ship_via;
	var $freight_cost;
	
	var $tax_group_id;
	var $tax_group_name;

	function Cart()
	{
		/*Constructor function initialises a new shopping cart */
		$this->line_items = array();
		$this->default_sales_type = "";
		$this->direct_invoice=false;
	}

	function add_to_cart($stock_id, $qty, $price, $disc, $qty_invoiced=0, $standard_cost=0, $description=null)
	{

		if (isset($stock_id) && $stock_id != "" && isset($qty) && $qty > 0)
		{

			$this->line_items[$stock_id] = new line_details($stock_id, $qty, $price, $disc, 
				$qty_invoiced, $standard_cost, $description);
			return 1;
		} 
		else 
		{
			// shouldn't come here under normal circumstances
			display_db_error("unexpected - adding an invalid item or null quantity", "", true);
		}
		return 0;
	}

	function find_cart_item($stock_id) 
	{
		if (isset($this->line_items[$stock_id]) && $this->line_items[$stock_id] != null)
			return $this->line_items[$stock_id];
		return null;
	}
	
	function update_cart_item($update_item, $qty, $price, $disc)
	{

		if ($qty > 0)
		{
			$this->line_items[$update_item]->quantity = $qty;
		}
		$this->line_items[$update_item]->price = $price;
		$this->line_items[$update_item]->discount_percent = $disc;
	}
	
	function update_add_cart_item_qty($update_item, $qty)
	{
		$this->line_items[$update_item]->quantity += $qty;
	}	

	function remove_from_cart(&$stock_id)
	{
		if (isset($stock_id))
		{
			unset($this->line_items[$stock_id]);
		}
	}
	
	function clear_items() 
	{
    	unset($this->line_items);
		$this->line_items = array();
		$this->default_sales_type = "";
		$this->customer_id = $this->order_no = 0;
	}
	
	function count_items() 
	{
		return count($this->line_items);
	}
	
	function get_items_total_dispatch($tax_group_id=null)
	{
		$total = 0;
		
		// preload the taxgroup !
		if ($tax_group_id != null)
			$tax_group_array = get_tax_group_items_as_array($tax_group_id);
		else 		
			$tax_group_array = null;
		
		foreach ($this->line_items as $ln_itm) 
		{
			$total += ($ln_itm->qty_dispatched * $ln_itm->taxfree_price($tax_group_id, $tax_group_array) * (1 - $ln_itm->discount_percent));			
		}		
		return $total;
	}
	
	function has_items_dispatch()
	{
		foreach ($this->line_items as $ln_itm) 
		{
			if ($ln_itm->qty_dispatched > 0)
				return true;			
		}		
		return false;
	}
	
	function get_items_total($tax_group_id=null)
	{
		$total = 0;
		
		// preload the taxgroup !
		if ($tax_group_id != null)
			$tax_group_array = get_tax_group_items_as_array($tax_group_id);
		else 		
			$tax_group_array = null;		
		
		foreach ($this->line_items as $ln_itm) {
			$total += ($ln_itm->quantity * $ln_itm->taxfree_price($tax_group_id, $tax_group_array) * (1 - $ln_itm->discount_percent));						
		}		
		return $total;
	}	

	function any_already_delivered()
	{
		/* Checks if there have been deliveries of line items */

		foreach ($this->line_items as $stock_item) 
		{
			if ($stock_item->qty_inv !=0)
			{
				return 1;
			}
		}

		return 0;

	}

	function some_already_delivered($stock_id)
	{
		/* Checks if there have been deliveries of a specific line item */
		if (isset($stock_id) && isset($this->line_items[$stock_id]) && 
			$this->line_items[$stock_id]->qty_inv != 0)
		{
			return 1;
		}
		return 0;
	}
	
    function get_taxes($tax_group_id=null, $shipping_cost=0)
    {
    	$items = array();
    	$prices = array();
    	
        if ($tax_group_id == null)
        	$tax_group_id = $this->tax_group_id;
        	
		// preload the taxgroup !
		$tax_group_array = get_tax_group_items_as_array($tax_group_id);
    	
        foreach ($this->line_items as $ln_itm) 
        {
        	$items[] = $ln_itm->stock_id;
        	$prices[] = ($ln_itm->qty_dispatched * $ln_itm->taxfree_price($tax_group_id, $tax_group_array) * (1 - $ln_itm->discount_percent));
        }
        	

        $taxes = get_tax_for_items($items, $prices, $shipping_cost, $tax_group_id, $tax_group_array);
        
        return $taxes;
    }	
	
} /* end of class defintion */

class line_details 
{
	var $stock_id;
	var $item_description;
	var $units;
	var $mb_flag;
	var $tax_type;
	var $tax_type_name;
			
	var $quantity;
	var $price;
	var $discount_percent;
	var $qty_inv;
	var $qty_dispatched;
	var $standard_cost;

	function line_details ($stock_id, $qty, $prc, $disc_percent,  
		$qty_invoiced, $standard_cost, $description)
	{
	/* Constructor function to add a new LineDetail object with passed params */
		$item_row = get_item($stock_id);
		
		if ($item_row == null)
			display_db_error("invalid item added to order : $stock_id", "");
		
		$this->mb_flag = $item_row["mb_flag"];
		$this->units = $item_row["units"];
		if ($description == null)
			$this->item_description = $item_row["description"];
		else
			$this->item_description = $description;
		//$this->standard_cost = $item_row["material_cost"] + $item_row["labour_cost"] + $item_row["overhead_cost"]; 
		$this->tax_type = $item_row["tax_type_id"];
		$this->tax_type_name = $item_row["tax_type_name"];

		$this->stock_id = $stock_id;
		$this->quantity = $qty;
		$this->price = $prc;
		$this->discount_percent = $disc_percent;
		$this->qty_inv = $qty_invoiced;
		$this->qty_dispatched = $qty - $qty_invoiced;
		$this->standard_cost = $standard_cost;
	}
	
	function full_price()
	{
		return $this->price;
	}
	
	function taxfree_price($tax_group_id, $tax_group_array=null)
	{
		if ($tax_group_id==null)
			return $this->price;
		return get_tax_free_price_for_item($this->stock_id, $this->price, $tax_group_id, $tax_group_array);
	}
}

?>
