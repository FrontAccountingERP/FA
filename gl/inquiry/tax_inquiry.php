<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU Affero General Public License,
	AGPL, as published by the Free Software Foundation, either version 
	3 of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/agpl-3.0.html>.
***********************************************************************/
$page_security = 8;
$path_to_root="../..";
include_once($path_to_root . "/includes/session.inc");


include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/gl/includes/gl_db.inc");

$js = '';
set_focus('account');
if ($use_popup_windows)
	$js .= get_js_open_window(800, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();

page(_("Tax Inquiry"), false, false, '', $js);

//----------------------------------------------------------------------------------------------------
// Ajax updates
//
if (get_post('Show')) 
{
	$Ajax->activate('trans_tbl');
}

if (get_post('TransFromDate') == "" && get_post('TransToDate') == "")
{
	$date = Today();
	$row = get_company_prefs();
	$edate = add_months($date, -$row['tax_last']);
	$edate = end_month($edate);
	$bdate = add_months($edate, -$row['tax_prd'] + 1);
	$_POST["TransFromDate"] = begin_month($bdate);
	$_POST["TransToDate"] = $edate;
}	

//----------------------------------------------------------------------------------------------------

function get_tax_types()
{
	$sql = "SELECT * FROM ".TB_PREF."tax_types ORDER BY id";
    return db_query($sql,"No transactions were returned");
}

function tax_inquiry_controls()
{
	global $table_style2;

    start_form();

    //start_table($table_style2);
    start_table("class='tablestyle_noborder'");
	start_row();

	date_cells(_("from:"), 'TransFromDate', '', null, -30);
	date_cells(_("to:"), 'TransToDate');
	submit_cells('Show',_("Show"),'','', true);

    end_row();

	end_table();

    end_form();
}

//----------------------------------------------------------------------------------------------------

function show_results()
{
	global $path_to_root, $table_style;

	$taxes = get_tax_types();

    /*Now get the transactions  */
	div_start('trans_tbl');
	start_table($table_style);

	$th = array(_("Type"), _("Description"), _("Amount"));
	table_header($th);
	$k = 0;
	$total = 0;
	$bdate = date2sql($_POST['TransFromDate']);
	$edate  = date2sql($_POST['TransToDate']);
	while ($tx = db_fetch($taxes))
	{
		if ($tx['sales_gl_code'] == $tx['purchasing_gl_code'])
		{
			$sql = "SELECT SUM(IF(amount >= 0, amount, 0)) AS payable, SUM(IF(amount < 0, -amount, 0)) AS collectible
				FROM ".TB_PREF."gl_trans WHERE account = '".$tx['sales_gl_code']."' AND tran_date >= '$bdate' AND tran_date <= '$edate'";
			$result = db_query($sql, "Error retrieving tax inquiry");
			$row = db_fetch($result);
			$payable = -$row['payable'];
			$collectible.= -$row['collectible'];
		}
		else
		{
			$sql = "SELECT SUM(amount) AS collectible
				FROM ".TB_PREF."gl_trans WHERE account = '".$tx['sales_gl_code']."' AND tran_date >= '$bdate' AND tran_date <= '$edate'";
			$result = db_query($sql, "Error retrieving tax inquiry");
			$row = db_fetch($result);
			$collectible = -$row['collectible'];
			$sql = "SELECT SUM(amount) AS payable
				FROM ".TB_PREF."gl_trans WHERE account = '".$tx['purchasing_gl_code']."' AND tran_date >= '$bdate' AND tran_date <= '$edate'";
			$result = db_query($sql, "Error retrieving tax inquiry");
			$row = db_fetch($result);
			$payable = -$row['payable'];
		}
		$net = $collectible + $payable;
		$total += $net;
		alt_table_row_color($k);
		label_cell($tx['name'] . " " . $tx['rate'] . "%");
		label_cell(_("Charged on sales") . " (" . _("Output Tax")."):");
		amount_cell($collectible);
		end_row();
		alt_table_row_color($k);
		label_cell($tx['name'] . " " . $tx['rate'] . "%");
		label_cell(_("Paid on purchases") . " (" . _("Input Tax")."):");
		amount_cell($payable);
		end_row();
		alt_table_row_color($k);
		label_cell("<b>".$tx['name'] . " " . $tx['rate'] . "%</b>");
		label_cell("<b>"._("Net payable or collectible") . ":</b>");
		amount_cell($net, true);
		end_row();
	}	
	alt_table_row_color($k);
	label_cell("");
	label_cell("<b>"._("Total payable or refund") . ":</b>");
	amount_cell($total, true);
	end_row();

	end_table(2);
	div_end();
}

//----------------------------------------------------------------------------------------------------

tax_inquiry_controls();

show_results();

//----------------------------------------------------------------------------------------------------

end_page();

?>
