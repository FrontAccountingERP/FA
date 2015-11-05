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
function get_wo_requirements($woid)
{
	$sql = "SELECT req.*, item.description, item.mb_flag, loc.location_name, 
			center.name AS WorkCentreDescription
		FROM (".TB_PREF."wo_requirements req,"
			.TB_PREF."locations loc,"
			.TB_PREF."workcentres center)
			INNER JOIN ".TB_PREF."stock_master item ON req.stock_id=item.stock_id 
		WHERE workorder_id=".db_escape($woid)."
		AND loc.loc_code = req.loc_code
		AND center.id=workcentre";

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
			VALUES (".db_escape($woid).", '" .
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
	$sql="DELETE FROM ".TB_PREF."wo_requirements WHERE workorder_id=".db_escape($woid);
	db_query($sql,"The work order requirements could not be deleted");	
}


//--------------------------------------------------------------------------------------

function update_wo_requirement_issued($woid, $stock_id, $quantity)
{
	$sql = "UPDATE ".TB_PREF."wo_requirements SET units_issued = units_issued + ".db_escape($quantity)."
		WHERE workorder_id = ".db_escape($woid)." AND stock_id = ".db_escape($stock_id);

	db_query($sql, "The work requirements issued quantity couldn't be updated");
}

//--------------------------------------------------------------------------------------

function void_wo_requirements($woid)
{
	$sql = "UPDATE ".TB_PREF."wo_requirements SET units_issued = 0 WHERE workorder_id = "
	.db_escape($woid);

	db_query($sql, "The work requirements issued quantity couldn't be voided");	
}

