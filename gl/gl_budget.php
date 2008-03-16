<?php

$page_security = 10;
$path_to_root="..";
include($path_to_root . "/includes/session.inc");

page(_("Budget Entry"));

include($path_to_root . "/includes/ui.inc");
include($path_to_root . "/gl/includes/gl_db.inc");
include_once($path_to_root . "/includes/data_checks.inc");

check_db_has_gl_account_groups(_("There are no account groups defined. Please define at least one account group before entering accounts."));

//-------------------------------------------------------------------------------------

function exists_gl_budget($date_, $account, $dimension, $dimension2)
{
	$sql = "SELECT account FROM ".TB_PREF."budget_trans WHERE account='$account' AND tran_date='$date_' AND
		dimension_id=$dimension AND dimension2_id=$dimension2";
	$result = db_query($sql, "Cannot retreive a gl transaction");

    return (db_num_rows($result) > 0);
}

function add_update_gl_budget_trans($date_, $account, $dimension, $dimension2, $amount)
{
	$date = date2sql($date_);
		
	if (exists_gl_budget($date, $account, $dimension, $dimension2))
		$sql = "UPDATE ".TB_PREF."budget_trans SET amount=$amount WHERE account='$account' AND
			dimension_id=$dimension AND dimension2_id=$dimension2 AND tran_date='$date'";
	else
		$sql = "INSERT INTO ".TB_PREF."budget_trans (tran_date,
			account, dimension_id, dimension2_id, amount) VALUES ('$date',
			'$account', $dimension, $dimension2, $amount)";

	db_query($sql, "The GL budget transaction could not be saved");
}

function delete_gl_budget_trans($date_, $account, $dimension, $dimension2)
{
	$date = date2sql($date_);
		
	$sql = "DELETE FROM ".TB_PREF."budget_trans WHERE account='$account' AND
			dimension_id=$dimension AND dimension2_id=$dimension2 AND tran_date='$date'";
	db_query($sql, "The GL budget transaction could not be deleted");
}

function get_only_budget_trans_from_to($from_date, $to_date, $account, $dimension=0, $dimension2=0) 
{

	$from = date2sql($from_date);
	$to = date2sql($to_date);
	
	$sql = "SELECT SUM(amount) FROM ".TB_PREF."budget_trans
		WHERE account='$account' AND tran_date >= '$from' AND tran_date <= '$to'
		 AND dimension_id = $dimension AND dimension2_id = $dimension2";
	$result = db_query($sql,"No budget accounts were returned");

	$row = db_fetch_row($result);
	return $row[0];
}

//-------------------------------------------------------------------------------------

if (isset($_POST['add']) || isset($_POST['delete'])) 
{
	begin_transaction();
	
	for ($i = 0, $da = $_POST['begin']; date1_greater_date2($_POST['end'], $da); $i++)
	{
		if (isset($_POST['add']))
			add_update_gl_budget_trans($da, $_POST['account'], $_POST['dim1'], $_POST['dim2'], input_num('amount'.$i));
		else	
			delete_gl_budget_trans($da, $_POST['account'], $_POST['dim1'], $_POST['dim2']);
		$da = add_months($da, 1);
	}
	commit_transaction();
	
	if (isset($_POST['add']))
		display_notification_centered(_("The Budget has been saved."));
	else	
		display_notification_centered(_("The Budget has been deleted."));
	
	//meta_forward($_SERVER['PHP_SELF']);    	
}	

//-------------------------------------------------------------------------------------

start_form();

if (db_has_gl_accounts()) 
{
	$dim = get_company_pref('use_dimension');
	start_table($table_style2);
	fiscalyears_list_row(_("Fiscal Year:"), 'fyear', null);
	gl_all_accounts_list_row(_("Account Code:"), 'account', null);
	if (!isset($_POST['dim1']))	
		$_POST['dim1'] = 0;	
	if (!isset($_POST['dim2']))	
		$_POST['dim2'] = 0;	
    if ($dim == 2)
    {
		dimensions_list_row(_("Dimension")." 1", 'dim1', $_POST['dim1'], true, null, false, 1);
		dimensions_list_row(_("Dimension")." 2", 'dim2', $_POST['dim2'], true, null, false, 2);
	}
	else if ($dim == 1)
	{
		dimensions_list_row(_("Dimension"), 'dim1', $_POST['dim1'], true, null, false, 1);
		hidden('dim2', 0);
	}
	else
	{
		hidden('dim1', 0);
		hidden('dim2', 0);
	}
	submit_row('submit', _("Get"));
	end_table(1);
	start_table($table_style2);
	$showdims = (($dim == 1 && $_POST['dim1'] == 0) || 
		($dim == 2 && $_POST['dim1'] == 0 && $_POST['dim2'] == 0));
	if ($showdims)	
		$th = array(_("Period"), _("Amount"), _("Dim. incl."), _("Last Year"));
	else	
		$th = array(_("Period"), _("Amount"), _("Last Year"));
	table_header($th);	
	$year = $_POST['fyear'];
	$sql = "SELECT * FROM ".TB_PREF."fiscal_year WHERE id=$year";

	$result = db_query($sql, "could not get current fiscal year");

	$fyear = db_fetch($result);
	$begin = sql2date($fyear['begin']);
	$end = sql2date($fyear['end']);
	hidden('begin', $begin);
	hidden('end', $end);
	$total = $btotal = $ltotal = 0;
	for ($i = 0, $date_ = $begin; date1_greater_date2($end, $date_); $i++)
	{
		start_row();
		$_POST['amount'.$i] = number_format2(get_only_budget_trans_from_to($date_, $date_, $_POST['account'], $_POST['dim1'], $_POST['dim2']), 0); 
		
		label_cell($date_);	
		if (!isset($_POST['amount'.$i]))
			$_POST['amount'.$i] = '0';
		amount_cells(null, 'amount'.$i, null, 15, null, 0);
		if ($showdims)
		{
			$d = get_budget_trans_from_to($date_, $date_, $_POST['account'], $_POST['dim1'], $_POST['dim2']);
			label_cell(number_format2($d, 0), "nowrap align=right");
			$btotal += $d;
		}	
		$lamount = get_gl_trans_from_to(add_years($date_, -1), add_years(end_month($date_), -1), $_POST['account'], $_POST['dim1'], $_POST['dim2']);
		$total += input_num('amount'.$i);
		$ltotal += $lamount;
		label_cell(number_format2($lamount, 0), "nowrap align=right");
		$date_ = add_months($date_, 1);
		end_row();
	}
	start_row();
	label_cell("<b>"._("Total")."</b>");
	label_cell("<b>".number_format2($total, 0)."</b>", 'align=right');
	if ($showdims)
		label_cell("<b>".number_format2($btotal, 0)."</b>", "nowrap align=right");
	label_cell("<b>".number_format2($ltotal, 0)."</b>", "nowrap align=right");
	end_row();
	end_table(1);
	submit_center_first('add', _("Save"));
	submit_center_last('delete', _("Delete"));
} 
end_form();
	
end_page();
	
?>
