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

function add_material_cost($stock_id, $qty, $date_, $advanced=false)
{
	$m_cost = 0;
    $result = get_bom($stock_id);
	while ($bom_item = db_fetch($result))
	{
		$standard_cost = get_standard_cost($bom_item['component']);
		$m_cost += ($bom_item['quantity'] * $standard_cost);
	}
	$bom_cost = $m_cost;
	
	$sql = "SELECT material_cost, labour_cost, overhead_cost FROM ".TB_PREF."stock_master WHERE stock_id = "
		.db_escape($stock_id);
	$result = db_query($sql);
	$myrow = db_fetch($result);
	$material_cost =  $myrow['material_cost'];
	
	if ($advanced)
	{
		//reduce overhead_cost and labour_cost from price as those will remain as is
		$m_cost = $m_cost - $myrow['labour_cost'] - $myrow['overhead_cost'];
	}
	
	$qoh = get_qoh_on_date($stock_id);
	$cost_adjust = false;
	if ($qoh < 0)
	{
		if ($qoh + $qty >= 0)
			$cost_adjust = true;
		$qoh = 0;
	}		
	if ($qoh + $qty != 0)
	{
		if ($qoh == 0) // 27.10.2014 apmuthu and dz.
			$material_cost += $m_cost;
		else	
			$material_cost = ($qoh * $material_cost + $qty * $m_cost) /	($qoh + $qty);
	}
	if ($advanced && $cost_adjust) // new 2010-02-10
		adjust_deliveries($stock_id, $bom_cost, $date_);	
	
	$sql = "UPDATE ".TB_PREF."stock_master SET material_cost=".db_escape($material_cost)."
		    WHERE stock_id=".db_escape($stock_id);
	db_query($sql,"The cost details for the inventory item could not be updated");
}

function add_overhead_cost($stock_id, $qty, $date_, $costs, $adj_only=false)
{
	if ($qty != 0)
		$costs /= $qty;
	$sql = "SELECT overhead_cost FROM ".TB_PREF."stock_master WHERE stock_id = "
		.db_escape($stock_id);
	$result = db_query($sql);
	$myrow = db_fetch($result);
	$overhead_cost =  $myrow['overhead_cost'];
	$qoh = get_qoh_on_date($stock_id);
	if ($qoh < 0)
		$qoh = 0;
	if ($adj_only)
	{
		if ($qty != 0)
			$costs = $qty * $costs;
		if ($qoh>0)
			$overhead_cost = ($qoh * $overhead_cost + $costs) / $qoh;
		else // Journal Entry if QOH is 0/negative 
		{
			global $Refs;

			$id = get_next_trans_no(ST_JOURNAL);
			$ref = $Refs->get_next(ST_JOURNAL);
			
			$stock_gl_code = get_stock_gl_code($stock_id);
			$memo = "WO Overhead cost settlement JV for zero/negative respository of ".$stock_id;
			//Reverse the inventory effect if $qoh <=0
			add_gl_trans_std_cost(ST_JOURNAL, $id, $date_, 
				$stock_gl_code["inventory_account"],
				$stock_gl_code['dimension_id'], $stock_gl_code['dimension2_id'], $memo, 
				-$costs);
			//GL Posting to inventory adjustment account
			add_gl_trans_std_cost(ST_JOURNAL, $id, $date_, 
				$stock_gl_code["adjustment_account"],
				$stock_gl_code['dimension_id'], $stock_gl_code['dimension2_id'], $memo,
				$costs);
				
			add_audit_trail(ST_JOURNAL, $id, $date_);
			add_comments(ST_JOURNAL, $id, $date_, $memo);
			$Refs->save(ST_JOURNAL, $id, $ref);
			if ($qty != 0) // 27.10.2014 dz
				$overhead_cost = ($qoh * $overhead_cost + $costs) / $qty;
		}
	}
	else
	{		
		if ($qoh + $qty != 0)	
			$overhead_cost = ($qoh * $overhead_cost + $qty * $costs) /	($qoh + $qty);
	}
	$sql = "UPDATE ".TB_PREF."stock_master SET overhead_cost=".db_escape($overhead_cost)."
		WHERE stock_id=".db_escape($stock_id);
	db_query($sql,"The cost details for the inventory item could not be updated");
}

