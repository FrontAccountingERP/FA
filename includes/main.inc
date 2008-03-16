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
	
    function page($title, $no_menu=false, $is_index=false, $onload="", $js="")
    {

    	global $path_to_root, $js_lib;

    	$hide_menu = $no_menu;

    	include($path_to_root . "/includes/page/header.inc");

    	page_header($title, $no_menu, $is_index, $onload, $js);
    }

    function end_page($no_menu=false, $is_index=false)
    {
    	global $path_to_root;

    	$hide_menu = $no_menu;

    	include($path_to_root . "/includes/page/footer.inc");

    	page_footer($no_menu, $is_index);
    }
	
	function add_js_file($filename) 
	{
	  global $js_static;
	  $search = array_search($filename, $js_static);
	  if ($search === false || $search === null) // php>4.2.0 returns null
		$js_static[] = $filename;
	}

	function add_js_source($text) 
	{
	  global $js_lib;
	  
	  $search = array_search($text, $js_lib);
	  if ($search === false || $search === null) // php>4.2.0 returns null
		$js_lib[] = $text;
	}
?>