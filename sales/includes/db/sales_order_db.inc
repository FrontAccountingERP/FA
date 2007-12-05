<?php

//----------------------------------------------------------------------------------------
function get_demand_qty($stockid, $location)
{
	$sql = "SELECT SUM(".TB_PREF."sales_order_details.quantity - ".TB_PREF."sales_order_details.qty_invoiced) AS QtyDemand
				FROM ".TB_PREF."sales_order_details,
					".TB_PREF."sales_orders
				WHERE ".TB_PREF."sales_order_details.order_no=".TB_PREF."sales_orders.order_no AND
					".TB_PREF."sales_orders.from_stk_loc ='$location' AND
					".TB_PREF."sales_order_details.stk_code = '$stockid'";

    $TransResult = db_query($sql,"No transactions were returned");
	$DemandRow = db_fetch($TransResult);
	return $DemandRow['QtyDemand'];
}

function get_demand_asm_qty($stockid, $location)
{
	$sql = "SELECT SUM((".TB_PREF."sales_order_details.quantity-".TB_PREF."sales_order_details.qty_invoiced)*".TB_PREF."bom.quantity)
				   AS Dem
				   FROM ".TB_PREF."sales_order_details,
						".TB_PREF."sales_orders,
						".TB_PREF."bom,
						".TB_PREF."stock_master
				   WHERE ".TB_PREF."sales_order_details.stk_code=".TB_PREF."bom.parent AND
				   ".TB_PREF."sales_orders.order_no = ".TB_PREF."sales_order_details.order_no AND
				   ".TB_PREF."sales_orders.from_stk_loc='$location' AND
				   ".TB_PREF."sales_order_details.quantity-".TB_PREF."sales_order_details.qty_invoiced > 0 AND
				   ".TB_PREF."bom.component='$stockid' AND
				   ".TB_PREF."stock_master.stock_id=".TB_PREF."bom.parent AND
				   ".TB_PREF."stock_master.mb_flag='A'";

    $TransResult = db_query($sql,"No transactions were returned");
	if (db_num_rows($TransResult)==1)
	{
		$DemandRow = db_fetch_row($TransResult);
		$DemandQty = $DemandRow[0];
	}
	else
		$DemandQty = 0.0;

    return $DemandQty;
}

function add_sales_order($order)
{
	global $loc_notification, $path_to_root;

	begin_transaction();

	if ($order->direct_invoice)
		$del_date = date2sql($order->orig_order_date);
	else
		$del_date = date2sql($order->delivery_date);

	$sql = "INSERT INTO ".TB_PREF."sales_orders (debtor_no, branch_code, customer_ref, Comments, ord_date,
		order_type, ship_via, deliver_to, delivery_address, contact_phone,
		contact_email, freight_cost, from_stk_loc, delivery_date)
		VALUES ('" . $order->customer_id . "', '" . $order->Branch . "', '".
			$order->cust_ref ."','". db_escape($order->Comments) ."','" .
			date2sql($order->orig_order_date) . "', '" .
			$order->default_sales_type . "', " .
			$_POST['ship_via'] .",'" . $order->deliver_to . "', '" .
			$order->delivery_address . "', '" .
			$order->phone . "', '" . $order->email . "', " .
			$order->freight_cost .", '" . $order->Location ."', '" .
			$del_date . "')";

	db_query($sql, "order Cannot be Added");

	$order_no = db_insert_id();
	if ($loc_notification == 1)
	{
		include_once($path_to_root . "/inventory/includes/inventory_db.inc");
		$st_ids = array();
		$st_names = array();
		$st_num = array();
		$st_reorder = array();
	}
	foreach ($order->line_items as $stock_item)
	{
		if ($loc_notification == 1 && is_inventory_item($stock_item->stock_id))
		{
			$sql = "SELECT ".TB_PREF."loc_stock.*, ".TB_PREF."locations.location_name, ".TB_PREF."locations.email
				FROM ".TB_PREF."loc_stock, ".TB_PREF."locations
				WHERE ".TB_PREF."loc_stock.loc_code=".TB_PREF."locations.loc_code
				AND ".TB_PREF."loc_stock.stock_id = '" . $stock_item->stock_id . "'
				AND ".TB_PREF."loc_stock.loc_code = '" . $order->Location . "'";
	 		$res = db_query($sql,"a location could not be retreived");
	 		$loc = db_fetch($res);
			if ($loc['email'] != "")
			{
				$qoh = get_qoh_on_date($stock_item->stock_id, $order->Location);
				$qoh -= get_demand_qty($stock_item->stock_id, $order->Location);
				$qoh -= get_demand_asm_qty($stock_item->stock_id, $order->Location);
				$qoh -= $stock_item->quantity;
				if ($qoh < $loc['reorder_level'])
				{
					$st_ids[] = $stock_item->stock_id;
					$st_names[] = $stock_item->item_description;
					$st_num[] = $qoh - $loc['reorder_level'];
					$st_reorder[] = $loc['reorder_level'];
				}
			}
		}
		$sql = "INSERT INTO ".TB_PREF."sales_order_details (order_no, stk_code, description, unit_price, quantity, discount_percent) VALUES (";
		$sql .= $order_no .
				",'$stock_item->stock_id', '$stock_item->item_description', $stock_item->price,
				$stock_item->quantity,
				$stock_item->discount_percent)";
		db_query($sql, "order Details Cannot be Added");

	} /* inserted line items into sales order details */

	add_forms_for_sys_type(systypes::sales_order(), $order_no);

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

