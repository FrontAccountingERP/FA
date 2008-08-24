<?php

function clear_shipping_tax_group() {
	  $sql = "UPDATE ".TB_PREF."tax_groups SET tax_shipping=0 WHERE 1";
	  db_query($sql, "could not update tax_shipping fields");	  
}

function add_tax_group($name, $tax_shipping, $taxes, $rates)
{
	begin_transaction();

	if($tax_shipping) 	// only one tax group for shipping
	  clear_shipping_tax_group();
		
	$sql = "INSERT INTO ".TB_PREF."tax_groups (name, tax_shipping) VALUES (".db_escape($name).", $tax_shipping)";
	db_query($sql, "could not add tax group");
	
	$id = db_insert_id();
	
	add_tax_group_items($id, $taxes, $rates);	
	
	commit_transaction();	
}

function update_tax_group($id, $name, $tax_shipping, $taxes, $rates)
{
	begin_transaction();	

	if($tax_shipping) 	// only one tax group for shipping
	  clear_shipping_tax_group();
	
    $sql = "UPDATE ".TB_PREF."tax_groups SET name=".db_escape($name).",tax_shipping=$tax_shipping WHERE id=$id";
	db_query($sql, "could not update tax group");
	
	delete_tax_group_items($id);
	add_tax_group_items($id, $taxes, $rates);	
	
	commit_transaction();			
}

function get_all_tax_groups()
{
	$sql = "SELECT * FROM ".TB_PREF."tax_groups";
	
	return db_query($sql, "could not get all tax group");
} 

function get_tax_group($type_id)
{
	$sql = "SELECT * FROM ".TB_PREF."tax_groups WHERE id=$type_id";
	
	$result = db_query($sql, "could not get tax group");
	
	return db_fetch($result);
}

function delete_tax_group($id)
{
	begin_transaction();
		
	$sql = "DELETE FROM ".TB_PREF."tax_groups WHERE id=$id";
		
	db_query($sql, "could not delete tax group");
	
	delete_tax_group_items($id);	
	
	commit_transaction();
}

function add_tax_group_items($id, $items, $rates)
{
	for ($i=0; $i < count($items); $i++) 
	{
		$sql = "INSERT INTO ".TB_PREF."tax_group_items (tax_group_id, tax_type_id, rate)
			VALUES ($id,  " . $items[$i] . ", " . $rates[$i] .")";
		db_query($sql, "could not add item tax group item");					
	}		
}

function delete_tax_group_items($id)
{
	$sql = "DELETE FROM ".TB_PREF."tax_group_items WHERE tax_group_id=$id";
	
	db_query($sql, "could not delete item tax group items");					
}

function get_tax_group_items($id)
{
	$sql = "SELECT ".TB_PREF."tax_group_items.*, ".TB_PREF."tax_types.name AS tax_type_name, 
		".TB_PREF."tax_types.sales_gl_code, ".TB_PREF."tax_types.purchasing_gl_code  
		FROM ".TB_PREF."tax_group_items, ".TB_PREF."tax_types 
		WHERE tax_group_id=$id
			AND ".TB_PREF."tax_types.id=tax_type_id";
	
	return db_query($sql, "could not get item tax type group items");
}

function get_tax_group_items_as_array($id)
{
	$ret_tax_array = array();
	
	$tax_group_items = get_tax_group_items($id);
	
	while ($tax_group_item = db_fetch($tax_group_items)) 
	{
		$index = $tax_group_item['tax_type_id'];
		$ret_tax_array[$index]['tax_type_id'] = $tax_group_item['tax_type_id'];
		$ret_tax_array[$index]['tax_type_name'] = $tax_group_item['tax_type_name'];
		$ret_tax_array[$index]['sales_gl_code'] = $tax_group_item['sales_gl_code'];
		$ret_tax_array[$index]['purchasing_gl_code'] = $tax_group_item['purchasing_gl_code'];
		$ret_tax_array[$index]['rate'] = $tax_group_item['rate'];
		$ret_tax_array[$index]['Value'] = 0;
	}
	
	return $ret_tax_array;
}

function get_shipping_tax_group_items()
{

	$sql = "SELECT ".TB_PREF."tax_group_items.*, ".TB_PREF."tax_types.name AS tax_type_name, 
		".TB_PREF."tax_types.sales_gl_code, ".TB_PREF."tax_types.purchasing_gl_code  
		FROM " .TB_PREF."tax_group_items, ".TB_PREF."tax_types, ".TB_PREF."tax_groups
		WHERE " .TB_PREF."tax_groups.tax_shipping=1
		AND " .TB_PREF."tax_groups.id=tax_group_id
		AND ".TB_PREF."tax_types.id=tax_type_id";
	return db_query($sql, "could not get shipping tax group items");
}

function get_shipping_tax_as_array()
{
	$ret_tax_array = array();

	
	$tax_group_items = get_shipping_tax_group_items();

	while ($tax_group_item = db_fetch($tax_group_items)) 
	{
		$index = $tax_group_item['tax_type_id'];
		$ret_tax_array[$index]['tax_type_id'] = $tax_group_item['tax_type_id'];
		$ret_tax_array[$index]['tax_type_name'] = $tax_group_item['tax_type_name'];
		$ret_tax_array[$index]['sales_gl_code'] = $tax_group_item['sales_gl_code'];
		$ret_tax_array[$index]['purchasing_gl_code'] = $tax_group_item['purchasing_gl_code'];
		$ret_tax_array[$index]['rate'] = $tax_group_item['rate'];
		$ret_tax_array[$index]['Value'] = 0;
	}
	
	return $ret_tax_array;
}
?>