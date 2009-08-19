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
//
/*
	This table is introduced only for additional grouping of sales areas,
	and should be consistent with convention used above.
*/

define('SM_SALES', 1<<8);
define('SM_PURCH', 2<<8);
define('SM_ITEMS', 3<<8);
define('SM_MANUF', 4<<8);
define('SM_DIM', 5<<8);
define('SM_GL', 6<<8);
define('SM_SETUP', 7<<8);

$security_modules = array(
 SM_SALES => _("Sales"),
 SM_PURCH => _("Purchases"),
 SM_ITEMS => _("Items and Inventory"),
 SM_MANUF => _("Manufacturing"),
 SM_DIM => _("Dimensions"),
 SM_GL => _("Banking & General Ledger"),
 SM_SETUP => _("Setup")
);

//	Access areas used in FrontAccounting. 
// 	Constants defined below should be used wherever access rights for current
//	user are checked. Set of allowed access areas is retrieved during login from
//	security_roles table using role granted to user as a key, and stored 
//	in user profile for use during login session.
//	
//----------------------------------------------------------------------------------
// Sales module
//
define('SA_CUSTOMER',	SM_SALES|1);
define('SA_SALESGROUP',	SM_SALES|2);
define('SA_PRICE',		SM_SALES|3);
define('SA_SALESMAN', 	SM_SALES|4);
define('SA_SALESAREA', 	SM_SALES|5);
define('SA_STATUS', 	SM_SALES|6);
define('SA_STEMPLATE', 	SM_SALES|7);
define('SA_SRECURRENT', SM_SALES|8);

define('SA_SALESORDER', 	SM_SALES|11);
define('SA_SALESDELIVERY', 	SM_SALES|12);
define('SA_SALESINVOICE', 	SM_SALES|13);
define('SA_SALESCREDITINV', SM_SALES|14);
define('SA_SALESCREDIT', 	SM_SALES|15);
define('SA_SALESPAYMNT', 	SM_SALES|16);

define('SA_SALESALLOC', SM_SALES|21);
define('SA_SALESANALYTIC', SM_SALES|22);
define('SA_SALESMANREP', SM_SALES|23);
define('SA_SALESVARREP', SM_SALES|24);

//
// Purchasing module 
//
define('SA_SUPPLIER', SM_PURCH|1);

define('SA_PURCHASEORDER', SM_PURCH|11);
define('SA_GRN', SM_PURCH|12);
define('SA_SUPPLIERINVOICE', SM_PURCH|13);
define('SA_SUPPLIERCREDIT', SM_PURCH|14);
define('SA_SUPPLIERPAYMNT', SM_PURCH|15);

define('SA_SUPPLIERALLOC', SM_PURCH|21);
define('SA_SUPPLIERANALYTIC', SM_PURCH|22);
define('SA_SUPPLIERMANREP', SM_PURCH|23);
define('SA_SUPPLIERVARREP', SM_PURCH|24);

//
// Inventory module 
//
define('SA_ITEM', SM_ITEMS|1);
define('SA_FORITEMCODE', SM_ITEMS|2);
define('SA_SALESKIT', SM_ITEMS|3);
define('SA_ITEMCATEGORY', SM_ITEMS|4);
define('SA_INVENTORYLOCATION', SM_ITEMS|5);
define('SA_INVENTORYMOVETYPE', SM_ITEMS|6);
define('SA_ITEMTAXTYPE', SM_ITEMS|7);
define('SA_UOM', SM_ITEMS|8);
define('SA_REORDER', SM_ITEMS|9);

define('SA_LOCATIONTRANSFER', SM_ITEMS|11);
define('SA_INVENTORYADJUSTMENT', SM_ITEMS|12);

define('SA_ITEMSANALYTIC', SM_ITEMS|21);
define('SA_ITEMSMANREP', SM_ITEMS|22);
define('SA_ITEMSVARREP', SM_ITEMS|23);

