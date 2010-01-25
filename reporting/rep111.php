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
$page_security = $_POST['PARAM_0'] == $_POST['PARAM_1'] ?
	'SA_SALESTRANSVIEW' : 'SA_SALESBULKREP';
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	Print Sales Quotations
// ----------------------------------------------------------------
$path_to_root="..";

include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/data_checks.inc");
include_once($path_to_root . "/sales/includes/sales_db.inc");

//----------------------------------------------------------------------------------------------------

print_sales_quotations();

function print_sales_quotations()
{
	global $path_to_root, $print_as_quote;

	include_once($path_to_root . "/reporting/includes/pdf_report.inc");

	$from = $_POST['PARAM_0'];
	$to = $_POST['PARAM_1'];
	$currency = $_POST['PARAM_2'];
	$email = $_POST['PARAM_3'];
	$comments = $_POST['PARAM_4'];

	if ($from == null)
		$from = 0;
	if ($to == null)
		$to = 0;
	$dec = user_price_dec();

	$cols = array(4, 60, 225, 300, 325, 385, 450, 515);

	// $headers in doctext.inc
	$aligns = array('left',	'left',	'right', 'left', 'right', 'right', 'right');

	$params = array('comments' => $comments);

	$cur = get_company_Pref('curr_default');

	if ($email == 0)
	{
		$rep = new FrontReport(_("SALES QUOTATION"), "SalesQuotationBulk", user_pagesize());
		$rep->currency = $cur;
		$rep->Font();
		$rep->Info($params, $cols, null, $aligns);
	}

	for ($i = $from; $i <= $to; $i++)
	{
		$myrow = get_sales_order_header($i, ST_SALESQUOTE);
		$baccount = get_default_bank_account($myrow['curr_code']);
		$params['bankaccount'] = $baccount['id'];
		$branch = get_branch($myrow["branch_code"]);
		if ($email == 1)
		{
			$rep = new FrontReport("", "", user_pagesize());
			$rep->currency = $cur;
			$rep->Font();
			$rep->filename = "SalesQuotation" . $i . ".pdf";
			$rep->Info($params, $cols, null, $aligns);
		}
		$rep->title = _("SALES QUOTATION");
		$rep->Header2($myrow, $branch, $myrow, $baccount, ST_SALESQUOTE);

		$result = get_sales_order_details($i, ST_SALESQUOTE);
		$SubTotal = 0;
		while ($myrow2=db_fetch($result))
		{
			$Net = round2(((1 - $myrow2["discount_percent"]) * $myrow2["unit_price"] * $myrow2["quantity"]),
			   user_price_dec());
			$SubTotal += $Net;
			$DisplayPrice = number_format2($myrow2["unit_price"],$dec);
			$DisplayQty = number_format2($myrow2["quantity"],get_qty_dec($myrow2['stk_code']));
			$DisplayNet = number_format2($Net,$dec);
			if ($myrow2["discount_percent"]==0)
				$DisplayDiscount ="";
			else
				$DisplayDiscount = number_format2($myrow2["discount_percent"]*100,user_percent_dec()) . "%";
			$rep->TextCol(0, 1,	$myrow2['stk_code'], -2);
			$oldrow = $rep->row;
			$rep->TextColLines(1, 2, $myrow2['description'], -2);
			$newrow = $rep->row;
			$rep->row = $oldrow;
			$rep->TextCol(2, 3,	$DisplayQty, -2);
			$rep->TextCol(3, 4,	$myrow2['units'], -2);
			$rep->TextCol(4, 5,	$DisplayPrice, -2);
			$rep->TextCol(5, 6,	$DisplayDiscount, -2);
			$rep->TextCol(6, 7,	$DisplayNet, -2);
			$rep->row = $newrow;
			//$rep->NewLine(1);
			if ($rep->row < $rep->bottomMargin + (15 * $rep->lineHeight))
				$rep->Header2($myrow, $branch, $myrow, $baccount, ST_SALESQUOTE);
		}
		if ($myrow['comments'] != "")
		{
			$rep->NewLine();
			$rep->TextColLines(1, 5, $myrow['comments'], -2);
		}
		$DisplaySubTot = number_format2($SubTotal,$dec);
		$DisplayFreight = number_format2($myrow["freight_cost"],$dec);

		$rep->row = $rep->bottomMargin + (15 * $rep->lineHeight);
		$linetype = true;
		$doctype = ST_SALESQUOTE;
		if ($rep->currency != $myrow['curr_code'])
		{
			include($path_to_root . "/reporting/includes/doctext2.inc");
		}
		else
		{
			include($path_to_root . "/reporting/includes/doctext.inc");
		}

		$rep->TextCol(3, 6, $doc_Sub_total, -2);
		$rep->TextCol(6, 7,	$DisplaySubTot, -2);
		$rep->NewLine();
		$rep->TextCol(3, 6, $doc_Shipping, -2);
		$rep->TextCol(6, 7,	$DisplayFreight, -2);
		$rep->NewLine();
		$DisplayTotal = number_format2($myrow["freight_cost"] + $SubTotal, $dec);
		$rep->Font('bold');
		if ($myrow['tax_included'] == 0)
			$rep->TextCol(3, 6, $doc_TOTAL_ORDER, - 2);
		else	
			$rep->TextCol(3, 6, $doc_TOTAL_ORDER2, - 2);
		$rep->TextCol(6, 7,	$DisplayTotal, -2);
		$words = price_in_words($myrow["freight_cost"] + $SubTotal, ST_SALESQUOTE);
		if ($words != "")
		{
			$rep->NewLine(1);
			$rep->TextCol(1, 7, $myrow['curr_code'] . ": " . $words, - 2);
		}	
		$rep->Font();
		if ($email == 1)
		{
			if ($myrow['contact_email'] == '')
			{
				$myrow['contact_email'] = $branch['email'];
				if ($myrow['contact_email'] == '')
					$myrow['contact_email'] = $myrow['master_email'];
				$myrow['DebtorName'] = $branch['br_name'];
			}
			//$myrow['reference'] = $i;
			$rep->End($email, $doc_Invoice_no . " " . $i, $myrow);
		}
	}
	if ($email == 0)
		$rep->End();
}

?>