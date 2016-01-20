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

include_once('InputRenderer.inc');

// ---------------------------------------------------------------------------------
class ControlRenderer
{
	/**
	 *
	 * @param ControlRenderer $controlRenderer
	 * @return ControlRenderer
	 */
	public static function get($controlRenderer = null)
	{
		global $path_to_root;
		static $instance = null;
		if ($controlRenderer != null) {
			$instance = $controlRenderer;
		}
		if ($instance == null) {
			// Give the theme an opportunity to register a custom renderer
			$themeName = user_theme();
			if ($themeName) {
				include_once($path_to_root . "/themes/".$themeName."/renderer.php");
			}
			// Otherwise create the default renderer
			if ($instance == null) {
				$instance = new ControlRenderer();
			}
		}
		return $instance;
	}

	private $form_nested;
	
	function ControlRenderer() {
		$this->form_nested = -1;
	}
	
	function start_form($multi = false, $dummy = false, $action = "", $name = "")
	{
		// $dummy - leaved for compatibility with 2.0 API
		
		if (++$this->form_nested) return;
		
		if ($name != "")
			$name = "name='$name'";
		if ($action == "")
			$action = $_SERVER['PHP_SELF'];

		if ($multi)
			echo "<form enctype='multipart/form-data' method='post' action='$action' $name>\n";
		else
			echo "<form method='post' action='$action' $name>\n";
	}

	private function output_hidden()
	{
		InputRenderer::get()->output_hidden();
	}

	// ---------------------------------------------------------------------------------
	function end_form($breaks = 0)
	{
		global $Ajax;

		if ($this->form_nested-- > 0) return;
		
		$_SESSION['csrf_token'] = hash('sha256', uniqid(mt_rand(), true));
		if ($breaks)
			br($breaks);
		hidden('_focus');
		hidden('_modified', get_post('_modified', 0));
		hidden('_confirmed'); // helper for final form confirmation
		hidden('_token', $_SESSION['csrf_token']);
	
		$this->output_hidden();
		echo "</form>\n";
		$Ajax->activate('_token');
		$Ajax->activate('_confirmed');
	}

	function check_csrf_token()
	{
		if ($_SESSION['csrf_token'] != @$_POST['_token'])
		{
			display_error(_("Request from outside of this page is forbidden."));
			error_log(_("CSRF attack detected from: ") . @$_SERVER['HTTP_HOST'] . ' (' . @$_SERVER['HTTP_REFERER'] . ')');
			return false;
		}
		return true;
	}

	function start_table($class = false, $extra = "", $padding = '2', $spacing = '0')
	{
		echo "<center><table";
		if ($class == TABLESTYLE_NOBORDER)
			echo " class='tablestyle_noborder'";
		elseif ($class == TABLESTYLE2)
			echo " class='tablestyle2'";
		elseif ($class == TABLESTYLE)
			echo " class='tablestyle'";
		if ($extra != "")
			echo " $extra";
		echo " cellpadding='$padding' cellspacing='$spacing'>\n";
	}

	function end_table($breaks = 0)
	{
		echo "</table></center>\n";
		$this->output_hidden();
		if ($breaks)
			br($breaks);
	}

	function start_outer_table($class = false, $extra = "", $padding = '2', $spacing = '0', $br = false)
	{
		if ($br)
			br();
		start_table($class, $extra, $padding, $spacing);
		echo "<tr valign=top><td>\n"; // outer table
	}

	function table_section($number = 1, $width = false)
	{
		if ($number > 1)
		{
			echo "</table>\n";
			$this->output_hidden();
			$width = ($width ? "width='$width'" : "");
			echo "</td><td style='border-left:1px solid #cccccc;' $width>\n"; // outer table
		}
		echo "<table class='tablestyle_inner'>\n";
	}

	function end_outer_table($breaks = 0, $close_table = true)
	{
		if ($close_table)
		{
			echo "</table>\n";
			$this->output_hidden();
		}
		echo "</td></tr>\n";
		end_table($breaks);
	}
	//
	// outer table spacer
	//
	function vertical_space($params = '')
	{
		echo "</td></tr><tr><td valign=center $params>";
	}