define('SA_SALESPRICING', SM_ITEMS|31);
define('SA_PURCHASEPRICING', SM_ITEMS|32);
define('SA_STANDARDCOST', SM_ITEMS|33);

//
// Manufacturing module 
//
define('SA_BOM', SM_MANUF|1);
define('SA_WORKCENTRE', SM_MANUF|2);

define('SA_WORKORDERENTRY', SM_MANUF|11);
define('SA_WORKORDEROPERATION', SM_MANUF|12);

define('SA_WORKORDERANALYTIC', SM_MANUF|21);
define('SA_WORKORDERMANREP', SM_MANUF|22);
define('SA_WORKORDERVARREP', SM_MANUF|23);

//
// Dimension module 
//
define('SA_DIMENSION', SM_DIM|1);

define('SA_DIMENSIONOPERATION', SM_DIM|11);

define('SA_DIMENSIONANALYTIC', SM_DIM|21);
define('SA_DIMENSIONMANREP', SM_DIM|22);
define('SA_DIMENSIONVARREP', SM_DIM|23);

//
// Banking and General Ledger module 
//
define('SA_BANKACCOUNT', SM_GL|1);
define('SA_QUICKENTRY', SM_GL|2);
define('SA_CURRENCY', SM_GL|3);
define('SA_EXCHANGERATE', SM_GL|4);
define('SA_GLACCOUNT', SM_GL|5);
define('SA_GLACCOUNTGROUP', SM_GL|6);
define('SA_GLACCOUNTCLASS', SM_GL|7);

define('SA_PAYMENT', SM_GL|11);
define('SA_DEPOSIT', SM_GL|12);
define('SA_BANKACCOUNTTRANSFER', SM_GL|13);
define('SA_JOURNALENTRY', SM_GL|14);
define('SA_BUDGETENTRY', SM_GL|15);
define('SA_RECONCILE', SM_GL|16);

define('SA_GLANALYTIC', SM_GL|21);
define('SA_GLMANREP', SM_GL|22);
define('SA_GLVARREP', SM_GL|23);

//
// Setup module 
//
define('SA_SETUPCOMPANY', SM_SETUP|1);
define('SA_SETUPUSER', SM_SETUP|2);
define('SA_SETUPFORM', SM_SETUP|3);
define('SA_SETUPTAX', SM_SETUP|4);
define('SA_SETUPFISCALYEAR', SM_SETUP|5);
define('SA_PRINTPROFILE', SM_SETUP|6);
define('SA_PAYMENTTERM', SM_SETUP|7);
define('SA_SHIPPING', SM_SETUP|8);
define('SA_SETUPPOS', SM_SETUP|9);
define('SA_SETUPPRINTER', SM_SETUP|10);

define('SA_VOIDTRANSACTION', SM_SETUP|11);
define('SA_VIEWPRINTTRANSACTION', SM_SETUP|12);
define('SA_ATTACHDOCUMENT', SM_SETUP|13);
define('SA_BACKUP', SM_SETUP|14);
define('SA_CREATECOMPANY', SM_SETUP|15);
define('SA_CREATELANGUAGE', SM_SETUP|16);
define('SA_CREATEMODULES', SM_SETUP|17);
define('SA_SOFTWAREUPGRADE', SM_SETUP|18);

