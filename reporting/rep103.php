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
$page_security = 'SA_CUSTBULKREP';
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	Customer Details Listing
// ----------------------------------------------------------------
$path_to_root="..";

include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/data_checks.inc");
include_once($path_to_root . "/gl/includes/gl_db.inc");

//----------------------------------------------------------------------------------------------------

print_customer_details_listing();

function get_customer_details_for_report($area=0, $salesid=0)
{
	$sql = "SELECT ".TB_PREF."debtors_master.debtor_no,
			".TB_PREF."debtors_master.name,
			".TB_PREF."debtors_master.address,
			".TB_PREF."debtors_master.curr_code,
			".TB_PREF."debtors_master.dimension_id,
			".TB_PREF."debtors_master.dimension2_id,
			".TB_PREF."sales_types.sales_type,
			".TB_PREF."cust_branch.branch_code,
			".TB_PREF."cust_branch.br_name,
			".TB_PREF."cust_branch.br_address,
			".TB_PREF."cust_branch.br_post_address,
			".TB_PREF."cust_branch.contact_name,
			".TB_PREF."cust_branch.area,
			".TB_PREF."cust_branch.salesman,
			".TB_PREF."areas.description,
			".TB_PREF."salesman.salesman_name
		FROM ".TB_PREF."debtors_master
		INNER JOIN ".TB_PREF."cust_branch
			ON ".TB_PREF."debtors_master.debtor_no=".TB_PREF."cust_branch.debtor_no
		INNER JOIN ".TB_PREF."sales_types
			ON ".TB_PREF."debtors_master.sales_type=".TB_PREF."sales_types.id
		INNER JOIN ".TB_PREF."areas
			ON ".TB_PREF."cust_branch.area = ".TB_PREF."areas.area_code
		INNER JOIN ".TB_PREF."salesman
			ON ".TB_PREF."cust_branch.salesman=".TB_PREF."salesman.salesman_code";
	if ($area != 0)
	{
		if ($salesid != 0)
			$sql .= " WHERE ".TB_PREF."salesman.salesman_code=".db_escape($salesid)."
				AND ".TB_PREF."areas.area_code=".db_escape($area);
		else
			$sql .= " WHERE ".TB_PREF."areas.area_code=".db_escape($area);
	}
	elseif ($salesid != 0)
		$sql .= " WHERE ".TB_PREF."salesman.salesman_code=".db_escape($salesid);
	$sql .= " ORDER BY description,
			".TB_PREF."salesman.salesman_name,
			".TB_PREF."debtors_master.debtor_no,
			".TB_PREF."cust_branch.branch_code";

    return db_query($sql,"No transactions were returned");
}

function get_contacts_for_branch($branch)
{
	$sql = "SELECT p.*, r.action, r.type, CONCAT(r.type,'.',r.action) as ext_type 
		FROM ".TB_PREF."crm_persons p,".TB_PREF."crm_contacts r WHERE r.person_id=p.id AND r.type='cust_branch' 
			AND r.entity_id=".db_escape($branch);
	$res = db_query($sql, "can't retrieve branch contacts");
	$results = array();
	while($contact = db_fetch($res))
		$results[] = $contact;
	return $results;
}

function getTransactions($debtorno, $branchcode, $date)
{
	$date = date2sql($date);

	$sql = "SELECT SUM((ov_amount+ov_freight+ov_discount)*rate) AS Turnover
		FROM ".TB_PREF."debtor_trans
		WHERE debtor_no=".db_escape($debtorno)."
		AND branch_code=".db_escape($branchcode)."
		AND (type=".ST_SALESINVOICE." OR type=".ST_CUSTCREDIT.")
		AND tran_date >='$date'";

    $result = db_query($sql,"No transactions were returned");

	$row = db_fetch_row($result);
	return $row[0];
}

//----------------------------------------------------------------------------------------------------

