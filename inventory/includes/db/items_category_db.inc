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
function add_item_category($description)

{
	$sql = "INSERT INTO ".TB_PREF."stock_category (description)
		VALUES (".db_escape($description).")";

	db_query($sql,"an item category could not be added");
}

function update_item_category($ItemCategory, $description)

{
	$sql = "UPDATE ".TB_PREF."stock_category SET description = ".db_escape($description)."
        WHERE category_id = ".db_escape($ItemCategory);

	db_query($sql,"an item category could not be updated");
}

function delete_item_category($ItemCategory)
{
	$sql="DELETE FROM ".TB_PREF."stock_category WHERE category_id=".db_escape($ItemCategory);

	db_query($sql,"an item category could not be deleted");
}

function get_item_category($ItemCategory)
{
	$sql="SELECT * FROM ".TB_PREF."stock_category WHERE category_id=".db_escape($ItemCategory);

	$result = db_query($sql,"an item category could not be retrieved");

	return db_fetch($result);
}

function get_category_name($id)
{
	$sql = "SELECT description FROM ".TB_PREF."stock_category WHERE category_id=".db_escape($id);

	$result = db_query($sql, "could not get sales type");

	$row = db_fetch_row($result);
	return $row[0];
}

?>