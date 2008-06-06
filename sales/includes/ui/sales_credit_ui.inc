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

	if ($order->customer_id != $_POST['customer_id'] || $order->sales_type != $_POST['sales_type_id'])
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
	if ($_SESSION['Items']->trans_no==0)
	    ref_row(_("Reference").':', 'ref');
	else
	    label_row(_("Reference").':', $_POST['ref'] );
	


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
    	$_POST['sales_type_id'] = $order->sales_type;
    sales_types_list_row(_("Sales Type"), 'sales_type_id', $_POST['sales_type_id'], true);

	label_row(_("Customer Discount:"), ($order->default_discount * 100) . "%");
	echo "</table>";

	echo "</td><td>"; // outer table

	echo "<table height='5'>";

	if (!isset($_POST['OrderDate']) || $_POST['OrderDate'] == "")
		$_POST['OrderDate'] = $order->document_date;

	date_row(_("Date:"), 'OrderDate');

//	if (!isset($_POST['tax_group_id']) || $_POST['tax_group_id'] == "")
//		$_POST['tax_group_id'] = $order->tax_group_id;
//    tax_groups_list_row(_("Tax Group:"), 'tax_group_id', null, true);
    shippers_list_row(_("Shipping Company:"), 'ShipperID', $order->ship_via);

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
	_("Price"), _("Discount %"), _("Total"),'');

    if (count($order->line_items)) $th[]= '';

    table_header($th);

    $subtotal = 0;
    $k = 0;  //row colour counter

    $id = find_submit('Edit');

    foreach ($order->line_items as $line_no=>$line)
    {
	$line_total =	round($line->qty_dispatched * $line->price * (1 - $line->discount_percent),
	   user_price_dec());

	if ( $id != $line_no)
	{
	    alt_table_row_color($k);

    	    label_cell("<a target='_blank' href='$path_to_root/inventory/inquiry/stock_status.php?" . SID . "stock_id=" . $line->stock_id . "'>$line->stock_id</a>");
    	    label_cell($line->item_description, "nowrap");
    	    qty_cell($line->qty_dispatched);
    	    label_cell($line->units);
    	    amount_cell($line->price);

	    amount_cell($line->discount_percent * 100);
    	    amount_cell($line_total);

    	    edit_button_cell("Edit$line_no", _('Edit'));
    	    edit_button_cell("Delete$line_no", _('Delete'));

    	    end_row();
        }
        else
        {
	    credit_edit_item_controls($order, $k, $line_no);
	}

	$subtotal += $line_total;
    }

    if ($id==-1)
        credit_edit_item_controls($order, $k);

    $display_sub_total = price_format($subtotal);
    label_row(_("Sub-total"), $display_sub_total, "colspan=6 align=right", "align=right", 2);

    if (!isset($_POST['ChargeFreightCost']) OR ($_POST['ChargeFreightCost'] == ""))
	$_POST['ChargeFreightCost'] = price_format(0);

    amount_cells_ex(_("Shipping"), 'ChargeFreightCost', 8, 8, $_POST['ChargeFreightCost'], "colspan=6 align=right");
    label_cell('', 'colspan=2');

    $taxes = $order->get_taxes($_POST['ChargeFreightCost']);

    $tax_total = display_edit_tax_items($taxes, 6, $_SESSION['Items']->tax_included);

    $display_total = price_format(($subtotal + $_POST['ChargeFreightCost'] + $tax_total));

    label_row(_("Credit Note Total"), $display_total, "colspan=6 align=right","class='amount'", 2);

    end_table();
}

//---------------------------------------------------------------------------------

function credit_edit_item_controls(&$order, $rowcounter, $line_no=-1)
{
	alt_table_row_color($rowcounter);
	$id = find_submit('Edit');

	if ($line_no!=-1 && $line_no == $id)
	{

		$_POST['stock_id'] = $order->line_items[$id]->stock_id;
		$_POST['qty'] = qty_format($order->line_items[$id]->qty_dispatched);
		$_POST['price'] = price_format($order->line_items[$id]->price);
		$_POST['Disc'] = percent_format(($order->line_items[$id]->discount_percent)*100);
		$_POST['units'] = $order->line_items[$id]->units;
		hidden('stock_id', $_POST['stock_id']);
		label_cell($_POST['stock_id']);
		label_cell($order->line_items[$id]->item_description, "nowrap");
	}
	else
	{
		stock_items_list_cells(null,'stock_id', null, false, true);
  		$item_info = get_item_edit_info($_POST['stock_id']);

		$_POST['units'] = $item_info["units"];

   		$_POST['qty'] = qty_format(0);
		$_POST['price'] = price_format(get_price($_POST['stock_id'], $order->customer_currency,
		    $order->sales_type, $order->price_factor, $order->document_date));

		// default to the customer's discount %
		$_POST['Disc'] = percent_format($order->default_discount * 100);
	}


	qty_cells(null, 'qty', $_POST['qty']);
//	if ($order->trans_no!=0) {
//		amount_cell($line_no==-1 ? 0 :$order->line_items[$line_no]->qty_done);
//	}
	label_cell($_POST['units']);
	amount_cells(null, 'price',  null);
	small_amount_cells(null, 'Disc', percent_format(0), null, null, user_percent_dec());

	amount_cell($_POST['qty'] * $_POST['price'] * (1 - $_POST['Disc']/100));

	if ($id!=-1)
	{
		edit_button_cell('UpdateItem', _("Update"));
		edit_button_cell('CancelItemChanges', _("Cancel"));
		hidden('line_no', $line_no);
		set_focus('qty');
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
			$_POST['Location'] = $_SESSION['Items']->Location;
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