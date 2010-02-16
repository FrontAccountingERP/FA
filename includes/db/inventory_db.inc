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
function get_qoh_on_date($stock_id, $location=null, $date_=null, $exclude=0)
{
	if ($date_ == null)
		$date_ = Today();

	$date = date2sql($date_);

	$sql = "SELECT SUM(qty) FROM ".TB_PREF."stock_moves
		WHERE stock_id=".db_escape($stock_id)."
		AND tran_date <= '$date'";

	if ($location != null)
		$sql .= " AND loc_code = ".db_escape($location);

	$result = db_query($sql, "QOH calulcation failed");

	$myrow = db_fetch_row($result);
	if ($exclude > 0)
	{
		$sql = "SELECT SUM(qty) FROM ".TB_PREF."stock_moves
			WHERE stock_id=".db_escape($stock_id)
			." AND type=".db_escape($exclude)
			." AND tran_date = '$date'";

		$result = db_query($sql, "QOH calulcation failed");
		$myrow2 = db_fetch_row($result);
		if ($myrow2 !== false)
			$myrow[0] -= $myrow2[0];
	}

	return $myrow[0];
}

//--------------------------------------------------------------------------------------

function get_item_edit_info($stock_id)
{
	$sql = "SELECT material_cost + labour_cost + overhead_cost AS standard_cost, units, decimals
		FROM ".TB_PREF."stock_master,".TB_PREF."item_units
		WHERE stock_id=".db_escape($stock_id)
		." AND ".TB_PREF."stock_master.units=".TB_PREF."item_units.abbr";
	$result = db_query($sql, "The standard cost cannot be retrieved");

	return db_fetch($result);
}

//--------------------------------------------------------------------------------------

function get_standard_cost($stock_id)
{
	$sql = "SELECT IF(s.mb_flag='D', 0, material_cost + labour_cost + overhead_cost) AS std_cost
		FROM ".TB_PREF."stock_master s WHERE stock_id=".db_escape($stock_id);
	$result = db_query($sql, "The standard cost cannot be retrieved");

	$myrow = db_fetch_row($result);

	return $myrow[0];
}

//--------------------------------------------------------------------------------------

function is_inventory_item($stock_id)
{
	$sql = "SELECT stock_id FROM ".TB_PREF."stock_master
		WHERE stock_id=".db_escape($stock_id)." AND mb_flag <> 'D'";
	$result = db_query($sql, "Cannot query is inventory item or not");

	return db_num_rows($result) > 0;
}

//-------------------------------------------------------------------

function last_negative_stock_begin_date($stock_id, $to)
{
	$to = date2sql($to);
	$sql ="SET @q = 0";
	db_query($sql);
	$sql = "SET @flag = 0";
	db_query($sql);
	$sql = "SELECT SUM(qty), @q:= @q + qty, IF(@q < 0 AND @flag=0, @flag:=1,@flag:=0), IF(@q < 0 AND @flag=1, tran_date,'') AS begin_date 
		FROM ".TB_PREF."stock_moves
		WHERE stock_id=".db_escape($stock_id)." AND tran_date<='$to' 
		AND qty <> 0
		GROUP BY stock_id ORDER BY tran_date";

	$result = db_query($sql, "The dstock moves could not be retrieved");
	$row = db_fetch_row($result);
	return $row[3];
}

//-------------------------------------------------------------------

function get_deliveries_between($stock_id, $from, $to)
{
	$from = date2sql($from);
	$to = date2sql($to);
	$sql = "SELECT SUM(-qty), SUM(-qty*standard_cost) FROM ".TB_PREF."stock_moves
		WHERE type=".ST_CUSTDELIVERY." AND stock_id=".db_escape($stock_id)." AND
			tran_date>='$from' AND tran_date<='$to' GROUP BY stock_id";

	$result = db_query($sql, "The deliveries could not be updated");
	return db_fetch_row($result);
}

//-------------------------------------------------------------------

function adjust_deliveries($stock_id, $material_cost, $to)
{
	if (!is_inventory_item($stock_id))
		return;
	$from = last_negative_stock_begin_date($stock_id, $to);
	if ($from == false || $from == "")
		return;
	$from = sql2date($from);
	$row = get_deliveries_between($stock_id, $from, $to);
	if ($row == false)
		return;	
	$old_cost = $row[1];
	$new_cost = $row[0] * $material_cost;
	$diff = $new_cost - $old_cost;
	if ($diff != 0)
	{
	   	$update_no = get_next_trans_no(ST_COSTUPDATE);
		if (!is_date_in_fiscalyear($to))
			$to = end_fiscalyear();
	   
		$stock_gl_code = get_stock_gl_code($stock_id);

		$memo_ = _("Cost was ") . $old_cost . _(" changed to ") . $new_cost . _(" for item ")."'$stock_id'";
		add_gl_trans_std_cost(ST_COSTUPDATE, $update_no, $to, $stock_gl_code["cogs_account"], 
			$stock_gl_code["dimension_id"], $stock_gl_code["dimension2_id"], $memo_, $diff);	   

		add_gl_trans_std_cost(ST_COSTUPDATE, $update_no, $to, $stock_gl_code["inventory_account"], 0, 0, $memo_, 
			-$diff);
		add_audit_trail(ST_COSTUPDATE, $update_no, $to);
   	}
}

