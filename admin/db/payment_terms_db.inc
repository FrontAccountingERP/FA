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

function add_payment_terms($terms, $type, $days=0, $early_discount=0, $early_days=0)
{
	begin_transaction(__FUNCTION__, func_get_args());

	$sql = "INSERT INTO ".TB_PREF."payment_terms (terms, type, days, early_discount, early_days)
		VALUES (" .db_escape($terms) . "," . db_escape($type) . ", ".db_escape($days). ", ".db_escape($early_discount). ", ".db_escape($early_days).")";

   	db_query($sql,"The payment term could not be added");

	commit_transaction();
}

function update_payment_terms($selected_id, $terms, $type, $days=0, $early_discount=0, $early_days=0)
{
	begin_transaction(__FUNCTION__, func_get_args());

	$sql = "UPDATE ".TB_PREF."payment_terms SET terms=" . db_escape($terms) . ",
		type=".db_escape($type).",
		days=" . db_escape($days).",
		early_discount=".db_escape($early_discount).",
		early_days=".db_escape($early_days).
		" WHERE id = " .db_escape($selected_id);

   	db_query($sql,"The payment term could not be updated");

	commit_transaction();
}

function delete_payment_terms($selected_id)
{
	begin_transaction(__FUNCTION__, func_get_args());

	$sql="DELETE FROM ".TB_PREF."payment_terms WHERE id=".db_escape($selected_id);
	db_query($sql,"could not delete a payment terms");

	commit_transaction();
}

function get_payment_terms($selected_id)
{
	$sql = "SELECT * FROM ".TB_PREF."payment_terms t WHERE id=".db_escape($selected_id);

	$result = db_query($sql,"could not get payment term");

	return db_fetch($result);
}

function get_payment_terms_all($show_inactive)
{
	$sql = "SELECT * FROM ".TB_PREF."payment_terms";
	if (!$show_inactive) $sql .= " WHERE !inactive";
	return db_query($sql,"could not get payment terms");
}

/*
	Calculate due date using terms data provided either as table of payment data or payment terms id
*/
function get_payment_due_date($terms_data, $date)
{
	if (!is_array($terms_data))
		$terms_data = get_payment_terms($terms_data);

	if (!is_date($date))
		$date = new_doc_date();
	
	if (!$terms_data)
		return $date;

	if ($terms_data['type'] == PTT_FOLLOWING) {
		$end = end_month(add_months(begin_month($date), 1));
		$duedate = add_days(end_month($date), $terms_data['days']);
		if (date1_greater_date2($duedate, $end))
			$duedate = $end;
	} elseif ($terms_data['type'] == PTT_DAYS)
		$duedate = add_days($date, $terms_data['days']);
	else
		$duedate = $date;
	return $duedate;
}
