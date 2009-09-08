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
	
	if (!($ret = db_query("SELECT MAX(`order_no`) FROM `{$pref}sales_orders`")) ||
		!db_num_rows($ret))
	{
		display_error(_('Cannot query max sales order number.'));
		return false;
	} 
	$row = db_fetch($ret);
	$max_order = $row[0];
	$next_ref = $max_order+1;
	$sql = "UPDATE `{$pref}sys_types` 
		SET `type_no`='$max_order', 
			`next_reference`='$next_ref'
		WHERE `type_id`=30";
	if(!db_query($sql))
	{
		display_error(_('Cannot store next sales order reference.'));
		return false;
	}

	return convert_roles($pref);
	}
	//
	//	Checking before install
	//
	function pre_check($pref)
	{
		global $security_groups;
		return isset($security_groups); // true when ok, fail otherwise
	}
	//
	//	Test if patch was applied before.
	//
	function installed($pref) {
		$n = 14; // number of features to be installed
		if (check_table($pref, 'company', 'custom1_name')) $n--;
		if (!check_table($pref, 'company', 'profit_loss_year_act')) $n--;
		if (!check_table($pref, 'company', 'login_tout')) $n--;
		if (!check_table($pref, 'stock_category', 'dflt_no_sale')) $n--;
		if (!check_table($pref, 'users', 'sticky_doc_date')) $n--;
		if (!check_table($pref, 'users', 'startup_tab')) $n--;
		if (!check_table($pref, 'cust_branch', 'inactive')) $n--;
		if (!check_table($pref, 'chart_class', 'ctype')) $n--;
		if (!check_table($pref, 'audit_trail')) $n--;
		if (!check_table($pref, 'currencies', 'auto_update')) $n--;
		if (!check_table($pref, 'stock_master','no_sale')) $n--;
		if (!check_table($pref, 'suppliers', 'supp_ref')) $n--;
		if (!check_table($pref, 'users', 'role_id')) $n--;
		if (!check_table($pref, 'sales_orders', 'reference')) $n--;
		return $n == 0 ? true : 14 - $n;
	}
};

