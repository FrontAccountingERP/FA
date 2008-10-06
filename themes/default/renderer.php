<?php

	class renderer
	{
		function wa_header()
		{
			page(_("Main Menu"), false, true);
		}

		function wa_footer()
		{
			end_page(false, true);
		}

		function menu_header($title, $no_menu, $is_index)
		{
			global $path_to_root, $applications, $help_base_url, $db_connections;
			// you can owerride the table styles from config.php here, if you want.
			//global $table_style, $table_style2;
			//$table_style 	= "cellpadding=3 border=1 bordercolor='#8cacbb' style='border-collapse: collapse'";
			//$table_style2 = "cellpadding=3 border=1 bordercolor='#cccccc' style='border-collapse: collapse'";
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
				$local_path_to_root = $path_to_root;
				$sel_app = $_SESSION['sel_app'];
				echo "<table cellpadding=0 cellspacing=0 width='100%'><tr><td>";
				echo "<div class=tabs>";
				foreach($applications as $app => $name)
				{
					$acc = access_string($name);
					echo "<a ".($sel_app == $app ? "class='selected' " : "").
					"href='$local_path_to_root/index.php?application=".$app.
						SID ."'$acc[1]>" .$acc[0] . "</a>";
				}
				echo "</div>";
				echo "</td></tr></table>";

				echo "<table class=logoutBar>";
				echo "<tr><td class=headingtext3>" . $db_connections[$_SESSION["wa_current_user"]->company]["name"] . " | " . $_SERVER['SERVER_NAME'] . " | " . $_SESSION["wa_current_user"]->name . "</td>";

				echo "  <td class='logoutBarRight'><a href='$path_to_root/admin/display_prefs.php?'>" . _("Preferences") . "</a>&nbsp;&nbsp;&nbsp;\n";
				echo "  <a href='$path_to_root/admin/change_current_user_password.php?selected_id=" . $_SESSION["wa_current_user"]->username . "'>" . _("Change password") . "</a>&nbsp;&nbsp;&nbsp;\n";

				if ($help_base_url != null)
				{
					echo "<a target = '_blank' onclick=" .'"'."javascript:openWindow(this.href,this.target); return false;".'" '. "href='". help_url($title, $sel_app)."'>" . _("Help") . "</a>&nbsp;&nbsp;&nbsp;";
				}
				echo "<a href='$local_path_to_root/access/logout.php?'>" . _("Logout") . "</a>&nbsp;&nbsp;&nbsp;";
				echo "</td></tr></table>";
			}
			echo "</td></tr></table>";

			if ($title && !$no_menu && !$is_index)
			{
				echo "<center><table width='100%'><tr><td width='100%' class='titletext'>$title</td>"
				."<td align=right>"
				.(user_hints() ? "<span id='hints'></span>" : '')
				."</td>"
				."</tr></table></center>";
			}

			if (!$is_index)
				echo "<br>";

		}

		function menu_footer($no_menu, $is_index)
		{
			global $version, $allow_demo_mode, $app_title, $power_url, $power_by, $path_to_root;
			include_once($path_to_root . "/includes/date_functions.inc");

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
				echo "<td align='center' class='footer'><a target='_blank' href='$power_url' tabindex='-1'><font color='#ffffff'>$app_title $version - " . _("Theme:") . " " . user_theme() . "</font></a></td>\n";
				echo "</tr>\n";
				echo "<tr>\n";
				echo "<td align='center' class='footer'><a target='_blank' href='$power_url' tabindex='-1'><font color='#ffff00'>$power_by</font></a></td>\n";
				echo "</tr>\n";
				if ($allow_demo_mode==true)
				{
					echo "<tr>\n";
					//echo "<td><br><div align='center'><a href='http://sourceforge.net'><img src='http://sourceforge.net/sflogo.php?group_id=89967&amp;type=5' alt='SourceForge.net Logo' width='210' height='62' border='0' align='middle' /></a></div></td>\n";
					echo "</tr>\n";
				}
				echo "</table><br><br>\n";
			}
		}

		function display_applications(&$waapp)
		{

			$selected_app = $waapp->get_selected_application();

			foreach ($selected_app->modules as $module)
			{
				// image
				echo "<tr>";
				// values
				echo "<td valign='top' class='menu_group'>";
				echo "<table border=0 width='100%'>";
				echo "<tr><td class='menu_group'>";
				echo $module->name;
				echo "</td></tr><tr>";
				echo "<td class='menu_group_items'>";

				foreach ($module->lappfunctions as $appfunction)
				{
					if ($_SESSION["wa_current_user"]->can_access_page($appfunction->access)) 
					{
						$lnk = access_string($appfunction->label);
						echo "<a href='$appfunction->link'$lnk[1]>$lnk[0]</a><br>";
					}
				}
				echo "</td>";
				if (sizeof($module->rappfunctions) > 0)
				{
					echo "<td width='50%' class='menu_group_items'>";
					foreach ($module->rappfunctions as $appfunction)
					{
						if ($_SESSION["wa_current_user"]->can_access_page($appfunction->access)) 
						{
							$lnk = access_string($appfunction->label);
							echo "<a href='$appfunction->link'$lnk[1]>$lnk[0]</a><br>";
						}
					}
					echo "</td>";
				}

				echo "</tr></table></td></tr>";
			}

			echo "</table>";
		}
	}

?>