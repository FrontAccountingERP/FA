<?php

include_once($path_to_root . "/sales/includes/cart_class.inc");
include_once($path_to_root . "/includes/manufacturing.inc");

//--------------------------------------------------------------------------------

function add_to_order(&$order, $new_item, $new_item_qty, $price, $discount)
{
    $already_on_order = 0;

	foreach ($order->line_items AS $order_item)
	{
        if (strcasecmp($order_item->stock_id, $new_item) == 0)
        {
            $already_on_order = 1;
            display_error(_("For Part :") . $new_item . " " . "This item is already on this order.  You can change the quantity ordered of the existing line if necessary.");
        }
	}


    if ($already_on_order != 1)
    {
         $order->add_to_cart ($new_item, $new_item_qty, $price, $discount);
    } /* end of if not already on the order */
}

//---------------------------------------------------------------------------------

function get_customer_details_to_order(&$order, $customer_id, $branch_id)
{
	$ret_error = "";
	// Now check to ensure this account is not on hold */
	$sql = "SELECT ".TB_PREF."debtors_master.name, ".TB_PREF."debtors_master.address, ".TB_PREF."credit_status.dissallow_invoices,
		".TB_PREF."debtors_master.sales_type AS salestype, ".TB_PREF."sales_types.sales_type, ".TB_PREF."debtors_master.curr_code,
		".TB_PREF."debtors_master.discount
		FROM ".TB_PREF."debtors_master, ".TB_PREF."credit_status, ".TB_PREF."sales_types
		WHERE ".TB_PREF."debtors_master.sales_type=".TB_PREF."sales_types.id
			AND ".TB_PREF."debtors_master.credit_status=".TB_PREF."credit_status.id
			AND ".TB_PREF."debtors_master.debtor_no = '" . $customer_id . "'";

	$result =db_query($sql,"Customer Record Retreive");

	$myrow = db_fetch($result);

	$order->customer_id = $customer_id;
	$order->Branch = $branch_id;
	$order->customer_name = $myrow['name'];

	if ($myrow['dissallow_invoices'] == 1)
		$ret_error = _("The selected customer account is currently on hold. Please contact the credit control personnel to discuss.");

	if (!isset($_POST['branch_id']) || $_POST['branch_id'] == "")
	{
		$ret_error = _("The selected customer does not have any branches. Please create at least one branch.");
		unset($_POST['branch_id']);
		$order->Branch = "";
	}

	# the sales type determines the price list to be used by default
	$order->default_sales_type = $myrow['salestype'];
	$order->sales_type_name = $myrow['sales_type'];
	$order->customer_currency = $myrow['curr_code'];
	$order->default_discount = $myrow['discount'];
	$deliver = $myrow['address']; // in case no deliveraddress.

	if ($order->Branch != "")
	{
    	# the branch was also selected from the customer selection so default the delivery details from the customer branches table cust_branch. The order process will ask for branch details later anyway
	 	$sql = "SELECT ".TB_PREF."cust_branch.br_name, ".TB_PREF."cust_branch.br_post_address, phone, email,
			default_location, default_ship_via,
			".TB_PREF."tax_groups.name AS tax_group_name, ".TB_PREF."tax_groups.id AS tax_group_id
			FROM ".TB_PREF."cust_branch, ".TB_PREF."tax_groups
				WHERE ".TB_PREF."cust_branch.tax_group_id = ".TB_PREF."tax_groups.id
					AND ".TB_PREF."cust_branch.branch_code='" . $order->Branch . "'
					AND ".TB_PREF."cust_branch.debtor_no = '" . $order->customer_id . "'";

    	$result = db_query($sql,"Customer Branch Record Retreive");

    	if (db_num_rows($result) == 0)
    	{
			return _("The selected customer and branch are not valid, or the customer does not have any branches.");
    	}

    	$myrow = db_fetch($result);
    	$order->deliver_to = $myrow["br_name"];
    	$order->delivery_address = $myrow["br_post_address"];
    	if (strlen($order->delivery_address) <= 1)
    		$order->delivery_address = $deliver;
    	$order->phone = $myrow["phone"];
    	$order->email = $myrow["email"];
    	$order->Location = $myrow["default_location"];
    	$order->ship_via = $myrow["default_ship_via"];
		$order->tax_group_name = $myrow["tax_group_name"];
		$order->tax_group_id = $myrow["tax_group_id"];
	}

	return $ret_error;
}

//---------------------------------------------------------------------------------

