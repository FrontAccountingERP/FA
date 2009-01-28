<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU Affero General Public License,
	AGPL, as published by the Free Software Foundation, either version 
	3 of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/agpl-3.0.html>.
***********************************************************************/
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

?>
