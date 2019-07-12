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
/* Definition of the Supplier Transactions class to hold all the information for an accounts payable invoice or credit note
*/

include_once($path_to_root . "/taxes/tax_calc.inc");

class supp_trans 
{

	var $grn_items; /*array of objects of class grn_item using the id as the pointer */
	var $gl_codes; /*array of objects of class gl_codes using a counter as the pointer */
	var $supplier_id;
	var $supplier_name;
	var $terms;
	var $ex_rate;
	
	var $tax_description;
	var $tax_group_id;
	var $tax_included;
	
	var $trans_type;	// invoice or credit
	var $trans_no;

	var $Comments;
	var $tran_date;
	var $due_date;
	var $src_docs = array();		// source invoice for this credit note (if any)

	var $supp_reference;
	var $reference;
	var $ov_amount;
	var $ov_discount;
	var $ov_gst;
	var $credit = 0;
	var $currency;
	var $tax_overrides = array();		// array of taxes manually inserted during sales invoice entry

	var $dimension,
		$dimension2;

	function __construct($trans_type, $trans_no=0)
	{
		$this->trans_type = $trans_type;
		/*Constructor function initialises a new Supplier Transaction object */
		$this->read($trans_type, $trans_no);
	}

	function read($trans_type, $trans_no)
	{
		$this->trans_type = $trans_type;
		$this->trans_no = $trans_no;
 		$this->grn_items = array();
 		$this->gl_codes = array();
		if ($trans_no) {
			read_supp_invoice($trans_no, $trans_type, $this);
			if ($trans_type == ST_SUPPCREDIT)
			{
				$this->src_docs = find_src_invoices($trans_no);
			}
			read_supplier_details_to_trans($this, $this->supplier_id);
		}
	}

	function add_grn_to_trans($grn_item_id, $po_detail_item, $item_code, $item_description, 
		$qty_recd, $prev_quantity_inv, $this_quantity_inv, $order_price, $chg_price, 
		$std_cost_unit=null, $gl_code='')
	{
		$this->grn_items[$grn_item_id] = new grn_item($grn_item_id, $po_detail_item, 
			$item_code, $item_description, $qty_recd, $prev_quantity_inv, $this_quantity_inv, 
			$order_price, $chg_price, $std_cost_unit, $gl_code, $this->tax_included);

		$this->src_docs = find_src_invoices($this);

		unset($this->tax_overrides); // cancel tax overrides after cart change
		return 1;
	}

	function add_gl_codes_to_trans($gl_code, $gl_act_name, $gl_dim, $gl_dim2, $amount, $memo_)
	{
		$this->gl_codes[] = new gl_codes($gl_code, $gl_act_name, $gl_dim, $gl_dim2, $amount, $memo_);
		unset($this->tax_overrides); // cancel tax overrides after cart change
		return 1;
	}

	function remove_grn_from_trans($grn_item_id)
	{
		unset($this->tax_overrides); // cancel tax overrides after cart change
	    unset($this->grn_items[$grn_item_id]);
	}
	function remove_gl_codes_from_trans($gl_code_counter)
	{
		unset($this->tax_overrides); // cancel tax overrides after cart change
		unset($this->gl_codes[$gl_code_counter]);
	}

	function is_valid_trans_to_post()
	{
		return (count($this->grn_items) > 0 || count($this->gl_codes) > 0 || 
			($this->ov_amount != 0) || ($this->ov_discount > 0));
	}

	function clear_items()
	{
		unset($this->grn_items);
		unset($this->gl_codes);
		$this->ov_amount = $this->ov_discount = $this->supplier_id = 0;

		$this->grn_items = array();
		$this->gl_codes = array();
	}

    function get_taxes($tax_group_id=null, $shipping_cost=0, $gl_codes=true)
    {
    	$items = array();
    	$prices = array();

        if ($tax_group_id == null)
        	$tax_group_id = $this->tax_group_id;

		// preload the taxgroup
		$tax_group = get_tax_group_items_as_array($tax_group_id);

        foreach ($this->grn_items as $ln_itm) 
        {
        	$items[] = $ln_itm->item_code;
        	$prices[] =round( ($ln_itm->this_quantity_inv * $ln_itm->chg_price),
				user_price_dec());
        }

        if ($tax_group_id == null)
        	$tax_group_id = $this->tax_group_id;
        $taxes = get_tax_for_items($this->trans_type, $items, $prices, $shipping_cost, $tax_group_id, 
        	$this->tax_included);

		if (isset($this->tax_overrides))
			foreach($this->tax_overrides as $id => $value) // add values entered manually
			{
				$taxes[$id]['Override'] = $value;
			}

		// Taxes included in gl_codes table have exact value, but count to overrides as well.
		// Therefore when we want to now taxes only for items (gl_codes==false),
		// we have to subtract gl_taxes from override values.
		foreach ($this->gl_codes as $gl_code)
		{
			$index = is_tax_account($gl_code->gl_code);
			if ($index !== false)
			{
				if ($gl_codes)
					$taxes[$index]['Value'] += $gl_code->amount;
				elseif (isset($this->tax_overrides))
					$taxes[$index]['Override'] -= $gl_code->amount;
			}
		}

        return $taxes;
    }
	//
	//	Returns total invoice amount without taxes.
	//
    function get_total_taxfree($tax_group_id=null)
    {
    	$total = 0;

		// preload the taxgroup !
		if ($tax_group_id != null)
			$tax_group = get_tax_group_items_as_array($tax_group_id);
		else
			$tax_group = null;

		foreach ($this->grn_items as $ln_itm)
    		$total += round(($ln_itm->this_quantity_inv * $ln_itm->taxfree_charge_price($tax_group_id, $tax_group)),
			 user_price_dec());

   		foreach ($this->gl_codes as $gl_line)
   		{
   			if (!is_tax_account($gl_line->gl_code))
   				$total += $gl_line->amount;
   		}
   		return $total;
    }
	//
	//	Returns transaction total 
	//
	function get_items_total()
	{
		$total = 0;

		foreach ($this->grn_items as $ln_itm)
			$total += round($ln_itm->this_quantity_inv * $ln_itm->chg_price, user_price_dec());

		foreach ($this->gl_codes as $gl_line)
		{
   			if (!is_tax_account($gl_line->gl_code) || $this->tax_included)
				$total += $gl_line->amount;
		}
		return $total;
	}

