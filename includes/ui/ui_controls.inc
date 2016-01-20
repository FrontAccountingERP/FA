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

include_once($path_to_root . "/includes/ui/ControlRenderer.inc");

/*
	Retrieve value of POST variable(s).
	For $name passed as array $dflt is not used,
	default values can be passed as values with non-numeric keys instead.
	If some field have user formatted numeric value, pass float default value to
	convert automatically to POSIX.
*/
function get_post($name, $dflt='')
{
	if (is_array($name)) {
		$ret = array();
		foreach($name as $key => $dflt)
			if (!is_numeric($key)) {
				$ret[$key] = is_float($dflt) ? input_num($key, $dflt) : get_post($key, $dflt);
			} else {
				$ret[$dflt] = get_post($dflt, null);
			}
		return $ret;
	} else
		return is_float($dflt) ? input_num($name, $dflt) :
				((!isset($_POST[$name]) /*|| $_POST[$name] === ''*/) ? $dflt : $_POST[$name]);
}
//---------------------------------------------------------------------------------

function start_form($multi=false, $dummy=false, $action="", $name="")
{
	// $dummy - leaved for compatibility with 2.0 API
	ControlRenderer::get()->start_form($multi, $dummy, $action, $name);
}

//---------------------------------------------------------------------------------

function end_form($breaks=0)
{
	ControlRenderer::get()->end_form($breaks);
}

function check_csrf_token()
{
	return ControlRenderer::get()->check_csrf_token();
}

function start_table($class=false, $extra="", $padding='2', $spacing='0')
{
	ControlRenderer::get()->start_table($class, $extra, $padding, $spacing);
}

function end_table($breaks=0)
{
	ControlRenderer::get()->end_table($breaks);
}

function start_outer_table($class=false, $extra="", $padding='2', $spacing='0', $br=false)
{
	ControlRenderer::get()->start_outer_table($class, $extra, $padding, $spacing, $br);
}

function table_section($number=1, $width=false)
{
	ControlRenderer::get()->table_section($number, $width);
}

function end_outer_table($breaks=0, $close_table=true)
{
	ControlRenderer::get()->end_outer_table($breaks, $close_table);
}
//
//  outer table spacer
//
function vertical_space($params='')
{
	ControlRenderer::get()->vertical_space();
}

function meta_forward($forward_to, $params="")
{
	ControlRenderer::get()->meta_forward($forward_to, $params);
}

//-----------------------------------------------------------------------------------
// Find and replace hotkey marker.
// if $clean == true marker is removed and clean label is returned
// (for use in wiki help system), otherwise result is array of label
// with underlined hotkey letter and access property string.
//
function access_string($label, $clean=false)
{
	return ControlRenderer::get()->access_string($label, $clean);
}

function hyperlink_back($center=true, $no_menu=true, $type_no=0, $trans_no=0, $final=false)
{
	ControlRenderer::get()->hyperlink_back($center, $no_menu, $type_no, $trans_no, $final);
}

function hyperlink_no_params($target, $label, $center=true)
{
	ControlRenderer::get()->hyperlink_no_params($target, $label, $center);
}

function hyperlink_no_params_td($target, $label)
{
	ControlRenderer::get()->hyperlink_no_params_td($target, $label);
}

function viewer_link($label, $url='', $class='', $id='',  $icon=null)
{
	return ControlRenderer::get()->viewer_link($label, $url, $class, $id, $icon);
}

function menu_link($url, $label, $id=null)
{
	return ControlRenderer::get()->menu_link($url, $label, $id);
}

function submenu_option($title, $url, $id=null)
{
	ControlRenderer::get()->submenu_option($title, $url, $id);
}

function submenu_view($title, $type, $number, $id=null)
{
	ControlRenderer::get()->submenu_view($title, $type, $number, $id);
}

function submenu_print($title, $type, $number, $id=null, $email=0, $extra=0)
{
	ControlRenderer::get()->submenu_print($title, $type, $number, $id, $email, $extra);
}
//-----------------------------------------------------------------------------------

function hyperlink_params($target, $label, $params, $center=true)
{
	ControlRenderer::get()->hyperlink_params($target, $label, $params, $center);
}