/*
	This table stores security area ranges. It is used by security_role 
	table administration interface. Every extension module can add their
	own security areas set by adding new entries here.
*/
$security_areas = array(
	// database table crud operations and administrative areas
	SA_CUSTOMER => _("Sales customer and branches"),
		_("Sales groups"),
		_("Sales prices"),
		_("Sales staff"),
		_("Sales areas"),
		_("Credit status definitions"),
		_("Sales templates"),
		_("Recurrent invoices definitions"),
	// documents, transactions entry
	SA_SALESORDER => _("Sales order entry"),
		_("Sales deliveries"),
		_("Sales invoices"),
		_("Sales credit notes against invoice"),
		_("Sales freehand credit notes"),
		_("Customer payments"),
	// maintenance and analytics
	SA_SALESALLOC => _("Customer allocations"),
		_("Customer analytical reports and inquiries"),
		_("Sales reports"),
		_("Sales other reports and inquires"),
	// database table crud operations and administrative areas
	SA_SUPPLIER => _("Suppliers"),
	// documents, transactions entry
	SA_PURCHASEORDER => _("Purchase Order Entry"),
		_("Outstanding Purchase Orders"),
		_("Supplier Invoices"),
		_("Supplier Credit Notes"),
		_("Supplier Payments"),
	// maintenance and analytics
	SA_SUPPLIERALLOC => _("Supplier allocations"),
		_("Supplier analytical reports and inquiries"),
		_("Supplier reports"),
		_("Supplier other reports and inquiries"),
	// database table crud operations and administrative areas
	SA_ITEM => _("Items"),
		_("Foreign Item Codes"),
		_("Sales Kits"),
		_("Item Categories"),
		_("Inventory Locations"),
		_("Inventory Movement Types"),
		_("Item Tax Types"),
		_("Unit of Measures"),
		_("Reorder Levels"),
	// documents, transactions entry
	SA_LOCATIONTRANSFER => _("Inventory Location Transfers"),
		_("Inventory Adjustments"),
	// maintenance and analytics
	SA_ITEMSANALYTIC => _("Items analytical reports and inquiries"),
		_("Inventory reports"),
		_("Inventory other reports and inquiries"),
	SA_SALESPRICING => _("Item Sales Pricing"),
		_("Item Purchasing Pricing"),
		_("Item Standard Costs"),
	// database table crud operations and administrative areas
	SA_BOM => _("Bill of Materials"),
		_("Work Centres"),
	// documents, transactions entry
	SA_WORKORDERENTRY => _("Work Order Entries"),
		_("Work Order Operations"),
	// maintenance and analytics
	SA_WORKORDERANALYTIC => _("Work Order analytical reports and inquiries"),
		_("Work Order Reports"),
		_("Work Order other reports and inquiries"),
	// database table crud operations and administrative areas
	SA_DIMENSION => _("Dimensions"),
	// documents, transactions entry
	SA_DIMENSIONOPERATION => _("Dimension Operations"),
	// maintenance and analytics
	SA_DIMENSIONANALYTIC => _("Dimension analytical reports and inquiries"),
		_("Dimension Reports"),
		_("Dimension other reports and inquiries"),
	// database table crud operations and administrative areas
	SA_BANKACCOUNT => _("Bank Accounts"),
		_("Quick Entries"),
		_("Currencies"),
		_("Exchange Rates"),
		_("GL Accounts"),
		_("GL Account Groups"),
		_("GL Account Classes"),
	// documents, transactions entry
	SA_PAYMENT => _("Bank Payments"),
		_("Bank Deposits"),
		_("Bank Account Transfers"),
		_("Journal Entries"),
		_("Budget Entries"),
		_("Bank Reconcile"),
	// maintenance and analytics
	SA_GLANALYTIC => _("Bank and GL analytical reports and inquiries"),
		_("Bank and GL Reports"),
		_("Bank and GL other reports and inquiries"),
	// database table crud operations and administrative areas
	SA_SETUPCOMPANY => _("Company Setup"),
		_("Users Setup"),
		_("Forms Setup"),
		_("Taxes"),
		_("Fiscal Years"),
		_("Print Profiles"),
		_("Payment Terms"),
		_("Shipping"),
		_("Point of Sales"),
		_("Printer Setup"),
	// documents, transactions entry
	SA_VOIDTRANSACTION => _("Voiding Transactions"),
		_("Viev and Print Transactions"),
		_("Attaching Documents"),
		_("Backup and Restore"),
		_("Install/Update Companies"),
		_("Install/Update Languages"),
		_("Install/Upgrade Modules"),
		_("Software Upgrades")
);
?>