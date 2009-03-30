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
if (list_updated('stock_id'))
		$Ajax->activate('_page_body');

start_form();
    start_table("class='tablestyle_noborder'");
	start_row();
	echo '<td>';
	stock_bom_items_list('stock_id', null, false, true);
	echo '</td>';
	end_table();
	
	echo "<hr>";

	display_heading(_("All Costs Are In:") . " " . get_company_currency());
	display_bom($_POST['stock_id']);

end_form();

end_page();
?>
