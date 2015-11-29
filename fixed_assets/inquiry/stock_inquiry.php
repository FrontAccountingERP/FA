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
$page_security = 'SA_ASSETSANALYTIC';
$path_to_root = "../..";
include($path_to_root . "/includes/db_pager.inc");
include($path_to_root . "/includes/session.inc");

include($path_to_root . "/purchasing/includes/purchasing_ui.inc");
include($path_to_root . "/reporting/includes/reporting.inc");
include($path_to_root . "/fixed_assets/includes/fixed_assets_db.inc");

$js = "";
if ($SysPrefs->use_popup_windows)
	$js .= get_js_open_window(900, 500);
if (user_use_date_picker())
	$js .= get_js_date_picker();
page(_($help_context = "FA Item Inquiry"), false, false, "", $js);

if (isset($_GET['location'])) 
{
	$_POST['location'] = $_GET['location'];
}

//------------------------------------------------------------------------------------------------

start_form();

start_table(TABLESTYLE_NOBORDER);
start_row();
//locations_list_cells(_("From Location:"), 'location', null, false, false, true);
check_cells( _("Show inactive:"), 'show_inactive', null);
submit_cells('RefreshInquiry', _("Search"),'',_('Refresh Inquiry'), 'default');
end_row();

end_table();

//------------------------------------------------------------------------------------------------

if(get_post('RefreshInquiry'))
{
	$Ajax->activate('totals_tbl');
}

function gl_view($row)
{
  $row = get_fixed_asset_move($row['stock_id'], ST_JOURNAL);

  //if ($row === false)
    //return "";

  //return get_journal_trans_view_str(ST_JOURNAL, $row["trans_no"], sql2date($row["tran_date"]));
	return get_gl_view_str(ST_JOURNAL, $row["trans_no"]);
}

function fa_link($row)
{
  $url = "inventory/manage/items.php?FixedAsset=1&stock_id=".$row['stock_id'];

	return viewer_link($row['stock_id'], $url);
}

function depr_method_title($row) {
  global $depreciation_methods;
  return $depreciation_methods[$row['depreciation_method']];
}

function depr_rate_title($row) {
  if ($row['depreciation_method'] == 'S')
    return $row['depreciation_rate'].' years';
  else
    return $row['depreciation_rate'].'%';
}

function status_title($row) {

	if ($row['inactive'] || ($row['disposal_date'] !== NULL))
		return _("Disposed"); // disposed or saled
	elseif ($row['purchase_date'] === NULL)
		return _("Purchasable"); // not yet purchased
    else
    	return _("Active");  // purchased

}

function purchase_link($row)
{

  if ($row['purchase_date'] === null)
    return "";

  return get_supplier_trans_view_str(ST_SUPPRECEIVE, $row["purchase_no"], sql2date($row["purchase_date"]));
}

function disposal_link($row)
{
  switch ($row['disposal_type']) {
    case ST_INVADJUST:
      return get_inventory_trans_view_str(ST_INVADJUST, $row["disposal_no"], sql2date($row["disposal_date"]));
    case ST_CUSTDELIVERY:
	    return get_customer_trans_view_str(ST_CUSTDELIVERY, $row["disposal_no"], sql2date($row["disposal_date"]));
    default:
      return "";
  }
}

//------------------------------------------------------------------------------------------------

$sql = get_sql_for_fixed_assets(get_post('show_inactive'));

$cols = array(
			//_("Type") => array('fun'=>'systype_name', 'ord'=>''), 
			//_("#") => array('fun'=>'trans_view', 'ord'=>''), 
			_("Item") => array('fun' => 'fa_link'), 
			_("FA Class"), 
			_("Units of Measure") => array('align' => 'center'), 
			_("Long description"),
			_("Depreciation Rate or Lifecycle") => array('fun' => 'depr_rate_title'), 
			_("Depreciation Method") => array('fun' => 'depr_method_title'), 
			_("Status") => array('fun' => 'status_title'), 
			_("Purchase") => array('fun' => 'purchase_link'), 
			_("Liquidation or Sale") => array('align' => 'center', 'fun' => 'disposal_link'), 
			//array('insert'=>true, 'fun'=>'gl_view'),
			//array('insert'=>true, 'fun'=>'rm_link'),
			//array('insert'=>true, 'fun'=>'edit_link'),
			//array('insert'=>true, 'fun'=>'prt_link'),
			);

//------------------------------------------------------------------------------------------------

/*show a table of the transactions returned by the sql */
$table =& new_db_pager('fixed_assets_tbl', $sql, $cols);

$table->width = "85%";

display_db_pager($table);

end_form();
end_page();
