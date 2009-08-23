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
	Security sections groups various areas on both functionality and privilige levels.
	Often analyti inquires are available only for management, and configuration
	for administration or management staff. This is why we have those three
	sections related to near every FA module.
	
	Every security section can contain up to 256 different areas.
	Section codes 0-99 are reserved for core FA functionalities.
	External modules can extend security roles system by adding rows to security sections and
	security areas using section code >=100.
*/
define('SS_SADMIN',	1<<8);	// site admin
define('SS_SETUP',	2<<8);	// company level setup
define('SS_SPEC',	3<<8);	// special administration

define('SS_SALES_C',11<<8); // configuration
define('SS_SALES',	12<<8);	// transactions
define('SS_SALES_A',13<<8); // analytic functions/reports/inquires

define('SS_PURCH_C',21<<8);
define('SS_PURCH',	22<<8);
define('SS_PURCH_A',23<<8);

define('SS_ITEMS_C',31<<8);
define('SS_ITEMS',	32<<8);
define('SS_ITEMS_A',33<<8);

define('SS_MANUF_C',41<<8);
define('SS_MANUF',	42<<8);
define('SS_MANUF_A',43<<8);

define('SS_DIM_C',	51<<8);
define('SS_DIM',	52<<8);
define('SS_DIM_A',	53<<8);

define('SS_GL_C',	61<<8);
define('SS_GL',		62<<8);
define('SS_GL_A',	63<<8);

$security_sections = array(
 SS_SADMIN => _("System administration"),
 SS_SETUP => _("Company setup"),
 SS_SPEC => _("Special maintenance"),
 SS_SALES_C => _("Sales configuration"),
 SS_SALES => _("Sales transactions"),
 SS_SALES_A => _("Sales analytics"),
 SS_PURCH_C => _("Purchase configuration"),
 SS_PURCH => _("Purchase transactions"),
 SS_PURCH_A => _("Purchase analytics"),
 SS_ITEMS_C => _("Inventory configuration"),
 SS_ITEMS => _("Inventory operations"),
 SS_ITEMS_A => _("Inventory analytics"),
 SS_MANUF_C => _("Manufacturing configuration"),
 SS_MANUF => _("Manufacturing transations"),
 SS_MANUF_A => _("Manufacturing analytics"),
 SS_DIM => _("Dimensions"),
 SS_GL_C => _("Banking & GL configuration"),
 SS_GL => _("Banking & GL transactions"),
 SS_GL_A => _("Banking & GL analytics")
);

