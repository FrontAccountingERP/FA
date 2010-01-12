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
$page_security = 'SA_FISCALYEARS';
$path_to_root = "..";
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/admin/db/company_db.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/sales/includes/db/cust_trans_db.inc");
include_once($path_to_root . "/admin/db/maintenance_db.inc");
$js = "";
if ($use_date_picker)
	$js .= get_js_date_picker();
page(_($help_context = "Fiscal Years"), false, false, "", $js);

simple_page_mode(true);
//---------------------------------------------------------------------------------------------

function is_date_in_fiscalyears($date)
{
	$date = date2sql($date);
	$sql = "SELECT * FROM ".TB_PREF."fiscal_year WHERE '$date' >= begin AND '$date' <= end";

	$result = db_query($sql, "could not get all fiscal years");
	return db_fetch($result) !== false;
}

function is_bad_begin_date($date)
{
	$bdate = date2sql($date);
	$sql = "SELECT MAX(end) FROM ".TB_PREF."fiscal_year WHERE begin < '$bdate'";

	$result = db_query($sql, "could not retrieve last fiscal years");
	$row = db_fetch_row($result);
	if ($row[0] === null)
		return false;
	$max = add_days(sql2date($row[0]), 1);
	return ($max !== $date);
}

function check_years_before($date, $closed=false)
{
	$date = date2sql($date);
	$sql = "SELECT COUNT(*) FROM ".TB_PREF."fiscal_year WHERE begin < '$date'";
	if (!$closed)
		$sql .= " AND closed=0";

	$result = db_query($sql, "could not check fiscal years before");
	$row = db_fetch_row($result);
	return ($row[0] > 0);
}

