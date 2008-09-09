<?php

include_once($path_to_root . "/purchasing/includes/db/invoice_items_db.inc");

//--------------------------------------------------------------------------------------------------

function read_supplier_details_to_trans(&$supp_trans, $supplier_id)
{
	$sql = "SELECT ".TB_PREF."suppliers.supp_name, ".TB_PREF."payment_terms.terms, ".TB_PREF."payment_terms.days_before_due,
		".TB_PREF."payment_terms.day_in_following_month,
		".TB_PREF."suppliers.tax_group_id, ".TB_PREF."tax_groups.name As tax_group_name
		From ".TB_PREF."suppliers, ".TB_PREF."payment_terms, ".TB_PREF."tax_groups
		WHERE ".TB_PREF."suppliers.tax_group_id = ".TB_PREF."tax_groups.id
		AND ".TB_PREF."suppliers.payment_terms=".TB_PREF."payment_terms.terms_indicator
		AND ".TB_PREF."suppliers.supplier_id = '" . $supplier_id . "'";

	$result = db_query($sql, "The supplier record selected: " . $supplier_id . " cannot be retrieved");

	$myrow = db_fetch($result);

    $supp_trans->supplier_id = $supplier_id;
    $supp_trans->supplier_name = $myrow['supp_name'];
	$supp_trans->terms_description = $myrow['terms'];

	if ($myrow['days_before_due'] == 0)
	{
		$supp_trans->terms = "1" . $myrow['day_in_following_month'];
	}
	else
	{
		$supp_trans->terms = "0" . $myrow['days_before_due'];
	}
	$supp_trans->tax_description = $myrow['tax_group_name'];
	$supp_trans->tax_group_id = $myrow['tax_group_id'];

    if ($supp_trans->tran_date == "")
    {
		$supp_trans->tran_date = Today();
		if (!is_date_in_fiscalyear($supp_trans->tran_date))
			$supp_trans->tran_date = end_fiscalyear();
	}
    //if ($supp_trans->due_date=="") {
    //	get_duedate_from_terms($supp_trans);
    //}
    get_duedate_from_terms($supp_trans);
}

//--------------------------------------------------------------------------------------------------

function update_supp_received_items_for_invoice($id, $po_detail_item, $qty_invoiced, $chg_price=null)
{
	if ($chg_price != null)
	{
		$sql = "SELECT act_price, unit_price FROM ".TB_PREF."purch_order_details WHERE
			po_detail_item = $po_detail_item";
		$result = db_query($sql, "The old actual price of the purchase order line could not be retrieved");
		$row = db_fetch_row($result);
		$ret = $row[0];

		$unit_price = $row[1]; //Added by Rasmus

		$sql = "SELECT delivery_date FROM ".TB_PREF."grn_batch,".TB_PREF."grn_items WHERE
			".TB_PREF."grn_batch.id = ".TB_PREF."grn_items.grn_batch_id AND ".TB_PREF."grn_items.id=$id";
		$result = db_query($sql, "The old delivery date from the received record cout not be retrieved");
		$row = db_fetch_row($result);
		$date = $row[0];
	}
	else
	{
		$ret = 0;
		$date = "";
		$unit_price = 0; // Added by Rasmus
	}
    $sql = "UPDATE ".TB_PREF."purch_order_details
		SET qty_invoiced = qty_invoiced + $qty_invoiced ";

	if ($chg_price != null)
		$sql .= " , act_price = $chg_price ";

	$sql .= " WHERE po_detail_item = $po_detail_item";
    db_query($sql, "The quantity invoiced of the purchase order line could not be updated");

    $sql = "UPDATE ".TB_PREF."grn_items
        SET quantity_inv = quantity_inv + $qty_invoiced
        WHERE id = $id";
	db_query($sql, "The quantity invoiced off the items received record could not be updated");
	return array($ret, $date, $unit_price);
}

