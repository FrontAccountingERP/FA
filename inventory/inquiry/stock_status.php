<?php

$page_security = 2;
$path_to_root="../..";
include_once($path_to_root . "/includes/session.inc");

if (isset($_GET['stock_id'])){
	$_POST['stock_id'] = $_GET['stock_id'];
	page(_("Inventory Item Status"), true);
} else {
	page(_("Inventory Item Status"));
}

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/manufacturing.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/inventory/includes/inventory_db.inc");

if (isset($_POST['_stock_id_update']))
	$Ajax->activate('status_tbl');
//----------------------------------------------------------------------------------------------------

check_db_has_stock_items(_("There are no items defined in the system."));

start_form(false, true);

if (!isset($_POST['stock_id']))
	$_POST['stock_id'] = get_global_stock_item();

echo "<center> " . _("Item:"). " ";
stock_items_list('stock_id', $_POST['stock_id'], false, true);
echo "<br>";

echo "<hr></center>";

set_global_stock_item($_POST['stock_id']);

$mb_flag = get_mb_flag($_POST['stock_id']);
$kitset_or_service = false;

if (is_service($mb_flag))
{
	display_note(_("This is a service and cannot have a stock holding, only the total quantity on outstanding sales orders is shown."));
	$kitset_or_service = true;
}

$loc_details = get_loc_details($_POST['stock_id']);

div_start('status_tbl');
start_table($table_style);

if ($kitset_or_service == true)
{
	$th = array(_("Location"), _("Demand"));
}
else
{
	$th = array(_("Location"), _("Quantity On Hand"), _("Re-Order Level"),
		_("Demand"), _("Available"), _("On Order"));
}
table_header($th);
$j = 1;
$k = 0; //row colour counter

while ($myrow = db_fetch($loc_details))
{

	alt_table_row_color($k);

	$sql = "SELECT Sum(".TB_PREF."sales_order_details.quantity-".TB_PREF."sales_order_details.qty_sent) AS DEM
		FROM ".TB_PREF."sales_order_details, ".TB_PREF."sales_orders
		WHERE ".TB_PREF."sales_orders.order_no = ".TB_PREF."sales_order_details.order_no
		AND ".TB_PREF."sales_orders.from_stk_loc='" . $myrow["loc_code"] . "'
		AND ".TB_PREF."sales_order_details.qty_sent < ".TB_PREF."sales_order_details.quantity
		AND ".TB_PREF."sales_order_details.stk_code='" . $_POST['stock_id'] . "'";

	$demand_result = db_query($sql,"Could not retreive demand for item");

	if (db_num_rows($demand_result) == 1)
	{
	  $demand_row = db_fetch_row($demand_result);
	  $demand_qty =  $demand_row[0];
	}
	else
	{
	  $demand_qty =0;
	}


	$qoh = get_qoh_on_date($_POST['stock_id'], $myrow["loc_code"]);

	if ($kitset_or_service == false)
	{
		$sql = "SELECT Sum(".TB_PREF."purch_order_details.quantity_ordered - ".TB_PREF."purch_order_details.quantity_received) AS qoo
			FROM ".TB_PREF."purch_order_details INNER JOIN ".TB_PREF."purch_orders ON ".TB_PREF."purch_order_details.order_no=".TB_PREF."purch_orders.order_no
			WHERE ".TB_PREF."purch_orders.into_stock_location='" . $myrow["loc_code"] . "'
			AND ".TB_PREF."purch_order_details.item_code='" . $_POST['stock_id'] . "'";
		$qoo_result = db_query($sql,"could not receive quantity on order for item");

		if (db_num_rows($qoo_result) == 1)
		{
    		$qoo_row = db_fetch_row($qoo_result);
    		$qoo =  $qoo_row[0];
		}
		else
		{
			$qoo = 0;
		}

		label_cell($myrow["location_name"]);
		qty_cell($qoh);
        qty_cell($myrow["reorder_level"]);
        qty_cell($demand_qty);
        qty_cell($qoh - $demand_qty);
        qty_cell($qoo);
        end_row();

	}
	else
	{
	/* It must be a service or kitset part */
		label_cell($myrow["location_name"]);
		qty_cell($demand_qty);
		end_row();

	}
	$j++;
	If ($j == 12)
	{
		$j = 1;
		table_header($th);
	}
}

end_table();
div_end();
end_form();
end_page();

?>
