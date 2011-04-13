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
include_once($path_to_root . "/taxes/db/tax_groups_db.inc");
include_once($path_to_root . "/taxes/db/tax_types_db.inc");
include_once($path_to_root . "/taxes/db/item_tax_types_db.inc");

//---------------------------------------------------------------------------------

// returns the price of a given item minus any included taxes
// for item $stock_id with line price $price,
// with applicable tax rates $tax_group_array or group id $tax_group

function get_tax_free_price_for_item($stock_id, $price, $tax_group, $tax_included, $tax_group_array=null)
{
	// if price is zero, then can't be taxed !
	if ($price == 0)
		return 0;

	if ($tax_included==0) return $price;
	
	// if array already read, then make a copy and use that
	if ($tax_group_array)
		$ret_tax_array = $tax_group_array;
	else
		$ret_tax_array = get_tax_group_items_as_array($tax_group);
	
	//print_r($ret_tax_array);

	$tax_array = get_taxes_for_item($stock_id, $ret_tax_array);
	// if no exemptions or taxgroup is empty, then no included/excluded taxes
	if ($tax_array == null)
		return $price;
	
	$tax_multiplier = 0;

	// loop for all taxes

	foreach ($tax_array as $taxitem) 
	{
		$tax_multiplier += $taxitem["rate"];
	}

	return round($price / (1 + ($tax_multiplier / 100)),  user_price_dec());
}
//
//	Full price (incl. VAT) for item $stock_id with line price $price,
//	with tax rates $tax_group_array or applicable group $tax_group
//
function get_full_price_for_item($stock_id, $price, $tax_group, $tax_included, $tax_group_array=null)
{
	// if price is zero, then can't be taxed !
	if ($price == 0)
		return 0;

	if ($tax_included==1) return $price;

	// if array already read, then make a copy and use that
	if ($tax_group_array)
		$ret_tax_array = $tax_group_array;
	else
		$ret_tax_array = get_tax_group_items_as_array($tax_group);
	
	//print_r($ret_tax_array);

	$tax_array = get_taxes_for_item($stock_id, $ret_tax_array);
	// if no exemptions or taxgroup is empty, then no included/excluded taxes
	if ($tax_array == null)
		return $price;
	
	$tax_multiplier = 0;	

	// loop for all items

	foreach ($tax_array as $taxitem) 
	{
			$tax_multiplier += $taxitem["rate"];
	}
	
	return round($price * (1 + ($tax_multiplier / 100)),  user_price_dec());
}

//---------------------------------------------------------------------------------
// return an array of (tax_type_id, tax_type_name, sales_gl_code, purchasing_gl_code, rate)

function get_taxes_for_item($stock_id, $tax_group_items_array)
{
	$item_tax_type = get_item_tax_type_for_item($stock_id);
	
	// if the item is exempt from all taxes then return 0
	if ($item_tax_type["exempt"])
		return null;
		
	// get the exemptions for this item tax type
	$item_tax_type_exemptions_db = get_item_tax_type_exemptions($item_tax_type["id"]);
	
	// read them all into an array to minimize db querying
	$item_tax_type_exemptions = array();
	while ($item_tax_type_exemp = db_fetch($item_tax_type_exemptions_db)) 
	{
		$item_tax_type_exemptions[] = $item_tax_type_exemp["tax_type_id"];
	}
	
	$ret_tax_array = array();
	
	// if any of the taxes of the tax group are in the exemptions, then skip
	foreach ($tax_group_items_array as $tax_group_item) 
	{ 
		
		$skip = false;			
		
		// if it's in the exemptions, skip
		foreach ($item_tax_type_exemptions as $exemption) 
		{
			if (($tax_group_item['tax_type_id'] == $exemption)) 
			{
    			$skip = true;
    			break;
			}
		}
		
		if (!$skip) 
		{
			$index = $tax_group_item['tax_type_id'];
			$ret_tax_array[$index] = $tax_group_item;
		}
	}
	
	return $ret_tax_array;
}
//-----------------------------------------------------------------------------------
// return an array of (tax_type_id, tax_type_name, sales_gl_code, purchasing_gl_code, rate, included_in_price, Value) 

function get_tax_for_items($items, $prices, $shipping_cost, $tax_group, $tax_included=null, $tax_items_array=null)
{
	// first create and set an array with all the tax types of the tax group
	if($tax_items_array!=null)
	  $ret_tax_array = $tax_items_array;
	else
	  $ret_tax_array = get_tax_group_items_as_array($tax_group);

	foreach($ret_tax_array as $k=>$t)
		$ret_tax_array[$k]['Net'] = 0;

	// loop for all items
	for ($i = 0; $i < count($items); $i++)
	{
		$item_taxes = get_taxes_for_item($items[$i], $ret_tax_array);
		if ($item_taxes != null) 
		{
			foreach ($item_taxes as $item_tax) 
			{
				if ($item_tax['rate'] !== null) {
					$index = $item_tax['tax_type_id'];
					if($tax_included==1) {// 2008-11-26 Joe Hunt Taxes are stored without roundings
					  $nprice = get_tax_free_price_for_item($items[$i], $prices[$i], $tax_group, $tax_included, null, $item_tax['tax_type_id']);
					  $ret_tax_array[$index]['Value'] += $nprice*$item_tax['rate']/100;
					  $ret_tax_array[$index]['Net'] += $nprice;
					} else {
					  $ret_tax_array[$index]['Value'] += ($prices[$i] * $item_tax['rate'] / 100);
					  $ret_tax_array[$index]['Net'] += $prices[$i];
					}
				}
			}
		}
	}

	// add the shipping taxes, only if non-zero, and only if tax group taxes shipping
	if ($shipping_cost != 0) 
	{
		$item_taxes = get_shipping_tax_as_array();
		if ($item_taxes != null) 
		{
			if ($tax_included == 1)
			{
				$tax_rate = 0;
				foreach ($item_taxes as $item_tax)
				{
					$index = $item_tax['tax_type_id'];
					if(isset($ret_tax_array[$index])) {
						$tax_rate += $item_tax['rate'];
					}
				}
				$shipping_net = round2($shipping_cost / (1 + ($tax_rate / 100)), user_price_dec());
			}
			foreach ($item_taxes as $item_tax) 
			{
				$index = $item_tax['tax_type_id'];
				if ($item_tax['rate'] !== null && $ret_tax_array[$index]['rate'] !== null) {
					  if($tax_included==1) {// 2008-11-26 Joe Hunt Taxes are stored without roundings
						$ret_tax_array[$index]['Value'] += ($shipping_net * $item_tax['rate'] / 100);
					    $ret_tax_array[$index]['Net'] += $shipping_net;
					  } else {
						$ret_tax_array[$index]['Value'] += ($shipping_cost * $item_tax['rate'] / 100);
					    $ret_tax_array[$index]['Net'] += $shipping_cost;
					  }
				}
			}
		}
	}
	//print_r($ret_tax_array);

	return $ret_tax_array;
}

?>