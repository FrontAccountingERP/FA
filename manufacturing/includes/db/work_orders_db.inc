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

function add_material_cost($stock_id, $qty, $date_)
{
	$m_cost = 0;
    $result = get_bom($stock_id);
	while ($bom_item = db_fetch($result))
	{
		$standard_cost = get_standard_cost($bom_item['component']);
		$m_cost += ($bom_item['quantity'] * $standard_cost);
	}
	$sql = "SELECT material_cost FROM ".TB_PREF."stock_master WHERE stock_id = '$stock_id'";
	$result = db_query($sql);
	$myrow = db_fetch($result);
	$material_cost =  $myrow['material_cost'];
	$qoh = get_qoh_on_date($stock_id, null, $date_);
	if ($qoh + $qty <= 0)
		$material_cost = 0;
	else
		$material_cost = ($qoh * $material_cost + $qty * $m_cost) /	($qoh + $qty);
	$sql = "UPDATE ".TB_PREF."stock_master SET material_cost=$material_cost
		WHERE stock_id='$stock_id'";
	db_query($sql,"The cost details for the inventory item could not be updated");
}

function add_overhead_cost($stock_id, $qty, $date_, $costs)
{
	if ($qty != 0)
		$costs /= $qty;
	$sql = "SELECT overhead_cost FROM ".TB_PREF."stock_master WHERE stock_id = '$stock_id'";
	$result = db_query($sql);
	$myrow = db_fetch($result);
	$overhead_cost =  $myrow['overhead_cost'];
	$qoh = get_qoh_on_date($stock_id, null, $date_);
	if ($qoh + $qty <= 0)
		$overhead_cost = 0;
	else
		$overhead_cost = ($qoh * $overhead_cost + $qty * $costs) /	($qoh + $qty);
	$sql = "UPDATE ".TB_PREF."stock_master SET overhead_cost=$overhead_cost
		WHERE stock_id='$stock_id'";
	db_query($sql,"The cost details for the inventory item could not be updated");
}

function add_labour_cost($stock_id, $qty, $date_, $costs)
{
	if ($qty != 0)
		$costs /= $qty;
	$sql = "SELECT labour_cost FROM ".TB_PREF."stock_master WHERE stock_id = '$stock_id'";
	$result = db_query($sql);
	$myrow = db_fetch($result);
	$labour_cost =  $myrow['labour_cost'];
	$qoh = get_qoh_on_date($stock_id, null, $date_);
	if ($qoh + $qty <= 0)
		$labour_cost = 0;
	else
		$labour_cost = ($qoh * $labour_cost + $qty * $costs) /	($qoh + $qty);
	$sql = "UPDATE ".TB_PREF."stock_master SET labour_cost=$labour_cost
		WHERE stock_id='$stock_id'";
	db_query($sql,"The cost details for the inventory item could not be updated");
}

function add_issue_cost($stock_id, $qty, $date_, $costs)
{
	if ($qty != 0)
		$costs /= $qty;
	$sql = "SELECT material_cost FROM ".TB_PREF."stock_master WHERE stock_id = '$stock_id'";
	$result = db_query($sql);
	$myrow = db_fetch($result);
	$material_cost =  $myrow['material_cost'];
	$qoh = get_qoh_on_date($stock_id, null, $date_);
	if ($qoh + $qty  <= 0)
		$material_cost = 0;
	else
		$material_cost = ($qty * $costs) /	($qoh + $qty);
	$sql = "UPDATE ".TB_PREF."stock_master SET material_cost=material_cost+$material_cost
		WHERE stock_id='$stock_id'";
	db_query($sql,"The cost details for the inventory item could not be updated");
}

function add_work_order($wo_ref, $loc_code, $units_reqd, $stock_id,
	$type, $date_, $required_by, $memo_, $costs, $cr_acc, $labour, $cr_lab_acc)
{
	if (!($type == wo_types::advanced()))
		return add_work_order_quick($wo_ref, $loc_code, $units_reqd, $stock_id, $type, $date_, $memo_, $costs, $cr_acc, $labour, $cr_lab_acc);

	begin_transaction();

	add_material_cost($stock_id, $units_reqd, $date_);

	$date = date2sql($date_);
	$required = date2sql($required_by);

	$sql = "INSERT INTO ".TB_PREF."workorders (wo_ref, loc_code, units_reqd, stock_id,
		type, date_, required_by)
    	VALUES (".db_escape($wo_ref).", ".db_escape($loc_code).", $units_reqd, '$stock_id',
		$type, '$date', '$required')";
	db_query($sql, "could not add work order");

	$woid = db_insert_id();

	add_comments(systypes::work_order(), $woid, $required_by, $memo_);

	references::save(systypes::work_order(), $woid, $wo_ref);
	add_audit_trail(systypes::work_order(), $woid, $date_);

	commit_transaction();

	return $woid;
}

//--------------------------------------------------------------------------------------

