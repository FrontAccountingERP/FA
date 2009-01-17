<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU Affero General Public License,
	AGPL, as published by the Free Software Foundation, either version 
	3 of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/agpl-3.0.html>.
***********************************************************************/
$page_security = 3;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("Quick Entries"));

include($path_to_root . "/gl/includes/gl_db.inc");

include($path_to_root . "/includes/ui.inc");

simple_page_mode(true);
simple_page_mode2(true);

function simple_page_mode2($numeric_id = true)
{
	global $Ajax, $Mode2, $selected_id2;

	$default = $numeric_id ? -1 : '';
	$selected_id2 = get_post('selected_id2', $default);
	foreach (array('ADD_ITEM2', 'UPDATE_ITEM2', 'RESET2') as $m) {
		if (isset($_POST[$m])) {
			$Ajax->activate('_page_body');
			if ($m == 'RESET2') 
				$selected_id2 = $default;
			$Mode2 = $m; return;
		}
	}
	foreach (array('BEd', 'BDel') as $m) {
		foreach ($_POST as $p => $pvar) {
			if (strpos($p, $m) === 0) {
//				$selected_id2 = strtr(substr($p, strlen($m)), array('%2E'=>'.'));
				unset($_POST['_focus']); // focus on first form entry
				$selected_id2 = quoted_printable_decode(substr($p, strlen($m)));
				$Ajax->activate('_page_body');
				$Mode2 = $m;
				return;
			}
		}
	}
	$Mode2 = '';
}

function submit_add_or_update_center2($add=true, $title=false, $async=false)
{
	echo "<center>";
	if ($add)
		submit('ADD_ITEM2', _("Add new"), true, $title, $async);
	else {
		submit('UPDATE_ITEM2', _("Update"), true, $title, $async);
		submit('RESET2', _("Cancel"), true, $title, $async);
	}
	echo "</center>";
}

//-----------------------------------------------------------------------------------

