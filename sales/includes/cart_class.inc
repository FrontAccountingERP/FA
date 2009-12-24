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
/* Definition of the cart class
this class can hold all the information for:

i)   a sales order
ii)  an invoice
iii) a credit note
iv)  a delivery note
*/

include_once($path_to_root . "/inventory/includes/inventory_db.inc");
include_once($path_to_root . "/taxes/tax_calc.inc");

class cart
{
	var $trans_type; // invoice, order, quotation, delivery note ...
	var $trans_no = array();// array (num1=>ver1,..) or 0 for new
	var $so_type = 0;		// for sales order: simple=0 template=1
	var $cart_id;		// used to detect multi-tab edition conflits
	var $line_items;  //array of objects of class line_details

	var $src_docs = array();	// array of arrays(num1=>ver1,...) or 0 for no src
	var $src_date;			// src document date (for info only)

	var $document_date;
	var $due_date;
	var $sales_type; // set to the customer's sales type
	var $sales_type_name; // set to customer's sales type name
	var $tax_included;

	var $customer_currency; // set to the customer's currency
	var $default_discount; // set to the customer's discount %
	var $customer_name;
	var $customer_id;
	var $Branch;
	var $email;

	var $deliver_to;
	var $delivery_address;
	var $phone;

	var $cust_ref;
	var $reference;
	var $Comments;
	var $Location;
	var $location_name;
	var $order_no; 		// the original order number
	var $trans_link = 0;

	var $ship_via;
	var $freight_cost = 0;

	var $tax_group_id;
	var $tax_group_name;
	var $tax_group_array = null; // saves db queries
	var $price_factor;	 // ditto for price calculations

	var	$pos;			// user assigned POS
	var $cash;			// cash transaction
	var $cash_account;
	var $account_name;
	var $cash_discount;	// not used as of FA 2.1
	var $dimension_id;
	var $dimension2_id;
	//-------------------------------------------------------------------------
	//
	//  $trans_no==0 => open new/direct document
	//  $trans_no!=0 && $view==false => read for view
	//  $trans_no!=0 && $view==true => read for edit (qty update from parent doc)
	//
	function Cart($type, $trans_no=0, $view=false ) {
		/*Constructor function initialises a new shopping cart */
		$this->line_items = array();
		$this->sales_type = "";
		if ($type == ST_SALESQUOTE)
			$this->trans_type = $type;
		else	
			$this->trans_type = ST_SALESORDER;
		$this->dimension_id = 0;
		$this->dimension2_id = 0;
		$this->read($type, $trans_no, $view );
		$this->cart_id = uniqid('');
	}

