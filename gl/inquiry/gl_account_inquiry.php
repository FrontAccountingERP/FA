<?php

$page_security = 8;
$path_to_root="../..";
include_once($path_to_root . "/includes/session.inc");


include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/gl/includes/gl_db.inc");

$js = get_js_set_focus('account');
if ($use_popup_windows)
	$js .= get_js_open_window(800, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();

page(_("General Ledger Account Inquiry"), false, false, "setFocus()", $js);

//----------------------------------------------------------------------------------------------------

if (isset($_GET["account"]))
	$_POST["account"] = $_GET["account"];
if (isset($_GET["TransFromDate"]))
	$_POST["TransFromDate"] = $_GET["TransFromDate"];
if (isset($_GET["TransToDate"]))
	$_POST["TransToDate"] = $_GET["TransToDate"];
if (isset($_GET["Dimension"]))
	$_POST["Dimension"] = $_GET["Dimension"];
if (isset($_GET["Dimension2"]))
	$_POST["Dimension2"] = $_GET["Dimension2"];

//----------------------------------------------------------------------------------------------------

function gl_inquiry_controls()
{
	global $table_style2;
	
	$dim = get_company_pref('use_dimension');
    start_form();

    //start_table($table_style2);
    start_table("class='tablestyle_noborder'");
	start_row();
	
    gl_all_accounts_list_cells(_("Account:"), 'account', null);

	date_cells(_("from:"), 'TransFromDate', null, -30);
	date_cells(_("to:"), 'TransToDate');
    submit_cells('Show',_("Show"));
    
    end_row();

	if ($dim >= 1)
		dimensions_list_row(_("Dimension")." 1", 'Dimension', null, true, " ", false, 1);
	if ($dim > 1)
		dimensions_list_row(_("Dimension")." 2", 'Dimension2', null, true, " ", false, 2);
	end_table();
    
    end_form();
}

//----------------------------------------------------------------------------------------------------

function show_results()
{
	global $path_to_root, $table_style;

	if (!isset($_POST["account"]) || $_POST["account"] == "")
		return;
	$act_name = get_gl_account_name($_POST["account"]);
	$dim = get_company_pref('use_dimension');

    /*Now get the transactions  */
    if (!isset($_POST['Dimension']))
    	$_POST['Dimension'] = 0;
    if (!isset($_POST['Dimension2']))
    	$_POST['Dimension2'] = 0;
	$result = get_gl_transactions($_POST['TransFromDate'], $_POST['TransToDate'], -1,
    	$_POST["account"], $_POST['Dimension'], $_POST['Dimension2']);

	if (db_num_rows($result) == 0)
	{
		display_note(_("No general ledger transactions have been created for this account on the selected dates."), 0, 1);
		return;
	}
	$colspan = ($dim == 2 ? "6" : ($dim == 1 ? "5" : "4"));
	//echo "\nDimension =". $_POST['Dimension'];
	display_heading($_POST["account"]. "&nbsp;&nbsp;&nbsp;".$act_name);

	start_table($table_style);
	if ($dim == 2)
		$th = array(_("Type"), _("#"), _("Date"), _("Dimension")." 1", _("Dimension")." 2",
			_("Person/Item"), _("Debit"), _("Credit"), _("Balance"), _("Memo"));
	else if ($dim == 1)
		$th = array(_("Type"), _("#"), _("Date"), _("Dimension"),
			_("Person/Item"), _("Debit"), _("Credit"), _("Balance"), _("Memo"));
	else		
		$th = array(_("Type"), _("#"), _("Date"), 
			_("Person/Item"), _("Debit"), _("Credit"), _("Balance"), _("Memo"));
	table_header($th);
    $bfw = get_gl_balance_from_to("", $_POST['TransFromDate'], $_POST["account"], $_POST['Dimension'], $_POST['Dimension2']);

	start_row("class='inquirybg'");
	label_cell("<b>"._("Opening Balance")." - ".$_POST['TransFromDate']."</b>", "colspan=$colspan");
	display_debit_or_credit_cells($bfw);
	label_cell("");
	end_row();
	//$running_total =0;
	$running_total = $bfw;
	$j = 1;
	$k = 0; //row colour counter

	while ($myrow = db_fetch($result)) 
	{

    	alt_table_row_color($k);

    	$running_total += $myrow["amount"];

    	$trandate = sql2date($myrow["tran_date"]);

    	label_cell(systypes::name($myrow["type"]));
		label_cell(get_gl_view_str($myrow["type"], $myrow["type_no"], $myrow["type_no"], true));
    	label_cell($trandate);
		if ($dim >= 1)
			label_cell(get_dimension_string($myrow['dimension_id'], true));
		if ($dim > 1)
			label_cell(get_dimension_string($myrow['dimension2_id'], true));
		label_cell(payment_person_types::person_name($myrow["person_type_id"],$myrow["person_id"]));
		display_debit_or_credit_cells($myrow["amount"]);
		amount_cell($running_total);
    	label_cell($myrow['memo_']);
    	end_row();

    	$j++;
    	if ($j == 12)
    	{ 
    		$j = 1; 
    		table_header($th);
    	}
	}
	//end of while loop

	start_row("class='inquirybg'");
	label_cell("<b>" . _("Ending Balance") ." - ".$_POST['TransToDate']. "</b>", "colspan=$colspan");
	display_debit_or_credit_cells($running_total);
	label_cell("");
	end_row();

	end_table(2);
}

//----------------------------------------------------------------------------------------------------

gl_inquiry_controls();

show_results();

//----------------------------------------------------------------------------------------------------

end_page();

?>
