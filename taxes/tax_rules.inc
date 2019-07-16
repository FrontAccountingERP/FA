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
/*
	FA tax basic rules.
*/
define('TAX_NONE', 0); // none option
define('TQ_NONE', 0); // none option

// implemented taxation options
// basic:
define('TAX_PAYABLE', 1);
define('TAX_DEDUCTIBLE', 2);
define('TAX_CHARGED', 4);
// category depenedent:
define('TAX_DUEDATE', 8);
define('TAX_PARTIAL', 16);

// quirks dependent on vat_categories
define('TQ_NONDEDUCT', 1);	// never deductible
define('TQ_PARTIAL', 2);	// partially deductible (using global factor)
define('TQ_DUEDATE', 4);	// taxable on due date instead of transaction date
define('TQ_REVERSE', 8);	// tax is reverse charged
define('TQ_IMPDEDUCTIBLE', 16);	// import tax is deductible

class tax_system
{
	//-------------------------- entity taxation rules ----------------------------
	// general rules: who and when pays taxes
	//
	// FIXME: credit notes
	//
	var	$description;
	var $ts_basic_rules;
	var $ts_category_quirks;

	function __construct()
	{
		$this->ts_basic_rules = array(
			TA_DOMESTIC => array(
				ST_SALESQUOTE => TAX_CHARGED,
				ST_SALESORDER => TAX_CHARGED,
				ST_CUSTDELIVERY => TAX_CHARGED,
				ST_SALESINVOICE => TAX_PAYABLE | TAX_CHARGED,
				ST_CUSTCREDIT => TAX_PAYABLE | TAX_CHARGED,
				ST_PURCHORDER => TAX_CHARGED,
				ST_SUPPRECEIVE => TAX_CHARGED,
				ST_SUPPINVOICE => TAX_DEDUCTIBLE | TAX_CHARGED,
				ST_SUPPCREDIT => TAX_DEDUCTIBLE | TAX_CHARGED,
			),
			TA_EXPORT => array(
				ST_SALESQUOTE => TAX_NONE,
				ST_SALESORDER => TAX_NONE,
				ST_CUSTDELIVERY => TAX_NONE,
				ST_SALESINVOICE => TAX_NONE,
				ST_CUSTCREDIT => TAX_NONE,
				ST_PURCHORDER => TAX_NONE,
				ST_SUPPRECEIVE => TAX_NONE,
				ST_SUPPINVOICE => TAX_NONE,
				ST_SUPPCREDIT => TAX_NONE,
			),
			TA_EU => array(
				ST_SALESQUOTE => TAX_NONE,
				ST_SALESORDER => TAX_NONE,
				ST_CUSTDELIVERY => TAX_NONE,
				ST_SALESINVOICE => TAX_NONE,
				ST_CUSTCREDIT => TAX_NONE,
				ST_PURCHORDER => TAX_NONE,
				ST_SUPPRECEIVE => TAX_NONE,
				ST_SUPPINVOICE => TAX_DEDUCTIBLE | TAX_PAYABLE,
				ST_SUPPCREDIT => TAX_DEDUCTIBLE | TAX_PAYABLE,
			),
		);

		//-------------------------- special goods dependent rules  ----------------------------
		//
		$this->ts_category_quirks = array(
			VC_OTHER => TQ_NONE,
			VC_MEDIA => TQ_DUEDATE, 		// is no longer used ?
			VC_ASSETS => TQ_NONE,			// just separate category in tax reg
			VC_NONDEDUCT => TQ_NONDEDUCT,
			VC_SERVICES => TQ_IMPDEDUCTIBLE,
			VC_PARTIAL => TQ_PARTIAL,
			VC_REVERSE => TQ_REVERSE,
		);
	}
	/*
		Returns tax options applicable for the arguments set

		$allow_reverse - decides whether reverse charging option is honoured for sales invoice
		(this depends on 'continue transaction' value as defined by law, so have to be set by case).
	*/
	function options($trans_type, $tax_area, $vat_category, $allow_reverse=true)
	{
		if (!isset($vat_category)) // exempt goods has really no category
			return TAX_NONE;

		$options = $this->ts_basic_rules[$tax_area][$trans_type];

		// per vat category quirks
		$quirks = $this->ts_category_quirks[$vat_category];

		if ($quirks & TQ_DUEDATE)
			$options |= TAX_DUEDATE;

		if ($quirks & TQ_NONDEDUCT)
			$options &= ~TAX_DEDUCTIBLE;

		if ($quirks & TQ_PARTIAL)
			if ($options & TAX_DEDUCTIBLE)
				$options |= TAX_PARTIAL;

		if ($quirks & TQ_IMPDEDUCTIBLE)
			if ($tax_area == TA_EXPORT && in_array($trans_type, array(ST_SUPPINVOICE, ST_SUPPCREDIT)))
			{
				$options |= TAX_DEDUCTIBLE | TAX_PAYABLE;
			}

		if (($quirks & TQ_REVERSE) && (!in_array($trans_type,array(ST_SALESINVOICE, ST_CUSTCREDIT)) || $allow_reverse))
			if ($tax_area == TA_DOMESTIC)
			{
				$options ^= TAX_PAYABLE;
				$options &= ~TAX_CHARGED;
			}

		return $options;
	}
};

function dbg_tax($options)
{
	$dbg_rules = array(
		TAX_PAYABLE => 'payable',
		TAX_DEDUCTIBLE => 'deductible',
		TAX_CHARGED => 'charged',
		TAX_DUEDATE => 'due_date',
		TAX_PARTIAL => 'partial'
	);

	$opts = array();
	foreach($dbg_rules as $key => $name)
		if ($options & $key)
			$opts[] = $name;

	_vd(implode($opts, ','));
}
