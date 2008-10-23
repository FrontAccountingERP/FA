<?php

//--------------------------------------------------------------------------------------------------

function copy_from_trans(&$supp_trans)
{
	$_POST['Comments'] = $supp_trans->Comments;
	$_POST['tran_date'] = $supp_trans->tran_date;
	$_POST['due_date'] = $supp_trans->due_date;
	$_POST['supp_reference'] = $supp_trans->supp_reference;
	$_POST['reference'] = $supp_trans->reference;
	$_POST['supplier_id'] = $supp_trans->supplier_id;
}

//--------------------------------------------------------------------------------------------------

function copy_to_trans(&$supp_trans)
{
	$supp_trans->Comments = $_POST['Comments'];
	$supp_trans->tran_date = $_POST['tran_date'];
	$supp_trans->due_date = $_POST['due_date'];
	$supp_trans->supp_reference = $_POST['supp_reference'];
	$supp_trans->reference = $_POST['reference'];

	$supp_trans->ov_amount = 0; /* for starters */
	if (count($supp_trans->grn_items) > 0)
	{
		foreach ( $supp_trans->grn_items as $grn)
		{
			$supp_trans->ov_amount += round(($grn->this_quantity_inv * $grn->chg_price),
			  user_price_dec());
		}
	}
	if (count($supp_trans->gl_codes) > 0)
	{
		foreach ( $supp_trans->gl_codes as $gl_line)
		{
			$supp_trans->ov_amount += $gl_line->amount;
		}
	}
}

//--------------------------------------------------------------------------------------------------

function invoice_header(&$supp_trans)
{
	global $Ajax;
	
	// if vars have been lost, recopy
	if (!isset($_POST['tran_date']))
		copy_from_trans($supp_trans);

	start_table("width=100%", 5);
	start_row();
	echo"<td>"; // outer

	echo "<table width=100%>";

    if (!isset($_POST['supplier_id']) && (get_global_supplier() != reserved_words::get_all()))
    	$_POST['supplier_id'] = get_global_supplier();

	supplier_list_row(_("Supplier:"), 'supplier_id', $_POST['supplier_id'], false, true);

	if ($supp_trans->supplier_id != $_POST['supplier_id'])
	{
		// supplier has changed
		// delete all the order items - drastic but necessary because of
		// change of currency, etc
		$supp_trans->clear_items();
		read_supplier_details_to_trans($supp_trans, $_POST['supplier_id']);
		copy_from_trans($supp_trans);
	}

	if ($supp_trans->is_invoice)
    	ref_row(_("Reference:"), 'reference', '', references::get_next(20));
    else
    	ref_row(_("Reference:"), 'reference', '', references::get_next(21));

   	text_row(_("Supplier's Ref.:"), 'supp_reference', $_POST['supp_reference'], 20, 20);

	echo "</table>";

	echo "</td><td class='tableseparator'>"; // outer

	echo "<table width=100%>";

   	date_row(_("Date") . ":", 'tran_date', '', null, 0, 0, 0, "", true);
	if (isset($_POST['_tran_date_changed'])) {
		$Ajax->activate('_ex_rate');
		$supp_trans->tran_date = $_POST['tran_date'];
		get_duedate_from_terms($supp_trans);
		$_POST['due_date'] = $supp_trans->due_date;
		$Ajax->activate('due_date');
	}

    date_row(_("Due Date") . ":", 'due_date');

    label_row(_("Terms:"), $supp_trans->terms_description);

	echo "</table>";

	echo "</td><td class='tableseparator'>"; // outer

	echo "<table width=100%>";

	$supplier_currency = get_supplier_currency($supp_trans->supplier_id);

	$company_currency = get_company_currency();

	if ($supplier_currency != $company_currency)
	{
        label_row(_("Supplier's Currency:"), "<b>" . $supplier_currency . "</b>");
		exchange_rate_display($supplier_currency, $company_currency, $_POST['tran_date']);
	}

  label_row(_("Tax Group:"), $supp_trans->tax_description);
	echo "</table>";

	echo "</td>";
	end_row();
	end_table(); // outer
}

//--------------------------------------------------------------------------------------------------

function invoice_totals(&$supp_trans)
{
	global $table_style, $table_style2;

	copy_to_trans($supp_trans);

    start_table("$table_style width=95%");
   	label_row(_("Sub-total:"), price_format( $supp_trans->ov_amount), "align=right", "align=right");

    $taxes = $supp_trans->get_taxes($supp_trans->tax_group_id);
    $tax_total = display_edit_tax_items($taxes, 1, 0); // tax_included==0 (we are the company)

    $display_total = price_format($supp_trans->ov_amount + $tax_total);

	if ($supp_trans->is_invoice)
    	label_row(_("Invoice Total:"), $display_total, "align=right", "align=right");
    else
		label_row("<font color=red>" . _("Credit Note Total:") . "</font>",
			"<font color=red><b>$display_total</b></font>", "align=right", "nowrap align=right");

    end_table();
	br(1);
    start_table($table_style2);
    textarea_row(_("Memo:"), "Comments", null, 50, 3);
    end_table();
    br(1);
}

