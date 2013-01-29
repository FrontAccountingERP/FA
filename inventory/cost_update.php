<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU General Public License, GPL, 
	as published by the Free Software Foundation, either version 3 
	of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/gpl-3.0.html>.
***********************************************************************/
$page_security = 'SA_STANDARDCOST';
if (!@$_GET['popup'])
	$path_to_root = "..";
else	
	$path_to_root = "../..";

include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/banking.inc");
include_once($path_to_root . "/includes/manufacturing.inc");
include_once($path_to_root . "/includes/data_checks.inc");
include_once($path_to_root . "/inventory/includes/inventory_db.inc");

if (!@$_GET['popup'])
{
	$js = "";
	if ($use_popup_windows)
		$js .= get_js_open_window(900, 500);
	page(_($help_context = "Inventory Item Cost Update"), false, false, "", $js);
}

//--------------------------------------------------------------------------------------

check_db_has_costable_items(_("There are no costable inventory items defined in the system (Purchased or manufactured items)."));

if (isset($_GET['stock_id']))
{
	$_POST['stock_id'] = $_GET['stock_id'];
}

//--------------------------------------------------------------------------------------
if (isset($_POST['UpdateData']))
{

	$old_cost = get_standard_cost($_POST['stock_id']);
     
   	$new_cost = input_num('material_cost') + input_num('labour_cost')
	     + input_num('overhead_cost');

   	$should_update = true;

	if (!check_num('material_cost') || !check_num('labour_cost') ||
		!check_num('overhead_cost'))
	{
		display_error( _("The entered cost is not numeric."));
		set_focus('material_cost');
   	 	$should_update = false;
	}
	elseif ($old_cost == $new_cost)
	{
   	 	display_error( _("The new cost is the same as the old cost. Cost was not updated."));
   	 	$should_update = false;
	}

   	if ($should_update)
   	{
		$update_no = stock_cost_update($_POST['stock_id'],
		    input_num('material_cost'), input_num('labour_cost'),
		    input_num('overhead_cost'),	$old_cost);

        display_notification(_("Cost has been updated."));

        if ($update_no > 0)
        {
    		display_notification(get_gl_view_str(ST_COSTUPDATE, $update_no, _("View the GL Journal Entries for this Cost Update")));
        }

   	}
}

if (list_updated('stock_id'))
	$Ajax->activate('cost_table');
//-----------------------------------------------------------------------------------------

$action = $_SERVER['PHP_SELF'];
if (@$_GET['popup'])
	$action .= "?stock_id=".get_post('stock_id');
start_form(false, false, $action);

if (!isset($_POST['stock_id']))
	$_POST['stock_id'] = get_global_stock_item();

if (!@$_GET['popup'])
{
	echo "<center>" . _("Item:"). "&nbsp;";
	//echo stock_costable_items_list('stock_id', $_POST['stock_id'], false, true);
	echo stock_items_list('stock_id', $_POST['stock_id'], false, true);

	echo "</center><hr>";
}
else
	br(2);

set_global_stock_item($_POST['stock_id']);

$myrow = get_item($_POST['stock_id']);

div_start('cost_table');

start_table(TABLESTYLE2);
$dec1 = $dec2 = $dec3 = 0;
$_POST['material_cost'] = price_decimal_format($myrow["material_cost"], $dec1);
$_POST['labour_cost'] = price_decimal_format($myrow["labour_cost"], $dec2);
$_POST['overhead_cost'] = price_decimal_format($myrow["overhead_cost"], $dec3);

amount_row(_("Standard Material Cost Per Unit"), "material_cost", null, "class='tableheader2'", null, $dec1);

if (@$_GET['popup'])
{
	hidden('_tabs_sel', get_post('_tabs_sel'));
	hidden('popup', @$_GET['popup']);
}
if ($myrow["mb_flag"]=='M')
{
	amount_row(_("Standard Labour Cost Per Unit"), "labour_cost", null, "class='tableheader2'", null, $dec2);
	amount_row(_("Standard Overhead Cost Per Unit"), "overhead_cost", null, "class='tableheader2'", null, $dec3);
}
else
{
	hidden("labour_cost", 0);
	hidden("overhead_cost", 0);
}

end_table(1);
div_end();
submit_center('UpdateData', _("Update"), true, false, 'default');

end_form();
if (!@$_GET['popup'])
	end_page(@$_GET['popup'], false, false);

?>
