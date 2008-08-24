<?php

$path_to_root="../..";
$page_security = 3;
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");

include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/banking.inc");

include_once($path_to_root . "/sales/includes/sales_db.inc");
$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
page(_("Supplier Allocations"), false, false, "", $js);

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
    if (!isset($_POST['supplier_id']))
    	$_POST['supplier_id'] = get_global_supplier();

    echo "<center>" . _("Select a Supplier: ") . "&nbsp;&nbsp;";
	supplier_list('supplier_id', $_POST['supplier_id'], true, true);
    echo "<br>";
    check(_("Show Settled Items:"), 'ShowSettled', null, true);
	echo "</center><br><br>";

	set_global_supplier($_POST['supplier_id']);

	if (isset($_POST['supplier_id']) && ($_POST['supplier_id'] == reserved_words::get_all())) 
	{
		unset($_POST['supplier_id']);
	}

	$settled = false;
	if (check_value('ShowSettled'))
		$settled = true;

	$supplier_id = null;
	if (isset($_POST['supplier_id']))
		$supplier_id = $_POST['supplier_id'];

	$trans_items = get_allocatable_from_supp_transactions($supplier_id, $settled);
	div_start('alloc_tbl');
	start_table($table_style);
	if (!isset($_POST['supplier_id']))
		$th = array(_("Transaction Type"), _("#"), _("Reference"), _("Date"), _("Supplier"), 
			_("Currency"), _("Total"), _("Left To Allocate"));
	else
		$th = array(_("Transaction Type"), _("#"), _("Reference"), _("Date"), 
			_("Total"), _("Left To Allocate"));
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

		if (!isset($_POST['supplier_id']))
		{
    		label_cell($myrow["supp_name"]);
    		label_cell($myrow["curr_code"]);
		}
		amount_cell(-$myrow["Total"]);
    	amount_cell(-$myrow["Total"]-$myrow["alloc"]);
    	label_cell("<a href='$path_to_root/purchasing/allocations/supplier_allocate.php?trans_no=" . $myrow["trans_no"] . "&trans_type=" . $myrow["type"]  . "'>" . _("Allocate") . "</a>");
    	end_row();
	}

	end_table();

	if ($has_settled_items)
		display_note(_("Marked items are settled."), 0, 1, "class='settledfg'");

	if (db_num_rows($trans_items) == 0)
		display_note(_("There are no allocations to be done."), 1, 2);
	div_end();
	end_form();
}

//--------------------------------------------------------------------------------

if (get_post('_ShowSettled_update')) {
	$Ajax->activate('alloc_tbl');
}
display_allocatable_transactions();

//--------------------------------------------------------------------------------

end_page();

?>