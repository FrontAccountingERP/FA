<?php

function get_wo_requirements($woid)
{
	$sql = "SELECT ".TB_PREF."wo_requirements.*, ".TB_PREF."stock_master.description,
		".TB_PREF."stock_master.mb_flag, 
		".TB_PREF."locations.location_name, 
		".TB_PREF."workcentres.name AS WorkCentreDescription FROM 
		(".TB_PREF."wo_requirements, ".TB_PREF."locations, ".TB_PREF."workcentres) INNER JOIN ".TB_PREF."stock_master ON 
		".TB_PREF."wo_requirements.stock_id = ".TB_PREF."stock_master.stock_id 
		WHERE workorder_id=$woid
		AND ".TB_PREF."locations.loc_code = ".TB_PREF."wo_requirements.loc_code
		AND ".TB_PREF."workcentres.id=workcentre";

	return db_query($sql, "The work order requirements could not be retrieved");    
}

//--------------------------------------------------------------------------------------

function create_wo_requirements($woid, $stock_id)
{
	// create Work Order Requirements based on the bom
	$result = get_bom($stock_id);	
	
	while ($myrow = db_fetch($result)) 
	{
		
		$sql = "INSERT INTO ".TB_PREF."wo_requirements (workorder_id, stock_id, workcentre, units_req, loc_code)
			VALUES ($woid, '" .
			$myrow["component"] . "', '"	.
			$myrow["workcentre_added"] . "', '"	.
			$myrow["quantity"] . "', '"	.
			$myrow["loc_code"] . "')";
			
        db_query($sql, "The work order requirements could not be added");
	}		
}

//--------------------------------------------------------------------------------------

function delete_wo_requirements($woid)
{
	$sql="DELETE FROM ".TB_PREF."wo_requirements WHERE workorder_id=$woid";
	db_query($sql,"The work order requirements could not be deleted");	
}


//--------------------------------------------------------------------------------------

function update_wo_requirement_issued($woReqID, $quantity)
{
	$sql = "UPDATE ".TB_PREF."wo_requirements SET units_issued = units_issued + $quantity
		WHERE id = '$woReqID'";
				
	db_query($sql, "The work requirements issued quantity couldn't be updated");
}

//--------------------------------------------------------------------------------------

function void_wo_requirements($woid)
{
	$sql = "UPDATE ".TB_PREF."wo_requirements SET units_issued = 0 WHERE workorder_id = $woid";
				
	db_query($sql, "The work requirements issued quantity couldn't be voided");	
}

//--------------------------------------------------------------------------------------

?>