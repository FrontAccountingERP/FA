<?php
/**********************************************************************
  Page for searching item list and select it to item selection
  in sales order and purchase order.
***********************************************************************/
$page_security = "SA_SALESORDER";
$path_to_root = "../..";

include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/ui.inc");

$js = get_js_select_combo_item();
$js .= get_js_populate_combo_item();

page(_($help_context = "Customers"), @$_REQUEST["popup"], false, "", $js);

// Activate Ajax on form submit
if(get_post("search")) {
	$Ajax->activate("customer_tbl");
}

// BEGIN: Filter form. Use query string so the client_id will not disappear
// after ajax form post.
start_form(false, false, $_SERVER['PHP_SELF'] . "?" . $_SERVER['QUERY_STRING']);

start_table(TABLESTYLE_NOBORDER);

start_row();

text_cells(_("Customer"), "customer");
submit_cells("search", _("Search"), "", _("Search customers"), "default");

end_row();

end_table();

end_form();
// END: Filter form

// BEGIN: Link to add new customer
// hyperlink_params($path_to_root . "/sales/manage/customers.php", _("Add new"), "popup=1");
// END: Link to add new customer

// BEGIN: Customer list
div_start("customer_tbl");

start_table(TABLESTYLE);

$th = array("", _("Customer"), _("Address"));

table_header($th);

// Query based on function customer_list in includes/ui/ui_lists.inc.
$sql = "SELECT debtor_no, name, address FROM ".TB_PREF."debtors_master 
	WHERE name LIKE " . db_escape("%" . get_post("customer"). "%") . "
	ORDER BY name LIMIT 0, 10"; // We only display 10 items.
$result = db_query($sql, "Failed in retreiving customer list.");

// Check if we should append selected option to dropdown.
// Usefull when the search customer is enabled from company setup.
$append = "false";
if (get_company_pref("no_customer_list") === "1") {
	$append = "true";
}

$k = 0; //row colour counter

while ($myrow = db_fetch_assoc($result)) {
	alt_table_row_color($k);
	ahref_cell(_("Select"), 'javascript:void(0)', '', 'selectComboItem(window.opener.document, &quot;' . $_GET["client_id"] . '&quot;, &quot;' . $myrow["debtor_no"] . '&quot;, &quot;' . $myrow["name"] . '&quot;, ' . $append . ')');
	label_cell($myrow["name"]);
	label_cell($myrow["address"]);
	end_row();
}

end_table(1);

div_end();
// END: Customer list

end_page();
