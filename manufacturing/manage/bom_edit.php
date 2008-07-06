<?php

$page_security = 9;
$path_to_root="../..";
include_once($path_to_root . "/includes/session.inc");

page(_("Bill Of Materials"));

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/includes/manufacturing.inc");

check_db_has_bom_stock_items(_("There are no manufactured or kit items defined in the system."));

check_db_has_workcentres(_("There are no work centres defined in the system. BOMs require at least one work centre be defined."));

simple_page_mode(true);
$selected_component = $selected_id;
//--------------------------------------------------------------------------------------------------

//if (isset($_GET["NewItem"]))
//{
//	$_POST['stock_id'] = $_GET["NewItem"];
//}
//if (isset($_GET['stock_id']))
//{
//	$_POST['stock_id'] = $_GET['stock_id'];
//	$selected_parent =  $_GET['stock_id'];
//}

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

function check_for_recursive_bom($ultimate_parent, $component_to_check)
{

	/* returns true ie 1 if the bom contains the parent part as a component
	ie the bom is recursive otherwise false ie 0 */

	$sql = "SELECT component FROM ".TB_PREF."bom WHERE parent='$component_to_check'";
	$result = db_query($sql,"could not check recursive bom");

	if ($result != 0)
	{
		while ($myrow = db_fetch_row($result))
		{
			if ($myrow[0] == $ultimate_parent)
			{
				return 1;
			}

			if (check_for_recursive_bom($ultimate_parent, $myrow[0]))
			{
				return 1;
			}
		} //(while loop)
	} //end if $result is true

	return 0;

} //end of function check_for_recursive_bom

//--------------------------------------------------------------------------------------------------

function display_bom_items($selected_parent)
{
	global $table_style;

	$result = get_bom($selected_parent);
div_start('bom');
	start_table("$table_style width=60%");
	$th = array(_("Code"), _("Description"), _("Location"),
		_("Work Centre"), _("Quantity"), _("Units"),'','');
	table_header($th);

	$k = 0;
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
 		edit_button_cell("Delete".$myrow['id'], _("Delete"));
        end_row();

	} //END WHILE LIST LOOP
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

		$sql = "UPDATE ".TB_PREF."bom SET workcentre_added='" . $_POST['workcentre_added'] . "',
			loc_code='" . $_POST['loc_code'] . "',
			quantity= " . input_num('quantity') . "
			WHERE parent='" . $selected_parent . "'
			AND id='" . $selected_component . "'";
		check_db_error("Could not update this bom component", $sql);

		db_query($sql,"could not update bom");
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
			$sql = "SELECT component FROM ".TB_PREF."bom
				WHERE parent='$selected_parent'
				AND component='" . $_POST['component'] . "'
				AND workcentre_added='" . $_POST['workcentre_added'] . "'
				AND loc_code='" . $_POST['loc_code'] . "'" ;
			$result = db_query($sql,"check failed");

			if (db_num_rows($result) == 0)
			{
				$sql = "INSERT INTO ".TB_PREF."bom (parent, component, workcentre_added, loc_code, quantity)
					VALUES ('$selected_parent', '" . $_POST['component'] . "', '"
					. $_POST['workcentre_added'] . "', '" . $_POST['loc_code'] . "', "
					. input_num('quantity') . ")";

				db_query($sql,"check failed");
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
	$sql = "DELETE FROM ".TB_PREF."bom WHERE id='" . $selected_component. "'";
	db_query($sql,"Could not delete this bom components");

	display_notification(_("The component item has been deleted from this bom"));
	$Mode = 'RESET';
}

if ($Mode == 'RESET')
{
	$selected_component = -1;
	unset($_POST['quantity']);
}

//--------------------------------------------------------------------------------------------------

start_form(false, true);

echo "<center>" . _("Select a manufacturable item:") . "&nbsp;";
stock_bom_items_list('selected_parent', null, false, true);
echo "</center><br>";

end_form();
if (isset($_POST['_selected_parent_update']))
	$Ajax->activate('_page_body');
//--------------------------------------------------------------------------------------------------

if (get_post('selected_parent') != '')
{ //Parent Item selected so display bom or edit component
	$selected_parent = $_POST['selected_parent'];
	if ($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM')
		on_submit($selected_parent, $selected_component);
	//--------------------------------------------------------------------------------------

start_form();
	display_bom_items($selected_parent);
//end_form();
	//--------------------------------------------------------------------------------------
	echo '<br>';
//	start_form(false, true);

	start_table($table_style2);

	if ($selected_component != -1)
	{
 		if ($Mode == 'Edit') {
			//editing a selected component from the link to the line item
			$sql = "SELECT ".TB_PREF."bom.*,".TB_PREF."stock_master.description FROM ".TB_PREF."bom,".TB_PREF."stock_master
				WHERE id='$selected_component'
				AND ".TB_PREF."stock_master.stock_id=".TB_PREF."bom.component";

			$result = db_query($sql, "could not get bom");
			$myrow = db_fetch($result);

			$_POST['loc_code'] = $myrow["loc_code"];
			$_POST['workcentre_added']  = $myrow["workcentre_added"];
			$_POST['quantity'] = number_format2($myrow["quantity"], get_qty_dec($myrow["component"]));
		}
		hidden('component', $selected_component);
		label_row(_("Component:"), $myrow["component"] . " - " . $myrow["description"]);
	}
	else
	{
		start_row();
		label_cell(_("Component:"));

		echo "<td>";
		stock_component_items_list('component', $selected_parent, null, false, true);
		if (get_post('_component_update')) 
		{
			$Ajax->activate('quantity');
		}
		echo "</td>";
		end_row();
	}
	hidden('selected_parent', $selected_parent);

	locations_list_row(_("Location to Draw From:"), 'loc_code', null);
	workcenter_list_row(_("Work Centre Added:"), 'workcentre_added', null);
	$dec = get_qty_dec(get_post('component'));
	$_POST['quantity'] = number_format2(input_num('quantity',1), $dec);
	qty_row(_("Quantity:"), 'quantity', null, null, null, $dec);

	end_table(1);
	submit_add_or_update_center($selected_component == -1, '', true);
	end_form();
}
// ----------------------------------------------------------------------------------

end_page();

?>
