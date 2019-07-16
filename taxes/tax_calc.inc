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
// Returns basic fiscal parameters for transaction item which depend on stock type and customer/supplier tax area.
//
// vat_category => stock tax category 
// tax_area => cust/supp tax area
// taxes => all taxes applicable:
// 	tax_type_id1 => (tax_type_id, tax_type_name, sales_gl_code, purchasing_gl_code, rate)
// 	tax_type_id2 => ...

function get_base_taxdata($stock_id, $group_id)
{
	static $last_group = null, $group_data;

	if ($last_group != $group_id) {	// cache group data for better performance
		$last_group = $group_id;
		$group_data = get_tax_group_data($group_id);
	}

	$taxdata = array('tax_area' => $group_data['tax_area'], 'taxes' => null);

	if ($stock_id===null) // shipping special case
	{
		$taxdata['vat_category'] = VC_SERVICES;
		$taxdata['taxes'] = get_tax_group_items_as_array(null);
		return $taxdata;
	}

	$item_tax_type = get_item_tax_type_for_item($stock_id);	// get item tax data
	$taxdata['vat_category'] = $item_tax_type['vat_category'];

	// if the item is exempt from all taxes thats all
	if ($item_tax_type["exempt"])
		return $taxdata;

	$taxdata['taxes'] = array();

	// get the exemptions for this item tax type
	$item_tax_type_exemptions_db = get_item_tax_type_exemptions($item_tax_type["id"]);

	// read them all into an array to minimize db querying
	$item_tax_type_exemptions = array();
	while ($item_tax_type_exemp = db_fetch($item_tax_type_exemptions_db)) 
	{
		$item_tax_type_exemptions[] = $item_tax_type_exemp["tax_type_id"];
	}

	$tax_group_items_array = $group_data['taxes'];
	// if any of the taxes of the tax group are in the exemptions, then skip
	foreach ($tax_group_items_array as $tax_group_item) 
	{

		$skip = false;

		// if it's in the exemptions, skip
		if (is_null($tax_group_item['rate']))
			$skip = true;
		else
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
			$taxdata['taxes'][$index] = $tax_group_item;
		}
	}

	return $taxdata;
}

/*
	Main tax procedure splitting transaction item value according to item tax rules applicable:
		$stock_id - stock item code; NULL for shipping
		$amount - price/value to be splitted
		$tax_group - entity tax group
		$tax_included - whether value includes all taxes
		$vat_factor - 0-1; tax deduction factor (purchases, not used for now; depends on whthere it is related to exempt or taxed sales)
		$allow_reverse - option for invoice - whether to honour reverse charging (depends on customer tax status)

	Returned array contains calculated values for GL postings and tax registration:
		'Net' - value without tax,
		'Tax' - tax sum,
		'Cost' - cost value (can be higher then Net value),
		'vat_category' -  stock tax category

	and (with numeric keys) detailed info for any applicable tax rate:
		'tax_type_id' - tax type id
		'Value' - charged tax value
		'Deductible' - tax deductible (can be lower than Value for special goods or mixed sales structure)
		'Payable' - tax payable
		'Adjust' - additional adjustment to deductible tax due to sales structure factor
		'rate' - tax rate
		'sales_gl_code' - sales tax GL account
		'purchasing_gl_code' - purchase tax GL account
		'tax_type_name' - name of tax type

	Price value is splitted as follows:
		Tax: sum of Value (for applicable taxes)
		Net: amount - Tax
		Cost: Net + sum(Payable-Deductible)
	 for every applicable tax rate:
		Value: tax calculated or 0
		Deductible: vat_factor*Value or 0
		Adjust: Value-Deductible or 0
		Payable: 0 or Value
*/
function split_item_price($stock_id, $amount, $group_id, $tax_included=false, $trans_type=ST_SUPPINVOICE, $vat_factor = 1, 
	$allow_reverse=true)
{
	global $TS;

	$dec = user_price_dec();

	$itemdata = get_base_taxdata($stock_id, $group_id);
	$vat_category = $itemdata['vat_category'];
	$item_taxes = $itemdata['taxes'];

	$taxopt = $TS->options($trans_type, $itemdata['tax_area'], $vat_category, $allow_reverse);

	if (empty($item_taxes))
	{
		  $ret_array['Net'] = $amount;
		  $ret_array['Cost'] = $amount;
		  $ret_array['Tax'] = 0;
		  if (!is_null($item_taxes))	// register empty taxes only for not fully exempt items
				$ret_array[] = array('Value'=>0, 'rate' => null, 'tax_type_id' => null, 'Deductible'=>0, 'Adjust' => 0, 'Payable' => 0);
	}
	else
	{
		$ret_array['Net'] = $ret_array['Cost'] = $ret_array['Tax'] = 0;

		$tax_multiplier = 0;

		if ($taxopt&TAX_CHARGED)	// divide tax for net and tax only if charged on document
			foreach ($item_taxes as $taxitem) 
				$tax_multiplier += $taxitem['rate'];

		$partial_vat_percent = get_company_pref('partial_vat_percent');

		foreach ($item_taxes as $tax_id => $item_tax) 
		{
				if ($item_tax['rate'] !== null)
				{

					// effective vat for some special purchases is lower than nominal
					$factor = $vat_category == VC_NONDEDUCT ? 0 : ($vat_category==VC_PARTIAL ? $partial_vat_percent/100.0 : 1);

					$net_value = $amount;
					if ($tax_included == true) {
						$vat_value = round($amount*$item_tax['rate']/(100+$tax_multiplier), 2);

						if ($taxopt&TAX_CHARGED)
							$net_value -= $vat_value;

					} else {

						$vat_value = round($amount * $item_tax['rate'] / 100, 2);
					}

					$ret_array['Net'] = round2($net_value, $dec);
			    	$ret_array['Cost'] = $ret_array['Net'];

					$tax = array('Value' => 0, 'Deductible' => 0, 'Adjust' => 0, 'Payable' => 0);

			    	$tax['purchasing_gl_code'] = $item_tax['purchasing_gl_code'];
			    	$tax['sales_gl_code'] = $item_tax['sales_gl_code'];
			    	$tax['rate'] = $item_tax['rate'];
					$tax['tax_type_id'] = $item_tax['tax_type_id'];
					$tax['tax_type_name'] = $item_tax['tax_type_name'];

					if ($taxopt & TAX_CHARGED)			 				// tax is charged on document
						$tax['Value'] =  round2($vat_value, $dec);

					if ($taxopt & TAX_PAYABLE)							 // transaction is taxable
						$tax['Payable'] =  round2($vat_value, $dec);

			    	if ($taxopt & TAX_DEDUCTIBLE) // tax is deductible
			    	{
						$tax['Deductible'] = round2($vat_value*$factor, 2); // avoid rounding issues if $dec > 2 decimal places
	  					$tax['Adjust'] = round2(-(1-$vat_factor)*$factor*$vat_value, $dec); // adjustment due to mixed taxed/exmpt sales activity
				    } else {
						$tax['Deductible'] = 0;
	  					$tax['Adjust'] = 0;
					}

					if ($tax['Payable'])
				    	$ret_array['Cost'] += ($tax['Payable'] - $tax['Deductible']);
				    elseif ($tax['Deductible'])
				    	$ret_array['Cost'] += $tax['Adjust'];

			    	$ret_array[] = $tax;
					$ret_array['Tax'] += $tax['Value'];
				}
		}
	}
    $ret_array['vat_category'] = $vat_category;
	return $ret_array;
}

