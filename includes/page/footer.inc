<?php

include_once($path_to_root . "/includes/date_functions.inc");

function page_footer($no_menu=false, $is_index=false)
{
	global $version, $allow_demo_mode;
	global $app_title, $power_url, $power_by;

	if (isset($_GET['ajax'])) return;

	echo set_js_focus();	// set focus on input (if $focus_on exists)
	if (function_exists('hyperlink_back'))
		hyperlink_back();
	if ($no_menu == false) 
	{
    	if ($is_index)
    		echo "<table class=bottomBar>\n";
    	else
    		echo "<table class=bottomBar2>\n";
    	echo "<tr>";
    	if (isset($_SESSION['wa_current_user']))
    		echo "<td class=bottomBarCell>" . Today() . " | " . Now() . "</td>\n";
    	echo "</tr></table>\n";
	}
	echo "</td></tr></table></td>\n";
    echo "</table>\n";
    if ($no_menu == false) 
    {
    	echo "<table align='center' id='footer'>\n";
    	echo "<tr>\n";
    	echo "<td align='center' class='footer'><a target='_blank' href='$power_url'><font color='#ffffff'>$app_title $version - " . _("Theme:") . " " . user_theme() . "</font></a></td>\n";
    	echo "</tr>\n";
    	echo "<tr>\n";
    	echo "<td align='center' class='footer'><a target='_blank' href='$power_url'><font color='#ffff00'>$power_by</font></a></td>\n";
    	echo "</tr>\n";
    	if ($allow_demo_mode==true)
    	{
          	echo "<tr>\n";
            //echo "<td><br><div align='center'><a href='http://sourceforge.net'><img src='http://sourceforge.net/sflogo.php?group_id=89967&amp;type=5' alt='SourceForge.net Logo' width='210' height='62' border='0' align='middle' /></a></div></td>\n";
          	echo "</tr>\n";
    	}
    	echo "</table><br><br>\n";
    }
    echo "</body></html>\n";
}

?>