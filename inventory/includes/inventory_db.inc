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
include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/banking.inc");
include_once($path_to_root . "/includes/inventory.inc");

include_once($path_to_root . "/inventory/includes/db/items_category_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_prices_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_purchases_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_codes_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_locations_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_adjust_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_transfer_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_units_db.inc");

function item_img_name($stock_id)
{
	$stock_id = strtr($stock_id, "><\\/:|*?", '________');
	return clean_file_name($stock_id);
}

function get_stock_movements($stock_id, $StockLocation,	$BeforeDate, $AfterDate)
{
	$before_date = date2sql($BeforeDate);
	$after_date = date2sql($AfterDate);
	// PO Delivery and Customer Credit Notes references should be saved in stock moves reference in 2.5
  	$sql = "SELECT move.*, IF(ISNULL(supplier.supplier_id), debtor.name, supplier.supp_name) name,
		IF(move.type=".ST_SUPPRECEIVE.", grn.reference, IF(move.type=".ST_CUSTCREDIT.", cust_trans.reference, move.reference)) reference";

	if(!$StockLocation) {
		 $sql .= ", move.loc_code";
	}
  	$sql.=    " FROM ".TB_PREF."stock_moves move
				LEFT JOIN ".TB_PREF."supp_trans credit ON credit.trans_no=move.trans_no AND credit.type=move.type
				LEFT JOIN ".TB_PREF."grn_batch grn ON grn.id=move.trans_no AND move.type=".ST_SUPPRECEIVE."
				LEFT JOIN ".TB_PREF."suppliers supplier ON IFNULL(grn.supplier_id, credit.supplier_id)=supplier.supplier_id
				LEFT JOIN ".TB_PREF."debtor_trans cust_trans ON cust_trans.trans_no=move.trans_no AND cust_trans.type=move.type
				LEFT JOIN ".TB_PREF."debtors_master debtor ON cust_trans.debtor_no=debtor.debtor_no
		WHERE";

  	if ($StockLocation) {
    	$sql.= " move.loc_code=".db_escape($StockLocation)." AND";
	}

	$sql.= " move.tran_date >= '". $after_date . "'
		AND move.tran_date <= '" . $before_date . "'
		AND move.stock_id = ".db_escape($stock_id) . " ORDER BY move.tran_date, move.trans_id";

  	return db_query($sql, "could not query stock moves");
}

function calculate_reorder_level($location, $line, &$st_ids, &$st_names, &$st_num, &$st_reorder)
{
	$sql = "SELECT stock.*, loc.location_name, loc.email
		FROM ".TB_PREF."loc_stock stock,"
			.TB_PREF."locations loc
		WHERE stock.loc_code=loc.loc_code
		AND stock.stock_id = '" . $line->stock_id . "'
		AND stock.loc_code = '" . $location . "'";
	$res = db_query($sql,"a location could not be retreived");
	$loc = db_fetch($res);
	if ($loc['email'] != "")
	{
		$qoh = get_qoh_on_date($line->stock_id, $location);
		$qoh -= get_demand_qty($line->stock_id, $location);
		$qoh -= get_demand_asm_qty($line->stock_id, $location);
		$qoh -= $line->quantity;
		if ($qoh < $loc['reorder_level'])
		{
			$st_ids[] = $line->stock_id;
			$st_names[] = $line->item_description;
			$st_num[] = $qoh - $loc['reorder_level'];
			$st_reorder[] = $loc['reorder_level'];
		}
	}
	return $loc;
}

function send_reorder_email($loc, $st_ids, $st_names, $st_num, $st_reorder)
{
	global $path_to_root;

	require_once($path_to_root . "/reporting/includes/class.mail.inc");
	$company = get_company_prefs();
	$mail = new email($company['coy_name'], $company['email']);
	$to = $loc['location_name'] . " <" . $loc['email'] . ">";
	$subject = _("Stocks below Re-Order Level at " . $loc['location_name']);
	$msg = "\n";
	for ($i = 0; $i < count($st_ids); $i++)
		$msg .= $st_ids[$i] . " " . $st_names[$i] . ", " . _("Re-Order Level") . ": " . $st_reorder[$i] . ", " . _("Below") . ": " . $st_num[$i] . "\n";
	$msg .= "\n" . _("Please reorder") . "\n\n";
	$msg .= $company['coy_name'];
	$mail->to($to);
	$mail->subject($subject);
	$mail->text($msg);
	return $mail->send();
}

