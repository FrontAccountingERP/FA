<?php

function help_url($title, $app)
{

	global $help_page_url, $help_base_url, $applications, $help_page_title;

	if (isset($help_page_url))
	{
		return $help_base_url.$help_page_url;
	}

	$help_page_url = '';
	if (isset($help_page_title))
	{
		$help_page_url = $help_page_title;
	}
	elseif ($title==_("Main Menu"))
	{
		$help_page_url = $applications[$app];
	}
	else
	{
		$help_page_url = $title;
	}

	return $help_base_url.urlencode(str_replace(' ', '', ucwords($help_page_url)));
}

function send_scripts() 
{
    global $js_lib, $js_static, $js_path, $js_userlib, $comp_path,
	$path_to_root, $go_debug;
    
    	add_user_js_data();

	$js ='';
	foreach($js_static as $jsfile) {
	    $fpath = $comp_path.'/'.user_company().'/js_cache/'.$jsfile;
	    // compress also static files
	  if (!file_exists($fpath) || $go_debug)
		cache_js_file($fpath, file_get_contents($js_path.$jsfile));
	
	  $js .= '<script language="javascript" type="text/javascript" src="'.
		$fpath . '"></script>';
	}
	foreach($js_userlib as $jsfile) {
	  $js .= '<script language="javascript" type="text/javascript" src="'.
	    $jsfile . '"></script>';
	}

	foreach($js_lib as $text) {
	  $js .= $text;
	}
        echo $js;
}

function page_header($title, $no_menu=false, $is_index=false, $onload="", $js="")
{
	// titles and screen header
	global $db_connections, $path_to_root, $def_app, $applications,
		$help_base_url, $help_page_url, $use_popup_windows;

	if (in_ajax()) return; // just for speed up

	$theme = user_theme();
	$local_path_to_root = $path_to_root;

	if ($help_base_url != null && $use_popup_windows && $js == '')
	{
		if (!function_exists('get_js_open_window'))
		{
			include_once($path_to_root.'/includes/ui/ui_view.inc');
		}
		add_js_source(get_js_open_window(900, 500));
	}
	if ($js!='')
	  add_js_source($js);

	if (!isset($no_menu))
	{
		$no_menu = false;
	}
	if (!isset($_SESSION["sel_app"]))
		session_register("sel_app");
	if (isset($_SESSION["App"]) && is_object($_SESSION["App"]) && isset($_SESSION["App"]->selected_application) &&
		$_SESSION["App"]->selected_application != "")
		$sel_app = $_SESSION["App"]->selected_application;
	elseif (isset($_SESSION["sel_app"]) && $_SESSION["sel_app"] != "")
		$sel_app = $_SESSION["sel_app"];
	else
		$sel_app = $def_app;
	$_SESSION["sel_app"] = $sel_app;
	$encoding = $_SESSION['language']->encoding;

	if (!headers_sent()){
		header("Content-type: text/html; charset='$encoding'");
	}
	echo "<html dir='" . $_SESSION['language']->dir . "' >\n";
	echo "<head><title>$title</title>";
	$local_stylesheet = $_SESSION['language']->get_stylesheet();
	echo "<meta http-equiv='Content-type' content='text/html'; charset='$encoding'>";
	echo "<link href='$local_path_to_root/themes/$theme/default.css' rel='stylesheet' type='text/css' /> \n";
	if (file_exists($local_path_to_root. "/".$local_stylesheet))
		echo "<link href='$local_path_to_root/$local_stylesheet' rel='stylesheet' type='text/css' /> \n";
	send_scripts($js);

	echo "</head> \n";
	if ($onload == "")
		echo "<body>";
	else
		echo "<body onload=\"$onload\">";

	echo "<table class='callout_main' border='0' cellpadding='0' cellspacing='0'>\n";
	echo "<tr>\n";
	echo "<td colspan='2' rowspan='2'>\n";

	echo "<table class='main_page' border='0' cellpadding='0' cellspacing='0'>\n";
	echo "<tr>\n";
	echo "<td>\n";
	echo "<table width='100%' border='0' cellpadding='0' cellspacing='0'>\n";
	echo "<tr>\n";
	echo "<td class='quick_menu'>\n";
	if (!$no_menu)
	{
		echo "<table cellpadding=0 cellspacing=0 width='100%'><tr><td>";
		echo "<div class=tabs>";
		foreach($applications as $app => $name)
		{
			echo "<a ".($sel_app == $app ? "class='selected' " : "").
			"href='$local_path_to_root/index.php?application=".$app.
				SID ."'>" .$name . "</a>";
		}
		echo "</div>";

		echo "</td></tr></table>";

		echo "<table class=logoutBar>";
		echo "<tr><td class=headingtext3>" . $db_connections[$_SESSION["wa_current_user"]->company]["name"] . " | " . $_SERVER['SERVER_NAME'] . " | " . $_SESSION["wa_current_user"]->name . "</td>";
		if ($help_base_url != null)
		{

			echo "<td align=right ><a target = '_blank' onclick=" .'"'."javascript:openWindow(this.href,this.target); return false;".'" '. "href='". help_url($title, $sel_app)."'>" . _("Help") . "</a></td>";
		}
		echo "<td class=logoutBarRight><a href='$local_path_to_root/access/logout.php?'>" . _("Logout") . "</a>";
		echo "</td></tr></table>";
	}

	echo "</td></tr></table>";

	if ($title && !$no_menu && !$is_index)
	{
		echo "<center><table width='100%'><tr><td width='100%' class='titletext'>$title</td></tr></table></center>";
	}

	if (!$is_index)
		echo "<br>";
}

?>