function display_order_summary($title, &$order, $editable_items=false)
{
	global $table_style, $path_to_root;

	display_heading($title);
	start_table("$table_style colspan=7 width=90%");
	$th = array(_("Item Code"), _("Item Description"), _("Quantity"),
		_("Unit"), _("Price"), _("Discount %"), _("Total"), "", "");
	table_header($th);

	$total = 0;
	$k = 0;  //row colour counter

	foreach ($order->line_items as $stock_item)
	{

		$line_total = $stock_item->quantity * $stock_item->price * (1 - $stock_item->discount_percent);

		if (!isset($_GET['Edit']))
			$id = "";
		else
			$id = $_GET['Edit'];
		if (!$editable_items || $id != $stock_item->stock_id)
		{
    		alt_table_row_color($k);

			view_stock_status_cell($stock_item->stock_id);

    		label_cell($stock_item->item_description);
    		qty_cell($stock_item->quantity);
    		label_cell($stock_item->units);
    		amount_cell($stock_item->price);

    		amount_cell($stock_item->discount_percent * 100);
    		amount_cell($line_total);

    		if ($editable_items)
    		{
        		edit_link_cell(SID . "Edit=$stock_item->stock_id");
        		delete_link_cell(SID . "Delete=$stock_item->stock_id");
    		}
        	end_row();
		}
		else
		{
			sales_order_item_controls($order, $stock_item->stock_id);
		}

		$total += $line_total;
	}

	if (!isset($_GET['Edit']) && $editable_items)
		sales_order_item_controls($order);

	$display_total = number_format2($total,user_price_dec());
	label_row(_("Total Excluding Tax/Shipping"), $display_total, "colspan=6 align=right",
		"nowrap align=right");

	end_table();
}

// ------------------------------------------------------------------------------

function display_order_header(&$order, $editable, $date_text, $display_tax_group=false)
{
	global $table_style;
	start_table("width=80% $table_style");
	echo "<tr><td valign=top>"; // outer table
	echo "<table>";

	$customer_error = "";

	if (isset($order) && !$editable)
	{
		// can't change the customer/branch if items already received on this order
		echo $order->customer_name . " - " . $order->deliver_to;
	}
	else
	{

        if (!isset($_POST['customer_id']) && (get_global_customer() != reserved_words::get_all()))
        	$_POST['customer_id'] = get_global_customer();

		customer_list_row(_("Customer:"), 'customer_id', null, false, true);

		if (!isset($_POST['delivery_date']))
		{
			if ($order->direct_invoice)
				$_POST['delivery_date'] = $_SESSION['Items']->delivery_date =
					get_invoice_duedate($_POST['customer_id'], $_POST['OrderDate']);
			else
				$_POST['delivery_date'] = $_SESSION['Items']->delivery_date =
					add_days($_POST['OrderDate'], 10);
		}
		if ($order->customer_id != $_POST['customer_id'])
		{
			// customer has changed

			// delete all the order items - drastic but necessary because of
			// change of currency, sales type, etc
    		$order->clear_items();

			// clear the branch selection
			unset($_POST['branch_id']);
		}

		customer_branches_list_row(_("Branch:"), $_POST['customer_id'], 'branch_id', null, false, true, true);

		if (!isset($_POST['branch_id']))
			$_POST['branch_id'] = "";
		//set_global_customer($_POST['customer_id']);
		if (($order->customer_id != $_POST['customer_id']) ||
			($order->Branch != $_POST['branch_id']))
			$customer_error = get_customer_details_to_order($order, $_POST['customer_id'], $_POST['branch_id']);

		set_global_customer($_POST['customer_id']);
	}
	echo "</table>";

	echo "</td><td>"; // outer table

	if (!is_company_currency($order->customer_currency))
	{
		echo "<table height='5'>";
		label_row(_("Customer Currency:"), $order->customer_currency);
		exchange_rate_display($order->customer_currency, get_company_currency(),
			($editable?$_POST['OrderDate']:$order->orig_order_date), $editable);
		echo "</table>";
		echo "</td><td>"; // outer table
	}

	echo "<table height='5'>";
	label_row(_("Sales Type:"), $order->sales_type_name);
	label_row(_("Customer Discount:"), ($order->default_discount * 100) . "%");
	echo "</table>";

	echo "</td><td>"; // outer table

	echo "<table height='5'>";

	if ($editable)
	{
    	if (!isset($_POST['OrderDate']) || $_POST['OrderDate'] == "")
    		$_POST['OrderDate'] = $order->orig_order_date;

		date_row($date_text, 'OrderDate');
	}
	else
	{
		label_row($date_text, $order->orig_order_date);
		hidden('OrderDate', $order->orig_order_date);
	}

	if ($display_tax_group)
	{
		if ($editable)
		{
            if (!isset($_POST['tax_group_id']))
            	$_POST['tax_group_id'] = $_SESSION['Items']->tax_group_id;
            tax_groups_list_row(_("Tax Group:"), 'tax_group_id', $_POST['tax_group_id'], true);
		}
		else
		{
			label_row(_("Tax Group:"), $order->tax_group_name);
			hidden('tax_group_id', $_SESSION['Items']->tax_group_id);
		}
	}
	echo "</table>";

	echo "</td></tr>";

	end_table(1); // outer table

	return $customer_error;
}