function can_process() 
{

	if (strlen($_POST['description']) == 0) 
	{
		display_error( _("The Quick Entry description cannot be empty."));
		set_focus('description');
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------------

if ($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM') 
{

	if (can_process()) 	
	{	

		if ($selected_id != -1) 
		{
			update_quick_entry($selected_id, $_POST['description'], $_POST['deposit'], $_POST['bank_only']);
			display_notification(_('Selected quick entry has been updated'));
		} 
		else 
		{
			add_quick_entry($_POST['description'], $_POST['deposit'], $_POST['bank_only']);
			display_notification(_('New quick entry has been added'));
		}
		$Mode = 'RESET';
	}
}

if ($Mode2=='ADD_ITEM2' || $Mode2=='UPDATE_ITEM2') 
{
	if ($selected_id2 != -1) 
	{
		update_quick_entry_line($selected_id2, $selected_id, $_POST['account'], $_POST['tax_acc'], $_POST['pct'], input_num('amount', 0), 
			$_POST['dimension_id'], $_POST['dimension2_id']);
		display_notification(_('Selected quick entry line has been updated'));
	} 
	else 
	{
		add_quick_entry_line($selected_id, $_POST['account'], $_POST['tax_acc'], $_POST['pct'], input_num('amount', 0), 
			$_POST['dimension_id'], $_POST['dimension2_id']);
		display_notification(_('New quick entry line has been added'));
	}
	$Mode2 = 'RESET2';
}

//-----------------------------------------------------------------------------------

if ($Mode == 'Delete')
{
	if (!has_quick_entry_lines($selected_id))
	{
		delete_quick_entry($selected_id);
		display_notification(_('Selected quick entry has been deleted'));
		$Mode = 'RESET';
	}
	else
	{
		display_error( _("The Quick Entry has Quick Entry Lines. Cannot be deleted."));
		set_focus('description');
	}
}

if ($Mode2 == 'BDel')
{
	delete_quick_entry_line($selected_id2);
	display_notification(_('Selected quick entry line has been deleted'));
	$Mode2 = 'RESET2';
}
//-----------------------------------------------------------------------------------
if ($Mode == 'RESET')
{
	$selected_id = -1;
	$_POST['description'] = $_POST['deposit'] = $_POST['bank_only'] = '';
}
if ($Mode2 == 'RESET2')
{
	$selected_id2 = -1;
	$_POST['account'] = $_POST['tax_acc'] = $_POST['pct'] = $_POST['amount'] = $_POST['dimension_id'] = $_POST['dimension2_id'] = '';
}
//-----------------------------------------------------------------------------------

$result = get_quick_entries();
start_form();
start_table($table_style);
$th = array(_("Description"), _("Deposit"), _("Bank Only"), "", "");
table_header($th);

$k = 0;
while ($myrow = db_fetch($result)) 
{
	alt_table_row_color($k);
	$deposit_text = ($myrow["deposit"] == 0 ? _("No") : _("Yes"));
	$bank_text = ($myrow["bank_only"] == 0 ? _("No") : _("Yes"));
	label_cell($myrow['description']);
	label_cell($deposit_text);
	label_cell($bank_text);
	edit_button_cell("Edit".$myrow["id"], _("Edit"));
	delete_button_cell("Delete".$myrow["id"], _("Delete"));
	end_row();
}

end_table();
end_form();
//-----------------------------------------------------------------------------------

start_form();

start_table($table_style2);

if ($selected_id != -1) 
{
 	//if ($Mode == 'Edit') 
 	//{
		//editing an existing status code
		$myrow = get_quick_entry($selected_id);

		$_POST['id']  = $myrow["id"];
		$_POST['description']  = $myrow["description"];
		$_POST['deposit']  = $myrow["deposit"];
		$_POST['bank_only']  = $myrow["bank_only"];
		hidden('selected_id', $selected_id);
 	//}
} 

text_row_ex(_("Description:"), 'description', 50, 60);

yesno_list_row(_("Deposit:"), 'deposit', null, "", "", false);

yesno_list_row(_("Bank Only:"), 'bank_only', null, "", "", false);

end_table(1);

submit_add_or_update_center($selected_id == -1, '', true);

end_form();

if ($selected_id != -1)
{
	display_heading(_("Quick Entry Lines") . " - " . $_POST['description']);
	$result = get_quick_entry_lines($selected_id);
	start_form();
	start_table($table_style2);
	$dim = get_company_pref('use_dimension');
	if ($dim == 2)
		$th = array(_("Account"), _("Use Tax"), _("Percent"), _("Amount"), _("Dimension"), _("Dimension")." 2", "", "");
	else if ($dim == 1)	
		$th = array(_("Account"), _("Use Tax"), _("Percent"), _("Amount"), _("Dimension"), "", "");
	else	
		$th = array(_("Account"), _("Use Tax"), _("Percent"), _("Amount"), "", "");
	table_header($th);
	$k = 0;
	while ($myrow = db_fetch($result)) 
	{
		alt_table_row_color($k);
		label_cell($myrow['account']." ".$myrow['account_name']);
		$tax_text = ($myrow['tax_acc'] == 0 ? _("No") : _("Yes"));
		label_cell($tax_text);
		$pct_text = ($myrow['pct'] == 0 ? _("No") : _("Yes"));
		label_cell($pct_text);
		amount_cell($myrow['amount']);
   		if ($dim >= 1)
			label_cell(get_dimension_string($myrow['dimension_id'], true));
   		if ($dim > 1)
			label_cell(get_dimension_string($myrow['dimension2_id'], true));
		edit_button_cell("BEd".$myrow["id"], _("Edit"));
		delete_button_cell("BDel".$myrow["id"], _("Delete"));
		end_row();
	}
	end_table();
	hidden('selected_id', $selected_id);
	hidden('selected_id2', $selected_id2);
	hidden('description', $_POST['description']);
	hidden('deposit', $_POST['deposit']);
	hidden('bank_only', $_POST['bank_only']);
	end_form();
	start_form();

	start_table($table_style2);

	if ($selected_id2 != -1) 
	{
	 	if ($Mode2 == 'BEd') 
	 	{
			//editing an existing status code
			$myrow = get_quick_entry_line($selected_id2);

			$_POST['id']  = $myrow["id"];
			$_POST['account']  = $myrow["account"];
			$_POST['tax_acc']  = $myrow["tax_acc"];
			$_POST['pct']  = $myrow["pct"];
			$_POST['amount']  = $myrow["amount"];
			$_POST['dimension_id']  = $myrow["dimension_id"];
			$_POST['dimension2_id']  = $myrow["dimension2_id"];
	 	}
	} 

	gl_all_accounts_list_row(_("Account"), 'account', null);
	yesno_list_row(_("Use Tax:"), 'tax_acc', null, "", "", false);
	yesno_list_row(_("Use Percent:"), 'pct', null, "", "", false);
	amount_row(_("Amount"), 'amount', null);
	if ($dim >= 1) 
		dimensions_list_row(_("Dimension"), 'dimension_id', null, true, " ", false, 1);
	if ($dim > 1) 
		dimensions_list_row(_("Dimension")." 2", 'dimension2_id', null, true, " ", false, 2);
	
	end_table(1);
	if ($dim < 2)
		hidden('dimension2_id', 0);
	if ($dim < 1)
		hidden('dimension_id', 0);
	hidden('selected_id', $selected_id);
	hidden('selected_id2', $selected_id2);
	hidden('description', $_POST['description']);
	hidden('deposit', $_POST['deposit']);
	hidden('bank_only', $_POST['bank_only']);

	submit_add_or_update_center2($selected_id2 == -1, '', true);

	end_form();
}		
//------------------------------------------------------------------------------------

end_page();

?>