	function meta_forward($forward_to, $params = "", $timeout=0)
	{
		global $Ajax;
		echo "<meta http-equiv='Refresh' content='".$timeout."; url=$forward_to?$params'>\n";
		echo "<center><br>" . _("You should automatically be forwarded.");
		echo " " . _("If this does not happen") . " " . "<a href='$forward_to?$params'>" . _("click here") . "</a> " . _("to continue") . ".<br><br></center>\n";
		if ($params !='') $params = '?'.$params;
		
		$Ajax->redirect($forward_to . $params);
		exit();
	}

	// -----------------------------------------------------------------------------------
	// Find and replace hotkey marker.
	// if $clean == true marker is removed and clean label is returned
	// (for use in wiki help system), otherwise result is array of label
	// with underlined hotkey letter and access property string.
	//
	function access_string($label, $clean = false)
	{
		$access = '';
		$slices = array();

		if (preg_match('/(.*)&([a-zA-Z0-9])(.*)/', $label, $slices))
		{
			$label = $clean ? $slices[1] . $slices[2] . $slices[3] : $slices[1] . '<u>' . $slices[2] . '</u>' . $slices[3];
			$access = " accesskey='" . strtoupper($slices[2]) . "'";
		}

		$label = str_replace('&&', '&', $label);

		return $clean ? $label : array($label, $access);
	}

	function hyperlink_back($center = true, $no_menu = true, $type_no = 0, $trans_no = 0, $final = false)
	{
		global $path_to_root;

		if ($center)
			echo "<center>";
		$id = 0;
		if ($no_menu && $trans_no != 0)
		{
			include_once ($path_to_root . "/admin/db/attachments_db.inc");
			$id = has_attachment($type_no, $trans_no);
			$attach = get_attachment_string($type_no, $trans_no);
			echo $attach;
		}
		$width = ($id != 0 ? "30%" : "20%");
		start_table(false, "width=$width");
		start_row();
		if ($no_menu)
		{
			echo "<td align=center><a href='javascript:window.print();'>" . _("Print") . "</a></td>\n";
		}
		echo "<td align=center><a href='javascript:goBack(" . ($final ? '-2' : '') . ");'>" . ($no_menu ? _("Close") : _("Back")) . "</a></td>\n";
		end_row();
		end_table();
		if ($center)
			echo "</center>";
		echo "<br>";
	}

	function hyperlink_no_params($target, $label, $center = true)
	{
		$id = default_focus();
		$pars = access_string($label);
		if ($target == '')
			$target = $_SERVER['PHP_SELF'];
		if ($center)
			echo "<br><center>";
		echo "<a href='$target' id='$id' $pars[1]>$pars[0]</a>\n";
		if ($center)
			echo "</center>";
	}

	function hyperlink_no_params_td($target, $label)
	{
		echo "<td>";
		hyperlink_no_params($target, $label);
		echo "</td>\n";
	}

	function viewer_link($label, $url = '', $class = '', $id = '', $icon = null)
	{
		global $path_to_root;

		if ($class != '')
			$class = " class='$class'";

		if ($id != '')
			$class = " id='$id'";

		if ($url != "")
		{
			$pars = access_string($label);
			if (user_graphic_links() && $icon)
				$pars[0] = set_icon($icon, $pars[0]);
			$preview_str = "<a target='_blank' $class $id href='$path_to_root/$url' onclick=\"javascript:openWindow(this.href,this.target); return false;\"$pars[1]>$pars[0]</a>";
		}
		else
			$preview_str = $label;
		return $preview_str;
	}

	function menu_link($url, $label, $id = null)
	{
		global $path_to_root;
		
		$id = default_focus($id);
		$pars = access_string($label);
		
		// REVIEW: The addition of $path_to_root to $url seems unnecessary or wrong CJP 2016-01
		if ($url[0] != '/')
			$url = '/'.$url;
		$url = $path_to_root.$url;
		return "<a href='$url' class='menu_option' id='$id' $pars[1]>$pars[0]</a>";
	}

	function submenu_option($title, $url, $id = null)
	{
		display_note(menu_link($url, $title, $id), 0, 1);
	}

	function submenu_view($title, $type, $number, $id = null)
	{
		display_note(get_trans_view_str($type, $number, $title, false, 'viewlink', $id), 0, 1);
	}

