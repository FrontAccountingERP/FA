<?php

function write_config_db($new = false)
{
	global $path_to_root, $def_coy, $db_connections, $tb_pref_counter;
	include_once($path_to_root . "/config_db.php");

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
	if (file_exists($filename) && is_writable($filename))
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

function db_import($filename, $connection)
{
	$data_queries = array();
	$drop_queries = array();
	$table_queries = array();

	ini_set("max_execution_time", "180");
	// uncrompress gziped backup files
	if (strpos($filename, ".gzip") || strpos($filename, ".GZIP"))
		$lines = db_ungzip("lines", $filename);
	elseif (strpos($filename, ".zip") || strpos($filename, ".ZIP"))
		$lines = db_unzip("lines", $filename);
	else
		$lines = file("". $filename);

	// divide insert and create sql queries
	// $table is set to TRUE if the next line belongs to a create sql query
	$table = false;
	foreach($lines as $line)
	{
		$line = trim($line);

		$line = str_replace("0_", $connection["tbpref"], $line);
		// $line = str_replace("Y_", "0_", $line);
		// the last line did not belong to a 'create' sql query
		if (!$table)
		{

			// this line does not, too
			if (strtolower(substr($line,0,6)) == "insert")
			{
				$data_queries[] = substr($line, 0, strlen($line) - 1);

			// this line does not, too
			}
			elseif (strtolower(substr($line,0,6)) == "update")
			{
				$data_queries[] = substr($line, 0, strlen($line) - 1);

			// this line does not, too
			}
			elseif (strtolower(substr($line, 0, 20)) == "drop table if exists")
			{
				$drop_queries[] = substr($line, 0, strlen($line) - 1);

			// this line does!
			}
			elseif (strtolower(substr($line, 0, 6)) == "create")
			{
				$table = true;
				$table_queries[] = $line . "\n";
			}
			elseif (strtolower(substr($line, 0, 11)) == "alter table")
			{
				$data_queries[] = substr($line, 0, strlen($line) - 1);
			}

		// the current line belongs to a create sql query
		}
		else
		{

			// create sql query ending in this line
			if (strtolower(substr($line, 0, 1)) == ")") {
				$table = false;
				$line = substr($line,0,strlen($line)-1);
		  }
			$table_queries[count($table_queries) - 1] .= $line . "\n";
		}
	}

	$sql_error = false;

	// execute drop tables if exists queries
	if (is_array($drop_queries))
	{
		foreach($drop_queries as $drop_query)
		{
			$sql_error = false;
			if (!db_query($drop_query))
			{
				$sql_error = true;
				//if ($CONF['import_error']) echo nl2br($drop_query)."\n<div class=\"bold_left\">".mysql_error()."</div><br>\n";
			}
		}
	}

	// execute create tables queries
	if (is_array($table_queries))
	{
		foreach($table_queries as $table_query)
		{
			$sql_error = false;
			if (!db_query($table_query))
			{
				$sql_error = true;
				//if ($CONF['import_error']) echo nl2br($table_query)."\n<div class=\"bold_left\">".mysql_error()."</div><br>\n";
			}
		}
	}

	// execute insert data queries
	if (is_array($data_queries))
	{
		foreach($data_queries as $data_query)
		{
			$sql_error = false;
			if (!db_query($data_query))
			{
				//if ($CONF['import_error']) echo $data_query."\n<div class=\"bold_left\">".mysql_error()."</div><br>\n";
				$sql_error = true;
				return false;
			}
		}
	}

	// show number successful executed querys or if an error did occur
	if ($sql_error == 1)
		return false;
		//echo "<div class=\"red\">".IM_ERROR.".</div>\n";
	else
		return true;
		//echo "<div class=\"green\">".IM_SUCCESS." ".count($table_queries)." ".IM_TABLES." ".count($data_queries)." ".IM_ROWS." (".$import_file.")</div>\n";
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
    $filename = ereg_replace(".*/", "", $path);
    $filename = substr($filename, 0, strlen($filename) - 4);

    // compare filname in zip and filename from $_GET
    if (substr($all, 30, strlen($filename)) != $filename)
    {
		return '';
        // exit if names differ
        //echo F_WRONG_FILE.".";
        //exit;
    }
    else
    {
    	// get the suffix of the filename in hex
        $crc_bugfix = substr(substr($filename, 0, strlen($filename) - 4), strlen($filename) - 12 - 4);
        $suffix = false;

        // convert hex to ascii
        for ($i=0; $i < 12; )
        	$suffix .= chr($crc_bugfix[$i++] . $crc_bugfix[$i++] . $crc_bugfix[$i++]);

        // remove central directory information (we have always just one ziped file)
        $comp = substr($all, -(strlen($all) - 30 - strlen($filename)));
        $comp = substr($comp, 0, (strlen($comp) - 80 - strlen($filename)));

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

// generates a dump of $db database
// $drop and $zip tell if to include the drop table statement or dry to pack
function db_export($conn, $filename, $zip='no', $comment='')
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
    $company = get_company_pref('coy_name');
    //create comment
    $out="# MySQL dump of database '".$conn["dbname"]."' on host '".$conn["host"]."'\n";
    $out.="# Backup Date and Time: ".date("Y-m-d H:i")."\n";
    $out.="# Built by " . $app_title . " " . $version ."\n";
    $out.="# ".$power_url."\n";
    $out.="# Company: ". @html_entity_decode($company, ENT_COMPAT, $_SESSION['language']->encoding)."\n";
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
			($conn["tbpref"] != "" && strpos($row['Name'], $conn["tbpref"]) !== false))
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

				$field_type = array();
				for ($j = 0; $j < db_num_rows($res3); $j++)
				{
					$row3 = db_fetch($res3);
					$field_type[] = $row3[2];
				}

				$res2 = db_query("SELECT * FROM `" . $tablename . "`");
				for ($j = 0; $j < db_num_rows($res2); $j++)
				{
					$out .= "INSERT INTO `" . $tablename . "` VALUES (";
					$row2 = db_fetch_row($res2);
					// run through each field
					for ($k = 0; $k < $nf = db_num_fields($res2); $k++)
					{
						$out .= db_escape(@html_entity_decode($row2[$k], ENT_COMPAT, $_SESSION['language']->encoding));
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

        // string needed for decoding (because of crc bug)
        //$name_suffix = substr($zdata, -4, 4);
        //$name_suffix2 = "_";
        //for ($i = 0; $i < 4; $i++)
        //	$name_suffix2 .= sprintf("%03d", ord($name_suffix[$i]));
        //$backupfile = substr($backupfile, 0, strlen($backupfile) - 8) . $name_suffix2 . ".sql.zip";
        $name = substr($backupfile, 0, strlen($backupfile) -4);

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