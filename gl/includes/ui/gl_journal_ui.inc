<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU General Public License, GPL, 
	as published by the Free Software Foundation, either version 3 
	of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/gpl-3.0.html>.
***********************************************************************/
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/ui/items_cart.inc");

//--------------------------------------------------------------------------------

function display_order_header(&$Order)
{
	global $Ajax;

	$qes = has_quick_entries(QE_JOURNAL);
	$new = $Order->order_id==0;

	hidden('ref_original');
	start_outer_table(TABLESTYLE2, "width='90%'");
	table_section(1, $qes ? "20%" : "");

    date_row(_("Date:"), 'date_', '', $new);

	table_section(2, $qes ? "20%" : "50%");
	ref_row(_("Reference:"), 'ref', '');

 	if ($new) {
		table_section(3, "20%");
 		start_row();
 		check_cells(_("Reverse Transaction:"), 'Reverse', null);
 		end_row();
 	}

	if ($qes !== false)
	{
		table_section(3, "40%");
		start_row();
		quick_entries_list_cells(_("Quick Entry").":", 'person_id', null, QE_JOURNAL, true);
		$qid = get_quick_entry(get_post('person_id'));
		if (list_updated('person_id')) {
			unset($_POST['totamount']); // enable default
		$Ajax->activate('totamount');
		}
		if ($qid['bal_type'] == 1)
		{
			end_row();
			start_row();
			$accname = get_gl_account_name($qid['base_desc']);
			label_cell(($qid['base_amount'] == 0 ? _("Yearly") : _("Monthly")) . " ". _("balance from account")." ".
				$qid['base_desc']." ".$accname."&nbsp;&nbsp;".submit('go', _("Go"), false, false, true), "colspan=2");
		}
		else
			amount_cells($qid['base_desc'].":", 'totamount', price_format($qid['base_amount']),
		 		null, "&nbsp;&nbsp;".submit('go', _("Go"), false, false, true));
		end_row();

	}
		
	end_outer_table(1);
}

//---------------------------------------------------------------------------------

function display_gl_items($title, &$order)
{
	global $path_to_root;

	display_heading($title);

	$dim = get_company_pref('use_dimension');

    div_start('items_table');
	start_table(TABLESTYLE, "width='95%'");
	if ($dim == 2)
		$th = array(_("Account Code"), _("Account Description"), _("Dimension")." 1",
			_("Dimension")." 2", _("Debit"), _("Credit"), _("Memo"), "");
	else if ($dim == 1)
		$th = array(_("Account Code"), _("Account Description"), _("Dimension"),
			_("Debit"), _("Credit"), _("Memo"), "");
	else
		$th = array(_("Account Code"), _("Account Description"),
			_("Debit"), _("Credit"), _("Memo"), "");

	if (count($order->gl_items)) $th[] = '';

	table_header($th);	

	$k = 0;

	$id = find_submit('Edit');
	foreach ($order->gl_items as $line => $item) 
	{
		if ($id != $line)
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

			edit_button_cell("Edit$line", _("Edit"),
				_('Edit journal line'));
			delete_button_cell("Delete$line", _("Delete"),
				_('Remove line from journal'));
    		end_row();
		} 
		else 
		{
			gl_edit_item_controls($order, $dim, $line);
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
		label_cell('', "colspan=3");
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
	    // Modifying an existing row
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

		hidden('Index', $id);
		$skip_bank = !$_SESSION["wa_current_user"]->can_access('SA_BANKJOURNAL');
		echo gl_all_accounts_list('code_id', null, $skip_bank, true);
		if ($dim >= 1) 
			dimensions_list_cells(null, 'dimension_id', null, true, " ", false, 1);
		if ($dim > 1) 
			dimensions_list_cells(null, 'dimension2_id', null, true, " ", false, 2);
	    $Ajax->activate('items_table');
	}
	else
	{
	    // Adding a new row
		$_POST['AmountDebit'] = '';  //price_format(0);
		$_POST['AmountCredit'] = ''; //price_format(0);
		$_POST['dimension_id'] = 0;
		$_POST['dimension2_id'] = 0;
		//$_POST['LineMemo'] = ""; // let memo go to next line Joe Hunt 2010-05-30
		$_POST['_code_id_edit'] = "";
		$_POST['code_id'] = "";
		if(isset($_POST['_code_id_update'])) {
			    $Ajax->activate('code_id');
		}
		
		$skip_bank = !$_SESSION["wa_current_user"]->can_access('SA_BANKJOURNAL');
		echo gl_all_accounts_list('code_id', null, $skip_bank, true);
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
	text_cells_ex(null, 'LineMemo', 35, 255);

	if ($id != -1)
	{
		button_cell('UpdateItem', _("Update"),
				_('Confirm changes'), ICON_UPDATE);
		button_cell('CancelItemChanges', _("Cancel"),
				_('Cancel changes'), ICON_CANCEL);
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