//--------------------------------------------------------------------------------------------------
function display_gl_controls(&$supp_trans)
{
	global $table_style, $Ajax;

	div_start('gl_ctrls'); // 2008-10-18 Joe Hunt Moved up a bit to compute num-rows = 0

	$accs = get_supplier_accounts($supp_trans->supplier_id);
	$_POST['gl_code'] = $accs['purchase_account'];
	$k = 0;
	alt_table_row_color($k);
	gl_all_accounts_list('gl_code', null, true, false, true);
	$dim = get_company_pref('use_dimension');
	if ($dim >= 1)
		dimensions_list_cells(null, 'dimension_id', null, true, " ", false, 1);
	if ($dim > 1)
		dimensions_list_cells(null, 'dimension2_id', null, true, " ", false, 2);
	amount_cells(null, 'amount');
	if ($dim < 1)	
		text_cells_ex(null, 'memo_', 35, 50, null, null, null, hidden('dimension_id', 0, false).hidden('dimension2_id', 0, false));
	else if ($dim < 2)	
		text_cells_ex(null, 'memo_', 35, 50, null, null, null, hidden('dimension2_id', 0, false));
	else	
		text_cells_ex(null, 'memo_', 35, 50, null, null, null);
	submit_cells('AddGLCodeToTrans', _("Add"), "",
		    _('Add GL Line'), true);
	submit_cells('ClearFields', _("Reset"), "",
		    _("Clear all GL entry fields"), true);
	end_row();	
	div_end();
}

// $mode = 0 none at the moment
//		 = 1 display on invoice/credit page
//		 = 2 display on view invoice
//		 = 3 display on view credit

function display_gl_items(&$supp_trans, $mode=0)
{
	global $table_style, $path_to_root;

    // if displaying in form, and no items, exit
    if (($mode == 2 || $mode == 3) && count($supp_trans->gl_codes) == 0)
    	return 0;

	echo "<br>";

	start_table("width=95%");
	echo "<tr><td>"; // outertable

	if ($supp_trans->is_invoice)
		$heading = _("GL Items for this Invoice");
	else
		$heading = _("GL Items for this Credit Note");

	echo "<table width=100%>";
	echo "<tr><td align=center><span class='headingtext'>$heading</span></td>";
	echo "</tr></table>";

	echo "</td></tr><tr><td>"; // ouer table

	div_start('gl_items');
	echo "<table width=100% $table_style>";

	$dim = get_company_pref('use_dimension');
	if ($dim == 2)
    	$th = array(_("Account"), _("Name"), _("Dimension")." 1", _("Dimension")." 2", _("Amount"), _("Memo"));
	else if ($dim == 1)
    	$th = array(_("Account"), _("Name"), _("Dimension"), _("Amount"), _("Memo"));
    else
    	$th = array(_("Account"), _("Name"), _("Amount"), _("Memo"));

	if ($mode == 1)
	{
		$th[] = "";
		$th[] = "";
	}	
	table_header($th);
	$total_gl_value=0;
	$i = $k = 0;

	if (count($supp_trans->gl_codes) > 0)
	{

		foreach ($supp_trans->gl_codes as $entered_gl_code)
		{

			alt_table_row_color($k);

			if ($mode == 3)
				$entered_gl_code->amount = -$entered_gl_code->amount;

			label_cell($entered_gl_code->gl_code);
			label_cell($entered_gl_code->gl_act_name);

			if ($dim >= 1)
   				label_cell(get_dimension_string($entered_gl_code->gl_dim, true));
			if ($dim > 1)
   				label_cell(get_dimension_string($entered_gl_code->gl_dim2, true));

			amount_cell($entered_gl_code->amount);
			label_cell($entered_gl_code->memo_);

			if ($mode == 1)
			{
				edit_button_cell("Delete2" . $entered_gl_code->Counter, _("Delete"),
					  _('Remove line from document'));
				label_cell("");
			}	
			end_row();

			$total_gl_value += $entered_gl_code->amount;

			$i++;
			if ($i > 15)
			{
				$i = 0;
				table_header($th);
			}
		}

	}
	if ($mode == 1)
		display_gl_controls($supp_trans);
	$colspan = ($dim == 2 ? 4 : ($dim == 1 ? 3 : 2));
	label_row(_("Total"), price_format($total_gl_value),
		"colspan=".$colspan." align=right", "nowrap align=right");

	echo "</table>";
	div_end();

    echo "</td></tr>";

    end_table(); // outertable
	return $total_gl_value;
}

