<?php
	
	include_once("./modules/installed_modules.php");
	class customers_app extends application 
	{
		function customers_app() 
		{
			global $installed_modules;
			$this->application("orders",_("Sales"));
		
			$this->add_module(_("Transactions"));
			$this->add_lapp_function(0, _("Sales &Order Entry"),"sales/sales_order_entry.php?NewOrder=Yes");
			$this->add_lapp_function(0, _("Direct &Delivery"),"sales/sales_order_entry.php?NewDelivery=0");			
			$this->add_lapp_function(0, _("Direct &Invoice"),"sales/sales_order_entry.php?NewInvoice=0");
			$this->add_lapp_function(0, "","");
			$this->add_lapp_function(0, _("&Delivery Against Sales Orders"),"sales/inquiry/sales_orders_view.php?OutstandingOnly=1");
			$this->add_lapp_function(0, _("&Invoice Against Sales Delivery"),"sales/inquiry/sales_deliveries_view.php?OutstandingOnly=1");

			$this->add_rapp_function(0, _("&Template Delivery"),"sales/inquiry/sales_orders_view.php?DeliveryTemplates=Yes");
			$this->add_rapp_function(0, _("&Template Invoice"),"sales/inquiry/sales_orders_view.php?InvoiceTemplates=Yes");
			$this->add_rapp_function(0, _("&Create and Print Recurrent Invoices"),"sales/create_recurrent_invoices.php?");
			$this->add_rapp_function(0, "","");
			$this->add_rapp_function(0, _("Customer &Payments"),"sales/customer_payments.php?");
			$this->add_rapp_function(0, _("Customer &Credit Notes"),"sales/credit_note_entry.php?NewCredit=Yes");
			$this->add_rapp_function(0, _("&Allocate Customer Payments or Credit Notes"),"sales/allocations/customer_allocation_main.php?");

			$this->add_module(_("Inquiries and Reports"));
			$this->add_lapp_function(1, _("Sales Order &Inquiry"),"sales/inquiry/sales_orders_view.php?");
			$this->add_lapp_function(1, _("Customer Transaction &Inquiry"),"sales/inquiry/customer_inquiry.php?");
			$this->add_lapp_function(1, "","");
			$this->add_lapp_function(1, _("Customer Allocation &Inquiry"),"sales/inquiry/customer_allocation_inquiry.php?");
			
			$this->add_rapp_function(1, _("Customer and Sales &Reports"),"reporting/reports_main.php?Class=0");
			
			$this->add_module(_("Maintenance"));
			$this->add_lapp_function(2, _("Add and Manage &Customers"),"sales/manage/customers.php?");
			$this->add_lapp_function(2, _("Customer &Branches"),"sales/manage/customer_branches.php?");
			$this->add_lapp_function(2, _("Sales &Groups"),"sales/manage/sales_groups.php?");
			$this->add_lapp_function(2, _("Recurrent &Invoices"),"sales/manage/recurrent_invoices.php?");
			$this->add_rapp_function(2, _("Sales T&ypes"),"sales/manage/sales_types.php?");
			$this->add_rapp_function(2, _("Sales &Persons"),"sales/manage/sales_people.php?");
			$this->add_rapp_function(2, _("Sales &Areas"),"sales/manage/sales_areas.php?");
			$this->add_rapp_function(2, _("Credit &Status Setup"),"sales/manage/credit_status.php?");
			if (count($installed_modules) > 0)
			{
				foreach ($installed_modules as $mod)
				{
					if ($mod["tab"] == "orders")
						$this->add_rapp_function(2, $mod["name"], "modules/".$mod["path"]."/".$mod["filename"]."?");
				}
			}	
		}
	}
	

?>