function delete_sales_order($order_no)
{
	begin_transaction();

	$sql = "DELETE FROM ".TB_PREF."sales_orders WHERE order_no=" . $order_no;
	db_query($sql, "order Header Delete");

	$sql = "DELETE FROM ".TB_PREF."sales_order_details WHERE order_no =" . $order_no;
	db_query($sql, "order Detail Delete");

	delete_forms_for_systype(systypes::sales_order(), $order_no);

	commit_transaction();
}

//----------------------------------------------------------------------------------------

function update_sales_order($order_no, $order)
{
	global $loc_notification, $path_to_root;

	$del_date = date2sql($order->delivery_date);
	$ord_date = date2sql($order->orig_order_date);

	begin_transaction();

	$sql = "UPDATE ".TB_PREF."sales_orders SET debtor_no = '" . $order->customer_id . "',
		branch_code = '" . $order->Branch . "',
		customer_ref = '". $order->cust_ref ."',
		Comments = '". db_escape($order->Comments) ."', ord_date = '" . $ord_date . "',
		order_type = '" . $order->default_sales_type . "', ship_via = " . $order->ship_via .",
		deliver_to = '" . $order->deliver_to . "', delivery_address = '" . $order->delivery_address . "',
		contact_phone = '" . $order->phone . "',
		contact_email = '" . $order->email . "', freight_cost = " . $order->freight_cost .",
		from_stk_loc = '" . $order->Location ."', delivery_date = '" . $del_date . "'
		WHERE order_no=" . $order_no;

	db_query($sql, "order Cannot be Updated");

	$sql = "DELETE FROM ".TB_PREF."sales_order_details WHERE order_no =" . $order_no;

	db_query($sql, "Old order Cannot be Deleted");

	if ($loc_notification == 1)
	{
		include_once($path_to_root . "/inventory/includes/inventory_db.inc");
		$st_ids = array();
		$st_names = array();
		$st_num = array();
		$st_reorder = array();
	}
	foreach ($order->line_items as $stock_item)
	{
		if ($loc_notification == 1 && is_inventory_item($stock_item->stock_id))
		{
			$sql = "SELECT ".TB_PREF."loc_stock.*, ".TB_PREF."locations.location_name, ".TB_PREF."locations.email
				FROM ".TB_PREF."loc_stock, ".TB_PREF."locations
				WHERE ".TB_PREF."loc_stock.loc_code=".TB_PREF."locations.loc_code
				AND ".TB_PREF."loc_stock.stock_id = '" . $stock_item->stock_id . "'
				AND ".TB_PREF."loc_stock.loc_code = '" . $order->Location . "'";
	 		$res = db_query($sql,"a location could not be retreived");
	 		$loc = db_fetch($res);
			if ($loc['email'] != "")
			{
				$qoh = get_qoh_on_date($stock_item->stock_id, $order->Location);
				$qoh -= get_demand_qty($stock_item->stock_id, $order->Location);
				$qoh -= get_demand_asm_qty($stock_item->stock_id, $order->Location);
				$qoh -= $stock_item->quantity;
				if ($qoh < $loc['reorder_level'])
				{
					$st_ids[] = $stock_item->stock_id;
					$st_names[] = $stock_item->item_description;
					$st_num[] = $qoh - $loc['reorder_level'];
					$st_reorder[] = $loc['reorder_level'];
				}
			}
		}

		$sql = "INSERT INTO ".TB_PREF."sales_order_details (order_no, stk_code,  description, unit_price, quantity, discount_percent, qty_invoiced) VALUES (";

		$sql .= $order_no . ",'" . $stock_item->stock_id . "','" . $stock_item->item_description . "', " . $stock_item->price . ", " . $stock_item->quantity . ", " . $stock_item->discount_percent . ", " . $stock_item->qty_inv . " )";

		db_query($sql, "Old order Cannot be Inserted");

	} /* inserted line items into sales order details */

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
}

//----------------------------------------------------------------------------------------

