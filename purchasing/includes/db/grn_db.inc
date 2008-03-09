<?php

//-------------------------------------------------------------------------------------------------------------

function add_grn(&$po, $date_, $reference, $location)
{
	begin_transaction();

	$grn = add_grn_batch($po->order_no, $po->supplier_id, $reference, $location, $date_);

	foreach ($po->line_items as $order_line)
	{

		if ($order_line->receive_qty != 0 && $order_line->receive_qty != "" && isset($order_line->receive_qty))
		{

			/*Update sales_order_details for the new quantity received and the standard cost used for postings to GL and recorded in the stock movements for FIFO/LIFO stocks valuations*/

			if ($order_line->qty_received == 0)
			{
				/*This must be the first receipt of goods against this line */
			   	/*Need to get the standard cost as it is now so we can process GL jorunals later*/
			   	$order_line->standard_cost = get_standard_cost($order_line->stock_id);
			}

			//------------------- update average material cost ------------------------------------------ Joe Hunt Mar-03-2008
			$currency = get_supplier_currency($po->supplier_id);
			if ($currency != null)
				$price_in_home_currency = to_home_currency($order_line->price, $currency, $date_);
			else
				$price_in_home_currency = $order_line->price;
			$sql = "SELECT material_cost FROM ".TB_PREF."stock_master WHERE stock_id='$order_line->stock_id'";
			$result = db_query($sql);
			$myrow = db_fetch($result);
			$material_cost = $myrow['material_cost'];
			$qoh = get_qoh_on_date($order_line->stock_id, null, $date_);
			if ($qoh + $order_line->receive_qty <= 0)
				$material_cost = 0;
			else
				$material_cost = ($qoh * $material_cost + $order_line->receive_qty * $price_in_home_currency) /
					($qoh + $order_line->receive_qty);
			$sql = "UPDATE ".TB_PREF."stock_master SET material_cost=$material_cost
				WHERE stock_id='$order_line->stock_id'";
			db_query($sql,"The cost details for the inventory item could not be updated");
			//----------------------------------------------------------------------------------------------------------------

			/*Need to insert a grn item */

			$grn_item = add_grn_detail_item($grn, $order_line->po_detail_rec,
				$order_line->stock_id, $order_line->item_description,
				$order_line->standard_cost,	$order_line->receive_qty);

			/* Update location stock records - NB  a po cannot be entered for a service/kit parts */

            add_stock_move(25, $order_line->stock_id, $grn, $location, $date_, "",
            	$order_line->receive_qty, $order_line->standard_cost,
            	$po->supplier_id, 1, $order_line->price);

		} /*quantity received is != 0 */
	} /*end of order_line loop */

	references::save_last($reference, 25);

	commit_transaction();

	return $grn;
}

//----------------------------------------------------------------------------------------

function add_grn_batch($po_number, $supplier_id, $reference, $location, $date_)
{
	$date = date2sql($date_);

	$sql = "INSERT INTO ".TB_PREF."grn_batch (purch_order_no, delivery_date, supplier_id, reference, loc_code)
			VALUES ($po_number, '$date', '$supplier_id', '$reference', '$location')";

	db_query($sql, "A grn batch record could not be inserted.");

	return db_insert_id();
}

//-------------------------------------------------------------------------------------------------------------

function add_grn_detail_item($grn_batch_id, $po_detail_item, $item_code, $description, $standard_unit_cost,
	$quantity_received)
{
	$sql = "UPDATE ".TB_PREF."purch_order_details
        SET quantity_received = quantity_received + $quantity_received,
        std_cost_unit=$standard_unit_cost
        WHERE po_detail_item = $po_detail_item";

	db_query($sql, "a purchase order details record could not be updated. This receipt of goods has not been processed ");

	$sql = "INSERT INTO ".TB_PREF."grn_items (grn_batch_id, po_detail_item, item_code, description, qty_recd)
		VALUES ($grn_batch_id, $po_detail_item, '$item_code', '$description', $quantity_received)";

	db_query($sql, "A GRN detail item could not be inserted.");

	return db_insert_id();
}

//----------------------------------------------------------------------------------------

function get_grn_items($grn_batch_id=0, $supplier_id="", $outstanding_only=false,
	$is_invoiced_only=false)
{
    $sql = "SELECT ".TB_PREF."grn_batch.*, ".TB_PREF."grn_items.*, ".TB_PREF."purch_order_details.unit_price,
		".TB_PREF."purch_order_details.std_cost_unit, units
    	FROM ".TB_PREF."grn_batch, ".TB_PREF."grn_items, ".TB_PREF."purch_order_details, ".TB_PREF."stock_master
    	WHERE ".TB_PREF."grn_items.grn_batch_id=".TB_PREF."grn_batch.id
		AND ".TB_PREF."grn_items.po_detail_item=".TB_PREF."purch_order_details.po_detail_item
    	AND ".TB_PREF."stock_master.stock_id=".TB_PREF."grn_items.item_code ";

	if ($grn_batch_id != 0)
		$sql .= " AND ".TB_PREF."grn_batch.id=$grn_batch_id AND ".TB_PREF."grn_items.grn_batch_id=$grn_batch_id ";

	if ($is_invoiced_only)
		$sql .= " AND ".TB_PREF."grn_items.quantity_inv > 0";

	if ($outstanding_only)
    	$sql .= " AND ".TB_PREF."grn_items.qty_recd - ".TB_PREF."grn_items.quantity_inv > 0";

	if ($supplier_id != "")
		$sql .= " AND ".TB_PREF."grn_batch.supplier_id ='$supplier_id' ";

	$sql .= " ORDER BY ".TB_PREF."grn_batch.delivery_date, ".TB_PREF."grn_batch.id, ".TB_PREF."grn_items.id";

	return db_query($sql, "Could not retreive GRNS");
}

