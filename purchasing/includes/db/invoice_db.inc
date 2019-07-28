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
include_once($path_to_root . "/purchasing/includes/db/invoice_items_db.inc");
include_once($path_to_root . "/admin/db/attachments_db.inc");

//--------------------------------------------------------------------------------------------------

function read_supplier_details_to_trans(&$supp_trans, $supplier_id)
{
	$sql = "SELECT supp.supp_name, supp.dimension_id, supp.dimension2_id, supp.tax_included,
		supp.tax_group_id, tax_group.name AS tax_group_name,
		supp.credit_limit - Sum((ov_amount + ov_gst + ov_discount)) as cur_credit,
		supp.curr_code,
		terms.*
		FROM ".TB_PREF."suppliers supp 
			LEFT JOIN ".TB_PREF."supp_trans trans ON supp.supplier_id = trans.supplier_id
			LEFT JOIN ".TB_PREF."payment_terms terms ON supp.payment_terms=terms.id
			LEFT JOIN ".TB_PREF."tax_groups tax_group ON supp.tax_group_id = tax_group.id
		WHERE supp.supplier_id = ".db_escape($supplier_id);

	$result = db_query($sql, "The supplier record selected: " . $supplier_id . " cannot be retrieved");

	$myrow = db_fetch($result);

    if ($supp_trans->tran_date == "")
    {
		$supp_trans->tran_date = Today();

		if (!is_date_in_fiscalyear($supp_trans->tran_date))
			$supp_trans->tran_date = end_fiscalyear();
	}
	if ($supp_trans->supplier_id != $supplier_id)
	    get_duedate_from_terms($supp_trans);

    $supp_trans->supplier_id = $supplier_id;
    $supp_trans->tax_included = $myrow['tax_included'];
    $supp_trans->supplier_name = $myrow['supp_name'];
	$supp_trans->terms = array( 
		'description' => $myrow['terms'],
		'type' => $myrow['type'], 
		'days' => $myrow['days'],
		'early_discount' => $myrow['early_discount'],
		'early_days' => $myrow['early_days'],
	);
	$supp_trans->credit = $myrow['cur_credit'];

	$supp_trans->tax_description = $myrow['tax_group_name'];
	$supp_trans->tax_group_id = $myrow['tax_group_id'];
	$supp_trans->dimension = $myrow['dimension_id'];
	$supp_trans->dimension2 = $myrow['dimension2_id'];
	$supp_trans->currency = $myrow['curr_code'];
 
}

//-------------------------------------------------------------------------------------------------
//	Updates invoiced quantity in PO and GRN line, and act_price in PO
//  Returns:
// if chg_price is set:	 previous act_price, delivery date and unit_price from PO
//
function update_supp_received_items_for_invoice($id, $po_detail_item, $qty_invoiced, $chg_price=null)
{
	if ($chg_price != null)
	{
		$sql = "SELECT act_price, unit_price FROM ".TB_PREF."purch_order_details WHERE
			po_detail_item = ".db_escape($po_detail_item);
		$result = db_query($sql, "The old actual price of the purchase order line could not be retrieved");
		$row = db_fetch($result);
		$ret = $row['act_price'];

		$unit_price = $row['unit_price']; //Added by Rasmus

		$sql = "SELECT delivery_date 
			FROM ".TB_PREF."grn_batch grn,"
				.TB_PREF."grn_items line
			WHERE
				grn.id = line.grn_batch_id AND line.id=".db_escape($id);
		$result = db_query($sql, "The old delivery date from the received record cout not be retrieved");
		$row = db_fetch($result);
		$date = $row['delivery_date'];
	}
	else
	{
		$ret = 0;
		$date = "";
		$unit_price = 0; // Added by Rasmus
	}
    $sql = "UPDATE ".TB_PREF."purch_order_details
		SET qty_invoiced = qty_invoiced + ".db_escape($qty_invoiced);

	if ($chg_price != null)
		$sql .= " , act_price = ".db_escape($chg_price);

	$sql .= " WHERE po_detail_item = ".db_escape($po_detail_item);
    db_query($sql, "The quantity invoiced of the purchase order line could not be updated");

    $sql = "UPDATE ".TB_PREF."grn_items
        SET quantity_inv = quantity_inv + ".db_escape($qty_invoiced)."
        WHERE id = ".db_escape($id);
	db_query($sql, "The quantity invoiced off the items received record could not be updated");
	return array($ret, $date, $unit_price);
}

