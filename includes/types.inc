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
//----------------------------------------------------------------------------------
//	FrontAccounting system transaction types
//
define('ST_JOURNAL', 0);

define('ST_BANKPAYMENT', 1);
define('ST_BANKDEPOSIT', 2);
define('ST_BANKTRANSFER', 4);

define('ST_SALESINVOICE', 10);
define('ST_CUSTCREDIT', 11);
define('ST_CUSTPAYMENT', 12);
define('ST_CUSTDELIVERY', 13);

define('ST_LOCTRANSFER', 16);
define('ST_INVADJUST', 17);

define('ST_PURCHORDER', 18);
define('ST_SUPPINVOICE', 20);
define('ST_SUPPCREDIT', 21);
define('ST_SUPPAYMENT', 22);
define('ST_SUPPRECEIVE', 25);

define('ST_WORKORDER', 26);
define('ST_MANUISSUE', 28);
define('ST_MANURECEIVE', 29);


define('ST_SALESORDER', 30);
define('ST_SALESQUOTE', 32);
define('ST_COSTUPDATE', 35);
define('ST_DIMENSION', 40);

// Don't include these defines in the $systypes_array.
// They are used for documents only.
define ('ST_STATEMENT', 91);
define ('ST_CHEQUE', 92);

$systypes_array = array (
	ST_JOURNAL => _("Journal Entry"),
	ST_BANKPAYMENT => _("Bank Payment"),
	ST_BANKDEPOSIT => _("Bank Deposit"),
	ST_BANKTRANSFER => _("Funds Transfer"),
	ST_SALESINVOICE => _("Sales Invoice"),
	ST_CUSTCREDIT => _("Customer Credit Note"),
	ST_CUSTPAYMENT => _("Customer Payment"),
	ST_CUSTDELIVERY => _("Delivery Note"),
	ST_LOCTRANSFER => _("Location Transfer"),
	ST_INVADJUST => _("Inventory Adjustment"),
	ST_PURCHORDER => _("Purchase Order"),
	ST_SUPPINVOICE => _("Supplier Invoice"),
	ST_SUPPCREDIT => _("Supplier Credit Note"),
	ST_SUPPAYMENT => _("Supplier Payment"),
	ST_SUPPRECEIVE => _("Purchase Order Delivery"),
	ST_WORKORDER => _("Work Order"),
	ST_MANUISSUE => _("Work Order Issue"),
	ST_MANURECEIVE => _("Work Order Production"),
	ST_SALESORDER => _("Sales Order"),
	ST_SALESQUOTE => _("Sales Quotation"),
	ST_COSTUPDATE => _("Cost Update"),
	ST_DIMENSION => _("Dimension")
	);

//----------------------------------------------------------------------------------
//		Bank transaction types
//
define('BT_TRANSFER', 0);
define('BT_CHEQUE', 1);
define('BT_CREDIT', 2);
define('BT_CASH', 3);

$bank_account_types = array (
	BT_TRANSFER => _("Savings Account"),
		_("Chequing Account"),
		_("Credit Account"),
		_("Cash Account")
	);

$bank_transfer_types = array(
	BT_TRANSFER => _("Transfer"),
			_("Cheque"),
			_("Credit"),
			_("Cash")
	);

include_once($path_to_root . "/manufacturing/includes/manufacturing_db.inc");
include_once($path_to_root . "/purchasing/includes/purchasing_db.inc");
include_once($path_to_root . "/sales/includes/sales_db.inc");
include_once($path_to_root . "/dimensions/includes/dimensions_db.inc");
//----------------------------------------------------------------------------------
//	Payment types
//
define('PT_MISC', 0);
define('PT_WORKORDER', 1);
define('PT_CUSTOMER', 2);
define('PT_SUPPLIER', 3);
define('PT_QUICKENTRY', 4);
define('PT_DIMESION', 5);

$payment_person_types = array (
	PT_MISC => _("Miscellaneous"),
				_("Work Order"),
				_("Customer"),
				_("Supplier"),
				_("Quick Entry")
	);

function payment_person_currency($type, $person_id)  {
	switch ($type)
	{
		case PT_MISC :
		case PT_QUICKENTRY :
		case PT_WORKORDER :
			return get_company_currency();

		case PT_CUSTOMER :
			return get_customer_currency($person_id);

		case PT_SUPPLIER :
			return get_supplier_currency($person_id);

		default :
			return get_company_currency();
	}
}

