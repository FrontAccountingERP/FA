<?php

function get_qoh_on_date($stock_id, $location=null, $date_=null)
{
	if ($date_ == null)
		$date_ = Today();
	
	$date = date2sql($date_);
	
	$sql = "SELECT SUM(qty) FROM ".TB_PREF."stock_moves 
		WHERE stock_id='$stock_id'
		AND tran_date <= '$date'";
	
	if ($location != null)
		$sql .= " AND loc_code = '$location'";
		 
	$result = db_query($sql, "QOH calulcation failed");	
	
	$myrow = db_fetch_row($result);
	
	return $myrow[0];	
}

//--------------------------------------------------------------------------------------

function get_item_edit_info($stock_id)
{
	$sql = "SELECT material_cost + labour_cost + overhead_cost AS standard_cost, units 
		FROM ".TB_PREF."stock_master WHERE stock_id='$stock_id'";
	$result = db_query($sql, "The standard cost cannot be retrieved");
	
	return db_fetch($result);
}

//--------------------------------------------------------------------------------------

function get_standard_cost($stock_id)
{
	$sql = "SELECT material_cost + labour_cost + overhead_cost AS std_cost 
		FROM ".TB_PREF."stock_master WHERE stock_id='$stock_id'";
	$result = db_query($sql, "The standard cost cannot be retrieved");
	
	$myrow = db_fetch_row($result);
	
	return $myrow[0];
}

//--------------------------------------------------------------------------------------

function is_inventory_item($stock_id)
{
	$sql = "SELECT stock_id FROM ".TB_PREF."stock_master 
		WHERE stock_id='$stock_id' AND mb_flag <> 'D'";
	$result = db_query($sql, "Cannot query is inventory item or not");
	
	return db_num_rows($result) > 0;	
}

//-------------------------------------------------------------------

Function get_stock_gl_code($stock_id)
{
	/*Gets the GL Codes relevant to the item account  */

	$sql = "SELECT inventory_account, cogs_account, 
		adjustment_account, sales_account, assembly_account, dimension_id, dimension2_id FROM 
		".TB_PREF."stock_master WHERE stock_id = '$stock_id'";

	$get = db_query($sql,"retreive stock gl code");
	return db_fetch($get);
}

//--------------------------------------------------------------------------------------

// $date_ - display / non-sql date
// $std_cost - in HOME currency
// $show_or_hide - wil this move be visible in reports, etc
// $price - in $person_id's currency

function add_stock_move($type, $stock_id, $trans_no, $location,
    $date_, $reference, $quantity, $std_cost, $person_id=null, $show_or_hide=1, 
    $price=0, $discount_percent=0, $error_msg="")
{
	// do not add a stock move if it's a non-inventory item
	if (!is_inventory_item($stock_id))
		return null;
	
	$date = date2sql($date_);
	
	$sql = "INSERT INTO ".TB_PREF."stock_moves (stock_id, trans_no, type, loc_code, 
		tran_date, person_id, reference, qty, standard_cost, visible, price, 
		discount_percent) VALUES ('$stock_id', $trans_no, $type,  
		'$location', '$date', '$person_id', '$reference', $quantity, $std_cost, 
		$show_or_hide, $price, $discount_percent)";
				
	if ($error_msg == "")
		$error_msg = "The stock movement record cannot be inserted";
	
	db_query($sql, $error_msg);
	
	return db_insert_id();		
}					   	

//--------------------------------------------------------------------------------------------------

function get_stock_moves($type, $type_no, $visible=false)
{
	$sql = "SELECT ".TB_PREF."stock_moves.*, ".TB_PREF."stock_master.description, ".TB_PREF."stock_master.units, 
		".TB_PREF."locations.location_name,
		".TB_PREF."stock_master.material_cost + ".TB_PREF."stock_master.labour_cost + ".TB_PREF."stock_master.overhead_cost AS FixedStandardCost
		FROM ".TB_PREF."stock_moves,".TB_PREF."locations,".TB_PREF."stock_master
		WHERE ".TB_PREF."stock_moves.stock_id = ".TB_PREF."stock_master.stock_id 
		AND ".TB_PREF."locations.loc_code=".TB_PREF."stock_moves.loc_code 
		AND type=$type AND trans_no=$type_no ORDER BY trans_id";
	if ($visible)
		$sql .= " AND ".TB_PREF."stock_moves.visible=1";
	
	return db_query($sql, "Could not get stock moves");
}

//--------------------------------------------------------------------------------------------------

function void_stock_move($type, $type_no)
{
	$sql = "UPDATE ".TB_PREF."stock_moves SET qty=0, price=0, discount_percent=0,
		standard_cost=0	WHERE type=$type AND trans_no=$type_no";
	
	db_query($sql, "Could not void stock moves");
}

//--------------------------------------------------------------------------------------------------

function get_location_name($loc_code)
{
	$sql = "SELECT location_name FROM ".TB_PREF."locations WHERE loc_code='$loc_code'";
	
	$result = db_query($sql, "could not retreive the location name for $loc_code");
	
	if (db_num_rows($result) == 1)
	{
		$row = db_fetch_row($result);
		return $row[0];
	}
		
	display_db_error("could not retreive the location name for $loc_code", $sql, true);
}

//--------------------------------------------------------------------------------------------------


?>