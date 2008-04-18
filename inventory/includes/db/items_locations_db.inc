<?php

function add_item_location($loc_code, $location_name, $delivery_address, $phone, $fax, $email, $contact)
{
	$sql = "INSERT INTO ".TB_PREF."locations (loc_code, location_name, delivery_address, phone, fax, email, contact)
		VALUES (".db_escape($loc_code).", ".db_escape($location_name).", ".db_escape($delivery_address).", ".db_escape($phone).", ".db_escape($fax).", ".db_escape($email).", ".db_escape($contact).")";

	db_query($sql,"a location could not be added");

	/* Also need to add loc_stock records for all existing items */
	$sql = "INSERT INTO ".TB_PREF."loc_stock (loc_code, stock_id, reorder_level)
		SELECT '$loc_code', ".TB_PREF."stock_master.stock_id, 0 FROM ".TB_PREF."stock_master";

	db_query($sql,"a location could not be added");
}

//------------------------------------------------------------------------------------

function update_item_location($loc_code, $location_name, $delivery_address, $phone, $fax, $email, $contact)

{
    $sql = "UPDATE ".TB_PREF."locations SET location_name=".db_escape($location_name).",
    	delivery_address=".db_escape($delivery_address).",
    	phone=".db_escape($phone).", fax=".db_escape($fax).",
    	email=".db_escape($email).", contact=".db_escape($contact)."
    	WHERE loc_code = '$loc_code'";

	db_query($sql,"a location could not be updated");
}

//------------------------------------------------------------------------------------

function delete_item_location($item_location)
{
	$sql="DELETE FROM ".TB_PREF."locations WHERE loc_code='$item_location'";
	db_query($sql,"a location could not be deleted");

	$sql = "DELETE FROM ".TB_PREF."loc_stock WHERE loc_code ='$item_location'";
	db_query($sql,"a location could not be deleted");
}

//------------------------------------------------------------------------------------

function get_item_location($item_location)
{
	$sql="SELECT * FROM ".TB_PREF."locations WHERE loc_code='$item_location'";

	$result = db_query($sql,"a location could not be retrieved");

	return db_fetch($result);
}

//------------------------------------------------------------------------------------

function set_reorder_level($stock_id, $loc_code, $reorder_level)
{
	$sql = "UPDATE ".TB_PREF."loc_stock SET reorder_level = $reorder_level
		WHERE stock_id = '$stock_id' AND loc_code = '$loc_code'";

	db_query($sql,"an item reorder could not be set");
}

//------------------------------------------------------------------------------------

function get_loc_details($stock_id)
{
	$sql = "SELECT ".TB_PREF."loc_stock.*, ".TB_PREF."locations.location_name
		FROM ".TB_PREF."loc_stock, ".TB_PREF."locations
		WHERE ".TB_PREF."loc_stock.loc_code=".TB_PREF."locations.loc_code
		AND ".TB_PREF."loc_stock.stock_id = '" . $stock_id . "' ORDER BY ".TB_PREF."loc_stock.loc_code";
	return db_query($sql,"an item reorder could not be retreived");
}

//------------------------------------------------------------------------------------

?>