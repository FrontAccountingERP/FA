<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU General Public License, GPL, 
	as published by the Free Software Foundation, either version 3 
	of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/gpl-3.0.html>.
***********************************************************************/
$page_security = 1;
$path_to_root="..";
include($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/ui/ui_view.inc");

page(_("Logout"), true, false, "", get_js_png_fix());

?>

<table width="100%" border="0">
  <tr>
	<td align="center"><img src="<?php echo "$path_to_root/themes/default/images/logo_frontaccounting.png";?>" alt="FrontAccounting" width="250" height="50" onload="fixPNG(this)"></td>
  </tr>
  <tr>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td><div align="center"><font size=2>
<?php
    		echo _("Thank you for using") . " ";

			echo "<strong>$app_title $version</strong>";
?>
         </font></div></td>
  </tr>
  <tr>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td><div align="center">
        <?php
     echo "<a href='$path_to_root/index.php?" . SID ."'><b>" . _("Click here to Login Again.") . "</b></a>";
?>
      </div></td>
  </tr>
</table>
<br>
<?php

	end_page(false, true);
	session_unset();
	session_destroy();
?>


