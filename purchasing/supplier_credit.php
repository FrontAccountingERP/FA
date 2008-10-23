<?php

$path_to_root="..";

include_once($path_to_root . "/purchasing/includes/supp_trans_class.inc");

$page_security=5;

include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/purchasing/includes/purchasing_db.inc");
include_once($path_to_root . "/purchasing/includes/purchasing_ui.inc");
$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();
page(_("Supplier Credit Note"), false, false, "", $js);

//----------------------------------------------------------------------------------------

check_db_has_suppliers(_("There are no suppliers defined in the system."));

//---------------------------------------------------------------------------------------------------------------
if ($ret = context_restore()) {
 // return from supplier editor
	copy_from_trans($_SESSION['supp_trans']);
	if(isset($ret['supplier_id']))
		$_POST['supplier_id'] = $ret['supplier_id'];
}
if (isset($_POST['_supplier_id_editor'])) {
	copy_to_trans($_SESSION['supp_trans']);
	context_call($path_to_root.'/purchasing/manage/suppliers.php?supplier_id='.$_POST['supplier_id'], 'supp_trans');
}

//---------------------------------------------------------------------------------------------------------------

if (isset($_GET['AddedID'])) 
{
	$invoice_no = $_GET['AddedID'];
	$trans_type = 21;


    echo "<center>";
    display_notification_centered(_("Supplier credit note has been processed."));
    display_note(get_trans_view_str($trans_type, $invoice_no, _("View this Credit Note")));

	display_note(get_gl_view_str($trans_type, $invoice_no, _("View the GL Journal Entries for this Credit Note")), 1);

    hyperlink_params($_SERVER['PHP_SELF'], _("Enter Another Credit Note"), "New=1");

	display_footer_exit();
}

//---------------------------------------------------------------------------------------------------

if (isset($_GET['New']))
{
	if (isset( $_SESSION['supp_trans']))
	{
		unset ($_SESSION['supp_trans']->grn_items);
		unset ($_SESSION['supp_trans']->gl_codes);
		unset ($_SESSION['supp_trans']);
	}

	$_SESSION['supp_trans'] = new supp_trans;
	$_SESSION['supp_trans']->is_invoice = false;
}

function clear_fields()
{
	global $Ajax;
	
	unset($_POST['gl_code']);
	unset($_POST['dimension_id']);
	unset($_POST['dimension2_id']);
	unset($_POST['amount']);
	unset($_POST['memo_']);
	unset($_POST['AddGLCodeToTrans']);
	$Ajax->activate('gl_ctrls');
	set_focus('gl_code');
}
//------------------------------------------------------------------------------------------------
//	GL postings are often entered in the same form to two accounts
//  so fileds are cleared only on user demand.
//
if (isset($_POST['ClearFields']))
{
	clear_fields();
}

if (isset($_POST['AddGLCodeToTrans'])){

	$Ajax->activate('gl_items');
	$input_error = false;

	$sql = "SELECT account_code, account_name FROM ".TB_PREF."chart_master WHERE account_code='" . $_POST['gl_code'] . "'";
	$result = db_query($sql,"get account information");
	if (db_num_rows($result) == 0)
	{
		display_error(_("The account code entered is not a valid code, this line cannot be added to the transaction."));
		set_focus('gl_code');
		$input_error = true;
	}
	else
	{
		$myrow = db_fetch_row($result);
		$gl_act_name = $myrow[1];
		if (!check_num('amount'))
		{
			display_error(_("The amount entered is not numeric. This line cannot be added to the transaction."));
			set_focus('amount');
			$input_error = true;
		}
	}

	if ($input_error == false)
	{
		$_SESSION['supp_trans']->add_gl_codes_to_trans($_POST['gl_code'], $gl_act_name,
			$_POST['dimension_id'], $_POST['dimension2_id'], 
			input_num('amount'), $_POST['memo_']);
		set_focus('gl_code');
	}
}


//---------------------------------------------------------------------------------------------------

