<?php

//----------------------------------------------------------------------------------------

function add_customer_trans_detail_item_base($debtor_trans_type, $debtor_trans_no, $stock_id, $description, $quantity,
	$unit_price, $unit_tax, $discount_percent, $standard_cost)
{
	$sql = "INSERT INTO ".TB_PREF."debtor_trans_details (debtor_trans_no, debtor_trans_type, stock_id, description, quantity, unit_price, unit_tax, discount_percent, standard_cost)
		VALUES ($debtor_trans_no, $debtor_trans_type, '$stock_id', '$description', $quantity, $unit_price, $unit_tax, $discount_percent, $standard_cost)";

	db_query($sql, "The debtor transaction detail could not be added");
}

//----------------------------------------------------------------------------------------

function get_customer_trans_details($debtor_trans_type, $debtor_trans_no)
{
	$sql = "SELECT ".TB_PREF."debtor_trans_details.*, ".TB_PREF."debtor_trans_details.unit_price+".TB_PREF."debtor_trans_details.unit_tax AS FullUnitPrice,
		".TB_PREF."debtor_trans_details.description As StockDescription,
		".TB_PREF."stock_master.units
		FROM ".TB_PREF."debtor_trans_details,".TB_PREF."stock_master
		WHERE debtor_trans_no=$debtor_trans_no
		AND debtor_trans_type=$debtor_trans_type 
		AND ".TB_PREF."stock_master.stock_id=".TB_PREF."debtor_trans_details.stock_id
		ORDER BY id";

	return db_query($sql, "The debtor transaction detail could not be queried");
}

//----------------------------------------------------------------------------------------

function void_customer_trans_details($type, $type_no)
{
	$sql = "UPDATE ".TB_PREF."debtor_trans_details SET quantity=0, unit_price=0,
		unit_tax=0, discount_percent=0, standard_cost=0
		WHERE debtor_trans_no=$type_no
		AND debtor_trans_type=$type";

	db_query($sql, "The debtor transaction details could not be voided");

	// clear the stock move items
	void_stock_move($type, $type_no);
}

//----------------------------------------------------------------------------------------

function add_customer_trans_detail_item($debtor_trans_type, $debtor_trans_no, $stock_id, $description,
	$Location, $date_, $quantity, $unit_price, $unit_tax, $discount_percent,
	$reference, $std_cost)
{
	add_customer_trans_detail_item_base($debtor_trans_type, $debtor_trans_no, $stock_id, $description,
		$quantity, $unit_price, $unit_tax, $discount_percent, $std_cost);

    if($debtor_trans_type==13)
	add_stock_move_customer($debtor_trans_type, $stock_id, $debtor_trans_no, $Location,
		$date_, $reference, $quantity, $std_cost, 1, $unit_price+$unit_tax, $discount_percent);

	return $std_cost;
}

//----------------------------------------------------------------------------------------

function add_customer_trans_detail_item_writeoff($debtor_trans_type, $debtor_trans_no, $stock_id, $description,
	$Location, $date_, $quantity, $unit_price, $unit_tax, $discount_percent,
	$reference, $std_cost)
{
	$retCost = add_customer_trans_detail_item($debtor_trans_type, $debtor_trans_no, $stock_id, $description,
		$Location, $date_, $quantity, $unit_price, $unit_tax, $discount_percent,
		$reference,	$std_cost);

	$reference = _("Write off") . " " . $reference;

	add_stock_move_customer($debtor_trans_type, $stock_id, $debtor_trans_no, $Location,
		$date_, $reference, -$quantity, $std_cost, 0, $unit_price+$unit_tax, $discount_percent);

	return $retCost;
}

//----------------------------------------------------------------------------------------

function add_customer_trans_tax_detail_item($debtor_trans_type, $debtor_trans_no,
	$tax_type_id, $rate, $included_in_price, $amount)
{
	$sql = "INSERT INTO ".TB_PREF."debtor_trans_tax_details (debtor_trans_no, debtor_trans_type, tax_type_id, rate, included_in_price, amount)
		VALUES ($debtor_trans_no, $debtor_trans_type, $tax_type_id, $rate, $included_in_price, $amount)";

	db_query($sql, "The debtor transaction tax detail could not be added");
}

//----------------------------------------------------------------------------------------

function get_customer_trans_tax_details($debtor_trans_type, $debtor_trans_no)
{
	$sql = "SELECT ".TB_PREF."debtor_trans_tax_details.*, ".TB_PREF."tax_types.name AS tax_type_name
		FROM ".TB_PREF."debtor_trans_tax_details,".TB_PREF."tax_types
		WHERE debtor_trans_no=$debtor_trans_no
		AND debtor_trans_type=$debtor_trans_type
		AND ".TB_PREF."tax_types.id = ".TB_PREF."debtor_trans_tax_details.tax_type_id";

	return db_query($sql, "The debtor transaction tax details could not be queried");
}

//----------------------------------------------------------------------------------------

function void_customer_trans_tax_details($type, $type_no)
{
	$sql = "UPDATE ".TB_PREF."debtor_trans_tax_details SET amount=0
		WHERE debtor_trans_no=$type_no
		AND debtor_trans_type=$type";

	db_query($sql, "The debtor transaction tax details could not be voided");
}

//----------------------------------------------------------------------------------------

?>