	function submenu_print($title, $type, $number, $id = null, $email = 0, $extra = 0)
	{
		display_note(print_document_link($number, $title, true, $type, false, 'printlink', $id, $email, $extra), 0, 1);
	}
	// -----------------------------------------------------------------------------------
	function hyperlink_params($target, $label, $params, $center = true)
	{
		$id = default_focus();

		$pars = access_string($label);
		if ($target == '')
			$target = $_SERVER['PHP_SELF'];
		if ($center)
			echo "<br><center>";
		echo "<a id='$id' href='$target?$params'$pars[1]>$pars[0]</a>\n";
		if ($center)
			echo "</center>";
	}

	function hyperlink_params_td($target, $label, $params)
	{
		echo "<td>";
		hyperlink_params($target, $label, $params, false);
		echo "</td>\n";
	}

	// -----------------------------------------------------------------------------------
	function hyperlink_params_separate($target, $label, $params, $center = false)
	{
		$id = default_focus();

		$pars = access_string($label);
		if ($center)
			echo "<br><center>";
		echo "<a target='_blank' id='$id' href='$target?$params' $pars[1]>$pars[0]</a>\n";
		if ($center)
			echo "</center>";
	}

	function hyperlink_params_separate_td($target, $label, $params)
	{
		echo "<td>";
		hyperlink_params_separate($target, $label, $params);
		echo "</td>\n";
	}

	// --------------------------------------------------------------------------------------------------
	function alt_table_row_color(&$k, $extra_class = null)
	{
		$classes = $extra_class ? array($extra_class) : array();
		if ($k == 1)
		{
			array_push($classes, 'oddrow');
			$k = 0;
		}
		else
		{
			array_push($classes, 'evenrow');
			$k ++;
		}
		echo "<tr class='" . implode(' ', $classes) . "'>\n";
	}

	function table_section_title($msg, $colspan = 2)
	{
		echo "<tr><td colspan=$colspan class='tableheader'>$msg</td></tr>\n";
	}

	function table_header($labels, $params = '')
	{
		start_row();
		foreach ($labels as $label)
			labelheader_cell($label, $params);
		end_row();
	}
	// -----------------------------------------------------------------------------------
	function start_row($param = "")
	{
		if ($param != "")
			echo "<tr $param>\n";
		else
			echo "<tr>\n";
	}

	function end_row()
	{
		echo "</tr>\n";
	}

	// TODO REVIEW: This function is no longer in stable CJP 2016-01
	/**
	 * @param array | string $cells
	 */
	function table_add_cells($cells)
	{
		if (is_array($cells)) {
			foreach ($cells as $cell) {
				echo $cell;
			}
		} else {
			echo $cells;
		}
	}

	function br($num = 1)
	{
		for ($i = 0; $i < $num; $i ++)
			echo "<br>";
	}

	var $ajax_divs = array();

	function div_start($id = '', $trigger = null, $non_ajax = false)
	{
		if ($non_ajax) { // div for non-ajax elements
			array_push($this->ajax_divs, array($id, null));
			echo "<div style='display:none' class='js_only' " . ($id != '' ? "id='$id'" : '') . ">";
		} else { // ajax ready div
			array_push($this->ajax_divs, array($id, $trigger === null ? $id : $trigger));
			echo "<div " . ($id != '' ? "id='$id'" : '') . ">";
			ob_start();
		}
	}

	function div_end()
	{
		global $Ajax;

		$this->output_hidden();
		if (count($this->ajax_divs))
		{
			$div = array_pop($this->ajax_divs);
			if ($div[1] !== null)
				$Ajax->addUpdate($div[1], $div[0], ob_get_flush());
			echo "</div>";
		}
	}

