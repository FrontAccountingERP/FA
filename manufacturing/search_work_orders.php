<?php

$page_security = 2;
$path_to_root="..";
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/manufacturing/includes/manufacturing_ui.inc");
$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(800, 500);
if (isset($_GET['outstanding_only'])) 
{
	$outstanding_only = 1;
	page(_("Search Outstanding Work Orders"), false, false, "", $js);
} 
else 
{
	$outstanding_only = 0;
	page(_("Search Work Orders"), false, false, "", $js);
}


//--------------------------------------------------------------------------------------

if (isset($_GET["stock_id"]))
	$_POST['SelectedStockItem'] = $_GET["stock_id"];

//--------------------------------------------------------------------------------------

start_form(false, false, $_SERVER['PHP_SELF'] ."?outstanding_only=" . $outstanding_only .SID);

start_table("class='tablestyle_noborder'");
start_row();
ref_cells(_("Reference:"), 'OrderNumber');

locations_list_cells(_("at Location:"), 'StockLocation', null, true);

check_cells( _("Only Overdue:"), 'OverdueOnly', null);

check_cells( _("Only Open:"), 'OpenOnly', null);

stock_manufactured_items_list_cells(_("for item:"), 'SelectedStockItem', null, true);

submit_cells('SearchOrders', _("Search"));
end_row();
end_table();

end_form();

$sql = "SELECT ".TB_PREF."workorders.*, ".TB_PREF."stock_master.description,".TB_PREF."locations.location_name
	FROM ".TB_PREF."workorders,".TB_PREF."stock_master,".TB_PREF."locations
	WHERE ".TB_PREF."stock_master.stock_id=".TB_PREF."workorders.stock_id AND
	".TB_PREF."locations.loc_code=".TB_PREF."workorders.loc_code ";

if (check_value('OpenOnly')) 
{
	$sql .= " AND ".TB_PREF."workorders.closed=0 ";
}

if (isset($_POST['StockLocation']) && $_POST['StockLocation'] != $all_items) 
{
	$sql .= "AND ".TB_PREF."workorders.loc_code='" . $_POST['StockLocation'] . "' ";
}

if (isset($_POST['OrderNumber']) && $_POST['OrderNumber'] != "") 
{
	$sql .= "AND ".TB_PREF."workorders.wo_ref LIKE '%". $_POST['OrderNumber'] . "%'";
}

if (isset($_POST['SelectedStockItem']) && $_POST['SelectedStockItem'] != $all_items) 
{
	$sql .= "AND ".TB_PREF."workorders.stock_id='". $_POST['SelectedStockItem'] . "'";
}

if (check_value('OverdueOnly')) 
{
	$Today = date2sql(Today());

	$sql .= "AND ".TB_PREF."workorders.required_by < '$Today' ";
}
$sql .= " ORDER BY ".TB_PREF."workorders.required_by";

$result = db_query($sql,"No orders were returned");

start_table("$table_style width=80%");

$th = array(_("#"), _("Reference"), _("Type"), _("Location"), _("Item"),
	_("Required"), _("Manufactured"), _("Date"), _("Required By"), _("Closed"), "");
table_header($th);

$j = 1;
$k = 0;

while ($myrow = db_fetch($result)) 
{


	// check if it's an overdue work order
	if (!$myrow["closed"] && date_diff(Today(), sql2date($myrow["required_by"]), "d") > 0) 
	{
		start_row("class='overduebg'");
	}
	else
		alt_table_row_color($k);

	$modify_page = $path_to_root . "/manufacturing/work_order_entry.php?" . SID . "trans_no=" . $myrow["id"];
	$release_page = $path_to_root . "/manufacturing/work_order_release.php?" . SID . "trans_no=" . $myrow["id"];
	if ($myrow["closed"] == 0) 
	{
		$issue = $path_to_root . "/manufacturing/work_order_issue.php?" . SID . "trans_no=" .$myrow["id"];
		$add_finished = $path_to_root . "/manufacturing/work_order_add_finished.php?" . SID . "trans_no=" .$myrow["id"];
		$costs = $path_to_root . "/gl/gl_payment.php?NewPayment=1&PayType=" . payment_person_types::WorkOrder(). "&PayPerson=" .$myrow["id"];
		$can_issue = $myrow["released"];
		$issue_link = $can_issue?("<a href=$issue>" . _("Issue") . "</a></td>
			<td><a href=$add_finished>" . _("Produce") . "</a></td>
			<td><a href=$costs>" . _("Costs") . "</a>"): _("Not Released");
	} 
	else 
	{
		$issue_link = "";
	}

	label_cell(get_trans_view_str(systypes::work_order(), $myrow["id"]));
	label_cell(get_trans_view_str(systypes::work_order(), $myrow["id"], $myrow["wo_ref"]));
	label_cell(wo_types::name($myrow["type"]));
	label_cell($myrow["location_name"]);
	view_stock_status_cell($myrow["stock_id"], $myrow["description"]);
	qty_cell($myrow["units_reqd"]);
	qty_cell($myrow["units_issued"]);
	label_cell(sql2date($myrow["date_"]));
	label_cell(sql2date($myrow["required_by"]));
	label_cell(($myrow["closed"]? _("Yes"):_("No")));
	if ($issue_link != "")
		label_cell($issue_link);
	if ($myrow["released"] == 0) 
	{
		label_cell("<a href=$release_page>" . _("Release") . "</a>");
	}
	if ($myrow["closed"] == 0) 
	{
		label_cell("<a href=$modify_page>" . _("Edit") . "</a>");
	}

	label_cell(get_gl_view_str(systypes::work_order(), $myrow["id"]));

	end_row();

	$j++;
	If ($j == 12)
	{
		$j = 1;
		table_header($th);
	}
	//end of page full new headings if
}
//end of while loop

end_table(1);

//---------------------------------------------------------------------------------

end_page();

?>
