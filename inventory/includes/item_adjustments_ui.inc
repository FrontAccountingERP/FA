<?php

include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/ui/items_cart.inc");

//--------------------------------------------------------------------------------

function add_to_order(&$order, $new_item, $new_item_qty, $standard_cost)
{
    $allready_on_order = 0;

	foreach ($order->line_items as $order_item) 
	{
        if (strcasecmp($order_item->stock_id, $new_item) == 0) 
        {
            $allready_on_order = 1;
            display_error(_("For Part :") . $new_item . " " . "This item is already on this order.  You can change the quantity ordered of the existing line if necessary.");
        }
	}

    if ($allready_on_order != 1)
    {
         $order->add_to_cart ($new_item, $new_item_qty, $standard_cost);
    } /* end of if not already on the order */
}

//--------------------------------------------------------------------------------

function display_order_header(&$order)
{
	global $table_style2;
	
	start_table("width=70% $table_style2"); // outer table
	echo "<tr><td valign=top width=33%>"; 
	echo "<table>"; // inner table

	locations_list_row(_("Location:"), 'StockLocation', null);
	ref_row(_("Reference:"), 'ref', references::get_next(systypes::inventory_adjustment()));

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
	start_table("$table_style width=80%");
	$th = array(_("Item Code"), _("Item Description"), _("Quantity"), 
		_("Unit"), _("Unit Cost"), _("Total"));
	if ( count($order->line_items)) $th[] = '';

	table_header($th);
	$total = 0;
	$k = 0;  //row colour counter

	foreach ($order->line_items as $stock_item) 
	{

		$quantity = number_format2($stock_item->quantity,user_qty_dec());

		$total += ($stock_item->standard_cost * $stock_item->quantity);

		if (!isset($_GET['Edit']) || $_GET['Edit'] != $stock_item->stock_id)
		{
    		alt_table_row_color($k);

			label_cell("<a target='_blank' href='$path_to_root/inventory/inquiry/stock_status.php?" . SID . "stock_id=" . $stock_item->stock_id . "'>$stock_item->stock_id</a>");
			label_cell($stock_item->item_description);
			label_cell($quantity, "nowrap align=right");
			label_cell($stock_item->units);
			amount_cell($stock_item->standard_cost);
			amount_cell($stock_item->standard_cost * $stock_item->quantity);

			edit_link_cell(SID . "Edit=$stock_item->stock_id");
			delete_link_cell(SID . "Delete=$stock_item->stock_id");
			end_row();
		} 
		else 
		{
			adjustment_edit_item_controls($order, $stock_item->stock_id);
		}
	}

	if (!isset($_GET['Edit']))
		adjustment_edit_item_controls($order);

	label_row(_("Total"), number_format2($total,user_price_dec()), "colspan=5", "align=right");

    end_table();
}

//---------------------------------------------------------------------------------

function adjustment_edit_item_controls(&$order, $stock_id=null)
{
	start_row();

	if (isset($_GET['Edit']) and $stock_id!=null)
	{
		if (!isset($_POST['stock_id']))
			$_POST['stock_id'] = $order->line_items[$stock_id]->stock_id;
		if (!isset($_POST['qty']) OR ($_POST['qty']==""))
			$_POST['qty'] = qty_format($order->line_items[$stock_id]->quantity);
		if (!isset($_POST['std_cost']) OR ($_POST['std_cost']==""))
			$_POST['std_cost'] = price_format($order->line_items[$stock_id]->standard_cost);

		$_POST['units'] = $order->line_items[$stock_id]->units;

		hidden('stock_id', $_POST['stock_id']);
		label_cell($_POST['stock_id']);
		label_cell($order->line_items[$stock_id]->item_description);
	}
	else
	{
    	echo "<td colspan=2>";
    	stock_costable_items_list('stock_id', $_POST['stock_id'], false, true);
    	echo "</td>";

    	$item_info = get_item_edit_info($_POST['stock_id']);

   		$_POST['qty'] = qty_format(0);
		$_POST['std_cost'] = price_format($item_info["standard_cost"]);
		$_POST['units'] = $item_info["units"];
	}

	amount_cells(null, 'qty', $_POST['qty']);

	label_cell($_POST['units']);
	amount_cells(null, 'std_cost', $_POST['std_cost']);
	label_cell("&nbsp;");

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

function adjustment_options_controls()
{
	  echo "<br>";
	  start_table();

	  textarea_row(_("Memo"), 'memo_', null, 50, 3);

	  end_table(1);
}


//---------------------------------------------------------------------------------

?>