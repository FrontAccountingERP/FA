<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU Affero General Public License,
	AGPL, as published by the Free Software Foundation, either version 
	3 of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/agpl-3.0.html>.
***********************************************************************/
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
		$help_page_url = access_string($applications[$app], true);
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
	foreach($js_static as $jsfile)
	{
	    $fpath = $comp_path.'/'.user_company().'/js_cache/'.$jsfile;
	    // compress also static files
	  	if (!file_exists($fpath) || $go_debug)
			cache_js_file($fpath, file_get_contents($js_path.$jsfile));

	  	$js .= '<script language="javascript" type="text/javascript" src="'.
			$fpath . '"></script>';
	}
	foreach($js_userlib as $jsfile)
	{
	  	$js .= '<script language="javascript" type="text/javascript" src="'.
	    	$jsfile . '"></script>';
	}

	foreach($js_lib as $text)
	{
	  	$js .= $text;
	}
    echo $js;
}

function page_header($title, $no_menu=false, $is_index=false, $onload="", $js="")
{
	// titles and screen header
	global $path_to_root, $def_app, $use_popup_windows, $help_base_url;

	if (in_ajax())
		return; // just for speed up

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
	if (isset($_SESSION["App"]) && is_object($_SESSION["App"]))
		$_SESSION["App"]->selected_application = $sel_app;
	$encoding = $_SESSION['language']->encoding;

	if (!headers_sent()){
		header("Content-type: text/html; charset='$encoding'");
	}
	echo "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n";
	echo "<html dir='" . $_SESSION['language']->dir . "' >\n";
	echo "<head><title>$title</title>";
	echo "<meta http-equiv='Content-type' content='text/html; charset=$encoding'>";
	echo "<link href='$local_path_to_root/themes/$theme/default.css' rel='stylesheet' type='text/css'> \n";
	send_scripts($js);

	echo "</head> \n";
	if ($onload == "")
		echo "<body>";
	else
		echo "<body onload=\"$onload\">";
	include_once($local_path_to_root."/themes/".user_theme()."/renderer.php");
	$rend = new renderer();
	$rend->menu_header($title, $no_menu, $is_index);
}

?>