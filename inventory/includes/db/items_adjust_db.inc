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
//-------------------------------------------------------------------------------------------------------------

function add_stock_adjustment($items, $location, $date_, $type, $increase, $reference, $memo_)
{
	begin_transaction();

	$adj_id = get_next_trans_no(systypes::inventory_adjustment());

	foreach ($items as $line_item)
	{

		if (!$increase)
			$line_item->quantity = -$line_item->quantity;

		add_stock_adjustment_item($adj_id, $line_item->stock_id, $location, $date_, $type, $reference,
			$line_item->quantity, $line_item->standard_cost, $memo_);
	}

	add_comments(systypes::inventory_adjustment(), $adj_id, $date_, $memo_);

	references::save_last($reference, systypes::inventory_adjustment());

	commit_transaction();

	return $adj_id;
}

//-------------------------------------------------------------------------------------------------------------

function void_stock_adjustment($type_no)
{
	void_gl_trans(systypes::inventory_adjustment(), $type_no);
	void_stock_move(systypes::inventory_adjustment(), $type_no);
}

//-------------------------------------------------------------------------------------------------------------

function get_stock_adjustment_items($trans_no)
{
	$result = get_stock_moves(systypes::inventory_adjustment(), $trans_no);

	if (db_num_rows($result) == 0)
	{
		return null;
	}

	return $result;
}

//--------------------------------------------------------------------------------------------------

function add_stock_adjustment_item($adj_id, $stock_id, $location, $date_, $type, $reference,
	$quantity, $standard_cost, $memo_)
{
	$mb_flag = get_mb_flag($stock_id);

    if (is_service($mb_flag))
    {
    	display_db_error("Cannot do inventory adjustment for Service item : $stock_id", "");
    }

	update_average_material_cost(null, $stock_id, $standard_cost, $quantity, $date_);

	add_stock_move(systypes::inventory_adjustment(), $stock_id, $adj_id, $location,
        $date_, $reference, $quantity, $standard_cost, $type);

	if ($standard_cost > 0)
	{

		$stock_gl_codes = get_stock_gl_code($stock_id);

		add_gl_trans_std_cost(systypes::inventory_adjustment(), $adj_id, $date_,
			$stock_gl_codes['adjustment_account'], $stock_gl_codes['dimension_id'], $stock_gl_codes['dimension2_id'], $memo_, ($standard_cost * -($quantity)));

		add_gl_trans_std_cost(systypes::inventory_adjustment(), $adj_id, $date_, $stock_gl_codes['inventory_account'], 0, 0, $memo_, ($standard_cost * $quantity));
	}
}

//-------------------------------------------------------------------------------------------------------------

?>