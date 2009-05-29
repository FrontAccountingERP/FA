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
include_once($path_to_root . "/includes/banking.inc");
include_once($path_to_root . "/includes/reserved.inc");
include_once($path_to_root . "/includes/types.inc");
include_once($path_to_root . "/includes/current_user.inc");

$_search_button = "<input %s type='submit' class='combo_submit' style='border:0;background:url($path_to_root/themes/"
	."%s/images/locate.png) no-repeat;%s' aspect='fallback' name='%s' value=' ' title='"._("Set filter")."'> ";

$_select_button = "<input %s type='submit' class='combo_select' style='border:0;background:url($path_to_root/themes/"
	."%s/images/button_ok.png) no-repeat;%s' aspect='fallback' name='%s' value=' ' title='"._("Select")."'> ";

$all_items = reserved_words::get_all();

//----------------------------------------------------------------------------
//	Universal sql combo generator
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
	'default' => '', // default value when $_POST is not set
	'select_submit' => false, //submit on select: true/false
	'edit_submit' => false, // call editor on F4
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
	'disabled' => false,
	'box_hint' => null, // box/selectors hints; null = std see below
	'category' => false	// category column name or false
);
// ------ merge options with defaults ----------
	if($options != null)
		$opts = array_merge($opts, $options);
	if (!is_array($opts['where']))  $opts['where'] = array($opts['where']);

	$search_box = $opts['search_box']===true ? '_'.$name.'_edit' : $opts['search_box'];
	$search_submit = $opts['search_submit']===true ? '_'.$name.'_button' : $opts['search_submit'];
	$select_submit =  $opts['select_submit']===true ? '_'.$name.'_update' : $opts['select_submit'];
	$spec_id = $opts['spec_id'];
	$spec_option = $opts['spec_option'];
	$by_id = ($opts['type'] == 0);
	$class = $by_id ? 'combo':'combo2';
	$disabled = $opts['disabled'] ? "disabled" : '';

	if(!count($opts['search'])) {
		$opts['search'] = array($by_id ? $valfield : $namefield);
	}
	if ($opts['sel_hint'] === null) 
		$opts['sel_hint'] = $by_id || $search_box==false ?
			'' : _('Press Space tab for search pattern entry');

	if ($opts['box_hint'] === null)
		$opts['box_hint'] = $search_box && $search_submit != false ?
			($by_id ? _('Enter code fragment to search or * for all')
			: _('Enter description fragment to search or * for all')) :'';

	if ($selected_id == null) {
		$selected_id = get_post($name, $opts['default']);
	}
	$txt = get_post($search_box);
	$rel = '';
	$limit = '';

	if (isset($_POST[$select_submit])) {
		if ($by_id) $txt = $_POST[$name];

		if (!$opts['async'])
			$Ajax->activate('_page_body');
		else
			$Ajax->activate($name);
	}
	if ($search_box) {
		// search related sql modifications

	$rel = "rel='$search_box'"; // set relation to list
   if ($opts['search_submit']) {
	if (isset($_POST[$search_submit])) {
		$selected_id = ''; // ignore selected_id while search
		if (!$opts['async'])
			$Ajax->activate('_page_body');
		else
			$Ajax->activate($name);
	}
	if ($txt == '') {
		if ($spec_option === false && $selected_id==null)
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
		$where = strpos($sql, 'WHERE')==false ? ' WHERE ':' AND ';
		$where .= '('. implode($opts['where'], ' AND ') . ')';
		$group_pos = strpos($sql, 'GROUP BY');
		if ($group_pos) {
			$group = substr($sql, $group_pos);
			$sql = substr($sql, 0, $group_pos) . $where.' '.$group;
		} else {
			$sql .= $where;
		}
	}
	if ($opts['order'] != false) {
		if (!is_array($opts['order']))
			$opts['order'] = array($opts['order']);
		$sql .= ' ORDER BY '.implode(',',$opts['order']);
	}

	$sql .= $limit;
	// ------ make selector ----------
	$selector = $first_opt = '';
	$first_id = false;
	$found = false;
	$lastcat = null;
//if($name=='stock_id') display_error($sql);
	if($result = db_query($sql)) {
		while ($contact_row = db_fetch($result)) {
			$value = $contact_row[0];
			$descr = $opts['format']==null ?  $contact_row[1] :
			call_user_func($opts['format'], $contact_row);
			$sel = '';
 		 	if (get_post($search_submit) && ($txt === $value)) {
 		 		$selected_id = $value;
 		 	}
 		 	if	((string)($selected_id) === $value) {
				$sel = 'selected';
				$found = $value;
			}
			if ($first_id === false) {
				$first_id = $value;
				$first_opt = $descr;
			}
			$cat = $contact_row[$opts['category']];
			if ($opts['category'] !== false && $cat != $lastcat){
				$selector .= "<optgroup label='".$cat."'>\n";
				$lastcat = $cat;
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
		$selected_id = $first_id;
	}
	$_POST[$name] = $selected_id;

	if ($by_id && $search_box != false) {
 		$txt = $found;
		$Ajax->addUpdate($name, $search_box, $txt ? $txt : '');
	}
	$aspect = $opts['edit_submit'] ? " aspect='editable'" : '';
	$selector = "<select $disabled name='$name' class='$class' title='"
		. $opts['sel_hint']."'$aspect $rel>".$selector."</select>\n";

	$Ajax->addUpdate($name, "_{$name}_sel", $selector);

	// because of bug which M$ cannot fix since IE 5.0
	// we must embed whole selector in span tags to enable proper ajax update
	$selector = "<span id='_{$name}_sel'>".$selector."</span>\n";

	if ($select_submit != false) { // if submit on change is used - add select button
		global $_select_button;
	// button class selects form reload/ajax selector update
		$selector .= sprintf($_select_button, $disabled, user_theme(),
			(fallback_mode() ? '' : 'display:none;'),
			$select_submit)."\n";
	}
// ------ make combo ----------

	$edit_entry = '';
	if ($search_box != false) {
		$edit_entry = "<input $disabled type='text' name='$search_box' id='$search_box' size='".
			$opts['size']."' maxlength='".$opts['max'].
			"' value='$txt' class='$class' rel='$name' autocomplete='off' title='"
			.$opts['box_hint']."'"
			.(!fallback_mode() && !$by_id ? " style=display:none;":'')
			.">\n";
		if ($search_submit != false) {
			global $_search_button;
			$edit_entry .= sprintf($_search_button, $disabled, user_theme(),
				(fallback_mode() ? '' : 'display:none;'),
				$search_submit)."\n";
		}
	}
	default_focus(($search_box && $by_id) ? $search_box : $name);
	$str = $selector;
	if ($search_box && $opts['cells'])
		echo ($edit_entry!='' ? "<td>$edit_entry</td>" : '')."<td>$selector</td>";
	else
		echo $edit_entry.$selector;

	return $str;
}

/*
	Helper function.
	Returns true if selector $name is subject to update.
*/
function list_updated($name)
{
	return isset($_POST['_'.$name.'_update']) || isset($_POST['_'.$name.'_button']);
}
//----------------------------------------------------------------------------------------------
//	Universal array combo generator
//	$items is array of options 'value' => 'description'
//	Options is reduced set of combo_selector options and is merged with defaults.

function array_selector($name, $selected_id, $items, $options=null)
{
	global $Ajax;

$opts = array(		// default options
	'spec_option'=>false, 	// option text or false
	'spec_id' => 0,		// option id
	'select_submit' => false, //submit on select: true/false
	'edit_submit' => false, // call editor on F4
	'async' => true,	// select update via ajax (true) vs _page_body reload
	'default' => '', // default value when $_POST is not set
		// search box parameters
	'sel_hint' => null,
	'disabled' => false
);
// ------ merge options with defaults ----------
	if($options != null)
		$opts = array_merge($opts, $options);
	$select_submit =  $opts['select_submit']===true ? '_'.$name.'_update' : $opts['select_submit'];
	$spec_id = $opts['spec_id'];
	$spec_option = $opts['spec_option'];
	$disabled = $opts['disabled'] ? "disabled" : '';

	if ($selected_id == null) {
		$selected_id = get_post($name, $opts['default']);
	}

	if (isset($_POST[$select_submit])) {
		if (!$opts['async'])
			$Ajax->activate('_page_body');
		else
			$Ajax->activate($name);
	}

	// ------ make selector ----------
	$selector = $first_opt = '';
	$first_id = false;
	$found = false;
//if($name=='SelectStockFromList') display_error($sql);
		foreach($items as $value=>$descr) {
			$sel = '';
			if ((string)$selected_id === (string)$value) {
				$sel = 'selected';
				$found = $value;
			}
			if ($first_id === false) {
				$first_id = $value;
				$first_opt = $descr;
			}
			$selector .= "<option $sel value='$value'>$descr</option>\n";
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
		$selected_id = $first_id;
	}
	$_POST[$name] = $selected_id;

	$aspect = $opts['edit_submit'] ? " aspect='editable'" : '';
	$selector = "<select $disabled name='$name' class='combo' title='"
		. $opts['sel_hint']."'$aspect >".$selector."</select>\n";

	$Ajax->addUpdate($name, "_{$name}_sel", $selector);

	$selector = "<span id='_{$name}_sel'>".$selector."</span>\n";

	if ($select_submit != false) { // if submit on change is used - add select button
		global $_select_button;
		$selector .= sprintf($_select_button, $disabled, user_theme(),
			(fallback_mode() ? '' : 'display:none;'),
			$select_submit)."\n";
	}
	default_focus($name);
	echo $selector;

	return $selector;
}
//----------------------------------------------------------------------------------------------

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

function supplier_list($name, $selected_id=null, $spec_option=false, $submit_on_change=false)
{
	global $all_items;

	$sql = "SELECT supplier_id, supp_name, curr_code FROM ".TB_PREF."suppliers ";

	$mode = get_company_pref('no_supplier_list');

	return combo_input($name, $selected_id, $sql, 'supplier_id', 'supp_name',
	array(
		'format' => '_format_add_curr',
		'search_box' => $mode!=0,
		'edit_submit' => true, 
		'type' => 1,
		'spec_option' => $spec_option === true ? _("All Suppliers") : $spec_option,
		'spec_id' => $all_items,
		'select_submit'=> $submit_on_change,
		'async' => false,
		'sel_hint' => $mode ? _('Press Space tab to filter by name fragment') :
		_('Select supplier')
		));
}

function supplier_list_cells($label, $name, $selected_id=null, $all_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td><td>\n";
	$str = supplier_list($name, $selected_id, $all_option, $submit_on_change);
		echo "</td>\n";
	return $str;
}

function supplier_list_row($label, $name, $selected_id=null, $all_option = false, $submit_on_change=false)
{
echo "<tr><td>$label</td><td>";
	$str = supplier_list($name, $selected_id, $all_option, $submit_on_change );
echo "</td></tr>\n";
return $str;
}
//----------------------------------------------------------------------------------------------

function customer_list($name, $selected_id=null, $spec_option=false, $submit_on_change=false)
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
		'edit_submit' => true, // call editor on F4
		'async' => false,
		'sel_hint' => $mode ? _('Press Space tab to filter by name fragment; F4 - entry new customer') :
		_('Select customer')
	) );
}

