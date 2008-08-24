<?php

$page_security = 9;
$path_to_root="../..";
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/banking.inc");

$js = "";
if ($use_date_picker)
	$js .= get_js_date_picker();
page(_("Exchange Rates"), false, false, "", $js);

simple_page_mode(false);

//---------------------------------------------------------------------------------------------
function check_data()
{
	if (!is_date($_POST['date_']))
	{
		display_error( _("The entered date is invalid."));
		set_focus('date_');
		return false;
	}
	if (!check_num('BuyRate', 0))
	{
		display_error( _("The exchange rate must be numeric and greater than zero."));
		set_focus('BuyRate');
		return false;
	}
	if ($_POST['BuyRate'] <= 0)
	{
		display_error( _("The exchange rate cannot be zero or a negative number."));
		set_focus('BuyRate');
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

	if ($selected_id != "")
	{

		update_exchange_rate($_POST['curr_abrev'], $_POST['date_'],
		input_num('BuyRate'), input_num('BuyRate'));
	}
	else
	{

		add_exchange_rate($_POST['curr_abrev'], $_POST['date_'],
		    input_num('BuyRate'), input_num('BuyRate'));
	}

	$selected_id = '';
}

//---------------------------------------------------------------------------------------------

function handle_delete()
{
	global $selected_id;

	if ($selected_id == "")
		return;
	delete_exchange_rate($selected_id);
	$selected_id = '';
}

//---------------------------------------------------------------------------------------------

function display_rates($curr_code)
{
	global $table_style;

	$result = get_exchange_rates($curr_code);

	br(2);
	start_table($table_style);
	$th = array(_("Date to Use From"), _("Exchange Rate"), "", "");
	table_header($th);

    $k = 0; //row colour counter

    while ($myrow = db_fetch($result))
    {

   		alt_table_row_color($k);

    	label_cell(sql2date($myrow["date_"]));
		label_cell(number_format2($myrow["rate_buy"], user_exrate_dec()), "nowrap align=right");
 		edit_button_cell("Edit".$myrow["id"], _("Edit"));
 		edit_button_cell("Delete".$myrow["id"], _("Delete"));

		end_row();

    }

    end_table();
}

//---------------------------------------------------------------------------------------------

function display_rate_edit()
{
	global $selected_id, $table_style2, $Ajax;

	start_table($table_style2);

	if ($selected_id != "")
	{
		//editing an existing exchange rate

		$myrow = get_exchange_rate($selected_id);

		$_POST['date_'] = sql2date($myrow["date_"]);
		$_POST['BuyRate'] = exrate_format($myrow["rate_buy"]);

		hidden('selected_id', $selected_id);
		hidden('date_', $_POST['date_']);

		label_row(_("Date to Use From:"), $_POST['date_']);
	}
	else
	{
		$_POST['date_'] = Today();
		$_POST['BuyRate'] = '';
		date_row(_("Date to Use From:"), 'date_');
	}
	if (isset($_POST['get_rate']))
	{
		$_POST['BuyRate'] = exrate_format(get_ecb_rate($_POST['curr_abrev']));
		$Ajax->activate('BuyRate');
	}
	small_amount_row(_("Exchange Rate:"), 'BuyRate', null, '',
	  	submit('get_rate',_("Get"), false, _('Get current ECB rate') , true),
		user_exrate_dec());

	end_table(1);

	submit_add_or_update_center($selected_id == '', '', true);

	display_note(_("Exchange rates are entered against the company currency."), 1);
}

//---------------------------------------------------------------------------------------------

function clear_data()
{
	unset($_POST['selected_id']);
	unset($_POST['date_']);
	unset($_POST['BuyRate']);
}

//---------------------------------------------------------------------------------------------

if ($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM') 
	handle_submit();

//---------------------------------------------------------------------------------------------

if ($Mode == 'Delete')
	handle_delete();


//---------------------------------------------------------------------------------------------

start_form(false, true);

if (!isset($_POST['curr_abrev']))
	$_POST['curr_abrev'] = get_global_curr_code();

echo "<center>";
echo _("Select a currency :") . "  ";
currencies_list('curr_abrev', null, true);
echo "</center>";

// if currency sel has changed, clear the form
if ($_POST['curr_abrev'] != get_global_curr_code())
{
	clear_data();
	$selected_id = "";
}

set_global_curr_code($_POST['curr_abrev']);

if (is_company_currency($_POST['curr_abrev']))
{

	display_note(_("The selected currency is the company currency."), 2);
	display_note(_("The company currency is the base currency so exchange rates cannot be set for it."), 1);
}
else
{

    display_rates($_POST['curr_abrev']);
   	br(1);
    display_rate_edit();
}

end_form();

end_page();

?>
