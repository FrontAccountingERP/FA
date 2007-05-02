<?php

	include_once("./modules/installed_modules.php");
	class manufacturing_app extends application 
	{
		function manufacturing_app() 
		{
			global $installed_modules;
			$this->application("manuf",_("Manufacturing"));

			$this->add_module(_("Transactions"));
			$this->add_lapp_function(0, _("Work Order Entry"),"manufacturing/work_order_entry.php?");
			$this->add_lapp_function(0, _("Outstanding Work Orders"),"manufacturing/search_work_orders.php?OutstandingOnly=1");

			$this->add_module(_("Inquiries and Reports"));
			//$this->add_lapp_function(1, _("Costed Bill Of Material Inquiry"),"manufacturing/inquiry/bom_cost_inquiry.php?");
			$this->add_lapp_function(1, _("Inventory Item Where Used Inquiry"),"manufacturing/inquiry/where_used_inquiry.php?");
			$this->add_lapp_function(1, _("Work Order Inquiry"),"manufacturing/search_work_orders.php?");
			$this->add_rapp_function(1, _("Manufactoring Reports"),"reporting/reports_main.php?Class=3");

			$this->add_module(_("Maintenance"));
			$this->add_lapp_function(2, _("Bills Of Material"),"manufacturing/manage/bom_edit.php?");
			$this->add_lapp_function(2, _("Work Centres"),"manufacturing/manage/work_centres.php?");
			if (count($installed_modules) > 0)
			{
				foreach ($installed_modules as $mod)
				{
					if ($mod["tab"] == "manuf")
						$this->add_rapp_function(2, $mod["name"], "modules/".$mod["path"]."/".$mod["filename"]."?");
				}
			}	
		}
	}


?>