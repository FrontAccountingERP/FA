<?php

include_once($path_to_root . "/gl/includes/gl_db.inc");

function validate_bank_trans()
{
	dump_msg("<b>Checking bank transactions.........</b>");

	// check that the account in the trans is actually a P/L account
	$sql = "SELECT * FROM ".TB_PREF."bank_trans,".TB_PREF."chart_master, ".TB_PREF."chart_types
		WHERE ".TB_PREF."bank_trans.bank_act=".TB_PREF."chart_master.account_code
		AND  ".TB_PREF."chart_master.account_type=".TB_PREF."chart_types.id
		AND (".TB_PREF."chart_types.class_id = 1 OR ".TB_PREF."chart_types.class_id = 2)";

	$result = db_query($sql);
	if (db_num_rows($result) > 0) 
	{
		dump_msg("There are bank transactions with accounts that are not Profit&Loss");
	}

	$sql = "SELECT * FROM ".TB_PREF."bank_trans";
	$result = db_query($sql);
	while ($banktrans = db_fetch($result)) 
	{
		// make sure bank_act is valid
		$get = get_gl_account($banktrans["bank_act"]);
		if ($get == null)
			dump_msg("Invalid Bank Account for bank trans " . $banktrans["id"]);

		// make sure the type is valid
		$get = get_bank_trans_type($banktrans["bank_trans_type_id"]);
		if ($get == null)
			dump_msg("Invalid bank_trans_type_id (" . $banktrans["bank_trans_type_id"] . ") for bank_trans " . $banktrans["id"]);

		if ($banktrans["type"] != 1 && $banktrans["type"] != 2 && $banktrans["type"] != 4
			&& $banktrans["type"] != 22 && $banktrans["type"] != 12)
			dump_msg("Invalid type for bank_trans " . $banktrans["id"]);
	}
}


?>