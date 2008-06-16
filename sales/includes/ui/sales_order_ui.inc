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
	global $table_style, $path_to_root, $Ajax;

	display_heading($title);

    div_start('items_table');
	start_table("$table_style colspan=7 width=90%");
	$th = array(_("Item Code"), _("Item Description"), _("Quantity"),
		_("Delivered"),
		_("Unit"), _("Price"), _("Discount %"), _("Total"), "");

	if ($order->trans_no == 0) {
	unset( $th[3] );
	}

	if (count($order->line_items))
	     $th[]= '';

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
			qty_cell($stock_item->qty_dispatched, false, get_qty_dec($stock_item->stock_id));

			if ($order->trans_no!=0)
				amount_cell($stock_item->qty_done);

			label_cell($stock_item->units);
			amount_cell($stock_item->price);

			percent_cell($stock_item->discount_percent * 100);
			amount_cell($line_total);

			if ($editable_items)
			{
				edit_button_cell("Edit$line_no", _("Edit"),
				_('Edit document line'));
				edit_button_cell("Delete$line_no", _("Delete"),
				_('Remove line from document'));
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
    div_end();
}

// ------------------------------------------------------------------------------

function display_order_header(&$order, $editable, $date_text, $display_tax_group=false)
{
	global $table_style, $Ajax;

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
		customer_list_row(_("Customer:"), 'customer_id', null, false, true);

		if ($order->customer_id != get_post('customer_id', -1))
		{
			// customer has changed
			// clear the branch selection
			$_POST['branch_id'] = '';
			$Ajax->activate('branch_id');
  		}
		customer_branches_list_row(_("Branch:"),
	  	  $_POST['customer_id'], 'branch_id', null, false, true, true);

		if (!isset($_POST['branch_id']) || $_POST['branch_id'] == "")
		{
			// ignore errors on customer search box call
			if ($_POST['customer_id'] == '')
			    $customer_error = _("No customer found for entered text.");
			else
			    $customer_error = _("The selected customer does not have any branches. Please create at least one branch.");
		    unset($_POST['branch_id']);
		    $order->Branch = 0;
		} else
		{
		if( ($order->customer_id != get_post('customer_id', -1)) ||
			($order->Branch != get_post('branch_id', -1))) {

				$old_order = (PHP_VERSION<5) ? $order : clone( $order );
				$customer_error = get_customer_details_to_order($order, $_POST['customer_id'], $_POST['branch_id']);

				$_POST['Location'] = $order->Location;
				$_POST['deliver_to'] = $order->deliver_to;
				$_POST['delivery_address'] = $order->delivery_address;
				$_POST['phone'] = $order->phone;
				$Ajax->activate('Location');
				$Ajax->activate('deliver_to');
				$Ajax->activate('phone');
				$Ajax->activate('delivery_address');
				// change prices if necessary
				// what about discount in template case?
				if ($old_order->customer_currency != $order->customer_currency) {
				    $change_prices = 1;
				}
				if ($old_order->sales_type != $order->sales_type) {
				//  || $old_order->default_discount!=$order->default_discount
					$_POST['sales_type'] = $order->sales_type;
				    $Ajax->activate('sales_type');
				    $change_prices = 1;
				}
				unset($old_order);
			}
		} // changed branch
		set_global_customer($_POST['customer_id']);
	}

	if ($_SESSION['Items']->trans_type!=30) {
		ref_cells(_("Reference").':', 'ref', _('Reference number unique for this document type'), null, '');
	}

	echo "</table>";

	echo "</td><td>"; // outer table

	if (!is_company_currency($order->customer_currency))
	{
	    div_start('currency');
		echo "<table height='5'>";
		label_row(_("Customer Currency:"), $order->customer_currency);
		exchange_rate_display($order->customer_currency, get_company_currency(),
			($editable ? $_POST['OrderDate'] : $order->document_date), $editable);
		echo "</table>";
	    div_end();
		echo "</td><td>"; // outer table
	}


	echo "<table height='5'>";
	if($editable) {
		$str = sales_types_list_row(_("Price List"), 'sales_type', null, true);
	} else {
		label_row(_("Price List:"), $order->sales_type_name);
	}
	if ($order->sales_type != $_POST['sales_type']) {
		$myrow = get_sales_type($_POST['sales_type']);
		$order->set_sales_type($myrow['id'], $myrow['sales_type'],
		$myrow['tax_included'], $myrow['factor']);
		$Ajax->activate('sales_type');
		$change_prices = 1;
	}

	if ($change_prices != 0) {
		foreach ($order->line_items as $line_no=>$item) {
			$line = &$order->line_items[$line_no];
			$line->price = get_price($line->stock_id, $order->customer_currency,
				$order->sales_type, $order->price_factor, $order->document_date);
		//		$line->discount_percent = $order->default_discount;
		}
	    $Ajax->activate('items_table');
	}
	label_row(_("Customer Discount:"), ($order->default_discount * 100) . "%");
	echo "</table>";

	echo "</td><td>"; // outer table

	echo "<table height='5'>";

	if ($editable)
	{
		if (!isset($_POST['OrderDate']) || $_POST['OrderDate'] == "")
			$_POST['OrderDate'] = $order->document_date;

		date_row($date_text, 'OrderDate',
		  _('Date of order receive'));
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
    global $Ajax;

	alt_table_row_color($rowcounter);

	$id = find_submit('Edit');
	if ($line_no!=-1 && $line_no == $id)
	{
		$_POST['stock_id'] = $order->line_items[$id]->stock_id;
		$dec = get_qty_dec($_POST['stock_id']);
		$_POST['qty'] = number_format2($order->line_items[$id]->qty_dispatched, $dec);
		$_POST['price'] = price_format($order->line_items[$id]->price);
		$_POST['Disc'] = percent_format($order->line_items[$id]->discount_percent*100);
		$units = $order->line_items[$id]->units;
		hidden('stock_id', $_POST['stock_id']);
		label_cell($_POST['stock_id']);
		label_cell($order->line_items[$line_no]->item_description, "nowrap");
	    $Ajax->activate('items_table');
	}
	else
	{
		stock_items_list_cells(null,'stock_id', null, false, true);
		if(isset($_POST['_stock_id_update'])) {
			    $Ajax->activate('price');
			    $Ajax->activate('units');
			    $Ajax->activate('line_total');
		}

		$item_info = get_item_edit_info($_POST['stock_id']);
		$units = $item_info["units"];
		$dec = get_qty_dec($_POST['stock_id']);
		$_POST['qty'] = number_format2(1, $dec);

		$_POST['price'] = price_format(get_price ($_POST['stock_id'],
			$order->customer_currency, $order->sales_type,
			$order->price_factor, $order->document_date));
		// default to the customer's discount %
		$_POST['Disc'] = percent_format($order->default_discount * 100);

	}

	qty_cells(null, 'qty', $_POST['qty'], null, null, $dec);

	if ($order->trans_no!=0) {
		qty_cell($line_no==-1 ? 0 :$order->line_items[$line_no]->qty_done, false, $dec);
	}

	label_cell($units, '', 'units');

	$str = amount_cells(null, 'price');

	small_amount_cells(null, 'Disc', percent_format($_POST['Disc']), null, null, user_percent_dec());

	$line_total = $_POST['qty'] * $_POST['price'] * (1 - $_POST['Disc'] / 100);

	amount_cell($line_total, false, '','line_total');

	if ($id!=-1)
	{
		edit_button_cell('UpdateItem', _("Update"),
				_('Confirm changes'));
		edit_button_cell('CancelItemChanges', _("Cancel"),
				_('Cancel changes'));
		hidden('LineNo', $line_no);
		set_focus('qty');
		$Ajax->addFocus(true, 'qty');
	}
	else
	{
		submit_cells('AddItem', _("Add Item"), "colspan=2",
		    _('Add new item to document'), true);
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

	locations_list_row(_("Deliver from Location:"), 'Location', $order->Location);

	date_row($delname, 'delivery_date',
	    _('Enter requested day of delivery'), $order->due_date, 0, 0, 0);

	text_row(_("Deliver To:"), 'deliver_to', $order->deliver_to, 40, 40,
	_('Additional identifier for delivery e.g. name of receiving person'));

	textarea_row(_("Address:"), 'delivery_address', $order->delivery_address, 35, 5,
	_('Delivery address. Default is address of customer branch'));
	text_row(_("Contact Phone Number:"), 'phone', $order->phone, 25, 25,
	    _('Phone number of ordering person. Defaults to branch phone number'));

	echo "</table>";

	echo "</td><td>"; // outer table

	echo "<table>";

	text_row(_("Customer Reference:"), 'cust_ref', $order->cust_ref, 25, 25,
	  _('Customer reference number for this order (if any)'));
	textarea_row(_("Comments:"), "Comments", $order->Comments, 31, 5);

	small_amount_row(_("Shipping Charge:"), 'freight_cost',
	    price_format(get_post('freight_cost',0)));

	shippers_list_row(_("Shipping Company:"), 'ship_via', $order->ship_via);

	echo "</table>";

	echo "</td></tr>";
	end_table(1); // outer table
}

?>