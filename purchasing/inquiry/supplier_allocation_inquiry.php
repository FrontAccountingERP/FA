<?php

$page_security=2;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

include($path_to_root . "/purchasing/includes/purchasing_ui.inc");
$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();
page(_("Supplier Allocation Inquiry"), false, false, "", $js);

if (isset($_GET['supplier_id']))
{
	$_POST['supplier_id'] = $_GET['supplier_id'];
}
if (isset($_GET['FromDate']))
{
	$_POST['TransAfterDate'] = $_GET['FromDate'];
}
if (isset($_GET['ToDate']))
{
	$_POST['TransToDate'] = $_GET['ToDate'];
}

//------------------------------------------------------------------------------------------------

start_form(false, true);

if (!isset($_POST['supplier_id']))
	$_POST['supplier_id'] = get_global_supplier();

start_table("class='tablestyle_noborder'");
start_row();

supplier_list_cells(_("Select a supplier: "), 'supplier_id', $_POST['supplier_id'], true);

date_cells(_("From:"), 'TransAfterDate', '', null, -30);
date_cells(_("To:"), 'TransToDate', '', null, 1);

supp_allocations_list_cell("filterType", null);

check_cells(_("show settled:"), 'showSettled', null);

submit_cells('RefreshInquiry', _("Search"),'',_('Refresh Inquiry'), true);

set_global_supplier($_POST['supplier_id']);

end_row();
end_table();
end_form();


//------------------------------------------------------------------------------------------------

function get_transactions()
{
	global $db;

    $date_after = date2sql($_POST['TransAfterDate']);
    $date_to = date2sql($_POST['TransToDate']);

    // Sherifoz 22.06.03 Also get the description
    $sql = "SELECT ".TB_PREF."supp_trans.type, ".TB_PREF."supp_trans.trans_no,
    	".TB_PREF."supp_trans.tran_date, ".TB_PREF."supp_trans.reference, ".TB_PREF."supp_trans.supp_reference,
    	(".TB_PREF."supp_trans.ov_amount + ".TB_PREF."supp_trans.ov_gst  + ".TB_PREF."supp_trans.ov_discount) AS TotalAmount, ".TB_PREF."supp_trans.alloc AS Allocated,
		((".TB_PREF."supp_trans.type = 20 OR ".TB_PREF."supp_trans.type = 21) AND ".TB_PREF."supp_trans.due_date < '" . date2sql(Today()) . "') AS OverDue,
		".TB_PREF."suppliers.curr_code, ".TB_PREF."suppliers.supp_name, ".TB_PREF."supp_trans.due_date
    	FROM ".TB_PREF."supp_trans, ".TB_PREF."suppliers
    	WHERE ".TB_PREF."suppliers.supplier_id = ".TB_PREF."supp_trans.supplier_id
     	AND ".TB_PREF."supp_trans.tran_date >= '$date_after'
    	AND ".TB_PREF."supp_trans.tran_date <= '$date_to'";
   	if ($_POST['supplier_id'] != reserved_words::get_all())
   		$sql .= " AND ".TB_PREF."supp_trans.supplier_id = '" . $_POST['supplier_id'] . "'";
   	if (isset($_POST['filterType']) && $_POST['filterType'] != reserved_words::get_all())
   	{
   		if (($_POST['filterType'] == '1') || ($_POST['filterType'] == '2'))
   		{
   			$sql .= " AND ".TB_PREF."supp_trans.type = 20 ";
   		}
   		elseif ($_POST['filterType'] == '3')
   		{
			$sql .= " AND ".TB_PREF."supp_trans.type = 22 ";
   		}
   		elseif (($_POST['filterType'] == '4') || ($_POST['filterType'] == '5'))
   		{
			$sql .= " AND ".TB_PREF."supp_trans.type = 21 ";
   		}

   		if (($_POST['filterType'] == '2') || ($_POST['filterType'] == '5'))
   		{
   			$today =  date2sql(Today());
			$sql .= " AND ".TB_PREF."supp_trans.due_date < '$today' ";
   		}
   	}

   	if (!check_value('showSettled'))
   	{
   		$sql .= " AND (round(abs(ov_amount + ov_gst + ov_discount) - alloc,6) != 0) ";
   	}

    $sql .= " ORDER BY ".TB_PREF."supp_trans.tran_date";

    return db_query($sql,"No supplier transactions were returned");
}

//------------------------------------------------------------------------------------------------

$result = get_transactions();
//------------------------------------------------------------------------------------------------
if(get_post('RefreshInquiry')) 
{
	$Ajax->activate('doc_tbl');
}

//------------------------------------------------------------------------------------------------

/*show a table of the transactions returned by the sql */

div_start('doc_tbl');
if (db_num_rows($result) == 0)
{
	display_note(_("There are no transactions to display for the given dates."), 1, 1);
} else
{
  start_table("$table_style width=90%");
  if ($_POST['supplier_id'] == reserved_words::get_all())
	$th = array(_("Type"), _("Number"), _("Reference"), _("Supplier"),
		_("Supp Reference"), _("Date"), _("Due Date"), _("Currency"),
		_("Debit"), _("Credit"), _("Allocated"), _("Balance"), "");
  else
	$th = array(_("Type"), _("Number"), _("Reference"),	_("Supp Reference"), _("Date"), _("Due Date"),
		_("Debit"), _("Credit"), _("Allocated"), _("Balance"), "");
  table_header($th);

  $j = 1;
  $k = 0; //row colour counter
  $over_due = false;
  while ($myrow = db_fetch($result))
  {

	if ($myrow['OverDue'] == 1)
	{
		start_row("class='overduebg'");
		$over_due = true;
	}
	else
	{
		alt_table_row_color($k);
	}

	$date = sql2date($myrow["tran_date"]);

	$duedate = ((($myrow["type"] == 20) || ($myrow["type"]== 21))?sql2date($myrow["due_date"]):"");


	label_cell(systypes::name($myrow["type"]));
	label_cell(get_trans_view_str($myrow["type"],$myrow["trans_no"]));
	label_cell($myrow["reference"]);
	if ($_POST['supplier_id'] == reserved_words::get_all())
		label_cell($myrow["supp_name"]);
	label_cell($myrow["supp_reference"]);
	label_cell($date);
	label_cell($duedate);
    if ($_POST['supplier_id'] == reserved_words::get_all())
    	label_cell($myrow["curr_code"]);
    if ($myrow["TotalAmount"] >= 0)
    	label_cell("");
	amount_cell(abs($myrow["TotalAmount"]));
	if ($myrow["TotalAmount"] < 0)
		label_cell("");
	amount_cell($myrow["Allocated"]);
	if ($myrow["type"] == 1 || $myrow["type"] == 21 || $myrow["type"] == 22)
		$balance = -$myrow["TotalAmount"] - $myrow["Allocated"];
	else
		$balance = $myrow["TotalAmount"] - $myrow["Allocated"];
	amount_cell($balance);

	//if (($myrow["type"] == 1 || $myrow["type"] == 21 || $myrow["type"] == 22) &&
	//	$myrow["Void"] == 0)
	if (($myrow["type"] == 1 || $myrow["type"] == 21 || $myrow["type"] == 22) &&
		$balance > 0)
	{
		label_cell("<a href='$path_to_root/purchasing/allocations/supplier_allocate.php?trans_no=" .
			$myrow["trans_no"]. "&trans_type=" . $myrow["type"] . "'>" . _("Allocations") . "</a>");
	}
	else
		label_cell("");

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
  if ($over_due)
	display_note(_("Marked items are overdue."), 0, 1, "class='overduefg'");
}
div_end();
end_page();
?>