function add_labour_cost($stock_id, $qty, $date_, $costs, $adj_only=false)
{
	if ($qty != 0)
		$costs /= $qty;
	$sql = "SELECT labour_cost FROM ".TB_PREF."stock_master WHERE stock_id = "
		.db_escape($stock_id);
	$result = db_query($sql);
	$myrow = db_fetch($result);
	$labour_cost =  $myrow['labour_cost'];
	$qoh = get_qoh_on_date($stock_id);
	if ($qoh < 0)
		$qoh = 0;
	if ($adj_only)
	{
		if ($qty != 0)
			$costs = $qty * $costs;
		if ($qoh>0)
			$labour_cost = ($qoh * $labour_cost + $costs) / $qoh;	
		else // Journal Entry if QOH is 0/negative 
		{
			global $Refs;

			$id = get_next_trans_no(ST_JOURNAL);
			$ref = $Refs->get_next(ST_JOURNAL);
			
			$stock_gl_code = get_stock_gl_code($stock_id);
			$memo = "WO labour cost settlement JV for zero/negative respository of ".$stock_id;
			//Reverse the inventory effect if $qoh <=0
			add_gl_trans_std_cost(ST_JOURNAL, $id, $date_, 
				$stock_gl_code["inventory_account"],
				$stock_gl_code['dimension_id'], $stock_gl_code['dimension2_id'], $memo, 
				-$costs);
			//GL Posting to inventory adjustment account
			add_gl_trans_std_cost(ST_JOURNAL, $id, $date_, 
				$stock_gl_code["adjustment_account"],
				$stock_gl_code['dimension_id'], $stock_gl_code['dimension2_id'], $memo,
				$costs);
				
			add_audit_trail(ST_JOURNAL, $id, $date_);
			add_comments(ST_JOURNAL, $id, $date_, $memo);
			$Refs->save(ST_JOURNAL, $id, $ref);	
			if ($qty != 0) // 27.10.2014 dz
				$labour_cost = ($qoh * $labour_cost + $costs) / $qty;
		}
	}
	else
	{		
		if ($qoh + $qty != 0)	
			$labour_cost = ($qoh * $labour_cost + $qty * $costs) /	($qoh + $qty);
	}	
	$sql = "UPDATE ".TB_PREF."stock_master SET labour_cost=".db_escape($labour_cost)."
		WHERE stock_id=".db_escape($stock_id);
	db_query($sql,"The cost details for the inventory item could not be updated");
}

function add_issue_cost($stock_id, $qty, $date_, $costs, $adj_only=false)
{
	if ($qty != 0)
		$costs /= $qty;
	$sql = "SELECT material_cost FROM ".TB_PREF."stock_master WHERE stock_id = "
		.db_escape($stock_id);
	$result = db_query($sql);
	$myrow = db_fetch($result);
	$material_cost =  $myrow['material_cost'];
	$qoh = get_qoh_on_date($stock_id);
	if ($qoh < 0)
		$qoh = 0;
	if ($adj_only)
	{
		if ($qty != 0)
			$costs = $qty * $costs;
		if ($qoh>0)
			$material_cost = $costs / $qoh;
		else // Journal Entry if QOH is 0/negative
		{
			global $Refs;

			$id = get_next_trans_no(ST_JOURNAL);
			$ref = $Refs->get_next(ST_JOURNAL);
			
			$stock_gl_code = get_stock_gl_code($stock_id);
			$memo = "WO Issue settlement JV for zero/negative respository of ".$stock_id;
			//Reverse the inventory effect if $qoh <=0
			add_gl_trans_std_cost(ST_JOURNAL, $id, $date_, 
				$stock_gl_code["inventory_account"],
				$stock_gl_code['dimension_id'], $stock_gl_code['dimension2_id'], $memo, 
				-$costs);
			//GL Posting to inventory adjustment account
			add_gl_trans_std_cost(ST_JOURNAL, $id, $date_, 
				$stock_gl_code["adjustment_account"],
				$stock_gl_code['dimension_id'], $stock_gl_code['dimension2_id'], $memo,
				$costs);
				
			add_audit_trail(ST_JOURNAL, $id, $date_);
			add_comments(ST_JOURNAL, $id, $date_, $memo);
			$Refs->save(ST_JOURNAL, $id, $ref);
			if ($qty != 0) // 27.10.2014 dz
			 	$material_cost = $costs / $qty; 
		}
	}
	else
	{
		if ($qoh + $qty != 0)	
			$material_cost = ($qty * $costs) /	($qoh + $qty);
	}	
	$sql = "UPDATE ".TB_PREF."stock_master SET material_cost=material_cost+"
		.db_escape($material_cost)
		." WHERE stock_id=".db_escape($stock_id);
	db_query($sql,"The cost details for the inventory item could not be updated");
}

