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
//--------------------------------------------------------------------------------------

function add_work_order_quick($wo_ref, $loc_code, $units_reqd, $stock_id, $type, $date_, $memo_, $costs, $cr_acc, $labour, $cr_lab_acc)
{
	global $Refs;

	begin_transaction();

	// if unassembling, reverse the stock movements
	if ($type == WO_UNASSEMBLY)
		$units_reqd = -$units_reqd;

	add_material_cost($stock_id, $units_reqd, $date_);

	$date = date2sql($date_);
	if (!isset($costs) || ($costs == ""))
		$costs = 0;
	add_overhead_cost($stock_id, $units_reqd, $date_, $costs);
	if (!isset($labour) || ($labour == ""))
		$labour = 0;
	add_labour_cost($stock_id, $units_reqd, $date_, $labour);
		
	$sql = "INSERT INTO ".TB_PREF."workorders (wo_ref, loc_code, units_reqd, units_issued, stock_id,
		type, additional_costs, date_, released_date, required_by, released, closed)
    	VALUES (".db_escape($wo_ref).", ".db_escape($loc_code).", ".db_escape($units_reqd)
    	.", ".db_escape($units_reqd).", ".db_escape($stock_id).",
		".db_escape($type).", ".db_escape($costs).", '$date', '$date', '$date', 1, 1)";
	db_query($sql, "could not add work order");

	$woid = db_insert_id();

	//--------------------------------------------------------------------------

	// create Work Order Requirements based on the bom
	$result = get_bom($stock_id);

	while ($bom_item = db_fetch($result))
	{

		$unit_quantity = $bom_item["quantity"];
		$item_quantity = $bom_item["quantity"] * $units_reqd;


		$sql = "INSERT INTO ".TB_PREF."wo_requirements (workorder_id, stock_id, workcentre, units_req, units_issued, loc_code)
			VALUES ($woid, " . "'" . $bom_item["component"] . "'" . ",
			'". $bom_item["workcentre_added"] . "',
			$unit_quantity,	$item_quantity, '" . $bom_item["loc_code"] . "')";

        db_query($sql, "The work order requirements could not be added");

		// insert a -ve stock move for each item
		add_stock_move(ST_WORKORDER, $bom_item["component"], $woid,
			$bom_item["loc_code"], $date_, $wo_ref, -$item_quantity, 0);
	}


	// -------------------------------------------------------------------------

	// insert a +ve stock move for the item being manufactured
	add_stock_move(ST_WORKORDER, $stock_id, $woid,	$loc_code, $date_,
		$wo_ref, $units_reqd, 0);

	// -------------------------------------------------------------------------

	work_order_quick_costs($woid, $stock_id, $units_reqd, $date_, 0, $costs, $cr_acc, $labour, $cr_lab_acc);

	// -------------------------------------------------------------------------

	add_comments(ST_WORKORDER, $woid, $date_, $memo_);

	$Refs->save(ST_WORKORDER, $woid, $wo_ref);
	add_audit_trail(ST_WORKORDER, $woid, $date_,_("Quick production."));
	commit_transaction();
	return $woid;
}

//--------------------------------------------------------------------------------------

function work_order_quick_costs($woid, $stock_id, $units_reqd, $date_, $advanced=0, $costs=0, $cr_acc="", $labour=0, $cr_lab_acc="")
{
	global $wo_cost_types;
	$result = get_bom($stock_id);

	// credit all the components
	$total_cost = 0;
	while ($bom_item = db_fetch($result))
	{

		$bom_accounts = get_stock_gl_code($bom_item["component"]);

		$bom_cost = $bom_item["ComponentCost"] * $units_reqd;

		if ($advanced)
		{
			update_wo_requirement_issued($woid, $bom_item['component'], $bom_item["quantity"] * $units_reqd); 			
			// insert a -ve stock move for each item
			add_stock_move(ST_MANURECEIVE, $bom_item["component"], $advanced,
				$bom_item["loc_code"], $date_, "", -$bom_item["quantity"] * $units_reqd, 0);
		}
		$total_cost += add_gl_trans_std_cost(ST_WORKORDER, $woid, $date_, $bom_accounts["inventory_account"], 0, 0,
			null, -$bom_cost);

	}
	if ($advanced)
	{
		// also take the additional issues
		$res = get_additional_issues($woid);
		$wo = get_work_order($woid);
		$issue_total = 0;
		while ($item = db_fetch($res))
		{
			$standard_cost = get_standard_cost($item['stock_id']);
			$issue_cost = $standard_cost * $item['qty_issued'] * $units_reqd / $wo['units_reqd'];
			$issue = get_stock_gl_code($item['stock_id']);
			$total_cost += add_gl_trans_std_cost(ST_WORKORDER, $woid, $date_, $issue["inventory_account"], 0, 0,
				null, -$issue_cost);
			$issue_total += $issue_cost;
		}
		if ($issue_total != 0)
			add_issue_cost($stock_id, $units_reqd, $date_, $issue_total);
		$lcost = get_gl_wo_cost($woid, WO_LABOUR);
		add_labour_cost($stock_id, $units_reqd, $date_, $lcost * $units_reqd / $wo['units_reqd']);
		$ocost = get_gl_wo_cost($woid, WO_OVERHEAD);
		add_overhead_cost($stock_id, $units_reqd, $date_, $ocost * $units_reqd / $wo['units_reqd']);
	}
	// credit additional costs
	$item_accounts = get_stock_gl_code($stock_id);
	if ($costs != 0.0)
	{
		add_gl_trans_std_cost(ST_WORKORDER, $woid, $date_, $cr_acc,
			0, 0, $wo_cost_types[WO_OVERHEAD], -$costs, PT_WORKORDER, WO_OVERHEAD);
		$is_bank_to = is_bank_account($cr_acc);
    	if ($is_bank_to)
    	{
    		add_bank_trans(ST_WORKORDER, $woid, $is_bank_to, "",
    			$date_, -$costs, PT_WORKORDER, WO_OVERHEAD, get_company_currency(),
    			"Cannot insert a destination bank transaction");
    	}
			
		add_gl_trans_std_cost(ST_WORKORDER, $woid, $date_, $item_accounts["assembly_account"],
			$item_accounts["dimension_id"], $item_accounts["dimension2_id"], $wo_cost_types[WO_OVERHEAD], $costs, 
			PT_WORKORDER, WO_OVERHEAD);
	}
	if ($labour != 0.0)
	{
		add_gl_trans_std_cost(ST_WORKORDER, $woid, $date_, $cr_lab_acc,
			0, 0, $wo_cost_types[WO_LABOUR], -$labour, PT_WORKORDER, WO_LABOUR);
		$is_bank_to = is_bank_account($cr_lab_acc);
    	if ($is_bank_to)
    	{
    		add_bank_trans(ST_WORKORDER, $woid, $is_bank_to, "",
    			$date_, -$labour, PT_WORKORDER, WO_LABOUR, get_company_currency(),
    			"Cannot insert a destination bank transaction");
    	}
			
		add_gl_trans_std_cost(ST_WORKORDER, $woid, $date_, $item_accounts["assembly_account"],
			$item_accounts["dimension_id"], $item_accounts["dimension2_id"], $wo_cost_types[WO_LABOUR], $labour, 
			PT_WORKORDER, WO_LABOUR);
	}
	// debit total components $total_cost
	add_gl_trans_std_cost(ST_WORKORDER, $woid, $date_, $item_accounts["inventory_account"],
		0, 0, null, -$total_cost);
}

//--------------------------------------------------------------------------------------

?>