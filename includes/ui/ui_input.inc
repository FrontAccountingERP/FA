<?php

function get_post($name, $dflt='')
{
	return ((!isset($_POST[$name]) || $_POST[$name] === '') ? $dflt : $_POST[$name]);
}
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

    foreach($_POST as $postkey=>$postval )
    {
		if (strpos($postkey, $prefix) === 0)
		{
			$id = substr($postkey, strlen($prefix));
			return $numeric ? (int)$id : $id;
		}
    }
    return $numeric ? -1 : null;
}
//------------------------------------------------------------------------------
//
// Helper function for simple db table editor pages
//
function simple_page_mode($numeric_id = true)
{
	global $Ajax, $Mode, $selected_id;

	$default = $numeric_id ? -1 : '';
	$selected_id = get_post('selected_id', $default);
	foreach (array('ADD_ITEM', 'UPDATE_ITEM', 'RESET') as $m) {
		if (isset($_POST[$m])) {
			$Ajax->activate('_page_body');
			if ($m == 'RESET') 
				$selected_id = $default;
			$Mode = $m; return;
		}
	}
	foreach (array('Edit', 'Delete') as $m) {
		foreach ($_POST as $p => $pvar) {
			if (strpos($p, $m) === 0) {
//				$selected_id = strtr(substr($p, strlen($m)), array('%2E'=>'.'));
				unset($_POST['_focus']); // focus on first form entry
				$selected_id = quoted_printable_decode(substr($p, strlen($m)));
				$Ajax->activate('_page_body');
				$Mode = $m;
				return;
			}
		}
	}
	$Mode = '';
}

//------------------------------------------------------------------------------
//
//	Read numeric value from user formatted input
//
function input_num($postname=null, $dflt=null)
{
	if (!isset($_POST[$postname]))
	  	return $dflt;

    return user_numeric($_POST[$postname]);
}

//---------------------------------------------------------------------------------

function hidden($name, $value=null, $echo=true)
{
  	global $Ajax;
	
	if ($value === null) 
		$value = get_post($name);
	
  	$ret = "<input type=\"hidden\" name=\"$name\" value=\"$value\">";
	$Ajax->addUpdate($name, $name, $value);
	if ($echo)
		echo $ret."\n";
	else
		return $ret;
}

function submit($name, $value, $echo=true, $title=false, $async=false, $icon=false)
{
	global $path_to_root;
	
	default_focus($name);
	$submit_str = "<button class=\""
	    .($async ? 'ajaxsubmit' : 'inputsubmit')
		."\""
		.($async === null ? (in_ajax() ? " style='display:none;'" : ' aspect="fallback"' ): 
			($async === 'process' ? 'aspect="process"' : '') )
	    ." name=\"$name\"  id=\"$name\" value=\"$value\""
	    .($title ? " title='$title'" : '')
	    ."><span>$value</span>"
		.($icon ? "<img src='$path_to_root/themes/default/images/$icon'>" : '')
		."</button>\n";
	if ($echo)
		echo $submit_str;
	else
		return $submit_str;
}

function submit_center($name, $value, $echo=true, $title=false, $async=false, $icon=false)
{
	echo "<center>";
	submit($name, $value, $echo, $title, $async, $icon);
	echo "</center>";
}

function submit_center_first($name, $value, $title=false, $async=false, $icon=false)
{
	echo "<center>";
	submit($name, $value, true, $title, $async, $icon);
	echo "&nbsp;";
}

function submit_center_last($name, $value, $title=false, $async=false, $icon=false)
{
	echo "&nbsp;";
	submit($name, $value, true, $title, $async, $icon);
	echo "</center>";
}

function submit_add_or_update($add=true, $title=false, $async=false)
{
	if ($add)
		submit('ADD_ITEM', _("Add new"), true, $title, $async);
	else {
		submit('UPDATE_ITEM', _("Update"), true, $title, $async);
		submit('RESET', _("Cancel"), true, $title, $async);
	}
}

function submit_add_or_update_center($add=true, $title=false, $async=false)
{
	echo "<center>";
	submit_add_or_update($add, $title, $async);
	echo "</center>";
}

