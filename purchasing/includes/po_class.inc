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
/*
	This class serves as cart for PO or GRN.
*/

class purch_order 
{
	// db interface
	var $trans_type; // order/grn/invoice (direct)
	var $supplier_id;
	var $Comments;
	var $tran_date;
	var $reference;
	var $supp_ref;
	var $Location;
	var $delivery_address;

	var $prep_amount = 0; // prepayment required
	var $alloc; // sum of payments allocated
	var $tax_included; // type of prices

	var $due_date;		// for grn this is delivery date
	var $order_no;		// for order modification, grn batch
	var $ex_rate;		// for grn

	var $line_items;
	//----

	var $curr_code;
	var $supplier_name;
	var $credit;
	var $tax_group_id;
	var $terms;
	var $cash_account;
	var $dimension,
		$dimension2;

	var $tax_overrides = array();		// array of taxes manually inserted during sales invoice entry (direct invoice)

	var $prepayments = array();

	var $fixed_asset = false;
	var $grn_id;	// grn batch id used in edition only

	function __construct()
	{
		$this->line_items = array();
		$this->order_no = $this->supplier_id = 0;
		$this->tax_group_id = find_domestic_tax_group(); // prevent tax errors until supplier is selected
	}
	
	function set_supplier($supplier_id, $supplier_name, $curr_code, $tax_group_id, $tax_included, $tax_area)
	{
		$this->supplier_id = $supplier_id;
		$this->supplier_name = $supplier_name;
		$this->curr_code = $curr_code;
		$this->tax_group_id = $tax_group_id;
		$this->tax_included = $tax_included;
		$this->tax_area = $tax_area;
	}
	
	function add_to_order($stock_id, $qty, $item_descr, $price, $req_del_date, $qty_inv, $qty_recd, $qty_ordered=0, $grn_item_id=0, $po_item_id=0, $unit_cost=0)
	{
		$this->line_items[] = new po_line_details($stock_id, $item_descr, $qty, $price,
			$req_del_date, $qty_inv, $qty_recd, $qty_ordered, $grn_item_id, $po_item_id, $unit_cost);
	}

	function update_order_item($line_no, $qty, $price, $req_del_date, $description="")
	{
		if ($description != "")
			$this->line_items[$line_no]->item_description = $description;
		$this->line_items[$line_no]->quantity = $qty;
		$this->line_items[$line_no]->price = $price;
		$this->line_items[$line_no]->req_del_date = $req_del_date;
		$this->line_items[$line_no]->item_description = $description;
	}

	function remove_from_order($line_no)
	{
		array_splice($this->line_items, $line_no, 1);
	}
	
	function order_has_items() 
	{
		return count($this->line_items) != 0;
	}
	
	function clear_items() 
	{
    	unset($this->line_items);
		$this->line_items = array();
		$this->order_no = 0;
	}

	
	function any_already_received()
	{
		/* Checks if there have been deliveries or invoiced entered against any of the line items */
		if (count($this->line_items) > 0)
		{
		   	foreach ($this->line_items as $ordered_items) 
		   	{
				if ($ordered_items->qty_received != 0 || $ordered_items->qty_inv != 0)
				{
					return 1;
				}
		   	}
		}
		return 0;
	}

	function some_already_received($line_no)
	{
		/* Checks if there have been deliveries or amounts invoiced against a specific line item */
		if (count($this->line_items) > 0)
		{
		   	if ($this->line_items[$line_no]->qty_received != 0 || 
		   		$this->line_items[$line_no]->qty_inv != 0)
		 	{
				return 1;
		   	}
		}
		return 0;
	}
	
	//
	//	Returns taxes for PO/GRN.
	//	$receival=true in purchase receive context.
	//
	function get_taxes($shipping_cost=null, $receival=false)
	{
		$items = array();
		$prices = array();
		if($shipping_cost==null)
			$shipping_cost = 0;//$this->freight_cost;

		foreach ($this->line_items as $ln_itm) {
			$items[] = $ln_itm->stock_id;
			$prices[] = round($ln_itm->price * $ln_itm->quantity,  user_price_dec());
		}
		$taxes = get_tax_for_items($this->trans_type, $items, $prices, $shipping_cost,
		  $this->tax_group_id, $this->tax_included);

    	// Adjustment for swiss franken, we always have 5 rappen = 1/20 franken
	    if ($this->curr_code == 'CHF') {
			$val = $taxes['1']['Value'];
			$val1 = (floatval((intval(round(($val*20),0)))/20));
			$taxes['1']['Value'] = $val1;
		}
		foreach($this->tax_overrides as $id => $value) // add values entered manually
		{
			$taxes[$id]['Override'] = $value;
		}
		return $taxes;
	}

