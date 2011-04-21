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
include_once($path_to_root . "/includes/prefs/sysprefs.inc");
include_once($path_to_root . "/inventory/includes/inventory_db.inc");

class items_cart
{
	var $trans_type;
	var $line_items;
	var $gl_items;

	var	$order_id;

	var $from_loc;
	var $to_loc;
	var $tran_date;
	var $transfer_type;
	var $increase;
	var $memo_;
	var $person_id;
	var $branch_id;
	var $reference;
	var $original_amount;
	
	function items_cart($type)
	{
		$this->trans_type = $type;
		$this->clear_items();
	}

	// --------------- line item functions

	function add_to_cart($line_no, $stock_id, $qty, $standard_cost, $description=null)
	{

		if (isset($stock_id) && $stock_id != "" && isset($qty))
		{
			$this->line_items[$line_no] = new line_item($stock_id, $qty,
				$standard_cost, $description);
			return true;
		}
		else
		{
			// shouldn't come here under normal circumstances
			display_error("unexpected - adding an invalid item or null quantity", "", true);
		}

		return false;
	}

	function find_cart_item($stock_id)
	{
		foreach($this->line_items as $line_no=>$line) {
			if ($line->stock_id == $stock_id)
				return $this->line_items[$line_no];
		}
		return null;
	}

	function update_cart_item($line_no, $qty, $standard_cost)
	{
		$this->line_items[$line_no]->quantity = $qty;
		$this->line_items[$line_no]->standard_cost = $standard_cost;
	}

	function remove_from_cart($line_no)
	{
		array_splice($this->line_items, $line_no, 1);
	}

	function count_items()
	{
		return count($this->line_items);
	}

	function check_qoh($location, $date_, $reverse=false)
	{
		foreach ($this->line_items as $line_no => $line_item)
		{
			$item_ret = $line_item->check_qoh($location, $date_, $reverse);
			if ($item_ret != null)
				return $line_no;
		}
		return -1;
	}

	// ----------- GL item functions

	function add_gl_item($code_id, $dimension_id, $dimension2_id, $amount, $reference, $description=null)
	{
		if (isset($code_id) && $code_id != "" && isset($amount) && isset($dimension_id)  &&
			isset($dimension2_id))
		{
			$this->gl_items[] = new gl_item($code_id, $dimension_id, $dimension2_id, $amount, $reference, $description);
			return true;
		}
		else
		{
			// shouldn't come here under normal circumstances
			display_error("unexpected - invalid parameters in add_gl_item($code_id, $dimension_id, $dimension2_id, $amount,...)", "", true);
		}

		return false;
	}

	function update_gl_item($index, $code_id, $dimension_id, $dimension2_id, $amount, $reference, $description=null)
	{
	    $this->gl_items[$index]->code_id = $code_id;
		$this->gl_items[$index]->dimension_id = $dimension_id;
		$this->gl_items[$index]->dimension2_id = $dimension2_id;
		$this->gl_items[$index]->amount = $amount;
		$this->gl_items[$index]->reference = $reference;
		if ($description == null)
			$this->gl_items[$index]->description = get_gl_account_name($code_id);
		else
			$this->gl_items[$index]->description = $description;

	}

	function remove_gl_item($index)
	{
		array_splice($this->gl_items, $index, 1);
	}

	function count_gl_items()
	{
		return count($this->gl_items);
	}

	function gl_items_total()
	{
		$total = 0;
		foreach ($this->gl_items as $gl_item)
			$total += $gl_item->amount;
		return $total;
	}

	function gl_items_total_debit()
	{
		$total = 0;
		foreach ($this->gl_items as $gl_item)
		{
			if ($gl_item->amount > 0)
				$total += $gl_item->amount;
		}
		return $total;
	}

	function gl_items_total_credit()
	{
		$total = 0;
		foreach ($this->gl_items as $gl_item)
		{
			if ($gl_item->amount < 0)
				$total += $gl_item->amount;
		}
		return $total;
	}

	// ------------ common functions

	function clear_items()
	{
    	unset($this->line_items);
		$this->line_items = array();

    	unset($this->gl_items);
		$this->gl_items = array();

	}
}

//--------------------------------------------------------------------------------------------

class line_item
{
	var $stock_id;
	var $item_description;
	var $units;
	var $mb_flag;

	var $quantity;
	var $price;
	var $standard_cost;

	function line_item ($stock_id, $qty, $standard_cost=null, $description=null)
	{
		$item_row = get_item($stock_id);

		if ($item_row == null)
			display_error("invalid item added to order : $stock_id", "");

		$this->mb_flag = $item_row["mb_flag"];
		$this->units = $item_row["units"];

		if ($description == null)
			$this->item_description = $item_row["description"];
		else
			$this->item_description = $description;

		if ($standard_cost == null)
			$this->standard_cost = $item_row["actual_cost"];
		else
			$this->standard_cost = $standard_cost;

		$this->stock_id = $stock_id;
		$this->quantity = $qty;
		//$this->price = $price;
		$this->price = 0;
	}

	function check_qoh($location, $date_, $reverse)
	{
		global $SysPrefs;
		
    	if (!$SysPrefs->allow_negative_stock())
    	{
			if (has_stock_holding($this->mb_flag))
			{
				$quantity = $this->quantity;
				if ($reverse)
					$quantity = -$this->quantity;

				if ($quantity >= 0)
					return null;

				$qoh = get_qoh_on_date($this->stock_id, $location, $date_);
        		if ($quantity + $qoh < 0)
        		{
        			return $this;
        		}
    		}
    	}

    	return null;
	}
}

//---------------------------------------------------------------------------------------

class gl_item
{

	var $code_id;
	var $dimension_id;
	var $dimension2_id;
	var $amount;
	var $reference;
	var $description;

	function gl_item($code_id, $dimension_id, $dimension2_id, $amount, $reference,
		$description=null)
	{
		//echo "adding $index, $code_id, $dimension_id, $amount, $reference<br>";

		if ($description == null)
			$this->description = get_gl_account_name($code_id);
		else
			$this->description = $description;

		$this->code_id = $code_id;
		$this->dimension_id = $dimension_id;
		$this->dimension2_id = $dimension2_id;
		$this->amount = $amount;
		$this->reference = $reference;
	}
}

//---------------------------------------------------------------------------------------

?>
