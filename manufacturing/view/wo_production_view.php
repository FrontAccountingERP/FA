<?php

$page_security = 10;
$path_to_root="../..";

include_once($path_to_root . "/includes/session.inc");

page(_("View Work Order Production"), true);

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/manufacturing.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/manufacturing/includes/manufacturing_db.inc");
include_once($path_to_root . "/manufacturing/includes/manufacturing_ui.inc");

//-------------------------------------------------------------------------------------------------

if ($_GET['trans_no'] != "")
{
	$wo_production = $_GET['trans_no'];
}

//-------------------------------------------------------------------------------------------------

function display_wo_production($prod_id)
{
	global $table_style;

    $myrow = get_work_order_produce($prod_id);

    start_table($table_style);
    $th = array(_("Production #"), _("Reference"), _("For Work Order #"),
    	_("Item"), _("Quantity Manufactured"), _("Date"));
    table_header($th);

	start_row();
	label_cell($myrow["id"]);
	label_cell($myrow["reference"]);
	label_cell(get_trans_view_str(systypes::work_order(),$myrow["workorder_id"]));
	label_cell($myrow["stock_id"] . " - " . $myrow["StockDescription"]);
	qty_cell($myrow["quantity"], false, get_qty_dec($myrow["stock_id"]));
	label_cell(sql2date($myrow["date_"]));
	end_row();

    comments_display_row(29, $prod_id);

	end_table(1);

	is_voided_display(29, $prod_id, _("This production has been voided."));
}

//-------------------------------------------------------------------------------------------------

display_heading(systypes::name(29) . " # " . $wo_production);

display_wo_production($wo_production);

//-------------------------------------------------------------------------------------------------

br(2);

end_page(true);

?>

