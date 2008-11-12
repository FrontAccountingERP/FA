<?php

$page_security = 2;
$path_to_root="../..";
include($path_to_root . "/includes/db_pager.inc");
include($path_to_root . "/includes/session.inc");

page(_("Inventory Item Where Used Inquiry"));

include($path_to_root . "/includes/ui.inc");

check_db_has_stock_items(_("There are no items defined in the system."));

start_form(false, true);

if (!isset($_POST['stock_id']))
	$_POST['stock_id'] = get_global_stock_item();

echo "<center>" . _("Select an item to display its parent item(s).") . "&nbsp;";
stock_items_list('stock_id', $_POST['stock_id'], false, true);
echo "<hr></center>";

set_global_stock_item($_POST['stock_id']);
//-----------------------------------------------------------------------------
function select_link($row)
{
	return  pager_link( $row["parent"]. " - " . $row["description"],
    		"/manufacturing/manage/bom_edit.php?stock_id=" . $row["parent"]);
}

$sql = "SELECT 
		bom.parent,
		workcentre.name As WorkCentreName,
		location.location_name,
		bom.quantity,
		parent.description
		FROM ".TB_PREF."bom as bom, "
			.TB_PREF."stock_master as parent, "
			.TB_PREF."workcentres as workcentre, "
			.TB_PREF."locations as location
		WHERE bom.parent = parent.stock_id 
			AND bom.workcentre_added = workcentre.id
			AND bom.loc_code = location.loc_code
			AND bom.component='" . $_POST['stock_id'] . "'";

   $cols = array(
   	_("Parent Item") => array('fun'=>'select_link'), 
	_("Work Centre"), 
	_("Location"), 
	_("Quantity Required")
	);

$table =& new_db_pager('usage_table', $sql, $cols);

if (list_updated('stock_id')) {
	$table->set_sql($sql);
	$table->set_columns($cols);
	$Ajax->activate('usage_table');
}

display_db_pager($table);

end_form();
end_page();

?>