function update_work_order($woid, $loc_code, $units_reqd, $stock_id,
					$date_, $required_by, $memo_)
{
	begin_transaction();

	add_material_cost($_POST['old_stk_id'], -$_POST['old_qty'], $date_);
	add_material_cost($stock_id, $units_reqd, $date_);

	$date = date2sql($date_);
	$required = date2sql($required_by);

	$sql = "UPDATE ".TB_PREF."workorders SET loc_code=".db_escape($loc_code).",
		units_reqd=$units_reqd, stock_id='$stock_id',
		required_by='$required',
		date_='$date'
		WHERE id = $woid";

	db_query($sql, "could not update work order");

	update_comments(systypes::work_order(), $woid, null, $memo_);
	add_audit_trail(systypes::work_order(), $woid, $date_, _("Updated."));

	commit_transaction();
}

function delete_work_order($woid)
{
	begin_transaction();

	add_material_cost($_POST['stock_id'], -$_POST['quantity'], $_POST['date_']);

	// delete the work order requirements
	delete_wo_requirements($woid);

	// delete the actual work order
	$sql = "DELETE FROM ".TB_PREF."workorders WHERE id=$woid";
	db_query($sql,"The work order could not be deleted");

	delete_comments(systypes::work_order(), $woid);
	add_audit_trail(systypes::work_order(), $woid, $_POST['date_'], _("Canceled."));

	commit_transaction();
}

//--------------------------------------------------------------------------------------

function get_work_order($woid, $allow_null=false)
{
    $sql = "SELECT ".TB_PREF."workorders.*, ".TB_PREF."stock_master.description As StockItemName,
		".TB_PREF."locations.location_name, ".TB_PREF."locations.delivery_address 
		FROM ".TB_PREF."workorders, ".TB_PREF."stock_master, ".TB_PREF."locations
		WHERE ".TB_PREF."stock_master.stock_id=".TB_PREF."workorders.stock_id
		AND	".TB_PREF."locations.loc_code=".TB_PREF."workorders.loc_code
		AND ".TB_PREF."workorders.id=$woid
		GROUP BY ".TB_PREF."workorders.id";

	$result = db_query($sql, "The work order issues could not be retrieved");

	if (!$allow_null && db_num_rows($result) == 0)
		display_db_error("Could not find work order $woid", $sql);

	return db_fetch($result);
}

//--------------------------------------------------------------------------------------

function work_order_has_productions($woid)
{
	$sql = "SELECT COUNT(*) FROM ".TB_PREF."wo_manufacture WHERE workorder_id=$woid";
	$result = db_query($sql, "query work order for productions");

	$myrow = db_fetch_row($result);
	return ($myrow[0] > 0);
}


//--------------------------------------------------------------------------------------

function work_order_has_issues($woid)
{
	$sql = "SELECT COUNT(*) FROM ".TB_PREF."wo_issues WHERE workorder_id=$woid";
	$result = db_query($sql, "query work order for issues");

	$myrow = db_fetch_row($result);
	return ($myrow[0] > 0);
}

//--------------------------------------------------------------------------------------

function work_order_has_payments($woid)
{
	$result = get_gl_wo_cost_trans($woid);

    return (db_num_rows($result) != 0);
}

//--------------------------------------------------------------------------------------

function release_work_order($woid, $releaseDate, $memo_)
{
	begin_transaction();

	$myrow = get_work_order($woid);
	$stock_id = $myrow["stock_id"];

	$date = date2sql($releaseDate);

	$sql = "UPDATE ".TB_PREF."workorders SET released_date='$date',
		released=1 WHERE id = $woid";
	db_query($sql, "could not release work order");

	// create Work Order Requirements based on the bom
	create_wo_requirements($woid, $stock_id);

	add_comments(systypes::work_order(), $woid, $releaseDate, $memo_);
	add_audit_trail(systypes::work_order(), $woid, $releaseDate,_("Released."));

	commit_transaction();
}

//--------------------------------------------------------------------------------------

function close_work_order($woid)
{
	$sql = "UPDATE ".TB_PREF."workorders SET closed=1 WHERE id = $woid";
	db_query($sql, "could not close work order");
}

//--------------------------------------------------------------------------------------

function work_order_is_closed($woid)
{
	$sql = "SELECT closed FROM ".TB_PREF."workorders WHERE id = $woid";
	$result = db_query($sql, "could not query work order");
	$row = db_fetch_row($result);
	return ($row[0] > 0);
}

//--------------------------------------------------------------------------------------

function work_order_update_finished_quantity($woid, $quantity, $force_close=0)
{
	$sql = "UPDATE ".TB_PREF."workorders SET units_issued = units_issued + $quantity,
		closed = ((units_issued >= units_reqd) OR $force_close)
		WHERE id = $woid";

	db_query($sql, "The work order issued quantity couldn't be updated");
}

//--------------------------------------------------------------------------------------

function void_work_order($woid)
{
	$work_order = get_work_order($woid);

	if (!($work_order["type"] == wo_types::advanced()))
	{
		begin_transaction();

		$sql = "UPDATE ".TB_PREF."workorders SET closed=1,units_issued=0 WHERE id = $woid";
		db_query($sql, "The work order couldn't be voided");

		// void all related stock moves
		void_stock_move(systypes::work_order(), $woid);

		// void any related gl trans
		void_gl_trans(systypes::work_order(), $woid, true);

		// clear the requirements units received
		void_wo_requirements($woid);

		commit_transaction();
	}
	else
	{
		// void everything inside the work order : issues, productions, payments
	}
}

//--------------------------------------------------------------------------------------

?>