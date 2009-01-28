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
function page_footer($no_menu=false, $is_index=false)
{
	global $path_to_root;

  	if (in_ajax())
  		return;// just for speed up

  	if (!$is_index && function_exists('hyperlink_back'))
		hyperlink_back();
	include_once($path_to_root."/themes/".user_theme()."/renderer.php");
	$rend = new renderer();
	$rend->menu_footer($no_menu, $is_index);
    echo "</body></html>\n";
}

?>