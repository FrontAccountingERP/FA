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
include_once ($path_to_root . "/includes/ui/ListRenderer.inc");

// TODO Move these variables inside the ListRenderer constructor. CP 2014-11
define('SEARCH_BUTTON', "<input %s type='submit' class='combo_submit' style='border:0;background:url($path_to_root/themes/"
	."%s/images/locate.png) no-repeat;%s' aspect='fallback' name='%s' value=' ' title='"._("Set filter")."'> ");

define('SELECT_BUTTON', "<input %s type='submit' class='combo_select' style='border:0;background:url($path_to_root/themes/"
	."%s/images/button_ok.png) no-repeat;%s' aspect='fallback' name='%s' value=' ' title='"._("Select")."'> ");

/*
 * Note that the order of instantiation of the above strings (in the overall context) was important.
 * When these strings were in ListRenderer.inc the XHR request on combo boxes stopped working.
 * I suspect when moved into say the ListRenderer class and instantiated from the constructor they
 * will be fine. CP 2014-11
 */

// ----------------------------------------------------------------------------
// Universal sql combo generator
// $sql must return selector values and selector texts in columns 0 & 1
// Options are merged with defaults.
function combo_input($name, $selected_id, $sql, $valfield, $namefield, $options = null)
{
	return ListRenderer::get()->combo_input($name, $selected_id, $sql, $valfield, $namefield, $options);
}

/*
 * Helper function. Returns true if selector $name is subject to update.
 */
function list_updated($name)
{
	return ListRenderer::get()->list_updated($name);
}

// ----------------------------------------------------------------------------------------------
// Universal array combo generator
// $items is array of options 'value' => 'description'
// Options is reduced set of combo_selector options and is merged with defaults.
function array_selector($name, $selected_id, $items, $options = null)
{
	return ListRenderer::get()->array_selector($name, $selected_id, $items, $options);
}
// ----------------------------------------------------------------------------------------------
function array_selector_row($label, $name, $selected_id, $items, $options=null)
{
	ListRenderer::get()->array_selector_row($label, $name, $selected_id, $items, $options);
}

//----------------------------------------------------------------------------------------------
function _format_add_curr($row)
{
	return ListRenderer::get()->_format_add_curr($row);
}

function add_edit_combo($type)
{
	return ListRenderer::get()->add_edit_combo($type);
}

function supplier_list($name, $selected_id = null, $spec_option = false, $submit_on_change = false, $all = false, $editkey = false)
{
	return ListRenderer::get()->supplier_list($name, $selected_id, $spec_option, $submit_on_change, $all, $editkey);
}

function supplier_list_cells($label, $name, $selected_id = null, $all_option = false, $submit_on_change = false, $all = false, $editkey = false)
{
	return ListRenderer::get()->supplier_list_cells($label, $name, $selected_id, $all_option, $submit_on_change, $all, $editkey);
}

function supplier_list_row($label, $name, $selected_id = null, $all_option = false, $submit_on_change = false, $all = false, $editkey = false)
{
	return ListRenderer::get()->supplier_list_row($label, $name, $selected_id, $all_option, $submit_on_change, $all, $editkey);
}

// ----------------------------------------------------------------------------------------------
function customer_list($name, $selected_id = null, $spec_option = false, $submit_on_change = false, $show_inactive = false, $editkey = false)
{
	return ListRenderer::get()->customer_list($name, $selected_id, $spec_option, $submit_on_change, $show_inactive, $editkey);
}

function customer_list_cells($label, $name, $selected_id = null, $all_option = false, $submit_on_change = false, $show_inactive = false, $editkey = false)
{
	return ListRenderer::get()->customer_list_cells($label, $name, $selected_id, $all_option, $submit_on_change, $show_inactive, $editkey);
}

function customer_list_row($label, $name, $selected_id = null, $all_option = false, $submit_on_change = false, $show_inactive = false, $editkey = false)
{
	return ListRenderer::get()->customer_list_row($label, $name, $selected_id, $all_option, $submit_on_change, $show_inactive, $editkey);
}

// ------------------------------------------------------------------------------------------------
function customer_branches_list($customer_id, $name, $selected_id = null, $spec_option = true, $enabled = true, $submit_on_change = false, $editkey = false)
{
	return ListRenderer::get()->customer_branches_list($customer_id, $name, $selected_id, $spec_option, $enabled, $submit_on_change, $editkey);
}
// ------------------------------------------------------------------------------------------------
function customer_branches_list_cells($label, $customer_id, $name, $selected_id = null, $all_option = true, $enabled = true, $submit_on_change = false, $editkey = false)
{
	return ListRenderer::get()->customer_branches_list_cells($label, $customer_id, $name, $selected_id, $all_option, $enabled, $submit_on_change, $editkey);
}

function customer_branches_list_row($label, $customer_id, $name, $selected_id = null, $all_option = true, $enabled = true, $submit_on_change = false, $editkey = false)
{
	return ListRenderer::get()->customer_branches_list_row($label, $customer_id, $name, $selected_id, $all_option, $enabled, $submit_on_change, $editkey);
}