	//-------------------------------------------------------------------------
	// Reading document into cart
	//
	function read($type, $trans_no = 0, $view=false ) {

		global $SysPrefs, $Refs;

		if (!is_array($trans_no)) $trans_no = array($trans_no);
		if ($trans_no[0]) // read old transaction
		{
			if ($type == ST_SALESORDER || $type == ST_SALESQUOTE) { // sales order || sales quotation
				read_sales_order($trans_no[0], $this, $type);
				if ($view) {	// prepare for DN/IV entry
					for($line_no = 0; $line_no < count($this->line_items); $line_no++) {
						$line = &$this->line_items[$line_no];
						$line->src_id = $line->id; // save src line ids for update
						$line->qty_dispatched = $line->quantity - $line->qty_done;
					}
				}
			} else {	// other type of sales transaction
					read_sales_trans($type, $trans_no, $this);
					if ($this->order_no) { // free hand credit notes have no order_no
						$sodata = get_sales_order_header($this->order_no, ST_SALESORDER);
						$this->cust_ref = $sodata["customer_ref"];
					// currently currency is hard linked to debtor account
					//	$this->customer_currency = $sodata["curr_code"];
						$this->delivery_to = $sodata["deliver_to"];
						$this->delivery_address = $sodata["delivery_address"];
					}
					// old derivative transaction edit
					if (!$view && ($type!=ST_CUSTCREDIT || $this->trans_link!=0)) {
						$src_type = get_parent_type($type);
						if ($src_type == ST_SALESORDER) { // get src data from sales_orders
							$this->src_docs = array( $sodata['order_no']=>$sodata['version']);
							$srcdetails = get_sales_order_details($this->order_no, ST_SALESORDER);
						} else {	// get src_data from debtor_trans
							$this->src_docs = get_customer_trans_version($src_type, get_parent_trans($type,$trans_no[0]));
							$srcdetails = get_customer_trans_details($src_type,array_keys($this->src_docs));
						}
						// calculate & save: qtys on other docs and free qtys on src doc
						for($line_no = 0; $srcline = db_fetch($srcdetails); $line_no++) {
							$sign = 1; // $type==13 ?  1 : -1; // this is strange debtor_trans atavism
							$line = &$this->line_items[$line_no];

							$line->src_id = $srcline['id']; // save src line ids for update
							$line->qty_old = $line->qty_dispatched = $line->quantity;
							$line->quantity += $sign * ($srcline['quantity'] - $srcline['qty_done']); // add free qty on src doc
						}
					} else { // prepare qtys for derivative document entry (not used in display)
						for($line_no = 0; $line_no < count($this->line_items); $line_no++) {
							$line = &$this->line_items[$line_no];
							$line->src_id = $line->id; // save src line ids for update
							$line->qty_dispatched = $line->quantity - $line->qty_done;
						}
					}
				}
		} else { // new document
				$this->trans_type = $type;
				$this->trans_no = 0;
				$this->customer_currency = get_company_currency();
				// set new sales document defaults here
				if (get_global_customer() != ALL_TEXT)
				  $this->customer_id = get_global_customer();
				else
				  $this->customer_id = '';
				$this->document_date = 	new_doc_date();
				if (!is_date_in_fiscalyear($this->document_date))
					$this->document_date = end_fiscalyear();
				$this->reference = $Refs->get_next($this->trans_type);
				if ($type != ST_SALESORDER && $type != ST_SALESQUOTE) // Added 2.1 Joe Hunt 2008-11-12
				{
					$dim = get_company_pref('use_dimension');
					if ($dim > 0)
					{
						if ($this->customer_id == '')
							$this->dimension_id = 0;
						else
						{
							$cust = get_customer($this->customer_id);
							$this->dimension_id = $cust['dimension_id'];
						}	
						if ($dim > 1)
						{
							if ($this->customer_id == '')
								$this->dimension2_id = 0;
							else
								$this->dimension2_id = $cust['dimension2_id'];
						}		
					}		
				}	
				if ($type == ST_SALESINVOICE) {
				  $this->due_date =
					get_invoice_duedate($this->customer_id, $this->document_date);
				  $this->pos = user_pos();
				  $pos = get_sales_point($this->pos);
				  $this->cash = !$pos['credit_sale'];
				  if (!$pos['cash_sale'] || !$pos['credit_sale']) 
				  	$this->pos = -1; // mark not editable payment type
				  else
					$this->cash = date_diff2($this->due_date, Today(), 'd')<2;
				if ($this->cash) {
					$this->Location = $pos['pos_location'];
					$this->location_name = $pos['location_name'];
					$this->cash_account = $pos['pos_account'];
					$this->account_name = $pos['bank_account_name'];
				}
				} else
				  $this->due_date =
					add_days($this->document_date, $SysPrefs->default_delivery_required_by());
			}
	}

