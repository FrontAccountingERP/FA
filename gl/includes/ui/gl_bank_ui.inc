<?php

function display_bank_header(&$order)
{
	global $table_style2, $Ajax;
	$payment = $order->trans_type == systypes::bank_payment();

	div_start('pmt_header');
	start_table("width=90% $table_style2"); // outer table
	echo "<tr><td valign=top width=33%>";

	echo "<table>"; // inner table

    bank_accounts_list_row( $payment ? _("From:") : _("To:"), 'bank_account', null, true);

    date_row(_("Date:"), 'date_', '', null, 0, 0, 0, null, true);

	echo "</table>"; // inner table

	echo "</td><td width=33%>";

	echo "<table>"; // inner table

	if (!isset($_POST['PayType']))
	{
		if (isset($_GET['PayType']))
			$_POST['PayType'] = $_GET['PayType'];
		else
			$_POST['PayType'] = "";
	}
	if (!isset($_POST['person_id']))
	{
		if (isset($_GET['PayPerson']))
			$_POST['person_id'] = $_GET['PayPerson'];
		else
			$_POST['person_id'] = "";
	}
	if (isset($_POST['_PayType_update'])) {
		$_POST['person_id'] = '';
		$Ajax->activate('pmt_header');
		$Ajax->activate('code_id');
	}
    payment_person_types_list_row( $payment ? _("Pay To:"):_("From:"),
		 'PayType', $_POST['PayType'], true);
    switch ($_POST['PayType'])
    {
		case payment_person_types::misc() :
    		text_row_ex($payment ?_("To the Order of:"):_("Name:"),
				 'person_id', 40, 50);
    		break;
		case payment_person_types::WorkOrder() :
    		workorders_list_row(_("Work Order:"), 'person_id', null);
    		break;
		case payment_person_types::supplier() :
    		supplier_list_row(_("Supplier:"), 'person_id', null, false, true);
    		break;
		case payment_person_types::customer() :
    		customer_list_row(_("Customer:"), 'person_id', null, false, true);

        	if (db_customer_has_branches($_POST['person_id']))
        	{
        		customer_branches_list_row(_("Branch:"), $_POST['person_id'], 'PersonDetailID', null, false, true, true);
        	}
        	else
        	{
				$_POST['PersonDetailID'] = reserved_words::get_any_numeric();
        		hidden('PersonDetailID');
        	}
    		break;
		case payment_person_types::QuickEntry() :
			quick_entries_list_row(_("Description:"), 'person_id', null, $payment);
			amount_row(_("Total Amount"), 'totamount', null, null, "&nbsp;&nbsp;".submit('go', _("Go"), false, false, true));			
			break;	
		//case payment_person_types::Project() :
    	//	dimensions_list_row(_("Dimension:"), 'person_id', $_POST['person_id'], false, null, true);
    	//	break;
    }

	$person_currency = payment_person_types::person_currency($_POST['PayType'], $_POST['person_id']);
	$bank_currency = get_bank_account_currency($_POST['bank_account']);

	exchange_rate_display($bank_currency, $person_currency, $_POST['date_']);

	echo "</table>"; // inner table

	echo "</td><td>";

	echo "<table>"; // inner table

	bank_trans_types_list_row(_("Type:"), 'type', null);

	if (isset($_GET['NewPayment']))
    	ref_row(_("Reference:"), 'ref', '', references::get_next(systypes::bank_payment()));
    else
    	ref_row(_("Reference:"), 'ref', '', references::get_next(systypes::bank_deposit()));

	echo "</table>"; // inner table

	echo "</td></tr>";

	end_table(1); // outer table
	div_end();
}
//---------------------------------------------------------------------------------

function display_gl_items($title, &$order)
{
	global $table_style, $path_to_root;

	$dim = get_company_pref('use_dimension');
	$colspan = ($dim == 2 ? 4 : ($dim == 1 ? 3 : 2));
	display_heading($title);

    div_start('items_table');
	start_table("$table_style colspan=7 width=95%");

	if ($dim == 2)
		$th = array(_("Account Code"), _("Account Description"), _("Dimension")." 1",
			_("Dimension")." 2", _("Amount"), _("Memo"));
	else if ($dim == 1)
		$th = array(_("Account Code"), _("Account Description"), _("Dimension"),
			_("Amount"), _("Memo"));
	else
		$th = array(_("Account Code"), _("Account Description"),
			_("Amount"), _("Memo"));

	if (count($order->gl_items)) $th[] = '';

	table_header($th);
	$k = 0;  //row colour counter

	$id = find_submit('Edit');
	foreach ($order->gl_items as $item)
	{
		if ($id != $item->index)
		{
    		alt_table_row_color($k);

			label_cell($item->code_id);
			label_cell($item->description);
    		if ($dim >= 1)
				label_cell(get_dimension_string($item->dimension_id, true));
    		if ($dim > 1)
				label_cell(get_dimension_string($item->dimension2_id, true));
			//amount_cell(abs($item->amount));
			if ($order->trans_type == systypes::bank_deposit())
				amount_cell(-$item->amount);
			else		
				amount_cell($item->amount);
			label_cell($item->reference);

			edit_button_cell("Edit$item->index", _("Edit"),
				_('Edit document line'));
			edit_button_cell("Delete$item->index", _("Delete"),
				_('Remove line from document'));
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
		label_row(_("Total"), number_format2(abs($order->gl_items_total()), user_price_dec()),"colspan=" . $colspan . " align=right", "align=right");

    end_table();
	div_end();
}

//---------------------------------------------------------------------------------

function gl_edit_item_controls(&$order, $dim, $Index=null)
{
	global $Ajax;
	$payment = $order->trans_type == systypes::bank_payment();

	start_row();
	$id = find_submit('Edit');
	if ($Index != -1 && $Index == $id)
	{
		$item = $order->gl_items[$Index];
		$_POST['code_id'] = $item->code_id;
		$_POST['dimension_id'] = $item->dimension_id;
		$_POST['dimension2_id'] = $item->dimension2_id;
		$_POST['amount'] = price_format(abs($item->amount));
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
		$_POST['amount'] = price_format(0);
		$_POST['dimension_id'] = 0;
		$_POST['dimension2_id'] = 0;
		$_POST['LineMemo'] = "";
		if(isset($_POST['_code_id_update'])) {
			    $Ajax->activate('code_id');
		}

		if ($_POST['PayType'] == payment_person_types::customer())
		{
			$acc = get_branch_accounts($_POST['PersonDetailID']);
			$_POST['code_id'] = $acc['receivables_account'];
		}
		elseif ($_POST['PayType'] == payment_person_types::supplier())
		{
			$acc = get_supplier_accounts($_POST['person_id']);
			$_POST['code_id'] = $acc['payable_account'];
		}
		elseif ($_POST['PayType'] == payment_person_types::WorkOrder())
			$_POST['code_id'] = get_company_pref('default_assembly_act');
		else {
			$_POST['code_id'] =
				get_company_pref($payment ? 'default_cogs_act':'default_inv_sales_act');
		}
		gl_all_accounts_list('code_id', null, false, false, true);
		if ($dim >= 1)
			dimensions_list_cells(null, 'dimension_id', null, true, " ", false, 1);
		if ($dim > 1)
			dimensions_list_cells(null, 'dimension2_id', null, true, " ", false, 2);
	}
	if ($dim < 1)
		hidden('dimension_id', 0);
	if ($dim < 2)
		hidden('dimension2_id', 0);

	amount_cells(null, 'amount');
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
	{
		submit_cells('AddItem', _("Add Item"), "colspan=2",
		    _('Add new item to document'), true);
	}

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