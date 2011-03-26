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
function work_order_produce($woid, $ref, $quantity, $date_, $memo_, $close_wo)
{
	global $Refs;

	begin_transaction();
	$args = func_get_args();
	$args = (object)array_combine(array('woid', 'ref', 'quantity', 'date_', 'memo_','close_wo'),
		$args);
	$args->trans_no = 0;
	hook_db_prewrite($args, ST_MANURECEIVE);

	$details = get_work_order($woid);

    if (strlen($details[0]) == 0)
    {
    	echo _("The order number sent is not valid.");
    	exit;
    }

	if (work_order_is_closed($woid))
	{
		display_error("UNEXPECTED : Producing Items for a closed Work Order");
		cancel_transaction();
		exit;
	}

    $date = date2sql($date_);

    $sql = "INSERT INTO ".TB_PREF."wo_manufacture (workorder_id, reference, quantity, date_)
		VALUES (".db_escape($woid).", ".db_escape($ref).", ".db_escape($quantity)
		.", '$date')";

	db_query($sql,"A work order manufacture could not be added");

	$id = db_insert_id();
	
	// -------------------------------------------------------------------------

	work_order_quick_costs($woid, $details["stock_id"], $quantity, $date_, $id);
	
	// -------------------------------------------------------------------------

	// Chaitanya: stamp BOM cost to finished item
	$m_cost = 0;
    $result = get_bom($details["stock_id"]);
	while ($bom_item = db_fetch($result))
	{
		$standard_cost = get_standard_cost($bom_item['component']);
		$m_cost += ($bom_item['quantity'] * $standard_cost);
	}	

	// insert a +ve stock move for the item being manufactured
	// negative means "unproduce" or unassemble
	add_stock_move(ST_MANURECEIVE, $details["stock_id"], $id,
		$details["loc_code"], $date_, $ref, $quantity, $m_cost);
	// update wo quantity and close wo if requested
	work_order_update_finished_quantity($woid, $quantity, $close_wo);

	if ($memo_)
		add_comments(ST_MANURECEIVE, $id, $date_, $memo_);

	$Refs->save(ST_MANURECEIVE, $id, $ref);
	add_audit_trail(ST_MANURECEIVE, $id, $date_, _("Production."));

	$args->trans_no = $id;
	hook_db_postwrite($args, ST_MANURECEIVE);
	commit_transaction();
}

//--------------------------------------------------------------------------------------------

function get_work_order_produce($id)
{
	$sql = "SELECT ".TB_PREF."wo_manufacture.*,".TB_PREF."workorders.stock_id, "
		.TB_PREF."stock_master.description AS StockDescription
		FROM ".TB_PREF."wo_manufacture, ".TB_PREF."workorders, ".TB_PREF."stock_master
		WHERE ".TB_PREF."wo_manufacture.workorder_id=".TB_PREF."workorders.id
		AND ".TB_PREF."stock_master.stock_id=".TB_PREF."workorders.stock_id
		AND ".TB_PREF."wo_manufacture.id=".db_escape($id);
    $result = db_query($sql, "The work order production could not be retrieved");

    return db_fetch($result);
}

//--------------------------------------------------------------------------------------

function get_work_order_productions($woid)
{
	$sql = "SELECT * FROM ".TB_PREF."wo_manufacture WHERE workorder_id="
		.db_escape($woid)." ORDER BY id";
    return db_query($sql, "The work order issues could not be retrieved");
}

//--------------------------------------------------------------------------------------

function exists_work_order_produce($id)
{
	$sql = "SELECT id FROM ".TB_PREF."wo_manufacture WHERE id=".db_escape($id);
	$result = db_query($sql, "Cannot retreive a wo production");

    return (db_num_rows($result) > 0);
}

//--------------------------------------------------------------------------------------------

function void_work_order_produce($type_no)
{
	begin_transaction();
	hook_db_prevoid(ST_MANURECEIVE, $type_no);
	
	//Chaitanya : Skip processing already voided entry i.e. explicitly voided
	$void_entry = get_voided_entry(ST_MANURECEIVE, $type_no);
	if ($void_entry)
		return;	

	$row = get_work_order_produce($type_no);

	// deduct the quantity of this production from the parent work order
	work_order_update_finished_quantity($row["workorder_id"], -$row["quantity"]);

	//Chaitanya : skipped this step as BOM may have got changed
	//work_order_quick_costs($row['workorder_id'], $row['stock_id'], -$row['quantity'], sql2date($row['date_']), $type_no);

	// void any related gl trans
	//Chaitanya : Nothing happens due to next statement as all gl postings are done against WO
	//void_gl_trans(ST_MANURECEIVE, $type_no, true);
	
	$woid = $row["workorder_id"];
	$date_ = sql2date($row["date_"]);
		
	$result = get_stock_moves(ST_MANURECEIVE, $type_no);
	while ($myrow = db_fetch($result))
	{
		$issue_cost = $myrow["qty"]*$myrow["standard_cost"];
		$issue = get_stock_gl_code($myrow["stock_id"]);
        $stockitem = get_item($myrow["stock_id"]);
		
		//Chaitanya : Compatibility for Service Items
		if (!is_service($issue["mb_flag"]))
			$ivaccount = $issue["inventory_account"];
		else
			$ivaccount = $issue["cogs_account"];		
		
		if ($issue_cost != 0)
		{
			add_gl_trans_std_cost(ST_WORKORDER, $woid, $date_, $ivaccount, 0, 0,
				$date_.": "._("Reversed the production ")." ".$stockitem["description"],
				-$issue_cost);
		}
	}	
			
	// clear the production record
	$sql = "UPDATE ".TB_PREF."wo_manufacture SET quantity=0 WHERE id=".db_escape($type_no);
	db_query($sql, "Cannot void a wo production");			
	
	//Chaitanya : Shifted below
	// void all related stock moves
	void_stock_move(ST_MANURECEIVE, $type_no);

	commit_transaction();
}


?>