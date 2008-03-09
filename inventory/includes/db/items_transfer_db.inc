<?php

//-------------------------------------------------------------------------------------------------------------

function add_stock_transfer($Items, $location_from, $location_to, $date_, $type, $reference, $memo_)
{
	begin_transaction();

	$transfer_id = get_next_trans_no(systypes::location_transfer());

	foreach ($Items as $line_item)
	{
		add_stock_transfer_item($transfer_id, $line_item->stock_id, $location_from,
			$location_to, $date_, $type, $reference, $line_item->quantity);
	}

	add_comments(systypes::location_transfer(), $transfer_id, $date_, $memo_);

	references::save_last($reference, systypes::location_transfer());

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
	add_stock_move(systypes::location_transfer(), $stock_id, $transfer_id, $location_from,
       	$date_, $reference, -$quantity, 0, $type);

	add_stock_move(systypes::location_transfer(), $stock_id, $transfer_id, $location_to,
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
	$result = get_stock_moves(systypes::location_transfer(), $trans_no);

	if (db_num_rows($result) == 0)
	{
		return null;
	}

	return $result;
}

//-------------------------------------------------------------------------------------------------------------

function void_stock_transfer($type_no)
{
	void_stock_move(systypes::location_transfer(), $type_no);
}

?>