/*
	This table stores security areas available in FA. 
	Key is area identifier used to check user rights, values are
	code stored for each role in security_roles table and description used
	in roles editor.

	Set of allowed access areas codes is retrieved during user login from
	security_roles table, and stored in user profile for direct during the session.

*/
$security_areas =array(
//
//	Site administration
//
	'SA_CREATECOMPANY' =>array(SS_SADMIN|1, _("Install/update companies")),
	'SA_CREATELANGUAGE' => array(SS_SADMIN|2, _("Install/update languages")),
	'SA_CREATEMODULES' => array(SS_SADMIN|3, _("Install/upgrade modules")),
	'SA_SOFTWAREUPGRADE' => array(SS_SADMIN|4, _("Software upgrades")),
//
//	Company setup
//
	'SA_CRSTATUS' => array(SS_SETUP|1, _("Credit status definitions changes")),
	'SA_INVENTORYLOCATION' => array(SS_SETUP|2, _("Inventory locations changes")),
	'SA_INVENTORYMOVETYPE'  => array(SS_SETUP|3, _("Inventory movement types")),
	'SA_WORKCENTRE' => array(SS_SETUP|4, _("Manufacture work centres ")),
	'SA_SETUPCOMPANY' => array(SS_SETUP|5, _("Company parameters")),
	'SA_SETUPUSER' => array(SS_SETUP|6, _("Users setup")),
	'SA_SETUPFORM' => array(SS_SETUP|7, _("Forms setup")),
	'SA_PRINTPROFILE' => array(SS_SETUP|8, _("Print profiles")),
	'SA_PAYMENTTERM' => array(SS_SETUP|9, _("Payment terms")),
	'SA_SHIPPING' => array(SS_SETUP|10, _("Shipping ways")),
	'SA_SETUPPOS' => array(SS_SETUP|11, _("Point of sales definitions")),
	'SA_SETUPPRINTER' => array(SS_SETUP|12, _("Printers configuration")),

// special and common functions
	'SA_VIEWPRINTTRANSACTION' => array(SS_SPEC|1, _("Common view/print transactions interface")),
	'SA_ATTACHDOCUMENT' => array(SS_SPEC|2, _("Attaching documents")),
	'SA_VOIDTRANSACTION' => array(SS_SPEC|3, _("Voiding transactions")),
	'SA_BACKUP' => array(SS_SPEC|4, _("Database backup/restore")),

// Sales related functionality
//
	'SA_STEMPLATE' => array(SS_SALES_C|1, _("Sales templates")),
	'SA_SRECURRENT' => array(SS_SALES_C|2, _("Recurrent invoices definitions")),
	'SA_SALESPRICE' => array(SS_SALES_C|3, _("Sales prices edition")),
	'SA_SALESGROUP' => array(SS_SALES_C|4, _("Sales groups changes")),
	'SA_SALESMAN' => array(SS_SALES_C|5, _("Sales staff maintenance")),
	'SA_SALESAREA' => array(SS_SALES_C|6, _("Sales areas maintenance")),

	'SA_CUSTOMER' => array(SS_SALES|1,  _("Sales customer and branches changes")),
	'SA_SALESORDER' => array(SS_SALES|2, _("Sales orders edition")),
	'SA_SALESDELIVERY' => array(SS_SALES|3, _("Sales deliveries edition")),
	'SA_SALESINVOICE' => array(SS_SALES|4, _("Sales invoices edition")),
	'SA_SALESCREDITINV' => array(SS_SALES|5, _("Sales credit notes against invoice")),
	'SA_SALESCREDIT' => array(SS_SALES|6, _("Sales freehand credit notes")),
	'SA_SALESPAYMNT' => array(SS_SALES|7, _("Customer payments entry")),
	'SA_SALESALLOC' => array(SS_SALES|1, _("Customer payments allocation")),

	'SA_SALESANALYTIC' => array(SS_SALES_A|2, _("Customer analytical reports and inquiries")),
	'SA_SALESMANREP' => array(SS_SALES_A|3, _("Sales reports")),
	'SA_SALESVARREP' => array(SS_SALES_A|4, _("Sales other reports and inquires")),

//
// Purchase related functions
//
	'SA_PURCHASEPRICING' => array(SS_PURCH_C|1, _("Purchase price changes")),

	'SA_SUPPLIER' => array(SS_PURCH|1, _("Suppliers data changes")),
	'SA_PURCHASEORDER' => array(SS_PURCH|2, _("Purchase order entry")),
	'SA_GRN' => array(SS_PURCH|3, _("Purchase receive")),
	'SA_SUPPLIERINVOICE' => array(SS_PURCH|4, _("Supplier invoices")),
	'SA_SUPPLIERCREDIT' => array(SS_PURCH|5, _("Supplier credit notes")),
	'SA_SUPPLIERPAYMNT' => array(SS_PURCH|6, _("Supplier payments")),
	'SA_SUPPLIERALLOC' => array(SS_PURCH|7, _("Supplier payments allocations")),

	'SA_SUPPLIERANALYTIC' => array(SS_PURCH_A|1, _("Supplier analytical reports and inquiries")),
	'SA_SUPPLIERMANREP' => array(SS_PURCH_A|2, _("Supplier reports")),
	'SA_SUPPLIERVARREP' => array(SS_PURCH_A|3, _("Supplier other reports and inquiries")),
//
// Inventory 
//
	'SA_ITEM' => array(SS_ITEMS_C|1, _("Stock items add/edit")),
	'SA_SALESKIT' => array(SS_ITEMS_C|2, _("Sales kits")),
	'SA_ITEMCATEGORY' => array(SS_ITEMS_C|3, _("Item categories")),
	'SA_UOM' => array(SS_ITEMS_C|4, _("Units of measure")),

	'SA_FORITEMCODE' => array(SS_ITEMS|1, _("Foreign item codes entry")),
	'SA_LOCATIONTRANSFER' => array(SS_ITEMS|2, _("Inventory location transfers")),
	'SA_INVENTORYADJUSTMENT' => array(SS_ITEMS|3, _("Inventory adjustments")),

	'SA_REORDER' => array(SS_ITEMS_A|1, _("Reorder levels")),
	'SA_ITEMSANALYTIC' => array(SS_ITEMS_A|2, _("Items analytical reports and inquiries")),
	'SA_ITEMSMANREP' => array(SS_ITEMS_A|3, _("Inventory reports")),
	'SA_ITEMSVARREP' => array(SS_ITEMS_A|4, _("Inventory other reports and inquiries")),

//
// Manufacturing module 
//
	'SA_BOM' => array(SS_MANUF_C|1, _("Bill of Materials")),

	'SA_WORKORDERENTRY' => array(SS_MANUF|1, _("Work order entry")),
	'SA_WORKORDEROPERATION' => array(SS_MANUF|2, _("Work order operations")),

	'SA_WORKORDERANALYTIC' => array(SS_MANUF_A|1, _("Work order analytical reports and inquiries")),
	'SA_WORKORDERMANREP' => array(SS_MANUF_A|2, _("Work order reports")),
	'SA_WORKORDERVARREP' => array(SS_MANUF_A|3, _("Work order other reports and inquiries")),

//
// Dimensions
//
	'SA_DIMENSION' => array(SS_DIM|1, _("Dimensions")),

	'SA_DIMENSIONOPERATION' => array(SS_DIM|2, _("Dimension operations")),

	'SA_DIMENSIONANALYTIC' => array(SS_DIM|3, _("Dimension analytical reports and inquiries")),
	'SA_DIMENSIONMANREP' => array(SS_DIM|4, _("Dimension reports")),
	'SA_DIMENSIONVARREP' => array(SS_DIM|5, _("Dimension other reports and inquiries")),
//
// Banking and General Ledger
//
	'SA_ITEMTAXTYPE' => array(SS_GL_C|1, _("Item tax type definitions")),
	'SA_GLACCOUNT' => array(SS_GL_C|2, _("GL accounts edition")),
	'SA_GLACCOUNTGROUP' => array(SS_GL_C|3, _("GL account groups")),
	'SA_GLACCOUNTCLASS' => array(SS_GL_C|4, _("GL account classes")),
	'SA_QUICKENTRY' => array(SS_GL_C|5, _("Quick GL entry definitions")),
	'SA_CURRENCY' => array(SS_GL_C|6, _("Currencies")),
	'SA_BANKACCOUNT' => array(SS_GL_C|7, _("Bank accounts")),
	'SA_SETUPTAX' => array(SS_GL_C|8, _("Tax rates")),
	'SA_SETUPFISCALYEAR' => array(SS_GL_C|9, _("Fiscal years maintenance")),

	'SA_EXCHANGERATE' => array(SS_GL|1, _("Exchange rate table changes")),
	'SA_PAYMENT' => array(SS_GL|2, _("Bank payments")),
	'SA_DEPOSIT' => array(SS_GL|3, _("Bank deposits")),
	'SA_BANKACCOUNTTRANSFER' => array(SS_GL|4, _("Bank account transfers")),
	'SA_JOURNALENTRY' => array(SS_GL|5, _("Manual journal entries")),
	'SA_BUDGETENTRY' => array(SS_GL|6, _("Budget edition")),
	'SA_RECONCILE' => array(SS_GL|7, _("Bank reconciliation")),
	'SA_STANDARDCOST' => array(SS_GL|8, _("Item standard costs")),

	'SA_GLANALYTIC' => array(SS_GL_A|1, _("Bank and GL analytical reports and inquiries")),
	'SA_GLMANREP' => array(SS_GL_A|2, _("Bank and GL reports")),
	'SA_GLVARREP' => array(SS_GL_A|3, _("Bank and GL other reports and inquiries"))
);
?>