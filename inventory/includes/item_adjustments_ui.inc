<?php

include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/ui/items_cart.inc");

//--------------------------------------------------------------------------------

function add_to_order(&$order, $new_item, $new_item_qty, $standard_cost)
{
    if ($order->find_cart_item($new_item))
         display_error(_("For Part :") . $new_item . " " . "This item is already on this order.  You can change the quantity ordered of the existing line if necessary.");
	else
         $order->add_to_cart (count($order->line_items), $new_item, $new_item_qty, $standard_cost);
}

//--------------------------------------------------------------------------------

function display_order_header(&$order)
{
	global $table_style2;

	start_table("width=70% $table_style2"); // outer table
	echo "<tr><td valign=top width=33%>";
	echo "<table>"; // inner table

	locations_list_row(_("Location:"), 'StockLocation', null);
	ref_row(_("Reference:"), 'ref', '', references::get_next(systypes::inventory_adjustment()));

	echo "</table>"; // inner table

	echo "</td><td width=33%>";

	echo "<table>"; // inner table

    date_row(_("Date:"), 'AdjDate');

	echo "</table>"; // inner table

	echo "</td><td width=33%>";

	echo "<table>"; // inner table
    movement_types_list_row(_("Detail:"), 'type', null);

    if (!isset($_POST['Increase']))
    	$_POST['Increase'] = 1;
    yesno_list_row(_("Type:"), 'Increase', $_POST['Increase'],
    	_("Positive Adjustment"), _("Negative Adjustment"));

	echo "</table>"; // inner table

	echo "</td></tr>";
	end_table(1); // outer table
}

//---------------------------------------------------------------------------------

function display_adjustment_items($title, &$order)
{
	global $table_style, $path_to_root;

	display_heading($title);
    div_start('items_table');
	start_table("$table_style width=80%");
	$th = array(_("Item Code"), _("Item Description"), _("Quantity"),
		_("Unit"), _("Unit Cost"), _("Total"));
	if ( count($order->line_items)) $th[] = '';

	table_header($th);
	$total = 0;
	$k = 0;  //row colour counter

	$id = find_submit('Edit');
	foreach ($order->line_items as $line_no=>$stock_item)
	{

		$total += ($stock_item->standard_cost * $stock_item->quantity);

		if ($id != $line_no)
		{
    		alt_table_row_color($k);

			label_cell("<a target='_blank' href='$path_to_root/inventory/inquiry/stock_status.php?" . SID . "stock_id=" . $stock_item->stock_id . "'>$stock_item->stock_id</a>");
			label_cell($stock_item->item_description);
    		qty_cell($stock_item->quantity, false, get_qty_dec($stock_item->stock_id));
			label_cell($stock_item->units);
			amount_cell($stock_item->standard_cost);
			amount_cell($stock_item->standard_cost * $stock_item->quantity);

			edit_button_cell("Edit$line_no", _("Edit"),
				_('Edit document line'));
			edit_button_cell("Delete$line_no", _("Delete"),
				_('Remove line from document'));
			end_row();
		}
		else
		{
			adjustment_edit_item_controls($order, $line_no);
		}
	}

	if ($id == -1)
		adjustment_edit_item_controls($order);

	label_row(_("Total"), number_format2($total,user_price_dec()), "colspan=5", "align=right");

    end_table();
	div_end();
}

//---------------------------------------------------------------------------------

function adjustment_edit_item_controls(&$order, $line_no=-1)
{
	global $Ajax;
	start_row();

	$id = find_submit('Edit');
	if ($line_no != -1 && $line_no == $id)
	{
		$_POST['stock_id'] = $order->line_items[$id]->stock_id;
		$_POST['qty'] = qty_format($order->line_items[$id]->quantity, 
			$order->line_items[$id]->stock_id, $dec);
		$_POST['std_cost'] = price_format($order->line_items[$id]->standard_cost);
		$_POST['units'] = $order->line_items[$id]->units;

		hidden('stock_id', $_POST['stock_id']);
		label_cell($_POST['stock_id']);
		label_cell($order->line_items[$id]->item_description);
	    $Ajax->activate('items_table');
	}
	else
	{
    	stock_costable_items_list_cells(null, 'stock_id', null, false, true);
		if(isset($_POST['_stock_id_update'])) {
			    $Ajax->activate('units');
			    $Ajax->activate('qty');
			    $Ajax->activate('std_cost');
		}

    	$item_info = get_item_edit_info($_POST['stock_id']);

   		$_POST['qty'] = qty_format(0, $_POST['stock_id'], $dec);
		$_POST['std_cost'] = price_format($item_info["standard_cost"]);
		$_POST['units'] = $item_info["units"];
	}

	qty_cells(null, 'qty', $_POST['qty'], null, null, $dec);
	label_cell($_POST['units'], '', 'units');

	amount_cells(null, 'std_cost', $_POST['std_cost']);
	label_cell("&nbsp;");

	if ($id != -1)
	{
		edit_button_cell('UpdateItem', _("Update"),
				_('Confirm changes'));
		edit_button_cell('CancelItemChanges', _("Cancel"),
				_('Cancel changes'));
		hidden('LineNo', $line_no);
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

function adjustment_options_controls()
{
	  echo "<br>";
	  start_table();

	  textarea_row(_("Memo"), 'memo_', null, 50, 3);

	  end_table(1);
}


//---------------------------------------------------------------------------------

?>