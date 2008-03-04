<?php

//--------------------------------------------------------------------------------------

function add_work_order_quick($wo_ref, $loc_code, $units_reqd, $stock_id, $type, $date_, $additional_costs, $memo_)
{
	begin_transaction();

	// if unassembling, reverse the stock movements
	if ($type == wo_types::unassemble())
		$units_reqd = -$units_reqd;

	add_material_cost($stock_id, $units_reqd, $date_);

	$date = date2sql($date_);
	if (!isset($additional_costs) || ($additional_costs == ""))
		$additional_costs = 0;

	$sql = "INSERT INTO ".TB_PREF."workorders (wo_ref, loc_code, units_reqd, units_issued, stock_id,
		type, additional_costs, date_, released_date, required_by, released, closed)
    	VALUES ('$wo_ref', '$loc_code', $units_reqd, $units_reqd, '$stock_id',
		$type, $additional_costs, '$date', '$date', '$date', 1, 1)";
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
		add_stock_move(systypes::work_order(), $bom_item["component"], $woid,
			$bom_item["loc_code"], $date_, $wo_ref, -$item_quantity, 0);
	}


	// -------------------------------------------------------------------------

	// insert a +ve stock move for the item being manufactured
	add_stock_move(systypes::work_order(), $stock_id, $woid,	$loc_code, $date_,
		$wo_ref, $units_reqd, 0);

	// -------------------------------------------------------------------------

	work_order_quick_costs($woid, $stock_id, $units_reqd, $date_, $additional_costs);

	// -------------------------------------------------------------------------

	add_comments(systypes::work_order(), $woid, $date_, $memo_);

	add_forms_for_sys_type(systypes::work_order(), $woid, $type);

	references::save_last($wo_ref, systypes::work_order());

	commit_transaction();
	return $woid;
}

//--------------------------------------------------------------------------------------

function work_order_quick_costs($woid, $stock_id, $units_reqd, $date_, $additional_costs)
{
	$result = get_bom($stock_id);

	// credit all the components
	$total_cost = 0;
	while ($bom_item = db_fetch($result))
	{

		$bom_accounts = get_stock_gl_code($bom_item["component"]);

		$bom_cost = $bom_item["standard_cost"] * $bom_item["quantity"] * $units_reqd;

		add_gl_trans_std_cost(systypes::work_order(), $woid, $date_, $bom_accounts["inventory_account"], 0, 0,
			null, -$bom_cost);

		$total_cost += $bom_cost;
	}
	// credit additional costs
	$item_accounts = get_stock_gl_code($stock_id);
	add_gl_trans_std_cost(systypes::work_order(), $woid, $date_, $item_accounts["assembly_account"],
		$item_accounts["dimension_id"], $item_accounts["dimension2_id"], null, -$additional_costs);

	// debit total components + additional
	$total_cost += $additional_costs;
	add_gl_trans_std_cost(systypes::work_order(), $woid, $date_, $item_accounts["inventory_account"],
		0, 0, null, $total_cost);
}

//--------------------------------------------------------------------------------------

?>