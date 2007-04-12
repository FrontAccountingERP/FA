<?php

function update_item($stock_id, $description, $long_description, $category_id, $tax_type_id,
	$sales_account, $inventory_account, $cogs_account, $adjustment_account,
	$assembly_account, $dimension_id, $dimension2_id)
{
	$sql = "UPDATE ".TB_PREF."stock_master SET long_description='$long_description',
		description='$description',
		category_id='$category_id',
		sales_account=$sales_account,
		inventory_account=$inventory_account,
		cogs_account=$cogs_account,
		adjustment_account=$adjustment_account,
		assembly_account=$assembly_account,
		dimension_id=$dimension_id,
		dimension2_id=$dimension2_id,
		tax_type_id=$tax_type_id
		WHERE stock_id='$stock_id'";

	db_query($sql, "The item could not be updated");
}

function add_item($stock_id, $description, $long_description, $category_id, $tax_type_id, $units, $mb_flag,
	$sales_account, $inventory_account, $cogs_account, $adjustment_account,
	$assembly_account, $dimension_id, $dimension2_id)
{
	$sql = "INSERT INTO ".TB_PREF."stock_master (stock_id, description, long_description, category_id,
		tax_type_id, units, mb_flag, sales_account, inventory_account, cogs_account,
		adjustment_account, assembly_account, dimension_id, dimension2_id)
		VALUES ('$stock_id', '$description', '$long_description',
		'$category_id', $tax_type_id, '$units', '$mb_flag',
		$sales_account, $inventory_account, $cogs_account,
		$adjustment_account, $assembly_account, $dimension_id, $dimension2_id)";

	db_query($sql, "The item could not be added");

	$sql = "INSERT INTO ".TB_PREF."loc_stock (loc_code, stock_id)
		SELECT ".TB_PREF."locations.loc_code, '$stock_id' FROM ".TB_PREF."locations";

	db_query($sql, "The item locstock could not be added");
}

function delete_item($stock_id)
{
	$sql="DELETE FROM ".TB_PREF."stock_master WHERE stock_id='$stock_id'";
	db_query($sql, "could not delete stock item");

	/*and cascade deletes in loc_stock */
	$sql ="DELETE FROM ".TB_PREF."loc_stock WHERE stock_id='$stock_id'";
	db_query($sql, "could not delete stock item loc stock");

	/*and cascade deletes in purch_data */
	$sql ="DELETE FROM ".TB_PREF."purch_data WHERE stock_id='$stock_id'";
	db_query($sql, "could not delete stock item purch data");

	/*and cascade deletes in prices */
	$sql ="DELETE FROM ".TB_PREF."prices WHERE stock_id='$stock_id'";
	db_query($sql, "could not delete stock item prices");

	/*and cascade delete the bill of material if any */
	$sql = "DELETE FROM ".TB_PREF."bom WHERE parent='$stock_id'";
	db_query($sql, "could not delete stock item bom");
}

function get_item($stock_id)
{
	$sql = "SELECT ".TB_PREF."stock_master.*,".TB_PREF."item_tax_types.name AS tax_type_name
		FROM ".TB_PREF."stock_master,".TB_PREF."item_tax_types
		WHERE ".TB_PREF."item_tax_types.id=".TB_PREF."stock_master.tax_type_id
		AND stock_id='$stock_id'";
	$result = db_query($sql,"an item could not be retreived");

	return db_fetch($result);
}

function get_items()
{
	$sql = "SELECT * FROM ".TB_PREF."stock_master";
	return db_query($sql,"items could not be retreived");
}

?>