function hyperlink_params_td($target, $label, $params)
{
	ControlRenderer::get()->hyperlink_params_td($target, $label, $params);
}

//-----------------------------------------------------------------------------------

function hyperlink_params_separate($target, $label, $params, $center=false)
{
	ControlRenderer::get()->hyperlink_params_separate($target, $label, $params, $center);
}

function hyperlink_params_separate_td($target, $label, $params)
{
	ControlRenderer::get()->hyperlink_params_td($target, $label, $params);
}

//--------------------------------------------------------------------------------------------------

function alt_table_row_color(&$k, $extra_class=null)
{
	ControlRenderer::get()->alt_table_row_color($k, $extra_class);
}

function table_section_title($msg, $colspan=2)
{
	ControlRenderer::get()->table_section_title($msg, $colspan);
}

function table_header($labels, $params='')
{
	ControlRenderer::get()->table_header($labels, $params);
}
//-----------------------------------------------------------------------------------

function start_row($param="")
{
	ControlRenderer::get()->start_row($param);
}

function end_row()
{
	ControlRenderer::get()->end_row();
}

function br($num=1)
{
	ControlRenderer::get()->br($num);
}

function div_start($id='', $trigger=null, $non_ajax=false)
{
	ControlRenderer::get()->div_start($id, $trigger, $non_ajax);
}

function div_end()
{
	ControlRenderer::get()->div_end();
}

//-----------------------------------------------------------------------------
//	Tabbed area:
// 	$name - prefix for widget internal elements:
//		Nth tab submit name:  {$name}_N
//		div id:	_{$name}_div
//		sel (hidden) name: _{$name}_sel
// $tabs - array of tabs; string: tab title or array(tab_title, enabled_status)

function tabbed_content_start($name, $tabs, $dft='') {
	ControlRenderer::get()->tabbed_content_start($name, $tabs, $dft);
}

function tabbed_content_end() {
	ControlRenderer::get()->tabbed_content_end();
}

function tab_changed($name)
{
	return ControlRenderer::get()->tab_changed($name);
}

/*
 Check whether tab has been just switched on
 */
function tab_opened($name, $tab)
{
	return ControlRenderer::get()->tab_opened($name, $tab);
}
/*
 Check whether tab has been just switched off
 */
function tab_closed($name, $tab)
{
	return ControlRenderer::get()->tab_closed($name, $tab);
}
/*
 Check whether tab is visible on current page
 */
function tab_visible($name, $tab)
{
	return ControlRenderer::get()->tab_visible($name, $tab);
}

/* Table editor interfaces. Key is editor type
	0 => url of editor page
	1 => hotkey code
	2 => context help
*/
$popup_editors = array(
	'customer' => array('/sales/manage/customers.php?debtor_no=',
		113,	_("Customers"), 900, 500),
	'branch' => array('/sales/manage/customer_branches.php?SelectedBranch=',
		114, _("Branches"), 900, 700),
	'supplier' => array('/purchasing/manage/suppliers.php?supplier_id=',
		113, _("Suppliers"), 900, 700),
	'item' => array('/inventory/manage/items.php?stock_id=',
		115, _("Items"), 800, 600),
	'fa_item' => array('/inventory/manage/items.php?FixedAsset=1&stock_id=', 
		115, _("Items"), 800, 600)
);
/*
	Bind editors for various selectors.
	$type - type of editor
	$input - name of related input field
	$caller - optional function key code (available values F1-F12: 112-123,
		true: default)
*/
function set_editor($type, $input, $caller=true)
{
	global $path_to_root, $Editors, $popup_editors, $Pagehelp;

	$key = $caller===true ? $popup_editors[$type][1] : $caller;

	$Editors[$key] = array( $path_to_root . $popup_editors[$type][0], $input,
		$popup_editors[$type][3], $popup_editors[$type][4]);

	$help = 'F' . ($key - 111) . ' - ';
	$help .= $popup_editors[$type][2];
	$Pagehelp[] = $help;
}
//------------------------------------------------------------------------------
// Procedures below are now obsolete. Preserved for eventual future use.

