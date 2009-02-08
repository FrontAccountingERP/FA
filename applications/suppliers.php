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
	include_once("./modules/installed_modules.php");
	class suppliers_app extends application 
	{
		function suppliers_app() 
		{
			global $installed_modules;
			$this->application("AP",_("Purchases"));
			
			$this->add_module(_("Transactions"));
			$this->add_lapp_function(0, _("Purchase &Order Entry"),"purchasing/po_entry_items.php?NewOrder=Yes");
			$this->add_lapp_function(0, _("&Outstanding Purchase Orders Maintenance"),"purchasing/inquiry/po_search.php?");
			$this->add_rapp_function(0, _("&Payments to Suppliers"),"purchasing/supplier_payment.php?");
			$this->add_rapp_function(0, "","");
			$this->add_rapp_function(0, _("Supplier &Invoices"),"purchasing/supplier_invoice.php?New=1");			
			$this->add_rapp_function(0, _("Supplier &Credit Notes"),"purchasing/supplier_credit.php?New=1");
			$this->add_rapp_function(0, _("&Allocate Supplier Payments or Credit Notes"),"purchasing/allocations/supplier_allocation_main.php?");
			
			$this->add_module(_("Inquiries and Reports"));
			$this->add_lapp_function(1, _("Purchase Orders &Inquiry"),"purchasing/inquiry/po_search_completed.php?");
			$this->add_lapp_function(1, _("Supplier Transaction &Inquiry"),"purchasing/inquiry/supplier_inquiry.php?");
			$this->add_lapp_function(1, "","");
			$this->add_lapp_function(1, _("Supplier Allocation &Inquiry"),"purchasing/inquiry/supplier_allocation_inquiry.php?");
			
			$this->add_rapp_function(1, _("Supplier and Purchasing &Reports"),"reporting/reports_main.php?Class=1");
			
			$this->add_module(_("Maintenance"));
			$this->add_lapp_function(2, _("&Suppliers"),"purchasing/manage/suppliers.php?");
			if (count($installed_modules) > 0)
			{
				foreach ($installed_modules as $mod)
				{
					if ($mod["tab"] == "AP")
						$this->add_rapp_function(2, $mod["name"], "modules/".$mod["path"]."/".$mod["filename"]."?");
				}
			}	
		}
	}
	

?>