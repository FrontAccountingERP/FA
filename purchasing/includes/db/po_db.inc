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
//----------------------------------------------------------------------------------------

function delete_po($po)
{
	global $Refs;

	begin_transaction();
	hook_db_prevoid($po, ST_PURCHORDER);
	$sql = "DELETE FROM ".TB_PREF."purch_orders WHERE order_no=".db_escape($po);
	db_query($sql, "The order header could not be deleted");

	$sql = "DELETE FROM ".TB_PREF."purch_order_details WHERE order_no =".db_escape($po);
	db_query($sql, "The order detail lines could not be deleted");

	$Refs->restore_last(ST_PURCHORDER, $po);
	commit_transaction();
}

//----------------------------------------------------------------------------------------

function add_po(&$po_obj)
{
	global $Refs;

	begin_transaction();
	hook_db_prewrite($po_obj, ST_PURCHORDER);

     /*Insert to purchase order header record */
     $sql = "INSERT INTO ".TB_PREF."purch_orders (supplier_id, Comments, ord_date, reference, 
     	requisition_no, into_stock_location, delivery_address, total, tax_included) VALUES(";
     $sql .= db_escape($po_obj->supplier_id) . "," .
         db_escape($po_obj->Comments) . ",'" .
         date2sql($po_obj->orig_order_date) . "', " .
		 db_escape($po_obj->reference) . ", " .
         db_escape($po_obj->supp_ref) . ", " .
         db_escape($po_obj->Location) . ", " .
         db_escape($po_obj->delivery_address) . ", " .
         db_escape($po_obj->get_trans_total()). ", " .
         db_escape($po_obj->tax_included) . ")";

	db_query($sql, "The purchase order header record could not be inserted");

     /*Get the auto increment value of the order number created from the sql above */
     $po_obj->order_no = db_insert_id();

     /*Insert the purchase order detail records */
     foreach ($po_obj->line_items as $line_no => $po_line)
     {
		$sql = "INSERT INTO ".TB_PREF."purch_order_details (order_no, item_code, description, delivery_date,	unit_price,	quantity_ordered) VALUES (";
		$sql .= $po_obj->order_no . ", " . db_escape($po_line->stock_id). "," .
		db_escape($po_line->item_description). ",'" .
		date2sql($po_line->req_del_date) . "'," .
		db_escape($po_line->price) . ", " .
		db_escape($po_line->quantity). ")";
		db_query($sql, "One of the purchase order detail records could not be inserted");
		$po_obj->line_items[$line_no]->po_detail_rec = db_insert_id();
     }

	$Refs->save(ST_PURCHORDER, $po_obj->order_no, $po_obj->reference);

	//add_comments(ST_PURCHORDER, $po_obj->order_no, $po_obj->orig_order_date, $po_obj->Comments);

	add_audit_trail(ST_PURCHORDER, $po_obj->order_no, $po_obj->orig_order_date);
	hook_db_postwrite($po_obj, ST_PURCHORDER);
	commit_transaction();

	return $po_obj->order_no;
}

//----------------------------------------------------------------------------------------

function update_po(&$po_obj)
{
	begin_transaction();
	hook_db_prewrite($po_obj, ST_PURCHORDER);

    /*Update the purchase order header with any changes */
    $sql = "UPDATE ".TB_PREF."purch_orders SET Comments=" . db_escape($po_obj->Comments) . ",
		requisition_no= ". db_escape( $po_obj->supp_ref). ",
		into_stock_location=" . db_escape($po_obj->Location). ",
		ord_date='" . date2sql($po_obj->orig_order_date) . "',
		delivery_address=" . db_escape($po_obj->delivery_address).",
		total=". db_escape($po_obj->get_trans_total()).",
		tax_included=". db_escape($po_obj->tax_included);
    $sql .= " WHERE order_no = " . $po_obj->order_no;
	db_query($sql, "The purchase order could not be updated");

	$sql = "DELETE FROM ".TB_PREF."purch_order_details WHERE order_no="
		.db_escape($po_obj->order_no);
	db_query($sql, "could not delete old purch order details");

    /*Now Update the purchase order detail records */
    foreach ($po_obj->line_items as $po_line)
    {
    	$sql = "INSERT INTO ".TB_PREF."purch_order_details (po_detail_item, order_no, item_code, 
    		description, delivery_date, unit_price, quantity_ordered, quantity_received) VALUES ("
			.db_escape($po_line->po_detail_rec ? $po_line->po_detail_rec : 0). ","
			.$po_obj->order_no . ","
			.db_escape($po_line->stock_id). ","
			.db_escape($po_line->item_description). ",'"
			.date2sql($po_line->req_del_date) . "',"
			.db_escape($po_line->price) . ", "
			.db_escape($po_line->quantity) . ", "
			.db_escape($po_line->qty_received) . ")";
		
		db_query($sql, "One of the purchase order detail records could not be updated");
    }

	// add_comments(ST_PURCHORDER, $po_obj->order_no, $po_obj->orig_order_date, $po_obj->Comments);

	add_audit_trail($po_obj->trans_type, $po_obj->order_no, Today(), _("Updated."));
	hook_db_postwrite($po_obj, ST_PURCHORDER);
	commit_transaction();

	return $po_obj->order_no;
}

