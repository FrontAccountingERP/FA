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
//	Access areas used in FrontAccounting. 
// 	Constants defined below should be used wherever access rights for current
//	user are checked. Set of allowed access areas is retrieved during login from
//	security_roles table using role granted to user as a key, and stored 
//	in user profile for use during login session.
//	
//----------------------------------------------------------------------------------
// Sales module
//
define('SA_CUSTOMER', 101);
define('SA_SALESGROUP', 102);
define('SA_PRICE', 103);
define('SA_SALESMAN', 104);
define('SA_SALESAREA', 105);
define('SA_STATUS', 106);
define('SA_STEMPLATE', 107);
define('SA_SRECCURENT', 108);

define('SA_SALESORDER', 111);
define('SA_SALESDELIVERY', 112);
define('SA_SALESINVOICE', 113);
define('SA_SALESCREDITINV', 114);
define('SA_SALESCREDIT', 115);
define('SA_SALESPAYMNT', 116);

define('SA_SALESALLOC', 121);
define('SA_SALESANALYTIC', 122);
define('SA_SALESMANREP', 123);
define('SA_SALESVARREP', 124);

//
// Purchasing module 
//
define('SA_SUPPLIER', 201);

define('SA_PURCHASEORDER', 211);
define('SA_GRN', 212);
define('SA_SUPPLIERINVOICE', 213);
define('SA_SUPPLIERCREDIT', 214);
define('SA_SUPPLIERPAYMNT', 215);

define('SA_SUPPLIERALLOC', 221);
define('SA_SUPPLIERANALYTIC', 222);
define('SA_SUPPLIERMANREP', 223);
define('SA_SUPPLIERVARREP', 224);

//
// Inventory module 
//
define('SA_ITEM', 301);
define('SA_FORITEMCODE', 302);
define('SA_SALESKIT', 303);
define('SA_ITEMCATEGORY', 304);
define('SA_INVENTORYLOCATION', 305);
define('SA_INVENTORYMOVETYPE', 306);
define('SA_ITEMTAXTYPE', 307);
define('SA_UOM', 308);
define('SA_REORDER', 309);

define('SA_LOCATIONTRANSFER', 311);
define('SA_INVENTORYADJUSTMENT', 312);

define('SA_ITEMSANALYTIC', 321);
define('SA_ITEMSMANREP', 322);
define('SA_ITEMSVARREP', 323);

define('SA_SALESPRICING', 331);
define('SA_PURCHASEPRICING', 332);
define('SA_STANDARDCOST', 333);

//
// Manufacturing module 
//
define('SA_BOM', 401);
define('SA_WORKCENTRE', 402);

define('SA_WORKORDERENTRY', 411);
define('SA_WORKORDEROPERATION', 412);

define('SA_WORKORDERANALYTIC', 421);
define('SA_WORKORDERMANREP', 422);
define('SA_WORKORDERVARREP', 423);

//
// Dimension module 
//
define('SA_DIMENSION', 501);

define('SA_DIMENTIONOPERATION', 511);

define('SA_DIMENSIONANALYTIC', 521);
define('SA_DIMENSIONMANREP', 522);
define('SA_DIMENSIONVARREP', 523);

//
// Banking and General Ledger module 
//
define('SA_BANKACCOUNT', 601);
define('SA_QUICKENTRY', 602);
define('SA_CURRENCY', 603);
define('SA_EXCHANGERATE', 604);
define('SA_GLACCOUNT', 605);
define('SA_GLACCOUNTGROUP', 606);
define('SA_GLACCOUNTCLASS', 607);

define('SA_PAYMENT', 611);
define('SA_DEPOSIT', 612);
define('SA_BANKACCOUNTTRANSFER', 613);
define('SA_JOURNALENTRY', 614);
define('SA_BUDGETENTRY', 615);
define('SA_RECONCILE', 616);

define('SA_GLANALYTIC', 621);
define('SA_GLMANREP', 622);
define('SA_GLVARREP', 623);

//
// Setup module 
//
define('SA_SETUPCOMPANY', 701);
define('SA_SETUPUSER', 702);
define('SA_SETUPFORM', 703);
define('SA_SETUPTAX', 704);
define('SA_SETUPFISCALYEAR', 705);
define('SA_PRINTPROFILE', 706);
define('SA_PAYMENTTERM', 707);
define('SA_SHIPPING', 708);
define('SA_SETUPPOS', 709);
define('SA_SETUPPRINTER', 710);

define('SA_VOIDTRANSACTION', 711);
define('SA_VIEWPRINTTRANSACTION', 712);
define('SA_ATTACHDOCUMENT', 713);
define('SA_BACKUP', 714);
define('SA_CREATECOMPANY', 715);
define('SA_CREATELANGUAGE', 716);
define('SA_CREATEMODULES', 717);
define('SA_SORTWAREUPGRADE', 718);

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
	SA_DIMENSiONOPERATION => _("Dimension Operations"),
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
		_("Software Upgrades"),
);
?>