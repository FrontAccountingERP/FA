<?php

$page_security=5;
$path_to_root="..";

include_once($path_to_root . "/purchasing/includes/purchasing_db.inc");

include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/banking.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/purchasing/includes/purchasing_ui.inc");
$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();
page(_("Enter Supplier Invoice"), false, false, "", $js);


//----------------------------------------------------------------------------------------

check_db_has_suppliers(_("There are no suppliers defined in the system."));

//---------------------------------------------------------------------------------------------------------------

if (isset($_GET['AddedID'])) 
{
	$invoice_no = $_GET['AddedID'];
	$trans_type = 20;


    echo "<center>";
    display_notification_centered(_("Supplier invoice has been processed."));
    display_note(get_trans_view_str($trans_type, $invoice_no, _("View this Invoice")));

	display_note(get_gl_view_str($trans_type, $invoice_no, _("View the GL Journal Entries for this Invoice")), 1);

    hyperlink_params($_SERVER['PHP_SELF'], _("Enter Another Invoice"), "New=1");

	display_footer_exit();
}

//--------------------------------------------------------------------------------------------------

if (isset($_GET['New']))
{
	if (isset( $_SESSION['supp_trans']))
	{
		unset ($_SESSION['supp_trans']->grn_items);
		unset ($_SESSION['supp_trans']->gl_codes);
		unset ($_SESSION['supp_trans']);
	}

	//session_register("SuppInv");
	session_register("supp_trans");
	$_SESSION['supp_trans'] = new supp_trans;
	$_SESSION['supp_trans']->is_invoice = true;
}

//--------------------------------------------------------------------------------------------------

function check_data()
{
	If (!$_SESSION['supp_trans']->is_valid_trans_to_post())
	{
		display_error(_("The invoice cannot be processed because the there are no items or values on the invoice.  Invoices are expected to have a charge."));
		return false;
	}

	if (!references::is_valid($_SESSION['supp_trans']->reference)) 
	{
		display_error(_("You must enter an invoice reference."));
		return false;
	}

	if (!is_new_reference($_SESSION['supp_trans']->reference, 20)) 
	{
		display_error(_("The entered reference is already in use."));
		return false;
	}

	if (!references::is_valid($_SESSION['supp_trans']->supp_reference)) 
	{
		display_error(_("You must enter a supplier's invoice reference."));
		return false;
	}

	if (!is_date( $_SESSION['supp_trans']->tran_date))
	{
		display_error(_("The invoice as entered cannot be processed because the invoice date is in an incorrect format."));
		return false;
	} 
	elseif (!is_date_in_fiscalyear($_SESSION['supp_trans']->tran_date)) 
	{
		display_error(_("The entered date is not in fiscal year."));
		return false;
	}
	if (!is_date( $_SESSION['supp_trans']->due_date))
	{
		display_error(_("The invoice as entered cannot be processed because the due date is in an incorrect format."));
		return false;
	}

	$sql = "SELECT Count(*) FROM ".TB_PREF."supp_trans WHERE supplier_id='" . $_SESSION['supp_trans']->supplier_id . "' AND supp_reference='" . $_POST['supp_reference'] . "'";
	$result=db_query($sql,"The sql to check for the previous entry of the same invoice failed");

	$myrow = db_fetch_row($result);
	if ($myrow[0] == 1)
	{ 	/*Transaction reference already entered */
		display_error(_("This invoice number has already been entered. It cannot be entered again." . " (" . $_POST['supp_reference'] . ")"));
		return false;
	}

	return true;
}

//--------------------------------------------------------------------------------------------------

function handle_commit_invoice()
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

if (isset($_POST['PostInvoice']))
{
	handle_commit_invoice();
}

//--------------------------------------------------------------------------------------------------

start_form(false, true);

start_table("$table_style2 width=80%", 8);
echo "<tr><td valign=center>"; // outer table

echo "<center>";

invoice_header($_SESSION['supp_trans']);

echo "</td></tr><tr><td valign=center>"; // outer table

echo "<center>";

display_grn_items($_SESSION['supp_trans']);

display_gl_items($_SESSION['supp_trans']);

//echo "</td></tr><tr><td align=center colspan=2>"; // outer table
echo "<br>";
invoice_totals($_SESSION['supp_trans']);

echo "</td></tr>";

end_table(); // outer table

echo "<br>";
submit_center('PostInvoice', _("Enter Invoice"));
echo "<br>";

end_form();

//--------------------------------------------------------------------------------------------------

end_page();
?>