function get_deliveries_between($stock_id, $from, $to)
{
	$from = date2sql($from);
	$to = date2sql($to);
	$sql = "SELECT SUM(-qty), AVG(standard_cost) FROM ".TB_PREF."stock_moves
		WHERE type=13 AND stock_id='$stock_id' AND
			tran_date>='$from' AND tran_date<='$to' GROUP BY stock_id";
	$result = db_query($sql, "The deliveries could not be updated");
	return db_fetch_row($result);
}
//----------------------------------------------------------------------------------------

function add_supp_invoice($supp_trans) // do not receive as ref because we change locally
{
	//$company_currency = get_company_currency();
	/*Start an sql transaction */
	begin_transaction();

	$tax_total = 0;
    $taxes = $supp_trans->get_taxes($supp_trans->tax_group_id);

    foreach ($taxes as $taxitem)
    {
    	$tax_total += $taxitem['Value'];
    }

    $invoice_items_total = $supp_trans->get_total_charged($supp_trans->tax_group_id);

	if ($supp_trans->is_invoice)
		$trans_type = 20;
	else
	{
		$trans_type = 21;
		// let's negate everything because it's a credit note
		$invoice_items_total = -$invoice_items_total;
		$tax_total = -$tax_total;
		$supp_trans->ov_discount = -$supp_trans->ov_discount; // this isn't used at all...
	}

    $date_ = $supp_trans->tran_date;

    /*First insert the invoice into the supp_trans table*/
	$invoice_id = add_supp_trans($trans_type, $supp_trans->supplier_id, $date_, $supp_trans->due_date,
		$supp_trans->reference, $supp_trans->supp_reference,
		$invoice_items_total, $tax_total, $supp_trans->ov_discount);

    /* Now the control account */
    $supplier_accounts = get_supplier_accounts($supp_trans->supplier_id);
    add_gl_trans_supplier($trans_type, $invoice_id, $date_, $supplier_accounts["payable_account"], 0, 0,
		-($invoice_items_total +  $tax_total + $supp_trans->ov_discount),
		$supp_trans->supplier_id,
		"The general ledger transaction for the control total could not be added");

    /*Loop through the GL Entries and create a debit posting for each of the accounts entered */

    /*the postings here are a little tricky, the logic goes like this:
    if its a general ledger amount it goes straight to the account specified

    if its a GRN amount invoiced then :

    The cost as originally credited to GRN suspense on arrival of items is debited to GRN suspense. Any difference
    between the std cost and the currency cost charged as converted at the ex rate of of the invoice is written off
    to the purchase price variance account applicable to the item being invoiced.
    */

    foreach ($supp_trans->gl_codes as $entered_gl_code)
    {

	    /*GL Items are straight forward - just do the debit postings to the GL accounts specified -
	    the credit is to creditors control act  done later for the total invoice value + tax*/

		if (!$supp_trans->is_invoice)
			$entered_gl_code->amount = -$entered_gl_code->amount;

		$memo_ = $entered_gl_code->memo_;
		add_gl_trans_supplier($trans_type, $invoice_id, $date_, $entered_gl_code->gl_code,
			$entered_gl_code->gl_dim, $entered_gl_code->gl_dim2, $entered_gl_code->amount, $supp_trans->supplier_id);

		add_supp_invoice_gl_item($trans_type, $invoice_id, $entered_gl_code->gl_code,
			$entered_gl_code->amount, $memo_);
    }
    foreach ($supp_trans->grn_items as $entered_grn)
    {

    	if (!$supp_trans->is_invoice)
    	{
			$entered_grn->this_quantity_inv = -$entered_grn->this_quantity_inv;
			set_grn_item_credited($entered_grn, $supp_trans->supplier_id, $invoice_id, $date_);
    	}

		$line_taxfree = $entered_grn->taxfree_charge_price($supp_trans->tax_group_id);
		$line_tax = $entered_grn->full_charge_price($supp_trans->tax_group_id) - $line_taxfree;
		$stock_gl_code = get_stock_gl_code($entered_grn->item_code);

		add_gl_trans_supplier($trans_type, $invoice_id, $date_, $stock_gl_code["inventory_account"],
			$stock_gl_code['dimension_id'], $stock_gl_code['dimension2_id'],
			$entered_grn->this_quantity_inv * $line_taxfree, $supp_trans->supplier_id);
    	// -------------- if price changed since po received. 16 Aug 2008 Joe Hunt
    	if($supp_trans->is_invoice)
    	{
    		$old = update_supp_received_items_for_invoice($entered_grn->id, $entered_grn->po_detail_item,
    			$entered_grn->this_quantity_inv, $entered_grn->chg_price);
			// Since the standard cost is always calculated on basis of the po unit_price,
			// this is also the price that should be the base of calculating the price diff.
			// In cases where there is two different po invoices on the same delivery with different unit prices this will not work either

			//$old_price = $old[0];
			 
			$old_price = $old[2];

		

			/*
 			If statement is removed. Should always check for deliveries nomatter if there has been a price change. 
			*/
			//if ($old_price != $entered_grn->chg_price) // price-change, so update
			//{
				$diff = $entered_grn->chg_price - $old_price;
				$old_date = sql2date($old[1]);
				// only update the diff (last parameter, adj_only is set to true).
				$mat_cost = update_average_material_cost($supp_trans->supplier_id, $entered_grn->item_code,
					$diff, $entered_grn->this_quantity_inv, $old_date, true);
				// function just above this
				$deliveries = get_deliveries_between($entered_grn->item_code, $old_date, $date_);
				if ($deliveries[0] != 0) // have deliveries been done during the period?
				{

					$amt = ($mat_cost - $deliveries[1]) * $deliveries[0]; // $amt in home currency

					add_gl_trans($trans_type, $invoice_id, $date_,	$stock_gl_code["cogs_account"],
						$stock_gl_code['dimension_id'], $stock_gl_code['dimension2_id'], _("Cost diff."),
						$amt, null, null, null,
						"The general ledger transaction could not be added for the price variance of the inventory item");
					add_gl_trans($trans_type, $invoice_id, $date_,	$stock_gl_code["inventory_account"],
						0, 0, _("Cost diff."), -$amt, null, null, null,
						"The general ledger transaction could not be added for the price variance of the inventory item");
					update_stock_move_pid(13, $entered_grn->item_code, $old_date, $date_, 0, $mat_cost);
				}
				update_stock_move_pid(25, $entered_grn->item_code, $old_date, $old_date, $supp_trans->supplier_id, $mat_cost);
			//}
		}
    	// ----------------------------------------------------------------------

		add_supp_invoice_item($trans_type, $invoice_id, $entered_grn->item_code,
			$entered_grn->item_description, 0, 	$line_taxfree, $line_tax,
			$entered_grn->this_quantity_inv, $entered_grn->id, $entered_grn->po_detail_item, "");
    } /* end of GRN postings */
    /* Now the TAX account */
    foreach ($taxes as $taxitem)
    {
    	if ($taxitem['Value'] != 0)
    	{

    		if (!$supp_trans->is_invoice)
    			$taxitem['Value'] = -$taxitem['Value'];
    		// here we suppose that tax is never included in price (we are company customer).
			add_supp_invoice_tax_item($trans_type, $invoice_id, $taxitem['tax_type_id'],
				$taxitem['rate'], 0, $taxitem['Value']);

    		add_gl_trans_supplier($trans_type, $invoice_id, $date_,
    			$taxitem['purchasing_gl_code'], 0, 0, $taxitem['Value'],
    			$supp_trans->supplier_id,
    			"A general ledger transaction for the tax amount could not be added");
    	}
    }
	add_comments($trans_type, $invoice_id, $date_, $supp_trans->Comments);

	references::save_last($supp_trans->reference, $trans_type);

    commit_transaction();

    return $invoice_id;
}