function add_work_order($wo_ref, $loc_code, $units_reqd, $stock_id,
	$type, $date_, $required_by, $memo_, $costs, $cr_acc, $labour, $cr_lab_acc)
{
	global $Refs;

	if (!($type == WO_ADVANCED))
		return add_work_order_quick($wo_ref, $loc_code, $units_reqd, $stock_id, $type, $date_, $memo_, $costs, $cr_acc, $labour, $cr_lab_acc);

	begin_transaction();
	$args = func_get_args();
	$args = (object)array_combine(array('wo_ref', 'loc_code', 'units_reqd', 'stock_id',
		'type', 'date_', 'required_by', 'memo_', 'costs', 'cr_acc', 'labour', 'cr_lab_acc'), $args);
	$args->woid = 0;
	hook_db_prewrite($args, ST_WORKORDER);

	$date = date2sql($date_);
	$required = date2sql($required_by);

	$sql = "INSERT INTO ".TB_PREF."workorders (wo_ref, loc_code, units_reqd, stock_id,
		type, date_, required_by)
    	VALUES (".db_escape($wo_ref).", ".db_escape($loc_code).", "
    	.db_escape($units_reqd).", ".db_escape($stock_id).",
		".db_escape($type).", '$date', ".db_escape($required).")";
	db_query($sql, "could not add work order");

	$woid = db_insert_id();

	add_comments(ST_WORKORDER, $woid, $required_by, $memo_);

	$Refs->save(ST_WORKORDER, $woid, $wo_ref);
	add_audit_trail(ST_WORKORDER, $woid, $date_);

	$args->woid = $woid;
	hook_db_postwrite($args, ST_WORKORDER);
	commit_transaction();

	return $woid;
}

//--------------------------------------------------------------------------------------

function update_work_order($woid, $loc_code, $units_reqd, $stock_id,
					$date_, $required_by, $memo_)
{
	begin_transaction();
	$args = func_get_args();
	$args = (object)array_combine(array('woid', 'loc_code', 'units_reqd', 'stock_id',
		'date_', 'required_by', 'memo_'), $args);
	hook_db_prewrite($args, ST_WORKORDER);

	$date = date2sql($date_);
	$required = date2sql($required_by);

	$sql = "UPDATE ".TB_PREF."workorders SET loc_code=".db_escape($loc_code).",
		units_reqd=".db_escape($units_reqd).", stock_id=".db_escape($stock_id).",
		required_by=".db_escape($required).",
		date_='$date'
		WHERE id = ".db_escape($woid);

	db_query($sql, "could not update work order");

	update_comments(ST_WORKORDER, $woid, null, $memo_);
	add_audit_trail(ST_WORKORDER, $woid, $date_, _("Updated."));

	hook_db_postwrite($args, ST_WORKORDER);
	commit_transaction();
}

function delete_work_order($woid)
{
	begin_transaction();
	hook_db_prevoid(ST_WORKORDER, $woid);

	// delete the work order requirements
	delete_wo_requirements($woid);

	// delete the actual work order
	$sql = "DELETE FROM ".TB_PREF."workorders WHERE id=".db_escape($woid);
	db_query($sql,"The work order could not be deleted");

	delete_comments(ST_WORKORDER, $woid);
	add_audit_trail(ST_WORKORDER, $woid, $_POST['date_'], _("Canceled."));

	commit_transaction();
}

//--------------------------------------------------------------------------------------

