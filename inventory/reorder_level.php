<?php

$page_security = 4;
$path_to_root="..";
include_once($path_to_root . "/includes/session.inc");

page(_("Reorder Levels"));

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/inventory/includes/inventory_db.inc");

check_db_has_costable_items(_("There are no inventory items defined in the system (Purchased or manufactured items)."));

//------------------------------------------------------------------------------------

if (isset($_GET['stock_id']))
{
	$_POST['stock_id'] = $_GET['stock_id'];
}

if (isset($_POST['_stock_id_update']))
	$Ajax->activate('reorders');
//------------------------------------------------------------------------------------

start_form(false, true);

if (!isset($_POST['stock_id']))
	$_POST['stock_id'] = get_global_stock_item();

echo "<center>" . _("Item:"). "&nbsp;";
stock_costable_items_list('stock_id', $_POST['stock_id'], false, true);

echo "<hr></center>";

stock_item_heading($_POST['stock_id']);

set_global_stock_item($_POST['stock_id']);

div_start('reorders');
start_table("$table_style width=30%");

$th = array(_("Location"), _("Quantity On Hand"), _("Re-Order Level"));
table_header($th);

$j = 1;
$k=0; //row colour counter

$result = get_loc_details($_POST['stock_id']);

while ($myrow = db_fetch($result))
{

	alt_table_row_color($k);

	if (isset($_POST['UpdateData']) && check_num($myrow["loc_code"]))
	{

		$myrow["reorder_level"] = input_num($myrow["loc_code"]);
		set_reorder_level($_POST['stock_id'], $myrow["loc_code"], input_num($myrow["loc_code"]));
	}

	$qoh = get_qoh_on_date($_POST['stock_id'], $myrow["loc_code"]);

	label_cell($myrow["location_name"]);

	$_POST[$myrow["loc_code"]] = qty_format($myrow["reorder_level"]);

	label_cell(number_format2($qoh,user_qty_dec()), "nowrap align='right'");
	qty_cells(null, $myrow["loc_code"]);
	end_row();
	$j++;
	If ($j == 12)
	{
		$j = 1;
		table_header($th);
	}
}

end_table(1);
div_end();
submit_center('UpdateData', _("Update"));

end_form();
end_page();

?>