/*
function submit_add_or_update_row($add=true)
{
	echo "<tr><td colspan=99 align=center>";
	submit_add_or_update($add);
	echo "</td></tr>\n";
}
*/
function submit_add_or_update_row($add=true, $right=true, $extra="", $title=false, $async=false)
{
	echo "<tr>";
	if ($right)
		echo "<td>&nbsp;</td>\n";
	echo "<td $extra>";
	submit_add_or_update($add, $title, $async);
	echo "</td></tr>\n";
}

function submit_cells($name, $value, $extra="", $title=false, $async=false)
{
	echo "<td $extra>";
	submit($name, $value, true, $title, $async);
	echo "</td>\n";
}

function submit_row($name, $value, $right=true, $extra="", $title=false, $async=false)
{
	echo "<tr>";
	if ($right)
		echo "<td>&nbsp;</td>\n";
	submit_cells($name, $value, $extra, $title, $async);
	echo "</tr>\n";
}

function submit_return($name, $value, $title=false, $async=false)
{
	if (count($_SESSION['Context'])) {
		submit($name, $value, true, $title, $async);
	}
}
//---------------------------------------------------------------------------------

function button($name, $value, $onclick, $title=false)
{
  	default_focus($name);
  	echo "<input type=\"button\" class=\"inputsubmit\" name=\"$name\" value=\"$value\" onclick=\"$onclick\""
	    .($title ? " title='$title'" : '')
	    ." />\n";
}

function button_cell($name, $value, $onclick, $title=false)
{
	echo "<td>";
	button($name, $value, $onclick, $title);
	echo "</td>\n";
}

//-----------------------------------------------------------------------------------

function check_value($name)
{
	if (!isset($_POST[$name]))
		return 0;
	return 1;
}

function check($label, $name, $value=null, $submit_on_change=false, $title=false)
{
  	global $Ajax;

	default_focus($name);
	if ($label)
		echo $label . "  ";

	if ($value === null)
		$value = get_post($name,0);

		echo "<input"
		    .($value == 1 ? ' checked':'')
		    ." type='checkbox' name='$name' value='1'"
		    .($submit_on_change ? " onclick='JsHttpRequest.request(\"_{$name}_update\", this.form);'" : '')
		    .($title ? " title='$title'" : '')
		    ." >\n";
	$Ajax->addUpdate($name, $name, $value);
}

function check_cells($label, $name, $value, $submit_on_change=false, $title=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	check(null, $name, $value, $submit_on_change, $title);
	echo "</td>";
}

