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
function add_credit_status($description, $disallow_invoicing)
{
	$sql = "INSERT INTO ".TB_PREF."credit_status (reason_description, dissallow_invoices) 
		VALUES (".db_escape($description).",$disallow_invoicing)";
		
	db_query($sql, "could not add credit status");		
}

function update_credit_status($status_id, $description, $disallow_invoicing)
{
	$sql = "UPDATE ".TB_PREF."credit_status SET reason_description=".db_escape($description).",
		dissallow_invoices=$disallow_invoicing WHERE id=$status_id";
	
	db_query($sql, "could not update credit status");			
}

function get_all_credit_status()
{
	$sql = "SELECT * FROM ".TB_PREF."credit_status";
	
	return db_query($sql, "could not get all credit status");
} 

function get_credit_status($status_id)
{
	$sql = "SELECT * FROM ".TB_PREF."credit_status WHERE id=$status_id";
	
	$result = db_query($sql, "could not get credit status");
	
	return db_fetch($result);
}

function delete_credit_status($status_id)
{
	$sql="DELETE FROM ".TB_PREF."credit_status WHERE id=$status_id";
		
	db_query($sql, "could not delete credit status");	
}

?>