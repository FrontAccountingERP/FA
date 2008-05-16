<?php

function get_post($name, $dflt='') {
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
function find_submit($prefix, $numeric=true) {

    foreach($_POST as $postkey=>$postval ) {
	if (strpos($postkey, $prefix) === 0) {
		$id = substr($postkey, strlen($prefix));
		return $numeric ? (int)$id : $id;
	}
    }
    return $numeric ? -1 : null;
}

//------------------------------------------------------------------------------
//
//	Read numeric value from user formatted input
//
function input_num($postname=null) {
	if (!isset($_POST[$postname])) 
	  return null;

    return user_numeric($_POST[$postname]);
}

//---------------------------------------------------------------------------------

function hidden($name, $value)
{
  global $Ajax;
	echo "<input type=\"hidden\" name=\"$name\" value=\"$value\">\n";
	$Ajax->addUpdate($name, $name, $value);
}

//---------------------------------------------------------------------------------

function submit($name, $value, $echo=true, $async=false)
{
	default_focus($name);
	$submit_str = "<input type=\"submit\" class=\""
	    .($async ? 'ajaxsubmit' : 'inputsubmit') 
	    ."\" name=\"$name\"  id=\"$name\" value=\"$value\" >\n";
	if ($echo)
		echo $submit_str;
	else
		return $submit_str;
}

function submit_center($name, $value, $echo=true, $async=false)
{
	echo "<center>";
	submit($name, $value, $echo, $async);
	echo "</center>";
}

function submit_center_first($name, $value, $async=false)
{
	echo "<center>";
	submit($name, $value, true, $async);
	echo "&nbsp;";
}

function submit_center_last($name, $value, $async=false)
{
	echo "&nbsp;";
	submit($name, $value, true, $async);
	echo "</center>";
}

function submit_add_or_update($add=true, $async=false)
{
	if ($add)
		submit('ADD_ITEM', _("Save"), true, $async);
	else
		submit('UPDATE_ITEM', _("Update"), true, $async);
}

function submit_add_or_update_center($add=true, $async=false)
{
	echo "<center>";
	submit_add_or_update($add, $async);
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
function submit_add_or_update_row($add=true, $right=true, $extra="", $async=false)
{
	echo "<tr>";
	if ($right)
		echo "<td>&nbsp;</td>\n";
	echo "<td $extra>";
	submit_add_or_update($add, $async);
	echo "</td></tr>\n";
}

function submit_cells($name, $value, $extra="", $async=false)
{
	echo "<td $extra>";
	submit($name, $value, true, $async);
	echo "</td>\n";
}

function submit_row($name, $value, $right=true, $extra="", $async=false)
{
	echo "<tr>";
	if ($right)
		echo "<td>&nbsp;</td>\n";
	submit_cells($name, $value, $extra);
	echo "</tr>\n";
}
//---------------------------------------------------------------------------------

function button($name, $value, $onclick)
{
  default_focus($name);
  echo "<input type=\"button\" class=\"inputsubmit\" name=\"$name\" value=\"$value\" onclick=\"$onclick\" />\n";
}

function button_cell($name, $value, $onclick)
{
	echo "<td>";
	button($name, $value, $onclick);
	echo "</td>\n";
}

//-----------------------------------------------------------------------------------

function check_value($name)
{
	if (!isset($_POST[$name]))
		return 0;
	return 1;
}

function check($label, $name, $value, $submit_on_change=false)
{
  global $Ajax;

	default_focus($name);
	if ($label)
		echo $label . "  ";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? 0 : $_POST[$name]);
	if ($value == 1)
		echo "<input checked type='checkbox' name='$name' value='1' ";
	else
		echo "<input type='checkbox' name='$name' value='1'";
	if ($submit_on_change)
		echo " onclick='this.form.submit();' ";
	echo " >\n";
	$Ajax->addUpdate($name, $name, $value);
}

function check_cells($label, $name, $value, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	check(null, $name, $value, $submit_on_change);
	echo "</td>";
}

function check_row($label, $name, $value, $submit_on_change=false)
{
	echo "<tr>";
	check_cells($label, $name, $value, $submit_on_change);
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
    
	if(isset($id)) {
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

function qty_cell($label, $bold=false, $dec=null, $id=null)
{
	if ($bold)
		label_cell("<b>".qty_format($label, $dec)."</b>", "nowrap align=right", $id);
	else
		label_cell(qty_format($label, $dec), "nowrap align=right", $id);
}

function label_cells($label, $value, $params="", $params2="")
{
	if ($label != null)
		echo "<td $params>$label</td>\n";
	echo "<td $params2>$value</td>\n";
}

function label_row($label, $value, $params="", $params2="", $leftfill=0)
{
	echo "<tr>";
	label_cells($label, $value, $params, $params2);
	if($leftfill!=0)
	  echo "<td colspan=$leftfill></td>";
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------

function text_cells($label, $name, $value, $size="", $max="", $params="", $post_label="", $disabled="")
{
  global $Ajax;

	default_focus($name);
	if ($label != null)
		label_cell($label, $params);
	echo "<td>";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	echo "<input $disabled type=\"text\" name=\"$name\" size=\"$size\" maxlength=\"$max\" value=\"$value\">";

	if ($post_label != "")
		echo " " . $post_label;

	echo "</td>\n";
	$Ajax->addUpdate($name, $name, $value);
}

function text_cells_ex($label, $name, $size, $max=null, $init=null, $params=null, $post_label=null)
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

	echo "<input type=\"text\" name=\"$name\" size=\"$size\" maxlength=\"$max\" value=\"" . $_POST[$name]. "\">";

	if ($post_label)
		echo " " . $post_label;

	echo "</td>\n";
	$Ajax->addUpdate($name, $name, $_POST[$name]);
}

function text_row($label, $name, $value, $size, $max, $params="", $post_label="")
{
	echo "<tr>";

	text_cells($label, $name, $value, $size, $max, $params, $post_label);

	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------

function text_row_ex($label, $name, $size, $max=null, $value=null, $params=null, $post_label=null)
{
	echo "<tr>";

	text_cells_ex($label, $name, $size, $max, $value, $params, $post_label);

	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------

function date_cells($label, $name, $init=null, $inc_days=0, $inc_months=0, $inc_years=0, $params=null)
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
		$post_label = "<a tabindex='-1' href=\"javascript:date_picker(document.forms[0].$name);\">"
		. "	<img src='$path_to_root/themes/default/images/cal.gif' width='16' height='16' border='0' alt='"._('Click Here to Pick up the date')."'></a>\n";
	else
		$post_label = "";
	text_cells_ex($label, $name, 9, 12, $_POST[$name], $params, $post_label);
}

function date_row($label, $name, $init=null, $inc_days=0, $inc_months=0, $inc_years=0, $params=null)
{
	echo "<tr>";
	date_cells($label, $name, $init, $inc_days, $inc_months, $inc_years, $params);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------

function ref_cells($label, $name, $init=null, $params=null)
{
	text_cells_ex($label, $name, 16, 18, $init, $params);
}

//-----------------------------------------------------------------------------------

function ref_row($label, $name, $init=null)
{
	echo "<tr>";
	ref_cells($label, $name, $init);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------

function percent_row($label, $name, $init=null)
{

	if (!isset($_POST[$name]) || $_POST[$name]=="")
	{
			$_POST[$name] = $init== null ? '' : $init;
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
		if ($init)
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
	if(!isset($dec))
	  $dec = user_qty_dec();

	amount_cells_ex($label, $name, 15, 15, $init, $params, $post_label, $dec);
}

function qty_row($label, $name, $init=null, $params=null, $post_label=null, $dec=null)
{
	if(!isset($dec))
	  $dec = user_qty_dec();

	echo "<tr>";
	amount_cells($label, $name, $init, $params, $post_label, $dec);
	echo "</tr>\n";
}

function small_qty_row($label, $name, $init=null, $params=null, $post_label=null, $dec=null)
{
	if(!isset($dec))
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

function textarea_cells($label, $name, $value, $cols, $rows, $params="")
{
  global $Ajax;
  
	default_focus($name);
	if ($label != null)
		echo "<td $params>$label</td>\n";
	if ($value == null)
		$value = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	echo "<td><textarea name='$name' cols='$cols' rows='$rows'>$value</textarea></td>\n";
	$Ajax->addUpdate($name, $name, $value);
}

function textarea_row($label, $name, $value, $cols, $rows, $params="")
{
	echo "<tr>";
	textarea_cells($label, $name, $value, $cols, $rows, $params);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------------


?>