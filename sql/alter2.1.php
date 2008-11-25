<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU Affero General Public License,
	AGPL, as published by the Free Software Foundation, either version 
	3 of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/agpl-3.0.html>.
***********************************************************************/
class fa2_1 {
	var $version = '2.1';	// version installed
	var $description = 'Version 2.1';
	var $sql = 'alter2.1.sql';
	//
	//	Install procedure. All additional changes 
	//	not included in sql file should go here.
	//
	function install($pref, $force) 
	{
		global $db;
	/*
	Statement below is allowed only for MySQL >=4.0.4:
	UPDATE `0_bank_trans`, `0_bank_accounts` 
		SET 0_bank_trans.bank_act=0_bank_accounts.id 
		WHERE 0_bank_trans.bank_act=0_bank_accounts.account_code;
	*/
		$sql = "SELECT id, account_code FROM ".$pref."bank_accounts";
		if(!($res = db_query($sql))) {
			display_error(_("Cannot retrieve bank accounts codes")
				.':<br>'. db_error_msg($db));
			return false;
		}
		while ($acc = db_fetch($res)) {
			$sql = "UPDATE ".$pref."bank_trans SET bank_act='"
				.$acc['id']."' WHERE bank_act=".$acc['account_code'];
			if (db_query($sql)==false) {
			display_error(_("Cannot update bank transactions")
				.':<br>'. db_error_msg($db));
				return false;
			}
		}
		// copy all item codes from stock_master into item_codes
		$sql = "SELECT `stock_id`,`description`,`category_id` FROM ".$pref."stock_master";
		$result = db_query($sql);
		if (!$result) {
			display_error(_("Cannot select stock identificators")
				.':<br>'. db_error_msg($db));
			return false;
		} else {
			while ($row = db_fetch_assoc($result)) {
				$sql = "INSERT IGNORE "
					.$pref."item_codes (`item_code`,`stock_id`,`description`,`category_id`)
					VALUES('".$row['stock_id']."','".$row['stock_id']."','"
					.$row['description']."','".$row['category_id']."')";
				$res2 = db_query($sql);
				if (!$res2) {
					display_error(_("Cannot insert stock id into item_codes")
						.':<br>'. db_error_msg($db));
					return false;
				}
			}
		}
		// remove obsolete bank_trans_types table 
		// (DROP queries are skipped during non-forced upgrade)
		$sql = "DROP TABLE IF EXISTS `0_bank_trans_types`";
		db_query($sql);
		
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
		if (check_table($pref, 'item_codes')) return false;
		if (check_table($pref, 'company', 'foreign_codes')) return false;
		return true;
	}
};

$install = new fa2_1;
?>