<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU General Public License,
	GPL, as published by the Free Software Foundation, either version 
	3 of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/gpl-3.0.html>.
***********************************************************************/

/**
 * @return Returns the array sorted as required
 * @param $aryData Array containing data to sort
 * @param $strIndex name of column to use as an index
 * @param $strSortBy Column to sort the array by
 * @param $strSortType String containing either asc or desc [default to asc]
 * @desc Naturally sorts an array using by the column $strSortBy
 */
function array_natsort($aryData, $strIndex, $strSortBy, $strSortType=false)
{
   //    if the parameters are invalid
   if (!is_array($aryData) || !$strSortBy)
       //    return the array
       return $aryData;

   //    create our temporary arrays
   $arySort = $aryResult = array();

   //    loop through the array
   foreach ($aryData as $key => $aryRow)
       //    set up the value in the array
       $arySort[$strIndex ? $aryRow[$strIndex] : $key] = $aryRow[$strSortBy];

   //    apply the natural sort
   natsort($arySort);

   //    if the sort type is descending
   if ($strSortType=="desc")
       //    reverse the array
       arsort($arySort);

   //    loop through the sorted and original data
		foreach ($arySort as $arySortKey => $arySorted)
			if($strIndex) 
			{
				foreach ($aryData as $aryOriginal)
				// if the key matches
					if ($aryOriginal[$strIndex]==$arySortKey)
						// add it to the output array
						array_push($aryResult, $aryOriginal);
			} else
				$aryResult[$arySortKey] = $aryData[$arySortKey];
   //    return the return
   return $aryResult;
}


function write_config_db($new = false)
{
	global $path_to_root, $def_coy, $db_connections, $tb_pref_counter;

	if ($new)
		$tb_pref_counter++;
	$n = count($db_connections);
	$msg = "<?php\n\n";
	$msg .= "/*Connection Information for the database\n";
	$msg .= "- \$def_coy is the default company that is pre-selected on login\n\n";
	$msg .= "- host is the computer ip address or name where the database is the default is localhost assuming that the web server is also the sql server\n\n";
	$msg .= "- user is the user name under which the database should be accessed - need to change to the mysql (or other DB) user set up for purpose\n";
	$msg .= "  NB it is not secure to use root as the user with no password - a user with appropriate privileges must be set up\n\n";
	$msg .= "- password is the password the user of the database requires to be sent to authorise the above database user\n\n";
	$msg .= "- DatabaseName is the name of the database as defined in the RDMS being used. Typically RDMS allow many databases to be maintained under the same server.\n";
	$msg .= "  The scripts for MySQL provided use the name logicworks */\n\n\n";

	$msg .= "\$def_coy = " . $def_coy . ";\n\n";
	$msg .= "\$tb_pref_counter = " . $tb_pref_counter . ";\n\n";
	$msg .= "\$db_connections = array (\n";
	$msg .= "\t0 => ";
	for ($i = 0; $i < $n; $i++)
	{
		if ($i > 0)
			$msg .= "\tarray ";
		else
			$msg .= "array ";
		$msg .= "('name' => '" . $db_connections[$i]['name'] . "',\n";
		$msg .= "\t\t'host' => '" . $db_connections[$i]['host'] . "',\n";
		$msg .= "\t\t'dbuser' => '" . $db_connections[$i]['dbuser'] . "',\n";
		$msg .= "\t\t'dbpassword' => '" . $db_connections[$i]['dbpassword'] . "',\n";
		$msg .= "\t\t'dbname' => '" . $db_connections[$i]['dbname'] . "',\n";
		$msg .= "\t\t'tbpref' => '" . $db_connections[$i]['tbpref'] . "')";
		if ($i != $n - 1)
			$msg .= ",";
		$msg .= "\n\n";
	}
	$msg .= "\t);\n?>";

	$filename = $path_to_root . "/config_db.php";
	// Check if the file exists and is writable first.
	if ((!file_exists($filename) && is_writable($path_to_root)) || is_writable($filename))
	{
		if (!$zp = fopen($filename, 'w'))
		{
			return -1;
		}
		else
		{
			if (!fwrite($zp, $msg))
			{
				fclose($zp);
				return -2;
			}
			// Close file
			fclose($zp);
		}
	}
	else
	{
		return -3;
	}
	return 0;
}

