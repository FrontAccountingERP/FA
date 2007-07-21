<?php

include_once($path_to_root . "/purchasing/includes/purchasing_db.inc");

// ------------------------------------------------------------------------------

function get_supplier_details_to_order(&$order, $supplier_id)
{
	$sql = "SELECT curr_code,supp_name FROM ".TB_PREF."suppliers
		WHERE supplier_id = '$supplier_id'";
	$result = db_query($sql, "The supplier details could not be retreived");

	$myrow = db_fetch($result);

	$order->curr_code = $_POST['curr_code'] = $myrow["curr_code"];
	$order->supplier_name = $_POST['supplier_name'] = $myrow["supp_name"];
	$order->supplier_id = $_POST['supplier_id'] = $supplier_id;
}

//---------------------------------------------------------------------------------------------------

function create_new_po()
{
	if (isset($_SESSION['PO']))
	{
		unset ($_SESSION['PO']->line_items);
		$_SESSION['PO']->lines_on_order = 0;
		unset ($_SESSION['PO']);
	}

	session_register("PO");

	$_SESSION['PO'] = new purch_order;
	$_POST['OrderDate'] = Today();
	if (!is_date_in_fiscalyear($_POST['OrderDate']))
		$_POST['OrderDate'] = end_fiscalyear();
	$_SESSION['PO']->orig_order_date = $_POST['OrderDate'];	
}

//---------------------------------------------------------------------------------------------------

function display_po_header(&$order)
{
	global $table_style2;

	$editable = ($order->order_no == 0);

	start_table("width=80% $table_style2");
	echo "<tr><td valign=center>"; // outer table
	echo "<table>";

    if ($editable) 
    {
        if (!isset($_POST['supplier_id']) && (get_global_supplier() != reserved_words::get_all()))
        	$_POST['supplier_id'] = get_global_supplier();

    	supplier_list_row(_("Supplier:"), 'supplier_id', $_POST['supplier_id'], false, true);

    	if ($order->supplier_id != $_POST['supplier_id']) 
    	{
    		// supplier has changed
    		// delete all the order items - drastic but necessary because of
    		// change of currency, etc
    		$order->clear_items();
    	}
	} 
	else 
	{
		hidden('supplier_id', $order->supplier_id);
		label_row(_("Supplier:"), $order->supplier_name);
    }

	//if ($editable || ($order->supplier_id != $_POST['supplier_id']))
	get_supplier_details_to_order($order, $_POST['supplier_id']);

	set_global_supplier($_POST['supplier_id']);

	if (!is_company_currency($order->curr_code))
	{
		label_row(_("Supplier Currency:"), $order->curr_code);
		exchange_rate_display($order->curr_code, get_company_currency(),
			$_POST['OrderDate']);
	}

    if ($editable) 
    {
    	ref_row(_("Reference:"), 'ref', references::get_next(systypes::po()));
    } 
    else 
    {
    	hidden('ref', $order->reference);
    	label_row(_("Reference:"), $order->reference);
    }

	echo "</table>";

	echo "</td><td valign=center>"; // outer table

	echo "<table height='5'>";
	// check this out?????????
	//if (!isset($_POST['OrderDate']) || $_POST['OrderDate'] == "")
	//	$_POST['OrderDate'] = $order->orig_order_date;
	//if (!isset($_POST['OrderDate']) || $_POST['OrderDate'] == "")
	//{
	//	$_POST['OrderDate'] = Today();
	//	if (!is_date_in_fiscalyear($_POST['OrderDate']))
	//		$_POST['OrderDate'] = end_fiscalyear();
	//}
	date_row(_("Order Date:"), 'OrderDate', $_POST['OrderDate'], 0, 0, 0);

  	text_row(_("Supplier's Reference:"), 'Requisition', null, 16, 15);

	echo "</table>";

	echo "</td><td valign=center>"; // outer table

  	echo "<table height='5'>";

	echo "<tr><td>" . _("Receive Into:") . "</td>";
	echo "<td>";
    locations_list('StkLocation', null);
    echo "  ";
    submit('GetAddress', _("Get Address"));
	echo "</td></tr>";

    if (!isset($_POST['StkLocation']) || $_POST['StkLocation'] == "" ||
    	isset($_POST['GetAddress']) || !isset($_POST['delivery_address']) || 
    	$_POST['delivery_address'] == "")
    {
    	/*If this is the first time the form loaded set up defaults */

        //$_POST['StkLocation'] = $_SESSION['UserStockLocation'];
        $sql = "SELECT delivery_address, phone FROM ".TB_PREF."locations WHERE loc_code='" . $_POST['StkLocation'] . "'";
        $result = db_query($sql,"could not get location info");

        if (db_num_rows($result) == 1)
        {
    	  	$loc_row = db_fetch($result);
    	  	$_POST['delivery_address'] = $loc_row["delivery_address"];
    	  	$_SESSION['PO']->Location = $_POST['StkLocation'];
    	  	$_SESSION['PO']->delivery_address = $_POST['delivery_address'];

        } 
        else 
        { /*The default location of the user is crook */
    	  	echo "<br>" . _("The default stock location set up for this user is not a currently defined stock location. Your system administrator needs to amend your user record.");
        }
    } 

	textarea_row(_("Deliver to:"), 'delivery_address', $_POST['delivery_address'], 35, 4);
  	echo "</table>";

	echo "</td></tr>";
	end_table(); // outer table
}

//---------------------------------------------------------------------------------------------------

