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
function clear_shipping_tax_group() {
	  $sql = "UPDATE ".TB_PREF."tax_groups SET tax_shipping=0 WHERE 1";
	  db_query($sql, "could not update tax_shipping fields");	  
}

function add_tax_group($name, $tax_shipping, $taxes, $rates)
{
	begin_transaction();

	if($tax_shipping) 	// only one tax group for shipping
	  clear_shipping_tax_group();
		
	$sql = "INSERT INTO ".TB_PREF."tax_groups (name, tax_shipping) VALUES (".db_escape($name).", ".db_escape($tax_shipping).")";
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
	
    $sql = "UPDATE ".TB_PREF."tax_groups SET name=".db_escape($name).",tax_shipping=".db_escape($tax_shipping)." WHERE id=".db_escape($id);
	db_query($sql, "could not update tax group");
	
	delete_tax_group_items($id);
	add_tax_group_items($id, $taxes, $rates);	
	
	commit_transaction();			
}

function get_all_tax_groups($all=false)
{
	$sql = "SELECT * FROM ".TB_PREF."tax_groups";
	if (!$all) $sql .= " WHERE !inactive";
	
	return db_query($sql, "could not get all tax group");
} 

function get_tax_group($type_id)
{
	$sql = "SELECT * FROM ".TB_PREF."tax_groups WHERE id=".db_escape($type_id);
	
	$result = db_query($sql, "could not get tax group");
	
	return db_fetch($result);
}

function delete_tax_group($id)
{
	begin_transaction();
		
	$sql = "DELETE FROM ".TB_PREF."tax_groups WHERE id=".db_escape($id);
		
	db_query($sql, "could not delete tax group");
	
	delete_tax_group_items($id);	
	
	commit_transaction();
}

function add_tax_group_items($id, $items, $rates)
{
	for ($i=0; $i < count($items); $i++) 
	{
		$sql = "INSERT INTO ".TB_PREF."tax_group_items (tax_group_id, tax_type_id, rate)
			VALUES (".db_escape($id).",  ".db_escape($items[$i]).", " . $rates[$i] .")";
		db_query($sql, "could not add item tax group item");					
	}		
}

function delete_tax_group_items($id)
{
	$sql = "DELETE FROM ".TB_PREF."tax_group_items WHERE tax_group_id=".db_escape($id);
	
	db_query($sql, "could not delete item tax group items");					
}

//
//	Return all tax types with rate value updated according to tax group selected
//	Call the function without arg to find shipment group taxes.
//
function get_tax_group_rates($group_id=null)
{
	global $suppress_tax_rates;

	$sql = 
	"SELECT t.id as tax_type_id,"
		.(isset($suppress_tax_rates) && $suppress_tax_rates == 1
			? "t.name as tax_type_name,"
			: "CONCAT(t.name, ' (', t.rate, '%)') as tax_type_name,")
		."t.sales_gl_code,
		  t.purchasing_gl_code,
		  IF(g.tax_type_id, t.rate, NULL) as rate
		FROM ".TB_PREF."tax_types t 
		  LEFT JOIN ".TB_PREF."tax_group_items g ON t.id=g.tax_type_id
			AND g.tax_group_id=". ($group_id ? db_escape($group_id) : "(SELECT id FROM ".TB_PREF."tax_groups WHERE tax_shipping=1)")
		." WHERE !t.inactive";

	return db_query($sql, "cannot get tax types as array");
}

function get_tax_group_items_as_array($id)
{
	$active_taxes = $ret_tax_array = array();

	$tax_group_items = get_tax_group_rates($id);

	while ($tax_group_item = db_fetch_assoc($tax_group_items)) 
	{
		$tax_group_item['Value'] = 0;
		$ret_tax_array[$tax_group_item['tax_type_id']] = $tax_group_item;
	}

	return $ret_tax_array;
}


function get_shipping_tax_as_array()
{
	$active_taxes = $ret_tax_array = array();

	$tax_group_items = get_tax_group_rates();

	while ($tax_group_item = db_fetch_assoc($tax_group_items)) 
	{
		$tax_group_item['Value'] = 0;
		$ret_tax_array[$tax_group_item['tax_type_id']] = $tax_group_item;
	}

	return $ret_tax_array;
}

