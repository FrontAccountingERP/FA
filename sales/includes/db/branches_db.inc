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
function add_branch($customer_id, $br_name, $br_ref, $br_address, $phone, $phone2, $fax, $contact_name, 
	$salesman, $area, $email, $tax_group_id, $sales_account, $sales_discount_account, $receivables_account, 
	$payment_discount_account, $default_location, $br_post_address, $disable_trans, $group_no,	$default_ship_via, $notes)
{
	$sql = "INSERT INTO ".TB_PREF."cust_branch (debtor_no, br_name, branch_ref, br_address,
		salesman, phone, phone2, fax,
		contact_name, area, email, tax_group_id, sales_account, receivables_account, payment_discount_account, sales_discount_account, default_location,
		br_post_address, disable_trans, group_no, default_ship_via, notes)
		VALUES (".db_escape($customer_id). ",".db_escape($br_name) . ", "
			.db_escape($br_ref) . ", "
			.db_escape($br_address) . ", ".db_escape($salesman) . ", "
			.db_escape($phone) . ", ".db_escape($phone2) . ", "
			.db_escape($fax) . ","
			.db_escape($contact_name) . ", ".db_escape($area) . ","
			.db_escape($email) . ", ".db_escape($tax_group_id) . ", "
			.db_escape($sales_account) . ", "
			.db_escape($receivables_account) . ", "
			.db_escape($payment_discount_account) . ", "
			.db_escape($sales_discount_account) . ", "
			.db_escape($default_location) . ", "
			.db_escape($br_post_address) . ","
			.db_escape($disable_trans) . ", "
			.db_escape($group_no) . ", "
			.db_escape($default_ship_via). ", "
			.db_escape($notes) . ")";
	db_query($sql,"The branch record could not be added");
}

function update_branch($customer_id, $branch_code, $br_name, $br_ref, $br_address, $phone, $phone2, $fax, $contact_name, 
	$salesman, $area, $email, $tax_group_id, $sales_account, $sales_discount_account, $receivables_account, 
	$payment_discount_account, $default_location, $br_post_address, $disable_trans, $group_no,	$default_ship_via, $notes)
{
	$sql = "UPDATE ".TB_PREF."cust_branch SET br_name = " . db_escape($br_name) . ",
		branch_ref = " . db_escape($br_ref) . ",
		br_address = ".db_escape($br_address). ",
		phone=".db_escape($phone). ",
		phone2=".db_escape($phone2). ",
		fax=".db_escape($fax).",
		contact_name=".db_escape($contact_name) . ",
		salesman= ".db_escape($salesman) . ",
		area=".db_escape($area) . ",
		email=".db_escape($email) . ",
		tax_group_id=".db_escape($tax_group_id). ",
		sales_account=".db_escape($sales_account) . ",
		sales_discount_account=".db_escape($sales_discount_account) . ",
		receivables_account=".db_escape($receivables_account) . ",
		payment_discount_account=".db_escape($payment_discount_account) . ",
		default_location=".db_escape($default_location) . ",
		br_post_address =".db_escape($br_post_address) . ",
		disable_trans=".db_escape($disable_trans) . ",
		group_no=".db_escape($group_no) . ", 
		default_ship_via=".db_escape($default_ship_via) . ",
		notes=".db_escape($notes) . "
		WHERE branch_code =".db_escape($branch_code) . "
		AND debtor_no=".db_escape($customer_id);
	db_query($sql,"The branch record could not be updated");
}

function delete_branch($customer_id, $branch_code)
{
	$sql="DELETE FROM ".TB_PREF."cust_branch WHERE branch_code=".db_escape($branch_code)." AND debtor_no=".db_escape($customer_id);
	db_query($sql,"could not delete branch");
}

function branch_in_foreign_table($customer_id, $branch_code, $table)
{
	$sql= "SELECT COUNT(*) FROM ".TB_PREF."$table WHERE branch_code=".db_escape($branch_code)
		." AND debtor_no = ".db_escape($customer_id);
	$result = db_query($sql,"could not query $table");
	$myrow = db_fetch_row($result);
	return ($myrow[0] > 0);
}

function get_branch($branch_id)
{
	$sql = "SELECT ".TB_PREF."cust_branch.*,".TB_PREF."salesman.salesman_name 
		FROM ".TB_PREF."cust_branch, ".TB_PREF."salesman 
		WHERE ".TB_PREF."cust_branch.salesman=".TB_PREF."salesman.salesman_code 
		AND branch_code=".db_escape($branch_id);
	
	$result = db_query($sql, "Cannot retreive a customer branch");
	
	return db_fetch($result);   	
}

function get_cust_branch($customer_id, $branch_code)
{
	$sql = "SELECT * FROM ".TB_PREF."cust_branch
		WHERE branch_code=".db_escape($branch_code)."
		AND debtor_no=".db_escape($customer_id);
	$result = db_query($sql,"check failed");
	return db_fetch($result);
}

function get_branch_accounts($branch_id)
{
	$sql = "SELECT receivables_account,sales_account, sales_discount_account, payment_discount_account 
		FROM ".TB_PREF."cust_branch WHERE branch_code=".db_escape($branch_id);
	
	$result = db_query($sql, "Cannot retreive a customer branch");
	
	return db_fetch($result);   	
}

function get_branch_name($branch_id)
{
	$sql = "SELECT br_name FROM ".TB_PREF."cust_branch 
		WHERE branch_code = ".db_escape($branch_id);

	$result = db_query($sql,"could not retreive name for branch" . $branch_id);

	$myrow = db_fetch_row($result);	
	return $myrow[0];
}

function get_cust_branches_from_group($group_no)
{
	$sql = "SELECT branch_code, debtor_no FROM ".TB_PREF."cust_branch 
		WHERE group_no = ".db_escape($group_no);

	return db_query($sql,"could not retreive branches for group " . $group_no);
}

function get_default_info_for_branch($customer_id)
{
	$sql = "SELECT name, address, email, debtor_ref
		FROM ".TB_PREF."debtors_master WHERE debtor_no = ".db_escape($customer_id);
	$result = db_query($sql,"check failed");
	return db_fetch($result);
}
function get_sql_for_customer_branches()
{
	$sql = "SELECT "
		."b.branch_code, "
		."b.branch_ref, "
		."b.br_name, "
		."b.contact_name, "
		."s.salesman_name, "
		."a.description, "
		."b.phone, "
		."b.fax, "
		."b.email, "
		."t.name AS tax_group_name, "
		."b.inactive
		FROM ".TB_PREF."cust_branch b, "
			.TB_PREF."debtors_master c, "
			.TB_PREF."areas a, "
			.TB_PREF."salesman s, "
			.TB_PREF."tax_groups t
		WHERE b.debtor_no=c.debtor_no
		AND b.tax_group_id=t.id
		AND b.area=a.area_code
		AND b.salesman=s.salesman_code
		AND b.debtor_no = ".db_escape($_POST['customer_id']);

	if (!get_post('show_inactive')) $sql .= " AND !b.inactive";
	return $sql;
}
?>