function payment_person_name($type, $person_id, $full=true) {
	global $payment_person_types;

	switch ($type)
	{
		case PT_MISC :
			return $person_id;
		case PT_QUICKENTRY :
			$qe = get_quick_entry($person_id);
			return ($full ? $payment_person_types[$type] . " ":"") . $qe["description"];
		case PT_WORKORDER :
			global $wo_cost_types;
			return $wo_cost_types[$person_id];
		case PT_CUSTOMER :
			return ($full ?$payment_person_types[$type] . " ":"") . get_customer_name($person_id);
		case PT_SUPPLIER :
			return ($full ? $payment_person_types[$type] . " ":"") . get_supplier_name($person_id);
		default :
			//DisplayDBerror("Invalid type sent to person_name");
			//return;
			return '';
	}
}

function payment_person_has_items($type) {
	switch ($type)
	{
		case PT_MISC :
			return true;
		case PT_QUICKENTRY :
			return db_has_quick_entries();
		case PT_WORKORDER : // 070305 changed to open workorders JH
			return db_has_open_workorders();
		case PT_CUSTOMER :
			return db_has_customers();
		case PT_SUPPLIER :
			return db_has_suppliers();
		default :
			display_db_error("Invalid type sent to has_items", "");
			return false;
	}
}

//----------------------------------------------------------------------------------
//	Manufacturing types
//
define('WO_ASSEMBLY', 0);
define('WO_UNASSEMBLY', 1);
define('WO_ADVANCED', 2);

$wo_types_array = array (
	WO_ASSEMBLY => _("Assemble"),
	WO_UNASSEMBLY => _("Unassemble"),
	WO_ADVANCED => _("Advanced Manufacture")
	);

define('WO_LABOUR', 0);
define('WO_OVERHEAD', 1);

$wo_cost_types = array(
		WO_LABOUR => _("Labour Cost"),
		WO_OVERHEAD => _("Overhead Cost"),
);

//----------------------------------------------------------------------------------
//	GL account classes
//
define('CL_NONE', 0); // for backward compatibility
define('CL_ASSETS', 1);
define('CL_LIABILITIES', 2);
define('CL_EQUITY', 3);
define('CL_INCOME', 4);
define('CL_COGS', 5);
define('CL_EXPENSE', 6);

$class_types = array(
		CL_ASSETS => _("Assets"),
		CL_LIABILITIES => _("Liabilities"),
		CL_EQUITY => _("Equity"),
		CL_INCOME => _("Income"),
		CL_COGS => _("Cost of Goods Sold"),
		CL_EXPENSE => _("Expense"),
);

function get_class_type_convert($ctype)
{
	global $use_oldstyle_convert;
	if (isset($use_oldstyle_convert) && $use_oldstyle_convert == 1)
		return (($ctype >= CL_INCOME || $ctype == CL_NONE) ? -1 : 1);
	else	
		return ((($ctype >= CL_LIABILITIES && $ctype <= CL_INCOME) || $ctype == CL_NONE) ? -1 : 1);
}
//----------------------------------------------------------------------------------
//	Quick entry types
//
$quick_actions = array(
	'=' => _('Remainder'), // post current base amount to GL account
	'a' => _('Amount'), // post amount to GL account
	'a+' => _('Amount, increase base'), // post amount to GL account and increase base
	'a-' => _('Amount, reduce base'), // post amount to GL account and reduce base
	'%' => _('% amount of base'),	// store acc*amount% to GL account
	'%+' => _('% amount of base, increase base'),	// ditto & increase base amount
	'%-' => _('% amount of base, reduce base'),	// ditto & reduce base amount
	'T' => _('Taxes added'), // post taxes calculated on base amount
	'T+' => _('Taxes added, increase base'), // ditto & increase base amount
	'T-' => _('Taxes added, reduce base'), // ditto & reduce base amount
	't' => _('Taxes included'), // post taxes calculated on base amount
	't+' => _('Taxes included, increase base'), // ditto & increase base amount
	't-' => _('Taxes included, reduce base') // ditto & reduce base amount
);

define('QE_PAYMENT', '1');
define('QE_DEPOSIT', '2');
define('QE_JOURNAL', '3');
define('QE_SUPPINV', '4');

$quick_entry_types = array(
		QE_DEPOSIT => _("Bank Deposit"),
		QE_PAYMENT => _("Bank Payment"),
		QE_JOURNAL => _("Journal Entry"),
		QE_SUPPINV => _("Supplier Invoice/Credit")
);

//----------------------------------------------------------------------------------
//	Special option values for various list selectors.
//
define('ANY_TEXT', '');
define('ANY_NUMERIC', -1);
define('ALL_TEXT', '');
define('ALL_NUMERIC', -1);


//----------------------------------------------------------------------------------
// Types of stock items
$stock_types = array(
		'M' => _("Manufactured"),
		'B' => _("Purchased"),
		'D' => _("Service")
);

//----------------------------------------------------------------------------------

define('TAG_ACCOUNT',   1);
define('TAG_DIMENSION', 2);

$tag_types = array (
		TAG_ACCOUNT   => _("Account"),
		TAG_DIMENSION => _("Dimension")
);

?>