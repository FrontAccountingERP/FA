<?php

include_once($path_to_root . "/includes/ui.inc");

// ------------------------------------------------------------------------------

function display_credit_header(&$order)
{
	global $table_style;
	start_table("width=80% $table_style");
	echo "<tr><td valign=top>"; // outer table
	echo "<table>";

	$customer_error = "";

    if (!isset($_POST['customer_id']) && (get_global_customer() != reserved_words::get_all()))
    	$_POST['customer_id'] = get_global_customer();

	customer_list_row(_("Customer:"), 'customer_id', null, false, true);

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

	//if (($_SESSION['credit_items']->order_no == 0) ||
	//	($order->customer_id != $_POST['customer_id']) ||
	//	($order->Branch != $_POST['branch_id']))
	//	$customer_error = get_customer_details_to_order($order, $_POST['customer_id'], $_POST['branch_id']);
	if (($order->customer_id != $_POST['customer_id']) ||
		($order->Branch != $_POST['branch_id']))
		$customer_error = get_customer_details_to_order($order, $_POST['customer_id'], $_POST['branch_id']);

	set_global_customer($_POST['customer_id']);

	if (!isset($_POST['ref']))
		$_POST['ref'] = references::get_next(11);

	ref_row(_("Reference:"), 'ref');

	echo "</table>";

	echo "</td><td>"; // outer table

	if (!is_company_currency($order->customer_currency))
	{
		echo "<table height='5'>";
		label_row(_("Customer Currency:"), $order->customer_currency);
		exchange_rate_display($order->customer_currency, get_company_currency(),
			$_POST['OrderDate'], true);
		echo "</table>";
		echo "</td><td>"; // outer table
	}

	echo "<table height='5'>";

    if (!isset($_POST['sales_type_id']))
    	$_POST['sales_type_id'] = $order->default_sales_type;
    sales_types_list_row(_("Sales Type"), 'sales_type_id', $_POST['sales_type_id']);

	label_row(_("Customer Discount:"), ($order->default_discount * 100) . "%");
	echo "</table>";

	echo "</td><td>"; // outer table

	echo "<table height='5'>";

	if (!isset($_POST['OrderDate']) || $_POST['OrderDate'] == "")
		$_POST['OrderDate'] = $order->orig_order_date;

	date_row(_("Date:"), 'OrderDate');

	if (!isset($_POST['tax_group_id']) || $_POST['tax_group_id'] == "")
		$_POST['tax_group_id'] = $order->tax_group_id;
    tax_groups_list_row(_("Tax Group:"), 'tax_group_id', null, true);

	echo "</table>";

	echo "</td></tr>";

	end_table(1); // outer table

	return $customer_error;
}

//---------------------------------------------------------------------------------

function display_credit_items($title, &$order)
{
	global $table_style, $path_to_root;

	display_heading($title);
	start_table("$table_style width=90%");
	$th = array(_("Item Code"), _("Item Description"), _("Quantity"), _("Unit"),
		_("Price"), _("Discount %"), _("Total"));
	table_header($th);

	$subtotal = 0;
	$k = 0;  //row colour counter

	foreach ($order->line_items as $line)
	{

		$line_total =	$line->quantity * $line->price * (1 - $line->discount_percent);

		if (!isset($_GET['Edit']) || $_GET['Edit'] != $line->line_no)
		{
    		alt_table_row_color($k);

    	 	label_cell("<a target='_blank' href='$path_to_root/inventory/inquiry/stock_status.php?" . SID . "stock_id=" . $line->stock_id . "'>$line->stock_id</a>");
    		label_cell($line->item_description);
    		qty_cell($line->quantity);
    		label_cell($line->units);
    		amount_cell($line->price);

   			amount_cell($line->discount_percent * 100);
    		amount_cell($line_total);

    		edit_link_cell(SID . "Edit=$line->line_no");
    		delete_link_cell(SID . "Delete=$line->line_no");

			//labelt_cell(get_tax_free_price_for_item($line->stock_id, $line_total, $_POST['tax_group_id']));

    		end_row();
		}
		else
		{
			credit_edit_item_controls($order, $line->stock_id);
		}

		$subtotal += $line_total;
	}

	if (!isset($_GET['Edit']))
		credit_edit_item_controls($order);

	$display_sub_total = number_format2($subtotal,user_price_dec());
	label_row(_("Sub-total"), $display_sub_total, "colspan=6 align=right", "align=right");

	if (!isset($_POST['ChargeFreightCost']) OR ($_POST['ChargeFreightCost'] == ""))
		$_POST['ChargeFreightCost'] = 0;

	text_row(_("Shipping"), 'ChargeFreightCost', $_POST['ChargeFreightCost'], 8, 8, "colspan=6 align=right");

    $taxes = $order->get_taxes($_POST['tax_group_id'], $_POST['ChargeFreightCost']);

	$tax_total = display_edit_tax_items($taxes, 6);

    $display_total = number_format2(($subtotal + $_POST['ChargeFreightCost'] + $tax_total), user_price_dec());
    label_row(_("Credit Note Total"), $display_total, "colspan=6 align=right","align=right");

    end_table();
}