// ------------------------------------------------------------------------------------------------
function locations_list($name, $selected_id = null, $all_option = false, $submit_on_change = false)
{
	return ListRenderer::get()->locations_list($name, $selected_id = null, $all_option, $submit_on_change);
}

function locations_list_cells($label, $name, $selected_id = null, $all_option = false, $submit_on_change = false)
{
	return ListRenderer::get()->locations_list_cells($label, $name, $selected_id, $all_option, $submit_on_change);
}

function locations_list_row($label, $name, $selected_id = null, $all_option = false, $submit_on_change = false)
{
	return ListRenderer::get()->locations_list_row($label, $name, $selected_id, $all_option, $submit_on_change);
}

// -----------------------------------------------------------------------------------------------
function currencies_list($name, $selected_id = null, $submit_on_change = false)
{
	return ListRenderer::get()->currencies_list($name, $selected_id, $submit_on_change);
}

function currencies_list_cells($label, $name, $selected_id = null, $submit_on_change = false)
{
	return ListRenderer::get()->currencies_list_cells($label, $name, $selected_id, $submit_on_change);
}

function currencies_list_row($label, $name, $selected_id = null, $submit_on_change = false)
{
	return ListRenderer::get()->currencies_list_row($label, $name, $selected_id, $submit_on_change);
}

// ---------------------------------------------------------------------------------------------------
function fiscalyears_list($name, $selected_id = null, $submit_on_change = false)
{
	return ListRenderer::get()->fiscalyears_list($name, $selected_id, $submit_on_change);
}

function _format_fiscalyears($row)
{
	return ListRenderer::get()->_format_fiscalyears($row);
}

function fiscalyears_list_cells($label, $name, $selected_id = null)
{
	return ListRenderer::get()->fiscalyears_list_cells($label, $name, $selected_id);
}

function fiscalyears_list_row($label, $name, $selected_id = null)
{
	return ListRenderer::get()->fiscalyears_list_row($label, $name, $selected_id);
}

// ------------------------------------------------------------------------------------
function dimensions_list($name, $selected_id = null, $no_option = false, $showname = ' ', $submit_on_change = false, $showclosed = false, $showtype = 1)
{
	return ListRenderer::get()->dimensions_list($name, $selected_id, $no_option, $showname, $submit_on_change, $showclosed, $showtype);
}

function dimensions_list_cells($label, $name, $selected_id = null, $no_option = false, $showname = null, $showclosed = false, $showtype = 0, $submit_on_change = false)
{
	return ListRenderer::get()->dimensions_list_cells($label, $name, $selected_id, $no_option, $showname, $showclosed, $showtype, $submit_on_change);
}

function dimensions_list_row($label, $name, $selected_id = null, $no_option = false, $showname = null, $showclosed = false, $showtype = 0, $submit_on_change = false)
{
	return ListRenderer::get()->dimensions_list_row($label, $name, $selected_id, $no_option, $showname, $showclosed, $showtype, $submit_on_change);
}

// ---------------------------------------------------------------------------------------------------
function stock_items_list($name, $selected_id = null, $all_option = false, $submit_on_change = false, $opts = array(), $editkey = false)
{
	return ListRenderer::get()->stock_items_list($name, $selected_id, $all_option, $submit_on_change, $opts, $editkey);
}

function _format_stock_items($row)
{
	return ListRenderer::get()->_format_stock_items($row);
}

function stock_items_list_cells($label, $name, $selected_id = null, $all_option = false, $submit_on_change = false, $all = false, $editkey = false)
{
	return ListRenderer::get()->stock_items_list_cells($label, $name, $selected_id, $all_option, $submit_on_change, $all, $editkey);
}

/*
 * function stock_items_list_row($label, $name, $selected_id=null, $all_option=false, $submit_on_change=false) {
 * ListRenderer::get()->stock_items_list_row($label, $name, $selected_id, $all_option, $submit_on_change); }
 */
// ---------------------------------------------------------------------------------------------------
//
// Select item via foreign code.
//
function sales_items_list($name, $selected_id = null, $all_option = false, $submit_on_change = false, $type = '', $opts = array())
{
	return ListRenderer::get()->sales_items_list($name, $selected_id, $all_option, $submit_on_change, $type, $opts);
}

function sales_items_list_cells($label, $name, $selected_id = null, $all_option = false, $submit_on_change = false, $editkey = false)
{
	return ListRenderer::get()->sales_items_list_cells($label, $name, $selected_id, $all_option, $submit_on_change, $editkey);
}

function sales_kits_list($name, $selected_id = null, $all_option = false, $submit_on_change = false)
{
	return ListRenderer::get()->sales_kits_list($name, $selected_id, $all_option, $submit_on_change);
}

