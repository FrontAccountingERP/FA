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
         $order->add_to_cart($new_item, $new_item_qty, $standard_cost);
    } /* end of if not already on the order */
}

//--------------------------------------------------------------------------------

function display_order_header(&$order)
{
	display_wo_details($_SESSION['issue_items']->order_id);
	echo "<br>";
}

//---------------------------------------------------------------------------------

function display_adjustment_items($Title, &$order)
{
	global $table_style, $path_to_root;

	display_heading($Title);
	start_table("$table_style colspan=7");
	$th = array(_("Item Code"), _("Item Description"), _("Quantity"),
		_("Unit"), _("Unit Cost"));
	table_header($th);	

	$subtotal = 0;
	$k = 0;  //row colour counter

	foreach ($order->line_items as $stock_item) 
	{

		if ($_GET['Edit'] != $stock_item->stock_id)
		{
    		alt_table_row_color($k);

    	 	label_cell("<a target='_blank' href='$path_to_root/inventory/inquiry/stock_status.php?" . SID . "stock_id=" . $stock_item->stock_id . "'>$stock_item->stock_id</a>");
    		label_cell($stock_item->item_description);
    		qty_cell($stock_item_quantity);
    		if (strlen($stock_item->units))
    			label_cell($stock_item->units);
    		else
    			label_cell("");
			amount_cell($stock_item->standard_cost);

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

    end_table();
}

//---------------------------------------------------------------------------------

function adjustment_edit_item_controls(&$order, $stock_id=null)
{
	start_row();

	if (isset($_GET['Edit']) && $stock_id != null)
	{
		if (!isset($_POST['stock_id']))
			$_POST['stock_id'] = $order->line_items[$stock_id]->stock_id;
		if (!isset($_POST['qty']) || ($_POST['qty'] == ""))
			$_POST['qty'] = $order->line_items[$stock_id]->quantity;
		if (!isset($_POST['std_cost']) || ($_POST['std_cost'] == ""))
			$_POST['std_cost'] = $order->line_items[$stock_id]->standard_cost;

		$_POST['units'] = $order->line_items[$stock_id]->units;

		hidden('stock_id', $_POST['stock_id']);
		label_cell($_POST['stock_id']);
		label_cell($order->line_items[$stock_id]->item_description);
	}
	else
	{
		$wo_details = get_work_order($_SESSION['issue_items']->order_id);

    	echo "<td colspan=2>";
    	stock_component_items_list('stock_id', $wo_details["stock_id"], $_POST['stock_id'], false, true);
    	echo "</td>";

    	$item_info = get_item_edit_info($_POST['stock_id']);

   		$_POST['qty'] = 0;
		$_POST['std_cost'] = $item_info["standard_cost"];
		$_POST['units'] = $item_info["units"];
	}


	text_cells(null, 'qty', $_POST['qty'], 13, 15);
	label_cell($_POST['units']);
	text_cells(null, 'std_cost', $_POST['std_cost'], 15, 14);

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

    ref_row(_("Reference:"), 'ref', '', references::get_next(28));

	if (!isset($_POST['IssueType']))
		$_POST['IssueType'] = 0;

	yesno_list_row(_("Type:"), 'IssueType', $_POST['IssueType'],
		_("Return Items to Location"), _("Issue Items to Work order"));

	locations_list_row(_("From Location:"), 'Location', $_POST['Location']);
	workcenter_list_row(_("To Work Centre:"), 'WorkCentre', $_POST['WorkCentre']);

	date_row(_("Issue Date:"), 'date_');

	textarea_row(_("Memo:"), 'memo_', $_POST['memo_'], 50, 3);

	end_table();
}


//---------------------------------------------------------------------------------

?>