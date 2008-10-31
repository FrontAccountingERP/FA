<?php

function add_sales_point($name, $location, $account, $cash, $credit)
{
	$sql = "INSERT INTO ".TB_PREF."sales_pos (pos_name, pos_location, pos_account, cash_sale, credit_sale) VALUES (".db_escape($name)
			. ",".db_escape($location).",".db_escape($account)
			. ",$cash,$credit)";
	db_query($sql, "could not add point of sale");
}

function update_sales_point($id, $name, $location, $account, $cash, $credit)
{

	$sql = "UPDATE ".TB_PREF."sales_pos SET pos_name=".db_escape($name)
				.",pos_location=".db_escape($location)
				.",pos_account=".db_escape($account)
				.",cash_sale =$cash"
				.",credit_sale =$credit"
		 		." WHERE id = $id";
	
	db_query($sql, "could not update sales type");			
}

function get_all_sales_points()
{
	$sql = "SELECT pos.*, loc.location_name, acc.bank_account_name FROM "
		.TB_PREF."sales_pos as pos,"
		.TB_PREF."locations as loc,"
		.TB_PREF."bank_accounts as acc"
		." WHERE pos.pos_location=loc.loc_code AND pos.pos_account=acc.account_code";
	
	return db_query($sql, "could not get all POS definitions");
} 

function get_sales_point($id)
{
	$sql = "SELECT pos.*, loc.location_name, acc.bank_account_name, acc.account_code FROM "
		.TB_PREF."sales_pos as pos,"
		.TB_PREF."locations as loc,"
		.TB_PREF."bank_accounts as acc"
		." WHERE id=$id AND pos.pos_location=loc.loc_code AND pos.pos_account=acc.account_code";
	
	$result = db_query($sql, "could not get POS definition");
	
	return db_fetch($result);
}

function get_sales_point_name($id)
{
	$sql = "SELECT pos_name FROM ".TB_PREF."sales_pos WHERE id=$id";
	
	$result = db_query($sql, "could not get POS name");
	
	$row = db_fetch_row($result);
	return $row[0];
}

function delete_sales_point($id)
{
	$sql="DELETE FROM ".TB_PREF."sales_pos WHERE id=$id";
	db_query($sql,"The point of sale record could not be deleted");
}

?>