	//-------------------------------------------------------------------------
	// Writing new/modified sales document to database.
	// Makes parent documents for direct delivery/invoice by recurent call.
	// $policy - 0 or 1:  writeoff/return for IV, back order/cancel for DN
	function write($policy=0) {
		if (count($this->src_docs) == 0 && ($this->trans_type == ST_SALESINVOICE || $this->trans_type == ST_CUSTDELIVERY)) {
			// this is direct document - first add parent
			$src = (PHP_VERSION<5) ? $this : clone( $this ); // make local copy of this cart
			$src->trans_type = get_parent_type($src->trans_type);
			$src->reference = 'auto';

			$src->write(1);
			$type = $this->trans_type;
			$ref = $this->reference;
			$date = $this->document_date;
			// re-read document
			$this->read($src->trans_type, key($src->trans_no), true);
			$this->document_date = $date;
			$this->reference = $ref;
			$this->trans_type = $type;
			$this->src_docs= $this->trans_no;
			$this->trans_no = 0;
			$this->order_no= $this->trans_type==ST_CUSTDELIVERY ? key($src->trans_no) : $src->order_no;
		}
		$this->reference = @html_entity_decode($this->reference, ENT_QUOTES);
		$this->Comments = @html_entity_decode($this->Comments, ENT_QUOTES);
		foreach($this->line_items as $lineno => $line) {
			$this->line_items[$lineno]->stock_id = @html_entity_decode($line->stock_id, ENT_QUOTES);
			$this->line_items[$lineno]->item_description = @html_entity_decode($line->item_description, ENT_QUOTES);
		}
		switch($this->trans_type) {
			case ST_SALESINVOICE:
				return write_sales_invoice($this);
			case ST_CUSTCREDIT:
				return write_credit_note($this, $policy);
			case ST_CUSTDELIVERY:
				return write_sales_delivery($this, $policy);
			case ST_SALESORDER:
			case ST_SALESQUOTE:
				if ($this->trans_no==0)	// new document
					return add_sales_order($this);
				else
					return update_sales_order($this);
		}
	}

	function set_customer($customer_id, $customer_name, $currency, $discount, $cdiscount=0)
	{
		$this->customer_name = $customer_name;
		$this->customer_id = $customer_id;
		$this->default_discount = $discount;
		$this->cash_discount = $cdiscount;
		$this->customer_currency = $currency;
	}

	function set_branch($branch_id, $tax_group_id, $tax_group_name, $phone='', $email='')
	{
		$this->Branch = $branch_id;
		$this->phone = $phone;
		$this->email = $email;
		$this->tax_group_id = $tax_group_id;
		$this->tax_group_array = get_tax_group_items_as_array($tax_group_id);
	}

	function set_sales_type($sales_type, $sales_name, $tax_included=0, $factor=0)
	{
	    $this->sales_type = $sales_type;
	    $this->sales_type_name = $sales_name;
	    $this->tax_included = $tax_included;
	    $this->price_factor = $factor;
	}

	function set_location($id, $name)
	{
		$this->Location = $id;
		$this->location_name = $name;
	}

	function set_delivery($shipper, $destination, $address, $freight_cost=null)
	{
		$this->ship_via = $shipper;
		$this->deliver_to = $destination;
		$this->delivery_address = $address;
		if (isset($freight_cost))
			$this->freight_cost = $freight_cost;
	}

	function add_to_cart($line_no,$stock_id, $qty, $price, $disc, $qty_done=0, $standard_cost=0, $description=null, $id=0, $src_no=0)
	{
		if (isset($stock_id) && $stock_id != "" && isset($qty)/* && $qty > 0*/)	{
			$this->line_items[$line_no] = new line_details($stock_id, $qty, $price, $disc,
				$qty_done,  $standard_cost, $description, $id, $src_no);
			return 1;
		} else {
			// shouldn't come here under normal circumstances
			display_db_error("unexpected - adding an invalid item or null quantity", "", true);
		}
		return 0;
	}

	function update_cart_item($line_no, $qty, $price, $disc, $description="")
	{
		if ($description != "")
			$this->line_items[$line_no]->item_description = $description;
		$this->line_items[$line_no]->quantity = $qty;
		$this->line_items[$line_no]->qty_dispatched = $qty;
		$this->line_items[$line_no]->price = $price;
		$this->line_items[$line_no]->discount_percent = $disc;
	}

	function update_add_cart_item_qty($line_no, $qty)
	{
		$this->line_items[$line_no]->quantity += $qty;
	}

	function remove_from_cart($line_no)
	{
		array_splice($this->line_items, $line_no, 1);
	}

	function clear_items()
	{
		unset($this->line_items);
		$this->line_items = array();
		$this->sales_type = "";
		$this->trans_no = 0;
		$this->customer_id = $this->order_no = 0;
	}

	function count_items()
	{
		$counter=0;
		foreach ($this->line_items as $line) {
			if ($line->quantity!=$line->qty_done) $counter++;
		}
		return $counter;
	}

