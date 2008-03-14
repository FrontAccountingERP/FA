<?php

include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/ui/items_cart.inc");

//--------------------------------------------------------------------------------

function add_to_order(&$order, $new_item, $new_item_qty, $standard_cost)
{
    $already_on_order = 0;

	foreach ($order->line_items as $order_item) 
	{
        if (strcasecmp($order_item->stock_id, $new_item) == 0) 
        {
            $already_on_order = 1;
            display_error(_("For Part :") . $new_item . " " . "This item is already on this order.  You can change the quantity ordered of the existing line if necessary.");
        }
	}

    if ($already_on_order != 1)
    {
         $order->add_to_cart ($new_item, $new_item_qty, $standard_cost);
    } /* end of if not already on the order */
}

//--------------------------------------------------------------------------------

function display_order_header(&$order)
{
	global $table_style;

	start_table("width=70% $table_style");
	echo "<tr><td width=33% valign=top>"; // outer table
	echo "<table>";

	locations_list_row(_("From Location:"), 'FromStockLocation', null);
	locations_list_row(_("To Location:"), 'ToStockLocation', null);

	echo "</table>";

	echo "</td><td width=33%>"; // outer table

	echo "<table>";

	ref_row(_("Reference:"), 'ref', references::get_next(systypes::location_transfer()));

    date_row(_("Date:"), 'AdjDate');

	echo "</table>";

	echo "</td><td width=33%>"; // outer table

	echo "<table>";
    movement_types_list_row(_("Transfer Type:"), 'type', null);
	echo "</table>";

	echo "</td></tr>";
	end_table(1); // outer table
}

//---------------------------------------------------------------------------------

function display_transfer_items($title, &$order)
{
	global $table_style, $path_to_root;

	display_heading($title);
	start_table("$table_style width=80%");
	$th = array(_("Item Code"), _("Item Description"), _("Quantity"), _("Unit"),'');
	table_header($th);
	$subtotal = 0;
	$k = 0;  //row colour counter

	foreach ($order->line_items as $stock_item) 
	{

		$quantity = number_format2($stock_item->quantity,user_qty_dec());

		if (!isset($_GET['Edit']) || $_GET['Edit'] != $stock_item->stock_id)
		{
    		alt_table_row_color($k);

    	 	label_cell("<a target='_blank' href='$path_to_root/inventory/inquiry/stock_status.php?" . SID . "stock_id=" . $stock_item->stock_id . "'>$stock_item->stock_id</a>");
    		label_cell($stock_item->item_description);
    		label_cell($quantity, "nowrap align=right");
    		label_cell($stock_item->units);

			edit_link_cell(SID . "Edit=$stock_item->stock_id");
			delete_link_cell(SID . "Delete=$stock_item->stock_id");
    		end_row();
		} 
		else 
		{
			transfer_edit_item_controls($order, $stock_item->stock_id);
		}
	}

	if (!isset($_GET['Edit']))
		transfer_edit_item_controls($order);

    end_table();
}

//---------------------------------------------------------------------------------

function transfer_edit_item_controls(&$order, $stock_id=null)
{
	start_row();

	if (isset($_GET['Edit']) and $stock_id!=null)
	{
		if (!isset($_POST['stock_id']))
			$_POST['stock_id'] = $order->line_items[$stock_id]->stock_id;
		if (!isset($_POST['qty']) || $_POST['qty'] == '')
			$_POST['qty'] = qty_format($order->line_items[$stock_id]->quantity);

		$_POST['units'] = $order->line_items[$stock_id]->units;

		hidden('stock_id', $_POST['stock_id']);
		label_cell($_POST['stock_id']);
		label_cell($order->line_items[$stock_id]->item_description);
	}
	else
	{
    	echo "<TD colspan=2>";
    	stock_costable_items_list('stock_id', $_POST['stock_id'], false, true);
    	echo "</TD>";

    	$item_info = get_item_edit_info($_POST['stock_id']);

   		$_POST['qty'] = qty_format(0);
		$_POST['units'] = $item_info["units"];
	}


	small_amount_cells(null, 'qty', $_POST['qty']);
	label_cell($_POST['units']);

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

function transfer_options_controls()
{
	  echo "<br>";
	  start_table();

	  textarea_row(_("Memo"), 'memo_', null, 50, 3);

	  end_table(1);
}


//---------------------------------------------------------------------------------

?>