<?php

function get_mb_flag($stock_id)
{
	$sql = "SELECT mb_flag FROM ".TB_PREF."stock_master WHERE stock_id = '" . $stock_id . "'";
	$result = db_query($sql, "retreive mb_flag from item");
	
	if (db_num_rows($result) == 0)
		return -1;

	$myrow = db_fetch_row($result);
	return $myrow[0];
}

//--------------------------------------------------------------------------------------

function get_bom($item)
{
	$sql = "SELECT ".TB_PREF."bom.*, ".TB_PREF."locations.location_name, ".TB_PREF."workcentres.name AS WorkCentreDescription, 
    	".TB_PREF."stock_master.description, ".TB_PREF."stock_master.mb_flag AS ResourceType, 
    	".TB_PREF."stock_master.material_cost+ ".TB_PREF."stock_master.labour_cost+".TB_PREF."stock_master.overhead_cost AS standard_cost, units, 
    	".TB_PREF."bom.quantity * (".TB_PREF."stock_master.material_cost+ ".TB_PREF."stock_master.labour_cost+ ".TB_PREF."stock_master.overhead_cost) AS ComponentCost 
    	FROM ".TB_PREF."workcentres, ".TB_PREF."locations, ".TB_PREF."bom INNER JOIN ".TB_PREF."stock_master ON ".TB_PREF."bom.component = ".TB_PREF."stock_master.stock_id 
    	WHERE ".TB_PREF."bom.parent = '" . $item . "'
		AND ".TB_PREF."workcentres.id=".TB_PREF."bom.workcentre_added
		AND ".TB_PREF."bom.loc_code = ".TB_PREF."locations.loc_code ORDER BY ".TB_PREF."bom.id";
	
	return db_query($sql, "The bill of material could not be retrieved");
}

//--------------------------------------------------------------------------------------

function has_bom($item)
{
    $result = get_bom($item);
    
    return (db_num_rows($result) != 0);
}

//--------------------------------------------------------------------------------------

?>