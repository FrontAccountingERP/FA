<?php

$page_security = 2;
$path_to_root="..";
include_once($path_to_root . "/includes/session.inc");

page(_("Inventory Item Sales prices"));

include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/inventory/includes/inventory_db.inc");

//---------------------------------------------------------------------------------------------------

check_db_has_stock_items(_("There are no items defined in the system."));

check_db_has_sales_types(_("There are no sales types in the system. Please set up sales types befor entering pricing."));

//---------------------------------------------------------------------------------------------------

$input_error = 0;

if (isset($_GET['stock_id']))
{
	$_POST['stock_id'] = $_GET['stock_id'];
}
if (isset($_GET['Item']))
{
	$_POST['stock_id'] = $_GET['Item'];
}

if (!isset($_POST['curr_abrev']))
{
	$_POST['curr_abrev'] = get_company_currency();
}

//---------------------------------------------------------------------------------------------------

start_form(false, true);

if (!isset($_POST['stock_id']))
	$_POST['stock_id'] = get_global_stock_item();

echo "<center>" . _("Item:"). "&nbsp;";
stock_items_list('stock_id', $_POST['stock_id'], false, true);
echo "<hr></center>";

// if stock sel has changed, clear the form
if ($_POST['stock_id'] != get_global_stock_item())
{
	clear_data();
}

set_global_stock_item($_POST['stock_id']);

//----------------------------------------------------------------------------------------------------

function clear_data()
{
	unset($_POST['PriceID']);
	unset($_POST['price']);
}

//----------------------------------------------------------------------------------------------------

if (isset($_POST['updatePrice']))
{

	if (!check_num('price', 0))
	{
		$input_error = 1;
		display_error( _("The price entered must be numeric."));
		set_focus('price');
	}

	if ($input_error != 1)
	{

		if (isset($_POST['PriceID']))
		{
			//editing an existing price
			update_item_price($_POST['PriceID'], $_POST['sales_type_id'],
			$_POST['curr_abrev'], input_num('price'));

			$msg = _("This price has been updated.");
		}
		elseif ($input_error !=1)
		{

			add_item_price($_POST['stock_id'], $_POST['sales_type_id'],
			    $_POST['curr_abrev'], input_num('price'));

			display_note(_("The new price has been added."));
		}
		clear_data();
	}

}

//------------------------------------------------------------------------------------------------------

if (isset($_GET['delete']))
{

	//the link to delete a selected record was clicked
	delete_item_price($_GET['PriceID']);
	echo _("The selected price has been deleted.");

}
if (isset($_POST['_stock_id_update'])) {
	$Ajax->activate('price_table');
	$Ajax->activate('price');
}
//---------------------------------------------------------------------------------------------------

$mb_flag = get_mb_flag($_POST['stock_id']);

$prices_list = get_prices($_POST['stock_id']);

div_start('price_table');
start_table("$table_style width=30%");

$th = array(_("Currency"), _("Sales Type"), _("Price"), "", "");
table_header($th);
$k = 0; //row colour counter

while ($myrow = db_fetch($prices_list))
{

	alt_table_row_color($k);

	label_cell($myrow["curr_abrev"]);
    label_cell($myrow["sales_type"]);
    amount_cell($myrow["price"]);
    edit_link_cell("PriceID=" . $myrow["id"]. "&Edit=1");
    delete_link_cell("PriceID=" . $myrow["id"]. "&delete=yes");
    end_row();

}
end_table();
div_end();
//------------------------------------------------------------------------------------------------

if (db_num_rows($prices_list) == 0)
{
	display_note(_("There are no prices set up for this part."));
}

echo "<br>";

if (isset($_GET['Edit']))
{
	$myrow = get_stock_price($_GET['PriceID']);
	hidden('PriceID', $_GET['PriceID']);
	$_POST['curr_abrev'] = $myrow["curr_abrev"];
	$_POST['sales_type_id'] = $myrow["sales_type_id"];
	$_POST['price'] = price_format($myrow["price"]);
}

start_table($table_style2);

currencies_list_row(_("Currency:"), 'curr_abrev', null);

sales_types_list_row(_("Sales Type:"), 'sales_type_id', null);

small_amount_row(_("Price:"), 'price', null);

end_table(1);

submit_center('updatePrice', _("Add/Update Price"));


end_form();
end_page();
?>
