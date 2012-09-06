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
function add_sales_order(&$order)
{
	global $loc_notification, $path_to_root, $Refs;

	begin_transaction();
	hook_db_prewrite($order, $order->trans_type);
	$order_no = get_next_trans_no($order->trans_type);
	$del_date = date2sql($order->due_date);
	$order_type = 0; // this is default on new order
	$total = $order->get_trans_total();
	$sql = "INSERT INTO ".TB_PREF."sales_orders (order_no, type, debtor_no, trans_type, branch_code, customer_ref, reference, comments, ord_date,
		order_type, ship_via, deliver_to, delivery_address, contact_phone,
		freight_cost, from_stk_loc, delivery_date, payment_terms, total)
		VALUES (" .db_escape($order_no) . "," .db_escape($order_type) . "," . db_escape($order->customer_id) .
		 ", " .db_escape($order->trans_type) . "," .db_escape($order->Branch) . ", ".
			db_escape($order->cust_ref) .",". 
			db_escape($order->reference) .",". 
			db_escape($order->Comments) .",'" . 
			date2sql($order->document_date) . "', " .
			db_escape($order->sales_type) . ", " .
			db_escape($order->ship_via)."," . 
			db_escape($order->deliver_to) . "," .
			db_escape($order->delivery_address) . ", " .
			db_escape($order->phone) . ", " . 
			db_escape($order->freight_cost) .", " . 
			db_escape($order->Location) .", " .
			db_escape($del_date) . "," .
			db_escape($order->payment) . "," .
			db_escape($total). ")";

	db_query($sql, "order Cannot be Added");

	$order->trans_no = array($order_no=>0);

	if ($loc_notification == 1)
	{
		include_once($path_to_root . "/inventory/includes/inventory_db.inc");
		$st_ids = array();
		$st_names = array();
		$st_num = array();
		$st_reorder = array();
	}
	foreach ($order->line_items as $line)
	{
		if ($loc_notification == 1 && is_inventory_item($line->stock_id))
		{
			$sql = "SELECT ".TB_PREF."loc_stock.*, ".TB_PREF."locations.location_name, ".TB_PREF."locations.email
				FROM ".TB_PREF."loc_stock, ".TB_PREF."locations
				WHERE ".TB_PREF."loc_stock.loc_code=".TB_PREF."locations.loc_code
				AND ".TB_PREF."loc_stock.stock_id = '" . $line->stock_id . "'
				AND ".TB_PREF."loc_stock.loc_code = '" . $order->Location . "'";
			$res = db_query($sql,"a location could not be retreived");
			$loc = db_fetch($res);
			if ($loc['email'] != "")
			{
				$qoh = get_qoh_on_date($line->stock_id, $order->Location);
				$qoh -= get_demand_qty($line->stock_id, $order->Location);
				$qoh -= get_demand_asm_qty($line->stock_id, $order->Location);
				$qoh -= $line->quantity;
				if ($qoh < $loc['reorder_level'])
				{
					$st_ids[] = $line->stock_id;
					$st_names[] = $line->item_description;
					$st_num[] = $qoh - $loc['reorder_level'];
					$st_reorder[] = $loc['reorder_level'];
				}
			}
		}

		$sql = "INSERT INTO ".TB_PREF."sales_order_details (order_no, trans_type, stk_code, description, unit_price, quantity, discount_percent) VALUES (";
		$sql .= $order_no . ",".$order->trans_type .
				",".db_escape($line->stock_id).", "
				.db_escape($line->item_description).", $line->price,
				$line->quantity,
				$line->discount_percent)";
		db_query($sql, "order Details Cannot be Added");

	// Now mark quotation line as processed
		if ($order->trans_type == ST_SALESORDER && $line->src_id)
			update_parent_line(ST_SALESORDER, $line->src_id, $line->qty_dispatched); // clear all the quote despite all or the part was ordered
	} /* inserted line items into sales order details */
	add_audit_trail($order->trans_type, $order_no, $order->document_date);
	$Refs->save($order->trans_type, $order_no, $order->reference);

	hook_db_postwrite($order, $order->trans_type);
	commit_transaction();

	if ($loc_notification == 1 && count($st_ids) > 0)
	{
		require_once($path_to_root . "/reporting/includes/class.mail.inc");
		$company = get_company_prefs();
		$mail = new email($company['coy_name'], $company['email']);
		$from = $company['coy_name'] . " <" . $company['email'] . ">";
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
		$ret = $mail->send();
	}
	return $order_no;
}

