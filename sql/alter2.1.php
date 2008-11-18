<?php
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
		return !check_table($pref, 'item_codes');
	}
};

$install = new fa2_1;
?>