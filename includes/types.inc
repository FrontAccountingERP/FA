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

$systypes_array = array (
					0=> array ('name' => _("Journal Entry")),
					1=> array ('name' => _("Bank Payment")),
					2=> array ('name' => _("Bank Deposit")),
					4=> array ('name' => _("Funds Transfer")),
                    10=> array ('name' => _("Sales Invoice")),
                    11=> array ('name' => _("Customer Credit Note")),
                    12=> array ('name' => _("Customer Payment")),
                    13=> array ('name' => _("Delivery Note")),
                    16=> array ('name' => _("Location Transfer")),
                    17=> array ('name' => _("Inventory Adjustment")),
                    18=> array ('name' => _("Purchase Order")),
                    20=> array ('name' => _("Supplier Invoice")),
                    21=> array ('name' => _("Supplier Credit Note")),
                    22=> array ('name' => _("Supplier Payment")),
                    25=> array ('name' => _("Purchase Order Delivery")),
                    26=> array ('name' => _("Work Order")),
                    28=> array ('name' => _("Work Order Issue")),
                    29=> array ('name' => _("Work Order Production")),
                    30=> array ('name' => _("Sales Order")),
                    35=> array ('name' => _("Cost Update")),
                    40=> array ('name' => _("Dimension"))
		 			);
class systypes 
{

	function journal_entry() 
	{
		return 0;
	}

	function bank_payment() 
	{
		return 1;
	}

	function bank_deposit() 
	{
		return 2;
	}

	function bank_transfer() 
	{
		return 4;
	}

	function cust_payment() 
	{
		return 12;
	}

	function cust_dispatch() 
	{
		return 13;
	}

	function location_transfer() 
	{
		return 16;
	}

	function inventory_adjustment() 
	{
		return 17;
	}

	function po() 
	{
		return 18;
	}

	function supp_payment() 
	{
		return 22;
	}

	function work_order() 
	{
		return 26;
	}

	function sales_order() 
	{
		return 30;
	}

	function cost_update() 
	{
		return 35;
	}

	function dimension() 
	{
		return 40;
	}

	function name($index) 
	{
		global $systypes_array;
		if ($index < 0)
			return '';
		return $systypes_array[$index]['name'];
	}
}

//----------------------------------------------------------------------------------

$bank_account_types_array = array (
	0=> array ('id' => 0, 'name' => _("Savings Account"), 'ptype' => _("Transfer")),
	1=> array ('id' => 1, 'name' => _("Chequing Account"),'ptype' => _("Cheque")),
	2=> array ('id' => 2, 'name' => _("Credit Account"), 'ptype' => _("Credit")),
	3=> array ('id' => 3, 'name' => _("Cash Account"), 'ptype' => _("Cash"))
	);

class bank_account_types 
{

	function get_all() 
	{
		global $bank_account_types_array;
		return $bank_account_types_array;;
	}

	function name($index) 
	{
		global $bank_account_types_array;
		return $bank_account_types_array[$index]['name'];
	}
	
	function transfer_type($index) 
	{
		global $bank_account_types_array;
		return $bank_account_types_array[$index]['ptype'];
	}
}
//----------------------------------------------------------------------------------

include_once($path_to_root . "/manufacturing/includes/manufacturing_db.inc");
include_once($path_to_root . "/purchasing/includes/purchasing_db.inc");
include_once($path_to_root . "/sales/includes/sales_db.inc");
include_once($path_to_root . "/dimensions/includes/dimensions_db.inc");

$payment_person_types_array = array (
							0=> array ('id' => 0, 'name' => _("Miscellaneous")),
							1=> array ('id' => 1, 'name' => _("Work Order")),
							2=> array ('id' => 2, 'name' => _("Customer")),
							3=> array ('id' => 3, 'name' => _("Supplier")),
							4=> array ('id' => 4, 'name' => _("Quick Entry"))
							);

class payment_person_types 
{

	function get_all() 
	{
		global $payment_person_types_array;
		return $payment_person_types_array;
	}

    function misc()  
    { 
    	return 0; 
    }

    function WorkOrder()  
    { 
    	return 1; 
    }

    function customer()  
    { 
    	return 2; 
    }

    function supplier()  
    { 
    	return 3; 
    }

    function QuickEntry()  
    { 
    	return 4; 
    }

    function dimension()  
    { 
    	return 5; 
    }

    function type_name($type)
    {
    	global $payment_person_types_array;
    	return $payment_person_types_array[$type]['name'];
    }

    function person_name($type, $person_id, $full=true)
    {
    	switch ($type)
    	{
    		case payment_person_types::misc() :
    			return $person_id;
    		case payment_person_types::QuickEntry() :
    			$qe = get_quick_entry($person_id);
    			return ($full?payment_person_types::type_name($type) . " ":"") . $qe["description"];
    		case payment_person_types::WorkOrder() :
    			global $wo_cost_types;
    			return $wo_cost_types[$person_id];
    		case payment_person_types::customer() :
    			return ($full?payment_person_types::type_name($type) . " ":"") . get_customer_name($person_id);
    		case payment_person_types::supplier() :
    			return ($full?payment_person_types::type_name($type) . " ":"") . get_supplier_name($person_id);
    		default :
    			//DisplayDBerror("Invalid type sent to person_name");
    			//return;
    			return '';
    	}
    }

    function person_currency($type, $person_id)
    {
    	switch ($type)
    	{
    		case payment_person_types::misc() :
    		case payment_person_types::QuickEntry() :
    		case payment_person_types::WorkOrder() :
    			return get_company_currency();

    		case payment_person_types::customer() :
    			return get_customer_currency($person_id);

    		case payment_person_types::supplier() :
    			return get_supplier_currency($person_id);

    		default :
    			return get_company_currency();
    	}
    }

    function has_items($type)
    {
    	switch ($type)
    	{
    		case payment_person_types::misc() :
    			return true;
    		case payment_person_types::QuickEntry() :
    			return db_has_quick_entries();
    		case payment_person_types::WorkOrder() : // 070305 changed to open workorders JH
    			return db_has_open_workorders();
    		case payment_person_types::customer() :
    			return db_has_customers();
    		case payment_person_types::supplier() :
    			return db_has_suppliers();
    		default :
    			display_db_error("Invalid type sent to has_items", "");
    			return false;
    	}
    }
}

//----------------------------------------------------------------------------------

$wo_types_array = array (
						0=> array ('id' => 0, 'name' => _("Assemble")),
						1=> array ('id' => 1, 'name' => _("Unassemble")),
						2=> array ('id' => 2, 'name' => _("Advanced Manufacture"))
			 			);

class wo_types 
{

	function assemble() 
	{ 
		return 0; 
	}

	function unassemble() 
	{ 
		return 1; 
	}

	function advanced() 
	{ 
		return 2; 
	}

	function get_all() 
	{
		global $wo_types_array;
		return $wo_types_array;;
	}

	function name($index) 
	{
		global $wo_types_array;
		return $wo_types_array[$index]['name'];
	}
}

define('WO_LABOUR', 0);
define('WO_OVERHEAD', 1);

$wo_cost_types = array(
		WO_LABOUR => _("Labour Cost"),
		WO_OVERHEAD => _("Overhead Cost"),
);

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

?>