function write_extensions($extensions=null, $company = -1)
{
	global $path_to_root, $installed_extensions, $next_extension_id;

	if (!isset($extensions)) {
		$extensions = $installed_extensions;
	}
	if (!isset($next_extension_id)) {
		$next_extension_id = 1;
	}
//	$exts = array_natsort($extensions, 'name', 'name');
//	$extensions = $exts;

	$msg = "<?php\n\n";
	if ($company == -1)
		$msg .=
"/* List of installed additional modules and plugins. If adding extensions manually 
	to the list make sure they have unique, so far not used extension_ids as a keys,
	and \$next_extension_id is also updated.
	
	'name' - name for identification purposes;
	'type' - type of extension: 'module' or 'plugin'
	'path' - FA root based installation path
	'filename' - name of module menu file, or plugin filename; related to path.
	'tab' - index of the module tab (new for module, or one of standard module names for plugin);
	'title' - is the menu text (for plugin) or new tab name
	'active' - current status of extension
	'acc_file' - (optional) file name with \$security_areas/\$security_sections extensions; 
		related to 'path'
	'access' - security area code in string form
*/
\n\$next_extension_id = $next_extension_id; // unique id for next installed extension\n\n";
	else 
		$msg .=
"/*
	Do not edit this file manually. This copy of global file is overwritten
	by extensions editor.
*/\n\n";

	$msg .= "\$installed_extensions = array (\n";
	foreach($extensions as $i => $ext) 
	{
		$msg .= "\t$i => ";
		$msg .= "array ( ";
		$t = '';
		foreach($ext as $key => $val) {
			$msg .= $t."'$key' => '$val',\n";
			$t = "\t\t\t";
		}
		$msg .= "\t\t),\n";
	}
	$msg .= "\t);\n?>";

	$filename = $path_to_root . ($company==-1 ? '' : '/company/'.$company)
		.'/installed_extensions.php';

	// Check if the file is writable first.
	if (!$zp = @fopen($filename, 'w'))
	{
		display_error(sprintf(_("Cannot open the extension setup file '%s' for writing."),
			 $filename));
		return false;
	}
	else
	{
		if (!fwrite($zp, $msg))
		{
			display_error(sprintf(_("Cannot write to the extensions setup file '%s'."),
				$filename));
			fclose($zp);
			return false;
		}
		// Close file
		fclose($zp);
	}
	return true;
}

function write_lang()
{
	global $path_to_root, $installed_languages, $dflt_lang;

	$conn = array_natsort($installed_languages, 'code', 'code');
	$installed_languages = $conn;
	//reset($installed_languages);
	$n = count($installed_languages);
	$msg = "<?php\n\n";

	$msg .= "/* How to make new entries here\n\n";
	$msg .= "-- if adding languages at the beginning of the list, make sure it's index is set to 0 (it has ' 0 => ')\n";
	$msg .= "-- 'code' should match the name of the directory for the language under \\lang\n";
	$msg .= "-- 'name' is the name that will be displayed in the language selection list (in Users and Display Setup)\n";
	$msg .= "-- 'rtl' only needs to be set for right-to-left languages like Arabic and Hebrew\n\n";
	$msg .= "*/\n\n\n";

	$msg .= "\$installed_languages = array (\n";
	if ($n > 0)
	    $msg .= "\t0 => ";
	for ($i = 0; $i < $n; $i++)
	{
		if ($i > 0)
			$msg .= "\t\tarray ";
		else
			$msg .= "array ";
		$msg .= "('code' => '" . $installed_languages[$i]['code'] . "', ";
		$msg .= "'name' => '" . $installed_languages[$i]['name'] . "', ";
		$msg .= "'encoding' => '" . $installed_languages[$i]['encoding'] . "'";
		if (isset($installed_languages[$i]['rtl']) && $installed_languages[$i]['rtl'])
			$msg .= ", 'rtl' => true),\n";
		else
			$msg .= "),\n";
	}

	$msg .= "\t);\n";
	$msg .= "\n\$dflt_lang = '$dflt_lang';\n?>\n";

	$path = $path_to_root . "/lang";
	$filename = $path.'/installed_languages.inc';
	// Check if directory exists and is writable first.
	if (file_exists($path) && is_writable($path))
	{
		if (!$zp = fopen($filename, 'w'))
		{
			display_error(_("Cannot open the languages file - ") . $filename);
			return false;
		}
		else
		{
			if (!fwrite($zp, $msg))
			{
				display_error(_("Cannot write to the language file - ") . $filename);
				fclose($zp);
				return false;
			}
			// Close file
			fclose($zp);
		}
	}
	else
	{
		display_error(_("The language files folder ") . $path . _(" is not writable. Change its permissions so it is, then re-run the operation."));
		return false;
	}
	return true;
}