function get_diff_in_home_currency($supplier, $old_date, $date, $amount1, $amount2)
{
	$dec = user_price_dec();
	price_decimal_format($amount2, $dec);
	$currency = get_supplier_currency($supplier);
	$ex_rate = get_exchange_rate_to_home_currency($currency, $old_date);
	$amount1 = $amount1 / $ex_rate;
	$ex_rate = get_exchange_rate_to_home_currency($currency, $date);
	$amount2 = $amount2 / $ex_rate;
	$diff = $amount2 - $amount1;
	//return round2($diff, $dec);
	return $diff;
}
//----------------------------------------------------------------------------------------

function add_supp_invoice(&$supp_trans)  //, $already_voided=false, $allocs=null)
{
	global $Refs, $systypes_array;

	$trans_no = $supp_trans->trans_no;
	$trans_type = $supp_trans->trans_type;
	$supplier = get_supplier($supp_trans->supplier_id);
	$dec = user_price_dec();

	begin_transaction(__FUNCTION__, func_get_args());
	hook_db_prewrite($supp_trans, $trans_type);

	if ($trans_no) {	// void old transaction
//	 if (!$already_voided) {	// transaction is already voided in case of direct invoice edition, which is needed for proper inventory value handling
		$allocs = get_payments_for($trans_no, $trans_type, $supp_trans->supplier_id); // save allocations
		void_supp_invoice($trans_type, $trans_no, true);
		add_audit_trail($trans_type, $trans_no, Today(), _("Voided."));
		add_voided_entry($trans_type, $trans_no, Today(), _("Document reentered."));
		$Refs->restore_last($trans_type, $trans_no);
//	 }
	} else
		$allocs = get_po_prepayments($supp_trans);

	// register exchange rate when used first time on date
	add_new_exchange_rate($supplier['curr_code'], $supp_trans->tran_date, $supp_trans->ex_rate);

	// store basic transaction info
	$invoice_id = write_supp_trans($trans_type, 0, $supp_trans->supplier_id, $supp_trans->tran_date, $supp_trans->due_date,
		$supp_trans->reference, $supp_trans->supp_reference, 0, 0, 0, $supp_trans->ex_rate, $supp_trans->tax_included);

	if ($trans_no)
		move_trans_attachments($trans_type, $trans_no, $invoice_id);

	$supp_trans->trans_no = $invoice_id;
    $date_ = $supp_trans->tran_date;

	if (!$supp_trans->ex_rate)
		$supp_trans->ex_rate = get_exchange_rate_from_home_currency($supplier['curr_code'], $date_);

	// prepare cart for GL postings 
	$gl_cart = new items_cart($trans_type, $invoice_id);
	$gl_cart->tran_date = $date_;
	$gl_cart->set_currency($supplier['curr_code'], $supp_trans->ex_rate);

    $clearing_act = get_company_pref('grn_clearing_act');

	$supp_trans->split_line_values();	// generate amounts for GL postings

	$tax_total = 0;
	$net_total = 0;
	foreach($supp_trans->grn_items as $item)
	{
		$item_gl = $item->gl_amounts;

		$stock_gl_code = get_stock_gl_code($item->item_code);
		$dimension = !empty($supp_trans->dimension) ? $supp_trans->dimension :
			($supplier['dimension_id'] ? $supplier['dimension_id'] : $stock_gl_code['dimension_id']);
		$dimension2 = !empty($supp_trans->dimension2) ? $supp_trans->dimension2 :
			($supplier['dimension2_id'] ? $supplier['dimension2_id'] : $stock_gl_code['dimension2_id']);

		$line_tax = 0;
		foreach($item_gl as $index => $value)
		{
			 if (is_numeric($index)) // taxes
			 {
				if ($value['Deductible'])
				{	// GL: VAT deductible
					$gl_cart->add_gl_item($value['purchasing_gl_code'], 0, 0, $value['Deductible']+$value['Adjust']);
					// GL: VAT adjustment due to vat factor
					if ($value['Adjust'])
						$gl_cart->add_gl_item(get_company_pref('tax_adjustments_act'), 0, 0, -$value['Adjust']);
				}
				if ($value['Payable']) // reverse charge/intracommunity aquisition
				{
	   				$gl_cart->add_gl_item($value['sales_gl_code'], 0, 0, -$value['Payable']);
				}
		   		// GL: AP account (vat part)
				if ($value['Value'])
				{
					$gl_cart->add_gl_item($supplier["payable_account"], 0, 0, -$value['Value'] , '', '', $supp_trans->supplier_id);
				}
				if (($item->vat_category == VC_REVERSE) && $supplier['tax_area'] == TA_EU)
					$vat_category = VC_OTHER;
				else
					$vat_category = $item->vat_category;

				add_trans_tax_details($trans_type, $invoice_id, 
					$value['tax_type_id'], $value['rate'], $supp_trans->tax_included, $value['Value'],
					$item_gl['Net'], $supp_trans->ex_rate, $date_, $supp_trans->supp_reference, TR_INPUT, $supp_trans->tax_group_id, $vat_category);

				$line_tax += $value['Payable'] ? $value['Payable'] : $value['Value'];
			}
		}

		// GL: AP account (net)
		$gl_cart->add_gl_item($supplier["payable_account"], 0, 0, -$item_gl['Net'], '', '', $supp_trans->supplier_id);

		$tax_total += $item_gl['Tax'];
		$net_total += $item_gl['Net'];

		// cost line value
		$taxfree_line =  $item_gl['Cost'];

		if (!is_inventory_item($item->item_code))
		{
			$gl_cart->add_gl_item($supplier["purchase_account"] ? $supplier["purchase_account"] : $stock_gl_code["cogs_account"], $dimension, $dimension2, $taxfree_line);

		} elseif ($trans_type != ST_SUPPCREDIT) {
			$gl_cart->add_gl_item($stock_gl_code["inventory_account"], $dimension, $dimension2, $taxfree_line, _('Return to supplier'));
			// we must use invoice value here to preserve proper inventory valuation,
			// but if the purchase changed average cost, and any item was sold between invoice and credit,
			// the average will not return to the previous one. To fix this additional cost update should be made here to compensate cogs difference on items sold.
			update_average_material_cost(null, $item->item_code, $item_gl['Cost']/$item->this_quantity_inv, -$item->this_quantity_inv, $date_);
		} else {

			// calculate difference between clearing cost and actual cost
			$diff = $item_gl['Cost'] - $item->std_cost_unit*$item->this_quantity_inv;

			update_average_material_cost(null, $item->item_code,
				$diff/$item->this_quantity_inv, $item->this_quantity_inv, null, true);

			if ($clearing_act)
			{
				if ($diff != 0) // if value on invoice differs from those posted on GRN receive, post the difference
				{

					$gl_cart->add_gl_item($stock_gl_code["inventory_account"],			// cart is in supplier currency, so need to fix by ex_rate here
						$dimension, $dimension2, $diff/$gl_cart->rate, _('GRN Provision')); // subject to rounding errors?

					$gl_cart->add_gl_item($clearing_act,
						$dimension, $dimension2, -$diff/$gl_cart->rate);

					//Chaitanya
					//If QOH is 0 or negative then update_average_material_cost will be skipped
					//Thus difference in PO and Supp Invoice should be handled separately
					$qoh = get_qoh_on_date($item->item_code);
					if ($qoh <= 0)
					{
						global $Refs;
							//Chaitanya : Post a journal entry
						$id = get_next_trans_no(ST_JOURNAL);
						$ref = $Refs->get_next(ST_JOURNAL, null, $date_);
						add_journal(ST_JOURNAL, $id, $diff, $date_, get_company_currency(), $ref);
						$stock_id = $item->item_code;
						$stock_gl_code = get_stock_gl_code($stock_id);
						$memo = _("Supplier invoice adjustment for zero inventory of ").$stock_id." "._("Invoice")." ".$supp_trans->reference;
						//Reverse the inventory effect if $qoh <=0
						add_gl_trans_std_cost(ST_JOURNAL, $id, $date_, 
							$stock_gl_code["inventory_account"],
							$dimension, $dimension2,
							$memo, -$diff);
						//GL Posting to inventory adjustment account
						add_gl_trans_std_cost(ST_JOURNAL, $id, $date_, 
							$stock_gl_code["adjustment_account"],
							$dimension, $dimension2,
							$memo, $diff);
							add_audit_trail(ST_JOURNAL, $id, $date_);
						add_comments(ST_JOURNAL, $id, $date_, $memo);
						$Refs->save(ST_JOURNAL, $id, $ref);
					}
				}
				$gl_cart->add_gl_item($clearing_act, $dimension, $dimension2, $taxfree_line);
			}
			else  // no postings on GRN, so post full cost here
				$gl_cart->add_gl_item($stock_gl_code["inventory_account"], $dimension, $dimension2,	$taxfree_line);

		}

//        if (is_fa_item($item->item_code)) {
//          add_actual_cost($item->order_price, $item->item_code);
//        }

    	if ($trans_type == ST_SUPPCREDIT)
			set_grn_item_credited($item, $supp_trans->supplier_id, $invoice_id, $date_);
		else {
			add_or_update_purchase_data($supp_trans->supplier_id, $item->item_code, $item->chg_price); 

			update_supp_received_items_for_invoice($item->id, $item->po_detail_item,
				$item->this_quantity_inv, $item->chg_price);
		}

		$qty = ($trans_type==ST_SUPPCREDIT ? 1 : -1)*$item->this_quantity_inv;
		add_supp_invoice_item($trans_type, $invoice_id, $item->item_code,
			$item->item_description, 0, 	$item->chg_price, $line_tax/$qty, $qty, $item->id, $item->po_detail_item);
    } // grn_items


    /*GL Items are straight forward - just do the debit postings to the GL accounts specified -
    the credit is to creditors control act */
    foreach ($supp_trans->gl_codes as $entered_gl_code)
    {
		$memo_ = $entered_gl_code->memo_;

		$index = is_tax_account($entered_gl_code->gl_code);
		if ($index !== false)
		{
			$gl_cart->add_gl_item($entered_gl_code->gl_code, $entered_gl_code->gl_dim, $entered_gl_code->gl_dim2, $entered_gl_code->amount);
			// store tax details if the gl account is a tax account
			add_gl_tax_details($entered_gl_code->gl_code, 
				$trans_type, $invoice_id, $entered_gl_code->amount,
				$supp_trans->ex_rate, $date_, $supp_trans->supp_reference, $supp_trans->tax_included, null, $supp_trans->tax_group_id);

			$tax_total += $entered_gl_code->amount;
		} else {
			$gl_cart->add_gl_item($entered_gl_code->gl_code, $entered_gl_code->gl_dim, $entered_gl_code->gl_dim2, $entered_gl_code->amount);
			$net_total += $entered_gl_code->amount;
		}
		$gl_cart->add_gl_item($supplier["payable_account"], 0, 0, -$entered_gl_code->amount, '', '', $supp_trans->supplier_id);

		add_supp_invoice_gl_item($trans_type, $invoice_id, $entered_gl_code->gl_code, $entered_gl_code->amount, $memo_,
			$entered_gl_code->gl_dim, $entered_gl_code->gl_dim2);
    }

	if ($trans_type == ST_SUPPCREDIT) {		// reverse postings if this is credit note
		foreach($gl_cart->gl_items as &$line)
			$line->amount = -$line->amount;
		$net_total = -$net_total;
		$tax_total = -$tax_total;
	}
	$gl_cart->reduce_gl();	// minimize GL lines

	$gl_cart->write_gl(false); // don't check balances here: we are working on two (maybe unbalanced) carts

	update_supp_trans_sums($trans_type, $invoice_id, round($net_total,2), round($tax_total,2));

	add_comments($trans_type, $invoice_id, $date_, $supp_trans->Comments);

	$Refs->save($trans_type, $invoice_id, $supp_trans->reference);

	if (count($supp_trans->src_docs) == 1)
	{
		$invoice_no = key($supp_trans->src_docs);
		$invoice_alloc_balance = get_supp_trans_allocation_balance(ST_SUPPINVOICE, $supp_trans->src_docs);
		if ($invoice_alloc_balance > 0) 
		{ 	//the invoice is not already fully allocated 

			$trans = get_supp_trans($invoice_id, ST_SUPPCREDIT);
			$total = -$trans['Total'];

			$allocate_amount = ($invoice_alloc_balance > $total) ? $total : $invoice_alloc_balance;

			/*Now insert the allocation record if > 0 */
			if ($allocate_amount != 0) 
			{
				add_supp_allocation($allocate_amount, ST_SUPPCREDIT, $invoice_id, ST_SUPPINVOICE, $invoice_no,
					$supp_trans->supplier_id, $date_);
				update_supp_trans_allocation(ST_SUPPINVOICE, $invoice_no, $supp_trans->supplier_id);
				update_supp_trans_allocation(ST_SUPPCREDIT, $invoice_id, $supp_trans->supplier_id);

				exchange_variation(ST_SUPPCREDIT, $invoice_id, ST_SUPPINVOICE, $supp_trans->src_docs, $date_,
					$allocate_amount, PT_SUPPLIER);
			}
		}
	}

	reallocate_payments($invoice_id, ST_SUPPINVOICE, $date_, $net_total+$tax_total, $allocs, $supp_trans->supplier_id);

	hook_db_postwrite($supp_trans, $supp_trans->trans_type);
    commit_transaction();
    return $invoice_id;
}