function get_sales_order($order_no)
{
	$sql = "SELECT ".TB_PREF."sales_orders.*, ".TB_PREF."debtors_master.name, ".TB_PREF."debtors_master.curr_code, ".TB_PREF."locations.location_name,
		".TB_PREF."debtors_master.payment_terms, ".TB_PREF."debtors_master.discount, ".TB_PREF."sales_types.sales_type, ".TB_PREF."shippers.shipper_name,
		".TB_PREF."tax_groups.name AS tax_group_name , ".TB_PREF."tax_groups.id AS tax_group_id
		FROM ".TB_PREF."sales_orders, ".TB_PREF."debtors_master, ".TB_PREF."sales_types, ".TB_PREF."tax_groups, ".TB_PREF."cust_branch, ".TB_PREF."locations, ".TB_PREF."shippers
			WHERE ".TB_PREF."sales_orders.order_type=".TB_PREF."sales_types.id
				AND ".TB_PREF."cust_branch.branch_code = ".TB_PREF."sales_orders.branch_code
				AND ".TB_PREF."cust_branch.tax_group_id = ".TB_PREF."tax_groups.id
				AND ".TB_PREF."sales_orders.debtor_no = ".TB_PREF."debtors_master.debtor_no
				AND ".TB_PREF."locations.loc_code = ".TB_PREF."sales_orders.from_stk_loc
				AND ".TB_PREF."shippers.shipper_id = ".TB_PREF."sales_orders.ship_via
				AND ".TB_PREF."sales_orders.order_no = " . $order_no;

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

function read_sales_order($order_no, &$order, $skip_completed_items=false)
{
	$myrow = get_sales_order($order_no);

	$order->customer_id = $myrow["debtor_no"];
	$order->Branch = $myrow["branch_code"];
	$order->customer_name = $myrow["name"];
	$order->cust_ref = $myrow["customer_ref"];
	$order->default_sales_type =$myrow["order_type"];
	$order->sales_type_name =$myrow["sales_type"];
	$order->customer_currency = $myrow["curr_code"];
	$order->default_discount = $myrow["discount"];

	$order->Comments = $myrow["comments"];

	$order->ship_via = $myrow["ship_via"];
	$order->deliver_to = $myrow["deliver_to"];
	$order->delivery_date = sql2date($myrow["delivery_date"]);
	$order->freight_cost = $myrow["freight_cost"];
	$order->delivery_address = $myrow["delivery_address"];
	$order->phone = $myrow["contact_phone"];
	$order->email = $myrow["contact_email"];
	$order->Location = $myrow["from_stk_loc"];
	$order->location_name = $myrow["location_name"];
	$order->orig_order_date = sql2date($myrow["ord_date"]);

	$order->tax_group_name = $myrow["tax_group_name"];
	$order->tax_group_id = $myrow["tax_group_id"];

	$sql = "SELECT stk_code, unit_price, ".TB_PREF."sales_order_details.description,
		".TB_PREF."sales_order_details.quantity, discount_percent,
		qty_invoiced, ".TB_PREF."stock_master.units,
		".TB_PREF."stock_master.material_cost + ".TB_PREF."stock_master.labour_cost + ".TB_PREF."stock_master.overhead_cost AS standard_cost
		FROM ".TB_PREF."sales_order_details, ".TB_PREF."stock_master
			WHERE ".TB_PREF."sales_order_details.stk_code = ".TB_PREF."stock_master.stock_id
				AND order_no =" . $order_no;

	if ($skip_completed_items)
		$sql .= "
			AND ".TB_PREF."sales_order_details.quantity - ".TB_PREF."sales_order_details.qty_invoiced > 0 ";

	$result = db_query($sql, "Retreive order Line Items");

	if (db_num_rows($result) > 0)
	{

		while ($myrow = db_fetch($result))
		{
			$order->add_to_cart($myrow["stk_code"],$myrow["quantity"],
				$myrow["unit_price"], $myrow["discount_percent"],
				$myrow["qty_invoiced"], $myrow["standard_cost"], $myrow["description"]);
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------

function sales_order_has_invoices($order_no)
{
	$sql = "SELECT COUNT(*) FROM ".TB_PREF."debtor_trans WHERE order_=$order_no";

	$result = db_query($sql, "could not query for sales order usage");

	$row = db_fetch_row($result);

	return ($row[0] > 0);
}

//----------------------------------------------------------------------------------------

function close_sales_order($order_no)
{
	// set the quantity of each item to the already invoiced quantity. this will mark item as closed.
	$sql = "UPDATE ".TB_PREF."sales_order_details
		SET quantity = qty_invoiced
 			WHERE order_no = $order_no";

	db_query($sql, "The sales order detail record could not be updated");
}

//----------------------------------------------------------------------------------------

function dispatch_sales_order_item($order_no, $stock_id, $qty_dispatched)
{
	$sql = "UPDATE ".TB_PREF."sales_order_details
		SET qty_invoiced = qty_invoiced + $qty_dispatched ";
	$sql .= " WHERE order_no = $order_no
		AND stk_code = '$stock_id'";

	db_query($sql, "The sales order detail record could not be updated");
}

//---------------------------------------------------------------------------------------------------------------

function get_invoice_duedate($debtorno, $invdate)
{
	if (!is_date($invdate))
	{
		return Today();
	}
    $sql = "SELECT ".TB_PREF."debtors_master.debtor_no, ".TB_PREF."debtors_master.payment_terms, ".TB_PREF."payment_terms.* FROM ".TB_PREF."debtors_master,
	   	".TB_PREF."payment_terms WHERE ".TB_PREF."debtors_master.payment_terms = ".TB_PREF."payment_terms.terms_indicator AND
	   	".TB_PREF."debtors_master.debtor_no = '$debtorno'";

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


?>