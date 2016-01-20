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

include_once($path_to_root . "/includes/ui/InputRenderer.inc");

//
// Sets local POST value and adds Value to ajax posting if needed
//
/*function set_post($name, $value, $ajax_trigger=true) {
    global $Ajax;

    $_POST[$name] = $value;
    if ($ajax_trigger) $Ajax->activate($name);
}
*/
//------------------------------------------------------------------------------
//    Seek for _POST variable with $prefix.
//    If var is found returns variable name with prefix stripped,
//    and null or -1 otherwise.
//
function find_submit($prefix, $numeric=true)
{
	return InputRenderer::get()->find_submit($prefix, $numeric);
}

/*
 Helper function.
 Returns true if input $name with $submit_on_change option set is subject to update.
 */
function input_changed($name)
{
	return InputRenderer::get()->input_changed($name);
}

//------------------------------------------------------------------------------
//
// Helper function for simple db table editor pages
//
function simple_page_mode($numeric_id = true)
{
	InputRenderer::get()->simple_page_mode($numeric_id);
}

//------------------------------------------------------------------------------
//
//	Read numeric value from user formatted input
//
function input_num($postname=null, $dflt=0)
{
	return InputRenderer::get()->input_num($postname, $dflt);
}

//---------------------------------------------------------------------------------

function hidden($name, $value=null, $echo=true)
{
	return InputRenderer::get()->hidden($name, $value, $echo);
}
/*
	Universal submit form button.
	$atype - type of submit:
	 Normal submit:
		false - normal button; optional icon
		null  - button visible only in fallback mode; optional icon
	 Ajax submit:
		true	  - standard button; optional icon

		'default' - default form submit on Ctrl-Enter press; dflt ICON_OK icon
		'selector' - ditto with closing current popup editor window
		'cancel'  - cancel form entry on Escape press; dflt ICON_CANCEL
		'process' - displays progress bar during call; optional icon
		'nonajax' - ditto, non-ajax submit
	$atype can contain also multiply type selectors separated by space,
	however make sense only combination of 'process' and one of defualt/selector/cancel
*/
function submit($name, $value, $echo=true, $title=false, $atype=false, $icon=false)
{
	return InputRenderer::get()->submit($name, $value, $echo, $title, $atype, $icon);
}

function submit_center($name, $value, $echo=true, $title=false, $async=false, $icon=false)
{
	InputRenderer::get()->submit_center($name, $value, $echo, $title, $async, $icon);
}

function submit_center_first($name, $value, $title=false, $async=false, $icon=false)
{
	InputRenderer::get()->submit_center_first($name, $value, $title, $async, $icon);
}

function submit_center_last($name, $value, $title=false, $async=false, $icon=false)
{
	InputRenderer::get()->submit_center_last($name, $value, $title, $async, $icon);
}
/*
	For following controls:
	'both' - use both Ctrl-Enter and Escape hotkeys
	'cancel' - apply to 'RESET' button
*/
function submit_add_or_update($add=true, $title=false, $async=false, $clone=false)
{
	InputRenderer::get()->submit_add_or_update($add, $title, $async, $clone);
}

function submit_add_or_update_center($add=true, $title=false, $async=false, $clone=false)
{
	InputRenderer::get()->submit_add_or_update_center($add, $title, $async, $clone);
}

function submit_add_or_update_row($add=true, $right=true, $extra="", $title=false, $async=false, $clone = false)
{
	InputRenderer::get()->submit_add_or_update_row($add, $right, $extra, $title, $async, $clone);
}

function submit_cells($name, $value, $extra="", $title=false, $async=false)
{
	InputRenderer::get()->submit_cells($name, $value, $extra, $title, $async);
}

function submit_row($name, $value, $right=true, $extra="", $title=false, $async=false)
{
	InputRenderer::get()->submit_row($name, $value, $right, $extra, $title, $async);
}

function submit_return($name, $value, $title=false)
{
	InputRenderer::get()->submit_return($name, $value, $title);
}

function submit_js_confirm($name, $msg, $set = true)
{
	InputRenderer::get()->submit_js_confirm($name, $msg, $set);
}
//-----------------------------------------------------------------------------------