//----------------------------------------------------------------------------------------

function read_po_header($order_no, &$order)
{
   	$sql = "SELECT ".TB_PREF."purch_orders.*, ".TB_PREF."suppliers.supp_name, ".TB_PREF."suppliers.tax_group_id,
   		".TB_PREF."suppliers.curr_code, ".TB_PREF."locations.location_name
		FROM ".TB_PREF."purch_orders, ".TB_PREF."suppliers, ".TB_PREF."locations
		WHERE ".TB_PREF."purch_orders.supplier_id = ".TB_PREF."suppliers.supplier_id
		AND ".TB_PREF."locations.loc_code = into_stock_location
		AND ".TB_PREF."purch_orders.order_no = ".db_escape($order_no);

   	$result = db_query($sql, "The order cannot be retrieved");

	if (db_num_rows($result) == 1)
	{

      	$myrow = db_fetch($result);

      	$order->trans_type = ST_PURCHORDER;
      	$order->order_no = $order_no;
      	
      	$order->set_supplier($myrow["supplier_id"], $myrow["supp_name"], $myrow["curr_code"],
      		$myrow['tax_group_id'], $myrow["tax_included"]);

		$order->credit = get_current_supp_credit($order->supplier_id);

      	$order->orig_order_date = sql2date($myrow["ord_date"]);
      	$order->Comments = nl2br($myrow["comments"]);
      	$order->Location = $myrow["into_stock_location"];
      	$order->supp_ref = $myrow["requisition_no"];
      	$order->reference = $myrow["reference"];
      	$order->delivery_address = $myrow["delivery_address"];

      	return true;
	}

	display_db_error("FATAL : duplicate purchase order found", "", true);
	return false;
}

//----------------------------------------------------------------------------------------

function read_po_items($order_no, &$order, $open_items_only=false)
{
	/*now populate the line po array with the purchase order details records */

	$sql = "SELECT ".TB_PREF."purch_order_details.*, units
		FROM ".TB_PREF."purch_order_details
		LEFT JOIN ".TB_PREF."stock_master
		ON ".TB_PREF."purch_order_details.item_code=".TB_PREF."stock_master.stock_id
		WHERE order_no =".db_escape($order_no);

    if ($open_items_only)
 		$sql .= " AND (".TB_PREF."purch_order_details.quantity_ordered > ".TB_PREF."purch_order_details.quantity_received) ";

	$sql .= " ORDER BY po_detail_item";

	$result = db_query($sql, "The lines on the purchase order cannot be retrieved");

    if (db_num_rows($result) > 0)
    {
		while ($myrow = db_fetch($result))
        {
        	$data = get_purchase_data($order->supplier_id, $myrow['item_code']);
        	if ($data !== false)
        	{
        		if ($data['supplier_description'] != "")
        			$myrow['description'] = $data['supplier_description'];
        		//if ($data['suppliers_uom'] != "")
        		//	$myrow['units'] = $data['suppliers_uom'];
        	}		
            if (is_null($myrow["units"]))
            {
		       	$units = "";
            }
            else
            {
                $units = $myrow["units"];
            }

            if ($order->add_to_order($order->lines_on_order, $myrow["item_code"],
            	$myrow["quantity_ordered"],$myrow["description"],
            	$myrow["unit_price"],$units, sql2date($myrow["delivery_date"]),
            	$myrow["qty_invoiced"], $myrow["quantity_received"])) {
            		$newline = &$order->line_items[$order->lines_on_order-1];
					$newline->po_detail_rec = $myrow["po_detail_item"];
					$newline->standard_cost = $myrow["std_cost_unit"];  /*Needed for receiving goods and GL interface */
				// set for later GRN edition
//	   	    	$newline->receive_qty = $newline->quantity - $newline->qty_dispatched;
			}
        } /* line po from purchase order details */
    } //end of checks on returned data set
}

//----------------------------------------------------------------------------------------

function read_po($order_no, &$order, $open_items_only=false)
{
	$result = read_po_header($order_no, $order);

	if ($result)
		read_po_items($order_no, $order, $open_items_only);
}

//----------------------------------------------------------------------------------------