//--------------//-----------------------------------------------------------------------------------------

function display_grn_items_for_selection(&$supp_trans)
{
	global $table_style;

	div_start('grn_table'); // 2008-10-18 Joe Hunt Moved up a bit to compute num-rows = 0
	if ($supp_trans->is_invoice)
		$result = get_grn_items(0, $supp_trans->supplier_id, true);
	else	
		$result = get_grn_items(0, $supp_trans->supplier_id, false, true);

    if (db_num_rows($result) == 0)
    {
		if ($supp_trans->is_invoice)
    		display_note(_("There are no outstanding items received from this supplier that have not been invoiced by them."), 0, 1);
    	else
    	{
			display_note(_("There are no received items for the selected supplier that have been invoiced."));
			display_note(_("Credits can only be applied to invoiced items."), 0, 1);
    	}
		div_end(); // Changed 2008-10-18 Joe Hunt
    	return;
    }
    
    /*Set up a table to show the outstanding GRN items for selection */

    $k = 0;

    while ($myrow = db_fetch($result))
    {
		$grn_already_on_invoice = false;

    	foreach ($supp_trans->grn_items as $entered_grn)
    	{
    		if ($entered_grn->id == $myrow["id"])
    		{
    			$grn_already_on_invoice = true;
    		}
    	}
    	if ($grn_already_on_invoice == false)
    	{

			alt_table_row_color($k);

			$n = $myrow["id"];
    		label_cell(get_trans_view_str(25, $myrow["grn_batch_id"]));
        	label_cell($myrow["id"].
            	hidden('qty_recd'.$n, $myrow["qty_recd"], false).
            	hidden('item_code'.$n, $myrow["item_code"], false).
            	hidden('item_description'.$n, $myrow["description"], false).
            	hidden('prev_quantity_inv'.$n, $myrow['quantity_inv'], false).
            	hidden('order_price'.$n, $myrow['unit_price'], false).
            	hidden('std_cost_unit'.$n, $myrow['std_cost_unit'], false).
            	hidden('po_detail_item'.$n, $myrow['po_detail_item'], false));
        	label_cell(get_trans_view_str(systypes::po(), $myrow["purch_order_no"]));
            label_cell($myrow["item_code"]);
            label_cell($myrow["description"]);
            label_cell(sql2date($myrow["delivery_date"]));
            $dec = get_qty_dec($myrow["item_code"]);
            qty_cell($myrow["qty_recd"], false, $dec);
            qty_cell($myrow["quantity_inv"], false, $dec);
            if ($supp_trans->is_invoice)
            	qty_cells(null, 'this_quantity_inv'.$n, number_format2($myrow["qty_recd"] - $myrow["quantity_inv"], $dec), 
            		null, null, $dec);
            else		
            	qty_cells(null, 'This_QuantityCredited'.$n, number_format2(max($myrow["quantity_inv"], 0), $dec), 
            		null, null, $dec);
            amount_cells(null, 'ChgPrice'.$n, price_format($myrow["unit_price"]));
            if ($supp_trans->is_invoice)
            	amount_cell(round($myrow["unit_price"] * ($myrow["qty_recd"] - $myrow["quantity_inv"]), user_price_dec()));
            else	
            	amount_cell(round($myrow["unit_price"] * max($myrow['quantity_inv'], 0), user_price_dec()));
            if ($supp_trans->is_invoice)	
        		submit_cells('grn_item_id'.$n, _("Add"), '', _("Add to Invoice"), true);
        	else	
        		submit_cells('grn_item_id'.$n, _("Add"), '', _("Add to Credit Note"), true);
    		if ($supp_trans->is_invoice && $_SESSION["wa_current_user"]->access == 2)	// Added 2008-10-18 by Joe Hunt. Only admins can remove GRNs
        		submit_cells('void_item_id'.$n, _("Remove"), '', _("WARNING! Be careful with removal. The operation is executed immediately and cannot be undone !!!"), true);
			end_row();
    	}
    }

	div_end();
}

//------------------------------------------------------------------------------------

// $mode = 0 none at the moment
//		 = 1 display on invoice/credit page
//		 = 2 display on view invoice
//		 = 3 display on view credit