//-----------------------------------------------------------------------------------
// return an array of (tax_type_id, tax_type_name, sales_gl_code, purchasing_gl_code, rate, included_in_price, Value, Net)
//
// $vat_factors - effective part of vat values included in tax; calculated but not included vat is added to net value
//
function get_tax_for_items($trans_type, $items, $prices, $shipping_cost, $tax_group, $tax_included=null,
	$tax_algorithm = null, $vat_factors = null, $allow_reverse = true)
{

	// if shipping cost is passed, just add to the prices/items tables
	if ($shipping_cost != 0)
	{
		$items[] = NULL;
		$prices[] = $shipping_cost;
		if ($vat_factors)
			$vat_factors[] = 1;
	}

	// calculate tax sums
	$ret_tax_array = array();
	foreach($items as $i => $stock_id)
	{
		$taxdata = split_item_price($stock_id, $prices[$i], $tax_group, $tax_included, $trans_type,
			 $vat_factors ? $vat_factors[$i] : 1, $allow_reverse);

		foreach ($taxdata as $key => $data)
		{
			if (is_numeric($key))
			{
				$tax_id = isset($data['tax_type_id']) ? $data['tax_type_id'] : 'exempt';

				if (!isset($ret_tax_array[$tax_id]))
				{
					$ret_tax_array[$tax_id] = $data;
					$ret_tax_array[$tax_id]['Net'] = $taxdata['Net'];
					$ret_tax_array[$tax_id]['vat_category'] = $taxdata['vat_category'];
				}
				else
				{
					foreach(array('Value', 'Payable', 'Deductible', 'Adjust') as $amt)
						$ret_tax_array[$tax_id][$amt] += $data[$amt];
					$ret_tax_array[$tax_id]['Net'] += $taxdata['Net'];
				}
			}
		}
	}

	if (!$tax_algorithm)
		$tax_algorithm = get_company_pref('tax_algorithm');

	if ($tax_algorithm == TCA_TOTALS) { // ?
		$dec = user_price_dec();
		// update taxes with 
		foreach($ret_tax_array as $index => $item_tax) {
			if ($ret_tax_array[$index]['Value'])
				$ret_tax_array[$index]['Value'] = round2($item_tax['Net'] * $item_tax['rate'] / 100, $dec);
		}
	}

	return $ret_tax_array;
}


//---------------------------------------------------------------------------------

// returns the price of a given item minus any included taxes
// for item $stock_id with line price $price and supplier/customer group_id $tax_group

function get_tax_free_price_for_item($trans_type, $stock_id, $price, $tax_group, $tax_included, $allow_reverse = true)
{
	// if price is zero, then can't be taxed !
	if ($price == 0)
		return 0;

	if ($tax_included==0) return $price;

	$taxdata = split_item_price($stock_id, $price, $tax_group, $tax_included, $trans_type, 1, $allow_reverse);

	return $taxdata['Net'];
}
//
//	Full price (incl. VAT) for item $stock_id
// 	calculated for line price $price, and applicable group $tax_group
//
function get_full_price_for_item($trans_type, $stock_id, $price, $tax_group, $tax_included, $allow_reverse = true)
{
	// if price is zero, then can't be taxed !
	if ($price == 0)
		return 0;

	if ($tax_included==1) return $price;

	$taxdata = split_item_price($stock_id, $price, $tax_group, $tax_included, $trans_type, 1, $allow_reverse);

	return $taxdata['Net'] + $taxdata['Tax'];
}
