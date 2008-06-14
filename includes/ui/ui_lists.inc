<?php

include_once($path_to_root . "/includes/banking.inc");
include_once($path_to_root . "/includes/reserved.inc");
include_once($path_to_root . "/includes/types.inc");
include_once($path_to_root . "/includes/current_user.inc");

$_search_button = "<input type='submit' class='combo_submit' style='border:0;background:url($path_to_root/themes/"
	."%s/images/locate.png) no-repeat;%s' name='%s' value=' ' title='"._("Set filter")."'> ";

$_select_button = "<input type='submit' class='combo_select' style='border:0;background:url($path_to_root/themes/"
	."%s/images/button_ok.png) no-repeat;%s' name='%s' value=' ' title='"._("Select")."'> ";

$all_items = reserved_words::get_all();

//----------------------------------------------------------------------------
//	Universal combo generator
//	$sql must return selector values and selector texts in columns 0 & 1
//	Options are merged with defaults.

function combo_input($name, $selected_id, $sql, $valfield, $namefield,
	$options=null)
{
global $Ajax;

$opts = array(		// default options
	'where'=> array(),		// additional constraints
	'order' => $namefield,	// list sort order
		// special option parameters
	'spec_option'=>false, 	// option text or false
	'spec_id' => 0,		// option id
		// submit on select parameters
	'select_submit' => false, //submit on select: true/false
	'async' => true,	// select update via ajax (true) vs _page_body reload
		// search box parameters
	'sel_hint' => null,
	'search_box' => false, 	// name or true/false
	'type' => 0,	// type of extended selector:
		// 0 - with (optional) visible search box, search by id
		// 1 - with hidden search box, search by option text
		// 2 - TODO reverse: box with hidden selector available via enter; this
		// would be convenient for optional ad hoc adding of new item
	'search_submit' => true, //search submit button: true/false
	'size' => 8,	// size and max of box tag
	'max' => 50,
	'cells' => false,	// combo displayed as 2 <td></td> cells
	'search' => array(), // sql field names to search
	'format' => null, 	 // format functions for regular options
	'disabled' => false, // FIX todo
	'box_hint' => null // box/selectors hints; null = std see below
);
// ------ merge options with defaults ----------
	$opts = array_merge($opts, $options);
	if (!is_array($opts['where']))  $opts['where'] = array($opts['where']);

	$search_box = $opts['search_box']===true ? '_'.$name.'_edit' : $opts['search_box'];
	$search_submit = $opts['search_submit']===true ? '_'.$name.'_button' : $opts['search_submit'];
	$select_submit =  $opts['select_submit']===true ? '_'.$name.'_update' : $opts['select_submit'];
	$spec_id = $opts['spec_id'];
	$spec_option = $opts['spec_option'];
	$by_id = ($opts['type'] == 0);
	$class = $by_id ? 'combo':'combo2';

	if(!count($opts['search'])) {
		$opts['search'] = array($by_id ? $valfield : $namefield);
	}
	if ($opts['sel_hint'] === null)
		$opts['sel_hint'] = $by_id || $search_box==false ?
		'' : _('Press Space tab for search pattern entry');

	if ($opts['box_hint'] === null)
		$opts['box_hint'] = $search_box ?
			($by_id ? _('Enter code fragment to search or * for all')
			: _('Enter description fragment to search or * for all')) :'';

	if ($selected_id == null) {
		$selected_id = get_post($name, null);
	}
	$txt = get_post($search_box);
	$rel = '';
	$limit = '';

	if (isset($_POST[$select_submit])) {
		if (!$opts['async'])
			$Ajax->activate('_page_body');
		else
			$Ajax->activate($name);
	}
	if ($search_box) {
		// search related sql modifications

	$rel = "rel='$search_box'"; // set relation to list
   if ($opts['search_submit']) {
	// if selected from list - set focus on next field
	if (isset($_POST[$search_submit])) {
		$Ajax->activate($name);
	}
	if ($txt == '') {
		if ($spec_option === false)
		$limit = ' LIMIT 1';
		else
		$opts['where'][] = $valfield . "='". get_post($name, $spec_id)."'";
	}
	else
		if ($txt != '*') {

		foreach($opts['search'] as $i=> $s)
			$opts['search'][$i] = $s . " LIKE '%{$txt}%'";
		$opts['where'][] = '('. implode($opts['search'], ' OR ') . ')';
		}
	}
   }
	// sql completion
	if (count($opts['where'])) {
		$sql .= strpos($sql, 'WHERE')==false ? ' WHERE ':' AND ';
		$sql .= '('. implode($opts['where'], ' AND ') . ')';
	}
	if ($opts['order'] != false) {
		$sql .= ' ORDER BY '.$opts['order'];
	}

	$sql .= $limit;
	// ------ make selector ----------
	$selector = $first_opt = '';
	$first_id = false;
	$found = false;
//if($name=='code_id')    display_error($sql);
	if($result = db_query($sql)) {
		while ($contact_row = db_fetch($result)) {
			$value = $contact_row[0];
			$descr = $opts['format']==null ?  $contact_row[1] :
			call_user_func($opts['format'], $contact_row);
			$sel = '';
			if ($selected_id === $value) {
				$sel = 'selected';
				$found = $value;
			}
			if ($first_id === false) {
				$first_id = $value;
				$first_opt = $descr;
			}
			$selector .= "<option $sel value='$value'>$descr</option>\n";
		}
		db_free_result($result);
	}
	// Prepend special option.
	if ($spec_option !== false) { // if special option used - add it
		$first_id = $spec_id;
		$first_opt = $spec_option;
		$sel = $found===false ? 'selected' : '';
		$selector = "<option $sel value='$spec_id'>$spec_option</option>\n"
			. $selector;
	}

	if ($found === false) {
		$_POST[$name] = $first_id;
	}

	if ($by_id) {
		$txt = $_POST[$name];
		if ($search_box)
			$Ajax->addUpdate($name, $search_box, $txt);
	}

	$selector = "<select name='$name' class='$class' title='"
		. $opts['sel_hint']."' $rel>".$selector."</select>\n";

	$Ajax->addUpdate($name, "_{$name}_sel", $selector);

	// because of bug which M$ cannot fix since IE 5.0
	// we must embed whole selector in span tags to enable proper ajax update
	$selector = "<span id='_{$name}_sel'>".$selector."</span>\n";

	if ($select_submit != false) { // if submit on change is used - add select button
		global $_select_button;
	// button class selects form reload/ajax selector update
		$selector .= sprintf($_select_button, user_theme(),
			(in_ajax() ? 'display:none;':''),
			$select_submit)."\n";
	}
// ------ make combo ----------

	$edit_entry = '';
	if ($search_box != false) {
		$edit_entry = "<input type='text' name='$search_box' id='$search_box' size='".
			$opts['size']."' maxlength='".$opts['max'].
			"' value='$txt' class='$class' rel='$name' autocomplete='off' title='"
			.$opts['box_hint']."'"
			.(in_ajax() && !$by_id ? " style=display:none;":'')
			.">\n";
		if ($search_submit != false) {
			global $_search_button;
			$edit_entry .= sprintf($_search_button, user_theme(),
				(in_ajax() ? 'display:none;':''),
				$search_submit)."\n";
		}
	}
	default_focus($name);
	$str = $selector;
	if ($search_box && $opts['cells'])
		echo ($edit_entry!='' ? "<td>$edit_entry</td>" : '')."<td>$selector</td>";
	else
		echo $edit_entry.$selector;

	return $str;
}