//----------------------------------------------------------------------------------------

// get all the invoices/credits for a given PO - quite long route to get there !

function get_po_invoices_credits($po_number)
{
	$sql = "SELECT DISTINCT trans.trans_no, trans.type,	ov_amount+ov_discount+ov_gst AS Total,
				trans.tran_date
			FROM ".TB_PREF."supp_trans trans,"
				.TB_PREF."supp_invoice_items line,"
				.TB_PREF."purch_order_details poline,"
				.TB_PREF."purch_orders po
		WHERE line.supp_trans_no = trans.trans_no
			AND line.supp_trans_type = trans.type
			AND line.po_detail_item_id = poline.po_detail_item
			AND po.supplier_id = trans.supplier_id
			AND po.order_no = poline.order_no
			AND poline.order_no = ".db_escape($po_number);

	return db_query($sql, "The invoices/credits for the po $po_number could not be retreived");
}

//----------------------------------------------------------------------------------------
//
// Retrieve tax ovverides from tax register.
//
function get_tax_overrides($trans_type, $trans_no)
{
	$tax_overrides = array();
	$sql = "SELECT amount, tax_type_id as id, rate
		FROM ".TB_PREF."trans_tax_details details
		WHERE trans_type=".db_escape($trans_type)." AND trans_no=".db_escape($trans_no)
		." AND amount>0"; // skip reposting of nondeductible VAT

	$result = db_query($sql, 'Cannot retrieve tax overrides');
	while($row = db_fetch($result))
	{
		$tax_overrides[$row['id']] = $row['amount'];
	}

	return $tax_overrides;
}
//----------------------------------------------------------------------------------------