function sales_local_items_list_row($label, $name, $selected_id = null, $all_option = false, $submit_on_change = false)
{
	return ListRenderer::get()->sales_local_items_list_row($label, $name, $selected_id, $all_option, $submit_on_change);
}

// ------------------------------------------------------------------------------------
function stock_manufactured_items_list($name, $selected_id = null, $all_option = false, $submit_on_change = false)
{
	return ListRenderer::get()->stock_manufactured_items_list($name, $selected_id, $all_option, $submit_on_change);
}

function stock_manufactured_items_list_cells($label, $name, $selected_id = null, $all_option = false, $submit_on_change = false)
{
	return ListRenderer::get()->stock_manufactured_items_list_cells($label, $name, $selected_id, $all_option, $submit_on_change);
}

function stock_manufactured_items_list_row($label, $name, $selected_id = null, $all_option = false, $submit_on_change = false)
{
	return ListRenderer::get()->stock_manufactured_items_list_row($label, $name, $selected_id, $all_option, $submit_on_change);
}

// ------------------------------------------------------------------------------------
function stock_component_items_list($name, $parent_stock_id, $selected_id = null, $all_option = false, $submit_on_change = false, $editkey = false)
{
	return ListRenderer::get()->stock_component_items_list($name, $parent_stock_id, $selected_id, $all_option, $submit_on_change, $editkey);
}

function stock_component_items_list_cells($label, $name, $parent_stock_id, $selected_id = null, $all_option = false, $submit_on_change = false, $editkey = false)
{
	return ListRenderer::get()->stock_component_items_list_cells($label, $name, $parent_stock_id, $selected_id, $all_option, $submit_on_change, $editkey);
}

// ------------------------------------------------------------------------------------
function stock_costable_items_list($name, $selected_id = null, $all_option = false, $submit_on_change = false)
{
	return ListRenderer::get()->stock_costable_items_list($name, $selected_id, $all_option, $submit_on_change);
}

function stock_costable_items_list_cells($label, $name, $selected_id = null, $all_option = false, $submit_on_change = false)
{
	return ListRenderer::get()->stock_costable_items_list_cells($label, $name, $selected_id, $all_option, $submit_on_change);
}

// ------------------------------------------------------------------------------------
function stock_purchasable_items_list($name, $selected_id = null, $all_option = false, $submit_on_change = false, $all = false, $editkey = false)
{
	return ListRenderer::get()->stock_purchasable_items_list($name, $selected_id, $all_option, $submit_on_change, $all, $editkey);
}

//
// This helper is used in PO/GRN/PI entry and supports editable descriptions.
//
function stock_purchasable_items_list_cells($label, $name, $selected_id = null, $all_option = false, $submit_on_change = false, $editkey = false)
{
	return ListRenderer::get()->stock_purchasable_items_list_cells($label, $name, $selected_id, $all_option, $submit_on_change, $editkey);
}

/*
 * function stock_purchasable_items_list_row($label, $name, $selected_id=null, $all_option=false,
 * $submit_on_change=false, $editkey=false) { ListRenderer::get()->stock_purchasable_items_list_row($label, $name,
 * $selected_id, $all_option, $submit_on_change, $editkey); }
 */
// ------------------------------------------------------------------------------------
function stock_item_types_list_row($label, $name, $selected_id = null, $enabled = true)
{
	return ListRenderer::get()->stock_item_types_list_row($label, $name, $selected_id, $enabled);
}

function stock_units_list_row($label, $name, $value = null, $enabled = true)
{
	return ListRenderer::get()->stock_units_list_row($label, $name, $value, $enabled);
}

// ------------------------------------------------------------------------------------
function tax_types_list($name, $selected_id = null, $none_option = false, $submit_on_change = false)
{
	return ListRenderer::get()->tax_types_list($name, $selected_id, $none_option, $submit_on_change);
}

function tax_types_list_cells($label, $name, $selected_id = null, $none_option = false, $submit_on_change = false)
{
	return ListRenderer::get()->tax_types_list_cells($label, $name, $selected_id, $none_option, $submit_on_change);
}

function tax_types_list_row($label, $name, $selected_id = null, $none_option = false, $submit_on_change = false)
{
	return ListRenderer::get()->tax_types_list_row($label, $name, $selected_id, $none_option, $submit_on_change);
}

// ------------------------------------------------------------------------------------
function tax_groups_list($name, $selected_id = null, $none_option = false, $submit_on_change = false)
{
	return ListRenderer::get()->tax_groups_list($name, $selected_id, $none_option, $submit_on_change);
}

function tax_groups_list_cells($label, $name, $selected_id = null, $none_option = false, $submit_on_change = false)
{
	return ListRenderer::get()->tax_groups_list_cells($label, $name, $selected_id, $none_option, $submit_on_change);
}

function tax_groups_list_row($label, $name, $selected_id = null, $none_option = false, $submit_on_change = false)
{
	return ListRenderer::get()->tax_groups_list_row($label, $name, $selected_id, $none_option, $submit_on_change);
}

