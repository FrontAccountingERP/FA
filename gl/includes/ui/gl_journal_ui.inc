<?php

include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/ui/items_cart.inc");

//--------------------------------------------------------------------------------

function display_order_header(&$Order)
{
	global $table_style2;

	$qes = has_quick_entries(-1, 0);
	start_table("$table_style2 width=90%");
	if ($qes)
		echo "<tr><td valign=top width=30%>"; // outer table
	else	
		echo "<tr><td valign=top width=50%>"; // outer table

	echo "<table>";
	start_row();
    date_cells(_("Date:"), 'date_');
	ref_cells(_("Reference:"), 'ref', '', references::get_next(0));
	end_row();

	echo "</table>";

	if ($qes)
		echo "</td><td width=20%>"; // outer table
	else	
		echo "</td><td width=50%>"; // outer table

	echo "<table>";
	start_row();
	check_cells(_("Reverse Transaction:"), 'Reverse', null);
	end_row();
	echo "</table>";
	if ($qes !== false)
	{
		echo "</td><td width=50%>"; // outer table
		echo "<table>";
		start_row();
		quick_entries_list_cells(_("Quick Entry:"), 'person_id', null, -1, 0);
		amount_cells(_("Total Amount"), 'totamount', null, null, "&nbsp;&nbsp;".submit('go', _("Go"), false, false, true));			
		end_row();
		echo "</table>";
	}
		

	echo "</td></tr>"; // outer table
	end_table(1);
}

//---------------------------------------------------------------------------------

function display_gl_items($title, &$order)
{
	global $table_style, $path_to_root;

	display_heading($title);

	$dim = get_company_pref('use_dimension');

    div_start('items_table');
	start_table("$table_style colspan=7 width=95%");
	if ($dim == 2)
		$th = array(_("Account Code"), _("Account Description"), _("Dimension")." 1",
			_("Dimension")." 2", _("Debit"), _("Credit"), _("Memo"));
	else if ($dim == 1)
		$th = array(_("Account Code"), _("Account Description"), _("Dimension"),
			_("Debit"), _("Credit"), _("Memo"));
	else
		$th = array(_("Account Code"), _("Account Description"),
			_("Debit"), _("Credit"), _("Memo"));

	if (count($order->gl_items)) $th[] = '';

	table_header($th);	

	$k = 0;

	$id = find_submit('Edit');
	foreach ($order->gl_items as $item) 
	{
		if ($id != $item->index)
		{
    		alt_table_row_color($k);

			label_cells($item->code_id, $item->description);
    		if ($dim >= 1)
   				label_cell(get_dimension_string($item->dimension_id, true));
    		if ($dim > 1)
   				label_cell(get_dimension_string($item->dimension2_id, true));
    		if ($item->amount > 0)
    		{
    			amount_cell(abs($item->amount));
    			label_cell("");
    		}	
    		else
    		{
    			label_cell("");
    			amount_cell(abs($item->amount));
    		}	
			label_cell($item->reference);

			edit_button_cell("Edit$item->index", _("Edit"),
				_('Edit journal line'));
			edit_button_cell("Delete$item->index", _("Delete"),
				_('Remove line from journal'));
    		end_row();
		} 
		else 
		{
			gl_edit_item_controls($order, $dim, $item->index);
		}
	}

	if ($id == -1)
		gl_edit_item_controls($order, $dim);

	if ($order->count_gl_items()) 
	{
		$colspan = ($dim == 2 ? "4" : ($dim == 1 ? "3" : "2"));
		start_row();
		label_cell(_("Total"), "align=right colspan=" . $colspan);
		amount_cell($order->gl_items_total_debit());
		amount_cell(abs($order->gl_items_total_credit()));
		end_row();
	}

    end_table();
	div_end();
}

//---------------------------------------------------------------------------------

function gl_edit_item_controls(&$order, $dim, $Index=null)
{
	global $Ajax;
	start_row();

	$id = find_submit('Edit');
	if ($Index != -1 && $Index == $id)
	{
		$item = $order->gl_items[$Index];
		$_POST['code_id'] = $item->code_id;
		$_POST['dimension_id'] = $item->dimension_id;
		$_POST['dimension2_id'] = $item->dimension2_id;
		if ($item->amount > 0)
		{
			$_POST['AmountDebit'] = price_format($item->amount);
			$_POST['AmountCredit'] = "";
		}
		else
		{
			$_POST['AmountDebit'] = "";
			$_POST['AmountCredit'] = price_format(abs($item->amount));
		}	
		$_POST['description'] = $item->description;
		$_POST['LineMemo'] = $item->reference;

		hidden('Index', $item->index);
		hidden('code_id', $item->code_id);
		label_cell($_POST['code_id']);
		label_cell($item->description);
		if ($dim >= 1) 
			dimensions_list_cells(null, 'dimension_id', null, true, " ", false, 1);
		if ($dim > 1) 
			dimensions_list_cells(null, 'dimension2_id', null, true, " ", false, 2);
	    $Ajax->activate('items_table');
	}
	else
	{
		$_POST['AmountDebit'] = '';  //price_format(0);
		$_POST['AmountCredit'] = ''; //price_format(0);
		$_POST['dimension_id'] = 0;
		$_POST['dimension2_id'] = 0;
		$_POST['LineMemo'] = "";
		$_POST['_code_id_edit'] = "";
		$_POST['code_id'] = "";
		if(isset($_POST['_code_id_update'])) {
			    $Ajax->activate('code_id');
		}
		
		$skip_bank = ($_SESSION["wa_current_user"]->access != 2);
		gl_all_accounts_list('code_id', null, $skip_bank, false, true);
		if ($dim >= 1)
			dimensions_list_cells(null, 'dimension_id', null, true, " ", false, 1);
		if ($dim > 1)
			dimensions_list_cells(null, 'dimension2_id', null, true, " ", false, 2);
	}
	if ($dim < 1)
		hidden('dimension_id', 0);
	if ($dim < 2)
		hidden('dimension2_id', 0);

	small_amount_cells(null, 'AmountDebit');
	small_amount_cells(null, 'AmountCredit');
	text_cells_ex(null, 'LineMemo', 35, 50);

	if ($id != -1)
	{
		edit_button_cell('UpdateItem', _("Update"),
				_('Confirm changes'));
		edit_button_cell('CancelItemChanges', _("Cancel"),
				_('Cancel changes'));
 		set_focus('amount');
	} 
	else 
		submit_cells('AddItem', _("Add Item"), "colspan=2",
		    _('Add new line to journal'), true);

	end_row();
}

//---------------------------------------------------------------------------------

function gl_options_controls()
{
	  echo "<br><table align='center'>";

	  textarea_row(_("Memo"), 'memo_', null, 50, 3);

	  echo "</table>";
}


//---------------------------------------------------------------------------------

?>