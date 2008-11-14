<?php


function get_branch($branch_id)
{
	$sql = "SELECT ".TB_PREF."cust_branch.*,".TB_PREF."salesman.salesman_name 
		FROM ".TB_PREF."cust_branch, ".TB_PREF."salesman 
		WHERE ".TB_PREF."cust_branch.salesman=".TB_PREF."salesman.salesman_code 
		AND branch_code=$branch_id";
	
	$result = db_query($sql, "Cannot retreive a customer branch");
	
	return db_fetch($result);   	
}

function get_branch_accounts($branch_id)
{
	$sql = "SELECT receivables_account,sales_account, sales_discount_account, payment_discount_account 
		FROM ".TB_PREF."cust_branch WHERE branch_code=$branch_id";
	
	$result = db_query($sql, "Cannot retreive a customer branch");
	
	return db_fetch($result);   	
}

function get_branch_name($branch_id)
{
	$sql = "SELECT br_name FROM ".TB_PREF."cust_branch 
		WHERE branch_code = '$branch_id'";

	$result = db_query($sql,"could not retreive name for branch" . $branch_id);

	$myrow = db_fetch_row($result);	
	return $myrow[0];
}

function get_cust_branches_from_group($group_no)
{
	$sql = "SELECT branch_code, debtor_no FROM ".TB_PREF."cust_branch 
		WHERE group_no = '$group_no'";

	return db_query($sql,"could not retreive branches for group " . $group_no);
}
?>