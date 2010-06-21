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

class fa2_3 {
	var $version = '2.3';	// version installed
	var $description;
	var $sql = 'alter2.3.sql';
	var $preconf = true;
	
	function fa2_3() {
		$this->description = _('Upgrade from version 2.2 to 2.3');
	}
	
	//
	//	Install procedure. All additional changes 
	//	not included in sql file should go here.
	//
	function install($pref, $force) 
	{
		$sql = "SELECT debtor_no, payment_terms FROM {$pref}debtors_master";
		
		$result = db_query($sql);
		if (!$result) {
			display_error("Cannot read customers"
			.':<br>'. db_error_msg($db));
			return false;
		}
		// update all sales orders and transactions with customer std payment terms
		while($cust = db_fetch($result)) {
			$sql = "UPDATE {$pref}debtor_trans SET "
				."payment_terms = '" .$cust['payment_terms']
				."' WHERE debtor_no='".$cust['debtor_no']."'";
			if (db_query($sql)==false) {
				display_error("Cannot update cust trans payment"
				.':<br>'. db_error_msg($db));
				return false;
			}
			$sql = "UPDATE {$pref}sales_orders SET "
				."payment_terms = '" .$cust['payment_terms']
				."' WHERE debtor_no='".$cust['debtor_no']."'";
			if (db_query($sql)==false) {
				display_error("Cannot update sales order payment"
				.':<br>'. db_error_msg($db));
				return false;
			}
		}
		//remove obsolete and temporary columns.
		// this have to be done here as db_import rearranges alter query order
		$dropcol = array(
			'crm_persons' => array('tmp_id','tmp_class'),
			'debtors_master' => array('email'),
			'cust_branch' => array('phone', 'phone2', 'fax', 'email'),
			'suppliers' => array('phone', 'phone2', 'fax', 'email')
		);

		foreach($dropcol as $table => $columns)
			foreach($columns as $col) {
			if (db_query("ALTER TABLE `{$pref}{$table}` DROP `$col`")==false) {
				display_error("Cannot drop {$table}.{$col} column:<br>".db_error_msg($db));
				return false;
			}
		}
		if (!update_totals_2_3($pref)) {
			display_error("Cannot update order totals");
			return false;
		}
		// remove old preferences table after upgrade script has been executed
		$sql = "DROP TABLE IF EXISTS `{$pref}company`";

		return db_query($sql) && update_company_prefs(array('version_id'=>'2.3'), $pref);
	}
	//
	//	Checking before install
	//
	function pre_check($pref, $force)
	{
		return true;
	}
	//
	//	Test if patch was applied before.
	//
	function installed($pref) {
		$n = 3; // number of patches to be installed
		$patchcnt = 0;

		if (!check_table($pref, 'comments', 'type', array('Key'=>'MUL'))) $patchcnt++;
		if (!check_table($pref, 'sys_prefs')) $patchcnt++;
		if (!check_table($pref, 'sales_orders', 'payment_terms')) $patchcnt++;

		$n -= $patchcnt;
		return $n == 0 ? true : $patchcnt;
	}
}
/*
	Update order totals
*/
function update_totals_2_3($pref)
{
	global $path_to_root;
	
	include_once("$path_to_root/sales/includes/cart_class.inc");
	include_once("$path_to_root/purchasing/includes/po_class.inc");
	$cart = new cart(ST_SALESORDER);
	$sql = "SELECT order_no FROM {$pref}sales_orders";
	$orders = db_query($sql);
	while ($order_no = db_fetch($orders)) {
		read_sales_order($order_no[0], $cart, ST_SALESORDER);
		update_sales_order($cart);
		unset($cart->line_items);
	}
	unset($cart);
	$cart = new purch_order();
	$sql = "SELECT order_no FROM {$pref}purch_orders";
	$orders = db_query($sql);
	while ($order_no = db_fetch($orders)) {
		read_po($order_no[0], $cart);
		update_po($cart);
		unset($cart->line_items);
	}
}

$install = new fa2_3;

?>