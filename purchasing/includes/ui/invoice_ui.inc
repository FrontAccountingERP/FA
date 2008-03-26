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
    	ref_row(_("Reference:"), 'reference', references::get_next(20));
    else
    	ref_row(_("Reference:"), 'reference', references::get_next(21));

   	text_row(_("Supplier's Ref.:"), 'supp_reference', $_POST['supp_reference'], 20, 20);

	echo "</table>";

	echo "</td><td class='tableseparator'>"; // outer

	echo "<table width=100%>";

   	date_row(_("Date") . ":", 'tran_date', null, 0, 0, 0, "", true);

    date_row(_("Due Date") . ":", 'due_date', null);

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

// $mode = 0 display on invoice page
//		 = 1 display on gl items edit page
//		 = 2 display on view invoice
//		 = 3 display on view credit

function display_gl_items(&$supp_trans, $mode=0)
{
	global $table_style, $path_to_root;

    if ($mode == 0 && isset($_POST["InvGL"]))
    {
    	copy_to_trans($supp_trans);
		meta_forward("$path_to_root/purchasing/supplier_trans_gl.php");
    	exit;
    }

    // if displaying in form, and no items, exit
    if (($mode == 2 || $mode == 3) && count($supp_trans->gl_codes) == 0)
    	return;

	echo "<br>";

	start_table("width=95%");
	echo "<tr><td>"; // outertable

	if ($supp_trans->is_invoice)
		$heading = _("GL Items for this Invoice");
	else
		$heading = _("GL Items for this Credit Note");

	echo "<table width=100%>";
	echo "<tr><td align=center><span class='headingtext'>$heading</span></td>";
	if ($mode == 0) 
	{
    	submit_cells('InvGL', _("Edit"), "align=right");
	}
	echo "</tr></table>";

	echo "</td></tr><tr><td>"; // ouer table

	echo "<table width=100% $table_style>";
	
	$dim = get_company_pref('use_dimension');
	if ($dim == 2)
    	$th = array(_("Account"), _("Name"), _("Dimension")." 1", _("Dimension")." 2", _("Amount"), _("Memo"));
	else if ($dim == 1)
    	$th = array(_("Account"), _("Name"), _("Dimension"), _("Amount"), _("Memo"));
    else
    	$th = array(_("Account"), _("Name"), _("Amount"), _("Memo"));

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
				delete_link_cell(SID . "Delete=" . $entered_gl_code->Counter);

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
	$colspan = ($dim == 2 ? 4 : ($dim == 1 ? 3 : 2));
	label_row(_("Total"), price_format($total_gl_value), 
		"colspan=".$colspan." align=right", "nowrap align=right");

	echo "</table>";

    echo "</td></tr>";
    
    end_table(); // outertable

	return $total_gl_value;
}

//--------------------------------------------------------------------------------------------------

// $mode = 0 display on invoice page
//		 = 1 display on grn items edit page
//		 = 2 display on view invoice
//		 = 3 display on view credit

function display_grn_items(&$supp_trans, $mode=0)
{
	global $table_style, $path_to_root;

    if ($mode == 0 && isset($_POST["InvGRN"]))
    {
    	copy_to_trans($supp_trans);
		if ($supp_trans->is_invoice)
			meta_forward("$path_to_root/purchasing/supplier_invoice_grns.php");
    	else
			meta_forward("$path_to_root/purchasing/supplier_credit_grns.php");
    	exit;
    }

    // if displaying in form, and no items, exit
    if (($mode == 2  || $mode == 3) && count($supp_trans->grn_items) == 0)
    	return;
	br(1);
	start_table("width=95%");
	echo "<tr><td>"; // outertable

	if ($supp_trans->is_invoice)
		$heading = _("Received Items Charged on this Invoice");
	else
		$heading = _("Received Items Credited on this Note");

	echo "<table width=100% >";
	echo "<tr><td align=center><span class='headingtext'>$heading</span></td>";
	if ($mode == 0) 
	{
		submit_cells('InvGRN', _("Edit"), "align=right");
	}
	echo "</tr></table>";

	echo "</td></tr><tr><td>"; // outer table

	echo "<table width=100% $table_style>";
	$th = array(_("Delivery Sequence #"), _("Item"), _("Description"),
		_("Quantity"), _("Price"), _("Line Value"));

	table_header($th);
    $total_grn_value = 0;
    $i = $k = 0;

	if (count($supp_trans->grn_items) > 0)
	{

    	foreach ($supp_trans->grn_items as $entered_grn)
    	{

    		alt_table_row_color($k);

    		label_cell(get_trans_view_str(25,$entered_grn->id));
			label_cell($entered_grn->item_code);
			label_cell($entered_grn->item_description);
			qty_cell(abs($entered_grn->this_quantity_inv));
			amount_cell($entered_grn->chg_price);
			amount_cell( round($entered_grn->chg_price * abs($entered_grn->this_quantity_inv)), user_price_dec());

			if ($mode == 1)
				delete_link_cell(SID . "Delete=" . $entered_grn->id);

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

	label_row(_("Total"), price_format($total_grn_value),
		"colspan=5 align=right", "nowrap align=right");
    echo "</table>";

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