function supplier_list($name, $selected_id, $spec_option=false, $submit_on_change=false)
{
	global $all_items;

	$sql = "SELECT supplier_id, supp_name, curr_code FROM ".TB_PREF."suppliers ";

	$mode = get_company_pref('no_supplier_list');

	return combo_input($name, $selected_id, $sql, 'supplier_id', 'supp_name',
	array(
		'format' => '_format_add_curr',
		'search_box' => $mode!=0,
		'type' => 1,
		'spec_option' => $spec_option === true ? _("All Suppliers") : $spec_option,
		'spec_id' => $all_items,
		'select_submit'=> $submit_on_change,
		'async' => false,
		'sel_hint' => $mode ? _('Press Space tab to filter by name fragment') :
		_('Select supplier')
		));
}

function _format_add_curr($row)
{
static $company_currency;

if ($company_currency == null)
{
	$company_currency = get_company_currency();
}
return $row[1] . ($row[2] == $company_currency ?
	'' : ("&nbsp;-&nbsp;" . $row[2]));
}

function supplier_list_cells($label, $name, $selected_id, $all_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td><td>\n";
	$str = supplier_list($name, $selected_id, $all_option, $submit_on_change);
		echo "</td>\n";
	return $str;
}

function supplier_list_row($label, $name, $selected_id, $all_option = false, $submit_on_change=false)
{
echo "<tr><td>$label</td><td>";
	$str = supplier_list($name, $selected_id, $all_option, $submit_on_change );
echo "</td></tr>\n";
return $str;
}
//----------------------------------------------------------------------------------------------

function customer_list($name, $selected_id, $spec_option=false, $submit_on_change=false)
{
	global $all_items;

	$sql = "SELECT debtor_no, name, curr_code FROM ".TB_PREF."debtors_master ";

	$mode = get_company_pref('no_customer_list');

return combo_input($name, $selected_id, $sql, 'debtor_no', 'name',
	array(
	    'format' => '_format_add_curr',
		'search_box' => $mode!=0,
		'type' => 1,
		'size' => 20,
		'spec_option' => $spec_option === true ? _("All Customers") : $spec_option,
		'spec_id' => $all_items,
		'select_submit'=> $submit_on_change,
		'async' => false,
		'sel_hint' => $mode ? _('Press Space tab to filter by name fragment') :
		_('Select customer')
	) );
}

function customer_list_cells($label, $name, $selected_id, $all_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td nowrap>";
	$str = customer_list($name, $selected_id, $all_option, $submit_on_change);
	echo "</td>\n";
	return $str;
}

function customer_list_row($label, $name, $selected_id, $all_option = false, $submit_on_change=false)
{
echo "<tr><td>$label</td><td nowrap>";
$str = customer_list($name, $selected_id, $all_option, $submit_on_change);
echo "</td>\n</tr>\n";
return $str;
}

//------------------------------------------------------------------------------------------------

function customer_branches_list($customer_id, $name, $selected_id,
	$spec_option = true, $enabled=true, $submit_on_change=false)
{
	global $all_items;

	$sql = "SELECT branch_code, br_name FROM ".TB_PREF."cust_branch
		WHERE debtor_no='" . $customer_id . "' ";

	$where = $enabled ? array("disable_trans = 0") : array();
return  combo_input($name, $selected_id, $sql, 'branch_code', 'br_name',
	array(
		'where' => $where,
		'spec_option' => $spec_option === true ? _('All branches') : $spec_option,
		'spec_id' => $all_items,
		'select_submit'=> $submit_on_change,
		'sel_hint' => _('Select customer branch')
//		  'async' => false
	) );
}
//------------------------------------------------------------------------------------------------

function customer_branches_list_cells($label,$customer_id, $name, $selected_id, $all_option = true, $enabled=true, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$ret = customer_branches_list($customer_id, $name, $selected_id, $all_option, $enabled, $submit_on_change);
	echo "</td>\n";
	return $ret;
}

function customer_branches_list_row($label,$customer_id, $name, $selected_id, $all_option = true, $enabled=true, $submit_on_change=false)
{
	echo "<tr>";
	$ret = customer_branches_list_cells($label, $customer_id, $name, $selected_id, $all_option, $enabled, $submit_on_change);
	echo "</tr>";
	return $ret;
}

//------------------------------------------------------------------------------------------------

function locations_list($name, $selected_id, $all_option=false, $submit_on_change=false)
{
	global $all_items;

	$sql = "SELECT loc_code, location_name FROM ".TB_PREF."locations";

return combo_input($name, $selected_id, $sql, 'loc_code', 'location_name',
	array(
		'spec_option' => $all_option === true ? _("All Locations") : $all_option,
		'spec_id' => $all_items,
		'select_submit'=> $submit_on_change,
		'async' => false
	) );
}

function locations_list_cells($label, $name, $selected_id, $all_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = locations_list($name, $selected_id, $all_option, $submit_on_change);
	echo "</td>\n";
	return $str;
}