function display_po_items(&$order, $editable=true)
{
	global $table_style;

    display_heading(_("Order Items"));

    start_table("$table_style width=80%");

   	$th = array(_("Item Code"), _("Item Description"), _("Quantity"), _("Unit"),
   		_("Required Delivery Date"), _("Price"), _("Line Total"));
   	table_header($th);	

	$total = 0;
	$k = 0;
   	foreach ($order->line_items as $po_line) 
   	{

		if ($po_line->Deleted == false)
		{
    		$line_total =	$po_line->quantity * $po_line->price;
    		if (!$editable || !isset($_GET['Edit']) || $_GET['Edit'] != $po_line->line_no)
			{
    			alt_table_row_color($k);
        		label_cell($po_line->stock_id);
    			label_cell($po_line->item_description);
                qty_cell($po_line->quantity);
    			label_cell($po_line->units);
                label_cell($po_line->req_del_date);
    			amount_cell($po_line->price);
                amount_cell($line_total);

                if ($editable) 
                {
                    edit_link_cell(SID."Edit=" . $po_line->line_no);
                    delete_link_cell(SID."Delete=" . $po_line->line_no);
                }
        		end_row();
			} 
			else 
			{
				po_item_controls($order, $po_line->stock_id);
			}
    		$total += $line_total;
		}
    }

	if (!isset($_GET['Edit']) && $editable)
		po_item_controls($order);

    $display_total = number_format2($total,user_price_dec());
    label_row(_("Total Excluding Shipping/Tax"), $display_total, "colspan=6 align=right", 
    	"nowrap align=right");

	end_table(1);
}

//---------------------------------------------------------------------------------------------------

function display_po_summary(&$po, $is_self=false, $editable=false)
{
	global $table_style2;
    start_table("$table_style2 width=90%");

    start_row();
    label_cells(_("Reference"), $po->reference, "class='tableheader2'");

    label_cells(_("Supplier"), $po->supplier_name, "class='tableheader2'");

    if (!is_company_currency($po->curr_code))
    	label_cells(_("Order Currency"), $po->curr_code, "class='tableheader2'");

    if (!$is_self)
    {
    	label_cells(_("Purchase Order"), get_trans_view_str(systypes::po(), $po->order_no),
    		"class='tableheader2'");
    }
	end_row();
	start_row();
    label_cells(_("Date"), $po->orig_order_date, "class='tableheader2'");

    if ($editable) 
    {
        if (!isset($_POST['Location']))
        	$_POST['Location'] = $po->Location;
        label_cell(_("Deliver Into Location"), "class='tableheader2'");	
        locations_list_cells(null, 'Location', $_POST['Location']);
    } 
    else 
    {
    	label_cells(_("Deliver Into Location"), get_location_name($po->Location),
    		"class='tableheader2'");
    }

    if ($po->requisition_no != "")
    	label_cells(_("Supplier's Reference"), $po->requisition_no, "class='tableheader2'");
    end_row();

    if (!$editable)
    	label_row(_("Delivery Address"), $po->delivery_address, "class='tableheader2'",
    		"colspan=9");

    if ($po->Comments != "")
    	label_row(_("Order Comments"), $po->Comments, "class='tableheader2'",
    		"colspan=9");
    end_table(1);
}

//--------------------------------------------------------------------------------

function po_item_controls(&$order, $stock_id=null)
{
	start_row();

	if (isset($_GET['Edit']) && $stock_id != null)
	{
		hidden('line_no', $_GET['Edit']);

		if (!isset($_POST['stock_id']))
			$_POST['stock_id'] = $order->line_items[$_GET['Edit']]->stock_id;
		if (!isset($_POST['qty']) || ($_POST['qty'] == ""))
			$_POST['qty'] = $order->line_items[$_GET['Edit']]->quantity;
		if (!isset($_POST['price']) || ($_POST['price'] == ""))
			$_POST['price'] = $order->line_items[$_GET['Edit']]->price;
		if (!isset($_POST['req_del_date']) || ($_POST['req_del_date'] == ""))
			$_POST['req_del_date'] = $order->line_items[$_GET['Edit']]->req_del_date;

		$_POST['units'] = $order->line_items[$_GET['Edit']]->units;

		hidden('stock_id', $_POST['stock_id']);
		label_cell($_POST['stock_id']);
		label_cell($order->line_items[$_GET['Edit']]->item_description);
	} 
	else 
	{

		hidden('line_no', ($_SESSION['PO']->lines_on_order + 1));

		text_cells(null, "StockID2", "", 12, 10, "", "", "onkeyup='recalcAccounts();' onKeyDown='if(event.keyCode==13) event.keyCode=9;' onblur='return setAccount(0, true);'");
		stock_purchasable_items_list_cells(null, 'stock_id', $_POST['stock_id'], false, false, "onchange='return setAccount(1, true)'");

    	$item_info = get_item_edit_info($_POST['stock_id']);
		$_POST['units'] = $item_info["units"];

   		$_POST['qty'] = 1;
		$_POST['price'] = get_purchase_price ($order->supplier_id, $_POST['stock_id']);
		$_POST['req_del_date'] = add_days(Today(), 10);
	}

	text_cells(null, 'qty', null, 13, 15);

	label_cell($_POST['units']);
	date_cells(null, 'req_del_date', null, 0, 0, 0);
	text_cells(null, 'price', null, 15, 14);

	//$line_total = $_POST['qty'] * $_POST['price'] * (1 - $_POST['Disc'] / 100);
	$line_total = $_POST['qty'] * $_POST['price'];
	amount_cell($line_total);

	if (isset($_GET['Edit'])) 
	{
    	submit_cells('UpdateLine', _("Update"));
    	submit_cells('CancelUpdate', _("Cancel"));
	} 
	else 
	{
		submit_cells('EnterLine', _("Add Item"), "colspan=2");
	}

	end_row();
}

//---------------------------------------------------------------------------------------------------



?>