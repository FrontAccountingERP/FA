<?php

$page_security=5;
$path_to_root="..";

include_once($path_to_root . "/purchasing/includes/supp_trans_class.inc");
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/purchasing/includes/purchasing_ui.inc");
$js = "";
if ($use_date_picker)
	$js .= get_js_date_picker();
page(_("Add GL Items"), false, false, "", $js);


if (!isset($_SESSION['supp_trans']))
{
	display_note(_("To enter a supplier invoice or credit note the supplier must first be selected."));
	exit;
	/*It all stops here if there aint no supplier selected and transaction initiated ie $_SESSION['supp_trans'] started off*/
}

//------------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------------

$id = find_submit('Delete');
if ($id != -1)
{
	$_SESSION['supp_trans']->remove_gl_codes_from_trans($id);
	clear_fields();
	$Ajax->activate('gl_items');
}

//------------------------------------------------------------------------------------------------

display_heading($_SESSION['supp_trans']->supplier_name);

start_form(false,true);
display_gl_items($_SESSION['supp_trans'], 1);
end_form();
echo "<br>";

if ($_SESSION['supp_trans']->is_invoice == true)
{
	hyperlink_no_params("$path_to_root/purchasing/supplier_invoice.php", _("Back to Invoice Entry"));
}
else
{
	hyperlink_no_params("$path_to_root/purchasing/supplier_credit.php", _("Back to Credit Note Entry"));
}

echo "<hr>";

//------------------------------------------------------------------------------------------------

/*Set up a form to allow input of new GL entries */
start_form(false, true);

display_heading2(_("Enter a GL Line"));

div_start('gl_ctrls');
start_table($table_style2);

$accs = get_supplier_accounts($_SESSION['supp_trans']->supplier_id);
$_POST['gl_code'] = $accs['purchase_account'];
gl_all_accounts_list_row(_("GL Account Selection:"), 'gl_code', null);
$dim = get_company_pref('use_dimension');
if ($dim >= 1)
	dimensions_list_row(_("Dimension")." 1", 'dimension_id', null, true, " ", false, 1);
if ($dim > 1)
	dimensions_list_row(_("Dimension")." 2", 'dimension2_id', null, true, " ", false, 2);
if ($dim < 1)
	hidden('dimension_id', 0);
if ($dim < 2)
	hidden('dimension2_id', 0);
amount_row( _("Amount:"), 'amount');

textarea_row(_("Memo:"), "memo_",  null, 40, 2);

end_table();
div_end();
echo '<br>';
submit_center_first('AddGLCodeToTrans', _("Add GL Line"), '', true);
submit_center_last('ClearFields', _('Reset'), _("Clear all GL entry fields"), true);
end_form();

//------------------------------------------------------------------------------------------------
echo '<br>';
end_page(false, true);

?>
