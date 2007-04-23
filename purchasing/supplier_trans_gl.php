<?php

$page_security=5;
$path_to_root="..";

include($path_to_root . "/purchasing/includes/supp_trans_class.inc");
include($path_to_root . "/includes/session.inc");

include($path_to_root . "/purchasing/includes/purchasing_ui.inc");
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
	unset($_POST['gl_code']);
	unset($_POST['dimension_id']);
	unset($_POST['dimension2_id']);
	unset($_POST['amount']);
	unset($_POST['memo_']);
	unset($_POST['AcctSelection']);
	unset($_POST['AddGLCodeToTrans']);	
}

//------------------------------------------------------------------------------------------------

if (isset($_POST['AddGLCodeToTrans'])){

	$input_error = false;
	if (!isset($_POST['gl_code']))
	{
		$_POST['gl_code'] = $_POST['AcctSelection'];
	}

	$sql = "SELECT account_code, account_name FROM ".TB_PREF."chart_master WHERE account_code=" . $_POST['gl_code'];
	$result = db_query($sql,"get account information");
	if (db_num_rows($result) == 0)
	{
		display_error(_("The account code entered is not a valid code, this line cannot be added to the transaction."));
		$input_error = true;
	} 
	else 
	{
		$myrow = db_fetch_row($result);
		$gl_act_name = $myrow[1];
		if (!is_numeric($_POST['amount']))
		{
			display_error(_("The amount entered is not numeric. This line cannot be added to the transaction."));
			$input_error = true;
		}
	}

	if ($input_error == false)
	{
		$_SESSION['supp_trans']->add_gl_codes_to_trans($_POST['gl_code'], $gl_act_name, 
			$_POST['dimension_id'], $_POST['dimension2_id'], $_POST['amount'], $_POST['memo_']);
		clear_fields();
	}
}

//------------------------------------------------------------------------------------------------

if (isset($_GET['Delete']))
{
	$_SESSION['supp_trans']->remove_gl_codes_from_trans($_GET['Delete']);
	clear_fields();	
}

//------------------------------------------------------------------------------------------------

display_heading($_SESSION['supp_trans']->supplier_name);	

display_gl_items($_SESSION['supp_trans'], 1);
						
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

start_table($table_style2);

$accs = get_supplier_accounts($_SESSION['supp_trans']->supplier_id);
$_POST['AcctSelection'] = $accs['purchase_account'];
gl_all_accounts_list_row(_("GL Account Selection:"), 'AcctSelection', $_POST['AcctSelection']);
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

end_table();;

submit_center('AddGLCodeToTrans', _("Add GL Line"));

end_form();

//------------------------------------------------------------------------------------------------

end_page();
?>
