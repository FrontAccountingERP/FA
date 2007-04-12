<?php

$page_security = 9;
$path_to_root="..";
include_once($path_to_root . "/includes/session.inc");

page(_("Fiscal Years"));

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/admin/db/company_db.inc");
include_once($path_to_root . "/includes/ui.inc");

//---------------------------------------------------------------------------------------------

if (isset($_GET['selected_id']))
{
	$selected_id = $_GET['selected_id'];
} 
elseif (isset($_POST['selected_id']))
{
	$selected_id = $_POST['selected_id'];
}
else
	$selected_id = null;

//---------------------------------------------------------------------------------------------

function check_data()
{
	if (!isset($selected_id))
		$from = $_POST['from_date'];
	else
		$from = $selected_id;
	if (!is_date($from)) 
	{
		display_error( _("Invalid BEGIN date in fiscal year."));
		return false;
	}
	if (!is_date($_POST['to_date'])) 
	{
		display_error( _("Invalid END date in fiscal year."));
		return false;
	}
	if (date1_greater_date2($from, $_POST['to_date'])) 
	{
		display_error( _("BEGIN date bigger than END date."));
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------------------------

function handle_submit()
{
	global $selected_id;

	if (!check_data())
		return false;

	if (isset($selected_id)) 
	{
   		update_fiscalyear($_POST['from_date'], $_POST['closed']);
	} 
	else 
	{
   		add_fiscalyear($_POST['from_date'], $_POST['to_date'], $_POST['closed']);
	}

	return true;
}

//---------------------------------------------------------------------------------------------

function check_can_delete($todate)
{
	global $selected_id;

	// PREVENT DELETES IF DEPENDENT RECORDS IN gl_trans
	$from = date2sql($selected_id);
	$to = date2sql($todate);
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

function handle_delete($todate)
{
	global $selected_id;

	if (!check_can_delete($todate))
		return;
	//only delete if used in neither customer or supplier, comp prefs, bank trans accounts

	delete_fiscalyear($selected_id);

	meta_forward($_SERVER['PHP_SELF']);
}

//---------------------------------------------------------------------------------------------

function display_fiscalyears()
{
	global $table_style;

	$company_year = get_company_pref('f_year');

	$result = get_all_fiscalyears();

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
		edit_link_cell("selected_id=" . urlencode($from));
		if ($myrow["id"] != $company_year)
			delete_link_cell("selected_id=" . urlencode($from) . "&to_date=" . urlencode($to) . "&delete=1");
		end_row();
	}

	end_table();;
	display_note(_("The marked fiscal year is the current fiscal year which cannot be deleted."), 0, 0, "class='currentfg'");
}

//---------------------------------------------------------------------------------------------

function display_fiscalyear_edit($selected_id)
{
	global $table_style2;
	
	start_form();
	start_table($table_style2);

	if ($selected_id) 
	{
		$myrow = get_fiscalyear($selected_id);

		$_POST['from_date'] = sql2date($myrow["begin"]);
		$_POST['to_date']  = sql2date($myrow["end"]);
		$_POST['closed']  = $myrow["closed"];
		hidden('selected_id', $selected_id);
		hidden('from_date', $_POST['from_date']);
		hidden('to_date', $_POST['to_date']);
		label_row(_("Fiscal Year Begin:"), $_POST['from_date']);
		label_row(_("Fiscal Year End:"), $_POST['to_date']);
	} 
	else 
	{
		text_row(_("Fiscal Year Begin:"), 'from_date', null, 15, 10);
		text_row(_("Fiscal Year End:"), 'to_date', null, 15, 10);
	}

	yesno_list_row(_("Is Closed:"), 'closed', null, "", "", false);

	end_table(1);

	submit_add_or_update_center(!isset($selected_id));

	end_form();
}

//---------------------------------------------------------------------------------------------

if (isset($_POST['ADD_ITEM']) || isset($_POST['UPDATE_ITEM'])) 
{
	if (handle_submit()) 
	{
		meta_forward($_SERVER['PHP_SELF']);
	}
}

//---------------------------------------------------------------------------------------------

if (isset($_GET['delete'])) 
{
	handle_delete($_GET['to_date']);
}

//---------------------------------------------------------------------------------------------

display_fiscalyears();

hyperlink_no_params($_SERVER['PHP_SELF'], _("Enter a New Fiscal Year"));

display_fiscalyear_edit($selected_id);

//---------------------------------------------------------------------------------------------

end_page();

?>