function customer_list_cells($label, $name, $selected_id=null, $all_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td nowrap>";
	$str = customer_list($name, $selected_id, $all_option, $submit_on_change);
	echo "</td>\n";
	return $str;
}

function customer_list_row($label, $name, $selected_id=null, $all_option = false, $submit_on_change=false)
{
	echo "<tr><td>$label</td><td nowrap>";
	$str = customer_list($name, $selected_id, $all_option, $submit_on_change);
	echo "</td>\n</tr>\n";
	return $str;
}

//------------------------------------------------------------------------------------------------

function customer_branches_list($customer_id, $name, $selected_id=null,
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
	) );
}
//------------------------------------------------------------------------------------------------

function customer_branches_list_cells($label,$customer_id, $name, $selected_id=null, $all_option = true, $enabled=true, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$ret = customer_branches_list($customer_id, $name, $selected_id, $all_option, $enabled, $submit_on_change);
	echo "</td>\n";
	return $ret;
}

function customer_branches_list_row($label,$customer_id, $name, $selected_id=null, $all_option = true, $enabled=true, $submit_on_change=false)
{
	echo "<tr>";
	$ret = customer_branches_list_cells($label, $customer_id, $name, $selected_id, $all_option, $enabled, $submit_on_change);
	echo "</tr>";
	return $ret;
}

//------------------------------------------------------------------------------------------------

function locations_list($name, $selected_id=null, $all_option=false, $submit_on_change=false)
{
	global $all_items;

	$sql = "SELECT loc_code, location_name FROM ".TB_PREF."locations";

return combo_input($name, $selected_id, $sql, 'loc_code', 'location_name',
	array(
		'spec_option' => $all_option === true ? _("All Locations") : $all_option,
		'spec_id' => $all_items,
		'select_submit'=> $submit_on_change
	) );
}

function locations_list_cells($label, $name, $selected_id=null, $all_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = locations_list($name, $selected_id, $all_option, $submit_on_change);
	echo "</td>\n";
	return $str;
}

