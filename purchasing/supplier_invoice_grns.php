<?php

$page_security = 5;
$path_to_root="..";

include_once($path_to_root . "/purchasing/includes/supp_trans_class.inc");
include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/purchasing/includes/purchasing_ui.inc");
include_once($path_to_root . "/purchasing/includes/purchasing_db.inc");

$js = "";
if ($use_date_picker)
	$js .= get_js_date_picker();
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
page(_("Select Received Items to Add"), false, false, "", $js);

if (!isset($_SESSION['supp_trans']))
{
	display_note("To enter supplier transactions the supplier must first be selected from the supplier selection screen, then the link to enter a supplier credit note must be clicked on.", 1, 0);;
	exit;
}

//-----------------------------------------------------------------------------------------

display_heading($_SESSION['supp_trans']->supplier_name);

echo "<br>";

//-----------------------------------------------------------------------------------------

function check_data()
{
	global $check_price_charged_vs_order_price,
		$check_qty_charged_vs_del_qty;
	if (!check_num('this_quantity_inv', 0) || input_num('this_quantity_inv')==0)
	{
		display_error( _("The quantity to invoice must be numeric and greater than zero."));
		set_focus('this_quantity_inv');
		return false;
	}

	if (!check_num('ChgPrice'))
	{
		display_error( _("The price is not numeric."));
		set_focus('ChgPrice');
		return false;
	}

	if ($check_price_charged_vs_order_price == True)
	{
		if ($_POST['order_price']!=input_num('ChgPrice')) {
		     if ($_POST['order_price']==0 ||
			input_num('ChgPrice')/$_POST['order_price'] >
			    (1 + (sys_prefs::over_charge_allowance() / 100)))
		    {
			display_error(_("The price being invoiced is more than the purchase order price by more than the allowed over-charge percentage. The system is set up to prohibit this. See the system administrator to modify the set up parameters if necessary.") .
			_("The over-charge percentage allowance is :") . sys_prefs::over_charge_allowance() . "%");
			set_focus('ChgPrice');
			return false;
		    }
		}
	}

	if ($check_qty_charged_vs_del_qty == True)
	{
		if (input_num('this_quantity_inv') / ($_POST['qty_recd'] - $_POST['prev_quantity_inv']) >
			(1+ (sys_prefs::over_charge_allowance() / 100)))
		{
			display_error( _("The quantity being invoiced is more than the outstanding quantity by more than the allowed over-charge percentage. The system is set up to prohibit this. See the system administrator to modify the set up parameters if necessary.")
			. _("The over-charge percentage allowance is :") . sys_prefs::over_charge_allowance() . "%");
			set_focus('this_quantity_inv');
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------------------

if (isset($_POST['AddGRNToTrans']))
{

	if (check_data())
	{
    	if (input_num('this_quantity_inv') >= ($_POST['qty_recd'] - $_POST['prev_quantity_inv']))
    	{
    		$complete = true;
    	}
    	else
    	{
    		$complete = false;
    	}

		$_SESSION['supp_trans']->add_grn_to_trans($_POST['GRNNumber'], $_POST['po_detail_item'],
			$_POST['item_code'], $_POST['item_description'], $_POST['qty_recd'],
			$_POST['prev_quantity_inv'], input_num('this_quantity_inv'),
			$_POST['order_price'], input_num('ChgPrice'), $complete,
			$_POST['std_cost_unit'], "");
	}
}

//-----------------------------------------------------------------------------------------
$id = find_submit('Delete');
if ($id != -1)
{
	$_SESSION['supp_trans']->remove_grn_from_trans($id);
	$Ajax->activate('grn_items');
	$Ajax->activate('grn_table');
}

//-----------------------------------------------------------------------------------------
start_form(false,true);
display_grn_items($_SESSION['supp_trans'], 1);
end_form();
echo "<br>";

hyperlink_no_params("$path_to_root/purchasing/supplier_invoice.php", _("Back to Supplier Invoice Entry"));
echo "<hr>";

//-----------------------------------------------------------------------------------------
start_form(false, true); // 2008-10-18 Joe Hunt. Moved form outside function

function display_grn_items_for_selection()
{
	global $table_style;

	div_start('grn_table'); // 2008-10-18 Joe Hunt Moved up a bit to compute num-rows = 0
	$result = get_grn_items(0, $_SESSION['supp_trans']->supplier_id, true);

    if (db_num_rows($result) == 0)
    {
    	display_note(_("There are no outstanding items received from this supplier that have not been invoiced by them."), 0, 1);
		div_end(); // Changed 2008-10-18 Joe Hunt
    	return;
    }
    
    /*Set up a table to show the outstanding GRN items for selection */

    display_heading2(_("Items Received Yet to be Invoiced"));
    if ($_SESSION["wa_current_user"]->access == 2)	// Added 2008-10-18 by Joe Hunt. Only admins can remove GRNs
    	display_note(_("WARNING! Be careful with removal. The operation is executed immediately and cannot be undone !!!"), 0, 0, "class='overduefg'");
    //function display_note($msg, $br=0, $br2=0, $extra="")	
	//div_start('grn_table');
    start_table("$table_style colspan=7 width=95%");
    $th = array(_("Delivery"), _("Sequence #"), _("P.O."), _("Item"), _("Description"),
    	_("Received On"), _("Quantity Received"), _("Quantity Invoiced"),
    	_("Uninvoiced Quantity"), _("Order Price"), _("Total"));
    if ($_SESSION["wa_current_user"]->access == 2)	// Added 2008-10-18 by Joe Hunt. Only admins can remove GRNs
    	$th[] = "";	
    table_header($th);
    $i = $k = 0;

    while ($myrow = db_fetch($result))
    {
		$grn_already_on_invoice = false;

    	foreach ($_SESSION['supp_trans']->grn_items as $entered_grn)
    	{
    		if ($entered_grn->id == $myrow["id"])
    		{
    			$grn_already_on_invoice = true;
    		}
    	}
    	if ($grn_already_on_invoice == false)
    	{

			alt_table_row_color($k);

    		label_cell(get_trans_view_str(25, $myrow["grn_batch_id"]));
        	//text_cells(null, 'grn_item_id', $myrow["id"]);
        	submit_cells('grn_item_id'.$myrow["id"], $myrow["id"], '', '', true);
        	label_cell(get_trans_view_str(systypes::po(), $myrow["purch_order_no"]));
            label_cell($myrow["item_code"]);
            label_cell($myrow["description"]);
            label_cell(sql2date($myrow["delivery_date"]));
            $dec = get_qty_dec($myrow["item_code"]);
            qty_cell($myrow["qty_recd"], false, $dec);
            qty_cell($myrow["quantity_inv"], false, $dec);
            qty_cell($myrow["qty_recd"] - $myrow["quantity_inv"], false, $dec);
            amount_cell($myrow["unit_price"]);
            amount_cell(round($myrow["unit_price"] * ($myrow["qty_recd"] - $myrow["quantity_inv"]),
			   user_price_dec()));
    		if ($_SESSION["wa_current_user"]->access == 2)	// Added 2008-10-18 by Joe Hunt. Only admins can remove GRNs
        		submit_cells('void_item_id'.$myrow["id"], _("Remove"), '', '', true);
			end_row();

    		$i++;
    		if ($i > 15)
    		{
    			$i = 0;
    			table_header($th);
    		}
    	}
    }

    end_table();
	div_end();
}

//-----------------------------------------------------------------------------------------
$id = find_submit('grn_item_id');
$id2 = find_submit('void_item_id');
if ($id != -1 || id2 != -1 || get_post('AddGRNToTrans'))
{
	$Ajax->activate('grn_selector');
}
if (get_post('AddGRNToTrans') || $id2 != -1)
{
	$Ajax->activate('grn_table');
	$Ajax->activate('grn_items');
}

if ($_SESSION["wa_current_user"]->access == 2)
{
	if ($id2 != -1) // Added section 2008-10-18 Joe Hunt for voiding delivery lines
	{
		begin_transaction();
		
		$myrow = get_grn_item_detail($id2);

		$grn = get_grn_batch($myrow['grn_batch_id']);

	    $sql = "UPDATE ".TB_PREF."purch_order_details
			SET quantity_received = qty_invoiced, quantity_ordered = qty_invoiced WHERE po_detail_item = ".$myrow["po_detail_item"];
	    db_query($sql, "The quantity invoiced of the purchase order line could not be updated");

	    $sql = "UPDATE ".TB_PREF."grn_items
	    	SET qty_recd = quantity_inv WHERE id = ".$myrow["id"];
		db_query($sql, "The quantity invoiced off the items received record could not be updated");
	
		update_average_material_cost($grn["supplier_id"], $myrow["item_code"],
			$myrow["unit_price"], -$myrow["QtyOstdg"], Today());

	   	add_stock_move(25, $myrow["item_code"], $myrow['grn_batch_id'], $grn['loc_code'], sql2date($grn["delivery_date"]), "",
	   		-$myrow["QtyOstdg"], $myrow['std_cost_unit'], $grn["supplier_id"], 1, $myrow['unit_price']);
	   		
	   	commit_transaction();
	}   		
}

display_grn_items_for_selection();

//-----------------------------------------------------------------------------------------

div_start('grn_selector');

//$id = find_submit('grn_item_id');
if ($id != -1)
{

	$myrow = get_grn_item_detail($id);

	echo "<br>";
	display_heading2(_("Delivery Item Selected For Adding To A Supplier Invoice"));
	start_table("$table_style width=80%");
	$th = array(_("Sequence #"), _("Item"), _("Description"), _("Quantity Outstanding"),
		_("Quantity to Invoice"), _("Order Price"), _("Actual Price"));
	table_header($th);

	start_row();
	label_cell($id);
	label_cell($myrow['item_code']);
	label_cell($myrow['description']);
	$dec = get_qty_dec($myrow['item_code']);
	qty_cell($myrow['QtyOstdg'], false, $dec);
	qty_cells(null, 'this_quantity_inv', number_format2($myrow['QtyOstdg'], $dec), null, null, $dec);
	amount_cell($myrow['unit_price']);
	small_amount_cells(null, 'ChgPrice', price_format($myrow['unit_price']));
	end_row();
	end_table(1);;

	submit_center('AddGRNToTrans', _("Add to Invoice"), true, '', true);

	hidden('GRNNumber', $id);
	hidden('item_code', $myrow['item_code']);
	hidden('item_description', $myrow['description']);;
	hidden('qty_recd', $myrow['qty_recd']);
	hidden('prev_quantity_inv', $myrow['quantity_inv']);
	hidden('order_price', $myrow['unit_price']);
	hidden('std_cost_unit', $myrow['std_cost_unit']);

	hidden('po_detail_item', $myrow['po_detail_item']);
}
div_end();

//----------------------------------------------------------------------------------------

end_form();
echo '<br>';
end_page(false, true);

?>
