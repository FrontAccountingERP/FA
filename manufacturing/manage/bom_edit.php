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
$page_security = 'SA_BOM';
$path_to_root = "../..";

include_once($path_to_root . "/includes/session.inc");

$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);

page(_($help_context = "Bill Of Materials"), @$_REQUEST['popup'], false, "", $js);

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/includes/manufacturing.inc");

// Add inventory database library. We need this to load the item data.
include_once($path_to_root . "/inventory/includes/db/items_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_locations_db.inc");

check_db_has_bom_stock_items(_("There are no manufactured or kit items defined in the system."));

check_db_has_workcentres(_("There are no work centres defined in the system. BOMs require at least one work centre be defined."));

simple_page_mode(true);
$selected_component = $selected_id;
//--------------------------------------------------------------------------------------------------

//if (isset($_GET["NewItem"]))
//{
//	$_POST['stock_id'] = $_GET["NewItem"];
//}
if (isset($_GET['stock_id']))
{
	$_POST['stock_id'] = $_GET['stock_id'];
	$selected_parent =  $_GET['stock_id'];
}

// Check if we should store BOM in session temporarily.
// This is very usefull when adding new item and its BOM.
if (isset($_GET['use_session']))
{
	$_SESSION['bom_session']['flag'] = 1;
}

/* selected_parent could come from a post or a get */
/*if (isset($_GET["selected_parent"]))
{
	$selected_parent = $_GET["selected_parent"];
}
else if (isset($_POST["selected_parent"]))
{
	$selected_parent = $_POST["selected_parent"];
}
*/
/* selected_component could also come from a post or a get */
/*if (isset($_GET["selected_component"]))
{
	$selected_component = $_GET["selected_component"];
}
else
{
	$selected_component = get_post("selected_component", -1);
}
*/

//--------------------------------------------------------------------------------------------------

function display_bom_items($selected_parent)
{
	$result = get_bom($selected_parent);
	div_start('bom');
	start_table(TABLESTYLE, "width='60%'");
	$th = array(_("Code"), _("Description"), _("Location"),
		_("Work Centre"), _("Quantity"), _("Units"),'','');
	table_header($th);

	$k = 0;

	// Check if we will read BOM from session or database.
	if (isset($_SESSION['bom_session']['flag'])) {
		if (isset($_SESSION['bom_session']['list'])) {
			foreach ($_SESSION['bom_session']['list'] as $myrow)
			{
				alt_table_row_color($k);

				label_cell($myrow["component"]);
				label_cell($myrow["description"]);
				label_cell($myrow["location_name"]);
				label_cell($myrow["WorkCentreDescription"]);
				qty_cell($myrow["quantity"], false, get_qty_dec($myrow["component"]));
				label_cell($myrow["units"]);
				edit_button_cell("Edit".$myrow['component'], _("Edit"));
				delete_button_cell("Delete".$myrow['component'], _("Delete"));
				end_row();
			} // END FOREACH LOOP
		}
	}
	else {
		while ($myrow = db_fetch($result))
		{

			alt_table_row_color($k);

			label_cell($myrow["component"]);
			label_cell($myrow["description"]);
			label_cell($myrow["location_name"]);
			label_cell($myrow["WorkCentreDescription"]);
			qty_cell($myrow["quantity"], false, get_qty_dec($myrow["component"]));
			label_cell($myrow["units"]);
			edit_button_cell("Edit".$myrow['id'], _("Edit"));
			delete_button_cell("Delete".$myrow['id'], _("Delete"));
			end_row();

		} //END WHILE LIST LOOP
	}
	end_table();
	div_end();
}

//--------------------------------------------------------------------------------------------------

