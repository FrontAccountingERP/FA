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
//------------------------------------------------------------------------------
//	Retreive parent document number(s) for given transaction
//
function get_parent_trans($trans_type, $trans_no) {

	$sql = 'SELECT trans_link FROM
			'.TB_PREF.'debtor_trans WHERE
			(trans_no='.db_escape($trans_no).' AND type='.db_escape($trans_type).' AND trans_link!=0)';

	$result = db_query($sql, 'Parent document numbers cannot be retrieved');

	if (db_num_rows($result)) {
		$link = db_fetch($result);
		return array($link['trans_link']);
	}
	if ($trans_type!=ST_SALESINVOICE) return 0;	// this is credit note with no parent invoice
	// invoice: find batch invoice parent trans.
	$sql = 'SELECT trans_no FROM
			'.TB_PREF.'debtor_trans WHERE
			(trans_link='.db_escape($trans_no).' AND type='. get_parent_type($trans_type) .')';

	$result = db_query($sql, 'Delivery links cannot be retrieved');

	$delivery = array();
	if(db_num_rows($result)>0) {
		while($link = db_fetch($result)) {
			$delivery[] = $link['trans_no'];
		}
	}
	return count($delivery) ? $delivery : 0;
}

//----------------------------------------------------------------------------------------
// Mark changes in debtor_trans_details
//
function update_customer_trans_version($type, $versions) {

	$sql= 'UPDATE '.TB_PREF. 'debtor_trans SET version=version+1
			WHERE type='.db_escape($type).' AND (';

	foreach ($versions as $trans_no=>$version)
		$where[] = 	'(trans_no='.db_escape($trans_no).' AND version='.$version.')';

		$sql .= implode(' OR ', $where) .')';

	return  db_query($sql, 'Concurrent editing conflict');
}

//----------------------------------------------------------------------------------------
// Gets document header versions for transaction set of type $type
// $trans_no = array(num1, num2,...);
// returns array(num1=>ver1, num2=>ver2...)
//
function get_customer_trans_version($type, $trans_no) {
	if (!is_array($trans_no))
		$trans_no = array( $trans_no );

	$sql= 'SELECT trans_no, version FROM '.TB_PREF. 'debtor_trans
			WHERE type='.db_escape($type).' AND (';

	foreach ($trans_no as $key=>$trans)
		$trans_no[$key] = 	'trans_no='.$trans_no[$key];

	$sql .= implode(' OR ', $trans_no) . ')';

	$res = db_query($sql, 'document version retreival');

	$vers = array();
	while($mysql=db_fetch($res)) {
		$vers[$mysql['trans_no']] = $mysql['version'];
	}
	return $vers;
}
//----------------------------------------------------------------------------------------
// $Total, $Tax, $Freight, $discount all in customer's currency
// date_ is display date (non-sql)
function write_customer_trans($trans_type, $trans_no, $debtor_no, $BranchNo,
	$date_, $reference, $Total, $discount=0, $Tax=0, $Freight=0, $FreightTax=0,
	$sales_type=0, $order_no=0, $trans_link=0, $ship_via=0, $due_date="",
	$AllocAmt=0, $rate=0, $dimension_id=0, $dimension2_id=0)
{
	$new = $trans_no==0;
	$curr = get_customer_currency($debtor_no);
	if ($rate == 0)
		$rate = get_exchange_rate_from_home_currency($curr, $date_);

	$SQLDate = date2sql($date_);
	if ($due_date == "")
		$SQLDueDate = "0000-00-00";
	else
		$SQLDueDate = date2sql($due_date);
	
	if ($trans_type == ST_BANKPAYMENT)
		$Total = -$Total;

	if ($new) {
	$trans_no = get_next_trans_no($trans_type);

	$sql = "INSERT INTO ".TB_PREF."debtor_trans (
		trans_no, type,
		debtor_no, branch_code,
		tran_date, due_date,
		reference, tpe,
		order_, ov_amount, ov_discount,
		ov_gst, ov_freight, ov_freight_tax,
		rate, ship_via, alloc, trans_link,
		dimension_id, dimension2_id
		) VALUES ($trans_no, ".db_escape($trans_type).",
		".db_escape($debtor_no).", ".db_escape($BranchNo).",
		'$SQLDate', '$SQLDueDate', ".db_escape($reference).",
		".db_escape($sales_type).", ".db_escape($order_no).", $Total, ".db_escape($discount).", $Tax,
		".db_escape($Freight).",
		$FreightTax, $rate, ".db_escape($ship_via).", $AllocAmt, ".db_escape($trans_link).",
		".db_escape($dimension_id).", ".db_escape($dimension2_id).")";
	} else {	// may be optional argument should stay unchanged ?
	$sql = "UPDATE ".TB_PREF."debtor_trans SET
		debtor_no=".db_escape($debtor_no)." , branch_code=".db_escape($BranchNo).",
		tran_date='$SQLDate', due_date='$SQLDueDate',
		reference=".db_escape($reference).", tpe=".db_escape($sales_type).", order_=".db_escape($order_no).",
		ov_amount=$Total, ov_discount=".db_escape($discount).", ov_gst=$Tax,
		ov_freight=".db_escape($Freight).", ov_freight_tax=$FreightTax, rate=$rate,
		ship_via=".db_escape($ship_via).", alloc=$AllocAmt, trans_link=$trans_link,
		dimension_id=".db_escape($dimension_id).", dimension2_id=".db_escape($dimension2_id)."
		WHERE trans_no=$trans_no AND type=".db_escape($trans_type);
	}
	db_query($sql, "The debtor transaction record could not be inserted");

	add_audit_trail($trans_type, $trans_no, $date_, $new ? '': _("Updated."));

	return $trans_no;
}

//----------------------------------------------------------------------------------------

function get_customer_trans($trans_id, $trans_type)
{
	$sql = "SELECT ".TB_PREF."debtor_trans.*,
		ov_amount+ov_gst+ov_freight+ov_freight_tax+ov_discount AS Total,
		".TB_PREF."debtors_master.name AS DebtorName, ".TB_PREF."debtors_master.address, ".TB_PREF."debtors_master.email AS email2,
		".TB_PREF."debtors_master.curr_code, ".TB_PREF."debtors_master.tax_id, ".TB_PREF."debtors_master.payment_terms ";

	if ($trans_type == ST_CUSTPAYMENT) {
		// it's a payment so also get the bank account
		$sql .= ", ".TB_PREF."bank_accounts.bank_name, ".TB_PREF."bank_accounts.bank_account_name,
			".TB_PREF."bank_accounts.account_type AS BankTransType ";
	}

	if ($trans_type == ST_SALESINVOICE || $trans_type == ST_CUSTCREDIT || $trans_type == ST_CUSTDELIVERY) {
		// it's an invoice so also get the shipper and salestype
		$sql .= ", ".TB_PREF."shippers.shipper_name, "
		.TB_PREF."sales_types.sales_type, "
		.TB_PREF."sales_types.tax_included, "
		.TB_PREF."cust_branch.*, "
		.TB_PREF."debtors_master.discount, "
		.TB_PREF."tax_groups.name AS tax_group_name, "
		.TB_PREF."tax_groups.id AS tax_group_id ";
	}


	$sql .= " FROM ".TB_PREF."debtor_trans, ".TB_PREF."debtors_master ";

	if ($trans_type == ST_CUSTPAYMENT) {
		// it's a payment so also get the bank account
		$sql .= ", ".TB_PREF."bank_trans, ".TB_PREF."bank_accounts";
	}

	if ($trans_type == ST_SALESINVOICE || $trans_type == ST_CUSTCREDIT || $trans_type == ST_CUSTDELIVERY) {
		// it's an invoice so also get the shipper, salestypes
		$sql .= ", ".TB_PREF."shippers, ".TB_PREF."sales_types, ".TB_PREF."cust_branch, ".TB_PREF."tax_groups ";
	}

	$sql .= " WHERE ".TB_PREF."debtor_trans.trans_no=".db_escape($trans_id)."
		AND ".TB_PREF."debtor_trans.type=".db_escape($trans_type)."
		AND ".TB_PREF."debtor_trans.debtor_no=".TB_PREF."debtors_master.debtor_no";

	if ($trans_type == ST_CUSTPAYMENT) {
		// it's a payment so also get the bank account
		$sql .= " AND ".TB_PREF."bank_trans.trans_no =$trans_id
			AND ".TB_PREF."bank_trans.type=$trans_type
			AND ".TB_PREF."bank_accounts.id=".TB_PREF."bank_trans.bank_act ";
	}
	if ($trans_type == ST_SALESINVOICE || $trans_type == ST_CUSTCREDIT || $trans_type == ST_CUSTDELIVERY) {
		// it's an invoice so also get the shipper
		$sql .= " AND ".TB_PREF."shippers.shipper_id=".TB_PREF."debtor_trans.ship_via
			AND ".TB_PREF."sales_types.id = ".TB_PREF."debtor_trans.tpe
			AND ".TB_PREF."cust_branch.branch_code = ".TB_PREF."debtor_trans.branch_code
			AND ".TB_PREF."cust_branch.tax_group_id = ".TB_PREF."tax_groups.id ";
	}

	$result = db_query($sql, "Cannot retreive a debtor transaction");

	if (db_num_rows($result) == 0) {
		// can't return nothing
		display_db_error("no debtor trans found for given params", $sql, true);
		exit;
	}

	if (db_num_rows($result) > 1) {
		// can't return multiple
		display_db_error("duplicate debtor transactions found for given params", $sql, true);
		exit;
	}

	//return db_fetch($result);
	$row = db_fetch($result);
	$row['email'] = $row['email2'];
	return $row;
}

//----------------------------------------------------------------------------------------

function exists_customer_trans($type, $type_no)
{
	$sql = "SELECT trans_no FROM ".TB_PREF."debtor_trans WHERE type=".db_escape($type)."
		AND trans_no=".db_escape($type_no);

	$result = db_query($sql, "Cannot retreive a debtor transaction");

	return (db_num_rows($result) > 0);
}

//----------------------------------------------------------------------------------------

// retreives the related sales order for a given trans

function get_customer_trans_order($type, $type_no)
{
	$sql = "SELECT order_ FROM ".TB_PREF."debtor_trans WHERE type=".db_escape($type)." AND trans_no=".db_escape($type_no);

	$result = db_query($sql, "The debtor transaction could not be queried");

	$row = db_fetch_row($result);

	return $row[0];
}

//----------------------------------------------------------------------------------------

function get_customer_details_from_trans($type, $type_no)
{
	$sql = "SELECT ".TB_PREF."debtors_master.name, ".TB_PREF."debtors_master.curr_code, ".TB_PREF."cust_branch.br_name
		FROM ".TB_PREF."debtors_master,".TB_PREF."cust_branch,".TB_PREF."debtor_trans
		WHERE ".TB_PREF."debtor_trans.type=".db_escape($type)." AND ".TB_PREF."debtor_trans.trans_no=".db_escape($type_no)."
		AND ".TB_PREF."debtors_master.debtor_no = ".TB_PREF."debtor_trans.debtor_no
		AND	".TB_PREF."cust_branch.branch_code = ".TB_PREF."debtor_trans.branch_code";

	$result = db_query($sql, "could not get customer details from trans");
	return db_fetch($result);
}

//----------------------------------------------------------------------------------------

function void_customer_trans($type, $type_no)
{
	// clear all values and mark as void
	$sql = "UPDATE ".TB_PREF."debtor_trans SET ov_amount=0, ov_discount=0, ov_gst=0, ov_freight=0,
		ov_freight_tax=0, alloc=0, version=version+1 WHERE type=".db_escape($type)." AND trans_no=".db_escape($type_no);

	db_query($sql, "could not void debtor transactions for type=$type and trans_no=$type_no");
}

//----------------------------------------------------------------------------------------

function post_void_customer_trans($type, $type_no)
{
	switch ($type) {
		case ST_SALESINVOICE :
		case ST_CUSTCREDIT   :
			void_sales_invoice($type, $type_no);
			break;
		case ST_CUSTDELIVERY :
			void_sales_delivery($type, $type_no);
			break;
		case ST_CUSTPAYMENT :
			void_customer_payment($type, $type_no);
			break;
	}
}

//----------------------------------------------------------------------------------------

function get_customer_trans_link($type, $type_no)
{
	$row = db_query("SELECT trans_link from ".TB_PREF."debtor_trans
		WHERE type=".db_escape($type)." AND trans_no=".db_escape($type_no),
		"could not get transaction link for type=$type and trans_no=$type_no");
	return $row[0];
}
//----------------------------------------------------------------------------------------

?>