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

	$order_no = get_next_trans_no($order->trans_type);
	$del_date = date2sql($order->due_date);
	$order_type = 0; // this is default on new order
	$sql = "INSERT INTO ".TB_PREF."sales_orders (order_no, type, debtor_no, trans_type, branch_code, customer_ref, reference, comments, ord_date,
		order_type, ship_via, deliver_to, delivery_address, contact_phone,
		contact_email, freight_cost, from_stk_loc, delivery_date)
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
			db_escape($order->email) . ", " .
			db_escape($order->freight_cost) .", " . 
			db_escape($order->Location) .", " .
			db_escape($del_date) . ")";

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

	} /* inserted line items into sales order details */

	add_audit_trail($order->trans_type, $order_no, $order->document_date);
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
  $sql= 'UPDATE '.TB_PREF.'sales_orders SET version=version+1 WHERE order_no='. $so_num.
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
		contact_email = " .db_escape($order->email) . ",
		freight_cost = " .db_escape($order->freight_cost) .",
		from_stk_loc = " .db_escape($order->Location) .",
		delivery_date = " .db_escape($del_date). ",
		version = ".($version+1)."
	 WHERE order_no=" . $order_no ."
	 AND trans_type=".$order->trans_type." AND version=".$version;
	db_query($sql, "order Cannot be Updated, this can be concurrent edition conflict");

	$sql = "DELETE FROM ".TB_PREF."sales_order_details WHERE order_no =" . $order_no . " AND trans_type=".$order->trans_type;

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
	$Refs->delete($order->trans_type, $order_no);
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
	$sql = "SELECT ".TB_PREF."sales_orders.*, "
	  .TB_PREF."debtors_master.name, "
	  .TB_PREF."debtors_master.curr_code, "
	  .TB_PREF."debtors_master.email AS master_email, "
	  .TB_PREF."locations.location_name, "
	  .TB_PREF."debtors_master.payment_terms, "
	  .TB_PREF."debtors_master.discount, "
	  .TB_PREF."sales_types.sales_type, "
	  .TB_PREF."sales_types.id AS sales_type_id, "
	  .TB_PREF."sales_types.tax_included, "
	  .TB_PREF."shippers.shipper_name, "
	  .TB_PREF."tax_groups.name AS tax_group_name , "
	  .TB_PREF."tax_groups.id AS tax_group_id
	FROM ".TB_PREF."sales_orders, "
	  .TB_PREF."debtors_master, "
	  .TB_PREF."sales_types, "
	  .TB_PREF."tax_groups, "
	  .TB_PREF."cust_branch, "
	  .TB_PREF."locations, "
	  .TB_PREF."shippers
	WHERE ".TB_PREF."sales_orders.order_type=".TB_PREF."sales_types.id
		AND ".TB_PREF."cust_branch.branch_code = ".TB_PREF."sales_orders.branch_code
		AND ".TB_PREF."cust_branch.tax_group_id = ".TB_PREF."tax_groups.id
		AND ".TB_PREF."sales_orders.debtor_no = ".TB_PREF."debtors_master.debtor_no
		AND ".TB_PREF."locations.loc_code = ".TB_PREF."sales_orders.from_stk_loc
		AND ".TB_PREF."shippers.shipper_id = ".TB_PREF."sales_orders.ship_via
		AND ".TB_PREF."sales_orders.trans_type = " . db_escape($trans_type) ."
		AND ".TB_PREF."sales_orders.order_no = " . db_escape($order_no );

	$result = db_query($sql, "order Retreival");

	$num = db_num_rows($result);
	if ($num > 1)
	{
		display_db_error("FATAL : sales order query returned a duplicate - " . db_num_rows($result), $sql, true);
	}
	else if ($num == 1)
	{
		return db_fetch($result);
	}
	else
		display_db_error("FATAL : sales order return nothing - " . db_num_rows($result), $sql, true);

}

//----------------------------------------------------------------------------------------