function db_create_db($connection)
{
	$db = mysql_connect($connection["host"] ,
		$connection["dbuser"], $connection["dbpassword"]);
	if (!mysql_select_db($connection["dbname"], $db))
	{
		$sql = "CREATE DATABASE " . $connection["dbname"] . "";
		if (!mysql_query($sql))
			return 0;
		mysql_select_db($connection["dbname"], $db);
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

function db_import($filename, $connection, $force=true)
{
	global $db, $go_debug;
	
	$allowed_commands = array(
		"create"  => 'table_queries', 
		"alter table" => 'table_queries', 
		"insert" => 'data_queries', 
		"update" => 'data_queries', 
		"drop table if exists" => 'drop_queries');
	$ignored_mysql_errors = array( //errors ignored in normal (non forced) mode
		'1022',	// duplicate key
		'1050', // Table %s already exists
		'1060', // duplicate column name
		'1061', // duplicate key name
		'1062', // duplicate key entry
		'1091'  // can't drop key/column check if exists
	);
	$data_queries = array();
	$drop_queries = array();
	$table_queries = array();
	$sql_errors = array();

	ini_set("max_execution_time", "180");
	db_query("SET foreign_key_checks=0");

	// uncrompress gziped backup files
	if (strpos($filename, ".gz") || strpos($filename, ".GZ"))
		$lines = db_ungzip("lines", $filename);
	elseif (strpos($filename, ".zip") || strpos($filename, ".ZIP"))
		$lines = db_unzip("lines", $filename);
	else
		$lines = file("". $filename);

	// parse input file
	$query_table = '';
	foreach($lines as $line_no => $line)
	{
		$line = trim($line);
		
		$line = str_replace("0_", $connection["tbpref"], $line);

		if ($query_table == '') 
		{	// check if line begins with one of allowed queries
		 	foreach($allowed_commands as $cmd => $table) 
			{
				if (strtolower(substr($line, 0, strlen($cmd))) == $cmd) 
				{
					$query_table = $table;
					${$query_table}[] = array('', $line_no+1);
					break;
				}
		 	}
		 }
		 if($query_table != '')  // inside allowed query
		 {
		 	$table = $query_table;
			if (substr($line, -1) == ';') // end of query found
			{
				$line = substr($line, 0, strlen($line) - 1); // strip ';'
				$query_table = '';
			}
			${$table}[count(${$table}) - 1][0] .= $line . "\n";
		}
		
	}
/*
	{ 	// for debugging purposes
	global $path_to_root;
	$f = fopen($path_to_root.'/tmp/dbimport.txt', 'w+');
	fwrite($f, print_r($drop_queries,true) ."\n");
	fwrite($f, print_r($table_queries,true) ."\n");
	fwrite($f, print_r($data_queries,true));
	fclose($f);
	}
*/
	// execute drop tables if exists queries
	if (is_array($drop_queries))
	{
		foreach($drop_queries as $drop_query)
		{
			if (!db_query($drop_query[0]))
			{
				if (!in_array(db_error_no(), $ignored_mysql_errors) || !$force)
					$sql_errors[] = array(db_error_msg($db), $drop_query[1]);
			}
		}
	}

	// execute create tables queries
	if (is_array($table_queries))
	{
		foreach($table_queries as $table_query)
		{
			if (!db_query($table_query[0]))
			{	
				if (!in_array(db_error_no(), $ignored_mysql_errors) || !$force) {
					$sql_errors[] = array(db_error_msg($db), $table_query[1]);
				}
			}
		}
	}

	// execute insert data queries
	if (is_array($data_queries))
	{
		foreach($data_queries as $data_query)
		{
			if (!db_query($data_query[0]))
			{
				if (!in_array(db_error_no(),$ignored_mysql_errors) || !$force)
					$sql_errors[] = array(db_error_msg($db), $data_query[1]);
			}
		}
	}
	
	db_query("SET foreign_key_checks=1");

	if (count($sql_errors)) {
		// display first failure message; the rest are probably derivative 
		$err = $sql_errors[0];
		display_error(sprintf(_("SQL script execution failed in line %d: %s"),
			$err[1], $err[0]));
		return false;
	} else
		return true;
	//$shell_command = C_MYSQL_PATH . " -h $host -u $user -p{$password} $dbname < $filename";
	//shell_exec($shell_command);
}

// returns the content of the gziped $path backup file. use of $mode see below
function db_ungzip($mode, $path)
{
    $file_data = gzfile($path);
    // returns one string or an array of lines
    if ($mode != "lines")
    	return implode("",$file_data);
    else
    	return $file_data;
}

// returns the content of the ziped $path backup file. use of $mode see below
function db_unzip($mode, $path)
{
    $all = false;
    $all = implode("", file($path));

    // convert path to name of ziped file
    $filename = preg_replace("/.*\//", "", $path);
    $filename = substr($filename, 0, strlen($filename) - 4);

    // compare filname in zip and filename from $_GET
    if (substr($all, 30, strlen($filename)-4) . substr($all, 30+strlen($filename)+9, 4)
	  != $filename) {
		return '';     // exit if names differ
    }
    else
    {
    	// get the suffix of the filename in hex
		$crc_bugfix = substr($all, 30, strlen($filename)+13);
        $crc_bugfix = substr(substr($crc_bugfix, 0, strlen($crc_bugfix) - 4), 
				strlen($crc_bugfix) - 12 - 4);
        $suffix = false;
        // convert hex to ascii
        for ($i=0; $i < 12; )
        	$suffix .= chr($crc_bugfix[$i++] . $crc_bugfix[$i++] . $crc_bugfix[$i++]);

        // remove central directory information (we have always just one ziped file)
        $comp = substr($all, -(strlen($all) - 30 - strlen($filename)-13));
        $comp = substr($comp, 0, (strlen($comp) - 80 - strlen($filename)-13));

        // fix the crc bugfix (see function save_to_file)
        $comp = "xœ" . $comp . $suffix;
        $file_data = gzuncompress($comp);
    }

    // returns one string or an array of lines
    if ($mode != "lines")
    	return $file_data;
    else
    	return explode("\n", $file_data);
}

function db_backup($conn, $ext='no', $comm='', $tbpref = TB_PREF)
{
	if ($conn['tbpref'] != "")
		$filename = $conn['dbname'] . "_" . $conn['tbpref'] . date("Ymd_Hi") . ".sql";
	else
		$filename = $conn['dbname'] . "_" . date("Ymd_Hi") . ".sql";

	return db_export($conn, $filename, $ext, $comm, $tbpref);
}

// generates a dump of $db database
// $drop and $zip tell if to include the drop table statement or dry to pack
function db_export($conn, $filename, $zip='no', $comment='', $tbpref = TB_PREF)
{

	global $app_title, $version, $power_url, $path_to_root;

    $error = false;
    // set max string size before writing to file
    $max_size = 1048576 * 2; // 2 MB
    // changes max size if value can be retrieved
    if (ini_get("memory_limit"))
    	$max_size = 900000 * ini_get("memory_limit");

    // set backupfile name
    if ($zip == "gzip")
    	$backupfile = $filename . ".gz";
    elseif ($zip == "zip")
    	$backupfile = $filename . ".zip";
    else
    	$backupfile = $filename;
    $company = get_company_pref('coy_name', $tbpref);

    //create comment
    $out="# MySQL dump of database '".$conn["dbname"]."' on host '".$conn["host"]."'\n";
    $out.="# Backup Date and Time: ".date("Y-m-d H:i")."\n";
    $out.="# Built by " . $app_title . " " . $version ."\n";
    $out.="# ".$power_url."\n";
    $out.="# Company: ". @html_entity_decode($company, ENT_QUOTES, $_SESSION['language']->encoding)."\n";
    $out.="# User: ".$_SESSION["wa_current_user"]->name."\n\n";

	// write users comment
	if ($comment)
	{
		$out .= "# Comment:\n";
		$comment=preg_replace("'\n'","\n# ","# ".$comment);
		//$comment=str_replace("\n", "\n# ", $comment);
		foreach(explode("\n",$comment) as $line)
			$out .= $line."\n";
		$out.="\n";
	}

    //$out.="use ".$db.";\n"; we don't use this option.

    // get auto_increment values and names of all tables
    $res = db_query("show table status");
    $all_tables = array();
    while($row = db_fetch($res))
    {
		//if ($conn["tbpref"] == "" || strpos($row['Name'], $conn["tbpref"]) !== false) replaced
		if (($conn["tbpref"] == "" && !preg_match('/[0-9]+_/', $row['Name'])) ||
			($conn["tbpref"] != "" && strpos($row['Name'], $conn["tbpref"]) === 0))
    		$all_tables[] = $row;
    }
        // get table structures
	foreach ($all_tables as $table)
	{
		$res1 = db_query("SHOW CREATE TABLE `" . $table['Name'] . "`");
		$tmp = db_fetch($res1);
		$table_sql[$table['Name']] = $tmp["Create Table"];
	}

	// find foreign keys
	$fks = array();
	if (isset($table_sql))
	{
		foreach($table_sql as $tablenme=>$table)
		{
			$tmp_table=$table;
			// save all tables, needed for creating this table in $fks
			while (($ref_pos = strpos($tmp_table, " REFERENCES ")) > 0)
			{
				$tmp_table = substr($tmp_table, $ref_pos + 12);
				$ref_pos = strpos($tmp_table, "(");
				$fks[$tablenme][] = substr($tmp_table, 0, $ref_pos);
			}
		}
	}
	// order $all_tables
	$all_tables = order_sql_tables($all_tables, $fks);

	// as long as no error occurred
	if (!$error)
	{
		//while($row=@mysql_fetch_array($res))
		foreach ($all_tables as $row)
		{
			$tablename = $row['Name'];
			$auto_incr[$tablename] = $row['Auto_increment'];

			$out.="\n\n";
			// export tables
			$out.="### Structure of table `".$tablename."` ###\n\n";

			$out.="DROP TABLE IF EXISTS `".$tablename."`;\n\n";
			$out.=$table_sql[$tablename];

			// add auto_increment value
			if ($auto_incr[$tablename])
				$out.=" AUTO_INCREMENT=".$auto_incr[$tablename];
			$out.=" ;";
			$out.="\n\n\n";

			// export data
			if (!$error)
			{
				$out.="### Data of table `".$tablename."` ###\n\n";

				// check if field types are NULL or NOT NULL
				$res3 = db_query("SHOW COLUMNS FROM `" . $tablename . "`");

				$field_null = array();
				for ($j = 0; $j < db_num_rows($res3); $j++)
				{
					$row3 = db_fetch($res3);
					$field_null[] = $row3[2]=='YES' && $row3[4]===null;
				}

				$res2 = db_query("SELECT * FROM `" . $tablename . "`");
				for ($j = 0; $j < db_num_rows($res2); $j++)
				{
					$out .= "INSERT INTO `" . $tablename . "` VALUES (";
					$row2 = db_fetch_row($res2);
					// run through each field
					for ($k = 0; $k < $nf = db_num_fields($res2); $k++)
					{
						$out .= db_escape($row2[$k], $field_null[$k]);
						if ($k < ($nf - 1))
							$out .= ", ";
					}
					$out .= ");\n";

					// if saving is successful, then empty $out, else set error flag
					if (strlen($out) > $max_size && $zip != "zip")
					{
						if (save_to_file($backupfile, $zip, $out))
							$out = "";
						else
							$error = true;
					}
				}

			// an error occurred! Try to delete file and return error status
			}
			elseif ($error)
			{
				@unlink(BACKUP_PATH . $backupfile);
				return false;
			}

			// if saving is successful, then empty $out, else set error flag
			if (strlen($out) > $max_size && $zip != "zip")
			{
				if (save_to_file($backupfile, $zip, $out))
					$out= "";
				else
					$error = true;
			}
		}

	// an error occurred! Try to delete file and return error status
	}
	else
	{
		@unlink(BACKUP_PATH . $backupfile);
		return false;
	}

	// if (mysql_error()) return "DB_ERROR";
	//@mysql_close($con);

	//if ($zip == "zip")
	//	$zip = $time;
	if (save_to_file($backupfile, $zip, $out))
	{
		$out = "";
	}
	else
	{
		@unlink(BACKUP_PATH . $backupfile);
		return false;
	}
    return $backupfile;
}

// orders the tables in $tables according to the constraints in $fks
// $fks musst be filled like this: $fks[tablename][0]=needed_table1; $fks[tablename][1]=needed_table2; ...
function order_sql_tables($tables, $fks)
{
	// do not order if no contraints exist
	if (!count($fks))
		return $tables;

	// order
	$new_tables = array();
	$existing = array();
	$modified = true;
	while (count($tables) && $modified == true)
	{
	 	$modified = false;
	    foreach ($tables as $key=>$row)
	    {
	        // delete from $tables and add to $new_tables
	        if (isset($fks[$row['Name']]))
	        {
	        	foreach($fks[$row['Name']] as $needed)
	        	{
	            	// go to next table if not all needed tables exist in $existing
	                if (!in_array($needed,$existing))
	                	continue 2;
	            }
	        }
            // delete from $tables and add to $new_tables
	        $existing[] = $row['Name'];
			$new_tables[] = $row;
            prev($tables);
            unset($tables[$key]);
            $modified = true;

	    }
	}

	if (count($tables))
	{
	    // probably there are 'circles' in the constraints, bacause of that no proper backups can be created yet
	    // TODO: this will be fixed sometime later through using 'alter table' commands to add the constraints after generating the tables
	    // until now, just add the lasting tables to $new_tables, return them and print a warning
	    foreach($tables as $row)
	    	$new_tables[] = $row;
	    echo "<div class=\"red_left\">THIS DATABASE SEEMS TO CONTAIN 'RING CONSTRAINTS'. WA DOES NOT SUPPORT THEM. PROBABLY THE FOLOWING BACKUP IS DEFECT!</div>";
	}
	return $new_tables;
}

// saves the string in $fileData to the file $backupfile as gz file or not ($zip)
// returns backup file name if name has changed (zip), else TRUE. If saving failed, return value is FALSE
function save_to_file($backupfile, $zip, $fileData)
{
	global $path_to_root;

    if ($zip == "gzip")
    {
        if ($zp = @gzopen(BACKUP_PATH . $backupfile, "a9"))
        {
			@gzwrite($zp, $fileData);
			@gzclose($zp);
			return true;
        }
        else
        {
        	return false;
        }

    // $zip contains the timestamp
    }
    elseif ($zip == "zip")
    {
        // based on zip.lib.php 2.2 from phpMyBackupAdmin
        // offical zip format: http://www.pkware.com/appnote.txt

        // End of central directory record
        $eof_ctrl_dir = "\x50\x4b\x05\x06\x00\x00\x00\x00";

        // "local file header" segment
        $unc_len = strlen($fileData);
        $crc = crc32($fileData);
        $zdata = gzcompress($fileData);

		// extend stored file name with suffix
        // needed for decoding (because of crc bug)
        $name_suffix = substr($zdata, -4, 4);
        $name_suffix2 = "_";
        for ($i = 0; $i < 4; $i++)
        	$name_suffix2 .= sprintf("%03d", ord($name_suffix[$i]));

      	$name = substr($backupfile, 0, strlen($backupfile) - 8) . $name_suffix2 . ".sql";

        // fix crc bug
        $zdata = substr(substr($zdata, 0, strlen($zdata) - 4), 2);
        $c_len = strlen($zdata);

        // dos time
        $timearray = getdate($zip);
        $dostime = (($timearray['year'] - 1980) << 25) | ($timearray['mon'] << 21) | ($timearray['mday'] << 16) |
            ($timearray['hours'] << 11) | ($timearray['minutes'] << 5) | ($timearray['seconds'] >> 1);
        $dtime = dechex($dostime);
        $hexdtime = "\x" . $dtime[6] . $dtime[7] . "\x" . $dtime[4].$dtime[5] . "\x" . $dtime[2] . $dtime[3] . "\x" . $dtime[0] . $dtime[1];
        eval('$hexdtime="' . $hexdtime . '";');

        // ver needed to extract, gen purpose bit flag, compression method, last mod time and date
        $sub1 = "\x14\x00" . "\x00\x00" . "\x08\x00" . $hexdtime;

        // crc32, compressed filesize, uncompressed filesize
        $sub2 = pack('V', $crc) . pack('V', $c_len) . pack('V', $unc_len);

        $fr = "\x50\x4b\x03\x04" . $sub1. $sub2;

        // length of filename, extra field length
        $fr .= pack('v', strlen($name)) . pack('v', 0);
        $fr .= $name;

        // "file data" segment and "data descriptor" segment (optional but necessary if archive is not served as file)
        $fr .= $zdata . $sub2;

        // now add to central directory record
        $cdrec = "\x50\x4b\x01\x02";
        $cdrec .= "\x00\x00";                // version made by
        $cdrec .= $sub1 . $sub2;

         // length of filename, extra field length, file comment length, disk number start, internal file attributes, external file attributes - 'archive' bit set, offset
        $cdrec .= pack('v', strlen($name)) . pack('v', 0) . pack('v', 0) . pack('v', 0) . pack('v', 0) . pack('V', 32) . pack('V',0);
        $cdrec .= $name;

        // combine data
        $fileData = $fr . $cdrec . $eof_ctrl_dir;

        // total # of entries "on this disk", total # of entries overall, size of central dir, offset to start of central dir, .zip file comment length
        $fileData .= pack('v', 1) . pack('v', 1) . pack('V', strlen($cdrec)) . pack('V', strlen($fr)) . "\x00\x00";

        if ($zp = @fopen(BACKUP_PATH . $backupfile, "a"))
        {
			@fwrite($zp, $fileData);
			@fclose($zp);
			return true;
        }
        else
        {
        	return false;
        }

	// uncompressed
    }
    else
    {
        if ($zp = @fopen(BACKUP_PATH . $backupfile, "a"))
        {
			@fwrite($zp, $fileData);
			@fclose($zp);
			return true;
        }
        else
        {
        	return false;
        }
    }
}

function create_comp_dirs($comp_path, $comp_subdirs)
{
		$index = "<?php\nheader(\"Location: ../index.php\");\n?>";
	    $cdir = $comp_path;
	    @mkdir($cdir);
		$f = @fopen("$cdir/index.php", "wb");
		@fwrite($f, $index);
		@fclose($f);

	    foreach($comp_subdirs as $dir)
	    {
			@mkdir($cdir.'/'.$dir);
			$f = @fopen("$cdir/$dir/index.php", "wb");
			@fwrite($f, $index);
			@fclose($f);
	    }
}
?>