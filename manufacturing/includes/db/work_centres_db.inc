<?php

function add_work_centre($name, $description)
{
	$sql = "INSERT INTO ".TB_PREF."workcentres (name, description) 
		VALUES ('$name','$description')";
		
	db_query($sql, "could not add work centre");		
}

function update_work_centre($type_id, $name, $description)
{
	$sql = "UPDATE ".TB_PREF."workcentres SET name='$name', description='$description'
		WHERE id=$type_id";
	
	db_query($sql, "could not update work centre");			
}

function get_all_work_centres()
{
	$sql = "SELECT * FROM ".TB_PREF."workcentres";
	
	return db_query($sql, "could not get all work centres");
} 

function get_work_centre($type_id)
{
	$sql = "SELECT * FROM ".TB_PREF."workcentres WHERE id=$type_id";
	
	$result = db_query($sql, "could not get work centre");
	
	return db_fetch($result);
}

function delete_work_centre($type_id)
{
	$sql="DELETE FROM ".TB_PREF."workcentres WHERE id=$type_id";
		
	db_query($sql, "could not delete work centre");	
}

?>