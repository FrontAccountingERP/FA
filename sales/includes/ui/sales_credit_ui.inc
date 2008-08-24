<?php

include_once($path_to_root . "/includes/ui.inc");

// ------------------------------------------------------------------------------

function display_credit_header(&$order)
{
	global $table_style, $Ajax;
	start_table("width=80% $table_style");
	echo "<tr><td valign=top>"; // outer table
	echo "<table>";

	$customer_error = "";
	$change_prices = 0;

    if (!isset($_POST['customer_id']) && (get_global_customer() != reserved_words::get_all()))
    	$_POST['customer_id'] = get_global_customer();

	customer_list_row(_("Customer:"), 'customer_id', null, false, true);

	if ($order->customer_id != $_POST['customer_id'] /*|| $order->sales_type != $_POST['sales_type_id']*/)
	{
		// customer has changed

		// delete all the order items - drastic but necessary because of
		// change of currency, sales type, etc
//		$order->clear_items();

		// clear the branch selection
//		unset($_POST['branch_id']);
			$_POST['branch_id'] = '';
			$Ajax->activate('branch_id');
	}

	customer_branches_list_row(_("Branch:"), $_POST['customer_id'], 
	  'branch_id', null, false, true, true);

	//if (($_SESSION['credit_items']->order_no == 0) ||
	//	($order->customer_id != $_POST['customer_id']) ||
	//	($order->Branch != $_POST['branch_id']))
	//	$customer_error = get_customer_details_to_order($order, $_POST['customer_id'], $_POST['branch_id']);
	if (($order->customer_id != $_POST['customer_id']) ||
		($order->Branch != $_POST['branch_id']))
	  {

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
				    $Ajax->activate('sales_type_id');
				    $change_prices = 1;
				}
				unset($old_order);
	  }
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
	
	if ($order->sales_type != $_POST['sales_type_id']) {
		$myrow = get_sales_type($_POST['sales_type_id']);
		$order->set_sales_type($myrow['id'], $myrow['sales_type'],
		$myrow['tax_included'], $myrow['factor']);
		$Ajax->activate('sales_type_id');
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

	if (!isset($_POST['OrderDate']) || $_POST['OrderDate'] == "")
		$_POST['OrderDate'] = $order->document_date;

	date_row(_("Date:"), 'OrderDate');

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
    div_start('items_table');
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
    	    qty_cell($line->qty_dispatched, false, get_qty_dec($line->stock_id));
    	    label_cell($line->units);
    	    amount_cell($line->price);

	    amount_cell($line->discount_percent * 100);
    	    amount_cell($line_total);

    	    edit_button_cell("Edit$line_no", _('Edit'),
				_('Edit document line'));
    	    edit_button_cell("Delete$line_no", _('Delete'),
				_('Remove line from document'));

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
    div_end();
}

//---------------------------------------------------------------------------------

function credit_edit_item_controls(&$order, $rowcounter, $line_no=-1)
{
	global $Ajax;
	alt_table_row_color($rowcounter);
	$id = find_submit('Edit');

	if ($line_no!=-1 && $line_no == $id)
	{
		$_POST['stock_id'] = $order->line_items[$id]->stock_id;
		$_POST['qty'] = qty_format($order->line_items[$id]->qty_dispatched, $_POST['stock_id'], $dec);
		$_POST['price'] = price_format($order->line_items[$id]->price);
		$_POST['Disc'] = percent_format(($order->line_items[$id]->discount_percent)*100);
		$_POST['units'] = $order->line_items[$id]->units;
		hidden('stock_id', $_POST['stock_id']);
		label_cell($_POST['stock_id']);
		label_cell($order->line_items[$id]->item_description, "nowrap");
	    $Ajax->activate('items_table');
	}
	else
	{
		stock_items_list_cells(null,'stock_id', null, false, true);
		if(isset($_POST['_stock_id_update'])) {
			    $Ajax->activate('price');
			    $Ajax->activate('qty');
			    $Ajax->activate('units');
			    $Ajax->activate('line_total');
		}
  		$item_info = get_item_edit_info($_POST['stock_id']);

		$_POST['units'] = $item_info["units"];
   		$_POST['qty'] = qty_format(0, $_POST['stock_id'], $dec);
		$_POST['price'] = price_format(get_price($_POST['stock_id'], $order->customer_currency,
		    $order->sales_type, $order->price_factor, $order->document_date));

		// default to the customer's discount %
		$_POST['Disc'] = percent_format($order->default_discount * 100);
	}

	qty_cells(null, 'qty', $_POST['qty'], null, null, $dec);

	label_cell($_POST['units']);
	amount_cells(null, 'price',  null);
	small_amount_cells(null, 'Disc', percent_format(0), null, null, user_percent_dec());

	amount_cell($_POST['qty'] * $_POST['price'] * (1 - $_POST['Disc']/100));

	if ($id!=-1)
	{
		edit_button_cell('UpdateItem', _("Update"),
				_('Confirm changes'));
		edit_button_cell('CancelItemChanges', _("Cancel"),
				_('Cancel changes'));
		hidden('line_no', $line_no);
		set_focus('qty');
	}
	else
	{
		submit_cells('AddItem', _("Add Item"), "colspan=2",
		    _('Add new item to document'), true);
	}

	end_row();
}


//---------------------------------------------------------------------------------

function credit_options_controls()
{
	global $table_style2, $Ajax;
	echo "<br>";

if (isset($_POST['_CreditType_update']))
	$Ajax->activate('options');

 div_start('options');
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
 div_end();
}


//---------------------------------------------------------------------------------

?>