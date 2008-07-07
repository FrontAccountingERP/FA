<?php

$page_security = 9;
$path_to_root="..";
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/admin/db/company_db.inc");
include_once($path_to_root . "/includes/ui.inc");
$js = "";
if ($use_date_picker)
	$js .= get_js_date_picker();
page(_("Fiscal Years"), false, false, "", $js);

simple_page_mode(true);
//---------------------------------------------------------------------------------------------

function check_data()
{
	if (!is_date($_POST['from_date']))
	{
		display_error( _("Invalid BEGIN date in fiscal year."));
		set_focus('from_date');
		return false;
	}
	if (!is_date($_POST['to_date']))
	{
		display_error( _("Invalid END date in fiscal year."));
		set_focus('to_date');
		return false;
	}
	if (date1_greater_date2($_POST['from_date'], $_POST['to_date']))
	{
		display_error( _("BEGIN date bigger than END date."));
		set_focus('from_date');
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------------------------

function handle_submit()
{
	global $selected_id, $Mode;

	if (!check_data())
		return false;

	if ($selected_id != -1)
	{
   		update_fiscalyear($_POST['from_date'], $_POST['closed']);
		display_notification(_('Selected fiscal year has been updated'));
	}
	else
	{
   		add_fiscalyear($_POST['from_date'], $_POST['to_date'], $_POST['closed']);
		display_notification(_('New fiscal year has been added'));
	}
	$Mode = 'RESET';
}

//---------------------------------------------------------------------------------------------

function check_can_delete($selected_id)
{
	$myrow = get_fiscalyear($selected_id);
	// PREVENT DELETES IF DEPENDENT RECORDS IN gl_trans
	$from = $myrow['begin'];
	$to = $myrow['end'];
	$sql= "SELECT COUNT(*) FROM ".TB_PREF."gl_trans WHERE tran_date >= '$from' AND tran_date <= '$to'";
	$result = db_query($sql, "could not query gl_trans master");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0)
	{
		display_error(_("Cannot delete this fiscal year because items have been created referring to it."));
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------------------------

function handle_delete()
{
	global $selected_id, $Mode;

	if (!check_can_delete($selected_id))
		return;
	//only delete if used in neither customer or supplier, comp prefs, bank trans accounts

	delete_fiscalyear($selected_id);
	display_notification(_('Selected fiscal year has been deleted'));
	$Mode = 'RESET';
}

//---------------------------------------------------------------------------------------------

function display_fiscalyears()
{
	global $table_style;

	$company_year = get_company_pref('f_year');

	$result = get_all_fiscalyears();
	start_form();
	start_table($table_style);

	$th = array(_("Fiscal Year Begin"), _("Fiscal Year End"), _("Closed"), "", "");
	table_header($th);

	$k=0;
	while ($myrow=db_fetch($result))
	{
    	if ($myrow['id'] == $company_year)
    	{
    		start_row("class='stockmankobg'");
    	}
    	else
    		alt_table_row_color($k);

		$from = sql2date($myrow["begin"]);
		$to = sql2date($myrow["end"]);
		if ($myrow["closed"] == 0)
		{
			$closed_text = _("No");
		}
		else
		{
			$closed_text = _("Yes");
		}
		label_cell($from);
		label_cell($to);
		label_cell($closed_text);
	 	edit_button_cell("Edit".$myrow['id'], _("Edit"));
		if ($myrow["id"] != $company_year)
 			edit_button_cell("Delete".$myrow['id'], _("Delete"));
		end_row();
	}

	end_table();
	end_form();
	display_note(_("The marked fiscal year is the current fiscal year which cannot be deleted."), 0, 0, "class='currentfg'");
}

//---------------------------------------------------------------------------------------------

function display_fiscalyear_edit($selected_id)
{
	global $table_style2, $Mode;

	start_form();
	start_table($table_style2);

	if ($selected_id != -1)
	{
		if($Mode =='Edit')
		{
			$myrow = get_fiscalyear($selected_id);

			$_POST['from_date'] = sql2date($myrow["begin"]);
			$_POST['to_date']  = sql2date($myrow["end"]);
			$_POST['closed']  = $myrow["closed"];
		}
			hidden('from_date');
			hidden('to_date');
			label_row(_("Fiscal Year Begin:"), $_POST['from_date']);
			label_row(_("Fiscal Year End:"), $_POST['to_date']);
	}
	else
	{
		date_row(_("Fiscal Year Begin:"), 'from_date', '', null, 0, 0, 1001);
		date_row(_("Fiscal Year End:"), 'to_date', '', null, 0, 0, 1001);
	}
	hidden('selected_id', $selected_id);

	yesno_list_row(_("Is Closed:"), 'closed', null, "", "", false);

	end_table(1);

	submit_add_or_update_center($selected_id == -1, '', true);

	end_form();
}

//---------------------------------------------------------------------------------------------

if ($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM') 
{
	handle_submit();
}

//---------------------------------------------------------------------------------------------

if ($Mode == 'Delete')
{
	global $selected_id;
	handle_delete($selected_id);
}

if ($Mode == 'RESET')
{
	$selected_id = -1;
}
//---------------------------------------------------------------------------------------------

display_fiscalyears();

echo '<br>';

display_fiscalyear_edit($selected_id);

//---------------------------------------------------------------------------------------------

end_page();

?>