// ------------------------------------------------------------------------------------
function item_tax_types_list($name, $selected_id = null)
{
	return ListRenderer::get()->item_tax_types_list($name, $selected_id);
}

function item_tax_types_list_cells($label, $name, $selected_id = null)
{
	return ListRenderer::get()->item_tax_types_list_cells($label, $name, $selected_id);
}

function item_tax_types_list_row($label, $name, $selected_id = null)
{
	return ListRenderer::get()->item_tax_types_list_row($label, $name, $selected_id);
}

// ------------------------------------------------------------------------------------
function shippers_list($name, $selected_id = null)
{
	return ListRenderer::get()->shippers_list($name, $selected_id);
}

function shippers_list_cells($label, $name, $selected_id = null)
{
	return ListRenderer::get()->shippers_list_cells($label, $name, $selected_id);
}

function shippers_list_row($label, $name, $selected_id = null)
{
	return ListRenderer::get()->shippers_list_row($label, $name, $selected_id);
}

// -------------------------------------------------------------------------------------
function sales_persons_list($name, $selected_id = null, $spec_opt = false)
{
	return ListRenderer::get()->sales_persons_list($name, $selected_id, $spec_opt);
}

function sales_persons_list_cells($label, $name, $selected_id = null, $spec_opt = false)
{
	return ListRenderer::get()->sales_persons_list_cells($label, $name, $selected_id, $spec_opt);
}

function sales_persons_list_row($label, $name, $selected_id = null, $spec_opt = false)
{
	return ListRenderer::get()->sales_persons_list_row($label, $name, $selected_id, $spec_opt);
}

// ------------------------------------------------------------------------------------
function sales_areas_list($name, $selected_id = null)
{
	return ListRenderer::get()->sales_areas_list($name, $selected_id);
}

function sales_areas_list_cells($label, $name, $selected_id = null)
{
	return ListRenderer::get()->sales_areas_list_cells($label, $name, $selected_id);
}

function sales_areas_list_row($label, $name, $selected_id = null)
{
	return ListRenderer::get()->sales_areas_list_row($label, $name, $selected_id);
}

// ------------------------------------------------------------------------------------
function sales_groups_list($name, $selected_id = null, $special_option = false)
{
	return ListRenderer::get()->sales_groups_list($name, $selected_id, $special_option);
}

function sales_groups_list_cells($label, $name, $selected_id = null, $special_option = false)
{
	return ListRenderer::get()->sales_groups_list_cells($label, $name, $selected_id, $special_option);
}

function sales_groups_list_row($label, $name, $selected_id = null, $special_option = false)
{
	return ListRenderer::get()->sales_groups_list_row($label, $name, $selected_id, $special_option);
}

// ------------------------------------------------------------------------------------
function _format_template_items($row)
{
	return ListRenderer::get()->_format_template_items($row);
}

function templates_list($name, $selected_id = null, $special_option = false)
{
	return ListRenderer::get()->templates_list($name, $selected_id, $special_option);
}

function templates_list_cells($label, $name, $selected_id = null, $special_option = false)
{
	return ListRenderer::get()->templates_list_cells($label, $name, $selected_id, $special_option);
}

function templates_list_row($label, $name, $selected_id = null, $special_option = false)
{
	return ListRenderer::get()->templates_list_row($label, $name, $selected_id, $special_option);
}

// ------------------------------------------------------------------------------------
function workorders_list($name, $selected_id = null)
{
	return ListRenderer::get()->workorders_list($name, $selected_id);
}

function workorders_list_cells($label, $name, $selected_id = null)
{
	return ListRenderer::get()->workorders_list_cells($label, $name, $selected_id);
}

function workorders_list_row($label, $name, $selected_id = null)
{
	return ListRenderer::get()->workorders_list_row($label, $name, $selected_id);
}

// ------------------------------------------------------------------------------------
function payment_terms_list($name, $selected_id = null)
{
	return ListRenderer::get()->payment_terms_list($name, $selected_id);
}

function payment_terms_list_cells($label, $name, $selected_id = null)
{
	return ListRenderer::get()->payment_terms_list_cells($label, $name, $selected_id);
}

function payment_terms_list_row($label, $name, $selected_id = null)
{
	return ListRenderer::get()->payment_terms_list_row($label, $name, $selected_id);
}

// ------------------------------------------------------------------------------------
function credit_status_list($name, $selected_id = null)
{
	return ListRenderer::get()->credit_status_list($name, $selected_id);
}

function credit_status_list_cells($label, $name, $selected_id = null)
{
	return ListRenderer::get()->credit_status_list_cells($label, $name, $selected_id);
}

function credit_status_list_row($label, $name, $selected_id = null)
{
	return ListRenderer::get()->credit_status_list_row($label, $name, $selected_id);
}

// -----------------------------------------------------------------------------------------------
function sales_types_list($name, $selected_id = null, $submit_on_change = false, $special_option = false)
{
	return ListRenderer::get()->sales_types_list($name, $selected_id, $submit_on_change, $special_option);
}

