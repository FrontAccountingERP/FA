<?php


function add_gl_account($account_code, $account_name, $account_type, $account_code2, $tax_code)
{
	$sql = "INSERT INTO ".TB_PREF."chart_master (account_code, account_code2, account_name, account_type,
		tax_code) 
		VALUES ('$account_code', '$account_code2', '$account_name', $account_type, $tax_code)";

	db_query($sql, "could not add gl account");
}

function update_gl_account($account_code, $account_name, $account_type, $account_code2, $tax_code)
{
    $sql = "UPDATE ".TB_PREF."chart_master SET account_name='$account_name',
		account_type=$account_type, account_code2='$account_code2',
		tax_code=$tax_code WHERE account_code = $account_code";

	db_query($sql, "could not update gl account");
}

function delete_gl_account($code)
{
	$sql = "DELETE FROM ".TB_PREF."chart_master WHERE account_code=$code";

	db_query($sql, "could not delete gl account");
}

function get_gl_accounts($from=null, $to=null)
{
	$sql = "SELECT ".TB_PREF."chart_master.*,".TB_PREF."chart_types.name AS AccountTypeName
		FROM ".TB_PREF."chart_master,".TB_PREF."chart_types
		WHERE ".TB_PREF."chart_master.account_type=".TB_PREF."chart_types.id";
	if ($from != null)
		$sql .= " AND ".TB_PREF."chart_master.account_code >= '$from'";
	if ($to != null)	
		$sql .= " AND ".TB_PREF."chart_master.account_code <= '$to'";
	$sql .= " ORDER BY account_code";
	
	return db_query($sql, "could not get gl accounts");
}

function get_gl_accounts_all($balance=-1)
{
	$sql = "SELECT ".TB_PREF."chart_master.*,".TB_PREF."chart_types.name AS AccountTypeName,".TB_PREF."chart_class.class_name AS AccountClassName
		FROM ".TB_PREF."chart_master,".TB_PREF."chart_types, ".TB_PREF."chart_class
		WHERE ".TB_PREF."chart_master.account_type=".TB_PREF."chart_types.id AND
		".TB_PREF."chart_types.class_id=".TB_PREF."chart_class.cid";
	if ($balance != -1)				
		$sql .= " AND ".TB_PREF."chart_class.balance_sheet=$balance";
	$sql .= " ORDER BY ".TB_PREF."chart_class.cid, ".TB_PREF."chart_types.id, ".TB_PREF."chart_master.account_code";
	
	return db_query($sql, "could not get gl accounts");
}

function get_gl_account($code)
{
	$sql = "SELECT * FROM ".TB_PREF."chart_master WHERE account_code=$code";

	$result = db_query($sql, "could not get gl account");
	return db_fetch($result);
}

function is_account_balancesheet($code)
{
	$sql = "SELECT ".TB_PREF."chart_class.balance_sheet FROM ".TB_PREF."chart_class, ".TB_PREF."chart_types, ".TB_PREF."chart_master
		WHERE ".TB_PREF."chart_master.account_type=".TB_PREF."chart_types.id AND
		".TB_PREF."chart_types.class_id=".TB_PREF."chart_class.cid
		AND ".TB_PREF."chart_master.account_code='$code'";
	
	$result = db_query($sql,"could not retreive the account class for $code");
	$row = db_fetch_row($result);
	return $row[0];
}
	
function get_gl_account_name($code)
{
	$sql = "SELECT account_name from ".TB_PREF."chart_master WHERE account_code='$code'";

	$result = db_query($sql,"could not retreive the account name for $code");

	if (db_num_rows($result) == 1)
	{
		$row = db_fetch_row($result);
		return $row[0];
	}

	display_db_error("could not retreive the account name for $code", $sql, true);
}


?>