//---------------------------------------------------------------------------------

function credit_edit_item_controls(&$order, $stock_id=null)
{
	start_row();

	if (isset($_GET['Edit']) and $stock_id!=null)
	{
		$line_no = $_GET['Edit'];
		hidden('line_no', $line_no);

		if (!isset($_POST['stock_id']))
			$_POST['stock_id'] = $order->line_items[$line_no]->stock_id;
		if (!isset($_POST['qty']) OR ($_POST['qty']==""))
			$_POST['qty'] = $order->line_items[$line_no]->quantity;
		if (!isset($_POST['price']) OR ($_POST['price']==""))
			$_POST['price'] = $order->line_items[$line_no]->price;
		if (!isset($_POST['Disc']) OR ($_POST['Disc']==""))
			$_POST['Disc'] = ($order->line_items[$line_no]->discount_percent)*100;

		$_POST['units'] = $order->line_items[$line_no]->units;

		hidden('stock_id', $_POST['stock_id']);
		label_cell($_POST['stock_id']);
		label_cell($order->line_items[$line_no]->item_description);
	}
	else
	{
		hidden('line_no', ($_SESSION['credit_items']->lines_on_order + 1));
    	echo "<td colspan=2>";
    	stock_items_list('stock_id', null, false, true);
    	echo "</td>";

    	$item_info = get_item_edit_info($_POST['stock_id']);

		$_POST['units'] = $item_info["units"];

   		$_POST['qty'] = 0;
		$_POST['price'] = get_price($_POST['stock_id'], $order->customer_id);
		// default to the customer's discount %
		$_POST['Disc'] = $order->default_discount * 100;
	}


	text_cells(null, 'qty', $_POST['qty'], 13, 15);
	label_cell($_POST['units']);
	text_cells(null, 'price',  null, 13, 15);
	text_cells(null, 'Disc', $_POST['Disc'], 7, 5);
	amount_cell($_POST['qty'] * $_POST['price'] * (1 - $_POST['Disc']/100));

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


//---------------------------------------------------------------------------------

function credit_options_controls()
{
	global $table_style2;
	echo "<br>";
	start_table("$table_style2");

	credit_type_list_row(_("Credit Note Type"), 'CreditType', null, true);

	if ($_POST['CreditType'] == "Return")
	{

		/*if the credit note is a return of goods then need to know which location to receive them into */
		if (!isset($_POST['Location']))
			$_POST['Location'] = $_SESSION['credit_items']->Location;
	   	locations_list_row(_("Items Returned to Location"), 'Location', $_POST['Location']);

	}
	else
	{
		/* the goods are to be written off to somewhere */
		gl_all_accounts_list_row(_("Write off the cost of the items to"), 'WriteOffGLCode', null);
	}

	textarea_row(_("Memo"), "CreditText", null, 51, 3);
	echo "</table>";
}


//---------------------------------------------------------------------------------

?>