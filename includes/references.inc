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
	
	//------------------------------------

	function is_valid($reference) 
	{
		return strlen(trim($reference)) > 0;
	}
	
	function increment($reference) 
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
			$nextval =  sprintf($fmt, intval($number + 1)); // Add one on, and put prefix back on

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
	$db_ref = $db_info[3];
	
	if ($db_ref != null) 
	{
		$sql = "SELECT $db_ref FROM $db_name WHERE $db_ref='$ref'";
		if ($db_type != null)
			$sql .= " AND $db_type=$type";
			 
		$result = db_query($sql, "could not test for unique reference");
		
		return (db_num_rows($result) == 0);
	}
	
	// it's a type that doesn't use references - shouldn't be calling here, but say yes anyways
	return true;
}

?>