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
//-------------------------------------------------------------------------------------------------------------

function stock_cost_update($stock_id, $material_cost, $labour_cost, $overhead_cost,
	$last_cost)
{
	$mb_flag = get_mb_flag($stock_id);
    
	$update_no = -1;

    if (is_service($mb_flag))
    {
    	//display_db_error("Cannot do cost update for Service item : $stock_id", "");	
		
		//Chaitanya
		$sql = "UPDATE ".TB_PREF."stock_master SET material_cost=".db_escape($material_cost)."
		WHERE stock_id=".db_escape($stock_id);
		
		db_query($sql,"The cost details for the inventory item could not be updated");
		
		return $update_no;
		
    }		
	
	begin_transaction();
	
	$sql = "UPDATE ".TB_PREF."stock_master SET material_cost=".db_escape($material_cost).", 
		labour_cost=".db_escape($labour_cost).", 
		overhead_cost=".db_escape($overhead_cost).", 
		last_cost=".db_escape($last_cost)." 
		WHERE stock_id=".db_escape($stock_id);
	db_query($sql,"The cost details for the inventory item could not be updated");

	$qoh = get_qoh_on_date($stock_id);
	
   	$date_ = Today();
	if (!is_date_in_fiscalyear($date_))
		$date_ = end_fiscalyear();

	if ($qoh > 0)
	{
		$new_cost = $material_cost + $labour_cost + $overhead_cost;

		$value_of_change = round2($qoh * ($new_cost - $last_cost), user_price_dec());

		if ($value_of_change != 0)
		{
			$stock_gl_code = get_stock_gl_code($stock_id);
	   		$update_no = get_next_trans_no(ST_COSTUPDATE);
			$memo_ = sprintf(_("Cost was %s changed to %s x quantity on hand for item '%s'"),
				number_format2($last_cost, 2), number_format2($new_cost, 2), $stock_id);
			add_gl_trans_std_cost(ST_COSTUPDATE, $update_no, $date_, $stock_gl_code["adjustment_account"], 
				$stock_gl_code["dimension_id"], $stock_gl_code["dimension2_id"], $memo_, (-$value_of_change));	   

			add_gl_trans_std_cost(ST_COSTUPDATE, $update_no, $date_, $stock_gl_code["inventory_account"], 0, 0, $memo_, 
				$value_of_change);
			change_stock_moves_std_cost($stock_id, $date_, $new_cost - $last_cost);	
		}		
   	}

	if ($update_no != -1)
		add_audit_trail(ST_COSTUPDATE, $update_no, $date_);
	commit_transaction();

	return $update_no;
}

//-------------------------------------------------------------------------------------------------------------

function change_stock_moves_std_cost($stock_id, $date, $diff_cost)
{
	$date = date2sql($date);
	$sql = "UPDATE ".TB_PREF."stock_moves SET standard_cost = standard_cost + ".db_escape($diff_cost). " WHERE stock_id = "
	.	db_escape($stock_id)." AND tran_date <= '$date' AND qty <> 0 AND standard_cost > 0.001 AND type <> ".ST_LOCTRANSFER;
	db_query($sql,"The stock moves cost details for the inventory item could not be updated");
}
?>