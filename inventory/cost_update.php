<?php

$page_security = 2;
$path_to_root="..";
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/banking.inc");
include_once($path_to_root . "/includes/manufacturing.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/inventory/includes/inventory_db.inc");
$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
page(_("Inventory Item Cost Update"), false, false, "", $js);

//--------------------------------------------------------------------------------------

check_db_has_costable_items(_("There are no costable inventory items defined in the system (Purchased or manufactured items)."));

if (isset($_GET['stock_id']))
{
	$_POST['stock_id'] = $_GET['stock_id'];
}

//--------------------------------------------------------------------------------------

if (isset($_POST['UpdateData']))
{

   	$old_cost = $_POST['OldMaterialCost'] + $_POST['OldLabourCost'] + $_POST['OldOverheadCost'];
   	$new_cost = $_POST['material_cost'] + $_POST['labour_cost'] + $_POST['overhead_cost'];

   	$should_update = true;

	if (!is_numeric($_POST['material_cost']) || !is_numeric($_POST['labour_cost']) ||
		!is_numeric($_POST['overhead_cost']))
	{
		display_error( _("The entered cost is not numeric."));
   	 	$should_update = false;
	}
	elseif ($old_cost == $new_cost)
	{
   	 	display_error( _("The new cost is the same as the old cost. Cost was not updated."));
   	 	$should_update = false;
	}

   	if ($should_update)
   	{
		$update_no = stock_cost_update($_POST['stock_id'], $_POST['material_cost'],
			$_POST['labour_cost'], $_POST['overhead_cost'],	$old_cost);

        display_note(_("Cost has been updated."));

        if ($update_no > 0)
        {
    		display_note(get_gl_view_str(systypes::cost_update(), $update_no, _("View the GL Journal Entries for this Cost Update")), 1, 0);
        }
   	}
}

//-----------------------------------------------------------------------------------------

start_form(false, true);

if (!isset($_POST['stock_id']))
	$_POST['stock_id'] = get_global_stock_item();

echo "<center>" . _("Item:"). "&nbsp;";
stock_costable_items_list('stock_id', $_POST['stock_id'], false, true);

echo "</center><hr>";
set_global_stock_item($_POST['stock_id']);

$sql = "SELECT description, units, last_cost, actual_cost, material_cost, labour_cost,
	overhead_cost, mb_flag
	FROM ".TB_PREF."stock_master
	WHERE stock_id='" . $_POST['stock_id'] . "'
	GROUP BY description, units, last_cost, actual_cost, material_cost, labour_cost, overhead_cost, mb_flag";
$result = db_query($sql);
check_db_error("The cost details for the item could not be retrieved", $sql);

$myrow = db_fetch($result);

hidden("OldMaterialCost", $myrow["material_cost"]);
hidden("OldLabourCost", $myrow["labour_cost"]);
hidden("OldOverheadCost", $myrow["overhead_cost"]);

start_table($table_style2);
label_row(_("Last Cost"), number_format2($myrow["last_cost"],user_price_dec()),
	"class='tableheader2'", "nowrap align=right");

text_row(_("Standard Material Cost Per Unit"), "material_cost",
	number_format($myrow["material_cost"],user_price_dec()), "", "", "class='tableheader2'");

if ($myrow["mb_flag"]=='M')
{
	text_row(_("Standard Labour Cost Per Unit"), "labour_cost",
		number_format($myrow["labour_cost"],user_price_dec()), "", "", "class='tableheader2'");
	text_row(_("Standard Overhead Cost Per Unit"), "overhead_cost",
		number_format($myrow["overhead_cost"],user_price_dec()), "", "", "class='tableheader2'");
}
else
{
	hidden("labour_cost", 0);
	hidden("overhead_cost", 0);
}

end_table(1);
submit_center('UpdateData', _("Update"));

end_form();
end_page();

?>
