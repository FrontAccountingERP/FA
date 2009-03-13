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
$page_security = 2;
$path_to_root="../..";
include_once($path_to_root . "/includes/session.inc");

page(_("Costed Bill Of Material Inquiry"));

include_once($path_to_root . "/manufacturing/includes/manufacturing_ui.inc");
include_once($path_to_root . "/includes/manufacturing.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/banking.inc");
include_once($path_to_root . "/includes/data_checks.inc");

check_db_has_bom_stock_items(_("There are no manufactured or kit items defined in the system."));

if (isset($_GET['stock_id']))
{
	$_POST['stock_id'] = $_GET['stock_id'];
} 

start_form(false, true);
echo "<center> " . _("Item:") . " ";	
//stock_bom_items_list('stock_id', $_POST['stock_id'], false, true);
stock_bom_items_list('stock_id', null, false, true);

echo "<hr>";

if ($_POST['stock_id'] != "")
{
	echo "<center>" . _("All Costs Are In:") . " " . get_company_currency() . "<br><br>";
	display_bom($_POST['stock_id']);
} 
else 
{ //no item entered
	echo "<p>" ._("Enter an item code above, to view the costed bill of material for.") . "</p>";
}	
end_form();

end_page();
?>
