<?php

$page_security = 2;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("Inventory Item Where Used Inquiry"));

//include($path_to_root . "/includes/date_functions.inc");
include($path_to_root . "/includes/ui.inc");

check_db_has_stock_items(_("There are no items defined in the system."));

start_form(false, true);

if (!isset($_POST['stock_id']))
	$_POST['stock_id'] = get_global_stock_item();

echo "<center>" . _("Select an item to display its parent item(s).") . "&nbsp;";
stock_items_list('stock_id', $_POST['stock_id'], false, true);
echo "<hr></center>";

set_global_stock_item($_POST['stock_id']);

if (isset($_POST['stock_id']))
{
    $sql = "SELECT ".TB_PREF."bom.*,".TB_PREF."stock_master.description,".TB_PREF."workcentres.name As WorkCentreName, ".TB_PREF."locations.location_name
		FROM ".TB_PREF."bom, ".TB_PREF."stock_master, ".TB_PREF."workcentres, ".TB_PREF."locations
		WHERE ".TB_PREF."bom.parent = ".TB_PREF."stock_master.stock_id AND ".TB_PREF."bom.workcentre_added = ".TB_PREF."workcentres.id
		AND ".TB_PREF."bom.loc_code = ".TB_PREF."locations.loc_code
		AND ".TB_PREF."bom.component='" . $_POST['stock_id'] . "'";

    $result = db_query($sql,"No parent items were returned");

   	if (db_num_rows($result) == 0)
   	{
   		display_note(_("The selected item is not used in any BOMs."));
   	}
   	else
   	{

        start_table("$table_style width=80%");

        $th = array(_("Parent Item"), _("Work Centre"), _("Location"), _("Quantity Required"));
        table_header($th);

		$k = $j = 0;
        while ($myrow = db_fetch($result))
        {

			alt_table_row_color($k);

    		$select_item = $path_to_root . "/manufacturing/manage/bom_edit.php?" . SID . "stock_id=" . $myrow["parent"];

        	label_cell("<a href='$select_item'>" . $myrow["parent"]. " - " . $myrow["description"]. "</a>");
        	label_cell($myrow["WorkCentreName"]);
        	label_cell($myrow["location_name"]);
        	label_cell(qty_format($myrow["quantity"]));
			end_row();

        	$j++;
        	If ($j == 12)
        	{
        		$j = 1;
        		table_header($th);
        	}
        //end of page full new headings if
        }

        end_table();
   	}
}

end_form();
end_page();

?>