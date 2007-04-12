<?php

$path_to_root="../..";
$page_security = 3;
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/sales/includes/sales_ui.inc");
include_once($path_to_root . "/sales/includes/sales_db.inc");
$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
page(_("Customer Allocations"), false, false, "", $js);

//--------------------------------------------------------------------------------

function display_allocatable_transactions()
{
	global $table_style, $path_to_root;
	start_form();

	/* show all outstanding receipts and credits to be allocated */
	/*Clear any previous allocation records */
	if (isset($_SESSION['alloc']))
	{
		unset($_SESSION['alloc']->allocs);
		unset($_SESSION['alloc']);
	}	
    if (!isset($_POST['customer_id']))
    	$_POST['customer_id'] = get_global_customer();

    echo "<center>" . _("Select a customer: ") . "&nbsp;&nbsp;";
	customer_list('customer_id', $_POST['customer_id'], true, true);
    echo "<br>";
    check(_("Show Settled Items:"), 'ShowSettled', null, true);
	echo "</center><br><br>";

	set_global_customer($_POST['customer_id']);

	if (isset($_POST['customer_id']) && ($_POST['customer_id'] == reserved_words::get_all())) 
	{
		unset($_POST['customer_id']);
	}

	/*if (isset($_POST['customer_id'])) {
		$custCurr = get_customer_currency($_POST['customer_id']);
		if (!is_company_currency($custCurr))
			echo _("Customer Currency:") . $custCurr;
	}*/

	$settled = false;
	if (check_value('ShowSettled'))
		$settled = true;

	$customer_id = null;
	if (isset($_POST['customer_id']))
		$customer_id = $_POST['customer_id'];

	$trans_items = get_allocatable_from_cust_transactions($customer_id, $settled);

	start_table($table_style);
	if (!isset($_POST['customer_id']))
		$th = array(_("Transaction Type"), _("#"), _("Reference"), _("Date"), _("Customer"),
			_("Currency"), _("Total"), _("Left To Allocate"), "");
	else	
		$th = array(_("Transaction Type"), _("#"), _("Reference"), _("Date"),
			_("Total"), _("Left To Allocate"), "");
	table_header($th);
	$k = 0; //row colour counter
	$has_settled_items = false;

	while ($myrow = db_fetch($trans_items))
	{
      	if ($myrow["settled"] == 1) 
      	{
      		start_row("class='settledbg'");
      		$has_settled_items = true;
      	} 
      	else 
      	{
    		alt_table_row_color($k);
      	}

    	label_cell(systypes::name($myrow["type"]));
    	label_cell(get_trans_view_str($myrow["type"], $myrow["trans_no"]));
		label_cell($myrow["reference"]);
    	label_cell(sql2date($myrow["tran_date"]));

		if (!isset($_POST['customer_id']))
		{
    		label_cell($myrow["DebtorName"]);
    		label_cell($myrow["curr_code"]);
		}
		amount_cell(-$myrow["Total"]);
    	amount_cell(-$myrow["Total"] - $myrow["alloc"]);
    	if (-$myrow["Total"] - $myrow["alloc"] != 0.0)
    		label_cell("<a href='$path_to_root/sales/allocations/customer_allocate.php?trans_no=" . $myrow["trans_no"] . "&trans_type=" . $myrow["type"]  . "'>" . _("Allocate") . "</a>");
    	else
    		label_cell("");
    	end_row();
	}

	end_table();

	if ($has_settled_items)
		display_note(_("Marked items are settled."), 0, 1, "class='settledfg'");

	if (db_num_rows($trans_items) == 0)
		display_note(_("There are no allocations to be done."), 1, 2);

	end_form();
}

//--------------------------------------------------------------------------------

display_allocatable_transactions();

//--------------------------------------------------------------------------------

end_page();

?>