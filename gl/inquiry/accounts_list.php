<?php
/**********************************************************************
  Page for searching GL account list and select it to GL account
  selection in page that has GL account dropdown list.
***********************************************************************/
$page_security = "SA_GLACCOUNT";
$path_to_root = "../..";

include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/ui.inc");

$js = get_js_select_combo_item();

page(_($help_context = "GL Accounts"), @$_REQUEST["popup"], false, "", $js);

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
			chart.account_name LIKE " . db_escape("%" . get_post("description"). "%") . "
			OR
			chart.account_code = " . db_escape(get_post("description")) . "
		) 
	ORDER BY chart.account_code LIMIT 0, 10"; // We only display 10 items.
$result = db_query($sql, "Failed in retreiving GL account list.");

$k = 0; //row colour counter

while ($myrow = db_fetch_assoc($result)) {
	alt_table_row_color($k);
	ahref_cell(_("Select"), 'javascript:void(0)', '', 'selectComboItem(window.opener.document, &quot;' . $_GET["client_id"] . '&quot;, &quot;' . $myrow["account_code"] . '&quot;)');
	label_cell($myrow["account_code"]);
	label_cell($myrow["account_name"]);
	label_cell($myrow["name"]);
	end_row();
}

end_table(1);

div_end();
// END: Account list

end_page();