function get_po_items($order_no)
{
	$sql = "SELECT item_code, quantity_ordered, quantity_received, qty_invoiced
		FROM ".TB_PREF."purch_order_details
		WHERE order_no=".db_escape($order_no)
		." ORDER BY po_detail_item";

	$result = db_query($sql, "could not query purch order details");
    check_db_error("Could not check that the details of the purchase order had not been changed by another user ", $sql);
    return $result;
}
//----------------------------------------------------------------------------------------

function get_short_info($stock_id)
{
	$sql = "SELECT description, units, mb_flag
		FROM ".TB_PREF."stock_master WHERE stock_id = ".db_escape($stock_id);

	return db_query($sql,"The stock details for " . $stock_id . " could not be retrieved");
}

function get_sql_for_po_search_completed($supplier_id=ALL_TEXT)
{
	global $order_number, $selected_stock_item;;

	$sql = "SELECT 
		porder.order_no, 
		porder.reference, 
		supplier.supp_name, 
		location.location_name,
		porder.requisition_no, 
		porder.ord_date, 
		supplier.curr_code, 
		Sum(line.unit_price*line.quantity_ordered) AS OrderValue,
		porder.into_stock_location
		FROM ".TB_PREF."purch_orders as porder, "
			.TB_PREF."purch_order_details as line, "
			.TB_PREF."suppliers as supplier, "
			.TB_PREF."locations as location
		WHERE porder.order_no = line.order_no
		AND porder.supplier_id = supplier.supplier_id
		AND location.loc_code = porder.into_stock_location ";

	if (isset($_GET['supplier_id']))
		$sql .= "AND supplier.supplier_id=".@$_GET['supplier_id']." ";
	if (isset($order_number) && $order_number != "")
	{
		$sql .= "AND porder.reference LIKE ".db_escape('%'. $order_number . '%');
	}
	else
	{

		$data_after = date2sql($_POST['OrdersAfterDate']);
		$date_before = date2sql($_POST['OrdersToDate']);

		$sql .= " AND porder.ord_date >= '$data_after'";
		$sql .= " AND porder.ord_date <= '$date_before'";

		if (isset($_POST['StockLocation']) && $_POST['StockLocation'] != ALL_TEXT)
		{
			$sql .= " AND porder.into_stock_location = ".db_escape($_POST['StockLocation']);
		}
		if (isset($selected_stock_item))
		{
			$sql .= " AND line.item_code=".db_escape($selected_stock_item);
		}
		if ($supplier_id != ALL_TEXT)
			$sql .= " AND supplier.supplier_id=".db_escape($supplier_id);
		
	} //end not order number selected

	$sql .= " GROUP BY porder.order_no";
	return $sql;
}	

function get_sql_for_po_search($supplier_id=ALL_TEXT)
{
	global $all_items, $order_number, $selected_stock_item;;
	
	$sql = "SELECT 
		porder.order_no, 
		porder.reference,
		supplier.supp_name, 
		location.location_name,
		porder.requisition_no, 
		porder.ord_date,
		supplier.curr_code,
		Sum(line.unit_price*line.quantity_ordered) AS OrderValue,
		Sum(line.delivery_date < '". date2sql(Today()) ."'
		AND (line.quantity_ordered > line.quantity_received)) As OverDue
		FROM "
			.TB_PREF."purch_orders as porder, "
			.TB_PREF."purch_order_details as line, "
			.TB_PREF."suppliers as supplier, "
			.TB_PREF."locations as location
		WHERE porder.order_no = line.order_no
		AND porder.supplier_id = supplier.supplier_id
		AND location.loc_code = porder.into_stock_location
		AND (line.quantity_ordered > line.quantity_received) ";

	if (isset($order_number) && $order_number != "")
	{
		$sql .= "AND porder.reference LIKE ".db_escape('%'. $order_number . '%');
	}
	else
	{
		$data_after = date2sql($_POST['OrdersAfterDate']);
		$data_before = date2sql($_POST['OrdersToDate']);

		$sql .= "  AND porder.ord_date >= '$data_after'";
		$sql .= "  AND porder.ord_date <= '$data_before'";

		if (isset($_POST['StockLocation']) && $_POST['StockLocation'] != $all_items)
		{
			$sql .= " AND porder.into_stock_location = ".db_escape($_POST['StockLocation']);
		}

		if (isset($selected_stock_item))
		{
			$sql .= " AND line.item_code=".db_escape($selected_stock_item);
		}
		if ($supplier_id != ALL_TEXT)
			$sql .= " AND supplier.supplier_id=".db_escape($supplier_id);
	} //end not order number selected

	$sql .= " GROUP BY porder.order_no";
	return $sql;
}
?>