function locations_list_row($label, $name, $selected_id=null, $all_option=false, $submit_on_change=false)
{
	echo "<tr>";
	$str = locations_list_cells($label, $name, $selected_id, $all_option, $submit_on_change);
	echo "</tr>\n";
	return $str;
}

//-----------------------------------------------------------------------------------------------

function currencies_list($name, $selected_id=null, $submit_on_change=false)
{
	$sql = "SELECT curr_abrev, currency FROM ".TB_PREF."currencies";

// default to the company currency

return combo_input($name, $selected_id, $sql, 'curr_abrev', 'currency',
	array(
		'select_submit'=> $submit_on_change,
		'default' => get_company_currency(),
		'async' => false		
	) );
}

function currencies_list_cells($label, $name, $selected_id=null, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = currencies_list($name, $selected_id, $submit_on_change);
	echo "</td>\n";
	return $str;
}

function currencies_list_row($label, $name, $selected_id=null, $submit_on_change=false)
{
	echo "<tr>\n";
	$str = currencies_list_cells($label, $name, $selected_id, $submit_on_change);
	echo "</tr>\n";
	return $str;
}

//---------------------------------------------------------------------------------------------------

function fiscalyears_list($name, $selected_id=null, $submit_on_change=false)
{

	$sql = "SELECT * FROM ".TB_PREF."fiscal_year";

// default to the company current fiscal year

return combo_input($name, $selected_id, $sql, 'id', '',
	array(
		'order' => 'begin',
		'default' => get_company_pref('f_year'),
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

function fiscalyears_list_cells($label, $name, $selected_id=null)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = fiscalyears_list($name, $selected_id);
	echo "</td>\n";
	return $str;
}

function fiscalyears_list_row($label, $name, $selected_id=null)
{
	echo "<tr>\n";
	$str = fiscalyears_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
	return $str;
}
//------------------------------------------------------------------------------------

function dimensions_list($name, $selected_id=null, $no_option=false, $showname=' ',
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

	if (!$showclosed)
		$options['where'][] = "closed=0";
	if($showtype)
		$options['where'][] = "type_=$showtype";

	return combo_input($name, $selected_id, $sql, 'id', 'ref', $options);
}

function dimensions_list_cells($label, $name, $selected_id=null, $no_option=false, $showname=null,
	$showclosed=false, $showtype=0, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = dimensions_list($name, $selected_id, $no_option, $showname, $submit_on_change, $showclosed, $showtype);
	echo "</td>\n";
	return $str;
}

function dimensions_list_row($label, $name, $selected_id=null, $no_option=false, $showname=null,
	$showclosed=false, $showtype=0, $submit_on_change=false)
{
	echo "<tr>\n";
	$str = dimensions_list_cells($label, $name, $selected_id, $no_option, $showname,
		$showclosed, $showtype, $submit_on_change);
	echo "</tr>\n";
	return $str;
}

//---------------------------------------------------------------------------------------------------

function stock_items_list($name, $selected_id=null, $all_option=false, $submit_on_change=false, $opts=array())
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
		'select_submit'=> $submit_on_change,
		'category' => 2,
		'order' => array('c.description','stock_id')
	  ), $opts) );
}

function _format_stock_items($row)
{
	return (user_show_codes() ?  ($row[0] . "&nbsp;-&nbsp;") : "") . $row[1];
}

function stock_items_list_cells($label, $name, $selected_id=null, $all_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	$str = stock_items_list($name, $selected_id, $all_option, $submit_on_change,
		array('cells'=>true));
	return $str;
}
/*
function stock_items_list_row($label, $name, $selected_id=null, $all_option=false, $submit_on_change=false)
{
	echo "<tr>\n";
	$str = stock_items_list_cells($label, $name, $selected_id, $all_option, $submit_on_change);
	echo "</tr>\n";
	return $str;
}
*/
//---------------------------------------------------------------------------------------------------
//
// Select item via foreign code.
//
function sales_items_list($name, $selected_id=null, $all_option=false, 
	$submit_on_change=false, $type='', $opts=array())
{
	global $all_items;
	// all sales codes
	$sql = "SELECT i.item_code, i.description, c.description, count(*)>1 as kit
			FROM
			".TB_PREF."item_codes i
			LEFT JOIN
			".TB_PREF."stock_category c
			ON i.category_id=c.category_id";

	if ($type == 'local')	{ // exclude foreign codes
		$sql .=	" WHERE !i.is_foreign"; 
	} elseif ($type == 'kits') { // sales kits
		$sql .=	" WHERE !i.is_foreign AND i.item_code!=i.stock_id";
	}
				
	$sql .= " GROUP BY i.item_code";

	return combo_input($name, $selected_id, $sql, 'i.item_code', 'c.description',
	array_merge(
	  array(
		'format' => '_format_stock_items',
		'spec_option' => $all_option===true ?  _("All Items") : $all_option,
		'spec_id' => $all_items,
		'search_box' => true,
		'search' => array("i.item_code", "i.description"),
		'search_submit' => get_company_pref('no_item_list')!=0,
		'size'=>15,
		'select_submit'=> $submit_on_change,
		'category' => 2,
		'order' => array('c.description','i.item_code')
	  ), $opts) );
}

function sales_items_list_cells($label, $name, $selected_id=null, $all_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	$str = sales_items_list($name, $selected_id, $all_option, $submit_on_change,
		'', array('cells'=>true));
	return $str;
}

function sales_kits_list($name, $selected_id=null, $all_option=false, $submit_on_change=false)
{
	$str = sales_items_list($name, $selected_id, $all_option, $submit_on_change,
		'kits', array('cells'=>false));
	return $str;
}

function sales_local_items_list_row($label, $name, $selected_id=null, $all_option=false, $submit_on_change=false)
{
	echo "<tr>";
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = sales_items_list($name, $selected_id, $all_option, $submit_on_change,
		'local', array('cells'=>false));
	echo "</td></tr>";
	return $str;
}
//------------------------------------------------------------------------------------

function base_stock_items_list($where, $name, $selected_id=null,
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
		'where' => $where,
		'category' => 2	) );
}
//------------------------------------------------------------------------------------

function stock_bom_items_list($name, $selected_id=null, $all_option=false, $submit_on_change=false)
{

	return base_stock_items_list("s.mb_flag='M'",
		$name, $selected_id, $all_option, $submit_on_change);
}
/*
function stock_bom_items_list_cells($label, $name, $selected_id=null, $all_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = stock_bom_items_list($name, $selected_id, $all_option, $submit_on_change);
	echo "</td>\n";
	return $str;
}

function stock_bom_items_list_row($label, $name, $selected_id=null, $all_option=false, $submit_on_change=false)
{
	echo "<tr>\n";
	$str = stock_bom_items_list_cells($label, $name, $selected_id, $all_option, $submit_on_change);
	echo "</tr>\n";
	return $str;
}
*/
//------------------------------------------------------------------------------------

