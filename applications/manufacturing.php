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
	class manufacturing_app extends application
	{
		function manufacturing_app()
		{
			global $installed_modules;
			$this->application("manuf",_("Manufacturing"));

			$this->add_module(_("Transactions"));
			$this->add_lapp_function(0, _("Work &Order Entry"),"manufacturing/work_order_entry.php?");
			$this->add_lapp_function(0, _("&Outstanding Work Orders"),"manufacturing/search_work_orders.php?outstanding_only=1");

			$this->add_module(_("Inquiries and Reports"));
			//$this->add_lapp_function(1, _("Costed Bill Of Material Inquiry"),"manufacturing/inquiry/bom_cost_inquiry.php?");
			$this->add_lapp_function(1, _("Inventory Item Where Used &Inquiry"),"manufacturing/inquiry/where_used_inquiry.php?");
			$this->add_lapp_function(1, _("Work Order &Inquiry"),"manufacturing/search_work_orders.php?");
			$this->add_rapp_function(1, _("Manufacturing &Reports"),"reporting/reports_main.php?Class=3");

			$this->add_module(_("Maintenance"));
			$this->add_lapp_function(2, _("&Bills Of Material"),"manufacturing/manage/bom_edit.php?");
			$this->add_lapp_function(2, _("&Work Centres"),"manufacturing/manage/work_centres.php?");
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