function check_data()
{
	global $total_grn_value, $total_gl_value;
	
	if (!$_SESSION['supp_trans']->is_valid_trans_to_post())
	{
		display_error(_("The credit note cannot be processed because the there are no items or values on the invoice.  Credit notes are expected to have a charge."));
		set_focus('');
		return false;
	}

	if (!references::is_valid($_SESSION['supp_trans']->reference)) 
	{
		display_error(_("You must enter an credit note reference."));
		set_focus('reference');
		return false;
	}

	if (!is_new_reference($_SESSION['supp_trans']->reference, 21)) 
	{
		display_error(_("The entered reference is already in use."));
		set_focus('reference');
		return false;
	}

	if (!references::is_valid($_SESSION['supp_trans']->supp_reference)) 
	{
		display_error(_("You must enter a supplier's credit note reference."));
		set_focus('supp_reference');
		return false;
	}

	if (!is_date($_SESSION['supp_trans']->tran_date))
	{
		display_error(_("The credit note as entered cannot be processed because the date entered is not valid."));
		set_focus('tran_date');
		return false;
	} 
	elseif (!is_date_in_fiscalyear($_SESSION['supp_trans']->tran_date)) 
	{
		display_error(_("The entered date is not in fiscal year."));
		set_focus('tran_date');
		return false;
	}
	if (!is_date( $_SESSION['supp_trans']->due_date))
	{
		display_error(_("The invoice as entered cannot be processed because the due date is in an incorrect format."));
		set_focus('due_date');
		return false;
	}

	if ($_SESSION['supp_trans']->ov_amount < ($total_gl_value + $total_grn_value))
	{
		display_error(_("The credit note total as entered is less than the sum of the the general ledger entires (if any) and the charges for goods received. There must be a mistake somewhere, the credit note as entered will not be processed."));
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------------------------------

function handle_commit_credit_note()
{
	copy_to_trans($_SESSION['supp_trans']);

	if (!check_data())
		return;

	$invoice_no = add_supp_invoice($_SESSION['supp_trans']);

    $_SESSION['supp_trans']->clear_items();
    unset($_SESSION['supp_trans']);

	meta_forward($_SERVER['PHP_SELF'], "AddedID=$invoice_no");
}

//--------------------------------------------------------------------------------------------------

if (isset($_POST['PostCreditNote']))
{
	handle_commit_credit_note();
}

function check_item_data($n)
{
	if (!check_num('This_QuantityCredited'.$n, 0))
	{
		display_error(_("The quantity to credit must be numeric and greater than zero."));
		set_focus('This_QuantityCredited'.$n);
		return false;
	}

	if (!check_num('ChgPrice'.$n, 0))
	{
		display_error(_("The price is either not numeric or negative."));
		set_focus('ChgPrice'.$n);
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------------------

$id = find_submit('grn_item_id');
if ($id != -1)
{
	if (check_item_data($id))
	{
		$complete = False;

		//$_SESSION['supp_trans']->add_grn_to_trans($_POST['GRNNumber'],
    	//	$_POST['po_detail_item'], $_POST['item_code'],
    	//	$_POST['item_description'], $_POST['qty_recd'],
    	//	$_POST['prev_quantity_inv'], $_POST['This_QuantityCredited'],
    	//	$_POST['order_price'], $_POST['ChgPrice'], $complete,
    	//	$_POST['std_cost_unit'], $_POST['gl_code']);
		$_SESSION['supp_trans']->add_grn_to_trans($id,
    		$_POST['po_detail_item'.$id], $_POST['item_code'.$id],
    		$_POST['item_description'.$id], $_POST['qty_recd'.$id],
    		$_POST['prev_quantity_inv'.$id], input_num('This_QuantityCredited'.$id),
    		$_POST['order_price'.$id], input_num('ChgPrice'.$id), $complete,
    		$_POST['std_cost_unit'.$id], "");
	}
}

//--------------------------------------------------------------------------------------------------
$id = find_submit('Delete');
if ($id != -1)
{
	$_SESSION['supp_trans']->remove_grn_from_trans($id);
	$Ajax->activate('grn_items');
	$Ajax->activate('grn_table');
	$Ajax->activate('inv_tot');
}

$id = find_submit('Delete2');
if ($id != -1)
{
	$_SESSION['supp_trans']->remove_gl_codes_from_trans($id);
	clear_fields();
	$Ajax->activate('gl_items');
	$Ajax->activate('inv_tot');
}


//--------------------------------------------------------------------------------------------------

start_form(false, true);

start_table("$table_style width=98%", 8);
echo "<tr><td valign=center>"; // outer table

echo "<center>";

invoice_header($_SESSION['supp_trans']);
if ($_POST['supplier_id']=='') 
	display_error('No supplier found for entered search text');
else {
	echo "</td></tr><tr><td valign=center>"; // outer table

	$total_grn_value = display_grn_items($_SESSION['supp_trans'], 1);

	$total_gl_value = display_gl_items($_SESSION['supp_trans'], 1);

	echo "</td></tr><tr><td align=center colspan=2>"; // outer table
	div_start('inv_tot');
	invoice_totals($_SESSION['supp_trans']);
	div_end();
}
echo "</td></tr>";

end_table(1); // outer table

$id = find_submit('grn_item_id');
if ($id != -1)
{
	$Ajax->activate('grn_table');
	$Ajax->activate('grn_items');
	$Ajax->activate('inv_tot');
}

if (get_post('AddGLCodeToTrans'))
	$Ajax->activate('inv_tot');


submit_center('PostCreditNote', _("Enter Credit Note"), true, '', true);
echo "<br><br>";

end_form();
end_page();
?>
