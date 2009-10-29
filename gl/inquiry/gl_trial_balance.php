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
$page_security = 'SA_GLANALYTIC';
$path_to_root="../..";

include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/gl/includes/gl_db.inc");

$js = "";
if ($use_date_picker)
	$js = get_js_date_picker();

page(_($help_context = "Trial Balance"), false, false, "", $js);

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

	submit_cells('Show',_("Show"),'','', 'default');
    end_table();
    end_form();
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
		$prev = get_balance($account["account_code"], 0, 0, $begin, $_POST['TransFromDate'], false, false);
		$curr = get_balance($account["account_code"], 0, 0, $_POST['TransFromDate'], $_POST['TransToDate'], true, true);
		$tot = get_balance($account["account_code"], 0, 0, $begin, $_POST['TransToDate'], false, true);
		if (check_value("NoZero") && !$prev['balance'] && !$curr['balance'] && !$tot['balance'])
			continue;
		alt_table_row_color($k);

		$url = "<a href='$path_to_root/gl/inquiry/gl_account_inquiry.php?TransFromDate=" . $_POST["TransFromDate"] . "&TransToDate=" . $_POST["TransToDate"] . "&account=" . $account["account_code"] . "'>" . $account["account_code"] . "</a>";

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

