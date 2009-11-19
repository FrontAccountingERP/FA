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
function begin_transaction()
{
	db_query("BEGIN", "could not start a transaction");
}

function commit_transaction()
{
	db_query("COMMIT", "could not commit a transaction");
}

function cancel_transaction()
{
	db_query("ROLLBACK", "could not cancel a transaction");	
}
//-----------------------------------------------------------------------------
//	Update record activity status.
//
function update_record_status($id, $status, $table, $key) {
	$sql = "UPDATE ".TB_PREF.$table." SET inactive = "
		. db_escape($status)." WHERE $key=".db_escape($id);
		
  	db_query($sql, "Can't update record status");
}
?>