function locations_list_row($label, $name, $selected_id, $all_option=false, $submit_on_change=false)
{
	echo "<tr>";
	$str = locations_list_cells($label, $name, $selected_id, $all_option, $submit_on_change);
	echo "</tr>\n";
	return $str;
}

//-----------------------------------------------------------------------------------------------

function currencies_list($name, $selected_id, $submit_on_change=false)
{
	$sql = "SELECT curr_abrev, currency FROM ".TB_PREF."currencies";

// default to the company currency
	if ($selected_id == "")
	{
			$selected_id = get_company_currency();
			$_POST[$name] = $selected_id;
	}

return combo_input($name, $selected_id, $sql, 'curr_abrev', 'currency',
	array(
		'select_submit'=> $submit_on_change,
		'async' => false
	) );
}

function currencies_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = currencies_list($name, $selected_id);
	echo "</td>\n";
	return $str;
}

function currencies_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	$str = currencies_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
	return $str;
}

//---------------------------------------------------------------------------------------------------

function fiscalyears_list($name, $selected_id, $submit_on_change=false)
{

	$sql = "SELECT * FROM ".TB_PREF."fiscal_year";

// default to the company current fiscal year
	if ($selected_id == "")
	{
			$selected_id = get_company_pref('f_year');
			$_POST[$name] = $selected_id;
	}

return combo_input($name, $selected_id, $sql, 'id', '',
	array(
		'order' => 'begin',
		'format' => '_format_fiscalyears',
		'select_submit'=> $submit_on_change,
		'async' => false
	) );
}

function _format_fiscalyears($row)
{
	return sql2date($row[1]) . "&nbsp;-&nbsp;" . sql2date($row[2])
	. "&nbsp;&nbsp;" . ($row[3] ? _('Closed') : _('Active')) . "</option>\n";
}

function fiscalyears_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = fiscalyears_list($name, $selected_id);
	echo "</td>\n";
	return $str;
}

function fiscalyears_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	$str = fiscalyears_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
	return $str;
}
//------------------------------------------------------------------------------------

function dimensions_list($name, $selected_id, $no_option=false, $showname=' ',
	$submit_on_change=false, $showclosed=false, $showtype=1)
{
$sql = "SELECT id, CONCAT(reference,'  ',name) as ref FROM ".TB_PREF."dimensions";

$options = array(
	'order' => 'reference',
	'spec_option'=>$no_option ? $showname : false,
	'spec_id' => 0,
	'select_submit'=> $submit_on_change,
	'async' => false,
	);

	if ($showclosed)
	$options['where'][] = "closed=0";
	if($showtype)
	$options['where'][] = "type_=$showtype";

	return combo_input($name, $selected_id, $sql, 'id', 'ref', $options);
}

function dimensions_list_cells($label, $name, $selected_id, $no_option=false, $showname=null,
	$showclosed=false, $showtype=0)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = dimensions_list($name, $selected_id, $no_option, $showname, false, $showclosed, $showtype);
	echo "</td>\n";
	return $str;
}

function dimensions_list_row($label, $name, $selected_id, $no_option=false, $showname=null,
	$showclosed=false, $showtype=0)
{
	echo "<tr>\n";
	$str = dimensions_list_cells($label, $name, $selected_id, $no_option, $showname,
		$showclosed, $showtype);
	echo "</tr>\n";
	return $str;
}

//---------------------------------------------------------------------------------------------------

function stock_items_list($name, $selected_id, $all_option=false, $submit_on_change=false, $opts=array())
{
	global $all_items;

	$sql = "SELECT stock_id, s.description, c.description
			FROM ".TB_PREF."stock_master s,".TB_PREF."stock_category c WHERE s.category_id=c.category_id";

	return combo_input($name, $selected_id, $sql, 'stock_id', 's.description',
	array_merge(
	  array(
		'format' => '_format_stock_items',
		'spec_option' => $all_option===true ?  _("All Items") : $all_option,
		'spec_id' => $all_items,
		'search_box' => true,
		'search' => array("stock_id", "c.description","s.description"),
		'search_submit' => get_company_pref('no_item_list')!=0,
		'size'=>10,
		'select_submit'=> $submit_on_change
	  ), $opts) );
}

function _format_stock_items($row)
{
	return (user_show_codes() ?  ($row[0] . "&nbsp;-&nbsp;") : "")
		. $row[2] . "&nbsp;-&nbsp;" . $row[1];
}

function stock_items_list_cells($label, $name, $selected_id, $all_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	$str = stock_items_list($name, $selected_id, $all_option, $submit_on_change,
	array('cells'=>true));
	return $str;
}
/*
function stock_items_list_row($label, $name, $selected_id, $all_option=false, $submit_on_change=false)
{
	echo "<tr>\n";
	$str = stock_items_list_cells($label, $name, $selected_id, $all_option, $submit_on_change);
	echo "</tr>\n";
	return $str;
}
*/
//------------------------------------------------------------------------------------

function base_stock_items_list($where, $name, $selected_id,
	$all_option=false, $submit_on_change=false)
{
	global $all_items;

	$sql = "SELECT stock_id, s.description, c.description
		FROM ".TB_PREF."stock_master s,".TB_PREF."stock_category c WHERE
		s.category_id=c.category_id";

	return combo_input($name, $selected_id, $sql, 'stock_id', 's.description',
	array(
		'format' => '_format_stock_items',
		'spec_option' => $all_option==true ?  _("All Items") : $all_option,
		'spec_id' => $all_items,
		'select_submit'=> $submit_on_change,
		'where' => $where
	) );
}
//------------------------------------------------------------------------------------

function stock_bom_items_list($name, $selected_id, $all_option=false, $submit_on_change=false)
{

	return base_stock_items_list("(s.mb_flag='M' OR s.mb_flag='K')",
		$name, $selected_id, $all_option, $submit_on_change);
}
/*
function stock_bom_items_list_cells($label, $name, $selected_id, $all_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = stock_bom_items_list($name, $selected_id, $all_option, $submit_on_change);
	echo "</td>\n";
	return $str;
}

function stock_bom_items_list_row($label, $name, $selected_id, $all_option=false, $submit_on_change=false)
{
	echo "<tr>\n";
	$str = stock_bom_items_list_cells($label, $name, $selected_id, $all_option, $submit_on_change);
	echo "</tr>\n";
	return $str;
}
*/
//------------------------------------------------------------------------------------