	// -----------------------------------------------------------------------------
	// Tabbed area:
	// $name - prefix for widget internal elements:
	// Nth tab submit name: {$name}_N
	// div id: _{$name}_div
	// sel (hidden) name: _{$name}_sel
	// $tabs - array of tabs; string: tab title or array(tab_title, enabled_status)
	function tabbed_content_start($name, $tabs, $dft = '')
	{
		global $Ajax;

		$selname = '_' . $name . '_sel';
		$div = '_' . $name . '_div';

		$sel = find_submit($name . '_', false);
		if ($sel == null)
			$sel = get_post($selname, (string) ($dft === '' ? key($tabs) : $dft));

		if ($sel !== @$_POST[$selname])
			$Ajax->activate($name);

		$_POST[$selname] = $sel;

		div_start($name);
		$str = "<ul class='ajaxtabs' rel='$div'>\n";
		foreach ($tabs as $tab_no => $tab) {

			$acc = access_string(is_array($tab) ? $tab[0] : $tab);
			$disabled = (is_array($tab) && ! $tab[1]) ? 'disabled ' : '';
			$str .= ("<li>" . "<button type='submit' name='{$name}_" . $tab_no . "' class='" . ((string) $tab_no === $sel ? 'current' : 'ajaxbutton') . "' $acc[1] $disabled>" . "<span>$acc[0]</span>" . "</button>\n" . "</li>\n");
		}

		$str .= "</ul>\n";
		$str .= "<div class='spaceBox'></div>\n";
		$str .= "<input type='hidden' name='$selname' value='$sel'>\n";
		$str .= "<div class='contentBox' id='$div'>\n";
		echo $str;
	}

	function tabbed_content_end()
	{
		$this->output_hidden();
		echo "</div>"; // content box (don't change to div_end() unless div_start() is used above)
		div_end(); // tabs widget
	}

	function tab_changed($name)
	{
		$to = find_submit("{$name}_", false);
		if (! $to)
			return null;

		return array(
			'from' => $from = get_post("_{$name}_sel"),
			'to' => $to
		);
	}

	/*
	 Check whether tab has been just switched on
	 */
	function tab_opened($name, $tab)
	{
		return (get_post('_'.$name.'_sel') != $tab) && (find_submit($name.'_', false) == $tab);
	}
	/*
	 Check whether tab has been just switched off
	 */
	function tab_closed($name, $tab)
	{
		return (get_post('_'.$name.'_sel') == $tab) && (find_submit($name.'_', false) != $tab);
	}
	/*
	 Check whether tab is visible on current page
	 */
	function tab_visible($name, $tab)
	{
		$new = find_submit($name.'_', false);
		return (get_post('_'.$name.'_sel') == $tab && !$new) || $new==$tab;
	}
	
	function pager_link($link_text, $url, $icon = false)
	{
		global $path_to_root;

		if (user_graphic_links() && $icon)
			$link_text = set_icon($icon, $link_text);

		$href = $path_to_root . $url;
		return "<a href='$href'>" . $link_text . "</a>";
	}

	function pager_button($name, $value, $enabled = true, $icon = false)
	{
		global $path_to_root;
		return "<button " . ($enabled ? '' : 'disabled') . " class=\"navibutton\" type=\"submit\"" . " name=\"$name\"  id=\"$name\" value=\"$value\">" . ($icon ? "<img src='$path_to_root/themes/" . user_theme() . "/images/" . $icon . "'>" : '') . "<span>$value</span></button>\n";
	}

	function pager_button_cell($name, $value, $enabled = true, $align = 'left')
	{
		label_cell(navi_button($name, $value, $enabled), "align='$align'");
	}

