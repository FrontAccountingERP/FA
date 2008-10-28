<?php

$page_security = 2;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

include($path_to_root . "/sales/includes/sales_ui.inc");
include_once($path_to_root . "/reporting/includes/reporting.inc");

$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 600);
if ($use_date_picker)
	$js .= get_js_date_picker();

if (isset($_GET['OutstandingOnly']) && ($_GET['OutstandingOnly'] == true))
{
	$_POST['OutstandingOnly'] = true;
	page(_("Search Not Invoiced Deliveries"), false, false, "", $js);
}
else
{
	$_POST['OutstandingOnly'] = false;
	page(_("Search All Deliveries"), false, false, "", $js);
}

if (isset($_GET['selected_customer']))
{
	$selected_customer = $_GET['selected_customer'];
}
elseif (isset($_POST['selected_customer']))
{
	$selected_customer = $_POST['selected_customer'];
}
else
	$selected_customer = -1;

if (isset($_POST['BatchInvoice']))
{

	// checking batch integrity
    $del_count = 0;
    foreach($_SESSION['Batch'] as $delivery)
    {
	  	$checkbox = 'Sel_'.$delivery['trans'];
	  	if (check_value($checkbox))
	  	{
	    	if (!$del_count)
	    	{
				$del_customer = $delivery['cust'];
				$del_branch = $delivery['branch'];
	    	}
	    	else
	    	{
				if ($del_customer!=$delivery['cust'] || $del_branch != $delivery['branch'])
				{
		    		$del_count=0;
		    		break;
				}
	    	}
	    	$selected[] = $delivery['trans'];
	    	$del_count++;
	  	}
    }

    if (!$del_count)
    {
		display_error(_('For batch invoicing you should
		    select at least one delivery. All items must be dispatched to
		    the same customer branch.'));
    }
    else
    {
		$_SESSION['DeliveryBatch'] = $selected;
		meta_forward($path_to_root . '/sales/customer_invoice.php','BatchInvoice=Yes');
    }
}

//-----------------------------------------------------------------------------------
if (get_post('SearchOrders')) 
{
	$Ajax->activate('deliveries_tbl');
} elseif (get_post('_DeliveryNumber_changed')) 
{
	$disable = get_post('DeliveryNumber') !== '';

	$Ajax->addDisable(true, 'DeliveryAfterDate', $disable);
	$Ajax->addDisable(true, 'DeliveryToDate', $disable);
	$Ajax->addDisable(true, 'StockLocation', $disable);
	$Ajax->addDisable(true, '_SelectStockFromList_edit', $disable);
	$Ajax->addDisable(true, 'SelectStockFromList', $disable);
	// if search is not empty rewrite table
	if ($disable) {
		$Ajax->addFocus(true, 'DeliveryNumber');
	} else
		$Ajax->addFocus(true, 'DeliveryAfterDate');
	$Ajax->activate('deliveries_tbl');
}

//-----------------------------------------------------------------------------------
print_hidden_script(13);

start_form(false, false, $_SERVER['PHP_SELF'] ."?OutstandingOnly=" . $_POST['OutstandingOnly'] .SID);

start_table("class='tablestyle_noborder'");
start_row();
ref_cells(_("#:"), 'DeliveryNumber', '',null, '', true);
date_cells(_("from:"), 'DeliveryAfterDate', '', null, -30);
date_cells(_("to:"), 'DeliveryToDate', '', null, 1);

locations_list_cells(_("Location:"), 'StockLocation', null, true);

stock_items_list_cells(_("Item:"), 'SelectStockFromList', null, true);

submit_cells('SearchOrders', _("Search"),'',_('Select documents'), true);

hidden('OutstandingOnly', $_POST['OutstandingOnly']);

end_row();

end_table();

//---------------------------------------------------------------------------------------------

if (isset($_POST['SelectStockFromList']) && ($_POST['SelectStockFromList'] != "") &&
	($_POST['SelectStockFromList'] != reserved_words::get_all()))
{
 	$selected_stock_item = $_POST['SelectStockFromList'];
}
else
{
	unset($selected_stock_item);
}

//---------------------------------------------------------------------------------------------
$sql = "SELECT ".TB_PREF."debtor_trans.trans_no, "
	.TB_PREF."debtors_master.curr_code, "
	.TB_PREF."debtors_master.name, "
	.TB_PREF."cust_branch.br_name, "
	.TB_PREF."debtor_trans.reference, "
	.TB_PREF."debtor_trans.tran_date, "
	.TB_PREF."debtor_trans.due_date, "
	.TB_PREF."sales_orders.customer_ref, "
	.TB_PREF."sales_orders.deliver_to, ";

$sql .= " Sum(".TB_PREF."debtor_trans_details.quantity-"
		 .TB_PREF."debtor_trans_details.qty_done) AS Outstanding, ";

$sql .= " Sum(".TB_PREF."debtor_trans_details.qty_done) AS Done, ";

$sql .= "(ov_amount+ov_gst+ov_freight+ov_freight_tax) AS DeliveryValue";
$sql .=" FROM "
	 .TB_PREF."sales_orders, "
	 .TB_PREF."debtor_trans, "
	 .TB_PREF."debtor_trans_details, "
	 .TB_PREF."debtors_master, "
	 .TB_PREF."cust_branch
		WHERE "
		.TB_PREF."sales_orders.order_no = ".TB_PREF."debtor_trans.order_ AND "
		.TB_PREF."debtor_trans.debtor_no = ".TB_PREF."debtors_master.debtor_no
			AND ".TB_PREF."debtor_trans.type = 13
			AND ".TB_PREF."debtor_trans_details.debtor_trans_no = ".TB_PREF."debtor_trans.trans_no
			AND ".TB_PREF."debtor_trans_details.debtor_trans_type = ".TB_PREF."debtor_trans.type
			AND ".TB_PREF."debtor_trans.branch_code = ".TB_PREF."cust_branch.branch_code
			AND ".TB_PREF."debtor_trans.debtor_no = ".TB_PREF."cust_branch.debtor_no ";

	if ($_POST['OutstandingOnly'] == true) {
	 $sql .= " AND ".TB_PREF."debtor_trans_details.qty_done < ".TB_PREF."debtor_trans_details.quantity ";
	}

//figure out the sql required from the inputs available
if (isset($_POST['DeliveryNumber']) && $_POST['DeliveryNumber'] != "")
{
// if ($_POST['DeliveryNumber'] != '*') // TODO paged table
	$sql .= " AND ".TB_PREF."debtor_trans.trans_no LIKE '%". $_POST['DeliveryNumber'] ."'";
 $sql .= " GROUP BY ".TB_PREF."debtor_trans.trans_no";
}
else
{

	$date_after = date2sql($_POST['DeliveryAfterDate']);
	$date_before = date2sql($_POST['DeliveryToDate']);

	$sql .= " AND ".TB_PREF."debtor_trans.tran_date >= '$date_after'";
	$sql .= " AND ".TB_PREF."debtor_trans.tran_date <= '$date_before'";

	if ($selected_customer != -1)
		$sql .= " AND ".TB_PREF."debtor_trans.debtor_no='" . $selected_customer . "' ";

	if (isset($selected_stock_item))
		$sql .= " AND ".TB_PREF."debtor_trans_details.stock_id='". $selected_stock_item ."' ";

	if (isset($_POST['StockLocation']) && $_POST['StockLocation'] != reserved_words::get_all())
		$sql .= " AND ".TB_PREF."sales_orders.from_stk_loc = '". $_POST['StockLocation'] . "' ";

	$sql .= " GROUP BY ".TB_PREF."debtor_trans.trans_no ";

} //end no delivery number selected

$result = db_query($sql,"No deliveries were returned");

//-----------------------------------------------------------------------------------
if (isset($_SESSION['Batch']))
{
    foreach($_SESSION['Batch'] as $trans=>$del)
    	unset($_SESSION['Batch'][$trans]);
    unset($_SESSION['Batch']);
}
if ($result)
{
	/*show a table of the deliveries returned by the sql */

	div_start('deliveries_tbl');

	start_table("$table_style colspan=7 width=95%");
	$th = array(_("Delivery #"), _("Customer"), _("Branch"), _("Reference"), _("Delivery Date"),
		_("Due By"), _("Delivery Total"), _("Currency"), submit('BatchInvoice','Batch Inv', false),
		 "", "", "");
	table_header($th);

	$j = 1;
	$k = 0; //row colour counter
	$overdue_items = false;
	while ($myrow = db_fetch($result))
	{
	    $_SESSION['Batch'][] = array('trans'=>$myrow["trans_no"],
	    'cust'=>$myrow["name"],'branch'=>$myrow["br_name"] );

	    $view_page = get_customer_trans_view_str(13, $myrow["trans_no"]);
	    $formated_del_date = sql2date($myrow["tran_date"]);
	    $formated_due_date = sql2date($myrow["due_date"]);
	    $not_closed =  $myrow["Outstanding"]!=0;

    	// if overdue orders, then highlight as so

    	if (date1_greater_date2(Today(), $formated_due_date) && $not_closed )
    	{
        	 start_row("class='overduebg'");
        	 $overdue_items = true;
    	}
    	else
    	{
			alt_table_row_color($k);
    	}

		label_cell($view_page);
		label_cell($myrow["name"]);
		label_cell($myrow["br_name"]);
		label_cell($myrow["reference"]);
		label_cell($formated_del_date);
		label_cell($formated_due_date);
		amount_cell($myrow["DeliveryValue"]);
		label_cell($myrow["curr_code"]);
		if (!$myrow['Done'])
		    check_cells(null,'Sel_'. $myrow['trans_no'],0,false);
		else
    		    label_cell("");
		if ($_POST['OutstandingOnly'] == true || $not_closed)
		{
    		$modify_page = $path_to_root . "/sales/customer_delivery.php?" . SID . "ModifyDelivery=" . $myrow["trans_no"];
    		$invoice_page = $path_to_root . "/sales/customer_invoice.php?" . SID . "DeliveryNumber=" .$myrow["trans_no"];
    		if (get_voided_entry(13, $myrow["trans_no"]) === false)
    			label_cell("<a href='$modify_page'>" . _("Edit") . "</a>");
    		else
    			label_cell("");
  		  	label_cell(print_document_link($myrow['trans_no'], _("Print")));

    		label_cell($not_closed ? "<a href='$invoice_page'>" . _("Invoice") . "</a>" : '');

		}
		else
		{
    		label_cell("");
    		label_cell("");
    		label_cell("");
		}
		end_row();;

		$j++;
		If ($j == 12)
		{
			$j = 1;
			table_header($th);
		}
		//end of page full new headings if
	}
	//end of while loop

	end_table();

   if ($overdue_items)
   		display_note(_("Marked items are overdue."), 0, 1, "class='overduefg'");
div_end();
}

echo "<br>";
end_form();

end_page();
?>

