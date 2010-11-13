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

function add_stock_transfer($Items, $location_from, $location_to, $date_, $type, $reference, $memo_)
{
	global $Refs;

	begin_transaction();
	$args = func_get_args();
	$args = (object)array_combine(array('Items', 'location_from', 'location_to', 
	'date_', 'type', 'reference', 'memo_'), $args);
	$args->trans_no = 0;
	hook_db_prewrite($args, ST_LOCTRANSFER);

	$transfer_id = get_next_trans_no(ST_LOCTRANSFER);

	foreach ($Items as $line_item)
	{
		add_stock_transfer_item($transfer_id, $line_item->stock_id, $location_from,
			$location_to, $date_, $type, $reference, $line_item->quantity);
	}

	add_comments(ST_LOCTRANSFER, $transfer_id, $date_, $memo_);

	$Refs->save(ST_LOCTRANSFER, $transfer_id, $reference);
	add_audit_trail(ST_LOCTRANSFER, $transfer_id, $date_);

	$args->trans_no = $transfer_id;
	hook_db_postwrite($args, ST_LOCTRANSFER);

	commit_transaction();

	return $transfer_id;
}

//-------------------------------------------------------------------------------------------------------------

// add 2 stock_moves entries for a stock transfer
// $date_ is display date (not sql)
// std_cost is in HOME currency
// it seems the standard_cost field is not used at all

function add_stock_transfer_item($transfer_id, $stock_id, $location_from, $location_to,
	$date_, $type, $reference, $quantity)
{
	add_stock_move(ST_LOCTRANSFER, $stock_id, $transfer_id, $location_from,
       	$date_, $reference, -$quantity, 0, $type);

	add_stock_move(ST_LOCTRANSFER, $stock_id, $transfer_id, $location_to,
	   	$date_, $reference, $quantity, 0, $type);

}

//-------------------------------------------------------------------------------------------------------------

function get_stock_transfer($trans_no)
{
	$result = get_stock_transfer_items($trans_no);
	if (db_num_rows($result) < 2)
	{
		display_db_error("transfer with less than 2 items : $trans_no", "");
	}

	// this function is very bad that it assumes that 1st record and 2nd record contain the
	// from and to locations - if get_stock_moves uses a different ordering than trans_no then
	// it will bomb
	$move1 = db_fetch($result);
	$move2 = db_fetch($result);

	// return an array of (From, To)
	if ($move1['qty'] < 0)
		return array($move1, $move2);
	else
		return array($move2, $move1);
}

//-------------------------------------------------------------------------------------------------------------

function get_stock_transfer_items($trans_no)
{
	$result = get_stock_moves(ST_LOCTRANSFER, $trans_no);

	if (db_num_rows($result) == 0)
	{
		return null;
	}

	return $result;
}

//-------------------------------------------------------------------------------------------------------------

function void_stock_transfer($type_no)
{
	hook_db_prevoid(ST_LOCTRANSFER, $type_no);
	void_stock_move(ST_LOCTRANSFER, $type_no);
}

?>