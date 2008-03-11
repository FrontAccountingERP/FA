<?php

function write_item_unit($selected, $abbr, $description, $decimals)
{
    if($selected!='')
		$sql = "UPDATE ".TB_PREF."item_units SET
	 	abbr = '$abbr',
	 	name = '$description',
	 	decimals = $decimals
        	WHERE 	 abbr = '$selected'";
    else
		$sql = "INSERT INTO ".TB_PREF."item_units
			(abbr, name, decimals) VALUES( '$abbr',
	  		'$description', $decimals)";
	  
	db_query($sql,"an item unit could not be updated");	
}

function delete_item_unit($unit)
{
	$sql="DELETE FROM ".TB_PREF."item_units WHERE abbr='$unit'";	
	
	db_query($sql,"an unit of measure could not be deleted");	
}

function get_item_unit($unit)
{
	$sql="SELECT * FROM ".TB_PREF."item_units WHERE abbr='$unit'";	
	
	$result = db_query($sql,"an unit of measure could not be retrieved");
	
	return db_fetch($result);	
}

function get_unit_descr($unit)
{
	$sql = "SELECT description FROM ".TB_PREF."item_units WHERE abbr='$id'";
	
	$result = db_query($sql, "could not unit description");
	
	$row = db_fetch_row($result);
	return $row[0];
}

function item_unit_used($unit) {
	$sql= "SELECT COUNT(*) FROM ".TB_PREF."stock_master WHERE units='$unit'";
	$result = db_query($sql, "could not query stock master");
	$myrow = db_fetch_row($result);
	return ($myrow[0] > 0);
}

function get_all_item_units() {
    $sql = "SELECT * FROM ".TB_PREF."item_units ORDER BY name";
    return  db_query($sql, "could not get stock categories");
}
?>