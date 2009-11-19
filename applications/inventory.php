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
class inventory_app extends application 
{
	function inventory_app() 
	{
		global $installed_extensions;
		$this->application("stock", _($this->help_context = "&Items and Inventory"));

		$this->add_module(_("Transactions"));
		$this->add_lapp_function(0, _("Inventory Location &Transfers"),
			"inventory/transfers.php?NewTransfer=1", 'SA_LOCATIONTRANSFER');
		$this->add_lapp_function(0, _("Inventory &Adjustments"),
			"inventory/adjustments.php?NewAdjustment=1", 'SA_INVENTORYADJUSTMENT');

		$this->add_module(_("Inquiries and Reports"));
		$this->add_lapp_function(1, _("Inventory Item &Movements"),
			"inventory/inquiry/stock_movements.php?", 'SA_ITEMSTRANSVIEW');
		$this->add_lapp_function(1, _("Inventory Item &Status"),
			"inventory/inquiry/stock_status.php?", 'SA_ITEMSSTATVIEW');
		$this->add_rapp_function(1, _("Inventory &Reports"),
			"reporting/reports_main.php?Class=2", 'SA_ITEMSTRANSVIEW');

		$this->add_module(_("Maintenance"));
		$this->add_lapp_function(2, _("&Items"),
			"inventory/manage/items.php?", 'SA_ITEM');
		$this->add_lapp_function(2, _("&Foreign Item Codes"),
			"inventory/manage/item_codes.php?", 'SA_FORITEMCODE');
		$this->add_lapp_function(2, _("Sales &Kits"),
			"inventory/manage/sales_kits.php?", 'SA_SALESKIT');
		$this->add_lapp_function(2, _("Item &Categories"),
			"inventory/manage/item_categories.php?", 'SA_ITEMCATEGORY');
		$this->add_lapp_function(2, _("Inventory &Locations"),
			"inventory/manage/locations.php?", 'SA_INVENTORYLOCATION');
		$this->add_rapp_function(2, _("Inventory &Movement Types"),
			"inventory/manage/movement_types.php?", 'SA_INVENTORYMOVETYPE');
		$this->add_rapp_function(2, _("&Units of Measure"),
			"inventory/manage/item_units.php?", 'SA_UOM');
		$this->add_rapp_function(2, _("&Reorder Levels"),
			"inventory/reorder_level.php?", 'SA_REORDER');

		$this->add_module(_("Pricing and Costs"));
		$this->add_lapp_function(3, _("Sales &Pricing"),
			"inventory/prices.php?", 'SA_SALESPRICE');
		$this->add_lapp_function(3, _("Purchasing &Pricing"),
			"inventory/purchasing_data.php?", 'SA_PURCHASEPRICING');
		$this->add_rapp_function(3, _("Standard &Costs"),
			"inventory/cost_update.php?", 'SA_STANDARDCOST');
		if (count($installed_extensions) > 0)
		{
			foreach ($installed_extensions as $mod)
			{
				if (@$mod['active'] && $mod['type'] == 'plugin' && $mod["tab"] == "stock")
					$this->add_rapp_function(2, $mod["title"], 
						"modules/".$mod["path"]."/".$mod["filename"]."?",
						isset($mod["access"]) ? $mod["access"] : 'SA_OPEN' );
			}
		}
	}
}


?>