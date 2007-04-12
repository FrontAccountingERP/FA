<?php

include_once($path_to_root . "/includes/db/references_db.inc");

class references 
{
	
	function save($type, $id, $reference) 
	{
		add_reference($type, $id, $reference);
		
		references::save_last($reference, $type);
	}
	
	function get($type, $id) 
	{
		return get_reference($type, $id);
	}	
	
	function delete($type, $id) 
	{
		delete_reference($type, $id);
	}	
	
	function exists($type, $reference) 
	{
		return (find_reference($type, $reference) != null);
	}
	
	function save_last($reference, $type) 
	{
		$next = references::increment($reference);
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
        if (is_numeric($reference))
        	return $reference + 1;
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