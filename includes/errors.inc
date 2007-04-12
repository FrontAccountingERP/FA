<?php

function display_db_error($msg, $sql_statement=null, $exit=true)
{
	global $db, $debug;
	$db_error = db_error_no();
	
	echo "<span class='errortext'><b>" . _("DATABASE ERROR :") . "</b> $msg</span><br>";
	
	if ($db_error != 0) 
	{
		echo "error code : " . $db_error . "<br>";
		echo "error message : " . db_error_msg($db) . "<br>";
	}
	
	if ($debug == 1) 
	{
		echo "sql that failed was : " . $sql_statement . "<br>";
	}
	
	echo "<br><br>";
	
	if ($exit)
		exit;
}

function frindly_db_error($db_error)
{
	global $db_duplicate_error_code;
	
	if ($db_error == $db_duplicate_error_code) 
	{
		display_error(_("The entered information is a duplicate. Please go back and enter different values.")
		. "<br><a href='javascript:history.go(-1)'>" . _("Back") . "</a>", true);
		return true;
	}
	
	return false;
}

function check_db_error($msg, $sql_statement, $exit_if_error=true, $rollback_if_error=true)
{
	global $db;
	$db_error = db_error_no();
	
	if ($db_error != 0) 
	{
		
		if (!frindly_db_error($db_error))
			display_db_error($msg, $sql_statement, false);
		
		if ($rollback_if_error) 
		{
		  $rollback_result = db_query("rollback","could not rollback");			
		}
		
		if ($exit_if_error) 
		{
			echo "<br><br>";
			exit;
		}
	}
	return $db_error;		
}

?>