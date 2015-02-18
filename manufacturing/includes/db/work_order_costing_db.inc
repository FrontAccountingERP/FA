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

function add_wo_costing($workorder_id, $cost_type, $trans_type, $trans_no, $factor=1)
{
	$sql = "INSERT INTO ".TB_PREF."wo_costing (workorder_id, cost_type, trans_type, trans_no, factor)
		VALUES (".db_escape($workorder_id).","
		.db_escape($cost_type).",".db_escape($trans_type).","
		.db_escape($trans_no).",".db_escape($factor).")";

	db_query($sql, "could not add work order costing");

}

function get_wo_costing($workorder_id)
{
	$sql="SELECT * FROM ".TB_PREF."wo_costing WHERE workorder_id=".db_escape($workorder_id);

	return db_query($sql, "could not get work order costing");
}

function delete_wo_costing($trans_type, $trans_no)
{
	$sql="DELETE FROM ".TB_PREF."wo_costing WHERE trans_type=".db_escape($trans_type)
		." AND trans_no=".db_escape($trans_no);

	db_query($sql, "could not delete work order costing");
}
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
		if ($qoh == 0)
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
			$ref = $Refs->get_next(ST_JOURNAL, null, $date_);

			$stock_gl_code = get_stock_gl_code($stock_id);
			add_journal(ST_JOURNAL, $id, $costs, $date_, get_company_currency(), $ref);
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
			if ($qty != 0)
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
			$ref = $Refs->get_next(ST_JOURNAL, null, $date_);
			add_journal(ST_JOURNAL, $id, $costs, $date_, get_company_currency(), $ref);

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
			if ($qty != 0)
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
			$ref = $Refs->get_next(ST_JOURNAL, null, $date_);
			add_journal(ST_JOURNAL, $id, $costs, $date_, get_company_currency(), $ref);
			
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
			if ($qty != 0)
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

/*
	Create journal entry for WO related costs.
*/
function add_wo_costs_journal($wo_id, $amount, $cost_type, $cr_acc, $db_acc, $date, $dim1=0, $dim2=0, $memo = null, $ref= null)
{
//-------- this should be done by single call to write_journal_entries() using items_cart()
//
	global $Refs, $wo_cost_types;

	begin_transaction();

    $journal_id = get_next_trans_no(ST_JOURNAL);
    if (!$ref) $ref = $Refs->get_next(ST_JOURNAL, null, $date);

	add_gl_trans_std_cost(ST_JOURNAL, $journal_id, $date, $cr_acc,
		0, 0, $wo_cost_types[$cost_type], -$amount);
	$is_bank_to = is_bank_account($cr_acc);
	if ($is_bank_to)
	{
		add_bank_trans(ST_JOURNAL, $journal_id, $is_bank_to, "",
			$date, -$amount, PT_WORKORDER, $wo_id, get_company_currency(),
			"Cannot insert a destination bank transaction");
	} else
		add_journal(ST_JOURNAL, $journal_id, $amount, $date, get_company_currency(), $ref);

	add_gl_trans_std_cost(ST_JOURNAL, $journal_id, $date, $db_acc,
		$dim1, $dim2,  $wo_cost_types[$cost_type], $amount);

	$Refs->save(ST_JOURNAL, $journal_id, $ref);

	add_wo_costing($wo_id, $cost_type, ST_JOURNAL, $journal_id);

	add_comments(ST_JOURNAL, $journal_id, $date, $memo);
	add_audit_trail(ST_JOURNAL, $journal_id, $date);

	commit_transaction();
}

function void_wo_costing($wo_id)
{
	$res = get_wo_costing($wo_id);

	while($row = db_fetch($res))
	{
		// void any related gl trans
		void_gl_trans($row['trans_type'], $row['trans_no'], true);
	}

	$sql = "DELETE FROM ".TB_PREF."wo_costing WHERE workorder_id=".db_escape($wo_id);
	db_query($sql, "could not delete work order costing");
}