function display_grn_items(&$supp_trans, $mode=0)
{
	global $table_style, $path_to_root;

    // if displaying in form, and no items, exit
    if (($mode == 2  || $mode == 3) && count($supp_trans->grn_items) == 0)
    	return 0;
	br(1);
	start_table("width=95%");
	echo "<tr><td>"; // outertable
	$heading2 = "";
	if ($mode == 1)
	{
		if ($supp_trans->is_invoice)
		{
			$heading = _("Items Received Yet to be Invoiced");
    		if ($_SESSION["wa_current_user"]->access == 2)	// Added 2008-10-18 by Joe Hunt. Only admins can remove GRNs
				$heading2 = _("WARNING! Be careful with removal. The operation is executed immediately and cannot be undone !!!");
		}
		else
			$heading = _("Delivery Item Selected For Adding To A Supplier Credit Note");
	}
	else
	{
		if ($supp_trans->is_invoice)
			$heading = _("Received Items Charged on this Invoice");
		else
			$heading = _("Received Items Credited on this Note");
	}		
	echo "<table width=100% >";
	echo "<tr><td align=center><span class='headingtext'>$heading</span></td>";
	if ($mode == 1 && $heading2 != "")
	{
		echo "</tr><td>";
		display_note($heading2, 0, 0, "class='overduefg'");
		echo "</td>\n";
	}	
	echo "</tr></table>";

	echo "</td></tr><tr><td>"; // outer table

  	div_start('grn_items');
	echo "<table width=100% $table_style>";
	if ($mode == 1)
	{
    	$th = array(_("Delivery"), _("Sequence #"), _("P.O."), _("Item"), _("Description"),
    		_("Received On"), _("Quantity Received"), _("Quantity Invoiced"),
    		_("Qty Yet To Invoice"), _("Order Price"), _("Total"), "");
    	if ($supp_trans->is_invoice && $_SESSION["wa_current_user"]->access == 2)	// Added 2008-10-18 by Joe Hunt. Only admins can remove GRNs
    		$th[] = "";
		if (!$supp_trans->is_invoice)
		{
			$th[8] = _("Qty Yet To Credit");
		}
    }
    else
		$th = array(_("Delivery"), _("Item"), _("Description"),
			_("Quantity"), _("Price"), _("Line Value"));

	table_header($th);
    $total_grn_value = 0;
    $i = $k = 0;

	if (count($supp_trans->grn_items) > 0)
	{

    	foreach ($supp_trans->grn_items as $entered_grn)
    	{

    		alt_table_row_color($k);

			$grn_batch = get_grn_batch_from_item($entered_grn->id);
    		label_cell(get_trans_view_str(25,$grn_batch));
    		if ($mode == 1)
    		{
				label_cell($entered_grn->id);
				label_cell(""); // PO
			}	
			label_cell($entered_grn->item_code);
			label_cell($entered_grn->item_description);
            $dec = get_qty_dec($entered_grn->item_code);
            if ($mode == 1)
            {
            	label_cell("");
				qty_cell($entered_grn->qty_recd, false, $dec);
				qty_cell($entered_grn->prev_quantity_inv, false, $dec);
            }
			qty_cell(abs($entered_grn->this_quantity_inv), false, $dec);
			amount_cell($entered_grn->chg_price);
			amount_cell( round($entered_grn->chg_price * abs($entered_grn->this_quantity_inv)), user_price_dec());

			if ($mode == 1)
			{
				edit_button_cell("Delete" . $entered_grn->id, _("Delete"),
					  _('Remove item from document'));
				if ($supp_trans->is_invoice && $_SESSION["wa_current_user"]->access == 2)	  
					label_cell("");
			}	
			end_row();

    		$total_grn_value += round($entered_grn->chg_price * abs($entered_grn->this_quantity_inv),
			   user_price_dec());

    		$i++;
    		if ($i > 15)
    		{
    		 	$i = 0;
    		 	table_header($th);
    		}
    	}
    }
	if ($mode == 1)
	{
		display_grn_items_for_selection($supp_trans);
		$colspan = 10;
	}
	else
		$colspan = 5;
	label_row(_("Total"), price_format($total_grn_value),
		"colspan=$colspan align=right", "nowrap align=right");
    echo "</table>";
    div_end();
    echo "</td></tr>";

    end_table(); // outertable
	return $total_grn_value;
}

//--------------------------------------------------------------------------------------------------
function get_duedate_from_terms(&$supp_trans)
{
	if (!is_date($supp_trans->tran_date))
	{
		$supp_trans->tran_date = Today();
	}
	if (substr( $supp_trans->terms, 0, 1) == "1")
	{ /*Its a day in the following month when due */
		$supp_trans->due_date = add_days(end_month($supp_trans->tran_date), (int) substr( $supp_trans->terms,1));
	}
	else
	{ /*Use the Days Before Due to add to the invoice date */
		$supp_trans->due_date = add_days($supp_trans->tran_date, (int) substr( $supp_trans->terms,1));
	}
}

//--------------------------------------------------------------------------------------------------

?>