	// -----------------------------------------------------------------------------
	//
	// Sql paged table view. Call this function inside form.
	//
	function pager(&$pager)
	{
		global $path_to_root;

		$pager->select_records();

		div_start("_{$pager->name}_span");
		$headers = array();

		foreach ($pager->columns as $num_col => $col) {
			// record status control column is displayed only when control checkbox is on
			if (isset($col['head']) && ($col['type'] != 'inactive' || get_post('show_inactive'))) {
				if (! isset($col['ord']))
					$headers[] = $col['head'];
				else {
					$icon = (($col['ord'] == 'desc') ? 'sort_desc.gif' : ($col['ord'] == 'asc' ? 'sort_asc.gif' : 'sort_none.gif'));
					$headers[] = navi_button($pager->name . '_sort_' . $num_col, $col['head'], true, $icon);
				}
			}
		}
		/* show a table of records returned by the sql */
		start_table(TABLESTYLE, "width=$pager->width");
		table_header($headers);

		if ($pager->header_fun) { // if set header handler
			start_row("class='{$pager->header_class}'");
			$fun = $pager->header_fun;
			if (method_exists($pager, $fun)) {
				$h = $pager->$fun($pager);
			} elseif (function_exists($fun)) {
				$h = $fun($pager);
			}

			foreach ($h as $c) { // draw header columns
				$pars = isset($c[1]) ? $c[1] : '';
				label_cell($c[0], $pars);
			}
			end_row();
		}

		$cc = 0; // row colour counter
		foreach ($pager->data as $line_no => $row) {

			$marker = $pager->marker;
			if ($marker && $marker($row))
				start_row("class='$pager->marker_class'");
			else
				alt_table_row_color($cc);
			foreach ($pager->columns as $k => $col) {
				$coltype = $col['type'];
				$cell = isset($col['name']) ? $row[$col['name']] : '';

				if (isset($col['fun'])) { // use data input function if defined
					$fun = $col['fun'];
					if (method_exists($pager, $fun)) {
						$cell = $pager->$fun($row, $cell);
					} elseif (function_exists($fun)) {
						$cell = $fun($row, $cell);
					} else
						$cell = '';
				}
				switch ($coltype) { // format column
					case 'time':
						label_cell($cell, "width=40");
						break;
					case 'date':
						label_cell(sql2date($cell), "align='center' nowrap");
						break;
					case 'dstamp': // time stamp displayed as date
						label_cell(sql2date(substr($cell, 0, 10)), "align='center' nowrap");
						break;
					case 'tstamp': // time stamp - FIX user format
						label_cell(sql2date(substr($cell, 0, 10)) . ' ' . substr($cell, 10), "align='center'");
						break;
					case 'percent':
						percent_cell($cell);
						break;
					case 'amount':
						if ($cell == '')
							label_cell('');
						else
							amount_cell($cell, false);
						break;
					case 'qty':
						if ($cell == '')
							label_cell('');
						else
							qty_cell($cell, false, isset($col['dec']) ? $col['dec'] : null);
						break;
					case 'email':
						email_cell($cell, isset($col['align']) ? "align='" . $col['align'] . "'" : null);
						break;
					case 'rate':
						label_cell(number_format2($cell, user_exrate_dec()), "align=center");
						break;
					case 'inactive':
						if (get_post('show_inactive'))
							$pager->inactive_control_cell($row);
						break;
					default:
						if (isset($col['align']))
							label_cell($cell, "align='" . $col['align'] . "'");
						else
							label_cell($cell);
					case 'skip': // column not displayed
				}
			}
			end_row();
		}
		// end of while loop

		if ($pager->footer_fun) { // if set footer handler
			start_row("class='{$pager->footer_class}'");
			$fun = $pager->footer_fun;
			if (method_exists($pager, $fun)) {
				$h = $pager->$fun($pager);
			} elseif (function_exists($fun)) {
				$h = $fun($pager);
			}

			foreach ($h as $c) { // draw footer columns
				$pars = isset($c[1]) ? $c[1] : '';
				label_cell($c[0], $pars);
			}
			end_row();
		}

		start_row("class='navibar'");
		$colspan = count($pager->columns);
		$inact = @$pager->inactive_ctrl == true ? ' ' . checkbox(null, 'show_inactive', null, true) . _("Show also Inactive") : '';
		if ($pager->rec_count) {
			echo "<td colspan=$colspan class='navibar' style='border:none;padding:3px;'>";
			echo "<div style='float:right;'>";
			$but_pref = $pager->name . '_page_';
			start_table();
			start_row();
			if (@$pager->inactive_ctrl)
				submit('Update', _('Update'), true, '', null); // inactive update
			echo navi_button_cell($but_pref . 'first', _('First'), $pager->first_page, 'right');
			echo navi_button_cell($but_pref . 'prev', _('Prev'), $pager->prev_page, 'right');
			echo navi_button_cell($but_pref . 'next', _('Next'), $pager->next_page, 'right');
			echo navi_button_cell($but_pref . 'last', _('Last'), $pager->last_page, 'right');
			end_row();
			end_table();
			echo "</div>";
			$from = ($pager->curr_page - 1) * $pager->page_len + 1;
			$to = $from + $pager->page_len - 1;
			if ($to > $pager->rec_count)
				$to = $pager->rec_count;
			$all = $pager->rec_count;
			echo sprintf(_('Records %d-%d of %d'), $from, $to, $all);
			echo $inact;
			echo "</td>";
		} else {
			label_cell(_('No records') . $inact, "colspan=$colspan class='navibar'");
		}

		end_row();

		end_table();

		if (isset($pager->marker_txt))
			display_note($pager->marker_txt, 0, 1, "class='$pager->notice_class'");

		div_end();
		return true;
	}

}

?>