function read_supp_invoice($trans_no, $trans_type, &$supp_trans)
{
	$sql = "SELECT trans.*, supp_name, dimension_id, dimension2_id, curr_code
		FROM ".TB_PREF."supp_trans trans,"
			.TB_PREF."suppliers sup
		WHERE trans_no = ".db_escape($trans_no)." AND type = ".db_escape($trans_type)."
		AND sup.supplier_id=trans.supplier_id";

	$result = db_query($sql, "Cannot retreive a supplier transaction");

	if (db_num_rows($result) == 1)
	{
		$trans_row = db_fetch($result);

		$supp_trans->supplier_id = $trans_row["supplier_id"];
		$supp_trans->supplier_name = $trans_row["supp_name"];
		$supp_trans->tran_date = sql2date($trans_row["tran_date"]);
		$supp_trans->due_date = sql2date($trans_row["due_date"]);
		$supp_trans->Comments = get_comments_string($trans_type, $trans_no);
		$supp_trans->reference = $trans_row["reference"];
		$supp_trans->supp_reference = $trans_row["supp_reference"];
		$supp_trans->ov_amount = $trans_row["ov_amount"];
		$supp_trans->ov_discount = $trans_row["ov_discount"];
		$supp_trans->ov_gst = $trans_row["ov_gst"];
		$supp_trans->tax_included = $trans_row["tax_included"];
		$supp_trans->dimension = $trans_row["dimension_id"];
		$supp_trans->dimension2 = $trans_row["dimension2_id"];
		$supp_trans->currency = $trans_row["curr_code"];

		$id = $trans_row["trans_no"];

		$result = get_supp_invoice_items($trans_type, $id);

		if (db_num_rows($result) > 0)
		{

            while ($details_row = db_fetch($result))
            {

             	if ($details_row["gl_code"] == 0)
             	{
             		$supp_trans->add_grn_to_trans($details_row["grn_item_id"], $details_row["po_detail_item_id"], $details_row["stock_id"],
           				$details_row["description"], 
           				$details_row['qty_recd'],
           				$details_row['quantity_inv']-$details_row["quantity"],
           				$details_row["quantity"], 0, $details_row["FullUnitPrice"],
           				0, 0);
             	}
             	else
             	{
             		$supp_trans->add_gl_codes_to_trans($details_row["gl_code"], get_gl_account_name($details_row["gl_code"]), 
             			$details_row["dimension_id"], $details_row["dimension2_id"],
           				$details_row["FullUnitPrice"], $details_row["memo_"]);
             	}
            }
			$supp_trans->tax_overrides = get_tax_overrides($trans_type, $trans_no);
        }
        else
        {
			return display_db_error("Invalid supptrans details for supptrans number : $trans_no and type : $trans_type", $sql, true);
		}

	}
	else
	{
		return display_db_error("Invalid supptrans number : $trans_no and type : $trans_type", $sql, true);
	}
}