//----------------------------------------------------------------------------------------

// get the details for a given grn item

function get_grn_item_detail($grn_item_no)
{
	$sql = "SELECT ".TB_PREF."grn_items.*, ".TB_PREF."purch_order_details.unit_price,
    	".TB_PREF."grn_items.qty_recd - ".TB_PREF."grn_items.quantity_inv AS QtyOstdg,
    	".TB_PREF."purch_order_details.std_cost_unit
		FROM ".TB_PREF."grn_items, ".TB_PREF."purch_order_details, ".TB_PREF."stock_master
		WHERE ".TB_PREF."grn_items.po_detail_item=".TB_PREF."purch_order_details.po_detail_item
 			AND ".TB_PREF."stock_master.stock_id=".TB_PREF."grn_items.item_code
			AND ".TB_PREF."grn_items.id=$grn_item_no";

	$result = db_query($sql, "could not retreive grn item details");
	return db_fetch($result);
}

//----------------------------------------------------------------------------------------

function read_grn_items_to_order($grn_batch, &$order)
{
	$result = get_grn_items($grn_batch);

  	if (db_num_rows($result) > 0)
  	{

	 	while ($myrow = db_fetch($result))
	 	{

		 	if (is_null($myrow["units"]))
		 	{
				$units = "";
		 	}
		 	else
		 	{
				$units = $myrow["units"];
		 	}

			$order->add_to_order($order->lines_on_order+1, $myrow["item_code"],
				1,$myrow["description"], $myrow["unit_price"],$units,
				sql2date($myrow["delivery_date"]), $myrow["quantity_inv"],
				$myrow["qty_recd"]);

			$order->line_items[$order->lines_on_order]->po_detail_rec = $myrow["po_detail_item"];
		} /* line po from purchase order details */
  	} //end of checks on returned data set
}

//----------------------------------------------------------------------------------------

// read a grn into an order class

function read_grn($grn_batch, &$order)
{
	$sql= "SELECT *	FROM ".TB_PREF."grn_batch WHERE id=$grn_batch";

	$result = db_query($sql, "The grn sent is not valid");

	$row = db_fetch($result);
	$po_number = $row["purch_order_no"];

	$result = read_po_header($po_number, $order);

	if ($result)
	{

		$order->orig_order_date = sql2date($row["delivery_date"]);
		$order->location = $row["loc_code"];
		$order->reference = $row["reference"];

		read_grn_items_to_order($grn_batch, $order);
	}
}

//----------------------------------------------------------------------------------------------------------

// get the GRNs (batch info not details) for a given po number

function get_po_grns($po_number)
{
    $sql = "SELECT * FROM ".TB_PREF."grn_batch WHERE purch_order_no=$po_number";

	return db_query($sql, "The grns for the po $po_number could not be retreived");
}

//----------------------------------------------------------------------------------------------------------

function exists_grn($grn_batch)
{
	$sql = "SELECT id FROM ".TB_PREF."grn_batch WHERE id=$grn_batch";
	$result = db_query($sql, "Cannot retreive a grn");

    return (db_num_rows($result) > 0);
}

//----------------------------------------------------------------------------------------------------------

function exists_grn_on_invoices($grn_batch)
{
	$sql = "SELECT ".TB_PREF."supp_invoice_items.id FROM ".TB_PREF."supp_invoice_items,".TB_PREF."grn_items
		WHERE ".TB_PREF."supp_invoice_items.grn_item_id=".TB_PREF."grn_items.id
		AND quantity != 0
		AND grn_batch_id=$grn_batch";
	$result = db_query($sql, "Cannot query GRNs");

    return (db_num_rows($result) > 0);
}

//----------------------------------------------------------------------------------------------------------

function void_grn($grn_batch)
{
	// if this grn is references on any invoices/credit notes, then it
	// can't be voided
	if (exists_grn_on_invoices($grn_batch))
		return false;

	begin_transaction();

	void_bank_trans(25, $grn_batch, true);
	void_gl_trans(25, $grn_batch, true);

	// clear the quantities of the grn items in the POs and invoices
	$result = get_grn_items($grn_batch);

    if (db_num_rows($result) > 0)
    {

    	while ($myrow = db_fetch($result))
        {

        	$sql = "UPDATE ".TB_PREF."purch_order_details
                SET quantity_received = quantity_received - " . $myrow["qty_recd"] . "
                WHERE po_detail_item = " . $myrow["po_detail_item"];

        	db_query($sql, "a purchase order details record could not be voided.");
        }
    }

	// clear the quantities in the grn items
	$sql = "UPDATE ".TB_PREF."grn_items SET qty_recd=0, quantity_inv=0
		WHERE grn_batch_id=$grn_batch";

	db_query($sql, "A grn detail item could not be voided.");

    // clear the stock move items
    void_stock_move(25, $grn_batch);

	commit_transaction();

	return true;
}

//----------------------------------------------------------------------------------------------------------

?>