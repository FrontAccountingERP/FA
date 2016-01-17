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

function add_work_order_issue($woid, $ref, $to_work_order, $items, $location, $workcentre,
	$date_, $memo_)
{
	global $Refs;

	begin_transaction();
	$args = func_get_args();
	$args = (object)array_combine(array('woid', 'ref', 'to_work_order', 'items', 'location', 
		'workcentre', 'date_', 'memo_'), $args);
	$args->trans_no = 0;
	hook_db_prewrite($args, ST_MANUISSUE);

	$details = get_work_order($woid);

    if (strlen($details[0]) == 0)
    {
    	echo _("The order number sent is not valid.");
    	cancel_transaction();
    	exit;
    }

	if (work_order_is_closed($woid))
	{
		display_error("UNEXPECTED : Issuing items for a closed Work Order");
		cancel_transaction();
		exit;
	}

	// insert the actual issue
	$sql = "INSERT INTO ".TB_PREF."wo_issues (workorder_id, reference, issue_date, loc_code, workcentre_id)
		VALUES (".db_escape($woid).", ".db_escape($ref).", '" .
		date2sql($date_) . "', ".db_escape($location).", ".db_escape($workcentre).")";
	db_query($sql,"The work order issue could not be added");

	$number = db_insert_id();

	$issue_total = $total_cost = 0;

	foreach ($items as $item)
	{

		if ($to_work_order)
			$item->quantity = -$item->quantity;

		//Chaitanya: Stamp the standard_cost
		$standard_cost = get_standard_cost($item->stock_id);
		// insert a -ve stock move for each item
		add_stock_move(ST_MANUISSUE, $item->stock_id, $number,
			$location, $date_, $memo_, -$item->quantity, $standard_cost);

		$sql = "INSERT INTO ".TB_PREF."wo_issue_items (issue_id, stock_id, qty_issued)
			VALUES (".db_escape($number).", ".db_escape($item->stock_id).", "
			.db_escape($item->quantity).")";
		db_query($sql,"A work order issue item could not be added");

		$standard_cost = get_standard_cost($item->stock_id);
		$issue_cost = $standard_cost * $item->quantity;
		$issue = get_stock_gl_code($item->stock_id);
        $stockitem = get_item($item->stock_id);
		
		//Chaitanya : Compatibility for Service Items
		if (!is_service($issue["mb_flag"]))
			$ivaccount = $issue["inventory_account"];
		else
			$ivaccount = $issue["cogs_account"];		
		
        $total_cost += add_gl_trans_std_cost(ST_WORKORDER, $woid, $date_, $ivaccount, 0, 0,
                $date_.": "._("Issue of")." ".$stockitem["description"], -$issue_cost);			
		$issue_total += $issue_cost;
	}	
	if ($issue_total != 0)
		//Chaitanya : Apply cost to QOH as adjustment only
		add_issue_cost($details['stock_id'], $details['units_reqd'], $date_, $issue_total, true);
	$issue = get_stock_gl_code($details['stock_id']);
    $stockitem = get_item($details['stock_id']);
    add_gl_trans_std_cost(ST_WORKORDER, $woid, $date_, $issue["inventory_account"],
        0, 0, $date_.": "._("Issue to")." ".$stockitem["description"], -$total_cost);	

	if ($memo_)
		add_comments(ST_MANUISSUE, $number, $date_, $memo_);

	$Refs->save(ST_MANUISSUE, $number, $ref);
	add_audit_trail(ST_MANUISSUE, $number, $date_);

	$args->trans_no = $number;
	hook_db_postwrite($args, ST_MANUISSUE);
	commit_transaction();
}

//--------------------------------------------------------------------------------------

function get_work_order_issues($woid)
{
	$sql = "SELECT * FROM ".TB_PREF."wo_issues WHERE workorder_id=".db_escape($woid)
	." ORDER BY issue_no";
    return db_query($sql, "The work order issues could not be retrieved");
}

function get_additional_issues($woid)
{
	$sql = "SELECT ".TB_PREF."wo_issues.*, ".TB_PREF."wo_issue_items.*
		FROM ".TB_PREF."wo_issues, ".TB_PREF."wo_issue_items
		WHERE ".TB_PREF."wo_issues.issue_no=".TB_PREF."wo_issue_items.issue_id
		AND ".TB_PREF."wo_issues.workorder_id=".db_escape($woid)
		." ORDER BY ".TB_PREF."wo_issue_items.id";
    return db_query($sql, "The work order issues could not be retrieved");
}
//--------------------------------------------------------------------------------------

function get_work_order_issue($issue_no)
{
	$sql = "SELECT DISTINCT ".TB_PREF."wo_issues.*, ".TB_PREF."workorders.stock_id,
		".TB_PREF."stock_master.description, ".TB_PREF."locations.location_name, "
		.TB_PREF."workcentres.name AS WorkCentreName
		FROM ".TB_PREF."wo_issues, ".TB_PREF."workorders, ".TB_PREF."stock_master, "
		.TB_PREF."locations, ".TB_PREF."workcentres
		WHERE issue_no=".db_escape($issue_no)."
		AND ".TB_PREF."workorders.id = ".TB_PREF."wo_issues.workorder_id
		AND ".TB_PREF."locations.loc_code = ".TB_PREF."wo_issues.loc_code
		AND ".TB_PREF."workcentres.id = ".TB_PREF."wo_issues.workcentre_id
		AND ".TB_PREF."stock_master.stock_id = ".TB_PREF."workorders.stock_id";
    $result = db_query($sql, "A work order issue could not be retrieved");

    return db_fetch($result);
}

//--------------------------------------------------------------------------------------

function get_work_order_issue_details($issue_no)
{
	$sql = "SELECT ".TB_PREF."wo_issue_items.*,"
	.TB_PREF."stock_master.description, ".TB_PREF."stock_master.units
		FROM ".TB_PREF."wo_issue_items, ".TB_PREF."stock_master
		WHERE issue_id=".db_escape($issue_no)."
		AND ".TB_PREF."stock_master.stock_id=".TB_PREF."wo_issue_items.stock_id
		ORDER BY ".TB_PREF."wo_issue_items.id";
    return db_query($sql, "The work order issue items could not be retrieved");
}

//--------------------------------------------------------------------------------------

function exists_work_order_issue($issue_no)
{
	$sql = "SELECT issue_no FROM ".TB_PREF."wo_issues WHERE issue_no=".db_escape($issue_no);
	$result = db_query($sql, "Cannot retreive a wo issue");

    return (db_num_rows($result) > 0);
}

//--------------------------------------------------------------------------------------

function void_work_order_issue($type_no)
{
	begin_transaction();
	hook_db_prevoid(ST_MANUISSUE, $type_no);
	
	//Chaitanya : Skip processing already voided entry i.e. explicitly voided
	$void_entry = get_voided_entry(ST_MANUISSUE, $type_no);
	if ($void_entry)
		return;

	// void the actual issue items and their quantities
	$sql = "UPDATE ".TB_PREF."wo_issue_items Set qty_issued = 0 WHERE issue_id="
		.db_escape($type_no);
	db_query($sql,"A work order issue item could not be voided");

	// void any related gl trans
	//Chaitanya : Nothing happens due to next statement as all gl postings are done against WO
	//void_gl_trans(ST_MANUISSUE, $type_no, true);
	
	//Chaitanya : Reverse the gl posting
	$issue = get_work_order_issue($type_no);
	$manf_stock_id = $issue["stock_id"];
	$date_ = sql2date($issue["issue_date"]);
	$woid = $issue["workorder_id"];
		
	$result = get_stock_moves(ST_MANUISSUE, $type_no);
	$total_cost = 0;
	$issue_total = 0;
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
			$total_cost += add_gl_trans_std_cost(ST_WORKORDER, $woid, $date_, $ivaccount, 0, 0,
				$date_.": "._("Reversed the issue of")." ".$stockitem["description"],
				-$issue_cost);			
			$issue_total += $issue_cost;
		}
	}
	if ($issue_total != 0)
		//Chaitanya : Revese cost effect on manfactured stock item as adjustment only
		add_issue_cost($manf_stock_id, 0, $date_, $issue_total, true);
	$issue = get_stock_gl_code($manf_stock_id);
    $stockitem = get_item($manf_stock_id);
	if ($total_cost != 0)
		add_gl_trans_std_cost(ST_WORKORDER, $woid, $date_, $issue["inventory_account"],
			0, 0, $date_.": "._("Reversed the issue to")." ".$stockitem["description"], 
			-$total_cost);	
	
	//Chaitanya : Shifted below void all related stock moves
	void_stock_move(ST_MANUISSUE, $type_no);
	
	commit_transaction();
}


//--------------------------------------------------------------------------------------

?>