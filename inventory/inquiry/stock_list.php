<?php
/**********************************************************************
  Page for searching item list and select it to item selection
  in sales order and purchase order.
***********************************************************************/
$page_security = "SA_ITEM";
$path_to_root = "../..";
include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/ui.inc");

$mode = get_company_pref('no_item_list');
if ($mode != 0)
	$js = get_js_set_combo_item();
else
	$js = get_js_select_combo_item();

page(_($help_context = "Items"), true, false, "", $js);

if (isset($SysPrefs->max_rows_in_search))
	$limit = $SysPrefs->max_rows_in_search;
else
	$limit = 10;

// Activate Ajax on form submit
if(get_post("search")) {
  $Ajax->activate("item_tbl");
}

// BEGIN: Filter form. Use query string so the client_id will not disappear
// after ajax form post.
start_form(false, false, $_SERVER['PHP_SELF'] . "?" . $_SERVER['QUERY_STRING']);

start_table(TABLESTYLE_NOBORDER);

start_row();

text_cells(_("Description"), "description");
submit_cells("search", _("Search"), "", _("Search items"), "default");

end_row();

end_table();

end_form();
// END: Filter form

// BEGIN: Link to add new item
// hyperlink_params($path_to_root . "/inventory/manage/items.php", _("Add new"), "popup=1");
// END: Link to add new item

// BEGIN: Item list
div_start("item_tbl");

start_table(TABLESTYLE);

$th = array("", _("Item Code"), _("Description"), _("Category"));

table_header($th);

// Query based on function sales_items_list in includes/ui/ui_lists.inc.
$sql = "SELECT COUNT(i.item_code) AS kit, i.item_code, i.description, c.description category
  FROM ".TB_PREF."stock_master s, ".TB_PREF."item_codes i
  LEFT JOIN ".TB_PREF."stock_category c
    ON i.category_id=c.category_id
  WHERE i.stock_id=s.stock_id
    AND !i.inactive AND !s.inactive
    AND (  i.item_code LIKE " . db_escape("%" . get_post("description"). "%") . " OR 
         i.description LIKE " . db_escape("%" . get_post("description"). "%") . " OR 
         c.description LIKE " . db_escape("%" . get_post("description"). "%") . ") ";

$type = "";
if (isset($_GET['type'])) {
  $type = $_GET['type'];
}

switch ($type) {
  case "sales":
    $sql .= " AND !s.no_sale AND mb_flag != 'F'";
    break;
  case "manufactured":
    $sql .= " AND mb_flag = 'M'";
    break;
  case "purchasable":
    $sql .= " AND NOT no_purchase AND mb_flag != 'F' AND i.item_code=i.stock_id";
    break;
  case "costable":
    $sql .= " AND mb_flag != 'D' AND mb_flag != 'F' AND  i.item_code=i.stock_id";
    break;
  case "component":
  	$parent = $_GET['parent'];
    $sql .= " AND  i.item_code=i.stock_id AND i.stock_id <> '$parent' AND mb_flag != 'F' ";
    break;
  case "assets":
    $sql .= " AND mb_flag = 'F'";
    break;
  case "kits":
  	$sql .= " AND !i.is_foreign AND i.item_code!=i.stock_id AND mb_flag != 'F'";
  	break;
  case "all":
    $sql .= " AND mb_flag != 'F' AND i.item_code=i.stock_id";
    // NOTHING TO DO.
    break;
}

$sql .= " GROUP BY i.item_code ORDER BY i.description LIMIT 0, $limit"; // We only display 10 items.

$result = db_query($sql, "Failed in retreiving item list.");

$k = 0; //row colour counter
$name = $_GET["client_id"];
while ($myrow = db_fetch_assoc($result)) {
	alt_table_row_color($k);
	$value = $myrow['item_code'];
	if ($mode != 0) {
		$text = $myrow['description'];
  		ahref_cell(_("Select"), 'javascript:void(0)', '', 'setComboItem(window.opener.document, "'.$name.'",  "'.$value.'", "'.$text.'")');
	}
	else {
  		ahref_cell(_("Select"), 'javascript:void(0)', '', 'selectComboItem(window.opener.document, "'.$name.'", "'.$value.'")');
	}
  	label_cell($myrow["item_code"]);
	label_cell($myrow["description"]);
  	label_cell($myrow["category"]);
	end_row();
}

end_table(1);

div_end();
// END: Item list

end_page(true);