function set_icon($icon, $title=false)
{
	return InputRenderer::get()->set_icon($icon, $title);
}

function button($name, $value, $title=false, $icon=false,  $aspect='')
{
	return InputRenderer::get()->button($name, $value, $title, $icon, $aspect);
}

function button_cell($name, $value, $title=false, $icon=false, $aspect='')
{
	InputRenderer::get()->button_cell($name, $value, $title, $icon, $aspect);
}

function delete_button_cell($name, $value, $title=false)
{
	InputRenderer::get()->delete_button_cell($name, $value, $title);
}

function edit_button_cell($name, $value, $title=false)
{
	InputRenderer::get()->edit_button_cell($name, $value, $title);
}

function select_button_cell($name, $value, $title=false)
{
	InputRenderer::get()->select_button_cell($name, $value, $title);
}
//-----------------------------------------------------------------------------------

function check_value($name)
{
	// TODO This function would be better off elsewhere CP 2014-11
	return InputRenderer::get()->check_value($name);
}

function checkbox($label, $name, $value=null, $submit_on_change=false, $title=false)
{
	return InputRenderer::get()->checkbox($label, $name, $value, $submit_on_change, $title);
}

function check($label, $name, $value=null, $submit_on_change=false, $title=false)
{
	InputRenderer::get()->check($label, $name, $value, $submit_on_change, $title);
}

function check_cells($label, $name, $value=null, $submit_on_change=false, $title=false, $params='')
{
	InputRenderer::get()->check_cells($label, $name, $value, $submit_on_change, $title, $params);
}

function check_row($label, $name, $value=null, $submit_on_change=false, $title=false)
{
	InputRenderer::get()->check_row($label, $name, $value, $submit_on_change, $title);
}

//-----------------------------------------------------------------------------------
function radio($label, $name, $value, $selected=null, $submit_on_change=false)
{
	return InputRenderer::get()->radio($label, $name, $value, $selected, $submit_on_change);
}

//-----------------------------------------------------------------------------------
function labelheader_cell($label, $params="")
{
	InputRenderer::get()->labelheader_cell($label, $params);
}

function label_cell($label, $params="", $id=null)
{
	return InputRenderer::get()->label_cell($label, $params, $id);
}

function email_cell($label, $params="", $id=null)
{
	InputRenderer::get()->email_cell($label, $params, $id);
}

function amount_decimal_cell($label, $params="", $id=null)
{
	InputRenderer::get()->amount_decimal_cell($label, $params, $id);
}

function amount_cell($label, $bold=false, $params="", $id=null)
{
	InputRenderer::get()->amount_cell($label, $bold, $params, $id);
}

//JAM  Allow entered unit prices to be fractional
function unit_amount_cell($label, $bold=false, $params="", $id=null)
{
	InputRenderer::get()->unit_amount_cell($label, $bold, $params, $id);
}

function percent_cell($label, $bold=false, $id=null)
{
	InputRenderer::get()->percent_cell($label, $bold, $id);
}
// 2008-06-15. Changed
function qty_cell($label, $bold=false, $dec=null, $id=null)
{
	InputRenderer::get()->qty_cell($label, $bold, $dec, $id);
}

function label_cells($label, $value, $params="", $params2="", $id='')
{
	InputRenderer::get()->label_cells($label, $value, $params, $params2, $id);
}

function label_row($label, $value, $params="", $params2="", $leftfill=0, $id='')
{
	InputRenderer::get()->label_row($label, $value, $params, $params2, $leftfill, $id);
}

function text_input($name, $value=null, $size='', $max='', $title='', $params='')
{
	InputRenderer::get()->text_input($name, $value, $size, $max, $title, $params);
}

//-----------------------------------------------------------------------------------

function text_cells(
	$label, $name, $value=null, $size="", $max="", $title=false,
	$labparams="", $post_label="", $inparams="")
{
	InputRenderer::get()->text_cells($label, $name, $value, $size, $max, $title, $labparams, $post_label, $inparams);
}

