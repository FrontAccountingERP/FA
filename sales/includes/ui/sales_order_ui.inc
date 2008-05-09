<?php

include_once($path_to_root . "/sales/includes/cart_class.inc");
include_once($path_to_root . "/includes/manufacturing.inc");

//--------------------------------------------------------------------------------

function add_to_order(&$order, $new_item, $new_item_qty, $price, $discount)
{

	foreach ($order->line_items AS $order_item)
	{
		if (strcasecmp($order_item->stock_id, $new_item) == 0)
		{
			display_notification(_("For Part :") . $new_item . " " . _("This item is already on this order. You have been warned."));
			break;
		}
	}

	$order->add_to_cart (count($order->line_items),$new_item, $new_item_qty, $price, $discount);
}

//---------------------------------------------------------------------------------

function get_customer_details_to_order(&$order, $customer_id, $branch_id)
{
	$ret_error = "";

	$myrow = get_customer_to_order($customer_id);

	$name = $myrow['name'];

	if ($myrow['dissallow_invoices'] == 1)
		$ret_error = _("The selected customer account is currently on hold. Please contact the credit control personnel to discuss.");

/*	if (!isset($_POST['branch_id']) || $_POST['branch_id'] == "")
	{
		$ret_error = _("The selected customer does not have any branches. Please create at least one branch.");
		unset($_POST['branch_id']);
		$order->Branch = 0;
	}
*/
	$deliver = $myrow['address']; // in case no delivery address.

	$order->set_customer($customer_id, $name, $myrow['curr_code'], $myrow['discount']);

	// the sales type determines the price list to be used by default
	$order->set_sales_type($myrow['salestype'], $myrow['sales_type'], $myrow['tax_included'],
	    $myrow['factor']);

//	if ($order->Branch != 0)
	{
		$result = get_branch_to_order($customer_id, $branch_id);

		if (db_num_rows($result) == 0)
		{
		return _("The selected customer and branch are not valid, or the customer does not have any branches.");
		}

		$myrow = db_fetch($result);

		$order->set_branch($branch_id, $myrow["tax_group_id"],
		$myrow["tax_group_name"], $myrow["phone"], $myrow["email"]);

		$address = $myrow["br_post_address"];

		if (strlen($address) <= 1)	// if branch has no address
			$address = $deliver;	// set sales order address

		$order->set_location($myrow["default_location"], $myrow["location_name"]);
		$order->set_delivery($myrow["default_ship_via"], $myrow["br_name"],
		$address);
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
		_("Delivered"),
		_("Unit"), _("Price"), _("Discount %"), _("Total"), "");

	if ($order->trans_no==0) {
	unset( $th[3] );
	}

	if (count($order->line_items)) $th[]= '';

	table_header($th);

	$total = 0;
	$k = 0;  //row colour counter

	$id = find_submit('Edit');

	foreach ($order->line_items as $line_no=>$stock_item)
	{

		$line_total = round($stock_item->qty_dispatched * $stock_item->price * (1 - $stock_item->discount_percent),
		   user_price_dec());


		if (!$editable_items || $id != $line_no)
		{
			alt_table_row_color($k);

			view_stock_status_cell($stock_item->stock_id);

			label_cell($stock_item->item_description, "nowrap" );
			qty_cell($stock_item->qty_dispatched);

			if ($order->trans_no!=0)
			amount_cell($stock_item->qty_done);

			label_cell($stock_item->units);
			amount_cell($stock_item->price);

			percent_cell($stock_item->discount_percent * 100);
			amount_cell($line_total);

			if ($editable_items)
			{
				edit_button_cell("Edit$line_no", _("Edit"));
				edit_button_cell("Delete$line_no", _("Delete"));
			}
			end_row();
		}
		else
		{
			sales_order_item_controls($order, $k,  $line_no);
		}

		$total += $line_total;
	}

	if ($id==-1 && $editable_items)
		sales_order_item_controls($order, $k);

	$display_total = price_format($total);
	label_row(_("Total Excluding Shipping"), $display_total, "colspan=6 align=right",
		"nowrap align=right", 2);

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
	$change_prices = 0;

	if (isset($order) && !$editable)
	{
		// can't change the customer/branch if items already received on this order
		echo $order->customer_name . " - " . $order->deliver_to;
		hidden('customer_id', $order->customer_id);
		hidden('branch_id', $order->Branch);
		hidden('sales_type', $order->sales_type);
	}
	else
	{

		if (!isset($_POST['customer_id']) && (get_global_customer() != reserved_words::get_all()))
				$_POST['customer_id'] = get_global_customer();

		customer_list_row(_("Customer:"), 'customer_id', null, false, true);

		if (!isset($_POST['delivery_date']))
		{
			if ($order->trans_type==10)
				$_POST['delivery_date'] = $_SESSION['Items']->due_date =
					get_invoice_duedate($_POST['customer_id'], $_POST['OrderDate']);
			else
				$_POST['delivery_date'] = $_SESSION['Items']->due_date =
					add_days($_POST['OrderDate'], 10); // FIX this should be in company prefs
		}
		if ($order->customer_id != $_POST['customer_id'])
		{
			// customer has changed
			// clear the branch selection
			unset($_POST['branch_id']);
		}
		if (!isset($_POST['branch_id']))
			$_POST['branch_id'] = "";

		customer_branches_list_row(_("Branch:"), $_POST['customer_id'], 'branch_id', null, false, true, true);

		if (!isset($_POST['branch_id']) || $_POST['branch_id'] == "")
		{
			$customer_error = _("The selected customer does not have any branches. Please create at least one branch.");
			unset($_POST['branch_id']);
			$order->Branch = 0;
		} else
		{
			//set_global_customer($_POST['customer_id']);
			if (($order->customer_id != $_POST['customer_id']) ||
				($order->Branch != $_POST['branch_id'])) {

				$old_order = (PHP_VERSION<5) ? $order : clone( $order );
				$customer_error = get_customer_details_to_order($order, $_POST['customer_id'], $_POST['branch_id']);
				$_POST['Location'] = $order->Location;
				$_POST['deliver_to'] = $order->deliver_to;
				$_POST['delivery_address'] = $order->delivery_address;
				$_POST['phone'] = $order->phone;
				$_POST['sales_type'] = $order->sales_type;
				$_POST['factor'] = $order->price_factor;

				// change prices if necessary
				// what about discount in template case?
				if ( $old_order->customer_currency!=$order->customer_currency
					|| $old_order->sales_type!=$order->sales_type
				//  || $old_order->default_discount!=$order->default_discount
					) {
					$change_prices = 1;
				}

			}
		} // changed branch
		set_global_customer($_POST['customer_id']);
	}

	if ($_SESSION['Items']->trans_type!=30) {
		ref_cells(_("Reference").':', 'ref', null, '');
	}

	echo "</table>";

	echo "</td><td>"; // outer table

	if (!is_company_currency($order->customer_currency))
	{
		echo "<table height='5'>";
		label_row(_("Customer Currency:"), $order->customer_currency);
		exchange_rate_display($order->customer_currency, get_company_currency(),
			($editable ? $_POST['OrderDate'] : $order->document_date), $editable);
		echo "</table>";
		echo "</td><td>"; // outer table
	}

	if ($order->sales_type != $_POST['sales_type']) {
		$myrow = get_sales_type($_POST['sales_type']);
		$order->set_sales_type($myrow['id'], $myrow['sales_type'],
		$myrow['tax_included'], $myrow['factor']);
		$change_prices = 1;
	}

	if ($change_prices != 0) {
		foreach ($order->line_items as $line_no=>$item) {
			$line = &$order->line_items[$line_no];
			$line->price = get_price($line->stock_id, $order->customer_currency,
				$order->sales_type, $order->price_factor, $order->document_date);
		//		$line->discount_percent = $order->default_discount;
		}
	}

	echo "<table height='5'>";
	if($editable)
		sales_types_list_row(_("Price List"), 'sales_type', $_POST['sales_type'], true);
	else {
		label_row(_("Price List:"), $order->sales_type_name);
	}
	label_row(_("Customer Discount:"), ($order->default_discount * 100) . "%");
	echo "</table>";

	echo "</td><td>"; // outer table

	echo "<table height='5'>";

	if ($editable)
	{
		if (!isset($_POST['OrderDate']) || $_POST['OrderDate'] == "")
			$_POST['OrderDate'] = $order->document_date;

		date_row($date_text, 'OrderDate');
	}
	else
	{
		label_row($date_text, $order->document_date);
		hidden('OrderDate', $order->document_date);
	}

	if ($display_tax_group)
	{
	    label_row(_("Tax Group:"), $order->tax_group_name);
	    hidden('tax_group_id', $_SESSION['Items']->tax_group_id);
	}
	echo "</table>";

	echo "</td></tr>";

	end_table(1); // outer table

	return $customer_error;
}