function stock_manufactured_items_list($name, $selected_id,
	$all_option=false, $submit_on_change=false)
{
	return base_stock_items_list("s.mb_flag='M'",
		$name, $selected_id, $all_option, $submit_on_change);
}

function stock_manufactured_items_list_cells($label, $name, $selected_id,
				$all_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = stock_manufactured_items_list($name, $selected_id, $all_option,
				$submit_on_change);
	echo "</td>\n";
	return $str;
}

function stock_manufactured_items_list_row($label, $name, $selected_id,
		$all_option=false, $submit_on_change=false)
{
	echo "<tr>\n";
	$str = stock_manufactured_items_list_cells($label, $name, $selected_id,
			$all_option, $submit_on_change);
	echo "</tr>\n";
	return $str;
}
//------------------------------------------------------------------------------------

function stock_component_items_list($name, $parent_stock_id, $selected_id,
	$all_option=false, $submit_on_change=false)
{
	return base_stock_items_list("stock_id != '$parent_stock_id'", $name, $selected_id,
		$all_option, $submit_on_change);
}
//------------------------------------------------------------------------------------

function stock_costable_items_list($name, $selected_id,
	$all_option=false, $submit_on_change=false)
{
	return base_stock_items_list("mb_flag!='D'", $name, $selected_id,
		$all_option, $submit_on_change);
}

//------------------------------------------------------------------------------------

function stock_purchasable_items_list($name, $selected_id,	$all_option=false,
		$submit_on_change=false, $opts=array())
{
 global $all_items;
	$sql = "SELECT stock_id, s.description, c.description
			FROM ".TB_PREF."stock_master s,".TB_PREF."stock_category c
			WHERE s.category_id=c.category_id
				AND mb_flag !='M' ";
	return combo_input($name, $selected_id, $sql, 'stock_id', 's.description',
	  array_merge( array(
		'format' => '_format_stock_items',
		'spec_option' => $all_option===true ?  _("All Items") : $all_option,
		'spec_id' => $all_items,
		'search_box' => true,
		'search' => array("stock_id", "c.description","s.description"),
		'search_submit' => get_company_pref('no_item_list')!=0,
		'size'=>10,
		'select_submit'=> $submit_on_change
	  ), $opts) );

}

function stock_purchasable_items_list_cells($label, $name, $selected_id,
			$all_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	$ret = stock_purchasable_items_list($name, $selected_id, $all_option,
		$submit_on_change, array('cells'=>true) );
	return $ret;
}

function stock_purchasable_items_list_row($label, $name, $selected_id,
			$all_option=false, $submit_on_change=false)
{
	echo "<tr>\n";
	$ret = stock_purchasable_items_list_cells($label, $name, $selected_id,
		$all_option, $submit_on_change);
	echo "</tr>\n";
	return $ret;
}

//------------------------------------------------------------------------------------

function stock_item_types_list_row($label, $name, $selected_id, $enabled=true)
{
	default_focus($name);
	echo "<tr>";
	if ($label != NULL)
		echo "<td>$label</td>\n";
	echo "<td>";
	if ($enabled)
		$sel = "<select name='$name' onchange='this.form.submit();'>\n";
	else
		$sel =  "<select disabled name='$name'>\n";
	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	if ($selected_id == "")
		$_POST[$name] = $selected_id = "B";
	$sel .= "<option " . ($selected_id == 'M'?" selected ":"") . " value='M'>" . _("Manufactured"). "</option>\n";
	$sel .= "<option " . ($selected_id == 'B'?" selected ":"") . " value='B'>" . _("Purchased"). "</option>\n";
	$sel .= "<option " . ($selected_id == 'D'?" selected ":"") . " value='D'>" . _("Service"). "</option>\n";
	$sel .= "</select>";
	echo $sel;
	echo "</td></tr>\n";
	return $sel;
}

function stock_units_list_row($label, $name, $value, $enabled=true)
{
	global $stock_units;

	default_focus($name);
	$result = get_all_item_units();
	echo "<tr><td>$label</td>\n";
	if ($enabled)
		echo "<td><select name='$name'>";
	else
		echo "<td><select disabled name='$name'>";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? "" : $_POST[$name]);

	while($unit = db_fetch($result))
	{
		if ($value == "")
		{
			$_POST[$name] = $unit['abbr'];
		}
		$descr = $unit['name'];
		if ($value==$unit['abbr'])
		{
			echo "<option selected value='".$unit['abbr']."'>$descr</option>\n";
		}
		else
		{
			echo "<option value='".$unit['abbr']."'>$descr</option>\n";
		}
	}
	echo "</select></td></tr>\n";
}

//------------------------------------------------------------------------------------

function tax_types_list($name, $selected_id, $none_option=false, $submit_on_change=false)
{
	global $all_items;

	$sql = "SELECT id, CONCAT(name, ' (',rate,'%)') as name FROM ".TB_PREF."tax_types";

	return combo_input($name, $selected_id, $sql, 'id', 'name',
	array(
		'spec_option' => $none_option,
		'spec_id' => 0,
		'select_submit'=> $submit_on_change,
		'async' => false,
	) );
}

function tax_types_list_cells($label, $name, $selected_id, $none_option=false,
	$submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = tax_types_list($name, $selected_id, $none_option, $submit_on_change);
	echo "</td>\n";
	return $str;
}

function tax_types_list_row($label, $name, $selected_id, $none_option=false,
	$submit_on_change=false)
{
	echo "<tr>\n";
	$str = tax_types_list_cells($label, $name, $selected_id, $none_option, $submit_on_change);
	echo "</tr>\n";
	return $str;
}

//------------------------------------------------------------------------------------

function tax_groups_list($name, $selected_id,
	$none_option=false, $submit_on_change=false)
{
	global $all_items;

	$sql = "SELECT id, name FROM ".TB_PREF."tax_groups";

	return combo_input($name, $selected_id, $sql, 'id', 'name',
	array(
		'order' => 'id',
		'spec_option' => $none_option,
		'spec_id' => 0,
		'select_submit'=> $submit_on_change,
		'async' => false,
	) );
}

function tax_groups_list_cells($label, $name, $selected_id, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = tax_groups_list($name, $selected_id, false, $submit_on_change);
	echo "</td>\n";
	return $str;
}

