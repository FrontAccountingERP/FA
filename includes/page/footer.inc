<?php

function page_footer($no_menu=false, $is_index=false)
{
	global $path_to_root;

  	if (in_ajax())
  		return;// just for speed up

  	if (function_exists('hyperlink_back'))
		hyperlink_back();
	include_once($path_to_root."/themes/".user_theme()."/renderer.php");
	$rend = new renderer();
	$rend->menu_footer($no_menu, $is_index);
    echo "</body></html>\n";
}

?>