<?php
	
	class suppliers_app extends application 
	{
		function suppliers_app() 
		{
			$this->application("AP",_("Purchases"));
			
			$this->add_module(_("Transactions"));
			$this->add_lapp_function(0, _("Purchase Order Entry"),"purchasing/po_entry_items.php?NewOrder=Yes");
			$this->add_lapp_function(0, _("Outstanding Purchase Orders Maintenance"),"purchasing/inquiry/po_search.php?");
			$this->add_rapp_function(0, _("Payments to Suppliers"),"purchasing/supplier_payment.php?");
			$this->add_rapp_function(0, "","");
			$this->add_rapp_function(0, _("Supplier Invoices"),"purchasing/supplier_invoice.php?New=1");			
			$this->add_rapp_function(0, _("Supplier Credit Notes"),"purchasing/supplier_credit.php?New=1");
			$this->add_rapp_function(0, _("Allocate Supplier Payments or Credit Notes"),"purchasing/allocations/supplier_allocation_main.php?");
			
			$this->add_module(_("Inquiries and Reports"));
			$this->add_lapp_function(1, _("Purchase Orders Inquiry"),"purchasing/inquiry/po_search_completed.php?");
			$this->add_lapp_function(1, _("Supplier Transaction Inquiry"),"purchasing/inquiry/supplier_inquiry.php?");
			$this->add_lapp_function(1, "","");
			$this->add_lapp_function(1, _("Supplier Allocation Inquiry"),"purchasing/inquiry/supplier_allocation_inquiry.php?");
			
			$this->add_rapp_function(1, _("Supplier and Purchasing Reports"),"reporting/reports_main.php?Class=1");
			
			$this->add_module(_("Maintenance"));
			$this->add_lapp_function(2, _("Suppliers"),"purchasing/manage/suppliers.php?");
		}
	}
	

?>