function get_stock_gl_code($stock_id)
{
	/*Gets the GL Codes relevant to the item account  */

	$sql = "SELECT inventory_account, cogs_account,
		adjustment_account, sales_account, assembly_account, dimension_id, dimension2_id FROM
		".TB_PREF."stock_master WHERE stock_id = ".db_escape($stock_id);

	$get = db_query($sql,"retreive stock gl code");
	return db_fetch($get);
}

//--------------------------------------------------------------------------------------

// $date_ - display / non-sql date
// $std_cost - in HOME currency
// $show_or_hide - wil this move be visible in reports, etc
// $price - in $person_id's currency

function add_stock_move($type, $stock_id, $trans_no, $location,
    $date_, $reference, $quantity, $std_cost, $person_id=0, $show_or_hide=1,
    $price=0, $discount_percent=0, $error_msg="")
{
	// do not add a stock move if it's a non-inventory item
	if (!is_inventory_item($stock_id))
		return null;

	$date = date2sql($date_);

	$sql = "INSERT INTO ".TB_PREF."stock_moves (stock_id, trans_no, type, loc_code,
		tran_date, person_id, reference, qty, standard_cost, visible, price,
		discount_percent) VALUES (".db_escape($stock_id)
		.", ".db_escape($trans_no).", ".db_escape($type)
		.",	".db_escape($location).", '$date', "
		.db_escape($person_id).", ".db_escape($reference).", "
		.db_escape($quantity).", ".db_escape($std_cost).","
		.db_escape($show_or_hide).", ".db_escape($price).", "
		.db_escape($discount_percent).")";

	if ($error_msg == "")
		$error_msg = "The stock movement record cannot be inserted";

	db_query($sql, $error_msg);

	return db_insert_id();
}

function update_stock_move_pid($type, $stock_id, $from, $to, $pid, $cost)
{
	$from = date2sql($from);
	$to = date2sql($to);
		$sql = "UPDATE ".TB_PREF."stock_moves SET standard_cost=".db_escape($cost)
			." WHERE type=".db_escape($type)
			."	AND stock_id=".db_escape($stock_id)
			."  AND tran_date>='$from' AND tran_date<='$to' 
			AND person_id = ".db_escape($pid);
	db_query($sql, "The stock movement standard_cost cannot be updated");
}

//--------------------------------------------------------------------------------------------------

function get_stock_moves($type, $type_no, $visible=false)
{
	$sql = "SELECT ".TB_PREF."stock_moves.*, ".TB_PREF."stock_master.description, "
		.TB_PREF."stock_master.units,".TB_PREF."locations.location_name,"
		.TB_PREF."stock_master.material_cost + "
			.TB_PREF."stock_master.labour_cost + "
			.TB_PREF."stock_master.overhead_cost AS FixedStandardCost
		FROM ".TB_PREF."stock_moves,".TB_PREF."locations,".TB_PREF."stock_master
		WHERE ".TB_PREF."stock_moves.stock_id = ".TB_PREF."stock_master.stock_id
		AND ".TB_PREF."locations.loc_code=".TB_PREF."stock_moves.loc_code
		AND type=".db_escape($type)." AND trans_no=".db_escape($type_no)." ORDER BY trans_id";
	if ($visible)
		$sql .= " AND ".TB_PREF."stock_moves.visible=1";

	return db_query($sql, "Could not get stock moves");
}

//--------------------------------------------------------------------------------------------------

function void_stock_move($type, $type_no)
{
	$sql = "UPDATE ".TB_PREF."stock_moves SET qty=0, price=0, discount_percent=0,
		standard_cost=0	WHERE type=".db_escape($type)." AND trans_no=".db_escape($type_no);

	db_query($sql, "Could not void stock moves");
}

//--------------------------------------------------------------------------------------------------

function get_location_name($loc_code)
{
	$sql = "SELECT location_name FROM ".TB_PREF."locations WHERE loc_code="
		.db_escape($loc_code);

	$result = db_query($sql, "could not retreive the location name for $loc_code");

	if (db_num_rows($result) == 1)
	{
		$row = db_fetch_row($result);
		return $row[0];
	}

	display_db_error("could not retreive the location name for $loc_code", $sql, true);
}

//--------------------------------------------------------------------------------------------------


?>