//--------------------------------------------------------------------------------

function sales_order_item_controls(&$order, $stock_id=null)
{
	start_row();

	if (isset($_GET['Edit']) && $stock_id != null)
	{
		if (!isset($_POST['stock_id']))
			$_POST['stock_id'] = $order->line_items[$_GET['Edit']]->stock_id;
		if (!isset($_POST['qty']) || ($_POST['qty'] == ""))
			$_POST['qty'] = $order->line_items[$_GET['Edit']]->quantity;
		if (!isset($_POST['price']) || ($_POST['price'] == ""))
			$_POST['price'] = $order->line_items[$_GET['Edit']]->price;
		if (!isset($_POST['Disc']) || ($_POST['Disc'] == ""))
			$_POST['Disc'] = ($order->line_items[$_GET['Edit']]->discount_percent)*100;

		$_POST['units'] = $order->line_items[$_GET['Edit']]->units;

		hidden('stock_id', $_POST['stock_id']);
		label_cell($_POST['stock_id']);
		label_cell($order->line_items[$stock_id]->item_description);
	}
	else
	{
		global $no_item_list;
		if ($no_item_list)
		{
			echo "<td colspan=2>\n";
			stock_items_list('stock_id', null, false, true);
			echo "</td>\n";
		}
		else
		{
			text_cells(null, "StockID2", "", 12, 10, "", "", "onkeyup='recalcAccounts();' onKeyDown='if(event.keyCode==13) event.keyCode=9;' onblur='return setAccount(0, true);'");
			stock_items_list_cells(null, 'stock_id', null, false, false, "onchange='return setAccount(1, true)'");
		}
		$item_info = get_item_edit_info($_POST['stock_id']);
		$_POST['units'] = $item_info["units"];

   		$_POST['qty'] = 1;
		$_POST['price'] = get_price ($_POST['stock_id'], $order->customer_id);
		// default to the customer's discount %
		$_POST['Disc'] = $order->default_discount * 100;
	}

	text_cells(null, 'qty', $_POST['qty'], 12, 15);

	label_cell($_POST['units']);
	amount_cells(null, 'price');
	text_cells(null, 'Disc', $_POST['Disc'], 7, 5);

	$line_total = $_POST['qty'] * $_POST['price'] * (1 - $_POST['Disc'] / 100);
	amount_cell($line_total);

	if (isset($_GET['Edit']))
	{
    	submit_cells('UpdateItem', _("Update"));
    	submit_cells('CancelItemChanges', _("Cancel"));
	}
	else
	{
		submit_cells('AddItem', _("Add Item"), "colspan=2");
	}

	end_row();
}

//--------------------------------------------------------------------------------

function display_delivery_details(&$order)
{
	global $table_style2;

	if ($order->direct_invoice)
	{
		$title = _("Invoice Delivery Details");
		$delname = _("Due Date");
	}
	else
	{
		$title = _("Order Delivery Details");
		$delname = _("Required Delivery Date:");
	}
	display_heading($title);
    echo "<br>";
    start_table("$table_style2 width=90%");
    echo "<tr valign=top><td>"; // outer table

    echo "<table>";
    locations_list_row(_("Deliver from Location:"), 'Location', $order->Location);

   	date_row($delname, 'delivery_date', $order->delivery_date, 0, 0, 0);

    text_row(_("Deliver To:"), 'deliver_to', $order->deliver_to, 40, 40);

    textarea_row(_("Address:"), 'delivery_address', $order->delivery_address, 35, 5);
    text_row(_("Contact Phone Number:"), 'phone', $order->phone, 25, 25);

    echo "</table>";

    echo "</td><td>"; // outer table

    echo "<table>";

    text_row(_("Customer Reference:"), 'cust_ref', $order->cust_ref, 25, 25);
    textarea_row(_("Comments:"), "Comments", $order->Comments, 31, 5);

    text_row(_("Shipping Charge:"), 'freight_cost', $order->freight_cost, 10, 10);

    shippers_list_row(_("Shipping Company:"), 'ship_via', $order->ship_via);
	if ($_SESSION['Items']->direct_invoice)
		textarea_row(_("Memo"), 'InvoiceText', null, 31, 3);
    echo "</table>";

    echo "</td></tr>";
    end_table(1); // outer table
}

?>