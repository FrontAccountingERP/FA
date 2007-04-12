<?php

$page_security = 2;
$path_to_root="../..";

include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(800, 500);

if ($_GET['outstanding_only']) 
{
	$outstanding_only = 1;
	page(_("Search Outstanding Dimensionss"), false, false, "", $js);
} 
else 
{
	$outstanding_only = 0;
	page(_("Search Dimensions"), false, false, "", $js);
}

//--------------------------------------------------------------------------------------

if (isset($_GET["stock_id"]))
	$_POST['SelectedStockItem'] = $_GET["stock_id"];

//--------------------------------------------------------------------------------------

start_form(false, true, $_SERVER['PHP_SELF'] ."?outstanding_only=" . $outstanding_only . SID);

start_table("class='tablestyle_noborder'");
start_row();

ref_cells(_("Reference:"), 'OrderNumber', null);

number_list_cells(_("Type"), 'type_', null, 0, 2);
date_cells(_("From:"), 'FromDate', null, 0, 0, -5);
date_cells(_("To:"), 'ToDate');

check_cells( _("Only Overdue:"), 'OverdueOnly', null);

if (!$outstanding_only) 
{
   	check_cells( _("Only Open:"), 'OpenOnly', null);
} 
else
	$_POST['OpenOnly'] = 1;

submit_cells('SearchOrders', _("Search"));

end_row();
end_table();

end_form();

$dim = get_company_pref('use_dimension');

$sql = "SELECT * FROM ".TB_PREF."dimensions WHERE id > 0";

if ($dim == 1)
	$sql .= " AND type_=1";
	
if (isset($_POST['OpenOnly'])) 
{
   	$sql .= " AND closed=0";
}

if (isset($_POST['type_']) && ($_POST['type_'] > 0)) 
{
   	$sql .= " AND type_=" . $_POST['type_'];
}

if (isset($_POST['OrderNumber']) && $_POST['OrderNumber'] != "") 
{
	$sql .= " AND reference LIKE '%". $_POST['OrderNumber'] . "%'";
}

if (isset($_POST['OverdueOnly'])) 
{
	$today = date2sql(Today());

   	$sql .= " AND due_date < '$today' ";
}

$sql .= " AND date_ >= '" . date2sql($_POST['FromDate']) . "'
	AND date_ <= '" . date2sql($_POST['ToDate']) . "'";

$sql .= " ORDER BY due_date";

$result = db_query($sql,"could not query dimensions");

start_table("$table_style width=80%"); 

if (!$outstanding_only)
	$th = array(_("#"), _("Reference"), _("Name"), _("Type"), _("Date"),
		_("Due Date"), _("Closed"), _("Balance"));
else
	$th = array(_("#"), _("Reference"), _("Name"), _("Type"), _("Date"),
		_("Due Date"), _("Balance"));
table_header($th);
$j = 1;
$k = 0;

while ($myrow = db_fetch($result)) 
{
	$sql = "SELECT SUM(amount) FROM ".TB_PREF."gl_trans WHERE tran_date >= '" . 
		date2sql($_POST['FromDate']) . "' AND 
		tran_date <= '" . date2sql($_POST['ToDate']) . "' AND dimension_id = " . 
		$myrow['id'];
	$res = db_query($sql, "Transactions could not be calculated");
	$row = db_fetch_row($res);
		
	if ($k == 1)
	{
		$row_text = "class='oddrow'";
		$k = 0;
	} 
	else 
	{
		$row_text = "class='evenrow'";
		$k++;
	}

	// check if it's an overdue work order
	if (date_diff(Today(), sql2date($myrow["due_date"]), "d") > 0) 
	{
		$row_text = "class='overduebg'";
	}

	start_row($row_text);

	$mpage = $path_to_root . "/dimensions/dimension_entry.php?" . SID . "trans_no=" . $myrow["id"];

	label_cell(get_dimensions_trans_view_str(systypes::dimension(), $myrow["id"]));
	label_cell(get_dimensions_trans_view_str(systypes::dimension(), $myrow["id"], $myrow["reference"]));
	label_cell($myrow["name"]);
	label_cell($myrow["type_"]);
	label_cell(sql2date($myrow["date_"]));
	label_cell(sql2date($myrow["due_date"]));
	if (!$outstanding_only)
		label_cell(($myrow["closed"] ? _("Yes") : _("No")));
	amount_cell($row[0]);
	if ($myrow["closed"] == 0) 
		label_cell("<a href='$mpage'>" . _("Edit") . "</a>");
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