function tax_groups_list_row($label, $name, $selected_id, $submit_on_change=false)
{
	echo "<tr>\n";
	$str = tax_groups_list_cells($label, $name, $selected_id, false, $submit_on_change);
	echo "</tr>\n";
	return $str;
}

//------------------------------------------------------------------------------------

function item_tax_types_list($name, $selected_id)
{
	$sql ="SELECT id, name FROM ".TB_PREF."item_tax_types";
	combo_input($name, $selected_id, $sql, 'id', 'name', array('order' => 'id') );
}

function item_tax_types_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	item_tax_types_list($name, $selected_id);
	echo "</td>\n";
}

function item_tax_types_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	item_tax_types_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function shippers_list($name, $selected_id)
{
	$sql = "SELECT shipper_id, shipper_name FROM ".TB_PREF."shippers";
	combo_input($name, $selected_id, $sql, 'shipper_id', 'shipper_name', array());
}

function shippers_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	shippers_list($name, $selected_id);
	echo "</td>\n";
}

function shippers_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	shippers_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//-------------------------------------------------------------------------------------

function sales_persons_list($name, $selected_id)
{
	$sql = "SELECT salesman_code, salesman_name FROM ".TB_PREF."salesman";
	combo_input($name, $selected_id, $sql, 'salesman_code', 'salesman_name', array());
}

function sales_persons_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>\n";
	sales_persons_list($name, $selected_id);
	echo "</td>\n";
}

function sales_persons_list_row($label, $name, $selected_id, $submit_on_change=false)
{
	echo "<tr>\n";
	sales_persons_list_cells($label, $name, $selected_id, $submit_on_change=false);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function sales_areas_list($name, $selected_id)
{
	$sql = "SELECT area_code, description FROM ".TB_PREF."areas";
	combo_input($name, $selected_id, $sql, 'area_code', 'description', array());
}

function sales_areas_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	sales_areas_list($name, $selected_id);
	echo "</td>\n";
}

function sales_areas_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	sales_areas_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function workorders_list($name, $selected_id)
{
	$sql = "SELECT id, wo_ref FROM ".TB_PREF."workorders WHERE closed=0";
	combo_input($name, $selected_id, $sql, 'id', 'wo_ref', array());
}

function workorders_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	workorders_list($name, $selected_id);
	echo "</td>\n";
}

function workorders_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	workorders_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function payment_terms_list($name, $selected_id)
{
	$sql = "SELECT terms_indicator, terms FROM ".TB_PREF."payment_terms";
	combo_input($name, $selected_id, $sql, 'terms_indicator', 'terms', array());
}

function payment_terms_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	payment_terms_list($name, $selected_id);
	echo "</td>\n";
}

function payment_terms_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	payment_terms_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function credit_status_list($name, $selected_id)
{
	$sql ="SELECT id, reason_description FROM ".TB_PREF."credit_status";
	combo_input($name, $selected_id, $sql, 'id', 'reason_description', array());
}

function credit_status_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	credit_status_list($name, $selected_id);
	echo "</td>\n";
}

function credit_status_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	credit_status_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------

function sales_types_list($name, $selected_id, $submit_on_change=false, $special_option=false)
{
	global $all_items;

	$sql = "SELECT id, sales_type FROM ".TB_PREF."sales_types";

	return combo_input($name, $selected_id, $sql, 'id', 'sales_type',
	array(
		'spec_option' => $special_option===true ? _("All Sales Types") : $special_option,
		'spec_id' => 0,
		'select_submit'=> $submit_on_change,
	//	  'async' => false,
	) );
}

function sales_types_list_cells($label, $name, $selected_id, $submit_on_change=false, $special_option=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = sales_types_list($name, $selected_id, $submit_on_change, $special_option);
	echo "</td>\n";
	return $str;
}

function sales_types_list_row($label, $name, $selected_id, $submit_on_change=false, $special_option=false)
{
	echo "<tr>\n";
	$str = sales_types_list_cells($label, $name, $selected_id, $submit_on_change, $special_option);
	echo "</tr>\n";
	return $str;
}

//-----------------------------------------------------------------------------------------------

function movement_types_list($name, $selected_id)
{
	$sql = "SELECT id, name FROM ".TB_PREF."movement_types";
	combo_input($name, $selected_id, $sql, 'id', 'name', array());
}

function movement_types_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	movement_types_list($name, $selected_id);
	echo "</td>\n";
}

function movement_types_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	movement_types_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------

function bank_trans_types_list($name, $selected_id)
{
	$sql = "SELECT id, name FROM ".TB_PREF."bank_trans_types";
	combo_input($name, $selected_id, $sql, 'id', 'name', array());
}

function bank_trans_types_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	bank_trans_types_list($name, $selected_id);
	echo "</td>\n";
}

function bank_trans_types_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	bank_trans_types_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------

function workcenter_list($name, $selected_id, $all_option=false)
{
	global $all_items;

	$sql = "SELECT id, name FROM ".TB_PREF."workcentres";

	return combo_input($name, $selected_id, $sql, 'id', 'name',
	array(
		'spec_option' =>$all_option===true ? _("All Suppliers") : $all_option,
		'spec_id' => $all_items,
	) );
}

function workcenter_list_cells($label, $name, $selected_id, $all_option=false)
{
	default_focus($name);
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	workcenter_list($name, $selected_id, $all_option);
	echo "</td>\n";
}