	function split_line_values()
	{
		// split nominal line values
		foreach($this->grn_items as $line)
			$line->split_item_value($this);

		// Exact tax values are currently entered as tax totals, so we need to move the differences back on line level.
		// currently first item with given tax type will be fixed with the calculated difference
		// FIXME: change UI moving tax edit to line level in line edit mode, then this workaround will be obsolete.
		foreach($this->get_taxes() as $tax_id => $tax)
		{
			if ($tax['Value'] != 0 && isset($tax['Override']) && ($tax['Value'] != $tax['Override']))
			{
				foreach($this->grn_items as $id => $line)
					if ($line->gl_amounts[0]['tax_type_id'] == $tax_id) // assumed single tax rate on item, so always gl_mount[0] is valid
					{
						$diff = $tax['Override'] - $tax['Value'];
						$this->grn_items[$id]->gl_amounts[0]['Value'] += $diff;
						if ($this->vat_category() != VC_NONDEDUCT)
							$this->grn_items[$id]->gl_amounts[0]['Deductible'] += $diff;
						else
							$this->grn_items[$id]->gl_amounts['Cost'] += $diff;
						 // when supplier uses net prices the price is exact, so don't fix net, still record exact VAT.
						if ($this->tax_included) {
							$this->grn_items[$id]->gl_amounts['Net'] -= $diff;
							$this->grn_items[$id]->gl_amounts['Cost'] -= $diff;
						}
						break;
					}
			}
		}
	}
} /* end of class defintion */

class grn_item 
{

/* Contains relavent information from the purch_order_details as well to provide in cached form,
all the info to do the necessary entries without looking up ie additional queries of the database again */

	var $id;
	var $po_detail_item;
	var $item_code;
	var $item_description;
	var $qty_recd;
	var $prev_quantity_inv;
	var $this_quantity_inv;
	var $order_price;
	var $chg_price;
	var $std_cost_unit;
	var $gl_code;
	var $tax_included;
	var $gl_amounts; 	// splited line value (after call to split_line_value method
	var $vat_category;

	function __construct($id, $po_detail_item, $item_code, $item_description, $qty_recd, 
		$prev_quantity_inv, $this_quantity_inv, $order_price, $chg_price,
		$std_cost_unit, $gl_code, $tax_included)
	{

		$this->id = $id;
		$this->po_detail_item = $po_detail_item;
		$this->item_code = $item_code;
		$this->item_description = $item_description;
		$this->qty_recd = $qty_recd;
		$this->prev_quantity_inv = $prev_quantity_inv;
		$this->this_quantity_inv = $this_quantity_inv;
		$this->order_price =$order_price;
		$this->chg_price = $chg_price;
		$this->std_cost_unit = $std_cost_unit;
		$this->gl_code = $gl_code;
		$this->tax_included = $tax_included;

		$opts = get_item($item_code);
		$this->vat_category = $opts['vat_category'];
	}

	function full_charge_price($tax_group_id, $trans_type=ST_PURCHORDER)
	{
		return get_full_price_for_item($trans_type, $this->item_code, 
		  $this->chg_price, $tax_group_id, $this->tax_included);
	}

	function taxfree_charge_price($tax_group_id, $trans_type=ST_PURCHORDER)
	{
		return get_tax_free_price_for_item($trans_type, $this->item_code, $this->chg_price, 
		  $tax_group_id, $this->tax_included);
	}

	function taxfree_value($tax_group_id, $trans_type=ST_PURCHORDER)
	{
		return get_tax_free_price_for_item($trans_type, $this->item_code, $this->this_quantity_inv * $this->chg_price, 
		  $tax_group_id, $this->tax_included, $tax_group);
	}

	/*
		Splits item value to parts posted to GL.
	*/
	function split_item_value($cart)
	{

        $vat_factor = 1;

		return $this->gl_amounts = split_item_price($this->item_code, $this->chg_price*$this->this_quantity_inv, $cart->tax_group_id, $cart->tax_included, 
			ST_SUPPINVOICE, $vat_factor, $cart->tran_date);
	}
}


class gl_codes 
{

	var $gl_code;
	var $gl_act_name;
	var $gl_dim;
	var $gl_dim2;
	var $amount;
	var $memo_;

	function __construct($gl_code, $gl_act_name, $gl_dim, $gl_dim2, $amount, $memo_)
	{
		$this->gl_code = $gl_code;
		$this->gl_act_name = $gl_act_name;
		$this->gl_dim = $gl_dim;
		$this->gl_dim2 = $gl_dim2;
		$this->amount = $amount;
		$this->memo_= $memo_;
	}
}

