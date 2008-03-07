<?php

//----------------------------------------------------------------------------------------

function get_customer_trans_details($debtor_trans_type, $debtor_trans_no)
{
if (!is_array($debtor_trans_no))
	$debtor_trans_no = array( 0=>$debtor_trans_no );

	$sql = "SELECT ".TB_PREF."debtor_trans_details.*,
		".TB_PREF."debtor_trans_details.unit_price+".TB_PREF."debtor_trans_details.unit_tax AS FullUnitPrice,
		".TB_PREF."debtor_trans_details.description As StockDescription,
		".TB_PREF."stock_master.units
		FROM ".TB_PREF."debtor_trans_details,".TB_PREF."stock_master
		WHERE (";

	$tr=array();
	foreach ($debtor_trans_no as $trans_no)
		$tr[] = 'debtor_trans_no='.$trans_no;

	$sql .= implode(' OR ', $tr);


	$sql.=	") AND debtor_trans_type=$debtor_trans_type
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
		AND amount != 0
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

function write_customer_trans_detail_item($debtor_trans_type, $debtor_trans_no, $stock_id, $description,
	$quantity, $unit_price, $unit_tax, $discount_percent, $std_cost, $line_id=0)
{
	if ($line_id!=0)
		$sql = "UPDATE ".TB_PREF."debtor_trans_details SET
			stock_id='$stock_id',
			description='$description',
			quantity=$quantity,
			unit_price=$unit_price,
			unit_tax=$unit_tax,
			discount_percent=$discount_percent,
			standard_cost=$std_cost WHERE
			id=$line_id";
	else
			$sql = "INSERT INTO ".TB_PREF."debtor_trans_details (debtor_trans_no,
				debtor_trans_type, stock_id, description, quantity, unit_price,
				unit_tax, discount_percent, standard_cost)
			VALUES ($debtor_trans_no, $debtor_trans_type, '$stock_id', '$description',
				$quantity, $unit_price, $unit_tax, $discount_percent, $std_cost)";

	db_query($sql, "The debtor transaction detail could not be written");
}

?>