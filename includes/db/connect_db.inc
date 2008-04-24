<?php

set_global_connection();

function set_global_connection()
{
	global $db;

    if (isset($_SESSION["wa_current_user"]) && $_SESSION["wa_current_user"]->company !='')
    	$db = $_SESSION["wa_current_user"]->get_db_connection();
    else
    	$db = null;
}

$db_duplicate_error_code = 1062;

//DB wrapper functions to change only once for whole application

function db_query($sql, $err_msg=null)
{
	global $db, $show_sql;

	//echo "<br>$sql<br>";
	if ($show_sql)
	{
		echo "<font face=arial size=2 color=000099><b>SQL..</b></font>";
		echo "<pre>";
		echo $sql;
		echo "</pre>\n";
	}


	$result = mysql_query($sql, $db);
	if ($err_msg != null)
	  if (function_exists('xdebug_call_file'))
		check_db_error('<br>At file '.xdebug_call_file().':'.xdebug_call_line().':<br>'.$err_msg, $sql);
	  else
		check_db_error($err_msg, $sql);

	return $result;
}

function db_fetch_row ($result)
{

	return mysql_fetch_row($result);
}

function db_fetch ($result)
{

	return mysql_fetch_array($result);
}

function db_seek (&$result,$record)
{
	mysql_data_seek($result, $record);
}

function db_free_result ($result)
{
	if ($result)
		mysql_free_result($result);
}

function db_num_rows (&$result)
{
	return mysql_num_rows($result);
}

function db_num_fields ($result)
{
	return mysql_num_fields($result);
}

function db_escape($value = "", $nullify = false)
{
	$value = @htmlspecialchars($value, ENT_COMPAT, $_SESSION['language']->encoding);

  	//reset default if second parameter is skipped
	$nullify = ($nullify === null) ? (false) : ($nullify);
  	//undo slashes for poorly configured servers
	$value = (get_magic_quotes_gpc()) ? (stripslashes($value)) : ($value);

  	//check for null/unset/empty strings
	if ((!isset($value)) || (is_null($value)) || ($value === "")) {
		$value = ($nullify) ? ("NULL") : ("''");
	} else {
		if (is_string($value)) {
      		//value is a string and should be quoted; determine best method based on available extensions
			if (function_exists('mysql_real_escape_string')) {
		  		$value = "'" . mysql_real_escape_string($value) . "'";
			} else {
			  $value = "'" . mysql_escape_string($value) . "'";
			}
		} else if (!is_numeric($value)) {
			//value is not a string nor numeric
			display_error("ERROR: incorrect data type send to sql query");
			echo '<br><br>';
			exit();
		}
	}
	return $value;
}

function db_error_no ()
{
	global $db;
	return mysql_errno($db);
}

function db_error_msg($conn)
{
	return mysql_error($conn);
}

function db_insert_id()
{
	global $db;
	return mysql_insert_id($db);
}

function db_num_affected_rows()
{
	global $db;
	return mysql_affected_rows($db);
}

?>