function sales_types_list_cells($label, $name, $selected_id = null, $submit_on_change = false, $special_option = false)
{
	return ListRenderer::get()->sales_types_list_cells($label, $name, $selected_id, $submit_on_change, $special_option);
}

function sales_types_list_row($label, $name, $selected_id = null, $submit_on_change = false, $special_option = false)
{
	return ListRenderer::get()->sales_types_list_row($label, $name, $selected_id, $submit_on_change, $special_option);
}

// -----------------------------------------------------------------------------------------------

function _format_date($row)
{
	return ListRenderer::get()->_format_date($row);
}

function bank_reconciliation_list($account, $name, $selected_id = null, $submit_on_change = false, $special_option = false)
{
	return ListRenderer::get()->bank_reconciliation_list($account, $name, $selected_id, $submit_on_change, $special_option);
}

function bank_reconciliation_list_cells($label, $account, $name, $selected_id = null, $submit_on_change = false, $special_option = false)
{
	return ListRenderer::get()->bank_reconciliation_list_cells($label, $account, $name, $selected_id, $submit_on_change, $special_option);
}

/*
function bank_reconciliation_list_row($label, $account, $name, $selected_id=null, $submit_on_change=false, $special_option=false)
{
	echo "<tr>\n";
	bank_reconciliation_list_cells($label, $account, $name, $selected_id, $submit_on_change, $special_option);
	echo "</tr>\n";
}
*/
// -----------------------------------------------------------------------------------------------
function workcenter_list($name, $selected_id = null, $all_option = false)
{
	return ListRenderer::get()->workcenter_list($name, $selected_id, $all_option);
}

function workcenter_list_cells($label, $name, $selected_id = null, $all_option = false)
{
	return ListRenderer::get()->workcenter_list_cells($label, $name, $selected_id, $all_option);
}

function workcenter_list_row($label, $name, $selected_id = null, $all_option = false)
{
	return ListRenderer::get()->workcenter_list_row($label, $name, $selected_id, $all_option);
}

// -----------------------------------------------------------------------------------------------
function bank_accounts_list($name, $selected_id = null, $submit_on_change = false, $spec_option = false)
{
	return ListRenderer::get()->bank_accounts_list($name, $selected_id, $submit_on_change, $spec_option);
}

function bank_accounts_list_cells($label, $name, $selected_id = null, $submit_on_change = false)
{
	return ListRenderer::get()->bank_accounts_list_cells($label, $name, $selected_id, $submit_on_change);
}

function bank_accounts_list_row($label, $name, $selected_id = null, $submit_on_change = false)
{
	return ListRenderer::get()->bank_accounts_list_row($label, $name, $selected_id, $submit_on_change);
}

// -----------------------------------------------------------------------------------------------
function cash_accounts_list_row($label, $name, $selected_id = null, $submit_on_change = false)
{
	return ListRenderer::get()->cash_accounts_list_row($label, $name, $selected_id, $submit_on_change);
}

// -----------------------------------------------------------------------------------------------
function pos_list_row($label, $name, $selected_id = null, $spec_option = false, $submit_on_change = false)
{
	return ListRenderer::get()->pos_list_row($label, $name, $selected_id, $spec_option, $submit_on_change);
}

// -----------------------------------------------------------------------------------------------
// Payment type selector for current user.
//
function sale_payment_list($name, $category, $selected_id = null, $submit_on_change = true)
{
	return ListRenderer::get()->sale_payment_list($name, $category, $selected_id, $submit_on_change);
}

function sale_payment_list_cells($label, $name, $category, $selected_id = null, $submit_on_change = true)
{
	return ListRenderer::get()->sale_payment_list_cells($label, $name, $category, $selected_id, $submit_on_change);
}

// -----------------------------------------------------------------------------------------------
function class_list($name, $selected_id = null, $submit_on_change = false)
{
	return ListRenderer::get()->class_list($name, $selected_id, $submit_on_change);
}

function class_list_cells($label, $name, $selected_id = null, $submit_on_change = false)
{
	return ListRenderer::get()->class_list_cells($label, $name, $selected_id, $submit_on_change);
}

function class_list_row($label, $name, $selected_id = null, $submit_on_change = false)
{
	return ListRenderer::get()->class_list_row($label, $name, $selected_id, $submit_on_change);
}

// -----------------------------------------------------------------------------------------------
function stock_categories_list($name, $selected_id = null, $spec_opt = false, $submit_on_change = false)
{
	return ListRenderer::get()->stock_categories_list($name, $selected_id, $spec_opt, $submit_on_change);
}

function stock_categories_list_cells($label, $name, $selected_id = null, $spec_opt = false, $submit_on_change = false)
{
	return ListRenderer::get()->stock_categories_list_cells($label, $name, $selected_id, $spec_opt, $submit_on_change);
}

