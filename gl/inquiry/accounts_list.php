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
/**********************************************************************
  Page for searching GL account list and select it to GL account
  selection in pages that have GL account dropdown lists.
  Author: bogeyman2007 from Discussion Forum. Modified by Joe Hunt
***********************************************************************/
$page_security = "SA_GLACCOUNT";
$path_to_root = "../..";
include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/ui.inc");

$js = get_js_select_combo_item();

page(_($help_context = "GL Accounts"), true, false, "", $js);

if (isset($SysPrefs->max_rows_in_search))
	$limit = $SysPrefs->max_rows_in_search;
else
	$limit = 10;

// Activate Ajax on form submit
if(get_post("search")) {
  	$Ajax->activate("account_tbl");
}

// BEGIN: Filter form. Use query string so the client_id will not disappear
// after ajax form post.
start_form(false, false, $_SERVER['PHP_SELF'] . "?" . $_SERVER['QUERY_STRING']);

start_table(TABLESTYLE_NOBORDER);

start_row();

text_cells(_("Description"), "description");
submit_cells("search", _("Search"), "", _("Search GL accounts"), "default");

end_row();

end_table();

end_form();
// END: Filter form

// BEGIN: Account list
div_start("account_tbl");

start_table(TABLESTYLE);

$th = array("", _("Account Code"), _("Description"), _("Category"));

table_header($th);

// Query based on function gl_all_accounts_list in includes/ui/ui_lists.inc.
$sql = "SELECT chart.account_code, chart.account_name, type.name
			FROM ".TB_PREF."chart_master chart,".TB_PREF."chart_types type
			WHERE chart.account_type=type.id
        AND (
          chart.account_name LIKE " . db_escape("%" . get_post("description"). "%") . " OR
          chart.account_code LIKE " . db_escape("%" . get_post("description"). "%") . "
        ) 
      	ORDER BY chart.account_code LIMIT 0, $limit"; // We only display 10 items.
$result = db_query($sql, "Failed in retreiving GL account list.");

$k = 0; //row colour counter
$name = $_GET["client_id"];
while ($myrow = db_fetch_assoc($result)) {
	alt_table_row_color($k);
	$value = $myrow['account_code'];
	ahref_cell(_("Select"), 'javascript:void(0)', '', 'selectComboItem(window.opener.document, "'.$name.'", "'.$value.'")');
  	label_cell($myrow["account_code"]);
	label_cell($myrow["account_name"]);
  	label_cell($myrow["name"]);
	end_row();
}

end_table(1);

div_end();
// END: Account list

end_page(true);