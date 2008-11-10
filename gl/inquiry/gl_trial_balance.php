<?php

$page_security = 8;
$path_to_root="../..";

include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/gl/includes/gl_db.inc");

$js = "";
if ($use_date_picker)
	$js = get_js_date_picker();

page(_("Trial Balance"), false, false, "", $js);

//----------------------------------------------------------------------------------------------------
// Ajax updates
//
if (get_post('Show')) 
{
	$Ajax->activate('balance_tbl');
}


function gl_inquiry_controls()
{
    start_form();

    start_table("class='tablestyle_noborder'");

    date_cells(_("From:"), 'TransFromDate', '', null, -30);
	date_cells(_("To:"), 'TransToDate');
	check_cells(_("No zero values"), 'NoZero', null);
	check_cells(_("Only balances"), 'Balance', null);

	submit_cells('Show',_("Show"),'','', true);
    end_table();
    end_form();
}

//----------------------------------------------------------------------------------------------------

function get_balance($account, $from, $to, $from_incl=true, $to_incl=true) 
{
	$sql = "SELECT SUM(IF(amount >= 0, amount, 0)) as debit, SUM(IF(amount < 0, -amount, 0)) as credit, SUM(amount) as balance 
		FROM ".TB_PREF."gl_trans,".TB_PREF."chart_master,".TB_PREF."chart_types, ".TB_PREF."chart_class 
		WHERE ".TB_PREF."gl_trans.account=".TB_PREF."chart_master.account_code AND ".TB_PREF."chart_master.account_type=".TB_PREF."chart_types.id 
		AND ".TB_PREF."chart_types.class_id=".TB_PREF."chart_class.cid AND";
		
	if ($account != null)
		$sql .= " account='$account' AND";
	$from_date = date2sql($from);
	if ($from_incl)
		$sql .= " tran_date >= '$from_date'  AND";
	else
		$sql .= " tran_date > IF(".TB_PREF."chart_class.balance_sheet=1, '0000-00-00', '$from_date') AND";
	$to_date = date2sql($to);
	if ($to_incl)
		$sql .= " tran_date <= '$to_date' ";
	else
		$sql .= " tran_date < '$to_date' ";

	$result = db_query($sql,"No general ledger accounts were returned");

	return db_fetch($result);
}

//----------------------------------------------------------------------------------------------------

function display_trial_balance()
{
	global $table_style, $path_to_root;

	div_start('balance_tbl');
	start_table($table_style);
	$tableheader =  "<tr>
        <td rowspan=2 class='tableheader'>" . _("Account") . "</td>
        <td rowspan=2 class='tableheader'>" . _("Account Name") . "</td>
		<td colspan=2 class='tableheader'>" . _("Brought Forward") . "</td>
		<td colspan=2 class='tableheader'>" . _("This Period") . "</td>
		<td colspan=2 class='tableheader'>" . _("Balance") . "</td>
		</tr><tr>
		<td class='tableheader'>" . _("Debit") . "</td>
        <td class='tableheader'>" . _("Credit") . "</td>
		<td class='tableheader'>" . _("Debit") . "</td>
		<td class='tableheader'>" . _("Credit") . "</td>
        <td class='tableheader'>" . _("Debit") . "</td>
        <td class='tableheader'>" . _("Credit") . "</td>
        </tr>";

    echo $tableheader;

	$k = 0;

	$accounts = get_gl_accounts();
	$pdeb = $pcre = $cdeb = $ccre = $tdeb = $tcre = $pbal = $cbal = $tbal = 0;
	$begin = begin_fiscalyear();
	if (date1_greater_date2($begin, $_POST['TransFromDate']))
		$begin = $_POST['TransFromDate'];
	$begin = add_days($begin, -1);
	
	while ($account = db_fetch($accounts))
	{
		$prev = get_balance($account["account_code"], $begin, $_POST['TransFromDate'], false, false);
		$curr = get_balance($account["account_code"], $_POST['TransFromDate'], $_POST['TransToDate'], true, true);
		$tot = get_balance($account["account_code"], $begin, $_POST['TransToDate'], false, true);
		if (check_value("NoZero") && !$prev['balance'] && !$curr['balance'] && !$tot['balance'])
			continue;
		alt_table_row_color($k);

		$url = "<a href='$path_to_root/gl/inquiry/gl_account_inquiry.php?" . SID . "TransFromDate=" . $_POST["TransFromDate"] . "&TransToDate=" . $_POST["TransToDate"] . "&account=" . $account["account_code"] . "'>" . $account["account_code"] . "</a>";

		label_cell($url);
		label_cell($account["account_name"]);
		if (check_value('Balance'))
		{
			display_debit_or_credit_cells($prev['balance']);
			display_debit_or_credit_cells($curr['balance']);
			display_debit_or_credit_cells($tot['balance']);
			
		}
		else
		{
			amount_cell($prev['debit']);
			amount_cell($prev['credit']);
			amount_cell($curr['debit']);
			amount_cell($curr['credit']);
			amount_cell($tot['debit']);
			amount_cell($tot['credit']);
			$pdeb += $prev['debit'];
			$pcre += $prev['credit'];
			$cdeb += $curr['debit'];
			$ccre += $curr['credit'];
			$tdeb += $tot['debit'];
			$tcre += $tot['credit'];
		}	
		$pbal += $prev['balance'];
		$cbal += $curr['balance'];
		$tbal += $tot['balance'];
		end_row();
	}

	//$prev = get_balance(null, $begin, $_POST['TransFromDate'], false, false);
	//$curr = get_balance(null, $_POST['TransFromDate'], $_POST['TransToDate'], true, true);
	//$tot = get_balance(null, $begin, $_POST['TransToDate'], false, true);
	if (!check_value('Balance'))
	{
		start_row("class='inquirybg' style='font-weight:bold'");
		label_cell(_("Total") ." - ".$_POST['TransToDate'], "colspan=2");
		amount_cell($pdeb);
		amount_cell($pcre);
		amount_cell($cdeb);
		amount_cell($ccre);
		amount_cell($tdeb);
		amount_cell($tcre);
		end_row();
	}	
	start_row("class='inquirybg' style='font-weight:bold'");
	label_cell(_("Ending Balance") ." - ".$_POST['TransToDate'], "colspan=2");
	display_debit_or_credit_cells($pbal);
	display_debit_or_credit_cells($cbal);
	display_debit_or_credit_cells($tbal);
	end_row();

	end_table(1);
	div_end();
}

//----------------------------------------------------------------------------------------------------

gl_inquiry_controls();

display_trial_balance();

//----------------------------------------------------------------------------------------------------

end_page();

?>