//----------------------------------------------------------------------------------------

function delete_sales_order($order_no, $trans_type)
{
	begin_transaction();
	hook_db_prevoid($trans_type, $order_no);

	$sql = "DELETE FROM ".TB_PREF."sales_orders WHERE order_no=" . db_escape($order_no) 
		. " AND trans_type=".db_escape($trans_type);

	db_query($sql, "order Header Delete");

	$sql = "DELETE FROM ".TB_PREF."sales_order_details WHERE order_no =" 
		.db_escape($order_no) . " AND trans_type=".db_escape($trans_type);
	db_query($sql, "order Detail Delete");

	delete_reference($trans_type, $order_no);

	add_audit_trail($trans_type, $order_no, Today(), _("Deleted."));
	commit_transaction();
}

//----------------------------------------------------------------------------------------
// Mark changes in sales_order_details
//
function update_sales_order_version($order)
{
  foreach ($order as $so_num => $so_ver) {
  $sql= 'UPDATE '.TB_PREF.'sales_orders SET version=version+1 WHERE order_no='. db_escape($so_num).
	' AND version='.$so_ver . " AND trans_type=30";
  db_query($sql, 'Concurrent editing conflict while sales order update');
  }
}

//----------------------------------------------------------------------------------------

function update_sales_order($order)
{
	global $loc_notification, $path_to_root, $Refs;

	$del_date = date2sql($order->due_date);
	$ord_date = date2sql($order->document_date);
	$order_no =  key($order->trans_no);
	$version= current($order->trans_no);
	$total = $order->get_trans_total();

	begin_transaction();

	$sql = "UPDATE ".TB_PREF."sales_orders SET type =".db_escape($order->so_type)." ,
		debtor_no = " . db_escape($order->customer_id) . ",
		branch_code = " . db_escape($order->Branch) . ",
		customer_ref = ". db_escape($order->cust_ref) .",
		reference = ". db_escape($order->reference) .",
		comments = ". db_escape($order->Comments) .",
		ord_date = " . db_escape($ord_date) . ",
		order_type = " .db_escape($order->sales_type) . ",
		ship_via = " . db_escape($order->ship_via) .",
		deliver_to = " . db_escape($order->deliver_to) . ",
		delivery_address = " . db_escape($order->delivery_address) . ",
		contact_phone = " .db_escape($order->phone) . ",
		freight_cost = " .db_escape($order->freight_cost) .",
		from_stk_loc = " .db_escape($order->Location) .",
		delivery_date = " .db_escape($del_date). ",
		version = ".($version+1).",
		payment_terms = " .db_escape($order->payment). ",
		total = ". db_escape($total) ."
	 WHERE order_no=" . db_escape($order_no) ."
	 AND trans_type=".$order->trans_type." AND version=".$version;
	db_query($sql, "order Cannot be Updated, this can be concurrent edition conflict");

	$sql = "DELETE FROM ".TB_PREF."sales_order_details WHERE order_no =" . db_escape($order_no) . " AND trans_type=".$order->trans_type;

	db_query($sql, "Old order Cannot be Deleted");

	if ($loc_notification == 1)
	{
		include_once($path_to_root . "/inventory/includes/inventory_db.inc");
		$st_ids = array();
		$st_names = array();
		$st_num = array();
		$st_reorder = array();
	}
	foreach ($order->line_items as $line)
	{
		if ($loc_notification == 1 && is_inventory_item($line->stock_id))
		{
			$sql = "SELECT ".TB_PREF."loc_stock.*, "
				  .TB_PREF."locations.location_name, "
				  .TB_PREF."locations.email
				FROM ".TB_PREF."loc_stock, "
				  .TB_PREF."locations
				WHERE ".TB_PREF."loc_stock.loc_code=".TB_PREF."locations.loc_code
				 AND ".TB_PREF."loc_stock.stock_id = ".db_escape($line->stock_id)."
				 AND ".TB_PREF."loc_stock.loc_code = ".db_escape($order->Location);
			$res = db_query($sql,"a location could not be retreived");
			$loc = db_fetch($res);
			if ($loc['email'] != "")
			{
				$qoh = get_qoh_on_date($line->stock_id, $order->Location);
				$qoh -= get_demand_qty($line->stock_id, $order->Location);
				$qoh -= get_demand_asm_qty($line->stock_id, $order->Location);
				$qoh -= $line->quantity;
				if ($qoh < $loc['reorder_level'])
				{
					$st_ids[] = $line->stock_id;
					$st_names[] = $line->item_description;
					$st_num[] = $qoh - $loc['reorder_level'];
					$st_reorder[] = $loc['reorder_level'];
				}
			}
		}
		$sql = "INSERT INTO ".TB_PREF."sales_order_details
		 (id, order_no, trans_type, stk_code,  description, unit_price, quantity,
		  discount_percent, qty_sent)
		 VALUES (";
		$sql .= db_escape($line->id ? $line->id : 0) . ","
		  .$order_no . ",".$order->trans_type.","
		  .db_escape($line->stock_id) . ","
		  .db_escape($line->item_description) . ", "
		  .db_escape($line->price) . ", "
		  .db_escape($line->quantity) . ", "
		  .db_escape($line->discount_percent) . ", "
		  .db_escape($line->qty_done) ." )";

		db_query($sql, "Old order Cannot be Inserted");

	} /* inserted line items into sales order details */

	add_audit_trail($order->trans_type, $order_no, $order->document_date, _("Updated."));
	$Refs->save($order->trans_type, $order_no, $order->reference);
	commit_transaction();
	if ($loc_notification == 1 && count($st_ids) > 0)
	{
		require_once($path_to_root . "/reporting/includes/class.mail.inc");
		$company = get_company_prefs();
		$mail = new email($company['coy_name'], $company['email']);
		$from = $company['coy_name'] . " <" . $company['email'] . ">";
		$to = $loc['location_name'] . " <" . $loc['email'] . ">";
		$subject = _("Stocks below Re-Order Level at " . $loc['location_name']);
		$msg = "\n";
		for ($i = 0; $i < count($st_ids); $i++)
			$msg .= $st_ids[$i] . " " . $st_names[$i] . ", "
			  . _("Re-Order Level") . ": " . $st_reorder[$i] . ", "
			  . _("Below") . ": " . $st_num[$i] . "\n";
		$msg .= "\n" . _("Please reorder") . "\n\n";
		$msg .= $company['coy_name'];
		$mail->to($to);
		$mail->subject($subject);
		$mail->text($msg);
		$ret = $mail->send();
	}
}

//----------------------------------------------------------------------------------------

function get_sales_order_header($order_no, $trans_type)
{
	$sql = "SELECT sorder.*, "
	  ."cust.name, "
	  ."cust.curr_code, "
	  ."loc.location_name, "
	  ."cust.discount, "
	  ."stype.sales_type, "
	  ."stype.id AS sales_type_id, "
	  ."stype.tax_included, "
	  ."stype.factor, "
 	  ."ship.shipper_name, "
	  ."tax_group.name AS tax_group_name , "
	  ."tax_group.id AS tax_group_id, "
	  ."cust.tax_id "
	."FROM ".TB_PREF."sales_orders sorder LEFT JOIN ".TB_PREF."shippers ship ON  ship.shipper_id = sorder.ship_via,"
	  .TB_PREF."debtors_master cust,"
	  .TB_PREF."sales_types stype, "
	  .TB_PREF."tax_groups tax_group, "
	  .TB_PREF."cust_branch branch,"
	  .TB_PREF."locations loc
	WHERE sorder.order_type=stype.id
		AND branch.branch_code = sorder.branch_code
		AND branch.tax_group_id = tax_group.id
		AND sorder.debtor_no = cust.debtor_no
		AND loc.loc_code = sorder.from_stk_loc
		AND sorder.trans_type = " . db_escape($trans_type) ."
		AND sorder.order_no = " . db_escape($order_no );

	$result = db_query($sql, "order Retreival");

	$num = db_num_rows($result);
	if ($num > 1)
	{
		display_warning("You have duplicate document in database: (type:$trans_type, number:$order_no).");
	}
	else if ($num == 1)
	{
		return db_fetch($result);
	}
	else
		display_warning("You have missing or invalid sales document in database (type:$trans_type, number:$order_no).");

}

//----------------------------------------------------------------------------------------

function get_sales_order_details($order_no, $trans_type) {
	$sql = "SELECT id, stk_code, unit_price, "
		.TB_PREF."sales_order_details.description,"
		.TB_PREF."sales_order_details.quantity,
		  discount_percent,
		  qty_sent as qty_done, "
		.TB_PREF."stock_master.units,"
		.TB_PREF."stock_master.mb_flag,"
		.TB_PREF."stock_master.material_cost + "
			.TB_PREF."stock_master.labour_cost + "
			.TB_PREF."stock_master.overhead_cost AS standard_cost
	FROM ".TB_PREF."sales_order_details, ".TB_PREF."stock_master
	WHERE ".TB_PREF."sales_order_details.stk_code = ".TB_PREF."stock_master.stock_id
	AND order_no =" . db_escape($order_no) 
		." AND trans_type = " . db_escape($trans_type) . " ORDER BY id";

	return db_query($sql, "Retreive order Line Items");
}
//----------------------------------------------------------------------------------------

function read_sales_order($order_no, &$order, $trans_type)
{

	$myrow = get_sales_order_header($order_no, $trans_type);

	$order->trans_type = $myrow['trans_type'];
	$order->so_type =  $myrow["type"];
	$order->trans_no = array($order_no=> $myrow["version"]);

	$order->set_customer($myrow["debtor_no"], $myrow["name"],
	  $myrow["curr_code"], $myrow["discount"], $myrow["payment_terms"]);

	$order->set_branch($myrow["branch_code"], $myrow["tax_group_id"],
	  $myrow["tax_group_name"], $myrow["contact_phone"]);

	$order->set_sales_type($myrow["sales_type_id"], $myrow["sales_type"], 
	    $myrow["tax_included"], $myrow["factor"]); // no default price calculations on edit

	$order->set_location($myrow["from_stk_loc"], $myrow["location_name"]);

	$order->set_delivery($myrow["ship_via"], $myrow["deliver_to"],
	  $myrow["delivery_address"], $myrow["freight_cost"]);

	$order->cust_ref = $myrow["customer_ref"];
	$order->sales_type =$myrow["order_type"];
	$order->reference = $myrow["reference"];
	$order->Comments = $myrow["comments"];
	$order->due_date = sql2date($myrow["delivery_date"]);
	$order->document_date = sql2date($myrow["ord_date"]);

	$result = get_sales_order_details($order_no, $order->trans_type);
	if (db_num_rows($result) > 0)
	{
		$line_no=0;
		while ($myrow = db_fetch($result))
		{
			$order->add_to_cart($line_no,$myrow["stk_code"],$myrow["quantity"],
				$myrow["unit_price"], $myrow["discount_percent"],
				$myrow["qty_done"], $myrow["standard_cost"], $myrow["description"], $myrow["id"] );
		$line_no++;
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------

function sales_order_has_deliveries($order_no)
{
	$sql = "SELECT SUM(qty_sent) FROM ".TB_PREF.
	"sales_order_details WHERE order_no=".db_escape($order_no)
	." AND trans_type=".ST_SALESORDER;

	$result = db_query($sql, "could not query for sales order usage");

	$row = db_fetch_row($result);

	if ($row[0] > 0)
		return true;  // 2010-04-21 added check for eventually voided deliveries, Joe Hunt
	$sql = "SELECT order_ FROM ".TB_PREF."debtor_trans WHERE type=".ST_CUSTDELIVERY." AND order_=".db_escape($order_no);
	$result = db_query($sql,"The related delivery notes could not be retreived");
	return (db_num_rows($result) > 0);	
}

//----------------------------------------------------------------------------------------

function close_sales_order($order_no)
{
	// set the quantity of each item to the already sent quantity. this will mark item as closed.
	$sql = "UPDATE ".TB_PREF."sales_order_details
		SET quantity = qty_sent WHERE order_no = ".db_escape($order_no)
		." AND trans_type=".ST_SALESORDER;

	db_query($sql, "The sales order detail record could not be updated");
}

//---------------------------------------------------------------------------------------------------------------

function get_invoice_duedate($terms, $invdate)
{
	if (!is_date($invdate))
	{
		return new_doc_date();
	}
	
	$myrow = get_payment_terms($terms);
	
	if (!$myrow)
		return $invdate;

	if ($myrow['day_in_following_month'] > 0)
		$duedate = add_days(end_month($invdate), $myrow['day_in_following_month']);
	else
		$duedate = add_days($invdate, $myrow['days_before_due']);
	return $duedate;
}

function get_customer_to_order($customer_id) {

	// Now check to ensure this account is not on hold */
	$sql = "SELECT cust.name, 
		  cust.address, "
		  .TB_PREF."credit_status.dissallow_invoices, 
		  cust.sales_type AS salestype,
		  cust.dimension_id,
		  cust.dimension2_id,
		  stype.sales_type,
		  stype.tax_included,
		  stype.factor,
		  cust.curr_code,
		  cust.discount,
		  cust.payment_terms,
		  cust.pymt_discount,
		  cust.credit_limit - Sum(IFNULL(IF(trans.type=11 OR trans.type=12 OR trans.type=2,
			-1, 1) * (ov_amount + ov_gst + ov_freight + ov_freight_tax + ov_discount),0)) as cur_credit
		FROM ".TB_PREF."debtors_master cust
		  LEFT JOIN ".TB_PREF."debtor_trans trans ON trans.type!=".ST_CUSTDELIVERY." AND trans.debtor_no = cust.debtor_no,"
		  .TB_PREF."credit_status, "
		  .TB_PREF."sales_types stype
		WHERE cust.sales_type=stype.id
			AND cust.credit_status=".TB_PREF."credit_status.id
			AND cust.debtor_no = ".db_escape($customer_id)
		." GROUP by cust.debtor_no";

	$result =db_query($sql,"Customer Record Retreive");
	return 	db_fetch($result);
}

function get_branch_to_order($customer_id, $branch_id) {

    	// the branch was also selected from the customer selection so default the delivery details from the customer branches table cust_branch. The order process will ask for branch details later anyway
	 	$sql = "SELECT ".TB_PREF."cust_branch.br_name, "
			.TB_PREF."cust_branch.br_address, "
			.TB_PREF."cust_branch.br_post_address, "
			." default_location, location_name, default_ship_via, "
			.TB_PREF."tax_groups.name AS tax_group_name, "
			.TB_PREF."tax_groups.id AS tax_group_id
			FROM ".TB_PREF."cust_branch, "
			  .TB_PREF."tax_groups, "
			  .TB_PREF."locations
			WHERE ".TB_PREF."cust_branch.tax_group_id = ".TB_PREF."tax_groups.id
				AND ".TB_PREF."locations.loc_code=default_location
				AND ".TB_PREF."cust_branch.branch_code=".db_escape($branch_id)."
				AND ".TB_PREF."cust_branch.debtor_no = ".db_escape($customer_id);

  	    return db_query($sql,"Customer Branch Record Retreive");
}

function get_sql_for_sales_orders_view($selected_customer, $trans_type, $trans_no, $filter, 
	$stock_item=null, $from='', $to='', $ref='', $location='', $customer_id=ALL_TEXT)
{

	$sql = "SELECT 
			sorder.order_no,
			sorder.reference,
			debtor.name,
			branch.br_name,"
			.($filter=='InvoiceTemplates' 
				|| $filter=='DeliveryTemplates' ?
			 "sorder.comments, " : "sorder.customer_ref, ")
			."sorder.ord_date,
			sorder.delivery_date,
			sorder.deliver_to,
			Sum(line.unit_price*line.quantity*(1-line.discount_percent))+freight_cost AS OrderValue,
			sorder.type,
			debtor.curr_code,
			Sum(line.qty_sent) AS TotDelivered,
			Sum(line.quantity) AS TotQuantity
		FROM ".TB_PREF."sales_orders as sorder, "
			.TB_PREF."sales_order_details as line, "
			.TB_PREF."debtors_master as debtor, "
			.TB_PREF."cust_branch as branch
			WHERE sorder.order_no = line.order_no
			AND sorder.trans_type = line.trans_type
			AND sorder.trans_type = ".db_escape($trans_type)."
			AND sorder.debtor_no = debtor.debtor_no
			AND sorder.branch_code = branch.branch_code
			AND debtor.debtor_no = branch.debtor_no";

	if (isset($trans_no) && $trans_no != "")
	{
		// search orders with number like 
		$number_like = "%".$trans_no;
		$sql .= " AND sorder.order_no LIKE ".db_escape($number_like);
//				." GROUP BY sorder.order_no";
	}
	elseif ($ref != "")
	{
		// search orders with reference like 
		$number_like = "%".$ref."%";
		$sql .= " AND sorder.reference LIKE ".db_escape($number_like);
//				." GROUP BY sorder.order_no";
	}
	else	// ... or select inquiry constraints
	{
		if ($filter!='DeliveryTemplates' && $filter!='InvoiceTemplates' && $filter!='OutstandingOnly')
		{
			$date_after = date2sql($from);
			$date_before = date2sql($to);

			$sql .=  " AND sorder.ord_date >= '$date_after'"
					." AND sorder.ord_date <= '$date_before'";
		}
	}
		if ($trans_type == ST_SALESQUOTE && !check_value('show_all'))
			$sql .= " AND sorder.delivery_date >= '".date2sql(Today())."' AND line.qty_sent=0"; // show only outstanding, not realized quotes

		if ($selected_customer != -1)
			$sql .= " AND sorder.debtor_no=".db_escape($selected_customer);

		if (isset($stock_item))
			$sql .= " AND line.stk_code=".db_escape($stock_item);

		if ($location)
			$sql .= " AND sorder.from_stk_loc = ".db_escape($location);

		if ($filter=='OutstandingOnly')
			$sql .= " AND line.qty_sent < line.quantity";

		elseif ($filter=='InvoiceTemplates' || $filter=='DeliveryTemplates')
			$sql .= " AND sorder.type=1";
			
		//Chaiatanya : New Filter
		if ($customer_id != ALL_TEXT)
			$sql .= " AND sorder.debtor_no = ".db_escape($customer_id);		

		$sql .= " GROUP BY sorder.order_no,
					sorder.debtor_no,
					sorder.branch_code,
					sorder.customer_ref,
					sorder.ord_date,
					sorder.deliver_to";
	return $sql;
}
?>