function workcenter_list_row($label, $name, $selected_id, $all_option=false)
{
	echo "<tr>\n";
	workcenter_list_cells($label, $name, $selected_id, $all_option);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------

function bank_accounts_list($name, $selected_id, $submit_on_change=false)
{
	$sql = "SELECT ".TB_PREF."bank_accounts.account_code, bank_account_name, bank_curr_code
		FROM ".TB_PREF."bank_accounts, ".TB_PREF."chart_master
		WHERE ".TB_PREF."bank_accounts.account_code=".TB_PREF."chart_master.account_code";

	return combo_input($name, $selected_id, $sql, 'account_code', 'bank_account_name',
	array(
		'format' => '_format_add_curr',
		'select_submit'=> $submit_on_change,
		'async' => false
	) );
}

function bank_accounts_list_cells($label, $name, $selected_id, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = bank_accounts_list($name, $selected_id, $submit_on_change);
	echo "</td>\n";
	return $str;
}

function bank_accounts_list_row($label, $name, $selected_id, $submit_on_change=false)
{
	echo "<tr>\n";
	$str = bank_accounts_list_cells($label, $name, $selected_id, $submit_on_change);
	echo "</tr>\n";
	return $str;
}

//-----------------------------------------------------------------------------------------------

function class_list($name, $selected_id, $submit_on_change=false)
{
	$sql = "SELECT cid, class_name FROM ".TB_PREF."chart_class";

	return combo_input($name, $selected_id, $sql, 'cid', 'class_name',
	array(
		'select_submit'=> $submit_on_change,
		'async' => false
	) );

}

function class_list_cells($label, $name, $selected_id, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = class_list($name, $selected_id, $submit_on_change);
	echo "</td>\n";
	return $str;
}

function class_list_row($label, $name, $selected_id, $submit_on_change=false)
{
	echo "<tr>\n";
	$str = class_list_cells($label, $name, $selected_id, $submit_on_change);
	echo "</tr>\n";
	return $str;
}

//-----------------------------------------------------------------------------------------------

function stock_categories_list($name, $selected_id)
{
	$sql = "SELECT category_id, description FROM ".TB_PREF."stock_category";
	combo_input($name, $selected_id, $sql, 'category_id', 'description',
	array('order'=>'category_id'));
}

function stock_categories_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	stock_categories_list($name, $selected_id);
	echo "</td>\n";
}

function stock_categories_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	stock_categories_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------

function gl_account_types_list($name, $selected_id, $all_option, $all_option_numeric)
{
	global $all_items;

	$sql = "SELECT id, name FROM ".TB_PREF."chart_types";

	combo_input($name, $selected_id, $sql, 'id', 'name',
	array(
		'order' => 'id',
		'spec_option' =>$all_option,
		'spec_id' => $all_option_numeric ? 0 : $all_items
	) );
}

function gl_account_types_list_cells($label, $name, $selected_id, $all_option=false,
	$all_option_numeric=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	gl_account_types_list($name, $selected_id, $all_option, $all_option_numeric);
	echo "</td>\n";
}

function gl_account_types_list_row($label, $name, $selected_id, $all_option=false,
	$all_option_numeric=false)
{
	echo "<tr>\n";
	gl_account_types_list_cells($label, $name, $selected_id, $all_option,
		$all_option_numeric);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------
function gl_all_accounts_list($name, $selected_id, $skip_bank_accounts=false,
	$show_group=false, $cells=false, $all_option=false)
{
	if ($skip_bank_accounts)
		$sql = "SELECT chart.account_code, chart.account_name, type.name
			FROM (".TB_PREF."chart_master chart,".TB_PREF."chart_types type) "
			."LEFT JOIN ".TB_PREF."bank_accounts acc "
			."ON chart.account_code=acc.account_code
				WHERE acc.account_code IS NULL
			AND chart.account_type=type.id";
	else
		$sql = "SELECT chart.account_code, chart.account_name, type.name
			FROM ".TB_PREF."chart_master chart,".TB_PREF."chart_types type
			WHERE chart.account_type=type.id";

	combo_input($name, $selected_id, $sql, 'chart.account_code', 'chart.account_name',
	array(
		'format' => '_format_account' .  ($show_group ? '2' : ''),
		'spec_option' => $all_option===true ?  _("Use Item Sales Accounts") : $all_option,
		'spec_id' => '',
		'order' => 'account_code',
		'search_box' => $cells,
			'search_submit' => false,
			'size' => 12,
			'max' => 10,
			'cells' => true
	) );

}

function _format_account($row)
{
		return $row[0] .  "&nbsp;&nbsp;&nbsp;&nbsp;" . $row[1];
}

function _format_account2($row)
{
		return $row[0] .  "&nbsp;&nbsp;&nbsp;&nbsp;" . $row[2] . "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" . $row[1];
}

function gl_all_accounts_list_cells($label, $name, $selected_id, $skip_bank_accounts=false,
	$show_group=false, $cells=false, $all_option=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	gl_all_accounts_list($name, $selected_id, $skip_bank_accounts, $show_group, $cells, $all_option);
	echo "</td>\n";
}

function gl_all_accounts_list_row($label, $name, $selected_id, $skip_bank_accounts=false,
	$show_group=false, $cells=false, $all_option=false)
{
	echo "<tr>\n";
	gl_all_accounts_list_cells($label, $name, $selected_id, $skip_bank_accounts,
		$show_group, $cells, $all_option);
	echo "</tr>\n";
}

function yesno_list($name, $selected_id, $name_yes="", $name_no="", $submit_on_change=false)
{
	default_focus($name);
	if ($submit_on_change == true)
		echo "<select name='$name' onchange='this.form.submit();'>"; // FIX ajax
	else
		echo "<select name='$name'>";

	if (strlen($name_yes) == 0)
	{
		unset($name_yes);
	}
	if (strlen($name_no) == 0)
	{
		unset($name_no);
	}

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? 0 : $_POST[$name]);
	if ($selected_id == 0)
		echo "<option value=1>";
	else
		echo "<option selected value=1>";
	if (!isset($name_yes))
		echo _("Yes") . "</option>\n";
	else
		echo $name_yes . "</option>\n";
	if ($selected_id == 0)
		echo "<option selected value=0>";
	else
		echo "<option value=0>";
	if (!isset($name_no))
		echo _("No") . "</option>\n";
	else
		echo $name_no . "</option>\n";
	echo "</select>";
}

function yesno_list_cells($label, $name, $selected_id, $name_yes="", $name_no="", $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = yesno_list($name, $selected_id, $name_yes, $name_no, $submit_on_change);
	echo "</td>\n";
	return $str;
}

function yesno_list_row($label, $name, $selected_id, $name_yes="", $name_no="", $submit_on_change=false)
{
	echo "<tr>\n";
	$str = yesno_list_cells($label, $name, $selected_id, $name_yes, $name_no, $submit_on_change);
	echo "</tr>\n";
	return $str;
}

//------------------------------------------------------------------------------------------------

function languages_list($name, $selected_id)
{
	global $installed_languages;

	default_focus($name);
	echo "<select name='$name'>";

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	foreach ($installed_languages as $lang)
	{
		echo "<option ";
		if ($selected_id == $lang['code'])
			echo "selected ";
		echo "value='" . $lang['code'] . "'>" . $lang['name'] . "</option>\n";
	}
}