function stock_categories_list_row($label, $name, $selected_id = null, $spec_opt = false, $submit_on_change = false)
{
	return ListRenderer::get()->stock_categories_list_row($label, $name, $selected_id, $spec_opt, $submit_on_change);
}

// -----------------------------------------------------------------------------------------------
function gl_account_types_list($name, $selected_id = null, $all_option = false, $all = true)
{
	return ListRenderer::get()->gl_account_types_list($name, $selected_id, $all_option, $all);
}

function gl_account_types_list_cells($label, $name, $selected_id = null, $all_option = false, $all = false)
{
	return ListRenderer::get()->gl_account_types_list_cells($label, $name, $selected_id, $all_option, $all);
}

function gl_account_types_list_row($label, $name, $selected_id = null, $all_option = false, $all = false)
{
	return ListRenderer::get()->gl_account_types_list_row($label, $name, $selected_id, $all_option, $all);
}

// -----------------------------------------------------------------------------------------------
function gl_all_accounts_list($name, $selected_id = null, $skip_bank_accounts = false, $cells = false, $all_option = false, $submit_on_change = false, $all = false)
{
	return ListRenderer::get()->gl_all_accounts_list($name, $selected_id, $skip_bank_accounts, $cells, $all_option, $submit_on_change, $all);
}

function _format_account($row)
{
	return ListRenderer::get()->_format_account($row);
}

function gl_all_accounts_list_cells($label, $name, $selected_id = null, $skip_bank_accounts = false, $cells = false, $all_option = false, $submit_on_change = false, $all = false)
{
	return ListRenderer::get()->gl_all_accounts_list_cells($label, $name, $selected_id, $skip_bank_accounts, $cells, $all_option, $submit_on_change, $all);
}

function gl_all_accounts_list_row($label, $name, $selected_id = null, $skip_bank_accounts = false, $cells = false, $all_option = false)
{
	return ListRenderer::get()->gl_all_accounts_list_row($label, $name, $selected_id, $skip_bank_accounts, $cells, $all_option);
}

function yesno_list($name, $selected_id = null, $name_yes = "", $name_no = "", $submit_on_change = false)
{
	return ListRenderer::get()->yesno_list($name, $selected_id, $name_yes, $name_no, $submit_on_change);
}

function yesno_list_cells($label, $name, $selected_id = null, $name_yes = "", $name_no = "", $submit_on_change = false)
{
	return ListRenderer::get()->yesno_list_cells($label, $name, $selected_id, $name_yes, $name_no, $submit_on_change);
}

function yesno_list_row($label, $name, $selected_id = null, $name_yes = "", $name_no = "", $submit_on_change = false)
{
	return ListRenderer::get()->yesno_list_row($label, $name, $selected_id, $name_yes, $name_no, $submit_on_change);
}

// ------------------------------------------------------------------------------------------------
function languages_list($name, $selected_id = null, $all_option = false)
{
	return ListRenderer::get()->languages_list($name, $selected_id, $all_option);
}

function languages_list_cells($label, $name, $selected_id = null, $all_option = false)
{
	return ListRenderer::get()->languages_list_cells($label, $name, $selected_id, $all_option);
}

function languages_list_row($label, $name, $selected_id = null, $all_option = false)
{
	return ListRenderer::get()->languages_list_row($label, $name, $selected_id, $all_option);
}

// ------------------------------------------------------------------------------------------------
function bank_account_types_list($name, $selected_id = null)
{
	return ListRenderer::get()->bank_account_types_list($name, $selected_id);
}

function bank_account_types_list_cells($label, $name, $selected_id = null)
{
	return ListRenderer::get()->bank_account_types_list_cells($label, $name, $selected_id);
}

function bank_account_types_list_row($label, $name, $selected_id = null)
{
	return ListRenderer::get()->bank_account_types_list_row($label, $name, $selected_id);
}

// ------------------------------------------------------------------------------------------------
function payment_person_types_list($name, $selected_id = null, $submit_on_change = false)
{
	return ListRenderer::get()->payment_person_types_list($name, $selected_id, $submit_on_change);
}

function payment_person_types_list_cells($label, $name, $selected_id = null, $related = null)
{
	return ListRenderer::get()->payment_person_types_list_cells($label, $name, $selected_id, $related);
}

function payment_person_types_list_row($label, $name, $selected_id = null, $related = null)
{
	return ListRenderer::get()->payment_person_types_list_row($label, $name, $selected_id, $related);
}

// ------------------------------------------------------------------------------------------------
function wo_types_list($name, $selected_id = null)
{
	return ListRenderer::get()->wo_types_list($name, $selected_id);
}

function wo_types_list_row($label, $name, $selected_id = null)
{
	return ListRenderer::get()->wo_types_list_row($label, $name, $selected_id);
}

// ------------------------------------------------------------------------------------------------
function dateformats_list_row($label, $name, $value = null)
{
	return ListRenderer::get()->dateformats_list_row($label, $name, $value);
}

