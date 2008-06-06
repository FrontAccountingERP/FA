<?php

include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/ui/items_cart.inc");

//--------------------------------------------------------------------------------

function display_order_header(&$Order)
{
	global $table_style2;

	start_table("$table_style2 width=90%");
	echo "<tr><td valign=top width=50%>"; // outer table

	echo "<table>";
	start_row();
    date_cells(_("Date:"), 'date_');
	ref_cells(_("Reference:"), 'ref', '', references::get_next(0));
	end_row();

	echo "</table>";

	echo "</td><td width=50%>"; // outer table

	echo "<table>";

	check_row(_("Reverse Transaction:"), 'Reverse', null);

	echo "</table>";

	echo "</td></tr>"; // outer table
	end_table(1);
}

//---------------------------------------------------------------------------------

function display_gl_items($title, &$order)
{
	global $table_style, $path_to_root;

	display_heading($title);

	$dim = get_company_pref('use_dimension');

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

	foreach ($order->gl_items as $item) 
	{
		if (!isset($_GET['Edit']) || $_GET['Edit'] != $item->index)
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
			edit_link_cell("Edit=$item->index");
			delete_link_cell("Delete=$item->index");
    		end_row();;
		} 
		else 
		{
			gl_edit_item_controls($order, $dim, $item->index);
		}
	}

	if (!isset($_GET['Edit']))
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
}

//---------------------------------------------------------------------------------

function gl_edit_item_controls(&$order, $dim, $Index=null)
{
	start_row();

	if (isset($_GET['Edit']) && $Index != null)
	{
		$item = $order->gl_items[$Index];
		if (!isset($_POST['code_id']))
			$_POST['code_id'] = $item->code_id;
		if (!isset($_POST['dimension_id']))
			$_POST['dimension_id'] = $item->dimension_id;
		if (!isset($_POST['dimension2_id']))
			$_POST['dimension2_id'] = $item->dimension2_id;

		if ((!isset($_POST['AmountDebit']) || ($_POST['AmountDebit']=="")) && $item->amount > 0)
			$_POST['AmountDebit'] = price_format($item->amount);

		if ((!isset($_POST['AmountCredit']) || ($_POST['AmountCredit']=="")) && $item->amount <= 0)
			$_POST['AmountCredit'] = price_format(abs($item->amount));

		if (!isset($_POST['description']) || ($_POST['description'] == ""))
			$_POST['description'] = $item->description;
		if (!isset($_POST['LineMemo']) || ($_POST['LineMemo'] == ""))
			$_POST['LineMemo'] = $item->reference;

		hidden('Index', $item->index);
		hidden('code_id', $item->code_id);
		label_cell($_POST['code_id']);
		label_cell($item->description);
		if ($dim >= 1) 
			dimensions_list_cells(null, 'dimension_id', $_POST['dimension_id'], true, " ", false, 1);
		if ($dim > 1) 
			dimensions_list_cells(null, 'dimension2_id', $_POST['dimension2_id'], true, " ", false, 2);
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

	if (isset($_GET['Edit'])) 
	{
    	submit_cells('UpdateItem', _("Update"));
    	submit_cells('CancelItemChanges', _("Cancel"));
	} 
	else 
		submit_cells('AddItem', _("Add item"), "colspan=2");

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