//----------------------------------------------------------------------------------------

function get_matching_invoice_item($stock_id, $po_item_id)
{
	$sql = "SELECT *, tran_date
		FROM ".TB_PREF."supp_invoice_items,"
			.TB_PREF."supp_trans
		WHERE supp_trans_type = ".ST_SUPPINVOICE." AND stock_id = "
		.db_escape($stock_id)." AND po_detail_item_id = ".db_escape($po_item_id)."
		AND supp_trans_no = trans_no";
	$result = db_query($sql, "Cannot retreive supplier transaction detail records");
	return db_fetch($result);  
}

function void_supp_invoice($type, $type_no)
{
	begin_transaction(__FUNCTION__, func_get_args());

	hook_db_prevoid($type, $type_no);
	$trans = get_supp_trans($type_no, $type);

	void_bank_trans($type, $type_no, true);

	void_gl_trans($type, $type_no, true);

	void_supp_allocations($type, $type_no);

	void_supp_trans($type, $type_no);

	$result = get_supp_invoice_items($type, $type_no);

	// now remove this invoice/credit from any GRNs/POs that it's related to
	if (db_num_rows($result) > 0)
	{
		$date_ = Today();
        while ($details_row = db_fetch($result))
        {
         	if ((int)$details_row["grn_item_id"] > 0) // it can be empty for GL items
         	{
         		// Changed 2008-10-17 by Joe Hunt to get the avg. material cost updated
				$old = update_supp_received_items_for_invoice($details_row["grn_item_id"],
					$details_row["po_detail_item_id"], -$details_row["quantity"], $details_row["FullUnitPrice"]); 
				
				//$diff = $details_row["FullUnitPrice"] - $old[2];
				$old_date = sql2date($old[1]);
				
				$batch = get_grn_batch_from_item($details_row["grn_item_id"]);	
				$grn = get_grn_batch($batch);
         		if ($type == ST_SUPPCREDIT) // credit note 2009-06-14 Joe Hunt Must restore the po and grn
         		{	// We must get the corresponding invoice item to check for price chg.
					$match = get_matching_invoice_item($details_row["stock_id"], $details_row["po_detail_item_id"]);
					//Chaitanya : Skipped costing block & handle in void_stock_move
					// We must get the corresponding invoice item to check for price chg.
					/*if ($match !== false)
						$mat_cost = update_average_material_cost($grn["supplier_id"], $details_row["stock_id"],  
							$match["unit_price"], -$details_row["quantity"], sql2date($match['tran_date']), $match['tran_date'] !== $trans['tran_date']);
					else		
						$mat_cost = update_average_material_cost($grn["supplier_id"], $details_row["stock_id"],  
							$details_row["FullUnitPrice"], -$details_row["quantity"], $old_date, $old[1] !== $trans['tran_date']);*/

					$sql = "UPDATE ".TB_PREF."purch_order_details
        				SET quantity_ordered = quantity_ordered + ".-$details_row["quantity"].", ";
        			if ($match !== false)
        				$sql .= "act_price=".$match['unit_price'].", ";
    				$sql .= "quantity_received = quantity_received + ".-$details_row["quantity"]."
    				    WHERE po_detail_item = ".$details_row["po_detail_item_id"];
					db_query($sql, "a purchase order details record could not be updated. This receipt of goods has not been processed ");
					$sql = "UPDATE ".TB_PREF."grn_items SET qty_recd=qty_recd+".-$details_row["quantity"]."
						WHERE id=".$details_row["grn_item_id"];
					db_query($sql);
    			}
    			else
    			{
					$diff = get_diff_in_home_currency($grn["supplier_id"], $old_date, sql2date($trans['tran_date']), $old[2], 
						$details_row["FullUnitPrice"]);
					// Only adjust the avg for the diff
					$mat_cost = update_average_material_cost(null, $details_row["stock_id"],
						$diff, -$details_row["quantity"], $old_date, true);
						
					//Chaitanya : Reverse effect
					//If QOH is 0 or negative then update_average_material_cost will be skipped
					//Thus difference in PO and Supp Invoice should be handled separately
					
					$qoh = get_qoh_on_date($details_row["stock_id"]);
					if ($diff*$details_row["quantity"] !=0 && $qoh <= 0)
					{
						global $Refs;

						//Chaitanya : Post a journal entry
						$id = get_next_trans_no(ST_JOURNAL);
						$ref = $Refs->get_next(ST_JOURNAL, null, $date_);
						add_journal(ST_JOURNAL, $id, $details_row["quantity"] * $diff, $old_date, get_company_currency(), $ref);
						$stock_id = $details_row["stock_id"];
						$stock_gl_code = get_stock_gl_code($stock_id);
						$memo = "Reversing Supplier invoice adjustment for zero inventory of ".$stock_id." Invoice: ".$trans['reference'];
						//Reverse the inventory effect if $qoh <=0
						add_gl_trans_std_cost(ST_JOURNAL, $id, $old_date, 
							$stock_gl_code["inventory_account"],
							$dim, $dim2, $memo, $details_row["quantity"] * $diff);
						//GL Posting to inventory adjustment account
						add_gl_trans_std_cost(ST_JOURNAL, $id, $old_date, 
							$stock_gl_code["adjustment_account"],
							$dim, $dim2, $memo, -$details_row["quantity"] * $diff);
						
						add_audit_trail(ST_JOURNAL, $id, $old_date);
						add_comments(ST_JOURNAL, $id, $old_date, $memo);
						$Refs->save(ST_JOURNAL, $id, $ref);
					}
				}
         	}
        }
	}

	if ($type == ST_SUPPCREDIT) // void the credits in stock moves
		void_stock_move($type, $type_no); 
	void_supp_invoice_items($type, $type_no);
	void_trans_tax_details($type, $type_no);

	commit_transaction();
}

