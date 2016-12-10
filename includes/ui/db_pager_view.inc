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
//--------------------------------------------------------------------------------------------------
function pager_link($link_text, $url, $icon=false)
{
	return ControlRenderer::get()->pager_link($link_text, $url, $icon);
}

function navi_button($name, $value, $enabled=true, $icon = false) {
	// NOTE: This function can be removed. It is private to the renderer implementation. CP 2014-11
	return ControlRenderer::get()->pager_button($name, $value, $enabled, $icon);
}

function navi_button_cell($name, $value, $enabled=true, $align='left') {
	// NOTE: This function can be removed. It is private to the renderer implementation. CP 2014-11
	ControlRenderer::get()->pager_button_cell($name, $value, $enabled, $align);
}

//-----------------------------------------------------------------------------
//
//    Sql paged table view. Call this function inside form.
//
function display_db_pager(&$pager) {
	// Note that the return seems redundant CP 2014-11
	return ControlRenderer::get()->pager($pager);
}

?>