function stock_manufactured_items_list($name, $selected_id=null,
	$all_option=false, $submit_on_change=false)
{
	return base_stock_items_list("s.mb_flag='M'",
		$name, $selected_id, $all_option, $submit_on_change);
}

function stock_manufactured_items_list_cells($label, $name, $selected_id=null,
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

function stock_manufactured_items_list_row($label, $name, $selected_id=null,
		$all_option=false, $submit_on_change=false)
{
	echo "<tr>\n";
	$str = stock_manufactured_items_list_cells($label, $name, $selected_id,
			$all_option, $submit_on_change);
	echo "</tr>\n";
	return $str;
}
//------------------------------------------------------------------------------------

function stock_component_items_list($name, $parent_stock_id, $selected_id=null,
	$all_option=false, $submit_on_change=false)
{
	$str = stock_items_list($name, $selected_id, $all_option, $submit_on_change,
		array('where'=>array("stock_id != '$parent_stock_id'")));
	return $str;
}

function stock_component_items_list_cells($label, $name, $parent_stock_id, 
	$selected_id=null, $all_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	$str = stock_items_list($name, $selected_id, $all_option, $submit_on_change,
		array('where'=>array("stock_id != '$parent_stock_id'"), 'cells'=>true));
	return $str;
}
//------------------------------------------------------------------------------------

function stock_costable_items_list($name, $selected_id=null,
	$all_option=false, $submit_on_change=false)
{
	$str = stock_items_list($name, $selected_id, $all_option, $submit_on_change,
		array('where'=>array("mb_flag!='D'")));
	return $str;
}

function stock_costable_items_list_cells($label, $name, $selected_id=null, 
	$all_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	$str = stock_items_list($name, $selected_id, $all_option, $submit_on_change,
		array('where'=>array("mb_flag!='D'"), 'cells'=>true));
	return $str;
}

//------------------------------------------------------------------------------------
function stock_purchasable_items_list($name, $selected_id=null,	
	$all_option=false, $submit_on_change=false)
{
	$str = stock_items_list($name, $selected_id, $all_option, $submit_on_change,
		array('where'=>array("mb_flag!= 'M'"), 
			'edit_submit' => true));
	return $str;
}

function stock_purchasable_items_list_cells($label, $name, $selected_id=null,
			$all_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	$str = stock_items_list($name, $selected_id, $all_option, $submit_on_change,
		array('where'=>array("mb_flag!= 'M'"), 
			 'edit_submit' => true,
			 'cells'=>true));
	return $str;
}

function stock_purchasable_items_list_row($label, $name, $selected_id=null,
			$all_option=false, $submit_on_change=false)
{
	echo "<tr>\n";
	$ret = stock_purchasable_items_list_cells($label, $name, $selected_id=null,
		$all_option, $submit_on_change);
	echo "</tr>\n";
	return $ret;
}

//------------------------------------------------------------------------------------

function stock_item_types_list_row($label, $name, $selected_id=null, $enabled=true)
{
	$types = array(
		'M' => _("Manufactured"),
		'B' => _("Purchased"),
		'D' => _("Service")
	);

	echo "<tr>";
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";

	array_selector($name, $selected_id, $types, 
		array( 
			'select_submit'=> true, 
			'disabled' => !$enabled) );
	echo "</td></tr>\n";
}

function stock_units_list_row($label, $name, $value=null, $enabled=true)
{
	$result = get_all_item_units();
	echo "<tr>";
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";

	while($unit = db_fetch($result))
		$units[$unit['abbr']] = $unit['name'];

	array_selector($name, $value, $units, 
		array( 
			'select_submit'=> true, 
			'disabled' => !$enabled) );

	echo "</td></tr>\n";
}

//------------------------------------------------------------------------------------

function tax_types_list($name, $selected_id=null, $none_option=false, $submit_on_change=false)
{
	$sql = "SELECT id, CONCAT(name, ' (',rate,'%)') as name FROM ".TB_PREF."tax_types";

	return combo_input($name, $selected_id, $sql, 'id', 'name',
	array(
		'spec_option' => $none_option,
		'spec_id' => reserved_words::get_all_numeric(),
		'select_submit'=> $submit_on_change,
		'async' => false,
	) );
}

function tax_types_list_cells($label, $name, $selected_id=null, $none_option=false,
	$submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = tax_types_list($name, $selected_id, $none_option, $submit_on_change);
	echo "</td>\n";
	return $str;
}

function tax_types_list_row($label, $name, $selected_id=null, $none_option=false,
	$submit_on_change=false)
{
	echo "<tr>\n";
	$str = tax_types_list_cells($label, $name, $selected_id, $none_option, $submit_on_change);
	echo "</tr>\n";
	return $str;
}

//------------------------------------------------------------------------------------

function tax_groups_list($name, $selected_id=null,
	$none_option=false, $submit_on_change=false)
{
	$sql = "SELECT id, name FROM ".TB_PREF."tax_groups";

	return combo_input($name, $selected_id, $sql, 'id', 'name',
	array(
		'order' => 'id',
		'spec_option' => $none_option,
		'spec_id' => reserved_words::get_all_numeric(),
		'select_submit'=> $submit_on_change,
		'async' => false,
	) );
}

function tax_groups_list_cells($label, $name, $selected_id=null, $none_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = tax_groups_list($name, $selected_id, $none_option, $submit_on_change);
	echo "</td>\n";
	return $str;
}

function tax_groups_list_row($label, $name, $selected_id=null, $none_option=false, $submit_on_change=false)
{
	echo "<tr>\n";
	$str = tax_groups_list_cells($label, $name, $selected_id, $none_option, $submit_on_change);
	echo "</tr>\n";
	return $str;
}

//------------------------------------------------------------------------------------

function item_tax_types_list($name, $selected_id=null)
{
	$sql ="SELECT id, name FROM ".TB_PREF."item_tax_types";
	combo_input($name, $selected_id, $sql, 'id', 'name', array('order' => 'id') );
}

function item_tax_types_list_cells($label, $name, $selected_id=null)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	item_tax_types_list($name, $selected_id);
	echo "</td>\n";
}

function item_tax_types_list_row($label, $name, $selected_id=null)
{
	echo "<tr>\n";
	item_tax_types_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function shippers_list($name, $selected_id=null)
{
	$sql = "SELECT shipper_id, shipper_name FROM ".TB_PREF."shippers";
	combo_input($name, $selected_id, $sql, 'shipper_id', 'shipper_name', array());
}

function shippers_list_cells($label, $name, $selected_id=null)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	shippers_list($name, $selected_id);
	echo "</td>\n";
}

