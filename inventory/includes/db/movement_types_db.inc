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
function add_movement_type($name)
{
	$sql = "INSERT INTO ".TB_PREF."movement_types (name)
		VALUES (".db_escape($name).")";

	db_query($sql, "could not add item movement type");
}

function update_movement_type($type_id, $name)
{
	$sql = "UPDATE ".TB_PREF."movement_types SET name=".db_escape($name)."
			WHERE id=".db_escape($type_id);

	db_query($sql, "could not update item movement type");
}

function get_all_movement_type($all=false)
{
	$sql = "SELECT * FROM ".TB_PREF."movement_types";
	if (!$all) $sql .= " WHERE !inactive";

	return db_query($sql, "could not get all item movement type");
}

function get_movement_type($type_id)
{
	$sql = "SELECT * FROM ".TB_PREF."movement_types WHERE id=".db_escape($type_id);

	$result = db_query($sql, "could not get item movement type");

	return db_fetch($result);
}

function delete_movement_type($type_id)
{
	$sql="DELETE FROM ".TB_PREF."movement_types WHERE id=".db_escape($type_id);

	db_query($sql, "could not delete item movement type");
}

function get_stock_movements($stock_id, $StockLocation,	$BeforeDate, $AfterDate)
{
	$before_date = date2sql($BeforeDate);
	$after_date = date2sql($AfterDate);
  $sql = "SELECT type, trans_no, tran_date, person_id, qty, reference";

	if(!$StockLocation) {
		 $sql .= ", loc_code";
	}
  $sql.=    " FROM ".TB_PREF."stock_moves
		WHERE";

  if ($StockLocation) {
    $sql.= " loc_code=".db_escape($StockLocation)." AND";
	}

	$sql.= " tran_date >= '". $after_date . "'
	AND tran_date <= '" . $before_date . "'
	AND stock_id = ".db_escape($stock_id) . " ORDER BY tran_date,trans_id";

  return db_query($sql, "could not query stock moves");
}

function get_stock_movements_before($stock_id, $StockLocation, $AfterDate)
{
	$after_date = date2sql($AfterDate);
	$sql = "SELECT SUM(qty) FROM ".TB_PREF."stock_moves WHERE stock_id=".db_escape($stock_id);
	if ($StockLocation)
		$sql .= " AND loc_code=".db_escape( $StockLocation);
	$sql .= " AND tran_date < '" . $after_date . "'";
	$before_qty = db_query($sql, "The starting quantity on hand could not be calculated");

	$before_qty_row = db_fetch_row($before_qty);
	return $before_qty_row[0];
}

function movement_types_in_stock_moves($selected_id)
{
	$sql= "SELECT COUNT(*) FROM ".TB_PREF."stock_moves 
		WHERE type=" . ST_INVADJUST. " AND person_id=".db_escape($selected_id);

	$result = db_query($sql, "could not query stock moves");
	$myrow = db_fetch_row($result);
	return ($myrow[0] > 0); 
}
?>