function dateseps_list_row($label, $name, $value = null)
{
	return ListRenderer::get()->dateseps_list_row($label, $name, $value);
}

function thoseps_list_row($label, $name, $value = null)
{
	return ListRenderer::get()->thoseps_list_row($label, $name, $value);
}

function decseps_list_row($label, $name, $value = null)
{
	return ListRenderer::get()->decseps_list_row($label, $name, $value);
}

function themes_list_row($label, $name, $value = null)
{
	return ListRenderer::get()->themes_list_row($label, $name, $value);
}

function pagesizes_list_row($label, $name, $value = null)
{
	return ListRenderer::get()->pagesizes_list_row($label, $name, $value);
}

function systypes_list($name, $value = null, $spec_opt = false, $submit_on_change = false, $exclude = array())
{
	return ListRenderer::get()->systypes_list($name, $value, $spec_opt, $submit_on_change, $exclude);
}

function systypes_list_cells($label, $name, $value = null, $submit_on_change = false, $exclude = array())
{
	return ListRenderer::get()->systypes_list_cells($label, $name, $value, $submit_on_change, $exclude);
}

function systypes_list_row($label, $name, $value = null, $submit_on_change = false, $exclude = array())
{
	return ListRenderer::get()->systypes_list_row($label, $name, $value, $submit_on_change, $exclude);
}

function journal_types_list_cells($label, $name, $value = null, $submit_on_change = false)
{
	return ListRenderer::get()->journal_types_list_cells($label, $name, $value, $submit_on_change);
}

function cust_allocations_list_cells($label, $name, $selected = null)
{
	return ListRenderer::get()->cust_allocations_list_cells($label, $name, $selected);
}

function supp_allocations_list_cell($name, $selected = null)
{
	return ListRenderer::get()->supp_allocations_list_cell($name, $selected);
}

function supp_transactions_list_cell($name, $selected = null)
{
	return ListRenderer::get()->supp_transactions_list_cell($name, $selected);
}

function policy_list_cells($label, $name, $selected = null)
{
	return ListRenderer::get()->policy_list_cells($label, $name, $selected);
}

function policy_list_row($label, $name, $selected = null)
{
	return ListRenderer::get()->policy_list_row($label, $name, $selected);
}

function credit_type_list_cells($label, $name, $selected = null, $submit_on_change = false)
{
	return ListRenderer::get()->credit_type_list_cells($label, $name, $selected, $submit_on_change);
}

function credit_type_list_row($label, $name, $selected = null, $submit_on_change = false)
{
	return ListRenderer::get()->credit_type_list_row($label, $name, $selected, $submit_on_change);
}

function number_list($name, $selected, $from, $to, $no_option = false)
{
	return ListRenderer::get()->number_list($name, $selected, $from, $to, $no_option);
}

function number_list_cells($label, $name, $selected, $from, $to, $no_option = false)
{
	return ListRenderer::get()->number_list_cells($label, $name, $selected, $from, $to, $no_option);
}

function number_list_row($label, $name, $selected, $from, $to, $no_option = false)
{
	return ListRenderer::get()->number_list_row($label, $name, $selected, $from, $to, $no_option);
}

function print_profiles_list_row($label, $name, $selected_id = null, $spec_opt = false, $submit_on_change = true)
{
	return ListRenderer::get()->print_profiles_list_row($label, $name, $selected_id, $spec_opt, $submit_on_change);
}

function printers_list($name, $selected_id = null, $spec_opt = false, $submit_on_change = false)
{
	return ListRenderer::get()->printers_list($name, $selected_id, $spec_opt, $submit_on_change);
}

// ------------------------------------------------------------------------------------------------
function quick_entries_list($name, $selected_id = null, $type = null, $submit_on_change = false)
{
	return ListRenderer::get()->quick_entries_list($name, $selected_id, $type, $submit_on_change);
}

function quick_entries_list_cells($label, $name, $selected_id = null, $type, $submit_on_change = false)
{
	return ListRenderer::get()->quick_entries_list_cells($label, $name, $selected_id, $type, $submit_on_change);
}

function quick_entries_list_row($label, $name, $selected_id = null, $type, $submit_on_change = false)
{
	return ListRenderer::get()->quick_entries_list_row($label, $name, $selected_id, $type, $submit_on_change);
}

function quick_actions_list_row($label, $name, $selected_id = null, $submit_on_change = false)
{
	return ListRenderer::get()->quick_actions_list_row($label, $name, $selected_id, $submit_on_change);
}

function quick_entry_types_list_row($label, $name, $selected_id = null, $submit_on_change = false)
{
	return ListRenderer::get()->quick_entry_types_list_row($label, $name, $selected_id, $submit_on_change);
}

function record_status_list_row($label, $name)
{
	return ListRenderer::get()->record_status_list_row($label, $name);
}

function class_types_list_row($label, $name, $selected_id = null, $submit_on_change = false)
{
	return ListRenderer::get()->class_types_list_row($label, $name, $selected_id, $submit_on_change);
}