function print_customer_details_listing()
{
    global $path_to_root;

    $from = $_POST['PARAM_0'];
    $area = $_POST['PARAM_1'];
    $folk = $_POST['PARAM_2'];
    $more = $_POST['PARAM_3'];
    $less = $_POST['PARAM_4'];
    $comments = $_POST['PARAM_5'];
	$orientation = $_POST['PARAM_6'];
	$destination = $_POST['PARAM_7'];
	if ($destination)
		include_once($path_to_root . "/reporting/includes/excel_report.inc");
	else
		include_once($path_to_root . "/reporting/includes/pdf_report.inc");

	$orientation = ($orientation ? 'L' : 'P');
    $dec = 0;

	if ($area == ALL_NUMERIC)
		$area = 0;
	if ($folk == ALL_NUMERIC)
		$folk = 0;

	if ($area == 0)
		$sarea = _('All Areas');
	else
		$sarea = get_area_name($area);
	if ($folk == 0)
		$salesfolk = _('All Sales Folk');
	else
		$salesfolk = get_salesman_name($folk);
	if ($more != '')
		$morestr = _('Greater than ') . number_format2($more, $dec);
	else
		$morestr = '';
	if ($less != '')
		$lessstr = _('Less than ') . number_format2($less, $dec);
	else
		$lessstr = '';

	$more = (double)$more;
	$less = (double)$less;

	$cols = array(0, 150, 300, 425, 550);

	$headers = array(_('Customer Postal Address'), _('Price/Turnover'),	_('Branch Contact Information'),
		_('Branch Delivery Address'));

	$aligns = array('left',	'left',	'left',	'left');

    $params =   array( 	0 => $comments,
    				    1 => array('text' => _('Activity Since'), 	'from' => $from, 		'to' => ''),
    				    2 => array('text' => _('Sales Areas'), 		'from' => $sarea, 		'to' => ''),
    				    3 => array('text' => _('Sales Folk'), 		'from' => $salesfolk, 	'to' => ''),
    				    4 => array('text' => _('Activity'), 		'from' => $morestr, 	'to' => $lessstr . " " . get_company_pref("curr_default")));

    $rep = new FrontReport(_('Customer Details Listing'), "CustomerDetailsListing", user_pagesize(), 9, $orientation);
    if ($orientation == 'L')
    	recalculate_cols($cols);

    $rep->Font();
    $rep->Info($params, $cols, $headers, $aligns);
    $rep->NewPage();

	$result = get_customer_details_for_report($area, $folk);

	$carea = '';
	$sman = '';
	while ($myrow=db_fetch($result))
	{
		$printcustomer = true;
		if ($more != '' || $less != '')
		{
			$turnover = getTransactions($myrow['debtor_no'], $myrow['branch_code'], $from);
			if ($more != 0.0 && $turnover <= (double)$more)
				$printcustomer = false;
			if ($less != 0.0 && $turnover >= (double)$less)
				$printcustomer = false;
		}
		if ($printcustomer)
		{
			if ($carea != $myrow['description'])
			{
				$rep->fontSize += 2;
				$rep->NewLine(2, 7);
				$rep->Font('bold');
				$rep->TextCol(0, 3,	_('Customers in') . " " . $myrow['description']);
				$carea = $myrow['description'];
				$rep->fontSize -= 2;
				$rep->Font();
				$rep->NewLine();
			}
			if ($sman != $myrow['salesman_name'])
			{
				$rep->fontSize += 2;
				$rep->NewLine(1, 7);
				$rep->Font('bold');
				$rep->TextCol(0, 3,	$myrow['salesman_name']);
				$sman = $myrow['salesman_name'];
				$rep->fontSize -= 2;
				$rep->Font();
				$rep->NewLine();
			}
			$rep->NewLine();
			// Here starts the new report lines 2010-11-02 Joe Hunt
			$contacts = get_contacts_for_branch($myrow['branch_code']);
			$rep->TextCol(0, 1,	$myrow['name']);
			$rep->TextCol(1, 2,	_('Price List') . ": " . $myrow['sales_type']);
			$rep->TextCol(2, 3,	$myrow['br_name']);
			$rep->NewLine();
			$adr = Explode("\n", $myrow['address']);
			if ($myrow['br_post_address'] == '')
				$adr2 = Explode("\n", $myrow['br_address']);
			else
				$adr2 = Explode("\n", $myrow['br_post_address']);
			$count1 = count($adr);
			$count2 = count($adr2);
			$count1 = max($count1, $count2);
			$count1 = max($count1, 4); 
			if (isset($adr[0]))
				$rep->TextCol(0, 1, $adr[0]);
			$rep->TextCol(1, 2,	_('Currency') . ": " . $myrow['curr_code']);
			if (isset($contacts[0]))
				$rep->TextCol(2, 3, $contacts[0]['name']. " " .$contacts[0]['name2']);
			if (isset($adr2[0]))	
				$rep->TextCol(3, 4, $adr2[0]);
			$rep->NewLine();
			if (isset($adr[1]))
				$rep->TextCol(0, 1, $adr[1]);
			if ($myrow['dimension_id'] != 0)
			{
				$dim = get_dimension($myrow['dimension_id']);
				$rep->TextCol(1, 2,	_('Dimension') . ": " . $dim['name']);
			}		
			if (isset($contacts[0]))
				$rep->TextCol(2, 3, _('Ph') . ": " . $contacts[0]['phone']);
			if (isset($adr2[1]))
				$rep->TextCol(3, 4, $adr2[1]);
			$rep->NewLine();
			if (isset($adr[2]))
				$rep->TextCol(0, 1, $adr[2]);
			if ($myrow['dimension2_id'] != 0)
			{
				$dim = get_dimension($myrow['dimension2_id']);
				$rep->TextCol(1, 2,	_('Dimension') . " 2: " . $dim['name']);
			}	
			if (isset($contacts[0]))
				$rep->TextCol(2, 3, _('Fax') . ": " . $contacts[0]['fax']);
			if (isset($adr2[2]))
				$rep->TextCol(3, 4, $adr2[2]);
			if ($more != 0.0 || $less != 0.0)
				$rep->TextCol(1, 2,	_('Turnover') . ": " . number_format2($turnover, $dec));
			for ($i = 3; $i < $count1; $i++)
			{
				$rep->NewLine();
				if (isset($adr[$i]))
					$rep->TextCol(0, 1, $adr[$i]);
				if ($i == 3 && isset($contacts[0]) && isset($contacts[0]['email']))	
					$rep->TextCol(2, 3, _('Email') . ": " . $contacts[0]['email']);
				if (isset($adr2[$i]))
					$rep->TextCol(3, 4, $adr2[$i]);
			}	
			$rep->NewLine();
			/*
			$rep->TextCol(0, 1,	$myrow['name']);
			$adr = Explode("\n", $myrow['address']);
			$count1 = count($adr);
			for ($i = 0; $i < $count1; $i++)
				$rep->TextCol(0, 1, $adr[$i], 0, ($i + 1) * $rep->lineHeight);
			$count1++;
			$rep->TextCol(1, 2,	_('Price List') . ": " . $myrow['sales_type']);
			if ($more != 0.0 || $less != 0.0)
				$rep->TextCol(1, 2,	_('Turnover') . ": " . number_format2($turnover, $dec), 0, $rep->lineHeight);
			$rep->TextCol(2, 3,	$myrow['br_name']);

			$contacts = get_branch_contacts($myrow['branch_code']);
			if (isset($contacts[0]))
			{
				$rep->TextCol(2, 3, $contacts[0]['name'], 0, $rep->lineHeight);
				$rep->TextCol(2, 3, _('Ph') . ": " . $contacts[0]['phone'], 0, 2 * $rep->lineHeight);
				$rep->TextCol(2, 3, _('Fax') . ": " . $contacts[0]['fax'], 0, 3 * $rep->lineHeight);
			}
			$adr = Explode("\n", $myrow['br_address']);
			$count2 = count($adr);
			for ($i = 0; $i < $count2; $i++)
				$rep->TextCol(3, 4, $adr[$i], 0, ($i + 1) * $rep->lineHeight);
			//$rep->TextCol(3, 4, $myrow['email'], 0, ($count2 + 1) * $rep->lineHeight);
			$count2++;
			$count1 = Max($count1, $count2);
			$count1 = Max($count1, 4);
			$rep->NewLine($count3);
			*/
			$rep->Line($rep->row + 8);
			$rep->NewLine(0, 3);
		}
	}
    $rep->End();
}

?>