//----------------------------------------------------------------------------------------
function get_gl_account_info($acc)
{
	$sql = "SELECT account_code, account_name FROM ".TB_PREF."chart_master WHERE account_code=".db_escape($acc);
	return db_query($sql,"get account information");
}

function is_reference_already_there($supplier_id, $supp_reference, $trans_no=0)
{
	$sql = "SELECT COUNT(*) FROM ".TB_PREF."supp_trans WHERE supplier_id="
		.db_escape($supplier_id) . " AND supp_reference=" 
		.db_escape($supp_reference) 
		. " AND ov_amount!=0"; // ignore voided invoice references
	if ($trans_no)
		$sql .= " AND trans_no!=$trans_no";
	$result=db_query($sql,"The sql to check for the previous entry of the same invoice failed");

	$myrow = db_fetch_row($result);
	return $myrow[0] > 0;
}

function find_src_invoices($cart)
{
	$invoices = $po_ids = array();
	foreach($cart->grn_items as $item)
		$po_ids[] = "'$item->po_detail_item'";	// credit item lines

	if (count($po_ids)) {
		$sql = "SELECT DISTINCT trans.trans_no, trans.reference, trans.supp_reference
			FROM ".TB_PREF."supp_invoice_items items
			LEFT JOIN ".TB_PREF."supp_trans trans ON trans.trans_no=items.supp_trans_no AND trans.`type`=items.supp_trans_type
			WHERE items.po_detail_item_id IN (" . implode(',', $po_ids). ")"." AND items.supp_trans_type=20";
		$result = db_query($sql, "cannot find source invoice details");
		while ($rec = db_fetch($result))
		{
			$invoices[$rec['trans_no']] = $rec['supp_reference']; // array($rec['reference'], $rec['supp_reference']);
		}
	}
	return $invoices;
}
