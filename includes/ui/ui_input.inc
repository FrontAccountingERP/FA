<?php

//---------------------------------------------------------------------------------

function hidden($name, $value)
{
	echo "<input type=\"hidden\" name=\"$name\" value=\"$value\">\n";
}

//---------------------------------------------------------------------------------

function submit($name, $value, $echo=true)
{
	$submit_str = "<input type=\"submit\" class=\"inputsubmit\" name=\"$name\" value=\"$value\">\n";
	if ($echo)
		echo $submit_str;
	else
		return $submit_str;
}

function submit_center($name, $value, $echo=true)
{
	echo "<center>";
	submit($name, $value, $echo);
	echo "</center>";
}

function submit_center_first($name, $value)
{
	echo "<center>";
	submit($name, $value);
	echo "&nbsp;";
}

function submit_center_last($name, $value)
{
	echo "&nbsp;";
	submit($name, $value);
	echo "</center>";
}

function submit_add_or_update($add=true)
{
	if ($add)
		submit('ADD_ITEM', _("Save"));
	else
		submit('UPDATE_ITEM', _("Update"));
}

function submit_add_or_update_center($add=true)
{
	echo "<center>";
	submit_add_or_update($add);
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
function submit_add_or_update_row($add=true, $right=true, $extra="")
{
	echo "<tr>";
	if ($right)
		echo "<td>&nbsp;</td>\n";
	echo "<td $extra>";
	submit_add_or_update($add);
	echo "</td></tr>\n";
}

function submit_cells($name, $value, $extra="")
{
	echo "<td $extra>";
	submit($name, $value);
	echo "</td>\n";
}

function submit_row($name, $value, $right=true, $extra="")
{
	echo "<tr>";
	if ($right)
		echo "<td>&nbsp;</td>\n";
	submit_cells($name, $value, $extra);
	echo "</tr>\n";
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

function label_cell($label, $params="")
{
	echo "<td $params>$label</td>\n";
}

function amount_cell($label, $bold=false)
{
	if ($bold)
		label_cell("<b>".number_format2($label,user_price_dec())."</b>", "nowrap align=right");
	else
		label_cell(number_format2($label,user_price_dec()), "nowrap align=right");
}

function percent_cell($label, $bold=false)
{
	if ($bold)
		label_cell("<b>".number_format2($label,user_percent_dec())." %</b>", "nowrap align=right");
	else
		label_cell(number_format2($label,user_percent_dec())." %", "nowrap align=right");
}

function qty_cell($label, $bold=false)
{
	if ($bold)
		label_cell("<b>".number_format2($label,user_qty_dec())."</b>", "nowrap align=right");
	else
		label_cell(number_format2($label,user_qty_dec()), "nowrap align=right");
}

function label_cells($label, $value, $params="", $params2="")
{
	if ($label != null)
		echo "<td $params>$label</td>\n";
	echo "<td $params2>$value</td>\n";
}

function label_row($label, $value, $params="", $params2="")
{
	echo "<tr>";
	label_cells($label, $value, $params, $params2);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------

function text_cells($label, $name, $value, $size="", $max="", $params="", $post_label="", $disabled="")
{
	if ($label != null)
		label_cell($label, $params);
	echo "<td>";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	echo "<input $disabled type=\"text\" name=\"$name\" size=\"$size\" maxlength=\"$max\" value=\"$value\">";

	if ($post_label != "")
		echo " " . $post_label;

	echo "</td>\n";
}

function text_cells_ex($label, $name, $size, $max=null, $init=null, $params=null, $post_label=null)
{
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
		$post_label = "<a href=\"javascript:date_picker(document.forms[0].$name);\">"
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
		if ($init)
			$_POST[$name] = $init;
	}

	text_row($label, $name, $_POST[$name], 6, 6, "", "%");
}

//-----------------------------------------------------------------------------------

function amount_cells($label, $name, $init=null, $params=null, $post_label=null)
{
	text_cells_ex($label, $name, 15, 15, $init, $params, $post_label);
}

function amount_row($label, $name, $init=null, $params=null, $post_label=null)
{
	echo "<tr>";
	amount_cells($label, $name, $init, $params, $post_label);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------

function small_amount_cells($label, $name, $init=null, $params=null, $post_label=null)
{
	text_cells_ex($label, $name, 7, 12, $init, $params, $post_label);
}

//-----------------------------------------------------------------------------------

function textarea_cells($label, $name, $value, $cols, $rows, $params="")
{
	if ($label != null)
		echo "<td $params>$label</td>\n";
	if ($value == null)
		$value = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	echo "<td><textarea name='$name' cols='$cols' rows='$rows'>$value</textarea></td>\n";
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
	echo "<tr><td>$label</td>\n";
	echo "<td>";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	echo "<input type=\"text\" name=\"$name\" size=\"$size\" maxlength=\"$max\" value=\"$value\">   ";

	submit($input_name, $input_value);

	echo "</td></tr>\n";
}

//-----------------------------------------------------------------------------------


?>