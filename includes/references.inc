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
include_once($path_to_root . "/includes/db/references_db.inc");

class references 
{
	
	function save($type, $id, $reference) 
	{
		add_reference($type, $id, $reference);
		if ($reference != 'auto')
			$this->save_last($reference, $type);
	}
	
	function get($type, $id) 
	{
		return get_reference($type, $id);
	}	
	
	function delete($type, $id) 
	{
		delete_reference($type, $id);
	}	
	
	function update($type, $id, $reference) 
	{
	    update_reference($type, $id, $reference);
		if ($reference != 'auto')
			$this->save_last($reference, $type);
	}
	// check if reference is used for any non voided transaction (used for ST_JOURNALENTRY type)
	function exists($type, $reference) 
	{
		return (find_reference($type, $reference) != null);
	}
	
	function save_last($reference, $type) 
	{
		$next = $this->increment($reference);
		save_next_reference($type, $next);
	}
	
	function get_next($type) 
	{
		return get_next_reference($type);
	}
	//
	//	Restore previous reference if voided trans ref was the last one.
	//
	function restore_last($type, $id) 
	{
		$reference = $this->get($type, $id);
		$last = $this->increment($this->get_next($type), true); //decrement
		if ($reference==$last) {
			save_next_reference($type, $last);
		}
	}
	
	//------------------------------------

	function is_valid($reference) 
	{
		return strlen(trim($reference)) > 0;
	}
	//
	//	Increments (or decrements if $back==true) reference template
	//
	function increment($reference, $back=false) 
	{
		// New method done by Pete. So f.i. WA036 will increment to WA037 and so on.
       	// If $reference contains at least one group of digits,
        // extract first didgits group and add 1, then put all together.
        // NB. preg_match returns 1 if the regex matches completely 
        // also $result[0] holds entire string, 1 the first captured, 2 the 2nd etc.
        //
        if (preg_match('/^(\D*?)(\d+)(.*)/', $reference, $result) == 1) 
        {
			list($all, $prefix, $number, $postfix) = $result;
			$dig_count = strlen($number); // How many digits? eg. 0003 = 4
			$fmt = '%0' . $dig_count . 'd'; // Make a format string - leading zeroes
			$val = intval($number + ($back ? ($number<1 ? 0 : -1) : 1));
			$nextval =  sprintf($fmt, $val); // Add one on, and put prefix back on

			return $prefix.$nextval.$postfix;
        }
        else 
            return $reference;
	}
	//------------------------------------
}

//----------------------------------------------------------------------------

function is_new_reference($ref, $type)
{
	$db_info = get_systype_db_info($type);
	$db_name = $db_info[0];
	$db_type = $db_info[1];
	$db_trans = $db_info[2];
	$db_ref = $db_info[3];
	
	if ($db_ref != null) 
	{
		if ($db_type != null) {
			$sql = "SELECT $db_ref FROM $db_name 
				LEFT JOIN ".TB_PREF."voided v ON 
					$db_name.$db_type=v.type AND $db_name.$db_trans=v.id
				WHERE $db_name.$db_ref='$ref' AND ISNULL(v.id)
					AND $db_name.$db_type=$type";
		} else {
			$sql = "SELECT $db_ref FROM $db_name 
				LEFT JOIN ".TB_PREF."voided v ON 
					v.type=$type AND $db_name.$db_trans=v.id
				WHERE $db_ref='$ref' AND ISNULL(v.id)";
		}
		$result = db_query($sql, "could not test for unique reference");
		
		return (db_num_rows($result) == 0);
	}
	
	// it's a type that doesn't use references - shouldn't be calling here, but say yes anyways
	return true;
}


?>