function check_row($label, $name, $value, $submit_on_change=false, $title=false)
{
	echo "<tr>";
	check_cells($label, $name, $value, $submit_on_change, $title);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------

function labelheader_cell($label, $params="")
{
	echo "<td class='tableheader' $params>$label</td>\n";
}

function label_cell($label, $params="", $id=null)
{
    global $Ajax;

	if(isset($id))
	{
	    $params .= " id='$id'";
	    $Ajax->addUpdate($id, $id, $label);
	}
	echo "<td $params>$label</td>\n";

	return $label;
}

function amount_cell($label, $bold=false, $params="", $id=null)
{
	if ($bold)
		label_cell("<b>".price_format($label)."</b>", "nowrap align=right ".$params, $id);
	else
		label_cell(price_format($label), "nowrap align=right ".$params, $id);
}

function percent_cell($label, $bold=false, $id=null)
{
	if ($bold)
		label_cell("<b>".percent_format($label)."</b>", "nowrap align=right", $id);
	else
		label_cell(percent_format($label), "nowrap align=right", $id);
}
// 2008-06-15. Changed
function qty_cell($label, $bold=false, $dec=null, $id=null)
{
	if ($dec == null)
		$dec = get_qty_dec();
	if ($bold)
		label_cell("<b>".number_format2($label, $dec)."</b>", "nowrap align=right", $id);
	else
		label_cell(number_format2($label, $dec), "nowrap align=right", $id);
}

function label_cells($label, $value, $params="", $params2="", $id='')
{
	if ($label != null)
		echo "<td $params>$label</td>\n";
	label_cell($value, $params2, $id);
}

function label_row($label, $value, $params="", $params2="", $leftfill=0, $id='')
{
	echo "<tr>";
	label_cells($label, $value, $params, $params2, $id);
	if ($leftfill!=0)
	  	echo "<td colspan=$leftfill></td>";
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------

function text_cells($label, $name, $value=null, $size="", $max="", $title=false, $params="", $post_label="", $disabled="")
{
  	global $Ajax;

	default_focus($name);
	if ($label != null)
		label_cell($label, $params);
	echo "<td>";

	if ($value === null)
		$value = get_post($name);
	echo "<input $disabled type=\"text\" name=\"$name\" size=\"$size\" maxlength=\"$max\" value=\"$value\""
	    .($title ? " title='$title'" : '')
	    .">";

	if ($post_label != "")
		echo " " . $post_label;

	echo "</td>\n";
	$Ajax->addUpdate($name, $name, $value);
}

function text_cells_ex($label, $name, $size, $max=null, $init=null, $title=null, $params=null, $post_label=null, $submit_on_change=false)
{
  	global $Ajax;

	default_focus($name);
	if (!isset($_POST[$name]) || $_POST[$name] == "")
	{
		if ($init)
			$_POST[$name] = $init;
		else
			$_POST[$name] = "";
	}
	if ($label != null)
		label_cell($label, $params);

	if (!isset($max))
		$max = $size;

	echo "<td>";
	$class = $submit_on_change ? 'class="searchbox"' : '';
	echo "<input $class type=\"text\" name=\"$name\" size=\"$size\" maxlength=\"$max\" value=\"" . $_POST[$name]. "\""
	 .($title ? " title='$title'": '')." >";

	if ($post_label)
		echo " " . $post_label;

	echo "</td>\n";
	$Ajax->addUpdate($name, $name, $_POST[$name]);
}

function text_row($label, $name, $value, $size, $max, $title=null, $params="", $post_label="")
{
	echo "<tr>";

	text_cells($label, $name, $value, $size, $max, $title, $params, $post_label);

	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------

function text_row_ex($label, $name, $size, $max=null, $title=null, $value=null, $params=null, $post_label=null)
{
	echo "<tr>";

	text_cells_ex($label, $name, $size, $max, $value, $title, $params, $post_label);

	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------

function date_cells($label, $name, $title = null, $init=null, $inc_days=0, 
	$inc_months=0, $inc_years=0, $params=null, $submit_on_change=false)
{
	global $use_date_picker, $path_to_root;
	if (!isset($_POST[$name]) || $_POST[$name] == "")
	{
		if (!$init)
		{
			if ($inc_years == 1001)
				$_POST[$name] = null;
			else
			{
				$dd = Today();
				if ($inc_days != 0)
					$dd = add_days($dd, $inc_days);
				if ($inc_months != 0)
					$dd = add_months($dd, $inc_months);
				if ($inc_years != 0)
					$dd = add_years($dd, $inc_years);
				$_POST[$name] = $dd;
			}
		}
		else
			$_POST[$name] = $init;
	}
	if ($use_date_picker)
		$post_label = "<a tabindex='-1' href=\"javascript:date_picker(document.getElementsByName('$name')[0]);\">"
		. "	<img src='$path_to_root/themes/default/images/cal.gif' width='16' height='16' border='0' alt='"._('Click Here to Pick up the date')."'></a>\n";
	else
		$post_label = "";
	text_cells_ex($label, $name, 9, 12, $_POST[$name], $title, $params, $post_label, $submit_on_change);
}

function date_row($label, $name, $title=null, $init=null, $inc_days=0, $inc_months=0, 
	$inc_years=0, $params=null, $submit_on_change=false)
{
	echo "<tr>";
	date_cells($label, $name, $title, $init, $inc_days, $inc_months, 
		$inc_years, $params, $submit_on_change);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------

function ref_cells($label, $name, $title=null, $init=null, $params=null, $submit_on_change=false)
{
	text_cells_ex($label, $name, 16, 18, $init, $title, $params, null, $submit_on_change);
}

//-----------------------------------------------------------------------------------

function ref_row($label, $name, $title=null, $init=null, $submit_on_change=false)
{
	echo "<tr>";
	ref_cells($label, $name, $title, $init, null, $submit_on_change);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------

function percent_row($label, $name, $init=null)
{

	if (!isset($_POST[$name]) || $_POST[$name]=="")
	{
		$_POST[$name] = $init == null ? '' : $init;
	}

	small_amount_row($label, $name, $_POST[$name], null, "%", user_percent_dec());
}

function amount_cells_ex($label, $name, $size, $max=null, $init=null, $params=null, $post_label=null, $dec=null)
{
	global $Ajax;

	if (!isset($dec))
	  	$dec = user_price_dec();
	if (!isset($_POST[$name]) || $_POST[$name] == "")
	{
		if ($init !== null)
			$_POST[$name] = $init;
		else
			$_POST[$name] = '';
	}
	if ($label != null)
		label_cell($label, $params);

	if (!isset($max))
		$max = $size;

	echo "<td>";

	echo "<input class='amount' type=\"text\" name=\"$name\" size=\"$size\" maxlength=\"$max\" dec=\"$dec\" value=\"" . $_POST[$name]. "\">";

	if ($post_label)
		echo " " . $post_label;

	echo "</td>\n";
	$Ajax->addUpdate($name, $name, $_POST[$name]);
	$Ajax->addAssign($name, $name, 'dec', $dec);
}


//-----------------------------------------------------------------------------------

function amount_cells($label, $name, $init=null, $params=null, $post_label=null, $dec=null)
{
	amount_cells_ex($label, $name, 15, 15, $init, $params, $post_label, $dec);
}

function amount_row($label, $name, $init=null, $params=null, $post_label=null, $dec=null)
{
	echo "<tr>";
	amount_cells($label, $name, $init, $params, $post_label, $dec);
	echo "</tr>\n";
}

function small_amount_row($label, $name, $init=null, $params=null, $post_label=null, $dec=null)
{
	echo "<tr>";
	small_amount_cells($label, $name, $init, $params, $post_label, $dec);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------

function qty_cells($label, $name, $init=null, $params=null, $post_label=null, $dec=null)
{
	if (!isset($dec))
	  	$dec = user_qty_dec();

	amount_cells_ex($label, $name, 15, 15, $init, $params, $post_label, $dec);
}

function qty_row($label, $name, $init=null, $params=null, $post_label=null, $dec=null)
{
	if (!isset($dec))
	  	$dec = user_qty_dec();

	echo "<tr>";
	amount_cells($label, $name, $init, $params, $post_label, $dec);
	echo "</tr>\n";
}

function small_qty_row($label, $name, $init=null, $params=null, $post_label=null, $dec=null)
{
	if (!isset($dec))
	  	$dec = user_qty_dec();

	echo "<tr>";
	small_amount_cells($label, $name, $init, $params, $post_label, $dec);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------

function small_amount_cells($label, $name, $init=null, $params=null, $post_label=null, $dec=null)
{
	amount_cells_ex($label, $name, 7, 12, $init, $params, $post_label, $dec);
}

//-----------------------------------------------------------------------------------

function small_qty_cells($label, $name, $init=null, $params=null, $post_label=null, $dec=null)
{
  	if (!isset($dec))
	  	$dec = user_qty_dec();
	amount_cells_ex($label, $name, 7, 12, $init, $params, $post_label, $dec);
}

//-----------------------------------------------------------------------------------

function textarea_cells($label, $name, $value, $cols, $rows, $title = null, $params="")
{
  	global $Ajax;

	default_focus($name);
	if ($label != null)
		echo "<td $params>$label</td>\n";
	if ($value == null)
		$value = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	echo "<td><textarea name='$name' cols='$cols' rows='$rows'"
	.($title ? " title='$title'" : '')
	.">$value</textarea></td>\n";
	$Ajax->addUpdate($name, $name, $value);
}

function textarea_row($label, $name, $value, $cols, $rows, $title=null, $params="")
{
	echo "<tr>";
	textarea_cells($label, $name, $value, $cols, $rows, $title, $params);
	echo "</tr>\n";
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


?>