/*
	External page call with saving current context.
	$call - url of external page
	$ctx - optional. name of SESSION context object or array of names of POST
		variables saved on call
*/
function context_call($call, $ctx='')
{
	if (is_array($ctx))
	{
		foreach($ctx as $postname)
		{
		 	$context[$postname] = get_post($postname);
		}
	} else
		$context = isset($_SESSION[$ctx]) ? $_SESSION[$ctx] : null;

	array_unshift($_SESSION['Context'], array('name' => $ctx,
		'ctx' => $context,
		'caller' => $_SERVER['PHP_SELF'],
		'ret' => array()));
	meta_forward($call);
}
/*
	Restores context after external page call and
	returns array of data passed by external page.
*/
function context_restore()
{
	if ( count($_SESSION['Context'])) {
		if ($_SERVER['PHP_SELF'] == $_SESSION['Context'][0]['caller']) {
			$ctx = array_shift($_SESSION['Context']);
			if ($ctx) {
				if (is_array($ctx['ctx'])) {
					foreach($ctx['ctx'] as $name => $val)
					{
						$_POST[$name] = $val;
					}
				} else
					if ($ctx['name']!='')
						$_SESSION[$ctx['name']] = $ctx['ctx'];
				return $ctx['ret'];
			}
		}
	}
	return false;
}

/*
	Return to caller page if the page was called from external context.
*/
function context_return($ret)
{
	if ( count($_SESSION['Context'])) {
		$ctx = &$_SESSION['Context'][0];
		$ctx['ret'] = $ret;
		meta_forward( $ctx['caller'] );
	}
}
/*
	Clearing context stack after page cancel.
*/
function context_reset()
{
	$_SESSION['Context'] = array();
}
/*
	Context stack initialization
*/
if (!isset($_SESSION['Context'])) {
		context_reset();
}
/*
	Redirector for selector F4 calls.
	$sel_editors is array of selname=>editor_page
*/
function editor_redirect($sel_editors, $save_fun='') {
	foreach ($sel_editors as $selname=>$editor)
		if (isset($_POST['_'.$selname.'_editor'])) {
			if (function_exists($save_fun))
				$save_fun();
			unset($_POST['_'.$selname.'_editor']);
			context_call($editor, array_keys($_POST));
		}
}
/*
	Return procedure for selector F4 calls
*/
function editor_return($vars, $restore_fun='') {
	if (function_exists($restore_fun))
		$restore_fun();

	if ($ret = context_restore()) {
		foreach ($vars as $postname=>$retname)
			if (isset($ret[$retname])) {
				$_POST[$postname] = $ret[$retname];
				set_focus($postname);
			}
	}
}

function confirm_dialog($submit, $msg) {
	if (find_post($submit)) {
		display_warning($msg);
		br();
		submit_center_first('DialogConfirm', _("Proceed"), '', true);
		submit_center_last('DialogCancel', _("Cancel"), '', 'cancel');
		return 0;
	} else
		return get_post('DialogConfirm', 0);
}

/*
 Confirm dialog to be used optionally in final form checking routine.
 Displays warning conditionally unless it was displayed
 */
function display_confirmation($msg)
{
	global $Ajax;

	if (!get_post('_confirmed'))
	{
		$_POST['_confirmed'] = 1;
		display_warning($msg);
		return false;
	} else
		return true;
}

/*
	Block menu/shortcut links during transaction procesing.
*/
function page_processing($msg = false)
{
	global $Ajax;

	if ($msg === true)
		$msg = _("Entered data has not been saved yet.\nDo you want to abandon changes?");

	$js = "_validate._processing=" . (
		$msg ? '\''.strtr($msg, array("\n"=>'\\n')) . '\';' : 'null;');
	if (in_ajax()) {
		$Ajax->addScript(true, $js);
	} else
		add_js_source($js);
}

function page_modified($status = true)
{
	global $Ajax;

	$js = "_validate._modified=" . ($status ? 1:0).';';
	if (in_ajax()) {
		$Ajax->addScript(true, $js);
	} else
		add_js_source($js);
}

?>