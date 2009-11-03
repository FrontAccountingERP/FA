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

function set_global_connection()
{
	global $db;

   	$db = $_SESSION["wa_current_user"]->get_db_connection();
}

$db_duplicate_error_code = 1062;

//DB wrapper functions to change only once for whole application

function db_query($sql, $err_msg=null)
{
	global $db, $show_sql, $sql_trail, $select_trail, $go_debug;
	
	if ($show_sql)
	{
		echo "<font face=arial size=2 color=000099><b>SQL..</b></font>";
		echo "<pre>";
		echo $sql;
		echo "</pre>\n";
	}
	

	$result = mysql_query($sql, $db);
	if($sql_trail) {
		if ($select_trail || (strstr($sql, 'SELECT') === false)) {
			mysql_query(
			"INSERT INTO ".TB_PREF."sql_trail
				(`sql`, `result`, `msg`)
				VALUES(".db_escape($sql).",".($result ? 1 : 0).",
				".db_escape($err_msg).")", $db);
		}
	}

	if ($err_msg != null || $go_debug) {
		$exit = $err_msg != null;
		if (function_exists('xdebug_call_file'))
			check_db_error('<br>At file '.xdebug_call_file().':'.xdebug_call_line().':<br>'.$err_msg, $sql, $exit);
		else
			check_db_error($err_msg, $sql, $exit);
	}
	return $result;
}

function db_fetch_row ($result)
{

	return mysql_fetch_row($result);
}

function db_fetch_assoc ($result)
{

	return mysql_fetch_assoc($result);
}

function db_fetch ($result)
{

	return mysql_fetch_array($result);
}

function db_seek (&$result,$record)
{
	return mysql_data_seek($result, $record);
}

function db_free_result ($result)
{
	if ($result)
		mysql_free_result($result);
}

function db_num_rows ($result)
{
	return mysql_num_rows($result);
}

function db_num_fields ($result)
{
	return mysql_num_fields($result);
}

function db_escape($value = "", $nullify = false)
{
	$value = @html_entity_decode($value, ENT_QUOTES, $_SESSION['language']->encoding);
	$value = @htmlspecialchars($value, ENT_QUOTES, $_SESSION['language']->encoding);

  	//reset default if second parameter is skipped
	$nullify = ($nullify === null) ? (false) : ($nullify);

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
