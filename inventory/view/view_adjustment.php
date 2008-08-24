<?php

$page_security = 1;
$path_to_root="../..";

include($path_to_root . "/includes/session.inc");

page(_("View Inventory Adjustment"), true);

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/inventory/includes/inventory_db.inc");

if (isset($_GET["trans_no"]))
{
	$trans_no = $_GET["trans_no"];
}

display_heading(systypes::name(systypes::inventory_adjustment()) . " #$trans_no");

$adjustment_items = get_stock_adjustment_items($trans_no);
$k = 0;
$header_shown = false;
while ($adjustment = db_fetch($adjustment_items))
{

	if (!$header_shown)
	{
		$adjustment_type = get_movement_type($adjustment['person_id']) ;

		start_table("$table_style2 width=90%");
		start_row();
		label_cells(_("At Location"), $adjustment['location_name'], "class='tableheader2'");
    	label_cells(_("Reference"), $adjustment['reference'], "class='tableheader2'", "colspan=6");
		label_cells(_("Date"), sql2date($adjustment['tran_date']), "class='tableheader2'");
		label_cells(_("Adjustment Type"), $adjustment_type['name'], "class='tableheader2'");
		end_row();
		comments_display_row(systypes::inventory_adjustment(), $trans_no);

		end_table();
		$header_shown = true;

		echo "<br>";
		start_table("$table_style width=90%");

    	$th = array(_("Item"), _("Description"), _("Quantity"),
    		_("Units"), _("Unit Cost"));
    	table_header($th);
	}

    alt_table_row_color($k);

    label_cell($adjustment['stock_id']);
    label_cell($adjustment['description']);
    qty_cell($adjustment['qty'], false, get_qty_dec($adjustment['stock_id']));
    label_cell($adjustment['units']);
    amount_cell($adjustment['standard_cost']);
    end_row();
}

end_table(1);

is_voided_display(systypes::inventory_adjustment(), $trans_no, _("This adjustment has been voided."));

end_page(true);
?>