//--------------------------------------------------------------------------------

function sales_order_item_controls(&$order, &$rowcounter, $line_no=-1)
{
	alt_table_row_color($rowcounter);

	$id = find_submit('Edit');
	if ($line_no!=-1 && $line_no == $id)
	{
		$_POST['stock_id'] = $order->line_items[$id]->stock_id;
		$_POST['qty'] = qty_format($order->line_items[$id]->qty_dispatched);
		$_POST['price'] = price_format($order->line_items[$id]->price);
		$_POST['Disc'] = percent_format($order->line_items[$id]->discount_percent*100);
		$_POST['units'] = $order->line_items[$id]->units;
		hidden('stock_id', $_POST['stock_id']);
		label_cell($_POST['stock_id']);
		label_cell($order->line_items[$line_no]->item_description, "nowrap");
	}
	else
	{

		if (get_company_pref('no_item_list'))
		{
			echo "<td colspan=2 nowrap>\n";
			stock_items_list('stock_id', null, false, true);
			echo "</td>\n";
		}
		else
		{
		text_cells(null, "StockID2", '', 12, 10, "", "", "class='combo' rel='stock_id'"/* onblur='this.form.submit();'"*/);
		stock_items_list_cells(null, 'stock_id', null, false, true, "class='combo' rel='StockID2'");
		}
		$item_info = get_item_edit_info($_POST['stock_id']);
		$_POST['units'] = $item_info["units"];

		$_POST['qty'] = qty_format(1);
		$_POST['price'] = price_format(get_price ($_POST['stock_id'],
			$order->customer_currency, $order->sales_type, 
			$order->price_factor, $order->document_date));
		// default to the customer's discount %
		$_POST['Disc'] = percent_format($order->default_discount * 100);
	}

	qty_cells(null, 'qty', qty_format($_POST['qty']));

	if ($order->trans_no!=0) {
		amount_cell($line_no==-1 ? 0 :$order->line_items[$line_no]->qty_done);
	}

	label_cell($_POST['units']);
	amount_cells(null, 'price');
	small_amount_cells(null, 'Disc', percent_format($_POST['Disc']), null, null, user_percent_dec());

	$line_total = $_POST['qty'] * $_POST['price'] * (1 - $_POST['Disc'] / 100);
	amount_cell($line_total);

	if ($id!=-1)
	{
		edit_button_cell('UpdateItem', _("Update"));
		edit_button_cell('CancelItemChanges', _("Cancel"));
		hidden('LineNo', $line_no);
		set_focus('qty');
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
	if ($order->trans_type==10)
	{
		$title = _("Delivery Details");
		$delname = _("Due Date").':';
	}
	elseif ($order->trans_type==13)
	{
		$title = _("Invoice Delivery Details");
		$delname = _("Invoice before").':';
	}
	else
	{
		$title = _("Order Delivery Details");
		$delname = _("Required Delivery Date").':';
	}
	display_heading($title);
	echo "<br>";
	start_table("$table_style2 width=90%");
	echo "<tr valign=top><td>"; // outer table

	echo "<table>";

//	echo $order->Location, $_POST['Location'];

	locations_list_row(_("Deliver from Location:"), 'Location', $_POST['Location']);

	date_row($delname, 'delivery_date', $order->due_date, 0, 0, 0);

	text_row(_("Deliver To:"), 'deliver_to', $order->deliver_to, 40, 40);

	textarea_row(_("Address:"), 'delivery_address', $order->delivery_address, 35, 5);
	text_row(_("Contact Phone Number:"), 'phone', $order->phone, 25, 25);

	echo "</table>";

	echo "</td><td>"; // outer table

	echo "<table>";

	text_row(_("Customer Reference:"), 'cust_ref', $order->cust_ref, 25, 25);
	textarea_row(_("Comments:"), "Comments", $order->Comments, 31, 5);

	small_amount_row(_("Shipping Charge:"), 'freight_cost',
	    price_format($_POST['freight_cost']));

	shippers_list_row(_("Shipping Company:"), 'ship_via', $order->ship_via);

	echo "</table>";

	echo "</td></tr>";
	end_table(1); // outer table
}

?>