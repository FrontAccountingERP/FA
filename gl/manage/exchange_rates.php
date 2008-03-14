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
	$selected_id = "";
//---------------------------------------------------------------------------------------------
function check_data()
{
	if (!is_date($_POST['date_'])) 
	{
		display_error( _("The entered date is invalid."));
		return false;
	}
	if (!check_num('BuyRate', 0))
	{
		display_error( _("The exchange rate must be numeric and greater than zero."));
		return false;
	}
	if ($_POST['BuyRate'] <= 0)
	{
		display_error( _("The exchange rate cannot be zero or a negative number."));
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

	return true;
}

//---------------------------------------------------------------------------------------------

function handle_delete()
{
	global $selected_id;

	if ($selected_id == "")
		return;
	delete_exchange_rate($selected_id);

	meta_forward($_SERVER['PHP_SELF']);
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
		edit_link_cell("selected_id=" . $myrow["id"]);
		delete_link_cell("selected_id=" . $myrow["id"]. "&delete=1");

		end_row();

    } //END WHILE LIST LOOP

    end_table();
}

//---------------------------------------------------------------------------------------------

function display_rate_edit()
{
	global $selected_id, $table_style2;

	start_table($table_style2);

	if (isset($_POST['get_rate']))
	{
		$_POST['BuyRate'] = exrate_format(get_ecb_rate($_POST['curr_abrev']));
	}	
	if ($selected_id != "") 
	{
		//editing an existing exchange rate

		$myrow = get_exchange_rate($selected_id);

		$_POST['date_'] = sql2date($myrow["date_"]);
		$_POST['BuyRate'] = exrate_format($myrow["rate_buy"]);

		hidden('selected_id', $selected_id);
		hidden('date_', $_POST['date_']);
		hidden('curr_abrev', $_POST['curr_abrev']);

		label_row(_("Date to Use From:"), $_POST['date_']);
	} 
	else 
	{
		date_row(_("Date to Use From:"), 'date_');
	}
	small_amount_row(_("Exchange Rate:"), 'BuyRate', null, '', submit('get_rate',_("Get"), false));

	end_table(1);

	submit_add_or_update_center($selected_id == "");

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

	handle_delete();
}

//---------------------------------------------------------------------------------------------

echo "<center>";
start_form(false, true);

if (!isset($_POST['curr_abrev']))
	$_POST['curr_abrev'] = get_global_curr_code();

echo _("Select a currency :") . "  ";
currencies_list('curr_abrev', $_POST['curr_abrev'], true);

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

   	hyperlink_no_params($_SERVER['PHP_SELF'], _("Enter a New Exchange Rate"));
   	br(1);

    display_rate_edit();
}

end_form();

end_page();

?>
