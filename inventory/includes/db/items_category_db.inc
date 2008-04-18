<?php

function add_item_category($description)

{
	$sql = "INSERT INTO ".TB_PREF."stock_category (description)
		VALUES (".db_quote($description).")";

	db_query($sql,"an item category could not be added");
}

function update_item_category($ItemCategory, $description)

{
	$sql = "UPDATE ".TB_PREF."stock_category SET description = ".db_quote($description)."
        WHERE category_id = '$ItemCategory'";

	db_query($sql,"an item category could not be updated");
}

function delete_item_category($ItemCategory)
{
	$sql="DELETE FROM ".TB_PREF."stock_category WHERE category_id='$ItemCategory'";

	db_query($sql,"an item category could not be deleted");
}

function get_item_category($ItemCategory)
{
	$sql="SELECT * FROM ".TB_PREF."stock_category WHERE category_id='$ItemCategory'";

	$result = db_query($sql,"an item category could not be retrieved");

	return db_fetch($result);
}

function get_category_name($id)
{
	$sql = "SELECT description FROM ".TB_PREF."stock_category WHERE category_id=$id";

	$result = db_query($sql, "could not get sales type");

	$row = db_fetch_row($result);
	return $row[0];
}


?>