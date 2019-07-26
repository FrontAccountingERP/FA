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
//
//	General database functions common for all modules.
//
//-------------------------------------------------------------------
// Multilevel transaction control.
//
$transaction_level = 0;
$transaction_data = null;

function begin_transaction($func=null, $args=null)
{
	global $transaction_level, $transaction_data, $SysPrefs;

	if (!$transaction_level) {
		if ($SysPrefs->db_trail) {
			$bt = debug_backtrace();
			$user = $_SESSION["wa_current_user"]->user;
			$transaction_data = array(
				'user' => $_SESSION["wa_current_user"]->user,
				'msg'=> basename($bt[count($bt)-1]['file']).'@'.$bt[count($bt)-1]['line'], // call start point
				'entry' => $func,
				'data' => serialize($args),
			);
		}
		db_query("BEGIN", "could not start a transaction");
	}
	$transaction_level++;
}

function commit_transaction()
{
	global $transaction_level, $transaction_data, $SysPrefs, $db_last_inserted_id, $db;

	$transaction_level--;

	if (!$transaction_level) {
		// FIXME: if logged to table remember to preserve last_insert_id!
		if ($SysPrefs->db_trail == 1) {
			$last_insert = $db_last_inserted_id;
			$db_last_inserted_id = mysqli_insert_id($db);	// preserve in case trail insert is done
			$sql = "INSERT INTO ".TB_PREF."db_trail (user, msg, entry, data) VALUES ("
					."'".$transaction_data['user']."','".$transaction_data['msg']."','".$transaction_data['entry']."',"
					.db_escape($transaction_data['data']).")";
			db_query($sql, 'cannot log user operation');
			$db_last_inserted_id = $last_insert;
		} elseif ($SysPrefs->db_trail == 2)
			error_log($transaction_data['msg'].'|'.$transaction_data['user'].'|'.$transaction_data['entry'].'|'.$transaction_data['data']);
		db_query("COMMIT", "could not commit a transaction");
	}
}

/*
	This function is called on end of script execution to cancel
	all aborted transactions (if any)
*/
function cancel_transaction()
{
	global $transaction_level;

	if ($transaction_level) {
		db_query("ROLLBACK", "could not cancel a transaction");	
	}
	$transaction_level = 0;
}

//-----------------------------------------------------------------------------
//	Update record activity status.
//
function update_record_status($id, $status, $table, $key) {
	$sql = "UPDATE ".TB_PREF.$table." SET inactive = "
		. ((int)$status)." WHERE $key=".db_escape($id);
		
  	db_query($sql, "Can't update record status");
}
//-----------------------------------------------------------------------------
//
//	Helper for sql subquery returning running totals from delta tables like stock_moves or bank_trans
//
//	$table - table name with optional WHERE clause
//  $column - delta column
//	$index  - comma delimited list of columns for total grouping and order
//  Returns running totals with respective index column
//
function running_total_sql($table, $column, $index)
{

	return "SELECT daily.$index, daily.$column, (@total:=@total+daily.$column) total 
		FROM
			(SELECT $index, sum($column) $column FROM $table GROUP BY $index ORDER BY $index) daily,
			(SELECT @total:=0) total_var";
}