	/*
		Returns order value including all taxes
	*/
	function get_trans_total() {
		
		$total = 0;
		$dec = user_price_dec();

		foreach ($this->line_items as $ln_itm) {
			$items[] = $ln_itm->stock_id;
			$value = round($ln_itm->quantity * $ln_itm->price, $dec);
			$prices[] =$value;
			$total += $value;
		}

		if (!$this->tax_included ) {
			$taxes = get_tax_for_items($this->trans_type, $items, $prices, 0, $this->tax_group_id,
			$this->tax_included);

			foreach($taxes as $tax)
				$total += round($tax['Value'], $dec);
		}
		return $total;
	}

	function split_line_values()
	{
		// split nominal line values
		foreach($this->line_items as $line)
			$line->split_item_value($this);

		// Exact tax values are currently entered as tax totals, so we need to move the differences back on line level.
		// currently first item with given tax type will be fixed with the calculated difference
		// FIXME: change UI moving tax edit to line level in line edit mode, then this workaround will be obsolete.
		foreach($this->get_taxes() as $tax_id => $tax)
		{
			if ($tax['Value'] != 0 && ($tax['Value'] != $tax['Override']))
			{
				foreach($this->line_items as $id => $line)
					if ($line->gl_amounts[0]['tax_type_id'] == $tax_id) // assumed single tax rate on item, so always gl_mount[0] is valid
					{
						$diff = $tax['Override'] - $tax['Value'];
						$this->line_items[$id]->gl_amounts[0]['Value'] += $diff;
						if ($line->vat_category != VC_NONDEDUCT)
							$this->line_items[$id]->gl_amounts[0]['Deductible'] += $diff;
						else
							$this->line_items[$id]->gl_amounts['Cost'] += $diff;
						 // when supplier uses net prices the price is exact, so don't fix net, still record exact VAT.
						if ($this->tax_included) 
						{
							$this->line_items[$id]->gl_amounts['Net'] -= $diff;
							$this->line_items[$id]->gl_amounts['Cost'] -= $diff;
						}
						break;
					}
			}
		}
	}

} /* end of class defintion */

class po_line_details 
{
	//db interface
	var $po_item_id;
	var $grn_item_id;
	var $stock_id;
	var $item_description;
	var $price;
	var $units;
	var $req_del_date;
	var $qty_inv;	// quantity already invoiced against this line (all PIs)
	var $qty_received;	// quantity already received against this line (all GRNs)
	var $qty_ordered;	// quantity on order (not used on PO entry)
	var $unit_cost;
	var $quantity;		// this document line quantity
	//---

	var $descr_editable;
	var $vat_category;
	var $gl_amounts; 	// splited line value (after call to split_line_value method)

	function __construct($stock_item, $item_descr, $qty, $prc, $req_del_date, 
		$qty_inv, $qty_recd, $qty_ordered=0, $grn_item_id=0, $po_item_id=0, $unit_cost=0)
	{
		$this->stock_id = $stock_item;
		$item_row = get_item($stock_item);
		if (!$item_row) 
			return;

		$this->descr_editable = $item_row["editable"];
		if ($item_descr == null || !$this->descr_editable)
			$this->item_description = $item_row["description"];
		else
			$this->item_description = $item_descr;
		$this->quantity = $qty;
		$this->req_del_date = $req_del_date;
		$this->price = $prc;
		$this->units = $item_row["units"];
		$this->qty_received = $qty_recd;
		$this->qty_inv = $qty_inv;
		$this->unit_cost = $unit_cost;
		$this->grn_item_id = $grn_item_id;
		$this->vat_category = $item_row["vat_category"];
		$this->qty_ordered = $qty_ordered;
		$this->po_item_id = $po_item_id;
	}
	
	//
	// GRN line tax free value.
	//
	function taxfree_charge_value($po)
	{
		return get_tax_free_price_for_item($po->trans_type, $this->stock_id, $this->quantity*$this->price,
		  $po->tax_group_id, $po->tax_included);
	}

	/*
		Splits item value to parts posted to GL.
	*/
	function split_item_value($cart)
	{
        $vat_factor = 1;

		return $this->gl_amounts = split_item_price($this->stock_id, $this->price*$this->quantity, $cart->tax_group_id, $cart->tax_included, 
			ST_SUPPINVOICE, $vat_factor);
	}
  
}

