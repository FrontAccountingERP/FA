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
include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/banking.inc");
include_once($path_to_root . "/includes/manufacturing.inc");

include_once($path_to_root . "/inventory/includes/db/items_category_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_trans_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_prices_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_purchases_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_codes_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_locations_db.inc");
include_once($path_to_root . "/inventory/includes/db/movement_types_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_adjust_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_transfer_db.inc");
include_once($path_to_root . "/inventory/includes/db/items_units_db.inc");

function item_img_name($stock_id)
{
	return strtr($stock_id, "><\\/:|*?", '________');
}

?>