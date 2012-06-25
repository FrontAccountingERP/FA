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

/*
	Converts encoding name to mysql standard.
*/
function get_mysql_encoding_name($encoding)
{
	$db_encoding = array(
		'UTF-8' => 'utf8',
		'ISO-8859-1' => 'latin1',
		'ISO-8859-2' => 'latin2',
		'ISO-8859-7' => 'greek',
		'ISO-8859-8' => 'hebrew',
		'ISO-8859-9' => 'latin5',
		'ISO-8859-13' => 'latin7',
		'KOI8-R' => 'koi8r',
		'KOI8-U' => 'koi8u',
		'CP850' => 'cp850',
		'CP866' => 'cp866',
		'CP932' => 'cp932',
		'CP1250' => 'cp1250',
		'CP1251' => 'cp1251',
		'CP1252' => 'latin1',
		'CP1256' => 'cp1256',
		'CP1257' => 'cp1257',
		'GB2312' => 'gb2312',
		'EUC-JP' => 'ujis',
		'EUC-KR' => 'euckr',
		'BIG5' => 'big5',
		'GBK' => 'gbk',
		'SHIFT_JIS' => 'sjis',
		'TIS-620' => 'tis620',
		'ASCII' => 'ascii',
	);
	$encoding = strtoupper($encoding);

	return isset($db_encoding[$encoding]) ? $db_encoding[$encoding] : null;
}

/*
	Returns 'best' collation for various locale language codes
*/
function get_mysql_collation($lang=null)
{
	if (!$lang)
		$lang = substr($_SESSION['language']->code, 0, 2);

	$db_collation = array(
		'is' => 'icelandic',
		'lv' => 'latvian',
		'ro' => 'romanian',
		'sl' => 'slovenian',
		'pl' => 'polish',
		'et' => 'estonian',
		'es' => 'spanish', // or 'spanish2',
		'sw' => 'swedish',
		'tr' => 'turkish',
		'cs' => 'czech',
		'da' => 'danish',
		'lt' => 'lithuanian',
		'sk' => 'slovak',
		'sp' => 'spanish2',
		'fa' => 'persian',
		'hu' => 'hungarian',
		'fr' => 'roman',
		'it' => 'roman',
	);

	return 'utf8_'.(isset($db_collation[$lang]) ? $db_collation[$lang] : 'general').'_ci';
}
/*
	Later we assume that database with version less than 2.4 is old database, 
	which is subject to invalid encodings on text columns,
	so no SET NAMES or equivalent should be used.
*/
function db_fixed()
{
	$result = db_query("SELECT value FROM ".TB_PREF."sys_prefs WHERE name='version_id'");
	$data = db_fetch($result);
	return !db_num_rows($result) // new database is fixed by default 
		|| ($data[0] > "2.3rc");
}
/*
	Check database default charset.
*/
function db_get_charset()
{
	$result = db_query("SELECT @@character_set_database");
	$var = db_fetch($result);
	return $var[0];
}

/*
	Set mysql client encoding.
	Default is is encoding used by default language.
*/
function db_set_encoding($ui_encoding=null)
{
	global $dflt_lang, $installed_languages;

	if (!isset($ui_encoding))
	{
		$lang = array_search_value($dflt_lang, $installed_languages, 'code');
		$ui_encoding = strtoupper($lang['encoding']);
	}

	if ($mysql_enc = get_mysql_encoding_name($ui_encoding))
		mysql_set_charset($mysql_enc);
}

/*
	Connects application to company database.
*/
function set_global_connection($company=-1)
{
	global $db, $transaction_level, $path_to_root;
	global  $db_connections;

	include ($path_to_root . "/config_db.php");
	if ($company == -1) 
		$company = $_SESSION["wa_current_user"]->company ? $_SESSION["wa_current_user"]->company : 0;

	cancel_transaction(); // cancel all aborted transactions if any
	$transaction_level = 0;

	$_SESSION["wa_current_user"]->cur_con = $company;

	$connection = $db_connections[$company];

	$db = mysql_connect($connection["host"], $connection["dbuser"], $connection["dbpassword"]);
		mysql_select_db($connection["dbname"], $db);

	return $db;
}

$db_duplicate_error_code = 1062;

function db_query($sql, $err_msg=null)
{
	global $db, $show_sql, $sql_trail, $select_trail, $go_debug, $sql_queries, $Ajax,
		$db_connections, $db_last_inserted_id;
	
	// set current db prefix
	$comp = isset($_SESSION["wa_current_user"]->cur_con) ? $_SESSION["wa_current_user"]->cur_con : 0;
	$cur_prefix = $db_connections[$comp]['tbpref'];
	$sql = str_replace(TB_PREF, $cur_prefix, $sql);

	if ($show_sql)
	{
		$Ajax->activate('footer_debug');
		$sql_queries .= "<pre>$sql</pre>\n<hr>";
	}

	// mysql profiling
	global $profile_sql;
	if (@$profile_sql) get_usec();
		$result = mysql_query($sql, $db);
	if (@$profile_sql)
	{
		$profile_sql= false;
		_vd($sql.'<br>:'.db_num_rows($result).'rows, '.get_usec());
	}

	if($sql_trail) {
		$db_last_inserted_id = mysql_insert_id($db);	// preserve in case trail insert is done
		if ($select_trail || (strstr($sql, 'SELECT') === false)) {
			mysql_query(
			"INSERT INTO ".$cur_prefix."sql_trail
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
	global $db_last_inserted_id, $sql_trail, $db;

	return $sql_trail ? $db_last_inserted_id : mysql_insert_id($db);
}

function db_num_affected_rows()
{
	global $db;
	return mysql_affected_rows($db);
}

function db_field_name($result, $n)
{
	return mysql_field_name($result, $n);
}

function db_create_db($connection)
{
	$db = mysql_connect($connection["host"] ,
		$connection["dbuser"], $connection["dbpassword"]);
	if (!mysql_select_db($connection["dbname"], $db))
	{
		$sql = "CREATE DATABASE " . $connection["dbname"] . " COLLATE ".get_mysql_collation();

		if (!mysql_query($sql) || !mysql_select_db($connection["dbname"], $db))
			return 0;
	}
	return $db;
}

function db_drop_db($connection)
{

	if ($connection["tbpref"] == "")
	{
		$sql = "DROP DATABASE " . $connection["dbname"] . "";
		return mysql_query($sql);
	}
	else
	{
    	$res = db_query("show table status");
    	$all_tables = array();
    	while($row = db_fetch($res))
    		$all_tables[] = $row;
        // get table structures
		foreach ($all_tables as $table)
		{
			if (strpos($table['Name'], $connection["tbpref"]) === 0)
				db_query("DROP TABLE `".$table['Name'] . "`");
		}
		//deleting the tables, how??
		return true;
	}
}

function db_close($dbase = null)
{
	global $db;
	
	if (!$dbase)
		$dbase = $db;
	return mysql_close($dbase);
}
