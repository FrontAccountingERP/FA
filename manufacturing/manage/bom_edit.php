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

//--------------------------------------------------------------------------------------------------

if (isset($_GET["NewItem"]))
{
	$_POST['stock_id'] = $_GET["NewItem"];
}
if (isset($_GET['stock_id']))
{
	$_POST['stock_id'] = $_GET['stock_id'];
	$selected_parent =  $_GET['stock_id'];
}

/* selected_parent could come from a post or a get */
if (isset($_GET["selected_parent"]))
{
	$selected_parent = $_GET["selected_parent"];
}
else if (isset($_POST["selected_parent"]))
{
	$selected_parent = $_POST["selected_parent"];
}
/* selected_component could also come from a post or a get */
if (isset($_GET["selected_component"]))
{
	$selected_component = $_GET["selected_component"];
}
elseif (isset($_POST["selected_component"]))
{
	$selected_component = $_POST["selected_component"];
}


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
        label_cell(qty_format($myrow["quantity"]));
        label_cell($myrow["units"]);
        edit_link_cell(SID . "NewItem=$selected_parent&selected_component=" . $myrow["id"]);
        delete_link_cell(SID . "delete=" . $myrow["id"]. "&stock_id=" . $_POST['stock_id']);
        end_row();

	} //END WHILE LIST LOOP
	end_table();
}

//--------------------------------------------------------------------------------------------------

function on_submit($selected_parent, $selected_component=null)
{
	if (!check_num('quantity', 0))
	{
		display_error(_("The quantity entered must be numeric and greater than zero."));
		return;
	}

	if (isset($selected_parent) && isset($selected_component))
	{

		$sql = "UPDATE ".TB_PREF."bom SET workcentre_added='" . $_POST['workcentre_added'] . "',
			loc_code='" . $_POST['loc_code'] . "',
			quantity= " . input_num('quantity') . "
			WHERE parent='" . $selected_parent . "'
			AND id='" . $selected_component . "'";
		check_db_error("Could not update this bom component", $sql);

		db_query($sql,"could not update bom");

	}
	elseif (!isset($selected_component) && isset($selected_parent))
	{

		/*Selected component is null cos no item selected on first time round 
		so must be adding a record must be Submitting new entries in the new 
		component form */

		//need to check not recursive bom component of itself!
		If (!check_for_recursive_bom($selected_parent, $_POST['component']))
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

				//$msg = _("A new component part has been added to the bill of material for this item.");

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

if (isset($_GET['delete']))
{

	$sql = "DELETE FROM ".TB_PREF."bom WHERE id='" . $_GET['delete']. "'";
	db_query($sql,"Could not delete this bom components");

	display_note(_("The component item has been deleted from this bom."));

}

//--------------------------------------------------------------------------------------------------

start_form(false, true);
//echo $msg;

echo "<center>" . _("Select a manufacturable item:") . "&nbsp;";
stock_bom_items_list('stock_id', null, false, true);

end_form();

//--------------------------------------------------------------------------------------------------

if (isset($_POST['stock_id']))
{ //Parent Item selected so display bom or edit component
	$selected_parent = $_POST['stock_id'];
	if (isset($selected_parent) && isset($_POST['Submit'])) {
	  if(isset($selected_component))
		on_submit($selected_parent, $selected_component);
	  else
		on_submit($selected_parent);
	}
	//--------------------------------------------------------------------------------------

	display_bom_items($selected_parent);

	if (isset($selected_parent) && isset($selected_component))
	{
		hyperlink_params($_SERVER['PHP_SELF'], _("Add a new Component"), "NewItem=$selected_parent");
	}

	//--------------------------------------------------------------------------------------

	start_form(false, true, $_SERVER['PHP_SELF'] . "?" . SID . "NewItem=" . $selected_parent);

	start_table($table_style2);

	if (isset($selected_component))
	{
		//editing a selected component from the link to the line item
		$sql = "SELECT ".TB_PREF."bom.*,".TB_PREF."stock_master.description FROM ".TB_PREF."bom,".TB_PREF."stock_master
			WHERE id='$selected_component'
			AND ".TB_PREF."stock_master.stock_id=".TB_PREF."bom.component";

		$result = db_query($sql, "could not get bom");
		$myrow = db_fetch($result);

		$_POST['loc_code'] = $myrow["loc_code"];
		$_POST['workcentre_added']  = $myrow["workcentre_added"];
		$_POST['quantity'] = qty_format($myrow["quantity"]);

		hidden('selected_parent', $selected_parent);
		hidden('selected_component', $selected_component);
		label_row(_("Component:"), $myrow["component"] . " - " . $myrow["description"]);

	}
	else
	{ //end of if $selected_component

		hidden('selected_parent', $selected_parent);

		start_row();
		label_cell(_("Component:"));

		echo "<td>";
		stock_component_items_list('component', $selected_parent, $_POST['component'], false, true);
		echo "</td>";
		end_row();
	}

	locations_list_row(_("Location to Draw From:"), 'loc_code', null);
	workcenter_list_row(_("Work Centre Added:"), 'workcentre_added', null);

	if (!isset($_POST['quantity']))
	{
		$_POST['quantity'] = qty_format(1);
	}
	amount_row(_("Quantity:"), 'quantity', $_POST['quantity']);

	end_table(1);
	submit_center('Submit', _("Add/Update"));

	end_form();
}

// ----------------------------------------------------------------------------------

end_page();

?>