//----------------------------------------- Backend interfaces ----------------------------------------------------

function stock_cost_update($stock_id, $material_cost, $labour_cost, $overhead_cost,
	$last_cost, $refline, $memo_)
{
	begin_transaction(__FUNCTION__, func_get_args());

	$mb_flag = get_mb_flag($stock_id);

	$update_no = -1;

    if (is_service($mb_flag))
    {
		$sql = "UPDATE ".TB_PREF."stock_master SET material_cost=".db_escape($material_cost)."
		WHERE stock_id=".db_escape($stock_id);

		db_query($sql,"The cost details for the inventory item could not be updated");

		commit_transaction();
		return $update_no;
    }

	$sql = "UPDATE ".TB_PREF."stock_master SET material_cost=".db_escape($material_cost).", 
		labour_cost=".db_escape($labour_cost).",
		overhead_cost=".db_escape($overhead_cost)."
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
			global $Refs;
			$stock_gl_code = get_stock_gl_code($stock_id);

			$cart = new items_cart(ST_COSTUPDATE);
			$cart->tran_date = $cart->doc_date = $cart->event_date = $date_;
			if (!is_date_in_fiscalyear($cart->tran_date))
				$cart->tran_date = end_fiscalyear();
			$cart->reference = $Refs->get_next(ST_COSTUPDATE, $refline, $cart->tran_date, $date_);
 
			if (empty($memo_))
				$cart->memo_ = sprintf(_("Cost was %s changed to %s x quantity on hand of %s"),
					number_format2($last_cost, 2), number_format2($new_cost), $qoh);
			else
				$cart->memo_ = $memo_;

			$cart->add_gl_item($stock_gl_code["adjustment_account"],
				$stock_gl_code["dimension_id"], $stock_gl_code["dimension2_id"], -$value_of_change);
			$cart->add_gl_item($stock_gl_code["inventory_account"], 0, 0, $value_of_change);

			write_journal_entries($cart);
		}
   	}

	if ($update_no != -1)
		add_audit_trail(ST_COSTUPDATE, $update_no, $date_);

	commit_transaction();
	return $update_no;
}

function write_item($id, $NewStockID, $description, $long_description, $category_id, $tax_type_id, $units, 
	$mb_flag, $sales_account, $inventory_account, $cogs_account, $adjustment_account, $wip_account, 
	$dim1, $dim2, $no_sale, $editable, $no_purchase, $depreciation_method, $depreciation_rate, $depreciation_factor, $depreciation_start,
	$fa_class_id, $vat_category, $shipper, $inactive)
{
	begin_transaction(__FUNCTION__, func_get_args());
	if ($id != '')
	{
	 	update_item($NewStockID, $description, $long_description, $category_id, $tax_type_id, $units, 
			$mb_flag, $sales_account, $inventory_account, $cogs_account, $adjustment_account, $wip_account, 
			$dim1, $dim2, $no_sale, $editable, $no_purchase, $depreciation_method, $depreciation_rate, $depreciation_factor, $depreciation_start,
			$fa_class_id, $vat_category, $shipper);
		update_record_status($NewStockID, $inactive, 'stock_master', 'stock_id');
		update_record_status($NewStockID, $inactive, 'item_codes', 'item_code');
	} else {
	 	add_item($NewStockID, $description, $long_description, $category_id, $tax_type_id, $units, 
			$mb_flag, $sales_account, $inventory_account, $cogs_account, $adjustment_account, $wip_account, 
			$dim1, $dim2, $no_sale, $editable, $no_purchase, $depreciation_method, $depreciation_rate, $depreciation_factor, $depreciation_start,
			$fa_class_id, $vat_category, $shipper);
	}
	commit_transaction();
}
