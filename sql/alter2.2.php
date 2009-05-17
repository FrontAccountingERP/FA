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
		global $db, $systypes_array;
		// set item category dflt accounts to values from company GL setup
		$prefs = get_company_prefs();
		$sql = "UPDATE {$pref}stock_category SET "
			."dflt_sales_act = '" . $prefs['default_inv_sales_act'] . "',"
			."dflt_cogs_act = '". $prefs['default_cogs_act'] . "',"
			."dflt_inventory_act = '" . $prefs['default_inventory_act'] . "',"
			."dflt_adjustment_act = '" . $prefs['default_adj_act'] . "',"
			."dflt_assembly_act = '" . $prefs['default_assembly_act']."'";
		if (db_query($sql)==false) {
			display_error("Cannot update category default GL accounts"
			.':<br>'. db_error_msg($db));
			return false;
		}
		// add all references to refs table for easy searching via journal interface
		foreach($systypes_array as $typeno => $typename) {
			$info = get_systype_db_info($typeno);
			if ($info == null || $info[3] == null) continue;
			$tbl = str_replace(TB_PREF, $pref, $info[0]);
			$sql = "SELECT {$info[2]} as id,{$info[3]} as ref FROM $tbl";
			if ($info[1])
				$sql .= " WHERE {$info[1]}=$typeno";
			$result = db_query($sql);
			if (db_num_rows($result)) {
				while ($row = db_fetch($result)) {
					$res2 = db_query("INSERT INTO {$pref}refs VALUES("
						. $row['id'].",".$typeno.",'".$row['ref']."')");
					if (!$res2) {
						display_error(_("Cannot copy references from $tbl")
							.':<br>'. db_error_msg($db));
						return false;
					}
				}
			}
		}
/* FIX		// restore/init audit_trail data 
		$datatbl = array (
			"gl_trans"=> array("type", "type_no","tran_date"),
			"purch_orders" => array("order_no", "'18'", "ord_date"), 
			"sales_orders" => array("order_no", "'30'", "ord_date"),
			"workorders" => array("id", "'26'", "date_") );
		foreach ( $datatbl as $tblname => $tbl) {
		  $sql = "SELECT {$tbl[0]} as type, {$tbl[1]} as trans, {$tbl[2]} as dat"
		  	. " FROM {$pref}{$tblname}";
		  $result = db_query($sql);
		  if (db_num_rows($result)) {
		  	$user = ;
			$year = ;
			while ($row = db_fetch($result)) {
				$sql2 = "INSERT INTO ".$pref."audit_trail"
				." (type, trans_no, user, fiscal_year, gl_date, gl_seq) VALUES ("
				. "{$row['type']},{$row['trans']},$user,$year,{$row['dat']},0)";
				$res2 = db_query($sql2);
				if (!$res2) {
					display_error(_("Cannot init audit_trail data")
						.':<br>'. db_error_msg($db));
					return false;
				}
			}
		  }
		}
*/		
		return true;
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
		if (check_table($pref, 'audit_trail')) return false;
		if (check_table($pref, 'stock_master','no_sale')) return false;
			return true;
	}
};

$install = new fa2_2;
?>