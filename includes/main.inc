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

    function page($title, $no_menu=false, $is_index=false, $onload="", $js="")
    {

    	global $path_to_root;

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

?>