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

if (isset($SysPrefs->max_rows_in_search))
	$limit = $SysPrefs->max_rows_in_search;
else
	$limit = 10;

page(_($help_context = "Customer Branches"), true, false, "", $js);

// Activate Ajax on form submit
if(get_post("search")) {
  $Ajax->activate("customer_branch_tbl");
}

// BEGIN: Filter form. Use query string so the client_id will not disappear
// after ajax form post.
start_form(false, false, $_SERVER['PHP_SELF'] . "?" . $_SERVER['QUERY_STRING']);

start_table(TABLESTYLE_NOBORDER);

start_row();

text_cells(_("Branch"), "branch");
submit_cells("search", _("Search"), "", _("Search branches"), "default");

end_row();

end_table();

end_form();
// END: Filter form

// BEGIN: Link to add new customer branch
// hyperlink_params($path_to_root . "/sales/manage/customer_branches.php", _("Add new"), "debtor_no=" . strip_tags($_GET["SelectedBranch"]) . "&popup=1");
// END: Link to add new customer branch

// BEGIN: Customer branches list
div_start("customer_branch_tbl");

start_table(TABLESTYLE);

$th = array("", _("Ref"), _("Branch"), _("Contact"), _("Phone"));

table_header($th);

// Query based on function get_sql_for_customer_branches in includes/db/branches_db.inc.
$sql = "SELECT 
    b.branch_code,
    b.branch_ref,
    b.br_name,
    p.name as contact_name,
    p.phone
  FROM ".TB_PREF."cust_branch b
  LEFT JOIN ".TB_PREF."crm_contacts c
    ON c.entity_id=b.branch_code AND c.type='cust_branch' AND c.action='general'
  LEFT JOIN ".TB_PREF."crm_persons p
    on c.person_id=p.id
  WHERE b.debtor_no = ".db_escape($_GET["customer_id"])."
    AND b.br_name LIKE " . db_escape("%" . get_post("branch"). "%") . "
  ORDER BY b.br_name LIMIT 0, $limit"; // We only display 10 items.

$result = db_query($sql, "Failed in retreiving branches list.");

$k = 0; //row colour counter
$name = $_GET["client_id"];
while ($myrow = db_fetch_assoc($result))
{
  	alt_table_row_color($k);
	$value = $myrow['branch_code'];
	ahref_cell(_("Select"), 'javascript:void(0)', '', 'selectComboItem(window.opener.document, "'.$name.'", "'.$value.'")');
  	label_cell($myrow["branch_ref"]);
  	label_cell($myrow["br_name"]);
  	label_cell($myrow["contact_name"]);
  	label_cell($myrow["phone"]);
	end_row();
}

end_table(1);

div_end();
// END: Customer list

end_page(true);
