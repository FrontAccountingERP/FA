<?php

$page_security = 1;
$path_to_root="../..";
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/sales/includes/sales_ui.inc");
include_once($path_to_root . "/sales/includes/sales_db.inc");

$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();
page(_("Customer Allocation Inquiry"), false, false, "", $js);

if (isset($_GET['customer_id']))
{
	$_POST['customer_id'] = $_GET['customer_id'];
}

//------------------------------------------------------------------------------------------------

if (!isset($_POST['customer_id']))
	$_POST['customer_id'] = get_global_customer();

start_form(false, true);

start_table("class='tablestyle_noborder'");
start_row();

customer_list_cells(_("Select a customer: "), 'customer_id', $_POST['customer_id'], true);

date_cells(_("from:"), 'TransAfterDate', '', null, -30);
date_cells(_("to:"), 'TransToDate', '', null, 1);

cust_allocations_list_cells(_("Type:"), 'filterType', null);

check_cells(" " . _("show settled:"), 'showSettled', null);

submit_cells('RefreshInquiry', _("Search"),'',_('Refresh Inquiry'), true);

set_global_customer($_POST['customer_id']);

end_row();
end_table();
end_form();

//------------------------------------------------------------------------------------------------

function get_transactions()
{
    $data_after = date2sql($_POST['TransAfterDate']);
    $date_to = date2sql($_POST['TransToDate']);

    $sql = "SELECT ".TB_PREF."debtor_trans.*,
		".TB_PREF."debtors_master.name AS CustName, ".TB_PREF."debtors_master.curr_code AS CustCurrCode,
    	(".TB_PREF."debtor_trans.ov_amount + ".TB_PREF."debtor_trans.ov_gst + "
	.TB_PREF."debtor_trans.ov_freight + ".TB_PREF."debtor_trans.ov_freight_tax + ".TB_PREF."debtor_trans.ov_discount)
		AS TotalAmount,
		".TB_PREF."debtor_trans.alloc AS Allocated,
		((".TB_PREF."debtor_trans.type = 10)
		AND ".TB_PREF."debtor_trans.due_date < '" . date2sql(Today()) . "') AS OverDue
    	FROM ".TB_PREF."debtor_trans, ".TB_PREF."debtors_master
    	WHERE ".TB_PREF."debtors_master.debtor_no = ".TB_PREF."debtor_trans.debtor_no
			AND (".TB_PREF."debtor_trans.ov_amount + ".TB_PREF."debtor_trans.ov_gst + "
			.TB_PREF."debtor_trans.ov_freight + ".TB_PREF."debtor_trans.ov_freight_tax + ".TB_PREF."debtor_trans.ov_discount != 0)
    		AND ".TB_PREF."debtor_trans.tran_date >= '$data_after'
    		AND ".TB_PREF."debtor_trans.tran_date <= '$date_to'";

   	if ($_POST['customer_id'] != reserved_words::get_all())
   		$sql .= " AND ".TB_PREF."debtor_trans.debtor_no = '" . $_POST['customer_id'] . "'";

   	if (isset($_POST['filterType']) && $_POST['filterType'] != reserved_words::get_all())
   	{
   		if ($_POST['filterType'] == '1' || $_POST['filterType'] == '2')
   		{
   			$sql .= " AND ".TB_PREF."debtor_trans.type = 10 ";
   		}
   		elseif ($_POST['filterType'] == '3')
   		{
			$sql .= " AND ".TB_PREF."debtor_trans.type = " . systypes::cust_payment();
   		}
   		elseif ($_POST['filterType'] == '4')
   		{
			$sql .= " AND ".TB_PREF."debtor_trans.type = 11 ";
   		}

    	if ($_POST['filterType'] == '2')
    	{
    		$today =  date2sql(Today());
    		$sql .= " AND ".TB_PREF."debtor_trans.due_date < '$today'
				AND (round(abs(".TB_PREF."debtor_trans.ov_amount + "
				.TB_PREF."debtor_trans.ov_gst + ".TB_PREF."debtor_trans.ov_freight + "
				.TB_PREF."debtor_trans.ov_freight_tax + ".TB_PREF."debtor_trans.ov_discount) - ".TB_PREF."debtor_trans.alloc,6) > 0) ";
    	}
   	}else
   	{
	    $sql .= " AND ".TB_PREF."debtor_trans.type != 13 ";
   	}


   	if (!check_value('showSettled'))
   	{
   		$sql .= " AND (round(abs(".TB_PREF."debtor_trans.ov_amount + ".TB_PREF."debtor_trans.ov_gst + "
		.TB_PREF."debtor_trans.ov_freight + ".TB_PREF."debtor_trans.ov_freight_tax + "
		.TB_PREF."debtor_trans.ov_discount) - ".TB_PREF."debtor_trans.alloc,6) != 0) ";
   	}

    $sql .= " ORDER BY ".TB_PREF."debtor_trans.tran_date";

    return db_query($sql,"No transactions were returned");
}

//------------------------------------------------------------------------------------------------

$result = get_transactions();

//------------------------------------------------------------------------------------------------
if(get_post('RefreshInquiry')) 
{
	$Ajax->activate('doc_tbl');
}
//------------------------------------------------------------------------------------------------
div_start('doc_tbl');
if (db_num_rows($result) == 0)
{
	display_note(_("The selected customer has no transactions for the given dates."), 1, 1);
} else {

	start_table("$table_style width='80%'");

if ($_POST['customer_id'] == reserved_words::get_all())
	$th = array(_("Type"), _("Number"), _("Reference"), _("Order"), _("Date"), _("Due Date"),
		_("Customer"), _("Currency"), _("Debit"), _("Credit"), _("Allocated"), _("Balance"), "");
else
	$th = array(_("Type"), _("Number"), _("Reference"), _("Order"), _("Date"), _("Due Date"),
		_("Debit"), _("Credit"), _("Allocated"), _("Balance"), "");

table_header($th);


$j = 1;
$k = 0; //row colour counter
$over_due = false;
while ($myrow = db_fetch($result))
{

	if ($myrow['OverDue'] == 1 && (abs($myrow["TotalAmount"]) - $myrow["Allocated"] != 0))
	{
		start_row("class='overduebg'");
		$over_due = true;
	}
	else
		alt_table_row_color($k);

	$date = sql2date($myrow["tran_date"]);

	if ($myrow["order_"] > 0)
		$preview_order_str = get_customer_trans_view_str(systypes::sales_order(), $myrow["order_"]);
	else
		$preview_order_str = "";

	$allocations_str = "";

	$allocations = "<a href='$path_to_root/sales/allocations/customer_allocate.php?trans_no=" . $myrow["trans_no"] ."&trans_type=" . $myrow["type"] ."'>" . _("Allocation") . "</a>";

	$due_date_str = "";

	if ($myrow["type"] == 10)
		$due_date_str = sql2date($myrow["due_date"]);
	elseif ($myrow["type"] == 11 && $myrow['TotalAmount'] > 0)
	{
		/*its a credit note which could have an allocation */
		$allocations_str = $allocations;

	}
	elseif (($myrow["type"] == systypes::cust_payment() || $myrow["type"] == systypes::bank_deposit()) &&
		($myrow['TotalAmount'] - $myrow['Allocated']) > 0)
	{
		/*its a receipt  which could have an allocation*/
		$allocations_str = $allocations;

	}
	elseif ($myrow["type"] == systypes::cust_payment() && $myrow['TotalAmount'] < 0)
	{
		/*its a negative receipt */
	}

	label_cell(systypes::name($myrow["type"]));
	label_cell(get_trans_view_str($myrow["type"], $myrow["trans_no"]));
	label_cell($myrow["reference"]);
	label_cell($preview_order_str);
	label_cell(sql2date($myrow["tran_date"]), "nowrap");
	label_cell($due_date_str, "nowrap");
	if ($_POST['customer_id'] == reserved_words::get_all())
	{
		label_cell($myrow["CustName"]);
		label_cell($myrow["CustCurrCode"]);
	}
	display_debit_or_credit_cells(
	    $myrow['type']==11 || $myrow['type']==12 || $myrow['type']==2 ?
		-$myrow["TotalAmount"] : $myrow["TotalAmount"]);
	amount_cell(abs($myrow["Allocated"]));
	amount_cell(abs($myrow["TotalAmount"]) - $myrow["Allocated"]);
	label_cell($allocations_str);


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