function get_work_order($woid, $allow_null=false)
{
    $sql = "SELECT wo.*,st.description As StockItemName,l.location_name,
    		l.delivery_address,l.email, l.contact
		FROM ".TB_PREF."workorders wo, ".TB_PREF."stock_master st, ".TB_PREF."locations l
		WHERE st.stock_id=wo.stock_id
		AND	l.loc_code=wo.loc_code
		AND wo.id=".db_escape($woid)."
		GROUP BY wo.id";

	$result = db_query($sql, "The work order issues could not be retrieved");

	if (!$allow_null && db_num_rows($result) == 0)
		display_db_error("Could not find work order $woid", $sql);

	return db_fetch($result);
}

//--------------------------------------------------------------------------------------

function work_order_has_productions($woid)
{
	$sql = "SELECT COUNT(*) FROM ".TB_PREF."wo_manufacture WHERE workorder_id=".db_escape($woid);
	$result = db_query($sql, "query work order for productions");

	$myrow = db_fetch_row($result);
	return ($myrow[0] > 0);
}


//--------------------------------------------------------------------------------------

function work_order_has_issues($woid)
{
	$sql = "SELECT COUNT(*) FROM ".TB_PREF."wo_issues WHERE workorder_id=".db_escape($woid);
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
		released=1 WHERE id = ".db_escape($woid);
	db_query($sql, "could not release work order");

	// create Work Order Requirements based on the bom
	create_wo_requirements($woid, $stock_id);

	add_comments(ST_WORKORDER, $woid, $releaseDate, $memo_);
	add_audit_trail(ST_WORKORDER, $woid, $releaseDate,_("Released."));

	commit_transaction();
}

//--------------------------------------------------------------------------------------

function close_work_order($woid)
{
	$sql = "UPDATE ".TB_PREF."workorders SET closed=1 WHERE id = ".db_escape($woid);
	db_query($sql, "could not close work order");
}

//--------------------------------------------------------------------------------------

function work_order_is_closed($woid)
{
	$sql = "SELECT closed FROM ".TB_PREF."workorders WHERE id = ".db_escape($woid);
	$result = db_query($sql, "could not query work order");
	$row = db_fetch_row($result);
	return ($row[0] > 0);
}

//--------------------------------------------------------------------------------------

function work_order_update_finished_quantity($woid, $quantity, $force_close=0)
{
	$sql = "UPDATE ".TB_PREF."workorders SET units_issued = units_issued + ".db_escape($quantity).",
		closed = ((units_issued >= units_reqd) OR ".db_escape($force_close).")
		WHERE id = ".db_escape($woid);

	db_query($sql, "The work order issued quantity couldn't be updated");
}

//--------------------------------------------------------------------------------------

function void_work_order($woid)
{
	begin_transaction();
	hook_db_prevoid(ST_WORKORDER, $woid);

	$work_order = get_work_order($woid);
	if (!($work_order["type"] == WO_ADVANCED))
	{
		$sql = "UPDATE ".TB_PREF."workorders SET closed=1,units_reqd=0,units_issued=0 WHERE id = "
			.db_escape($woid);
		db_query($sql, "The work order couldn't be voided");

		// void all related stock moves
		void_stock_move(ST_WORKORDER, $woid);

		// void any related bank/gl trans
		void_bank_trans(ST_WORKORDER, $woid, true);

		// clear the requirements units received
		void_wo_requirements($woid);
	}
	else
	{
		// void everything inside the work order : issues, productions, payments
		$date = sql2date($work_order['date_']);
		
		$result = get_work_order_productions($woid); // check the produced quantity
		$qty = 0;
		while ($row = db_fetch($result))
		{
			void_work_order_produce($row['id']);
			
			//Post voided entry if not prevoided explicitly
			$void_entry = get_voided_entry(ST_MANURECEIVE, $row['id']);
			if ($void_entry)
				continue;
			$memo_ = _("Voiding Work Order Trans # ").$woid;
			add_audit_trail(ST_MANURECEIVE, $row['id'], today(), _("Voided.")."\n".$memo_);
			add_voided_entry(ST_MANURECEIVE, $row['id'], today(), $memo_);
		}

		$result = get_work_order_issues($woid);
		$cost = 0;
		$issue_no = 0;
		while ($row = db_fetch($result))
		{
			void_work_order_issue($row['issue_no']);
			
			//Post voided entry if not prevoided explicitly
			$void_entry = get_voided_entry(ST_MANUISSUE, $row['issue_no']);
			if ($void_entry)
				continue;
			$memo_ = _("Voiding Work Order Trans # ").$woid;
			add_audit_trail(ST_MANUISSUE, $row['issue_no'], today(), _("Voided.")."\n".$memo_);
			add_voided_entry(ST_MANUISSUE, $row['issue_no'], today(), $memo_);				
		}

		//Adust avg labour cost
		$cost = get_gl_wo_cost($woid, WO_LABOUR); 
		if ($cost != 0)
			add_labour_cost($work_order['stock_id'], 1, $date, -$cost, true);
			
		//Adust avg overhead cost
		$cost = get_gl_wo_cost($woid, WO_OVERHEAD); 
		if ($cost != 0)
			add_overhead_cost($work_order['stock_id'], 1, $date, -$cost, true);
		
		$sql = "UPDATE ".TB_PREF."workorders SET closed=1,units_reqd=0,units_issued=0 WHERE id = "
			.db_escape($woid);
		db_query($sql, "The work order couldn't be voided");

		// void all related stock moves
		void_stock_move(ST_WORKORDER, $woid);

		// void any related bank/gl trans
		void_bank_trans(ST_WORKORDER, $woid, true);

		// clear the requirements units received
		void_wo_requirements($woid);
	}
	commit_transaction();
}