function check_data()
{
	if (!is_date($_POST['from_date']) || is_date_in_fiscalyears($_POST['from_date']) || is_bad_begin_date($_POST['from_date']))
	{
		display_error( _("Invalid BEGIN date in fiscal year."));
		set_focus('from_date');
		return false;
	}
	if (!is_date($_POST['to_date']) || is_date_in_fiscalyears($_POST['to_date']))
	{
		display_error( _("Invalid END date in fiscal year."));
		set_focus('to_date');
		return false;
	}
	if (date1_greater_date2($_POST['from_date'], $_POST['to_date']))
	{
		display_error( _("BEGIN date bigger than END date."));
		set_focus('from_date');
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------------------------
function close_year($year)
{
	$co = get_company_prefs();
	if (get_gl_account($co['retained_earnings_act']) == false || get_gl_account($co['profit_loss_year_act']) == false)
	{
		display_error(_("The Retained Earnings Account or the Profit and Loss Year Account has not been set in System and General GL Setup"));
		return false;
	}
	begin_transaction();

	$myrow = get_fiscalyear($year);
	$to = $myrow['end'];
	// retrieve total balances from balance sheet accounts
    $sql = "SELECT SUM(amount) FROM ".TB_PREF."gl_trans INNER JOIN ".TB_PREF."chart_master ON account=account_code
    	INNER JOIN ".TB_PREF."chart_types ON account_type=id INNER JOIN ".TB_PREF."chart_class ON class_id=cid 
		WHERE ctype>=".CL_ASSETS." AND ctype <=".CL_EQUITY." AND tran_date <= '$to'";
	$result = db_query($sql, "The total balance could not be calculated");

	$row = db_fetch_row($result);
	$balance = round2($row[0], user_price_dec());

	$to = sql2date($to);

	if ($balance != 0.0)
	{
		$trans_type = ST_JOURNAL;
		$trans_id = get_next_trans_no($trans_type);

		add_gl_trans($trans_type, $trans_id, $to, $co['retained_earnings_act'],
			0, 0, _("Closing Year"), -$balance);
		add_gl_trans($trans_type, $trans_id, $to, $co['profit_loss_year_act'],
			0, 0, _("Closing Year"), $balance);

	}	
	close_transactions($to);

	commit_transaction();
	return true;
}

function open_year($year)
{
	$myrow = get_fiscalyear($year);
	$from = sql2date($myrow['begin']);

	begin_transaction();
	open_transactions($from);
	commit_transaction();
}

function handle_submit()
{
	global $selected_id, $Mode;

	$ok = true;
	if ($selected_id != -1)
	{
		if ($_POST['closed'] == 1)
		{
			if (check_years_before($_POST['from_date'], false))
			{
				display_error( _("Cannot CLOSE this year because there are open fiscal years before"));
				set_focus('closed');
				return false;
			}	
			$ok = close_year($selected_id);
		}	
		else
			open_year($selected_id);
		if ($ok)
		{
   			update_fiscalyear($selected_id, $_POST['closed']);
			display_notification(_('Selected fiscal year has been updated'));
		}	
	}
	else
	{
		if (!check_data())
			return false;
   		add_fiscalyear($_POST['from_date'], $_POST['to_date'], $_POST['closed']);
		display_notification(_('New fiscal year has been added'));
	}
	$Mode = 'RESET';
}

//---------------------------------------------------------------------------------------------

function check_can_delete($selected_id)
{
	$myrow = get_fiscalyear($selected_id);
	// PREVENT DELETES IF DEPENDENT RECORDS IN gl_trans
	if (check_years_before(sql2date($myrow['begin']), true))
	{
		display_error(_("Cannot delete this fiscal year because thera are fiscal years before."));
		return false;
	}
	if ($myrow['closed'] == 0)
	{
		display_error(_("Cannot delete this fiscal year because the fiscal year is not closed."));
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------------------------
function delete_attachments_and_comments($type_no, $trans_no)
{
	global $comp_path;
	
	$sql = "SELECT * FROM ".TB_PREF."attachments WHERE type_no = $type_no AND trans_no = $trans_no";
	$result = db_query($sql, "Could not retrieve attachments");
	while ($row = db_fetch($result))
	{
		$dir =  $comp_path."/".user_company(). "/attachments";
		if (file_exists($dir."/".$row['unique_name']))
			unlink($dir."/".$row['unique_name']);
		$sql = "DELETE FROM ".TB_PREF."attachments WHERE  type_no = $type_no AND trans_no = $trans_no";
		db_query($sql, "Could not delete attachment");
	}	
	$sql = "DELETE FROM ".TB_PREF."comments WHERE  type = $type_no AND id = $trans_no";
	db_query($sql, "Could not delete comments");
	$sql = "DELETE FROM ".TB_PREF."refs WHERE  type = $type_no AND id = $trans_no";
	db_query($sql, "Could not delete refs");
}	

function delete_this_fiscalyear($selected_id)
{
	global $db_connections;
	
	db_backup($db_connections[$_SESSION["wa_current_user"]->company], 'Security backup before Fiscal Year Removal');
	begin_transaction();
	$ref = _("Open Balance");
	$myrow = get_fiscalyear($selected_id);
	$to = $myrow['end'];
	$sql = "SELECT order_no, trans_type FROM ".TB_PREF."sales_orders WHERE ord_date <= '$to' AND type <> 1"; // don't take the templates
	$result = db_query($sql, "Could not retrieve sales orders");
	while ($row = db_fetch($result))
	{
		$sql = "SELECT SUM(qty_sent), SUM(quantity) FROM ".TB_PREF."sales_order_details WHERE order_no = {$row['order_no']} AND trans_type = {$row['trans_type']}";
		$res = db_query($sql, "Could not retrieve sales order details");
		$row2 = db_fetch_row($res);
		if ($row2[0] == $row2[1])
		{
			$sql = "DELETE FROM ".TB_PREF."sales_order_details WHERE order_no = {$row['order_no']} AND trans_type = {$row['trans_type']}";
			db_query($sql, "Could not delete sales order details");
			$sql = "DELETE FROM ".TB_PREF."sales_orders WHERE order_no = {$row['order_no']} AND trans_type = {$row['trans_type']}";
			db_query($sql, "Could not delete sales order");
			delete_attachments_and_comments($row['trans_type'], $row['order_no']);
		}
	}
	$sql = "SELECT order_no FROM ".TB_PREF."purch_orders WHERE ord_date <= '$to'";
	$result = db_query($sql, "Could not retrieve purchase orders");
	while ($row = db_fetch($result))
	{
		$sql = "SELECT SUM(quantity_ordered), SUM(quantity_received) FROM ".TB_PREF."purch_order_details WHERE order_no = {$row['order_no']}";
		$res = db_query($sql, "Could not retrieve purchase order details");
		$row2 = db_fetch_row($res);
		if ($row2[0] == $row2[1])
		{
			$sql = "DELETE FROM ".TB_PREF."purch_order_details WHERE order_no = {$row['order_no']}";
			db_query($sql, "Could not delete purchase order details");
			$sql = "DELETE FROM ".TB_PREF."purch_orders WHERE order_no = {$row['order_no']}";
			db_query($sql, "Could not delete purchase order");
			delete_attachments_and_comments(ST_PURCHORDER, $row['order_no']);
		}
	}
	$sql = "SELECT id FROM ".TB_PREF."grn_batch WHERE delivery_date <= '$to'";
	$result = db_query($sql, "Could not retrieve grn batch");
	while ($row = db_fetch($result))
	{
		$sql = "DELETE FROM ".TB_PREF."grn_items WHERE grn_batch_id = {$row['id']}";
		db_query($sql, "Could not delete grn items");
		$sql = "DELETE FROM ".TB_PREF."grn_batch WHERE id = {$row['id']}";
		db_query($sql, "Could not delete grn batch");
		delete_attachments_and_comments(25, $row['id']);
	}
	$sql = "SELECT trans_no, type FROM ".TB_PREF."debtor_trans WHERE tran_date <= '$to' AND 
		(ov_amount + ov_gst + ov_freight + ov_freight_tax + ov_discount) = alloc";
	$result = db_query($sql, "Could not retrieve debtor trans");
	while ($row = db_fetch($result))
	{
		if ($row['type'] == ST_SALESINVOICE)
		{
			$deliveries = get_parent_trans(ST_SALESINVOICE,$row['trans_no']);
			foreach ($deliveries as $delivery)
			{
				$sql = "DELETE FROM ".TB_PREF."debtor_trans_details WHERE debtor_trans_no = $delivery AND debtor_trans_type = ".ST_CUSTDELIVERY;
				db_query($sql, "Could not delete debtor trans details");
				$sql = "DELETE FROM ".TB_PREF."debtor_trans WHERE trans_no = $delivery AND type = ".ST_CUSTDELIVERY;
				db_query($sql, "Could not delete debtor trans");
				delete_attachments_and_comments(ST_CUSTDELIVERY, $delivery);
			}		
		}	
		$sql = "DELETE FROM ".TB_PREF."cust_allocations WHERE trans_no_from = {$row['trans_no']} AND trans_type_from = {$row['type']}";
		db_query($sql, "Could not delete cust allocations");
		$sql = "DELETE FROM ".TB_PREF."debtor_trans_details WHERE debtor_trans_no = {$row['trans_no']} AND debtor_trans_type = {$row['type']}";
		db_query($sql, "Could not delete debtor trans details");
		$sql = "DELETE FROM ".TB_PREF."debtor_trans WHERE trans_no = {$row['trans_no']} AND type = {$row['type']}";
		db_query($sql, "Could not delete debtor trans");
		delete_attachments_and_comments($row['type'], $row['trans_no']);
	}
	$sql = "SELECT trans_no, type FROM ".TB_PREF."supp_trans WHERE tran_date <= '$to' AND 
		ABS(ov_amount + ov_gst + ov_discount) = alloc";
	$result = db_query($sql, "Could not retrieve supp trans");
	while ($row = db_fetch($result))
	{
		$sql = "DELETE FROM ".TB_PREF."supp_allocations WHERE trans_no_from = {$row['trans_no']} AND trans_type_from = {$row['type']}";
		db_query($sql, "Could not delete supp allocations");
		$sql = "DELETE FROM ".TB_PREF."supp_invoice_items WHERE supp_trans_no = {$row['trans_no']} AND supp_trans_type = {$row['type']}";
		db_query($sql, "Could not delete supp invoice items");
		$sql = "DELETE FROM ".TB_PREF."supp_trans WHERE trans_no = {$row['trans_no']} AND type = {$row['type']}";
		db_query($sql, "Could not delete supp trans");
		delete_attachments_and_comments($row['type'], $row['trans_no']);
	}
	$sql = "SELECT id FROM ".TB_PREF."workorders WHERE released_date <= '$to' AND closed=1";
	$result = db_query($sql, "Could not retrieve supp trans");
	while ($row = db_fetch($result))
	{
		$sql = "SELECT issue_no FROM ".TB_PREF."wo_issues WHERE workorder_id = {$row['id']}"; 
		$res = db_query($sql, "Could not retrieve wo issues");
		while ($row2 = db_fetch_row($res))
		{
			$sql = "DELETE FROM ".TB_PREF."wo_issue_items WHERE issue_id = {$row2[0]}";
			db_query($sql, "Could not delete wo issue items");
		}	
		delete_attachments_and_comments(ST_MANUISSUE, $row['id']);
		$sql = "DELETE FROM ".TB_PREF."wo_issues WHERE workorder_id = {$row['id']}";
		db_query($sql, "Could not delete wo issues");
		$sql = "DELETE FROM ".TB_PREF."wo_manufacture WHERE workorder_id = {$row['id']}";
		db_query($sql, "Could not delete wo manufacture");
		$sql = "DELETE FROM ".TB_PREF."wo_requirements WHERE workorder_id = {$row['id']}";
		db_query($sql, "Could not delete wo requirements");
		$sql = "DELETE FROM ".TB_PREF."workorders WHERE id = {$row['id']}";
		db_query($sql, "Could not delete workorders");
		delete_attachments_and_comments(ST_WORKORDER, $row['id']);
	}
	$sql = "SELECT loc_code, stock_id, SUM(qty) AS qty, SUM(qty*standard_cost) AS std_cost FROM ".TB_PREF."stock_moves WHERE tran_date <= '$to' GROUP by 
		loc_code, stock_id";
	$result = db_query($sql, "Could not retrieve supp trans");
	while ($row = db_fetch($result))
	{
		$sql = "DELETE FROM ".TB_PREF."stock_moves WHERE tran_date <= '$to' AND loc_code = '{$row['loc_code']}' AND stock_id = '{$row['stock_id']}'";
		db_query($sql, "Could not delete stock moves");
		$qty = $row['qty'];
		$std_cost = ($qty == 0 ? 0 : round2($row['std_cost'] / $qty, user_price_dec()));
		$sql = "INSERT INTO ".TB_PREF."stock_moves (stock_id, loc_code, tran_date, reference, qty, standard_cost) VALUES
			('{$row['stock_id']}', '{$row['loc_code']}', '$to', '$ref', $qty, $std_cost)";   
		db_query($sql, "Could not insert stock move");
	}		
	$sql = "DELETE FROM ".TB_PREF."voided WHERE date_ <= '$to'";
	db_query($sql, "Could not delete voided items");
	$sql = "DELETE FROM ".TB_PREF."trans_tax_details WHERE tran_date <= '$to'";
	db_query($sql, "Could not delete trans tax details");
	$sql = "DELETE FROM ".TB_PREF."exchange_rates WHERE date_ <= '$to'";
	db_query($sql, "Could not delete exchange rates");
	$sql = "DELETE FROM ".TB_PREF."budget_trans WHERE tran_date <= '$to'";
	db_query($sql, "Could not delete exchange rates");
	
	$sql = "SELECT account, SUM(amount) AS amount FROM ".TB_PREF."gl_trans WHERE tran_date <= '$to' GROUP by account";
	$result = db_query($sql, "Could not retrieve gl trans");
	while ($row = db_fetch($result))
	{
		$sql = "DELETE FROM ".TB_PREF."gl_trans WHERE tran_date <= '$to' AND account = '{$row['account']}'";
		db_query($sql, "Could not delete gl trans");
		if (is_account_balancesheet($row['account']))
		{
			$trans_no = get_next_trans_no(ST_JOURNAL);
			$sql = "INSERT INTO ".TB_PREF."gl_trans (type, type_no, tran_date, account, memo_, amount) VALUES
				(".ST_JOURNAL.", $trans_no, '$to', '{$row['account']}', '$ref', {$row['amount']})";
			db_query($sql, "Could not insert gl trans");
		}
	}
	
	$sql = "SELECT bank_act, SUM(amount) AS amount FROM ".TB_PREF."bank_trans WHERE trans_date <= '$to' GROUP BY bank_act";
	$result = db_query($sql, "Could not retrieve bank trans");
	while ($row = db_fetch($result))
	{
		$sql = "DELETE FROM ".TB_PREF."bank_trans WHERE trans_date <= '$to' AND bank_act = '{$row['bank_act']}'";
		db_query($sql, "Could not delete bank trans");
		$sql = "INSERT INTO ".TB_PREF."bank_trans (type, trans_no, trans_date, bank_act, ref, amount) VALUES
			(0, 0, '$to', '{$row['bank_act']}', '$ref', {$row['amount']})";
		db_query($sql, "Could not insert bank trans");
	}	
	
	$sql = "DELETE FROM ".TB_PREF."audit_trail WHERE gl_date <= '$to'";
	db_query($sql, "Could not delete audit trail");
	
	$sql = "SELECT type, id FROM ".TB_PREF."comments WHERE type != ".ST_SALESQUOTE." AND type != ".ST_SALESORDER." AND type != ".ST_PURCHORDER;
	$result = db_query($sql, "Could not retrieve comments");
	while ($row = db_fetch($result))
	{
		$sql = "SELECT count(*) FROM ".TB_PREF."gl_trans WHERE type = {$row['type']} AND type_no = {$row['id']}";
		$res = db_query($sql, "Could not retrieve gl_trans");
		$row2 = db_fetch_row($res);
		if ($row2[0] == 0) // if no link, then delete comments
		{
			$sql = "DELETE FROM ".TB_PREF."comments WHERE type = {$row['type']} AND id = {$row['id']}";
			db_query($sql, "Could not delete comments");
		}
	}	
	$sql = "SELECT type, id FROM ".TB_PREF."refs WHERE type != ".ST_SALESQUOTE." AND type != ".ST_SALESORDER." AND type != ".ST_PURCHORDER;
	$result = db_query($sql, "Could not retrieve refs");
	while ($row = db_fetch($result))
	{
		$sql = "SELECT count(*) FROM ".TB_PREF."gl_trans WHERE type = {$row['type']} AND type_no = {$row['id']}";
		$res = db_query($sql, "Could not retrieve gl_trans");
		$row2 = db_fetch_row($res);
		if ($row2[0] == 0) // if no link, then delete refs
		{
			$sql = "DELETE FROM ".TB_PREF."refs WHERE type = {$row['type']} AND id = {$row['id']}";
			db_query($sql, "Could not delete refs");
		}
	}	
		
	delete_fiscalyear($selected_id);
	commit_transaction();	
}

function handle_delete()
{
	global $selected_id, $Mode;

	if (check_can_delete($selected_id)) {
	//only delete if used in neither customer or supplier, comp prefs, bank trans accounts
		delete_this_fiscalyear($selected_id);
		display_notification(_('Selected fiscal year has been deleted'));
	}
	$Mode = 'RESET';
}

//---------------------------------------------------------------------------------------------

function display_fiscalyears()
{
	global $table_style;

	$company_year = get_company_pref('f_year');

	$result = get_all_fiscalyears();
	start_form();
	display_note(_("Warning: Deleting a fiscal year all transactions 
		are removed and converted into relevant balances. This process is irreversible!"), 
		0, 0, "class='currentfg'");
	start_table($table_style);

	$th = array(_("Fiscal Year Begin"), _("Fiscal Year End"), _("Closed"), "", "");
	table_header($th);

	$k=0;
	while ($myrow=db_fetch($result))
	{
    	if ($myrow['id'] == $company_year)
    	{
    		start_row("class='stockmankobg'");
    	}
    	else
    		alt_table_row_color($k);

		$from = sql2date($myrow["begin"]);
		$to = sql2date($myrow["end"]);
		if ($myrow["closed"] == 0)
		{
			$closed_text = _("No");
		}
		else
		{
			$closed_text = _("Yes");
		}
		label_cell($from);
		label_cell($to);
		label_cell($closed_text);
	 	edit_button_cell("Edit".$myrow['id'], _("Edit"));
		if ($myrow["id"] != $company_year) {
 			delete_button_cell("Delete".$myrow['id'], _("Delete"));
			submit_js_confirm("Delete".$myrow['id'],
				sprintf(_("Are you sure you want to delete fiscal year %s - %s? All transactions are deleted and converted into relevant balances. Do you want to continue ?"), $from, $to));
		} else
			label_cell('');
		end_row();
	}

	end_table();
	end_form();
	display_note(_("The marked fiscal year is the current fiscal year which cannot be deleted."), 0, 0, "class='currentfg'");
}

//---------------------------------------------------------------------------------------------

function display_fiscalyear_edit($selected_id)
{
	global $table_style2, $Mode;

	start_form();
	start_table($table_style2);

	if ($selected_id != -1)
	{
		if($Mode =='Edit')
		{
			$myrow = get_fiscalyear($selected_id);

			$_POST['from_date'] = sql2date($myrow["begin"]);
			$_POST['to_date']  = sql2date($myrow["end"]);
			$_POST['closed']  = $myrow["closed"];
		}
		hidden('from_date');
		hidden('to_date');
		label_row(_("Fiscal Year Begin:"), $_POST['from_date']);
		label_row(_("Fiscal Year End:"), $_POST['to_date']);
	}
	else
	{
		date_row(_("Fiscal Year Begin:"), 'from_date', '', null, 0, 0, 1001);
		date_row(_("Fiscal Year End:"), 'to_date', '', null, 0, 0, 1001);
	}
	hidden('selected_id', $selected_id);

	yesno_list_row(_("Is Closed:"), 'closed', null, "", "", false);

	end_table(1);

	submit_add_or_update_center($selected_id == -1, '', 'both');

	end_form();
}

//---------------------------------------------------------------------------------------------

if ($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM')
{
	handle_submit();
}

//---------------------------------------------------------------------------------------------

if ($Mode == 'Delete')
{
	global $selected_id;
	handle_delete($selected_id);
}

if ($Mode == 'RESET')
{
	$selected_id = -1;
}
//---------------------------------------------------------------------------------------------

display_fiscalyears();

echo '<br>';

display_fiscalyear_edit($selected_id);

//---------------------------------------------------------------------------------------------

end_page();

?>