function languages_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	languages_list($name, $selected_id);
	echo "</td>\n";
}

function languages_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	languages_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------------------

function bank_account_types_list($name, $selected_id)
{
	$bank_account_types = bank_account_types::get_all();

	default_focus($name);
	echo "<select name='$name'>";

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	foreach ($bank_account_types as $type)
	{
		echo "<option ";
		if ($selected_id == "" || $selected_id == $type['id'])
			echo "selected ";
		echo "value='" . $type['id'] . "'>" . $type['name'] . "</option>\n";
	}
}

function bank_account_types_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	bank_account_types_list($name, $selected_id);
	echo "</td>\n";
}

function bank_account_types_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	bank_account_types_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------------------

function payment_person_types_list($name, $selected_id, $related=null)
{
	$types = payment_person_types::get_all();

	default_focus($name);
	echo "<select name='$name'";
	if ($related)	// FIX ajax
		echo " onchange='this.form.$related.value=\"\"; this.form.submit();' ";
	echo ">";

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	foreach ($types as $type)
	{
		if (payment_person_types::has_items($type['id']))
		{
			if ($selected_id == "")
				$_POST[$name] = $selected_id = $type['id'];
			echo "<option ";
			if ($selected_id == $type['id'])
				echo "selected ";
			echo "value='" . $type['id'] . "'>" . $type['name'] . "</option>\n";
		}
	}
}

function payment_person_types_list_cells($label, $name, $selected_id, $related=null)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = payment_person_types_list($name, $selected_id, $related);
	echo "</td>\n";
	return $str;
}

function payment_person_types_list_row($label, $name, $selected_id, $related=null)
{
	echo "<tr>\n";
	$str = payment_person_types_list_cells($label, $name, $selected_id, $related);
	echo "</tr>\n";
	return $str;
}

//------------------------------------------------------------------------------------------------

function wo_types_list($name, $selected_id)
{
	$types = wo_types::get_all();

	default_focus($name);	// FIX ajax
	echo "<select name='$name' onchange='this.form.submit();'>";

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	foreach ($types as $type)
	{
		echo "<option ";
		if ($selected_id == $type['id'])
			echo "selected ";
		echo "value='" . $type['id'] . "'>" . $type['name'] . "</option>\n";
	}
	echo "</select>";
}

function wo_types_list_row($label, $name, $selected_id)
{
	echo "<tr><td>$label</td><td>\n";
	$str = wo_types_list($name, $selected_id);
	echo "</td></tr>\n";
	return $str;
}

//------------------------------------------------------------------------------------------------

function dateformats_list_row($label, $name, $value)
{
	global $dateformats;

	default_focus($name);
	echo "<tr><td>$label</td>\n";
	echo "<td><select name='$name'>";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? 0 : $_POST[$name]);
	$counter = 0;
	foreach ($dateformats as $df)
	{

		if ($value==$counter)
		{
			echo "<option selected value='$counter'>$df</option>\n";
		}
		else
		{
			echo "<option value='$counter'>$df</option>\n";
		}
		$counter++;
	}
	echo "</select></td></tr>\n";
}

function dateseps_list_row($label, $name, $value)
{
	global $dateseps;

	default_focus($name);
	echo "<tr><td>$label</td>\n";
	echo "<td><select name='$name'>";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? 0 : $_POST[$name]);
	$counter = 0;
	foreach ($dateseps as $ds)
	{

		if ($value==$counter)
		{
			echo "<option selected value='$counter'>$ds</option>\n";
		}
		else
		{
			echo "<option value='$counter'>$ds</option>\n";
		}
		$counter++;
	}
	echo "</select></td></tr>\n";
}

function thoseps_list_row($label, $name, $value)
{
	global $thoseps;

	default_focus($name);
	echo "<tr><td>$label</td>\n";
	echo "<td><select name='$name'>";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? 0 : $_POST[$name]);
	$counter = 0;
	foreach ($thoseps as $ts)
	{

		if ($value==$counter)
		{
			echo "<option selected value='$counter'>$ts</option>\n";
		}
		else
		{
			echo "<option value='$counter'>$ts</option>\n";
		}
		$counter++;
	}
	echo "</select></td></tr>\n";
}

function decseps_list_row($label, $name, $value)
{
	global $decseps;

	default_focus($name);
	echo "<tr><td>$label</td>\n";
	echo "<td><select name='$name'>";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? 0 : $_POST[$name]);
	$counter = 0;
	foreach ($decseps as $ds)
	{

		if ($value==$counter)
		{
			echo "<option selected value='$counter'>$ds</option>\n";
		}
		else
		{
			echo "<option value='$counter'>$ds</option>\n";
		}
		$counter++;
	}
	echo "</select></td></tr>\n";
}

function themes_list_row($label, $name, $value)
{
	global $path_to_root;

	default_focus($name);
	$path = $path_to_root.'/themes/';
	$themes = array();
	$themedir = opendir($path);
	while(false !== ($fname = readdir($themedir)))
	{
		if($fname!='.' && $fname!='..' && $fname!='CVS' && is_dir($path.$fname))
		{
			$themes[] =  $fname;
		}
	}
	sort($themes);
	echo "<tr><td>$label</td>\n";
	echo "<td><select name='$name'>";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	foreach ($themes as $th)
	{

		if ($value==$th)
		{
			echo "<option selected value='$th'>$th</option>\n";
		}
		else
		{
			echo "<option value='$th'>$th</option>\n";
		}
	}
	echo "</select></td></tr>\n";
}

function pagesizes_list_row($label, $name, $value)
{
	global $pagesizes;

	default_focus($name);
	echo "<tr><td>$label</td>\n";
	echo "<td><select name='$name'>";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	foreach ($pagesizes as $pz)
	{

		if ($value==$pz)
		{
			echo "<option selected value='$pz'>$pz</option>\n";
		}
		else
		{
			echo "<option value='$pz'>$pz</option>\n";
		}
	}
	echo "</select></td></tr>\n";
}