function text_cells_ex(
	$label, $name, $size, $max=null, $init=null, $title=null,
	$labparams=null, $post_label=null, $submit_on_change=false)
{
	InputRenderer::get()->text_cells_ex($label, $name, $size, $max, $init, $title, $labparams, $post_label, $submit_on_change);
}

function text_row($label, $name, $value, $size, $max, $title=null, $params="", $post_label="")
{
	InputRenderer::get()->text_row($label, $name, $value, $size, $max, $title, $params, $post_label);
}

//-----------------------------------------------------------------------------------

function text_row_ex($label, $name, $size, $max=null, $title=null, $value=null, $params=null, $post_label=null)
{
	InputRenderer::get()->text_row_ex($label, $name, $size, $max, $title, $value, $params, $post_label);
}

//-----------------------------------------------------------------------------------
function email_row($label, $name, $value, $size, $max, $title=null, $params="", $post_label="")
{
	InputRenderer::get()->email_row($label, $name, $value, $size, $max, $title, $params, $post_label);
}

function email_row_ex($label, $name, $size, $max=null, $title=null, $value=null, $params=null, $post_label=null)
{
	InputRenderer::get()->email_row_ex($label, $name, $size, $max, $title, $value, $params, $post_label);
}

function link_row($label, $name, $value, $size, $max, $title=null, $params="", $post_label="")
{
	InputRenderer::get()->link_row($label, $name, $value, $size, $max, $title, $params, $post_label);
}

function link_row_ex($label, $name, $size, $max=null, $title=null, $value=null, $params=null, $post_label=null)
{
	InputRenderer::get()->link_row_ex($label, $name, $size, $max, $title, $val, $params, $post_label);
}

//-----------------------------------------------------------------------------------
//
//	Since FA 2.2  $init parameter is superseded by $check.
//  When $check!=null current date is displayed in red when set to other
//	than current date.
//
function date_cells(
	$label, $name, $title = null, $check=null, $inc_days=0,
	$inc_months=0, $inc_years=0, $params=null, $submit_on_change=false)
{
	InputRenderer::get()->date_cells($label, $name, $title, $check, $inc_days, $inc_months, $inc_years, $params, $submit_on_change);
}

function date_row(
	$label, $name, $title=null, $check=null, $inc_days=0, $inc_months=0,
	$inc_years=0, $params=null, $submit_on_change=false)
{
	InputRenderer::get()->date_row($label, $name, $check, $inc_days, $inc_months, $inc_years, $params, $submit_on_change);
}

//-----------------------------------------------------------------------------------
function password_row($label, $name, $value)
{
	InputRenderer::get()->password_row($label, $name, $value);
}

//-----------------------------------------------------------------------------------
function file_cells($label, $name, $id="")
{
	InputRenderer::get()->file_cells($label, $name, $id);
}

function file_row($label, $name, $id = "")
{
	InputRenderer::get()->file_row($label, $name, $id);
}

//-----------------------------------------------------------------------------------

function ref_cells($label, $name, $title=null, $init=null, $params=null, $submit_on_change=false)
{
	InputRenderer::get()->ref_cells($label, $name, $title, $init, $params, $submit_on_change);
}

//-----------------------------------------------------------------------------------

function ref_row($label, $name, $title=null, $init=null, $submit_on_change=false)
{
	InputRenderer::get()->ref_row($label, $name, $title, $init, $submit_on_change);
}

//-----------------------------------------------------------------------------------

function percent_row($label, $name, $init=null)
{
	InputRenderer::get()->percent_row($label, $name, $init);
}

function amount_cells_ex($label, $name, $size, $max=null, $init=null, $params=null, $post_label=null, $dec=null)
{
	InputRenderer::get()->amount_cells_ex($label, $name, $size, $max, $init, $params, $post_label, $dec);
}


//-----------------------------------------------------------------------------------

function amount_cells($label, $name, $init=null, $params=null, $post_label=null, $dec=null)
{
	InputRenderer::get()->amount_cells($label, $name, $init, $params, $post_label, $dec);
}

