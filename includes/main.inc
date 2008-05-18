<?php

    include_once($path_to_root . "/includes/db/connect_db.inc");

    include_once($path_to_root . "/includes/reserved.inc");
    include_once($path_to_root . "/includes/errors.inc");
    include_once($path_to_root . "/includes/types.inc");
    include_once($path_to_root . "/includes/systypes.inc");
    include_once($path_to_root . "/includes/references.inc");
    include_once($path_to_root . "/includes/prefs/sysprefs.inc");
    include_once($path_to_root . "/includes/db/comments_db.inc");
    include_once($path_to_root . "/includes/db/sql_functions.inc");

    include_once($path_to_root . "/admin/db/users_db.inc");
    include_once($path_to_root . "/includes/ui/ui_view.inc");
	
	function output_html($text) 
	{
  	  global $before_box;
  	  return  in_ajax() ? fmt_errors() : ($before_box.fmt_errors().$text);
	}

    function page($title, $no_menu=false, $is_index=false, $onload="", $js="")
    {

    	global $path_to_root;

    	$hide_menu = $no_menu;
		// intercept all output to destroy it in case of ajax call
		register_shutdown_function('ob_end_flush');
		ob_start('output_html',0);
		// colect all error msgs
		set_error_handler('error_handler' /*, errtypes */);

    	include($path_to_root . "/includes/page/header.inc");

    	page_header($title, $no_menu, $is_index, $onload, $js);
		error_box();
    }

    function end_page($no_menu=false, $is_index=false)
    {
    	global $path_to_root, $Ajax;

    	$hide_menu = $no_menu;

    	include($path_to_root . "/includes/page/footer.inc");

		$Ajax->run();
    	page_footer($no_menu, $is_index);
    }

    function flush_dir($path) {
	$dir = opendir($path);
	while(false !== ($fname = readdir($dir))) {
		if($fname=='.' || $fname=='..') continue;
  		if(is_dir($path.'/'.$fname)) {
		    flush_dir($path.'/'.$fname);
		    @rmdir($path.'/'.$fname);
		} else
		    @unlink($path.'/'.$fname);
  	}
    }

    function cache_js_file($fpath, $text) 
    {
	global $go_debug;

	if(!$go_debug) $text = js_compress($text);

        $file = fopen($fpath, 'w');
	if (!$file) return false;
	if (!fwrite($file, $text)) return false;
	return fclose($file);

    }

    function add_js_file($filename) 
    {
	  global $js_static;

	  $search = array_search($filename, $js_static);
	  if ($search === false || $search === null) // php>4.2.0 returns null
		$js_static[] = $filename;	
    }

    function add_js_ufile($filename) 
    {
	  global $js_userlib;

	  $search = array_search($filename, $js_userlib);
	  if ($search === false || $search === null) // php>4.2.0 returns null
		$js_userlib[] = $filename;
    }

    function add_js_source($text) 
    {
	  global $js_lib;
	  
	  $search = array_search($text, $js_lib);
	  if ($search === false || $search === null) // php>4.2.0 returns null
		$js_lib[] = $text;
    }

/**
 * Compresses the Javascript code for more efficient delivery.
 * copyright (c) 2005 by Jared White & J. Max Wilson
 * http://www.xajaxproject.org
 * Added removing comments from output.
 * Warning: Fails on RegExp with quotes - use new RegExp() in this case.
 */
function js_compress($sJS)
{
	//remove windows cariage returns
	$sJS = str_replace("\r","",$sJS);
	
	//array to store replaced literal strings
	$literal_strings = array();
	
	//explode the string into lines
	$lines = explode("\n",$sJS);
	//loop through all the lines, building a new string at the same time as removing literal strings
	$clean = "";
	$inComment = false;
	$literal = "";
	$inQuote = false;
	$escaped = false;
	$quoteChar = "";
	
	for($i=0;$i<count($lines);$i++)
	{
		$line = $lines[$i];
		$inNormalComment = false;
	
		//loop through line's characters and take out any literal strings, replace them with ___i___ where i is the index of this string
		for($j=0;$j<strlen($line);$j++)
		{
			$c = substr($line,$j,1);
			$d = substr($line,$j,2);
	
			//look for start of quote
			if(!$inQuote && !$inComment)
			{
				//is this character a quote or a comment
				if(($c=="\"" || $c=="'") && !$inComment && !$inNormalComment)
				{
					$inQuote = true;
					$inComment = false;
					$escaped = false;
					$quoteChar = $c;
					$literal = $c;
				}
				else if($d=="/*" && !$inNormalComment)
				{
					$inQuote = false;
					$inComment = true;
					$escaped = false;
					$quoteChar = $d;
					$literal = $d;	
					$j++;	
				}
				else if($d=="//") //ignore string markers that are found inside comments
				{
					$inNormalComment = true;
					$clean .= $c;
				}
				else
				{
					$clean .= $c;
				}
			}
			else //allready in a string so find end quote
			{
				if($c == $quoteChar && !$escaped && !$inComment)
				{
					$inQuote = false;
					$literal .= $c;
	
					//subsitute in a marker for the string
					$clean .= "___" . count($literal_strings) . "___";
	
					//push the string onto our array
					array_push($literal_strings,$literal);
	
				}
				else if($inComment && $d=="*/")
				{
					$inComment = false;
					$literal .= $d;
	
					//subsitute in a marker for the string
					$clean .= "___" . count($literal_strings) . "___";
	
					//push the string onto our array
					array_push($literal_strings,$literal);
	
					$j++;
				}
				else if($c == "\\" && !$escaped)
					$escaped = true;
				else
					$escaped = false;
	
				$literal .= $c;
			}
		}
		if($inComment) $literal .= "\n";
		$clean .= "\n";
	}
	//explode the clean string into lines again
	$lines = explode("\n",$clean);
	
	//now process each line at a time
	for($i=0;$i<count($lines);$i++)
	{
		$line = $lines[$i];
	
		//remove comments
		$line = preg_replace("/\/\/(.*)/","",$line);
	
		//strip leading and trailing whitespace
		$line = trim($line);
	
		//remove all whitespace with a single space
		$line = preg_replace("/\s+/"," ",$line);
	
		//remove any whitespace that occurs after/before an operator
		$line = preg_replace("/\s*([!\}\{;,&=\|\-\+\*\/\)\(:])\s*/","\\1",$line);
	
		$lines[$i] = $line;
	}
	
	//implode the lines
	$sJS = implode("\n",$lines);
	
	//make sure there is a max of 1 \n after each line
	$sJS = preg_replace("/[\n]+/","\n",$sJS);
	
	//strip out line breaks that immediately follow a semi-colon
	$sJS = preg_replace("/;\n/",";",$sJS);
	
	//curly brackets aren't on their own
	$sJS = preg_replace("/[\n]*\{[\n]*/","{",$sJS);
	
	//finally loop through and replace all the literal strings:
	for($i=0;$i<count($literal_strings);$i++) {
	    if (strpos($literal_strings[$i],"/*")!==false) 
		$literal_strings[$i]= '';
		$sJS = str_replace("___".$i."___",$literal_strings[$i],$sJS);
	}
	return $sJS;
}
?>