function get_sql_for_work_orders($outstanding_only, $all_items)
{
	$sql = "SELECT
		workorder.id,
		workorder.wo_ref,
		workorder.type,
		location.location_name,
		item.description,
		workorder.units_reqd,
		workorder.units_issued,
		workorder.date_,
		workorder.required_by,
		workorder.released_date,
		workorder.closed,
		workorder.released,
		workorder.stock_id,
		unit.decimals
		FROM ".TB_PREF."workorders as workorder,"
			.TB_PREF."stock_master as item,"
			.TB_PREF."item_units as unit,"
			.TB_PREF."locations as location
		WHERE workorder.stock_id=item.stock_id 
			AND workorder.loc_code=location.loc_code
			AND item.units=unit.abbr";

	if (check_value('OpenOnly') || $outstanding_only != 0)
	{
		$sql .= " AND workorder.closed=0";
	}

	if (isset($_POST['StockLocation']) && $_POST['StockLocation'] != $all_items)
	{
		$sql .= " AND workorder.loc_code=".db_escape($_POST['StockLocation']);
	}

	if (isset($_POST['OrderId']) && $_POST['OrderId'] != "")
	{
		$sql .= " AND workorder.id LIKE ".db_escape('%'.$_POST['OrderId'].'%');
	}

	if (isset($_POST['OrderNumber']) && $_POST['OrderNumber'] != "")
	{
		$sql .= " AND workorder.wo_ref LIKE ".db_escape('%'.$_POST['OrderNumber'].'%');
	}

	if (isset($_POST['SelectedStockItem']) && $_POST['SelectedStockItem'] != $all_items)
	{
		$sql .= " AND workorder.stock_id=".db_escape($_POST['SelectedStockItem']);
	}

	if (check_value('OverdueOnly'))
	{
		$Today = date2sql(Today());

		$sql .= " AND workorder.required_by < '$Today' ";
	}
	return $sql;
}

function get_sql_for_where_used()
{
	$sql = "SELECT 
			bom.parent,
			workcentre.name As WorkCentreName,
			location.location_name,
			bom.quantity,
			parent.description
			FROM ".TB_PREF."bom as bom, "
				.TB_PREF."stock_master as parent, "
				.TB_PREF."workcentres as workcentre, "
				.TB_PREF."locations as location
			WHERE bom.parent = parent.stock_id 
				AND bom.workcentre_added = workcentre.id
				AND bom.loc_code = location.loc_code
				AND bom.component=".db_escape($_POST['stock_id']);
	return $sql;			
}
//--------------------------------------------------------------------------------------
function get_gl_wo_cost($woid, $cost_type)
{
	$cost = 0;
	$result = get_gl_wo_cost_trans($woid, $cost_type);
	while ($row = db_fetch($result))
		$cost += -$row['amount'];
	return $cost;	
}

?>