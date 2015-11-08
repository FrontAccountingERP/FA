<?php
/**********************************************************************
  Page for searching item list and select it to item selection
  in sales order and purchase order.
***********************************************************************/
$page_security = "SA_PURCHASEORDER";
$path_to_root = "../..";
include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/ui.inc");

$js = get_js_select_combo_item();

page(_($help_context = "Suppliers"), @$_REQUEST["popup"], false, "", $js);

// Activate Ajax on form submit
if(get_post("search")) {
  $Ajax->activate("supplier_tbl");
}

// BEGIN: Filter form. Use query string so the client_id will not disappear
// after ajax form post.
start_form(false, false, $_SERVER['PHP_SELF'] . "?" . $_SERVER['QUERY_STRING']);

start_table(TABLESTYLE_NOBORDER);

start_row();

text_cells(_("Supplier"), "supplier");
submit_cells("search", _("Search"), "", _("Search suppliers"), "default");

end_row();

end_table();

end_form();
// END: Filter form

// BEGIN: Link to add new supplier
// hyperlink_params($path_to_root . "/purchasing/manage/suppliers.php", _("Add new"), "popup=1");
// END: Link to add new supplier

// BEGIN: Supplier list
div_start("supplier_tbl");

start_table(TABLESTYLE);

$th = array("", _("Supplier"), _("Address"));

table_header($th);

// Query based on function supplier_list in includes/ui/ui_lists.inc.
$sql = "SELECT supplier_id, supp_name, address FROM ".TB_PREF."suppliers
  WHERE supp_name LIKE " . db_escape("%" . get_post("supplier"). "%") . "
  ORDER BY supp_name LIMIT 0, 10"; // We only display 10 items.
$result = db_query($sql, "Failed in retreiving supplier list.");

$k = 0; //row colour counter

while ($myrow = db_fetch_assoc($result)) {
	alt_table_row_color($k);
  ahref_cell(_("Select"), 'javascript:void(0)', '', 'selectComboItem(window.opener.document, &quot;' . $_GET["client_id"] . '&quot;, &quot;' . $myrow["supplier_id"] . '&quot;)');
  label_cell($myrow["supp_name"]);
  label_cell($myrow["address"]);
	end_row();
}

end_table(1);

div_end();
// END: Supplier list

end_page();
