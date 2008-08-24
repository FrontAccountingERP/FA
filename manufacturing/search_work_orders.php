<?php

$page_security = 2;
$path_to_root="..";
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/manufacturing/includes/manufacturing_ui.inc");
$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(800, 500);
if (isset($_GET['outstanding_only']) && ($_GET['outstanding_only'] == true))
{
// curently outstanding simply means not closed
	$outstanding_only = 1;
	page(_("Search Outstanding Work Orders"), false, false, "", $js);
}
else
{
	$outstanding_only = 0;
	page(_("Search Work Orders"), false, false, "", $js);
}
//-----------------------------------------------------------------------------------
// Ajax updates
//
if (get_post('SearchOrders')) 
{
	$Ajax->activate('orders_tbl');
} elseif (get_post('_OrderNumber_changed')) 
{
	$disable = get_post('OrderNumber') !== '';

	$Ajax->addDisable(true, 'StockLocation', $disable);
	$Ajax->addDisable(true, 'OverdueOnly', $disable);
	$Ajax->addDisable(true, 'OpenOnly', $disable);
	$Ajax->addDisable(true, 'SelectedStockItem', $disable);

	if ($disable) {
//		$Ajax->addFocus(true, 'OrderNumber');
		set_focus('OrderNumber');
	} else
//		$Ajax->addFocus(true, 'StockLocation');
		set_focus('StockLocation');

	$Ajax->activate('orders_tbl');
}

//--------------------------------------------------------------------------------------

if (isset($_GET["stock_id"]))
	$_POST['SelectedStockItem'] = $_GET["stock_id"];

//--------------------------------------------------------------------------------------

start_form(false, false, $_SERVER['PHP_SELF'] ."?outstanding_only=" . $outstanding_only .SID);

start_table("class='tablestyle_noborder'");
start_row();
ref_cells(_("Reference:"), 'OrderNumber', '',null, '', true);

locations_list_cells(_("at Location:"), 'StockLocation', null, true);

check_cells( _("Only Overdue:"), 'OverdueOnly', null);

if ($outstanding_only==0)
	check_cells( _("Only Open:"), 'OpenOnly', null);

stock_manufactured_items_list_cells(_("for item:"), 'SelectedStockItem', null, true);

submit_cells('SearchOrders', _("Search"),'',_('Select documents'), true);
end_row();
end_table();

end_form();

$sql = "SELECT ".TB_PREF."workorders.*, ".TB_PREF."stock_master.description,".TB_PREF."locations.location_name
	FROM ".TB_PREF."workorders,".TB_PREF."stock_master,".TB_PREF."locations
	WHERE ".TB_PREF."stock_master.stock_id=".TB_PREF."workorders.stock_id AND
	".TB_PREF."locations.loc_code=".TB_PREF."workorders.loc_code ";

if (check_value('OpenOnly') || $outstanding_only != 0)
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

div_start('orders_tbl');
start_table("$table_style width=80%");

$th = array(_("#"), _("Reference"), _("Type"), _("Location"), _("Item"),
	_("Required"), _("Manufactured"), _("Date"), _("Required By"),
	'', '', '', '', '');
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

	$dec = get_qty_dec($myrow["stock_id"]);
	label_cell(get_trans_view_str(systypes::work_order(), $myrow["id"]));
	label_cell(get_trans_view_str(systypes::work_order(), $myrow["id"], $myrow["wo_ref"]));
	label_cell(wo_types::name($myrow["type"]));
	label_cell($myrow["location_name"]);
	view_stock_status_cell($myrow["stock_id"], $myrow["description"]);
	qty_cell($myrow["units_reqd"], false, $dec);
	qty_cell($myrow["units_issued"], false, $dec);
	label_cell(sql2date($myrow["date_"]));
	label_cell(sql2date($myrow["required_by"]));

	$l1 = $l2 = $l3 = $l4 = '';
	if ($myrow["closed"] == 0)
	{
		$modify_page = $path_to_root . "/manufacturing/work_order_entry.php?" . SID . "trans_no=" . $myrow["id"];
		$l1 = "<a href=$modify_page>"._('Edit').'</a>';
	 	if ($myrow["released"] == 0) 
	 	{
			$release_page = $path_to_root . "/manufacturing/work_order_release.php?" . SID . "trans_no=" . $myrow["id"];
	 		$l2 = "<a href=$release_page>"._('Release').'</a>';
	 	} 
	 	else 
	 	{
			$issue = $path_to_root . "/manufacturing/work_order_issue.php?" . SID . "trans_no=" .$myrow["id"];
			$add_finished = $path_to_root . "/manufacturing/work_order_add_finished.php?" . SID . "trans_no=" .$myrow["id"];
			$costs = $path_to_root . "/gl/gl_bank.php?NewPayment=1&PayType=" . payment_person_types::WorkOrder(). "&PayPerson=" .$myrow["id"];
	 		$l2 = "<a href=$issue>" . _("Issue") . "</a>";
	 		$l3 = "<a href=$add_finished>" . _("Produce") . "</a>";
	 		$l4 = "<a href=$costs>" . _("Costs") . "</a>";
		}
	}
	else
	{
		$l1 = "<i>"._('Closed')."</i>";
	}
	label_cell($l1);
	label_cell($l2);
	label_cell($l3);
	label_cell($l4);
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
div_end();
//---------------------------------------------------------------------------------

end_page();

?>