//----------------------------------------------------------------------------------------

// get all the invoices/credits for a given PO - quite long route to get there !

function get_po_invoices_credits($po_number)
{
	$sql = "SELECT DISTINCT ".TB_PREF."supp_trans.trans_no, ".TB_PREF."supp_trans.type,
		ov_amount+ov_discount+ov_gst AS Total,
		".TB_PREF."supp_trans.tran_date
		FROM ".TB_PREF."supp_trans, ".TB_PREF."supp_invoice_items, ".TB_PREF."purch_order_details
		WHERE ".TB_PREF."supp_invoice_items.supp_trans_no = ".TB_PREF."supp_trans.trans_no
		AND ".TB_PREF."supp_invoice_items.po_detail_item_id = ".TB_PREF."purch_order_details.po_detail_item
		AND ".TB_PREF."purch_order_details.order_no = $po_number";

	return db_query($sql, "The invoices/credits for the po $po_number could not be retreived");
}

//----------------------------------------------------------------------------------------

function read_supp_invoice($trans_no, $trans_type, &$supp_trans)
{
	$sql = "SELECT ".TB_PREF."supp_trans.*, supp_name FROM ".TB_PREF."supp_trans,".TB_PREF."suppliers
		WHERE trans_no = $trans_no AND type = $trans_type
		AND ".TB_PREF."suppliers.supplier_id=".TB_PREF."supp_trans.supplier_id";
	$result = db_query($sql, "Cannot retreive a supplier transaction");

	if (db_num_rows($result) == 1)
	{
		$trans_row = db_fetch($result);

		$supp_trans->supplier_id = $trans_row["supplier_id"];
		$supp_trans->supplier_name = $trans_row["supp_name"];
		$supp_trans->tran_date = sql2date($trans_row["tran_date"]);
		$supp_trans->due_date = sql2date($trans_row["due_date"]);
		//$supp_trans->Comments = $trans_row["TransText"];
		$supp_trans->Comments = "";
		$supp_trans->reference = $trans_row["reference"];
		$supp_trans->supp_reference = $trans_row["supp_reference"];
		$supp_trans->ov_amount = $trans_row["ov_amount"];
		$supp_trans->ov_discount = $trans_row["ov_discount"];
		$supp_trans->ov_gst = $trans_row["ov_gst"];

		$id = $trans_row["trans_no"];

		$result = get_supp_invoice_items($trans_type, $id);

		if (db_num_rows($result) > 0)
		{

            while ($details_row = db_fetch($result))
            {

             	if ($details_row["gl_code"] == 0)
             	{
             		$supp_trans->add_grn_to_trans($details_row["grn_item_id"], $details_row["po_detail_item_id"], $details_row["stock_id"],
           				$details_row["description"], 0, 0, $details_row["quantity"], 0, $details_row["FullUnitPrice"],
           				false, 0, 0);
             	}
             	else
             	{
             		$supp_trans->add_gl_codes_to_trans($details_row["gl_code"], get_gl_account_name($details_row["gl_code"]), 0, 0,
           				$details_row["FullUnitPrice"], $details_row["memo_"]);
             	}
            }
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

function void_supp_invoice($type, $type_no)
{
	begin_transaction();

	void_bank_trans($type, $type_no, true);

	void_gl_trans($type, $type_no, true);

	void_supp_allocations($type, $type_no);

	void_supp_trans($type, $type_no);

	$result = get_supp_invoice_items($type, $type_no);

	// now remove this invoice/credit from any GRNs/POs that it's related to
	if (db_num_rows($result) > 0)
	{
        while ($details_row = db_fetch($result))
        {
         	if (strlen($details_row["grn_item_id"]) > 0) // it can be empty for GL items
         	{
				update_supp_received_items_for_invoice($details_row["grn_item_id"],
					$details_row["po_detail_item_id"], -$details_row["quantity"]);
         	}
        }
	}

	void_supp_invoice_items($type, $type_no);
	void_supp_invoice_tax_items($type, $type_no);

	commit_transaction();
}

//----------------------------------------------------------------------------------------


?>