function on_submit($selected_parent, $selected_component=-1)
{
	if (!check_num('quantity', 0))
	{
		display_error(_("The quantity entered must be numeric and greater than zero."));
		set_focus('quantity');
		return;
	}

	if ($selected_component != -1)
	{
		// Check if we will update BOM in session or database.
		if (isset($_SESSION['bom_session']['flag'])) {
			$row1 = get_item($selected_component);
			$row2 = get_work_centre($_POST['workcentre_added']);
			$row3 = get_item_location($_POST['loc_code']);
			$_SESSION['bom_session']['list'][$selected_component] = array(
				'component' => $selected_component,
				'description' => $row1['description'],
				'workcentre_added' => $_POST['workcentre_added'],
				'WorkCentreDescription' => $row2['name'],
				'location' => $_POST['loc_code'],
				'location_name' => $row3['location_name'],
				'quantity' => input_num('quantity'),
				'units' => $row1['units'],
			);
		}
		else {
			update_bom($selected_parent, $selected_component, $_POST['workcentre_added'], $_POST['loc_code'],
				input_num('quantity'));
		}
		display_notification(_('Selected component has been updated'));
		$Mode = 'RESET';
	}
	else
	{

		/*Selected component is null cos no item selected on first time round
		so must be adding a record must be Submitting new entries in the new
		component form */

		//need to check not recursive bom component of itself!
		if (!check_for_recursive_bom($selected_parent, $_POST['component']))
		{

			/*Now check to see that the component is not already on the bom */
			if (!is_component_already_on_bom($_POST['component'], $_POST['workcentre_added'],
				$_POST['loc_code'], $selected_parent))
			{
				// Check if we will insert BOM in session or database.
				if (isset($_SESSION['bom_session']['flag'])) {
					$row1 = get_item($_POST['component']);
					$row2 = get_work_centre($_POST['workcentre_added']);
					$row3 = get_item_location($_POST['loc_code']);
					$_SESSION['bom_session']['list'][$_POST['component']] = array(
						'component' => $_POST['component'],
						'description' => $row1['description'],
						'workcentre_added' => $_POST['workcentre_added'],
						'WorkCentreDescription' => $row2['name'],
						'location' => $_POST['loc_code'],
						'location_name' => $row3['location_name'],
						'quantity' => input_num('quantity'),
						'units' => $row1['units'],
					);
				}
				else {
					add_bom($selected_parent, $_POST['component'], $_POST['workcentre_added'],
						$_POST['loc_code'], input_num('quantity'));
				}
				display_notification(_("A new component part has been added to the bill of material for this item."));
				$Mode = 'RESET';
			}
			else
			{
				/*The component must already be on the bom */
				display_error(_("The selected component is already on this bom. You can modify it's quantity but it cannot appear more than once on the same bom."));
			}

		} //end of if its not a recursive bom
		else
		{
			display_error(_("The selected component is a parent of the current item. Recursive BOMs are not allowed."));
		}
	}
}

//--------------------------------------------------------------------------------------------------

if ($Mode == 'Delete')
{
	// Check if we will delete BOM from session or database.
	if (isset($_SESSION['bom_session']['flag']) && isset($_SESSION['bom_session']['list'])) {
		unset($_SESSION['bom_session']['list'][$selected_id]);
	}
	else {
		delete_bom($selected_id);
	}

	display_notification(_("The component item has been deleted from this bom"));
	$Mode = 'RESET';
}

if ($Mode == 'RESET')
{
	$selected_id = -1;
	unset($_POST['quantity']);
}

//--------------------------------------------------------------------------------------------------

start_form();

start_form(false, true);
start_table(TABLESTYLE_NOBORDER);
start_row();

// If we use session for storing BOM, don't display the list for choosing manufacturable items.
// Because that means the parent item is not existed yet.
if (!isset($_SESSION['bom_session']['flag'])) {
	stock_manufactured_items_list_cells(_("Select a manufacturable item:"), 'stock_id', null, false, true);
}

end_row();
if (list_updated('stock_id'))
	$Ajax->activate('_page_body');
end_table();
br();

end_form();
//--------------------------------------------------------------------------------------------------

if (get_post('stock_id') != '' || isset($_SESSION['bom_session']['flag']))
{ //Parent Item selected so display bom or edit component
	$selected_parent = $_POST['stock_id'];
	if ($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM')
		on_submit($selected_parent, $selected_id);
	//--------------------------------------------------------------------------------------

	start_form();
	display_bom_items($selected_parent);
	//--------------------------------------------------------------------------------------
	echo '<br>';

	start_table(TABLESTYLE2);

	if ($selected_id != -1)
	{
 		if ($Mode == 'Edit') {
			//editing a selected component from the link to the line item

			// Check if we will load the BOM from session or database.
			if (isset($_SESSION['bom_session']['flag']) && isset($_SESSION['bom_session']['list'][$selected_id])) {
				$myrow = $_SESSION['bom_session']['list'][$selected_id];
			}
			else {
				$myrow = get_component_from_bom($selected_id);
			}

			$_POST['loc_code'] = $myrow["loc_code"];
			$_POST['component'] = $myrow["component"]; // by Tom Moulton
			$_POST['workcentre_added']  = $myrow["workcentre_added"];
			$_POST['quantity'] = number_format2($myrow["quantity"], get_qty_dec($myrow["component"]));
			label_row(_("Component:"), $myrow["component"] . " - " . $myrow["description"]);
		}
		hidden('selected_id', $selected_id);
	}
	else
	{
		start_row();
		label_cell(_("Component:"), "class='label'");

		echo "<td>";
		echo stock_component_items_list('component', $selected_parent, null, false, true);
		if (get_post('_component_update'))
		{
			$Ajax->activate('quantity');
		}
		echo "</td>";
		end_row();
	}
	hidden('stock_id', $selected_parent);

	locations_list_row(_("Location to Draw From:"), 'loc_code', null);
	workcenter_list_row(_("Work Centre Added:"), 'workcentre_added', null);
	$dec = get_qty_dec(get_post('component'));
	$_POST['quantity'] = number_format2(input_num('quantity',1), $dec);
	qty_row(_("Quantity:"), 'quantity', null, null, null, $dec);

	end_table(1);
	submit_add_or_update_center($selected_id == -1, '', 'both');
	end_form();
}
// ----------------------------------------------------------------------------------

end_page();
