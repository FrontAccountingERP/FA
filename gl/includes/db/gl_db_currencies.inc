<?php

//---------------------------------------------------------------------------------------------

function update_currency($curr_abrev, $symbol, $currency, $country, $hundreds_name)
{
	$sql = "UPDATE ".TB_PREF."currencies SET currency=".db_escape($currency).", curr_symbol='$symbol',
		country=".db_escape($country).", hundreds_name=".db_escape($hundreds_name)." WHERE curr_abrev = '$curr_abrev'";

	db_query($sql, "could not update currency for $curr_abrev");
}

//---------------------------------------------------------------------------------------------

function add_currency($curr_abrev, $symbol, $currency, $country, $hundreds_name)
{
	$sql = "INSERT INTO ".TB_PREF."currencies (curr_abrev, curr_symbol, currency, country, hundreds_name)
		VALUES (".db_escape($curr_abrev).", '$symbol', ".db_escape($currency).", ".db_escape($country).", ".db_escape($hundreds_name).")";

	db_query($sql, "could not add currency for $curr_abrev");
}

//---------------------------------------------------------------------------------------------

function delete_currency($curr_code)
{
	$sql="DELETE FROM ".TB_PREF."currencies WHERE curr_abrev='$curr_code'";
	db_query($sql, "could not delete currency	$curr_code");

	$sql="DELETE FROM ".TB_PREF."exchange_rates WHERE curr_code='$curr_code'";
	db_query($sql, "could not delete exchange rates for currency $curr_code");
}

//---------------------------------------------------------------------------------------------

function get_currency($curr_code)
{
	$sql = "SELECT * FROM ".TB_PREF."currencies WHERE curr_abrev='$curr_code'";
	$result = db_query($sql, "could not get currency $curr_code");

	$row = db_fetch($result);
	return $row;
}

//---------------------------------------------------------------------------------------------

function get_currencies()
{
	$sql = "SELECT * FROM ".TB_PREF."currencies";
	return db_query($sql, "could not get currencies");
}

//---------------------------------------------------------------------------------------------

?>