function shippers_list_row($label, $name, $selected_id=null)
{
	echo "<tr>\n";
	shippers_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//-------------------------------------------------------------------------------------

function sales_persons_list($name, $selected_id=null)
{
	$sql = "SELECT salesman_code, salesman_name FROM ".TB_PREF."salesman";
	combo_input($name, $selected_id, $sql, 'salesman_code', 'salesman_name', array());
}

function sales_persons_list_cells($label, $name, $selected_id=null)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>\n";
	sales_persons_list($name, $selected_id);
	echo "</td>\n";
}

function sales_persons_list_row($label, $name, $selected_id=null, $submit_on_change=false)
{
	echo "<tr>\n";
	sales_persons_list_cells($label, $name, $selected_id, $submit_on_change=false);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function sales_areas_list($name, $selected_id=null)
{
	$sql = "SELECT area_code, description FROM ".TB_PREF."areas";
	combo_input($name, $selected_id, $sql, 'area_code', 'description', array());
}

function sales_areas_list_cells($label, $name, $selected_id=null)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	sales_areas_list($name, $selected_id);
	echo "</td>\n";
}

function sales_areas_list_row($label, $name, $selected_id=null)
{
	echo "<tr>\n";
	sales_areas_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function sales_groups_list($name, $selected_id=null, $special_option=false)
{
	$sql = "SELECT id, description FROM ".TB_PREF."groups";
	combo_input($name, $selected_id, $sql, 'id', 'description', array(
		'spec_option' => $special_option===true ? ' ' : $special_option,
		'order' => 'description', 'spec_id' => 0,
	));
}

function sales_groups_list_cells($label, $name, $selected_id=null, $special_option=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	sales_groups_list($name, $selected_id, $special_option);
	echo "</td>\n";
}

function sales_groups_list_row($label, $name, $selected_id=null, $special_option=false)
{
	echo "<tr>\n";
	sales_groups_list_cells($label, $name, $selected_id, $special_option);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function _format_template_items($row)
{
	return ($row[0] . "&nbsp;- &nbsp;" . _("Amount") . "&nbsp;".$row[1]);
}

function templates_list($name, $selected_id=null, $special_option=false)
{
	$sql = "SELECT sorder.order_no,	Sum(line.unit_price*line.quantity*(1-line.discount_percent)) AS OrderValue
		FROM ".TB_PREF."sales_orders as sorder, ".TB_PREF."sales_order_details as line
		WHERE sorder.order_no = line.order_no AND sorder.type = 1 GROUP BY line.order_no";
	combo_input($name, $selected_id, $sql, 'order_no', 'OrderValue', array(
		'format' => '_format_template_items',
		'spec_option' => $special_option===true ? ' ' : $special_option,
		'order' => 'order_no', 'spec_id' => 0,
	));
}

function templates_list_cells($label, $name, $selected_id=null, $special_option=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	templates_list($name, $selected_id, $special_option);
	echo "</td>\n";
}

function templates_list_row($label, $name, $selected_id=null, $special_option=false)
{
	echo "<tr>\n";
	templates_list_cells($label, $name, $selected_id, $special_option);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function workorders_list($name, $selected_id=null)
{
	$sql = "SELECT id, wo_ref FROM ".TB_PREF."workorders WHERE closed=0";
	combo_input($name, $selected_id, $sql, 'id', 'wo_ref', array());
}

function workorders_list_cells($label, $name, $selected_id=null)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	workorders_list($name, $selected_id);
	echo "</td>\n";
}

function workorders_list_row($label, $name, $selected_id=null)
{
	echo "<tr>\n";
	workorders_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function payment_terms_list($name, $selected_id=null)
{
	$sql = "SELECT terms_indicator, terms FROM ".TB_PREF."payment_terms";
	combo_input($name, $selected_id, $sql, 'terms_indicator', 'terms', array());
}

function payment_terms_list_cells($label, $name, $selected_id=null)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	payment_terms_list($name, $selected_id);
	echo "</td>\n";
}

function payment_terms_list_row($label, $name, $selected_id=null)
{
	echo "<tr>\n";
	payment_terms_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function credit_status_list($name, $selected_id=null)
{
	$sql ="SELECT id, reason_description FROM ".TB_PREF."credit_status";
	combo_input($name, $selected_id, $sql, 'id', 'reason_description', array());
}

function credit_status_list_cells($label, $name, $selected_id=null)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	credit_status_list($name, $selected_id);
	echo "</td>\n";
}

function credit_status_list_row($label, $name, $selected_id=null)
{
	echo "<tr>\n";
	credit_status_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------

function sales_types_list($name, $selected_id=null, $submit_on_change=false, $special_option=false)
{
	$sql = "SELECT id, sales_type FROM ".TB_PREF."sales_types";

	return combo_input($name, $selected_id, $sql, 'id', 'sales_type',
	array(
		'spec_option' => $special_option===true ? _("All Sales Types") : $special_option,
		'spec_id' => 0,
		'select_submit'=> $submit_on_change,
	//	  'async' => false,
	) );
}

function sales_types_list_cells($label, $name, $selected_id=null, $submit_on_change=false, $special_option=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = sales_types_list($name, $selected_id, $submit_on_change, $special_option);
	echo "</td>\n";
	return $str;
}

function sales_types_list_row($label, $name, $selected_id=null, $submit_on_change=false, $special_option=false)
{
	echo "<tr>\n";
	$str = sales_types_list_cells($label, $name, $selected_id, $submit_on_change, $special_option);
	echo "</tr>\n";
	return $str;
}

//-----------------------------------------------------------------------------------------------

function movement_types_list($name, $selected_id=null)
{
	$sql = "SELECT id, name FROM ".TB_PREF."movement_types";
	combo_input($name, $selected_id, $sql, 'id', 'name', array());
}

function movement_types_list_cells($label, $name, $selected_id=null)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	movement_types_list($name, $selected_id);
	echo "</td>\n";
}

function movement_types_list_row($label, $name, $selected_id=null)
{
	echo "<tr>\n";
	movement_types_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------
function _format_date($row)
{
	return sql2date($row['reconciled']);
}

function bank_reconciliation_list($account, $name, $selected_id=null, $submit_on_change=false, $special_option=false)
{
	$sql = "SELECT reconciled, reconciled FROM ".TB_PREF."bank_trans
		WHERE bank_act=".db_escape($account)." AND reconciled IS NOT NULL
		GROUP BY reconciled";
	combo_input($name, $selected_id, $sql, 'id', 'reconciled',
	array(
		'spec_option' => $special_option,
		'format' => '_format_date',
		'spec_id' => '',
		'select_submit'=> $submit_on_change
	) );
}

function bank_reconciliation_list_cells($label,$account, $name, $selected_id=null, $submit_on_change=false, $special_option=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	bank_reconciliation_list($account, $name, $selected_id, $submit_on_change, $special_option);
	echo "</td>\n";
}
/*
function bank_reconciliation_list_row($label, $account, $name, $selected_id=null, $submit_on_change=false, $special_option=false)
{
	echo "<tr>\n";
	bank_reconciliation_list_cells($label, $account, $name, $selected_id, $submit_on_change, $special_option);
	echo "</tr>\n";
}
*/
//-----------------------------------------------------------------------------------------------

function workcenter_list($name, $selected_id=null, $all_option=false)
{
	global $all_items;

	$sql = "SELECT id, name FROM ".TB_PREF."workcentres";

	return combo_input($name, $selected_id, $sql, 'id', 'name',
	array(
		'spec_option' =>$all_option===true ? _("All Suppliers") : $all_option,
		'spec_id' => $all_items,
	) );
}

function workcenter_list_cells($label, $name, $selected_id=null, $all_option=false)
{
	default_focus($name);
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	workcenter_list($name, $selected_id, $all_option);
	echo "</td>\n";
}

function workcenter_list_row($label, $name, $selected_id=null, $all_option=false)
{
	echo "<tr>\n";
	workcenter_list_cells($label, $name, $selected_id, $all_option);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------

function bank_accounts_list($name, $selected_id=null, $submit_on_change=false)
{
	$sql = "SELECT ".TB_PREF."bank_accounts.id, bank_account_name, bank_curr_code
		FROM ".TB_PREF."bank_accounts";
//		, ".TB_PREF."chart_master
//		WHERE ".TB_PREF."bank_accounts.account_code=".TB_PREF."chart_master.account_code";

	return combo_input($name, $selected_id, $sql, 'id', 'bank_account_name',
	array(
		'format' => '_format_add_curr',
		'select_submit'=> $submit_on_change,
		'async' => false
	) );
}

function bank_accounts_list_cells($label, $name, $selected_id=null, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = bank_accounts_list($name, $selected_id, $submit_on_change);
	echo "</td>\n";
	return $str;
}

function bank_accounts_list_row($label, $name, $selected_id=null, $submit_on_change=false)
{
	echo "<tr>\n";
	$str = bank_accounts_list_cells($label, $name, $selected_id, $submit_on_change);
	echo "</tr>\n";
	return $str;
}
//-----------------------------------------------------------------------------------------------

function cash_accounts_list_row($label, $name, $selected_id=null, $submit_on_change=false)
{
	$sql = "SELECT ".TB_PREF."bank_accounts.id, bank_account_name, bank_curr_code
		FROM ".TB_PREF."bank_accounts
		WHERE ".TB_PREF."bank_accounts.account_type=3";

	if ($label != null)
		echo "<tr><td>$label</td>\n";
	echo "<td>";
	$str = combo_input($name, $selected_id, $sql, 'id', 'bank_account_name',
	array(
		'format' => '_format_add_curr',
		'select_submit'=> $submit_on_change,
		'async' => true
	) );
	echo "</td></tr>\n";
	return $str;
}
//-----------------------------------------------------------------------------------------------

function pos_list_row($label, $name, $selected_id=null, $spec_option=false, $submit_on_change=false)
{
	$sql = "SELECT id, pos_name FROM ".TB_PREF."sales_pos";

	default_focus($name);
	echo '<tr>';
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";

	$str = combo_input($name, $selected_id, $sql, 'id', 'pos_name',
	array(
		'select_submit'=> $submit_on_change,
		'async' => true,
		'spec_option' =>$spec_option,
		'spec_id' => -1,
	) );
	echo "</td></tr>\n";

	return $str;	
}
//-----------------------------------------------------------------------------------------------

function sale_payment_list_cells($label, $name, $selected_id=null, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = yesno_list($name, $selected_id, _('Cash'), _('Delayed'), $submit_on_change);
	echo "</td>\n";
	return $str;
}
//-----------------------------------------------------------------------------------------------

function class_list($name, $selected_id=null, $submit_on_change=false)
{
	$sql = "SELECT cid, class_name FROM ".TB_PREF."chart_class";

	return combo_input($name, $selected_id, $sql, 'cid', 'class_name',
	array(
		'select_submit'=> $submit_on_change,
		'async' => false
	) );

}

function class_list_cells($label, $name, $selected_id=null, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = class_list($name, $selected_id, $submit_on_change);
	echo "</td>\n";
	return $str;
}

function class_list_row($label, $name, $selected_id=null, $submit_on_change=false)
{
	echo "<tr>\n";
	$str = class_list_cells($label, $name, $selected_id, $submit_on_change);
	echo "</tr>\n";
	return $str;
}

//-----------------------------------------------------------------------------------------------

function stock_categories_list($name, $selected_id=null)
{
	$sql = "SELECT category_id, description FROM ".TB_PREF."stock_category";
	combo_input($name, $selected_id, $sql, 'category_id', 'description',
	array('order'=>'category_id'));
}

function stock_categories_list_cells($label, $name, $selected_id=null)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	stock_categories_list($name, $selected_id);
	echo "</td>\n";
}

function stock_categories_list_row($label, $name, $selected_id=null)
{
	echo "<tr>\n";
	stock_categories_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------

function gl_account_types_list($name, $selected_id=null, $all_option, $all_option_numeric)
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

function gl_account_types_list_cells($label, $name, $selected_id=null, $all_option=false,
	$all_option_numeric=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	gl_account_types_list($name, $selected_id, $all_option, $all_option_numeric);
	echo "</td>\n";
}

function gl_account_types_list_row($label, $name, $selected_id=null, $all_option=false,
	$all_option_numeric=false)
{
	echo "<tr>\n";
	gl_account_types_list_cells($label, $name, $selected_id, $all_option,
		$all_option_numeric);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------
function gl_all_accounts_list($name, $selected_id=null, $skip_bank_accounts=false,
	$cells=false, $all_option=false, $submit_on_change=false)
{
	if ($skip_bank_accounts)
		$sql = "SELECT chart.account_code, chart.account_name, type.name
			FROM (".TB_PREF."chart_master chart,".TB_PREF."chart_types type) "
			."LEFT JOIN ".TB_PREF."bank_accounts acc "
			."ON chart.account_code=acc.account_code
				WHERE acc.account_code  IS NULL
			AND chart.account_type=type.id";
	else
		$sql = "SELECT chart.account_code, chart.account_name, type.name
			FROM ".TB_PREF."chart_master chart,".TB_PREF."chart_types type
			WHERE chart.account_type=type.id";

	combo_input($name, $selected_id, $sql, 'chart.account_code', 'chart.account_name',
	array(
		'format' => '_format_account',
		'spec_option' => $all_option===true ?  _("Use Item Sales Accounts") : $all_option,
		'spec_id' => '',
		'order' => array('name','account_code'),
		'search_box' => $cells,
			'search_submit' => false,
			'size' => 12,
			'max' => 10,
			'cells' => true,
		'select_submit'=> $submit_on_change,
		'async' => false,
		'category' => 2
	) );

}

function _format_account($row)
{
		return $row[0] .  "&nbsp;&nbsp;&nbsp;&nbsp;" . $row[1];
}

function gl_all_accounts_list_cells($label, $name, $selected_id=null, 
	$skip_bank_accounts=false, $cells=false, $all_option=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	gl_all_accounts_list($name, $selected_id, 
		$skip_bank_accounts, $cells, $all_option);
	echo "</td>\n";
}

function gl_all_accounts_list_row($label, $name, $selected_id=null, 
	$skip_bank_accounts=false, $cells=false, $all_option=false)
{
	echo "<tr>\n";
	gl_all_accounts_list_cells($label, $name, $selected_id, 
		$skip_bank_accounts, $cells, $all_option);
	echo "</tr>\n";
}

function yesno_list($name, $selected_id=null, $name_yes="", $name_no="", $submit_on_change=false)
{
	$items = array();
	$items['0'] = strlen($name_no) ? $name_no : _("No");
	$items['1'] = strlen($name_yes) ? $name_yes : _("Yes");

	return array_selector($name, $selected_id, $items, 
		array( 
			'select_submit'=> $submit_on_change,
			'async' => false ) ); // FIX?
}

function yesno_list_cells($label, $name, $selected_id=null, $name_yes="", $name_no="", $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = yesno_list($name, $selected_id, $name_yes, $name_no, $submit_on_change);
	echo "</td>\n";
	return $str;
}

function yesno_list_row($label, $name, $selected_id=null, $name_yes="", $name_no="", $submit_on_change=false)
{
	echo "<tr>\n";
	$str = yesno_list_cells($label, $name, $selected_id, $name_yes, $name_no, $submit_on_change);
	echo "</tr>\n";
	return $str;
}

//------------------------------------------------------------------------------------------------

function languages_list($name, $selected_id=null)
{
	global $installed_languages;

	$items = array();
	foreach ($installed_languages as $lang)
			$items[$lang['code']] = $lang['name'];
	
	return array_selector($name, $selected_id, $items );
}

function languages_list_cells($label, $name, $selected_id=null)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	languages_list($name, $selected_id);
	echo "</td>\n";
}

function languages_list_row($label, $name, $selected_id=null)
{
	echo "<tr>\n";
	languages_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------------------

function bank_account_types_list($name, $selected_id=null)
{
	$types = bank_account_types::get_all();

	$items = array();
	foreach ($types as $type)
	{
			$items[$type['id']] = $type['name'];
	}
	
	return array_selector($name, $selected_id, $items );
}

function bank_account_types_list_cells($label, $name, $selected_id=null)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	bank_account_types_list($name, $selected_id);
	echo "</td>\n";
}

function bank_account_types_list_row($label, $name, $selected_id=null)
{
	echo "<tr>\n";
	bank_account_types_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------------------
function payment_person_types_list($name, $selected_id=null, $submit_on_change=false)
{
	$types = payment_person_types::get_all();

	$items = array();
	foreach ($types as $type)
	{
		if (payment_person_types::has_items($type['id']))
		{
			$items[$type['id']] = $type['name'];
		}
	}
	
	return array_selector($name, $selected_id, $items, 
		array( 'select_submit'=> $submit_on_change ) );
}

function payment_person_types_list_cells($label, $name, $selected_id=null, $related=null)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	$str = payment_person_types_list($name, $selected_id, $related);
	echo "</td>\n";
	return $str;
}

function payment_person_types_list_row($label, $name, $selected_id=null, $related=null)
{
	echo "<tr>\n";
	$str = payment_person_types_list_cells($label, $name, $selected_id, $related);
	echo "</tr>\n";
	return $str;
}

//------------------------------------------------------------------------------------------------

function wo_types_list($name, $selected_id=null)
{
	$types = wo_types::get_all();

	$items = array();
	foreach ($types as $type)
		$items[$type['id']] = $type['name'];
	
	return array_selector($name, $selected_id, $items, 
		array( 'select_submit'=> true, 'async' => true ) );
}

function wo_types_list_row($label, $name, $selected_id=null)
{
	echo "<tr><td>$label</td><td>\n";
	$str = wo_types_list($name, $selected_id);
	echo "</td></tr>\n";
	return $str;
}

//------------------------------------------------------------------------------------------------

function dateformats_list_row($label, $name, $value=null)
{
	global $dateformats;

	echo "<tr><td>$label</td>\n<td>";
	array_selector( $name, $value, $dateformats );
	echo "</td></tr>\n";
}

function dateseps_list_row($label, $name, $value=null)
{
	global $dateseps;

	echo "<tr><td>$label</td>\n<td>";
	array_selector( $name, $value, $dateseps );
	echo "</td></tr>\n";
}

function thoseps_list_row($label, $name, $value=null)
{
	global $thoseps;

	echo "<tr><td>$label</td>\n<td>";
	array_selector( $name, $value, $thoseps );
	echo "</td></tr>\n";
}

function decseps_list_row($label, $name, $value=null)
{
	global $decseps;

	echo "<tr><td>$label</td>\n<td>";
	array_selector( $name, $value, $decseps );
	echo "</td></tr>\n";
}

function themes_list_row($label, $name, $value=null)
{
	global $path_to_root;

	$path = $path_to_root.'/themes/';
	$themes = array();
	$themedir = opendir($path);
	while(false !== ($fname = readdir($themedir)))
	{
		if($fname!='.' && $fname!='..' && $fname!='CVS' && is_dir($path.$fname))
		{
			$themes[$fname] =  $fname;
		}
	}
	ksort($themes);

	echo "<tr><td>$label</td>\n<td>";
	array_selector( $name, $value, $themes );
	echo "</td></tr>\n";
}

function pagesizes_list_row($label, $name, $value=null)
{
	global $pagesizes;

	$items = array();
	foreach ($pagesizes as $pz)
		$items[$pz] = $pz;

	echo "<tr><td>$label</td>\n<td>";
	array_selector( $name, $value, $items );
	echo "</td></tr>\n";
}

function security_headings_list_row($label, $name, $value=null)
{
	global $security_headings;

	echo "<tr><td>$label</td>\n<td>";
	array_selector( $name, $value, $security_headings );
	echo "</td></tr>\n";
}

function systypes_list_cells($label, $name, $value=null, $submit_on_change=false)
{
	global $systypes_array;

	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";

	$items = array();
	foreach ($systypes_array as $key=>$type)
			$items[$key] = $type['name'];
	
	$str = array_selector($name, $value, $items, 
		array( 
			'select_submit'=> $submit_on_change,
			'async' => false
			)
	);
	echo "</td>\n";
	return $str;
}

function systypes_list_row($label, $name, $value=null, $submit_on_change=false)
{
	echo "<tr>\n";
	$str = systypes_list_cells($label, $name, $value, $submit_on_change);
	echo "</tr>\n";
	return $str;
}

function cust_allocations_list_cells($label, $name, $selected=null)
{
	global $all_items;

	if ($label != null)
		label_cell($label);
	echo "<td>\n";
	$allocs = array( 
		$all_items=>_("All Types"),
		'1'=> _("Sales Invoices"),
		'2'=> _("Overdue Invoices"),
		'3' => _("Payments"),
		'4' => _("Credit Notes"),
		'5' => _("Delivery Notes")
	);
	$str = array_selector($name, $selected, $allocs);
	echo "</td>\n";
	return $str;
}

function supp_allocations_list_cell($name, $selected=null)
{
	global $all_items;

	echo "<td>\n";
	$allocs = array( 
		$all_items=>_("All Types"),
		'1'=> _("Invoices"),
		'2'=> _("Overdue Invoices"),
		'3' => _("Payments"),
		'4' => _("Credit Notes"),
		'5' => _("Overdue Credit Notes")
	);
	$str = array_selector($name, $selected, $allocs);
	echo "</td>\n";
	return $str;
}

function policy_list_cells($label, $name, $selected=null)
{
	if ($label != null)
		label_cell($label);
	echo "<td>\n";
	$str = array_selector($name, $selected, 
				array( '' => _("Automatically put balance on back order"),
			 		'CAN' => _("Cancel any quantites not delivered")) );
	echo "</td>\n";
	return $str;
}

function policy_list_row($label, $name, $selected=null)
{
	echo "<tr>\n";
	policy_list_cells($label, $name, $selected);
	echo "</tr>\n";
}

function credit_type_list_cells($label, $name, $selected=null, $submit_on_change=false)
{
	if ($label != null)
		label_cell($label);
	echo "<td>\n";
	$str = array_selector($name, $selected, 
				array( 'Return' => _("Items Returned to Inventory Location"),
			 		'WriteOff' => _("Items Written Off")),
				array( 'select_submit'=> $submit_on_change ) );
	echo "</td>\n";
	return $str;
}

function credit_type_list_row($label, $name, $selected=null, $submit_on_change=false)
{
	echo "<tr>\n";
	$str = credit_type_list_cells($label, $name, $selected, $submit_on_change);
	echo "</tr>\n";
	return $str;
}

function number_list($name, $selected, $from, $to, $no_option=false)
{
	$items = array();
	for ($i = $from; $i <= $to; $i++)
		$items[$i] = "$i";

	return array_selector($name, $selected, $items,
				array(	'spec_option' => $no_option,
						'spec_id' => reserved_words::get_all_numeric()) );
}

function number_list_cells($label, $name, $selected, $from, $to, $no_option=false)
{
	if ($label != null)
		label_cell($label);
	echo "<td>\n";
	number_list($name, $selected, $from, $to, $no_option);
	echo "</td>\n";
}

function number_list_row($label, $name, $selected, $from, $to, $no_option=false)
{
	echo "<tr>\n";
	number_list_cells($label, $name, $selected, $from, $to, $no_option);
	echo "</tr>\n";
}

function print_profiles_list_row($label, $name, $selected_id=null, $spec_opt=false,
	$submit_on_change=true)
{
	$sql = "SELECT profile FROM ".TB_PREF."print_profiles"
		." GROUP BY profile";
	$result = db_query($sql, 'cannot get all profile names');
	$profiles = array();
	while($myrow=db_fetch($result)) {
		$profiles[$myrow['profile']] = $myrow['profile'];
	}

	echo "<tr>";
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";

	array_selector($name, $selected_id, $profiles, 
		array( 'select_submit'=> $submit_on_change,
			'spec_option'=>$spec_opt,
			'spec_id' => ''
		 ));

	echo "</td></tr>\n";
}

function printers_list($name, $selected_id=null, $spec_opt=false, $submit_on_change=false)
{
	static $printers; // query only once for page display

	if (!$printers) {
		$sql = "SELECT id, name, description FROM ".TB_PREF."printers";	
		$result = db_query($sql, 'cannot get all printers');
		$printers = array();
		while($myrow=db_fetch($result)) {
			$printers[$myrow['id']] = $myrow['name'].'&nbsp;-&nbsp;'.$myrow['description'];
		}
	}
	array_selector($name, $selected_id, $printers, 
		array( 'select_submit'=> $submit_on_change,
			'spec_option'=>$spec_opt,
			'spec_id' => ''
		 ));
}

//------------------------------------------------------------------------------------------------

function quick_entries_list($name, $selected_id=null, $type=null, $submit_on_change=false)
{
	$where = false;
	$sql = "SELECT id, description FROM ".TB_PREF."quick_entries";
	if ($type != null)
		$sql .= " WHERE type=$type";

	combo_input($name, $selected_id, $sql, 'id', 'description',
		array(
			'spec_id' => '',
			'order' => 'description',
			'select_submit'=> $submit_on_change,
			'async' => false
		) );

}

function quick_entries_list_cells($label, $name, $selected_id=null, $type, $submit_on_change=false)
{
	echo "<td>$label</td><td>\n";
	quick_entries_list($name, $selected_id, $type, $submit_on_change);
	echo "</td>";
}

function quick_entries_list_row($label, $name, $selected_id=null, $type, $submit_on_change=false)
{
	echo "<tr>\n";
	quick_entries_list_cells($label, $name, $selected_id, $type, $submit_on_change);
	echo "</tr>\n";
}


function quick_actions_list_row($label, $name, $selected_id=null, $submit_on_change=false)
{
	global $quick_actions;
	
	echo "<tr><td>$label</td><td>";
	array_selector($name, $selected_id, $quick_actions, 
		array( 
			'select_submit'=> $submit_on_change
		) );
	echo "</td></tr>\n";
}

function quick_entry_types_list_row($label, $name, $selected_id=null, $submit_on_change=false)
{
	global $quick_entry_types;
		
	echo "<tr><td>$label</td><td>";
	array_selector($name, $selected_id, $quick_entry_types, 
		array( 
			'select_submit'=> $submit_on_change
			) );
	echo "</td></tr>\n";
}


?>