/*
	Conversion of old security roles stored into $security_groups table
*/
function convert_roles($pref) 
{
		global $security_groups, $security_headings, $security_areas, $path_to_root;
		include_once($path_to_root."/includes/access_levels.inc");

	$trans_sec = array(
		1 => array('SA_CHGPASSWD', 'SA_SETUPDISPLAY', 'SA_BANKTRANSVIEW',
			'SA_ITEMSTRANSVIEW','SA_SUPPTRANSVIEW', 'SA_SALESORDER',
			'SA_SALESALLOC', 'SA_SALESTRANSVIEW'),
		2 => array('SA_DIMTRANSVIEW', 'SA_STANDARDCOST', 'SA_ITEMSTRANSVIEW',
			'SA_ITEMSSTATVIEW', 'SA_SALESPRICE', 'SA_MANUFTRANSVIEW',
			'SA_WORKORDERANALYTIC', 'SA_WORKORDERCOST', 'SA_SUPPTRANSVIEW',
			'SA_SUPPLIERALLOC', 'SA_STEMPLATE', 'SA_SALESTRANSVIEW',
			'SA_SALESINVOICE', 'SA_SALESDELIVERY', 'SA_CUSTPAYMREP',
			'SA_CUSTBULKREP', 'SA_PRICEREP', 'SA_SALESBULKREP', 'SA_SALESMANREP',
			'SA_SALESBULKREP', 'SA_CUSTSTATREP', 'SA_SUPPLIERANALYTIC',
			'SA_SUPPPAYMREP', 'SA_SUPPBULKREP', 'SA_ITEMSVALREP', 'SA_ITEMSANALYTIC',
			'SA_BOMREP', 'SA_MANUFBULKREP', 'SA_DIMENSIONREP', 'SA_BANKREP', 'SA_GLREP',
			'SA_GLANALYTIC', 'SA_TAXREP', 'SA_SALESANALYTIC'),
		3 => array('SA_GLACCOUNTGROUP', 'SA_GLACCOUNTCLASS','SA_PAYMENT', 
			'SA_DEPOSIT', 'SA_JOURNALENTRY', 'SA_INVENTORYMOVETYPE',
			'SA_LOCATIONTRANSFER', 'SA_INVENTORYADJUSTMENT', 'SA_WORKCENTRES',
			'SA_MANUFISSUE', 'SA_SUPPLIERALLOC', 'SA_CUSTOMER', 'SA_CRSTATUS',
			'SA_SALESMAN', 'SA_SALESAREA', 'SA_SALESALLOC', 'SA_SALESCREDITINV',
			'SA_SALESPAYMNT', 'SA_SALESCREDIT', 'SA_SALESGROUP', 'SA_SRECURRENT',
			'SA_TAXRATES', 'SA_ITEMTAXTYPE', 'SA_TAXGROUPS', 'SA_QUICKENTRY'),
		4 => array('SA_REORDER', 'SA_PURCHASEPRICING', 'SA_PURCHASEORDER'),
		5 => array('SA_VIEWPRINTTRANSACTION', 'SA_BANKTRANSFER', 'SA_SUPPLIER',
			'SA_SUPPLIERINVOICE', 'SA_SUPPLIERPAYMNT', 'SA_SUPPLIERCREDIT'),
		8 => array('SA_ATTACHDOCUMENT', 'SA_RECONCILE',	'SA_GLANALYTIC',
			'SA_TAXREP', 'SA_BANKTRANSVIEW', 'SA_GLTRANSVIEW'),
		9 => array('SA_FISCALYEARS', 'SA_CURRENCY', 'SA_EXCHANGERATE', 
			'SA_BOM'),
		10 => array('SA_PAYTERMS', 'SA_GLSETUP', 'SA_SETUPCOMPANY',
			'SA_FORMSETUP', 'SA_DIMTRANSVIEW', 'SA_DIMENSION', 'SA_BANKACCOUNT',
			'SA_GLACCOUNT', 'SA_BUDGETENTRY', 'SA_MANUFRECEIVE',
			'SA_MANUFRELEASE', 'SA_WORKORDERENTRY', 'SA_MANUFTRANSVIEW',
			'SA_WORKORDERCOST'),
		11 => array('SA_ITEMCATEGORY', 'SA_ITEM', 'SA_UOM', 'SA_INVENTORYLOCATION',
			 'SA_GRN', 'SA_FORITEMCODE', 'SA_SALESKIT'),
		14 => array('SA_SHIPPING', 'SA_VOIDTRANSACTION', 'SA_SALESTYPES'),
		15 => array('SA_PRINTERS', 'SA_PRINTPROFILE', 'SA_BACKUP', 'SA_USERS',
			'SA_POSSETUP'),
		20 => array('SA_CREATECOMPANY', 'SA_CREATELANGUAGE', 'SA_CREATEMODULES',
			'SA_SOFTWAREUPGRADE', 'SA_SECROLES')
		);
		$new_ids = array();
		foreach ($security_groups as $role_id => $areas) {
			$area_set = array();
			$sections = array();
			foreach ($areas as $a) {
			 if (isset($trans_sec[$a]))
				foreach ($trans_sec[$a] as $id) {
				 if ($security_areas[$id][0] != 0)
//				 	error_log('invalid area id: '.$a.':'.$id);
					$area_set[] = $security_areas[$id][0];
					$sections[$security_areas[$id][0]&~0xff] = 1;
				}
			}
			$sections  = array_keys($sections);
			sort($sections); sort($area_set);
			import_security_role($pref, $security_headings[$role_id], $sections, $area_set);
			$new_ids[$role_id] = db_insert_id();
		}
		$result = get_users(true);
		$users = array();
		while($row = db_fetch($result)) { // complete old user ids and roles
			$users[$row['role_id']][] = $row['id'];
		}
		foreach($users as $old_id => $uids)
			foreach( $uids as $id) {
				$sql = "UPDATE {$pref}users set role_id=".$new_ids[$old_id].
					" WHERE id=$id";
				$ret = db_query($sql, 'cannot update users roles');
				if(!$ret) return false;
			}
		return true;
}

function import_security_role($pref, $name, $sections, $areas)
{
	$sql = "INSERT INTO {$pref}security_roles (role, description, sections, areas)
	VALUES (".db_escape('FA 2.1 '.$name).",".db_escape($name).","
	.db_escape(implode(';',$sections)).",".db_escape(implode(';',$areas)).")";

	db_query($sql, "could not add new security role");
}

$install = new fa2_2;

?>