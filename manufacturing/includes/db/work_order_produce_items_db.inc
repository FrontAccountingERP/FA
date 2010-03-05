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
	// insert a +ve stock move for the item being manufactured
	// negative means "unproduce" or unassemble
	add_stock_move(ST_MANURECEIVE, $details["stock_id"], $id,
		$details["loc_code"], $date_, $memo_, $quantity, 0);
	// update wo quantity and close wo if requested
	work_order_update_finished_quantity($woid, $quantity, $close_wo);


	if ($memo_)
		add_comments(ST_MANURECEIVE, $id, $date_, $memo_);

	$Refs->save(ST_MANURECEIVE, $id, $ref);
	add_audit_trail(ST_MANURECEIVE, $id, $date_, _("Production."));

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

	$row = get_work_order_produce($type_no);

	// deduct the quantity of this production from the parent work order
	work_order_update_finished_quantity($row["workorder_id"], -$row["quantity"]);

	work_order_quick_costs($row['workorder_id'], $row['stock_id'], -$row['quantity'], sql2date($row['date_']), $type_no);

	// clear the production record
	$sql = "UPDATE ".TB_PREF."wo_manufacture SET quantity=0 WHERE id=".db_escape($type_no);
	db_query($sql, "Cannot void a wo production");

	// void all related stock moves
	void_stock_move(ST_MANURECEIVE, $type_no);

	// void any related gl trans
	void_gl_trans(ST_MANURECEIVE, $type_no, true);

	commit_transaction();
}


?>