// ------------------------------------------------------------------------------------------------
function security_roles_list($name, $selected_id = null, $new_item = false, $submit_on_change = false, $show_inactive = false)
{
	return ListRenderer::get()->security_roles_list($name, $selected_id, $new_item, $submit_on_change, $show_inactive);
}

function security_roles_list_cells($label, $name, $selected_id = null, $new_item = false, $submit_on_change = false, $show_inactive = false)
{
	return ListRenderer::get()->security_roles_list_cells($label, $name, $selected_id, $new_item, $submit_on_change, $show_inactive);
}

function security_roles_list_row($label, $name, $selected_id = null, $new_item = false, $submit_on_change = false, $show_inactive = false)
{
	return ListRenderer::get()->security_roles_list_row($label, $name, $selected_id, $new_item, $submit_on_change, $show_inactive);
}

function tab_list_row($label, $name, $selected_id = null)
{
	return ListRenderer::get()->tab_list_row($label, $name, $selected_id);
}

// -----------------------------------------------------------------------------------------------
function tag_list($name, $height, $type, $multi = false, $all = false, $spec_opt = false)
{
	return ListRenderer::get()->tag_list($name, $height, $type, $multi, $all, $spec_opt);
}

function tag_list_cells($label, $name, $height, $type, $mult = false, $all = false, $spec_opt = false)
{
	return ListRenderer::get()->tag_list_cells($label, $name, $height, $type, $mult, $all, $spec_opt);
}

function tag_list_row($label, $name, $height, $type, $mult = false, $all = false, $spec_opt = false)
{
	return ListRenderer::get()->tag_list_row($label, $name, $height, $type, $mult, $all, $spec_opt);
}

// ---------------------------------------------------------------------------------------------
// List of sets of active extensions
//
function extset_list($name, $value = null, $submit_on_change = false)
{
	return ListRenderer::get()->extset_list($name, $value, $submit_on_change);
}

function crm_category_types_list($name, $selected_id = null, $filter = array(), $submit_on_change = true)
{
	return ListRenderer::get()->crm_category_types_list($name, $selected_id, $filter, $submit_on_change);
}

function crm_category_types_list_row($label, $name, $selected_id = null, $filter = array(), $submit_on_change = true)
{
	return ListRenderer::get()->crm_category_types_list_row($label, $name, $selected_id, $filter, $submit_on_change);
}

function payment_type_list_row($label, $name, $selected_id = null, $submit_on_change = false)
{
	return ListRenderer::get()->payment_type_list_row($label, $name, $selected_id, $submit_on_change);
}

function coa_list_row($label, $name, $value = null)
{
	return ListRenderer::get()->coa_list_row($label, $name, $value);
}

function payment_services($name)
{
	return ListRenderer::get()->payment_services($name);
}

function tax_algorithm_list($name, $value=null, $submit_on_change = false)
{
	return ListRenderer::get()->tax_algorithm_list($name, $value, $submit_on_change);
}

function tax_algorithm_list_cells($label, $name, $value=null, $submit_on_change=false)
{
	ListRenderer::get()->tax_algorithm_list_cells($label, $name, $value, $submit_on_change);
}

function tax_algorithm_list_row($label, $name, $value=null, $submit_on_change=false)
{
	ListRenderer::get()->tax_algorithm_list_row($label, $name, $value, $submit_on_change);
}

function refline_list($name, $type, $value=null, $spec_option=false)
{
	return ListRenderer::get()->refline_list($name, $type, $value, $spec_option);
}

function refline_list_row($label, $name, $type, $selected_id=null, $spec_option=false)
{
	ListRenderer::get()->refline_list_row($label, $name, $type, $selected_id, $spec_option);
}

//----------------------------------------------------------------------------------------------

function subledger_list($name, $account, $selected_id=null)
{
	return ListRenderer::get()->subledger_list($name, $account, $selected_id);
}

function subledger_list_cells($label, $name, $account, $selected_id=null)
{
	ListRenderer::get()->subledger_list_cells($label, $name, $account, $selected_id);
}

function subledger_list_row($label, $name, $selected_id=null, $all_option = false,
	$submit_on_change=false, $show_inactive=false, $editkey = false)
{
	ListRenderer::get()->subledger_list_row($label, $name, $selected_id, $all_option, $submit_on_change, $show_inactive, $editkey);
}

function accounts_type_list_row($label, $name, $selected_id=null)
{
	ListRenderer::get()->accounts_type_list_row($label, $name, $selected_id);
}

function users_list_cells($label, $name, $selected_id=null, $submit_on_change=false, $spec_opt=true)
{
	ListRenderer::get()->users_list_cells($label, $name, $selected_id, $submit_on_change, $spec_opt);
}

function collations_list_row($label, $name, $selected_id=null)
{
	ListRenderer::get()->collations_list_row($label, $name, $selected_id);
}

?>