//JAM  Allow entered unit prices to be fractional
function unit_amount_cells($label, $name, $init=null, $params=null, $post_label=null, $dec=null)
{
	InputRenderer::get()->unit_amount_cells($label, $name, $init, $params, $post_label, $dec);
}

function amount_row($label, $name, $init=null, $params=null, $post_label=null, $dec=null)
{
	InputRenderer::get()->amount_row($label, $name, $init, $params, $post_label, $dec);
}

function small_amount_row($label, $name, $init=null, $params=null, $post_label=null, $dec=null)
{
	InputRenderer::get()->small_amount_row($label, $name, $init, $params, $post_label, $dec);
}

//-----------------------------------------------------------------------------------

function qty_cells($label, $name, $init=null, $params=null, $post_label=null, $dec=null)
{
	InputRenderer::get()->qty_cells($label, $name, $init, $params, $post_label, $dec);
}

function qty_row($label, $name, $init=null, $params=null, $post_label=null, $dec=null)
{
	InputRenderer::get()->qty_row($label, $name, $init, $params, $post_label, $dec);
}

function small_qty_row($label, $name, $init=null, $params=null, $post_label=null, $dec=null)
{
	InputRenderer::get()->small_qty_row($label, $name, $init, $params, $post_label, $dec);
}

//-----------------------------------------------------------------------------------

function small_amount_cells($label, $name, $init=null, $params=null, $post_label=null, $dec=null)
{
	InputRenderer::get()->small_amount_cells($label, $name, $init, $params, $post_label, $dec);
}

//-----------------------------------------------------------------------------------

function small_qty_cells($label, $name, $init=null, $params=null, $post_label=null, $dec=null)
{
	InputRenderer::get()->small_qty_cells($label, $name, $init, $params, $post_label, $dec);
}

//-----------------------------------------------------------------------------------

function textarea_cells($label, $name, $value, $cols, $rows, $title = null, $params="")
{
	InputRenderer::get()->textarea_cells($label, $name, $value, $cols, $rows, $title, $params);
}

function textarea_row($label, $name, $value, $cols, $rows, $title=null, $params="")
{
	InputRenderer::get()->textarea_row($label, $name, $value, $cols, $rows, $title, $params);
}

//-----------------------------------------------------------------------------------
/*
function text_row_with_submit($label, $name, $value, $size, $max, $input_name, $input_value)
{
  	global $Ajax;

	default_focus($name);
	echo "<tr><td>$label</td>\n";
	echo "<td>";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	echo "<input type=\"text\" name=\"$name\" size=\"$size\" maxlength=\"$max\" value=\"$value\">   ";

	submit($input_name, $input_value);

	echo "</td></tr>\n";
	$Ajax->addUpdate($name, $name, $value);
}
*/
//-----------------------------------------------------------------------------------
//
//	When show_inactive page option is set
//  displays value of inactive field as checkbox cell.
//  Also updates database record after status change.
//
function inactive_control_cell($id, $value, $table, $key)
{
	InputRenderer::get()->inactive_control_cell($id, $value, $table, $key);
}
//
//	Displays controls for optional display of inactive records
//
function inactive_control_row($th) {
	InputRenderer::get()->inactive_control_row($th);
}
//
//	Inserts additional column header when display of inactive records is on.
//
function inactive_control_column(&$th) {
	InputRenderer::get()->inactive_control_column($th);
}

function customer_credit_row($customer, $credit, $parms='')
{
	InputRenderer::get()->customer_credit_row($customer, $credit, $parms);
}

function supplier_credit_row($supplier, $credit, $parms='')
{
	InputRenderer::get()->supplier_credit_row($supplier, $credit, $parms);
}

function bank_balance_row($bank_acc, $parms='')
{
	InputRenderer::get()->bank_balance_row($bank_acc, $parms);
}

function ahref($label, $href, $target="", $onclick="") {
	InputRenderer::get()->ahref($label, $href, $target, $onclick);
}

function ahref_cell($label, $href, $target="", $onclick="") {
	InputRenderer::get()->ahref_cell($label, $href, $target, $onclick);
}

?>