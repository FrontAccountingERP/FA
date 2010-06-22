<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU General Public License, GPL, 
	as published by the Free Software Foundation, either version 3 
	of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/gpl-3.0.html>.
***********************************************************************/
$page_security = 'SA_SALESINVOICE';
$path_to_root = "..";
include_once($path_to_root . "/sales/includes/cart_class.inc");
include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/sales/includes/ui/sales_order_ui.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/reporting/includes/reporting.inc");

$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 600);

page(_($help_context = "Create and Print Recurrent Invoices"), false, false, "", $js);

function set_last_sent($id, $date)
{
	$date = date2sql($date);
	$sql = "UPDATE ".TB_PREF."recurrent_invoices SET last_sent='$date' WHERE id=".db_escape($id);
   	db_query($sql,"The recurrent invoice could not be updated or added");
}	

function create_recurrent_invoices($customer_id, $branch_id, $order_no, $tmpl_no)
{
	global $Refs;

	$doc = new Cart(ST_SALESORDER, array($order_no));

	get_customer_details_to_order($doc, $customer_id, $branch_id);

	$doc->trans_type = ST_SALESORDER;
	$doc->trans_no = 0;
	$doc->document_date = Today(); // 2006-06-15. Added so Invoices and Deliveries get current day

	$doc->due_date = get_invoice_duedate($doc->customer_id, $doc->document_date);
	$doc->reference = $Refs->get_next($doc->trans_type);
	//$doc->Comments='';

	foreach ($doc->line_items as $line_no=>$item) {
		$line = &$doc->line_items[$line_no];
		$line->price = get_price($line->stock_id, $doc->customer_currency,
			$doc->sales_type, $doc->price_factor, $doc->document_date);
	}	
	$cart = $doc;
	$cart->trans_type = ST_SALESINVOICE;
	$cart->reference = $Refs->get_next($cart->trans_type);
	$invno = $cart->write(1);
	set_last_sent($tmpl_no, $cart->document_date);
	return $invno;
}

if (isset($_GET['recurrent']))
{
	$date = Today();
	if (is_date_in_fiscalyear($date))
	{
		$invs = array();
		$sql = "SELECT * FROM ".TB_PREF."recurrent_invoices WHERE id=".db_escape($_GET['recurrent']);

		$result = db_query($sql,"could not get recurrent invoice");
		$myrow = db_fetch($result);
		if ($myrow['debtor_no'] == 0)
		{
			$cust = get_cust_branches_from_group($myrow['group_no']);
			while ($row = db_fetch($cust))
			{
				$invs[] = create_recurrent_invoices($row['debtor_no'], $row['branch_code'], $myrow['order_no'], $myrow['id']);
			}	
		}
		else
		{
			$invs[] = create_recurrent_invoices($myrow['debtor_no'], $myrow['group_no'], $myrow['order_no'], $myrow['id']);
		}
		if (count($invs) > 0)
		{
			$min = min($invs);
			$max = max($invs);
		}
		else 
			$min = $max = 0;
		display_notification(sprintf(_("%s recurrent invoice(s) created, # $min - # $max."), count($invs)));
		if (count($invs) > 0)
		{
			$ar = array('PARAM_0' => $min."-".ST_SALESINVOICE,	'PARAM_1' => $max."-".ST_SALESINVOICE, 'PARAM_2' => "",
				'PARAM_3' => 0,	'PARAM_4' => 0,	'PARAM_5' => "", 'PARAM_6' => ST_SALESINVOICE);
			display_note(print_link(_("&Print Recurrent Invoices # $min - # $max"), 107, $ar), 0, 1);
			$ar['PARAM_3'] = 1; 
			display_note(print_link(_("&Email Recurrent Invoices # $min - # $max"), 107, $ar), 0, 1);
		}
	}
	else
		display_error(_("The entered date is not in fiscal year."));
}	

//-------------------------------------------------------------------------------------------------
function get_sales_group_name($group_no)
{
	$sql = "SELECT description FROM ".TB_PREF."groups WHERE id = ".db_escape($group_no);
	$result = db_query($sql, "could not get group");
	$row = db_fetch($result);
	return $row[0];
}

$sql = "SELECT * FROM ".TB_PREF."recurrent_invoices ORDER BY description, group_no, debtor_no";
$result = db_query($sql,"could not get recurrent invoices");

start_table("$table_style width=70%");
$th = array(_("Description"), _("Template No"),_("Customer"),_("Branch")."/"._("Group"),_("Days"),_("Monthly"),_("Begin"),_("End"),_("Last Created"),"");
table_header($th);
$k = 0;
$today = add_days(Today(), 1);
$due = false;
while ($myrow = db_fetch($result)) 
{
	$begin = sql2date($myrow["begin"]);
	$end = sql2date($myrow["end"]);
	$last_sent = sql2date($myrow["last_sent"]);
	if ($myrow['monthly'] > 0)
		$due_date = begin_month($last_sent);
	else
		$due_date = $last_sent;
 	$due_date = add_months($due_date, $myrow['monthly']);
 	$due_date = add_days($due_date, $myrow['days']);
 	$overdue = date1_greater_date2($today, $due_date) && date1_greater_date2($today, $begin)
 		&& date1_greater_date2($end, $today);
	if ($overdue)
	{
		start_row("class='overduebg'");
		$due = true;
	}	
	else	
		alt_table_row_color($k);
		
	label_cell($myrow["description"]);
	label_cell(get_customer_trans_view_str(30, $myrow["order_no"]));
	if ($myrow["debtor_no"] == 0)
	{
		label_cell("");
		label_cell(get_sales_group_name($myrow["group_no"]));
	}	
	else
	{
		label_cell(get_customer_name($myrow["debtor_no"]));
		label_cell(get_branch_name($myrow['group_no']));
	}	
	label_cell($myrow["days"]);
	label_cell($myrow['monthly']);
	label_cell($begin);
	label_cell($end);
	label_cell($last_sent);
 	if ($overdue)
 		label_cell("<a href='$path_to_root/sales/create_recurrent_invoices.php?recurrent=" . $myrow["id"] . "'>" . _("Create Invoices") . "</a>");
 	else
 		label_cell("");
	end_row();
}
end_table();
if ($due)
	display_note(_("Marked items are due."), 1, 0, "class='overduefg'");
else
	display_note(_("No recurrent invoices are due."), 1, 0);

echo '<br>';

end_page();
?>
