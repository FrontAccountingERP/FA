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
//----------------------------------------------------------------------------------------
function get_demand_qty($stock_id, $location)
{
	$sql = "SELECT SUM(".TB_PREF."sales_order_details.quantity - ".TB_PREF."sales_order_details.qty_sent) AS QtyDemand
				FROM ".TB_PREF."sales_order_details,
					".TB_PREF."sales_orders
				WHERE ".TB_PREF."sales_order_details.order_no=".TB_PREF."sales_orders.order_no AND ";
	if ($location != "")
		$sql .= TB_PREF."sales_orders.from_stk_loc ='$location' AND ";
	$sql .= TB_PREF."sales_order_details.stk_code = '$stock_id'";

    $result = db_query($sql,"No transactions were returned");
	$row = db_fetch($result);
	return $row['QtyDemand'];
}

// recursion fixed by Tom Moulton
function stock_demand_manufacture($stock_id, $qty, $demand_id, $level=0) 
{
	$demand = 0.0;
	if ($level > 4) {
		display_notification("BOM Too many Manufacturing levels deep $level");
		return $demand;
	}
	$sql = "SELECT parent, component, quantity FROM ".TB_PREF."bom WHERE parent = '$stock_id'";
	$result = db_query($sql, "Could not search bom");
	$bom = array();
	while ($row = db_fetch_row($result)) {
		$bom[] = array($row[0], $row[1], $row[2]);
	}
	db_free_result($result);
	$len = count($bom);
	$i = 0;
	while ($i < $len) {
		$row = $bom[$i];
		$i++;
		$q = $qty * $row[2];
		if ($row[1] == $demand_id) 
			$demand += $q;
		$demand += stock_demand_manufacture($row[1], $q, $demand_id, $level+1);
	}
	return $demand;
}

// recursion fixed by Tom Moulton
function get_demand_asm_qty($stock_id, $location) 
{
	$demand_qty = 0.0;
	$sql = "SELECT ".TB_PREF."sales_order_details.stk_code, SUM(".TB_PREF."sales_order_details.quantity-".TB_PREF."sales_order_details.qty_sent)
				   AS Demmand
				   FROM ".TB_PREF."sales_order_details,
						".TB_PREF."sales_orders,
						".TB_PREF."stock_master
				   WHERE ".TB_PREF."sales_orders.order_no = ".TB_PREF."sales_order_details.order_no AND ";
	if ($location != "")
		$sql .= TB_PREF."sales_orders.from_stk_loc ='$location' AND ";
	$sql .= TB_PREF."sales_order_details.quantity-".TB_PREF."sales_order_details.qty_sent > 0 AND
				   ".TB_PREF."stock_master.stock_id=".TB_PREF."sales_order_details.stk_code AND
				   (".TB_PREF."stock_master.mb_flag='M' OR ".TB_PREF."stock_master.mb_flag='A')
				   GROUP BY ".TB_PREF."sales_order_details.stk_code";
    	$result = db_query($sql, "No transactions were returned");
	while ($row = db_fetch_row($result)) {
		$demand_qty += stock_demand_manufacture($row[0], $row[1], $stock_id);
	}
	return $demand_qty;
}

function get_on_porder_qty($stock_id, $location)
{
	$sql = "SELECT Sum(".TB_PREF."purch_order_details.quantity_ordered - ".TB_PREF."purch_order_details.quantity_received) AS qoo
		FROM ".TB_PREF."purch_order_details INNER JOIN ".TB_PREF."purch_orders ON ".TB_PREF."purch_order_details.order_no=".TB_PREF."purch_orders.order_no
		WHERE ".TB_PREF."purch_order_details.item_code='$stock_id' ";
	if ($location != "")
		$sql .= "AND ".TB_PREF."purch_orders.into_stock_location='$location' ";
	$sql .= "AND ".TB_PREF."purch_order_details.item_code='$stock_id'";
	$qoo_result = db_query($sql,"could not receive quantity on order for item");

	if (db_num_rows($qoo_result) == 1)
	{
		$qoo_row = db_fetch_row($qoo_result);
		$qoo =  $qoo_row[0];
	}
	else
	{
		$qoo = 0;
	}
	return $qoo;
}

function get_on_worder_qty($stock_id, $location)
{
	$sql = "SELECT Sum((".TB_PREF."workorders.units_reqd-".TB_PREF."workorders.units_issued) * 
		(".TB_PREF."wo_requirements.units_req-".TB_PREF."wo_requirements.units_issued)) AS qoo
		FROM ".TB_PREF."wo_requirements INNER JOIN ".TB_PREF."workorders 
			ON ".TB_PREF."wo_requirements.workorder_id=".TB_PREF."workorders.id
		WHERE ".TB_PREF."wo_requirements.stock_id='$stock_id' ";
	if ($location != "")
		$sql .= "AND ".TB_PREF."wo_requirements.loc_code='$location' ";
	$sql .= "AND ".TB_PREF."workorders.released=1";
	$qoo_result = db_query($sql,"could not receive quantity on order for item");
	if (db_num_rows($qoo_result) == 1)
	{
		$qoo_row = db_fetch_row($qoo_result);
		$qoo =  $qoo_row[0];
	}
	else
		$qoo = 0.0;
	$flag = get_mb_flag($stock_id);
	if ($flag == 'A' || $flag == 'M')
	{
		$sql = "SELECT Sum((".TB_PREF."workorders.units_reqd-".TB_PREF."workorders.units_issued)) AS qoo
			FROM ".TB_PREF."workorders 
			WHERE ".TB_PREF."workorders.stock_id='$stock_id' ";
		if ($location != "")	
			$sql .= "AND ".TB_PREF."workorders.loc_code='$location' ";
		$sql .= "AND ".TB_PREF."workorders.released=1";
		$qoo_result = db_query($sql,"could not receive quantity on order for item");
		if (db_num_rows($qoo_result) == 1)
		{
			$qoo_row = db_fetch_row($qoo_result);
			$qoo +=  $qoo_row[0];
		}
	}
	return $qoo;
}

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
    	FROM (".TB_PREF."workcentres, ".TB_PREF."locations, ".TB_PREF."bom) INNER JOIN ".TB_PREF."stock_master ON ".TB_PREF."bom.component = ".TB_PREF."stock_master.stock_id 
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
