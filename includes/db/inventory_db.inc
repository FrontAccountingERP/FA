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
    {
        $sql = "SELECT SUM(qty) FROM ".TB_PREF."stock_moves
            WHERE stock_id=".db_escape($stock_id);
        $date_ = Today();
        $date = date2sql($date_);
    }
    else
    {
        $date = date2sql($date_);
        $sql = "SELECT SUM(qty) FROM ".TB_PREF."stock_moves
            WHERE stock_id=".db_escape($stock_id)."
            AND tran_date <= '$date'"; 
    }
        
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
	$sql = "SELECT (material_cost + labour_cost + overhead_cost) AS std_cost
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

function get_already_delivered($stock_id, $location, $trans_no)
{
	$sql = "SELECT ".TB_PREF."stock_moves.qty
		FROM ".TB_PREF."stock_moves
		WHERE ".TB_PREF."stock_moves.stock_id = ".db_escape($stock_id)."
		AND ".TB_PREF."stock_moves.loc_code = ".db_escape($location)."
		AND type=".ST_CUSTDELIVERY." AND trans_no=".db_escape($trans_no);
	$result = db_query($sql, "Could not get stock moves");
	$row = db_fetch_row($result);
	return $row[0];
}

function last_negative_stock_trans_id($stock_id, $to)
{
	$sql = "SELECT * from ".TB_PREF."stock_moves
		WHERE stock_id=".db_escape($stock_id)." 
		AND qty <> 0 order by trans_id asc";
	
	$result = db_query($sql, "The query on stock moves failed.");
	
	$qty = 0;
	$flag = 0;
	$negative_trans_id = -1;
	
	while ($myrow = db_fetch($result))
	{
		$qty += $myrow['qty'];
		if ($qty < 0 && $flag == 0)
		{
			$flag = 1;
			$negative_trans_id = $myrow['trans_id'];
		}
		if ($qty >= 0)
			$flag = 0;	
	}
		
	if ($flag == 1)
		return $negative_trans_id;
	else 
		return false;
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

function get_deliveries_from_trans($stock_id, $from)
{
	// -ve qty is delivery either by ST_CUSTDELIVERY or inventory adjustment
    //Price for GRN and SUPPCREDIT and std_cost for other trans_types
    $sql = "SELECT SUM(-qty), SUM(-qty*IF(type=".ST_SUPPRECEIVE." OR type=".ST_SUPPCREDIT.", price, standard_cost))
        FROM ".TB_PREF."stock_moves
        WHERE stock_id=".db_escape($stock_id)." AND qty < 0 AND
            trans_id>='$from' GROUP BY stock_id";
	$result = db_query($sql, "The deliveries could not be updated");
	$row = db_fetch_row($result);
	
    $sql = "SELECT IF(type=".ST_SUPPRECEIVE." OR type=".ST_SUPPCREDIT.", price, standard_cost)
        FROM ".TB_PREF."stock_moves
        WHERE stock_id=".db_escape($stock_id)
            ." AND trans_id ='$from'";
    $result = db_query($sql, "The deliveries could not be updated");
    $cost = db_fetch_row($result);
	
	// Adjusting QOH valuation 
	$sql = "SELECT SUM(qty) FROM ".TB_PREF."stock_moves
		WHERE stock_id=".db_escape($stock_id)." AND
			trans_id<'$from' GROUP BY stock_id";
	$result = db_query($sql, "The deliveries could not be updated");
	$qoh = db_fetch_row($result);
	
	$qty = $row[0] - $qoh[0]; //QOH prior to -ve stock is subtracted
	$final_cost = $row[1] - $qoh[0]*$cost[0];
	
	return array($qty,$final_cost); 
}

function get_purchases_from_trans($stock_id, $from)
{
	// Calculate All inward stock moves i.e. qty > 0
	$sql = "SELECT SUM(qty), SUM(qty*standard_cost) FROM ".TB_PREF."stock_moves
		WHERE stock_id=".db_escape($stock_id)." AND qty > 0 AND 
			trans_id>'$from' GROUP BY stock_id";
	$result = db_query($sql, "Could not get get_purchases_from_trans");
	$row = db_fetch_row($result);
	
	return $row;
}

//-------------------------------------------------------------------

function adjust_deliveries($stock_id, $material_cost, $to)
{
	if (!is_inventory_item($stock_id))
		return;
	
	$from = last_negative_stock_trans_id($stock_id, $to);
	if ($from == false || $from == -1)
		return;

	$row = get_deliveries_from_trans($stock_id, $from);
		
	if ($row == false)
		return;	
	$old_sales_cost = $row[1];
	$new_sales_cost = $row[0] * $material_cost;
	$sales_diff = $new_sales_cost - $old_sales_cost;
	
	$row = get_purchases_from_trans($stock_id, $from);
	$purchase_diff = 0;
	$old_purchase_cost = 0;
	if ($row != false)
	{
		$old_purchase_cost = $row[1];
		$new_purchase_cost = $row[0] * $material_cost;
		$purchase_diff = $new_purchase_cost - $old_purchase_cost;
	}
	
	$diff =  $sales_diff - $purchase_diff;
	
	if ($diff != 0)
	{
	   	$update_no = get_next_trans_no(ST_COSTUPDATE);
		if (!is_date_in_fiscalyear($to))
			$to = end_fiscalyear();
	   
		$stock_gl_code = get_stock_gl_code($stock_id);

		$dec = user_price_dec();
		$old_cost = -round2($old_sales_cost-$old_purchase_cost,$dec);
		$new_cost = -round2($new_sales_cost-$new_purchase_cost,$dec);

		$memo_ = sprintf(_("Cost was %s changed to %s x quantity on hand for item '%s'"),
			number_format2($old_cost, 2), number_format2($new_cost, 2), $stock_id);
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
	$sql = "SELECT mb_flag, inventory_account, cogs_account,
		adjustment_account, sales_account, assembly_account, dimension_id, dimension2_id FROM
		".TB_PREF."stock_master WHERE stock_id = ".db_escape($stock_id);

	$get = db_query($sql,"retreive stock gl code");
	return db_fetch($get);
}

//-----------------------------------------------------------------------------------------

function handle_negative_inventory($stock_id, $quantity, $standard_cost, $date_)
{
	//If negative adjustment result in negative or zero inventory
	//then difference should be adjusted
	$qoh = get_qoh_on_date($stock_id);

	if ($qoh + $quantity <= 0 && $qoh > 0) //Positive inventory turning zero/negative
	{
		global $Refs;

		$id = get_next_trans_no(ST_JOURNAL);
		$ref = $Refs->get_next(ST_JOURNAL);
		$diff = get_standard_cost($stock_id) - $standard_cost;
		
		if ($diff !=0)
		{
			$stock_gl_code = get_stock_gl_code($stock_id);
			$memo = _("Zero/negative inventory handling");
			//Reverse the inventory effect if $qoh <=0
			add_gl_trans_std_cost(ST_JOURNAL, $id, $date_, 
				$stock_gl_code["inventory_account"],
				$stock_gl_code['dimension_id'], $stock_gl_code['dimension2_id'], $memo, 
				-$qoh * $diff);
			//GL Posting to inventory adjustment account
			add_gl_trans_std_cost(ST_JOURNAL, $id, $date_, 
				$stock_gl_code["adjustment_account"],
				$stock_gl_code['dimension_id'], $stock_gl_code['dimension2_id'], $memo,
				$qoh * $diff);
				
			add_audit_trail(ST_JOURNAL, $id, $date_);
			add_comments(ST_JOURNAL, $id, $date_, $memo);
			$Refs->save(ST_JOURNAL, $id, $ref);	
		}
	}
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
	$date = date2sql($date_);

	$sql = "INSERT INTO ".TB_PREF."stock_moves (stock_id, trans_no, type, loc_code,
		tran_date, person_id, reference, qty, standard_cost, visible, price,
		discount_percent) VALUES (".db_escape($stock_id)
		.", ".db_escape($trans_no).", ".db_escape($type)
		.",	".db_escape($location).", '$date', "
		.db_escape($person_id).", ".db_escape($reference).", "
		.db_escape($quantity).", ".db_escape($std_cost).","
		.db_escape($show_or_hide).", "
		.db_escape($price).", ".db_escape($discount_percent).")";

	if ($error_msg == "")
		$error_msg = "The stock movement record cannot be inserted";

	db_query($sql, $error_msg);

	return db_insert_id();
}

function update_stock_move($type, $trans_no, $stock_id, $cost)
{
	$sql = "UPDATE ".TB_PREF."stock_moves SET standard_cost=".db_escape($cost)
			." WHERE type=".db_escape($type)
			."	AND trans_no=".db_escape($trans_no)
			."	AND stock_id=".db_escape($stock_id);
	db_query($sql, "The stock movement standard_cost cannot be updated");
}

function update_stock_move_pid($type, $type_no, $stock_id, $pid, $cost)
{
	$sql = "UPDATE ".TB_PREF."stock_moves SET standard_cost=".db_escape($cost)
		." WHERE type=".db_escape($type)
		."	AND trans_no=".db_escape($type_no)
		."	AND stock_id=".db_escape($stock_id)
		."  AND person_id = ".db_escape($pid);
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
    $sql = "SELECT * from ".TB_PREF."stock_moves WHERE type=".db_escape($type)." AND trans_no=".db_escape($type_no);
    $result = db_query($sql, "Could not void stock moves");
    while ($row = db_fetch($result))
    {
		//Skip cost averaging of service items
		if (is_inventory_item($row["stock_id"]))
		{
			// The cost has to be adjusted.
			// Transaction rates are stored either as price or standard_cost depending on types
			$types = array(ST_SUPPCREDIT, ST_SUPPRECEIVE);
			if (in_array($type,$types))
				$trans_rate = $row["price"];
			else
				$trans_rate = $row["standard_cost"];

			update_average_material_cost(0, $row["stock_id"],
				$trans_rate, -$row["qty"], sql2date($row["tran_date"]));
		}

    }
	$sql = "DELETE FROM ".TB_PREF."stock_moves WHERE type=".db_escape($type)
		."	AND trans_no=".db_escape($type_no);
	db_query($sql, "The stock movement cannot be delated");
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