function get_sales_order_details($order_no, $trans_type) {
	$sql = "SELECT id, stk_code, unit_price, "
		.TB_PREF."sales_order_details.description,"
		.TB_PREF."sales_order_details.quantity,
		  discount_percent,
		  qty_sent as qty_done, "
		.TB_PREF."stock_master.units,
		".TB_PREF."stock_master.material_cost + "
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
	  $myrow["curr_code"], $myrow["discount"]);

	$order->set_branch($myrow["branch_code"], $myrow["tax_group_id"],
	  $myrow["tax_group_name"], $myrow["contact_phone"], $myrow["contact_email"]);

	$order->set_sales_type($myrow["sales_type_id"], $myrow["sales_type"], 
	    $myrow["tax_included"], 0); // no default price calculations on edit

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
	." AND trans_type=".ST_SALESORDER."";

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
		." AND trans_type=".ST_SALESORDER."";

	db_query($sql, "The sales order detail record could not be updated");
}

//---------------------------------------------------------------------------------------------------------------

function get_invoice_duedate($debtorno, $invdate)
{
	if (!is_date($invdate))
	{
		return new_doc_date();
	}
	$sql = "SELECT ".TB_PREF."debtors_master.debtor_no, ".TB_PREF."debtors_master.payment_terms, ".TB_PREF."payment_terms.* FROM ".TB_PREF."debtors_master,
		".TB_PREF."payment_terms WHERE ".TB_PREF."debtors_master.payment_terms = ".TB_PREF."payment_terms.terms_indicator AND
		".TB_PREF."debtors_master.debtor_no = ".db_escape($debtorno);

	$result = db_query($sql,"The customer details could not be retrieved");
	$myrow = db_fetch($result);

	if (db_num_rows($result) == 0)
		return $invdate;
	if ($myrow['day_in_following_month'] > 0)
		$duedate = add_days(end_month($invdate), $myrow['day_in_following_month']);
	else
		$duedate = add_days($invdate, $myrow['days_before_due']);
	return $duedate;
}

function get_customer_to_order($customer_id) {

	// Now check to ensure this account is not on hold */
	$sql = "SELECT ".TB_PREF."debtors_master.name, "
		  .TB_PREF."debtors_master.address, "
		  .TB_PREF."credit_status.dissallow_invoices, "
		  .TB_PREF."debtors_master.sales_type AS salestype, "
		  .TB_PREF."debtors_master.dimension_id, "
		  .TB_PREF."debtors_master.dimension2_id, "
		  .TB_PREF."sales_types.sales_type, "
		  .TB_PREF."sales_types.tax_included, "
		  .TB_PREF."sales_types.factor, "
		  .TB_PREF."debtors_master.curr_code, "
		  .TB_PREF."debtors_master.discount,"
		  .TB_PREF."debtors_master.pymt_discount
		FROM ".TB_PREF."debtors_master, "
		  .TB_PREF."credit_status, "
		  .TB_PREF."sales_types
		WHERE ".TB_PREF."debtors_master.sales_type="
		  .TB_PREF."sales_types.id
		AND ".TB_PREF."debtors_master.credit_status=".TB_PREF."credit_status.id
		AND ".TB_PREF."debtors_master.debtor_no = ".db_escape($customer_id);

	$result =db_query($sql,"Customer Record Retreive");
	return 	db_fetch($result);
}

function get_branch_to_order($customer_id, $branch_id) {

    	// the branch was also selected from the customer selection so default the delivery details from the customer branches table cust_branch. The order process will ask for branch details later anyway
	 	$sql = "SELECT ".TB_PREF."cust_branch.br_name, "
			.TB_PREF."cust_branch.br_address, "
			.TB_PREF."cust_branch.br_post_address, "
			.TB_PREF."cust_branch.phone, "
			.TB_PREF."cust_branch.email,
			  default_location, location_name, default_ship_via, "
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
?>