	function get_items_total()
	{
		$total = 0;

		foreach ($this->line_items as $ln_itm) {
			$price = $ln_itm->line_price();
			$total += round($ln_itm->quantity * $price * (1 - $ln_itm->discount_percent), 
			   user_price_dec());
		}
		return $total;
	}

	function get_items_total_dispatch()
	{
		$total = 0;

		foreach ($this->line_items as $ln_itm) {
			$price = $ln_itm->line_price();
			$total += round(($ln_itm->qty_dispatched * $price * (1 - $ln_itm->discount_percent)), 
			   user_price_dec());
		}
		return $total;
	}

	function has_items_dispatch()
	{
		foreach ($this->line_items as $ln_itm) {
			if ($ln_itm->qty_dispatched > 0)
				return true;
		}
		return false;
	}

	function any_already_delivered()
	{
		/* Checks if there have been any line item processed */

		foreach ($this->line_items as $stock_item) {
			if ($stock_item->qty_done !=0) {
				return 1;
			}
		}

		return 0;

	}

	function some_already_delivered($line_no)
	{
		/* Checks if there have been deliveries of a specific line item */
		if (isset($this->line_items[$line_no]) &&
			$this->line_items[$line_no]->qty_done != 0) {
			return 1;
		}
		return 0;
	}

	function get_taxes($shipping_cost=null)
	{
		$items = array();
		$prices = array();
		if($shipping_cost==null)
			$shipping_cost = $this->freight_cost;

		foreach ($this->line_items as $ln_itm) {
			$items[] = $ln_itm->stock_id;
			$prices[] = round(($ln_itm->qty_dispatched *
				$ln_itm->line_price()* (1 - $ln_itm->discount_percent)),  user_price_dec());
		}

		$taxes = get_tax_for_items($items, $prices, $shipping_cost,
		  $this->tax_group_id, $this->tax_included,  $this->tax_group_array);

    // Adjustment for swiss franken, we always have 5 rappen = 1/20 franken
    if ($this->customer_currency == 'CHF') {
			$val = $taxes['1']['Value'];
      $val1 = (floatval((intval(round(($val*20),0)))/20));
			$taxes['1']['Value'] = $val1;
		} 
		return $taxes;
	}


	function get_tax_free_shipping()
	{

		if ($this->tax_included==0)
			return $this->freight_cost;
		else
			return ($this->freight_cost - $this->get_shipping_tax());
	}

	function get_shipping_tax()
	{

		$tax_items = get_shipping_tax_as_array();
		$tax_rate = 0;
		if ($tax_items != null) {
			foreach ($tax_items as $item_tax) {
				$index = $item_tax['tax_type_id'];
				if (isset($this->tax_group_array[$index])) {
					$tax_rate += $item_tax['rate'];
				}
			}
		}
		if($this->tax_included)
			return round($this->freight_cost*$tax_rate/($tax_rate+100),  user_price_dec());
		else
			return round($this->freight_cost*$tax_rate/100,  user_price_dec());
	}

} /* end of class defintion */

class line_details
{
	var $id;
	var $stock_id;
	var $item_description;
	var $units;
	var $mb_flag;
	var $tax_type;
	var $tax_type_name;
	var $src_no;	// number of src doc for this line
	var $src_id;
	var $quantity;
	var $price;
	var $discount_percent;
	var $qty_done;	// quantity processed on child documents
	var $qty_dispatched; // quantity selected to process
	var $qty_old=0;	// quantity dispatched before edition
	var $standard_cost;

	function line_details ($stock_id, $qty, $prc, $disc_percent,
		$qty_done, $standard_cost, $description, $id=0, $src_no=0 )
	{
	/* Constructor function to add a new LineDetail object with passed params */

		$this->id = $id;
		$this->src_no = $src_no;
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
		$this->qty_dispatched = $qty;
		$this->price = $prc;
		$this->discount_percent = $disc_percent;
		$this->qty_done = $qty_done;
		$this->standard_cost = $standard_cost;
	}

	// get unit price as stated on document
	function line_price()
	{
		return $this->price;
	}
}

?>