function security_headings_list_row($label, $name, $value)
{
	global $security_headings;

	default_focus($name);
	echo "<tr><td>$label</td>\n";
	echo "<td><select name='$name'>";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? 0 : (int)$_POST[$name]);
	$counter=0;
	foreach ($security_headings as $sh)
	{

		if ($value==$counter)
		{
			echo "<option selected value='$counter'>$sh</option>\n";
		}
		else
		{
			echo "<option value='$counter'>$sh</option>\n";
		}
		$counter++;
	}
	echo "</select></td></tr>\n";
}

function systypes_list_cells($label, $name, $value, $submit_on_change=false)
{
	global $systypes_array;

	default_focus($name);
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td><select name='$name'";
	if ($submit_on_change)
		echo " onchange='this.form.submit();'>";	// FIX ajax
	else
		echo ">";
	if ($value == null)
		$value = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	foreach ($systypes_array as $key=>$type)
	{

		if ($value==$key)
		{
			echo "<option selected value='$key'>".$type['name']."</option>\n";
		}
		else
		{
			echo "<option value='$key'>".$type['name']."</option>\n";
		}
	}
	echo "</select></td>\n";
}

function systypes_list_row($label, $name, $value, $submit_on_change=false)
{
	echo "<tr>\n";
	$str = systypes_list_cells($label, $name, $value, $submit_on_change);
	echo "</tr>\n";
	return $str;
}

function cust_allocations_list_cells($label, $name, $selected)
{
	global $all_items;
	default_focus($name);
	if ($label != null)
		label_cell($label);
	if ($selected == null)
		$selected = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	echo "<td><select name='$name'>";
	echo "<option " . ($selected == $all_items ? " selected " : "") . " value='$all_items'>" . _("All Types"). "</option>\n";
	echo "<option " . ($selected == '1'?" selected ":"") . " value='1'>" . _("Sales Invoices"). "</option>\n";
	echo "<option " . ($selected == '2'?" selected ":"") . " value='2'>" . _("Overdue Invoices"). "</option>\n";
	echo "<option " . ($selected == '3'?" selected ":"") . " value='3'>" . _("Payments"). "</option>\n";
	echo "<option " . ($selected == '4'?" selected ":"") . " value='4'>" . _("Credit Notes"). "</option>\n";
	echo "<option " . ($selected == '5'?" selected ":"") . " value='5'>" . _("Delivery Notes"). "</option>\n";
	echo "</select></td>\n";
}

function supp_allocations_list_cells($name, $selected)
{
	global $all_items;

	default_focus($name);
	if ($selected == null)
		$selected = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	echo "<td><select name='$name'>";
	echo "<option " . ($selected == $all_items ? " selected " : "") . " value='$all_items'>" . _("All Types"). "</option>\n";
	echo "<option " . ($selected == '1'?" selected ":"") . " value='1'>" . _("Invoices"). "</option>\n";
	echo "<option " . ($selected == '2'?" selected ":"") . " value='2'>" . _("Overdue Invoices"). "</option>\n";
	echo "<option " . ($selected == '3'?" selected ":"") . " value='3'>" . _("Payments"). "</option>\n";
	echo "<option " . ($selected == '4'?" selected ":"") . " value='4'>" . _("Credit Notes"). "</option>\n";
	echo "<option " . ($selected == '5'?" selected ":"") . " value='5'>" . _("Overdue Credit Notes"). "</option>\n";
	echo "</select></td>\n";
}

function policy_list_cells($label, $name, $selected)
{
	default_focus($name);
	if ($selected == null)
	{
		$selected = (!isset($_POST[$name]) ? "" : $_POST[$name]);
		if ($selected == "")
			$_POST[$name] = $selected;
	}
	if ($label != null)
		label_cell($label);
	echo "<td><select name='$name'>";
	echo "<option " . ($selected == ''?" selected ":"") . " value=''>" . _("Automatically put balance on back order"). "</option>\n";
	echo "<option " . ($selected == 'CAN'?" selected ":"") . " value='CAN'>" . _("Cancel any quantites not delivered"). "</option>\n";
	echo "</select></td>\n";
}

function policy_list_row($label, $name, $selected)
{
	echo "<tr>\n";
	policy_list_cells($label, $name, $selected);
	echo "</tr>\n";
}

function credit_type_list_cells($label, $name, $selected, $submit_on_change=false)
{
	default_focus($name);
	if ($selected == null)
	{
		$selected = (!isset($_POST[$name]) ? "Return" : $_POST[$name]);
		if ($selected == "Return")
			$_POST[$name] = $selected;
	}
	if ($label != null)
		label_cell($label);
	echo "<td><select name='$name'";
	if ($submit_on_change)
		echo " onchange='this.form.submit();'>";	// FIX ajax
	else
		echo ">";
	echo "<option " . ($selected == 'Return'?" selected ":"") . " value='Return'>" . _("Items Returned to Inventory Location"). "</option>\n";
	echo "<option " . ($selected == 'WriteOff'?" selected ":"") . " value='WriteOff'>" . _("Items Written Off"). "</option>\n";
	echo "</select></td>\n";
}

function credit_type_list_row($label, $name, $selected, $submit_on_change=false)
{
	echo "<tr>\n";
	$str = credit_type_list_cells($label, $name, $selected, $submit_on_change);
	echo "</tr>\n";
	return $str;
}

function number_list($name, $selected, $from, $to, $firstlabel="")
{
	default_focus($name);
	if ($selected == null)
	{
		$selected = (!isset($_POST[$name]) ? $from : $_POST[$name]);
		if ($selected == $from)
			$_POST[$name] = $selected;
	}
	echo "<select name='$name'>";
	for ($i = $from; $i <= $to; $i++)
	{
		if ($i == 0 && $firstlabel != "")
			$label = $firstlabel;
		else
			$label = $i;
		if ($selected == $i)
		{
			echo "<option selected value='$i'>$label</option>\n";
		}
		else
		{
			echo "<option value='$i'>$label</option>\n";
		}
	}
	echo "</select>\n";
}

function number_list_cells($label, $name, $selected, $from, $to)
{
	if ($label != null)
		label_cell($label);
	echo "<td>\n";
	number_list($name, $selected, $from, $to);
	echo "</td>\n";
}

function number_list_row($label, $name, $selected, $from, $to)
{
	echo "<tr>\n";
	number_list_cells($label, $name, $selected, $from, $to);
	echo "</tr>\n";
}
?>