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

function update_company_gl_setup($retained_act, $profit_loss_act, $debtors_act, $pyt_discount_act, $creditors_act,
		$freight_act,
		$exchange_diff_act,
		$bank_charge_act,
		$default_sales_act,
		$default_sales_discount_act,
		$default_prompt_payment_act,
		$default_inventory_act,
		$default_cogs_act,
		$default_adj_act,
		$default_inv_sales_act,
		$default_assembly_act,
		$allow_negative_stock,
		$po_over_receive,
		$po_over_charge,
		$accumulate_shipping,
		$legal_text,
		$past_due_days,
		$default_credit_limit,
		$default_workorder_required,
		$default_dim_required,
		$default_delivery_required)
{
	$sql = "UPDATE ".TB_PREF."company SET
		retained_earnings_act=".db_escape($retained_act).", profit_loss_year_act=".db_escape($profit_loss_act).",
		debtors_act=".db_escape($debtors_act).", pyt_discount_act=".db_escape($pyt_discount_act).",
		creditors_act=".db_escape($creditors_act).",
		freight_act=".db_escape($freight_act).",
		exchange_diff_act=".db_escape($exchange_diff_act).",
		bank_charge_act=".db_escape($bank_charge_act).",
		default_sales_act=".db_escape($default_sales_act).",
		default_sales_discount_act=".db_escape($default_sales_discount_act).",
		default_prompt_payment_act=".db_escape($default_prompt_payment_act).",
		default_inventory_act=".db_escape($default_inventory_act).",
		default_cogs_act=".db_escape($default_cogs_act).",
		default_adj_act=".db_escape($default_adj_act).",
		default_inv_sales_act=".db_escape($default_inv_sales_act).",
		default_assembly_act=".db_escape($default_assembly_act).",
		allow_negative_stock=$allow_negative_stock,
		po_over_receive=$po_over_receive,
		po_over_charge=$po_over_charge,
		accumulate_shipping=$accumulate_shipping,
		legal_text=".db_escape($legal_text).",
		past_due_days=$past_due_days,
		default_credit_limit=$default_credit_limit,
		default_workorder_required=$default_workorder_required,
		default_dim_required=$default_dim_required,
		default_delivery_required=$default_delivery_required
		WHERE coy_code=1";

	db_query($sql, "The company gl setup could not be updated ");
}

function update_company_setup($coy_name, $coy_no, $gst_no, $tax_prd, $tax_last, 
	$postal_address, $phone, $fax, $email, $coy_logo, $domicile, $Dimension, 
	$curr_default, $f_year, $no_item_list, $no_customer_list, $no_supplier_list, 
	$base_sales, $time_zone, $add_pct, $round_to, $login_tout)
{
	if ($f_year == null)
		$f_year = 0;
	$sql = "UPDATE ".TB_PREF."company SET coy_name=".db_escape($coy_name).",
		coy_no = ".db_escape($coy_no).",
		gst_no=".db_escape($gst_no).",
		tax_prd=$tax_prd,
		tax_last=$tax_last,
		postal_address =".db_escape($postal_address).",
		phone=".db_escape($phone).", fax=".db_escape($fax).",
		email=".db_escape($email).",
		coy_logo=".db_escape($coy_logo).",
		domicile=".db_escape($domicile).",
		use_dimension=$Dimension,
		no_item_list=$no_item_list,
		no_customer_list=$no_customer_list,
		no_supplier_list=$no_supplier_list,
		curr_default=".db_escape($curr_default).",
		f_year=$f_year,
		base_sales=$base_sales,
		time_zone=$time_zone,
		add_pct=$add_pct,
		round_to=$round_to,
		login_tout = ".db_escape($login_tout)."
		WHERE coy_code=1";

	db_query($sql, "The company setup could not be updated ");
}

function get_company_prefs($tbpref = TB_PREF)
{
	$sql = "SELECT * FROM ".$tbpref."company WHERE coy_code=1";
	
	$result = db_query($sql, "The company preferences could not be retrieved");

	if (db_num_rows($result) == 0)
		display_db_error("FATAL : Could not find company prefs", $sql);

	return db_fetch($result);
}

function get_company_pref($pref_name, $tbpref = TB_PREF)
{
	$prefs = get_company_prefs($tbpref);
	return $prefs[$pref_name];
}

// fiscal year routines
function add_fiscalyear($from_date, $to_date, $closed)
{
	$from = date2sql($from_date);
	$to = date2sql($to_date);

	$sql = "INSERT INTO ".TB_PREF."fiscal_year (begin, end, closed)
		VALUES (".db_escape($from).",".db_escape($to).", ".db_escape($closed).")";

	db_query($sql, "could not add fiscal year");
}

function update_fiscalyear($id, $closed)
{
	$sql = "UPDATE ".TB_PREF."fiscal_year SET closed=".db_escape($closed)."
		WHERE id=".db_escape($id);

	db_query($sql, "could not update fiscal year");
}

function get_all_fiscalyears()
{
	$sql = "SELECT * FROM ".TB_PREF."fiscal_year ORDER BY begin";

	return db_query($sql, "could not get all fiscal years");
}

function get_fiscalyear($id)
{
	$sql = "SELECT * FROM ".TB_PREF."fiscal_year WHERE id=".db_escape($id);

	$result = db_query($sql, "could not get fiscal year");

	return db_fetch($result);
}

function get_current_fiscalyear()
{
	$year = get_company_pref('f_year');

	$sql = "SELECT * FROM ".TB_PREF."fiscal_year WHERE id=".db_escape($year);

	$result = db_query($sql, "could not get current fiscal year");

	return db_fetch($result);
}

function delete_fiscalyear($id)
{
	begin_transaction();

	$sql="DELETE FROM ".TB_PREF."fiscal_year WHERE id=".db_escape($id);

	db_query($sql, "could not delete fiscal year");

	commit_transaction();
}

function get_base_sales_type()
{
	$sql = "SELECT base_sales FROM ".TB_PREF."company WHERE coy_code=1";

	$result = db_query($sql, "could not get base sales type");
	$myrow = db_fetch($result);
	return $myrow[0];
}

function get_company_extensions($id = -1) {
	global $path_to_root;

	$file = $path_to_root.($id == -1 ? '' : '/company/'.$id).'/installed_extensions.php';
	$installed_extensions = array();
	if (is_file($file)) {
		include($file);
	}
	return $installed_extensions;
}


?>