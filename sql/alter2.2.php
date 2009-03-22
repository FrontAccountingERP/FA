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
class fa2_2 {
	var $version = '2.2';	// version installed
	var $description = 'Version 2.2';
	var $sql = 'alter2.2.sql';
	//
	//	Install procedure. All additional changes 
	//	not included in sql file should go here.
	//
	function install($pref, $force) 
	{
		global $db;
		// set item category dflt accounts to values from company GL setup
		$prefs = get_company_prefs();
		$sql = "UPDATE {$pref}stock_category SET "
			."dflt_sales_act = '" . $prefs['default_inv_sales_act'] . "',"
			."dflt_cogs_act = '". $prefs['default_cogs_act'] . "',"
			."dflt_inventory_act = '" . $prefs['default_inventory_act'] . "',"
			."dflt_adjustment_act = '" . $prefs['default_adj_act'] . "',"
			."dflt_assembly_act = '" . $prefs['default_assembly_act']."'";
		$ret = db_query($sql, "Cannot update category default GL accounts");
		return $ret;
	}
	//
	//	Checking before install
	//
	function pre_check($pref)
	{
		return true; // true when ok, fail otherwise
	}
	//
	//	Test if patch was applied before.
	//
	function installed($pref) {
		if (check_table($pref, 'company', 'default_delivery_required')) return false;
		if (check_table($pref, 'stock_category', 'dflt_dim2')) return false;
		if (check_table($pref, 'users', 'sticky_doc_date')) return false;
		return true;
	}
};

$install = new fa2_2;
?>