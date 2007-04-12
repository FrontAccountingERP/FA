<?php

$page_security =10;
$path_to_root="..";
include($path_to_root . "/includes/session.inc");

page(_("Company Setup"));

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");

include_once($path_to_root . "/admin/db/company_db.inc");

//-------------------------------------------------------------------------------------------------

if (isset($_POST['submit']) && $_POST['submit'] != "") 
{

	$input_error = 0;

	if (strlen($_POST['coy_name'])==0) 
	{
		$input_error = 1;
		display_error(_("The company name must be entered."));
	}
	if ($input_error != 1)
	{
		update_company_setup($_POST['coy_name'], $_POST['coy_no'], $_POST['gst_no'], $_POST['tax_prd'], $_POST['tax_last'],
			$_POST['postal_address'], $_POST['phone'], $_POST['fax'], $_POST['email'], $_POST['coy_logo'], $_POST['domicile'],
			$_POST['use_dimension'], $_POST['custom1_name'], $_POST['custom2_name'], $_POST['custom3_name'],
			$_POST['custom1_value'], $_POST['custom2_value'], $_POST['custom3_value'],
			$_POST['curr_default'], $_POST['f_year']);

		display_notification_centered(_("Company setup has been updated."));
	}

} /* end of if submit */

//---------------------------------------------------------------------------------------------


start_form();

$myrow = get_company_prefs();

$_POST['coy_name'] = $myrow["coy_name"];
$_POST['gst_no'] = $myrow["gst_no"];
$_POST['tax_prd'] = $myrow["tax_prd"];
$_POST['tax_last'] = $myrow["tax_last"];
$_POST['coy_no']  = $myrow["coy_no"];
$_POST['postal_address']  = $myrow["postal_address"];
$_POST['phone']  = $myrow["phone"];
$_POST['fax']  = $myrow["fax"];
$_POST['email']  = $myrow["email"];
$_POST['coy_logo']  = $myrow["coy_logo"];
$_POST['domicile']  = $myrow["domicile"];
$_POST['use_dimension']  = $myrow["use_dimension"];
$_POST['custom1_name']  = $myrow["custom1_name"];
$_POST['custom2_name']  = $myrow["custom2_name"];
$_POST['custom3_name']  = $myrow["custom3_name"];
$_POST['custom1_value']  = $myrow["custom1_value"];
$_POST['custom2_value']  = $myrow["custom2_value"];
$_POST['custom3_value']  = $myrow["custom3_value"];
$_POST['curr_default']  = $myrow["curr_default"];
$_POST['f_year']  = $myrow["f_year"];

start_table($table_style2);

text_row_ex(_("Name (to appear on reports):"), 'coy_name', 42, 50);
text_row_ex(_("Official Company Number:"), 'coy_no', 25);
text_row_ex(_("Tax Authority Reference:"), 'gst_no', 25);

text_row_ex(_("Tax Periods:"), 'tax_prd', 10, 10, null, null, _('Months.'));
text_row_ex(_("Tax Last Period:"), 'tax_last', 10, 10, null, null, _('Months back.'));

currencies_list_row(_("Home Currency:"), 'curr_default', $_POST['curr_default']);
fiscalyears_list_row(_("Fiscal Year:"), 'f_year', $_POST['f_year']);

textarea_row(_("Address:"), 'postal_address', $_POST['postal_address'], 35, 5);

text_row_ex(_("Telephone Number:"), 'phone', 25, 55);
text_row_ex(_("Facsimile Number:"), 'fax', 25);
text_row_ex(_("Email Address:"), 'email', 25, 55);
text_row_ex(_("Company Logo:"), 'coy_logo', 25, 55);
text_row_ex(_("Domicile:"), 'domicile', 25, 55);

number_list_row(_("Use Dimensions:"), 'use_dimension', null, 0, 2);

start_row();
end_row();
label_row(_("Custom Field Name"), _("Custom Field Value"));

start_row();
text_cells(null, 'custom1_name', $_POST['custom1_name'], 25, 25);
text_cells(null, 'custom1_value', $_POST['custom1_value'], 30, 30);
end_row();

start_row();
text_cells(null, 'custom2_name', $_POST['custom2_name'], 25, 25);
text_cells(null, 'custom2_value', $_POST['custom2_value'], 30, 30);
end_row();

start_row();
text_cells(null, 'custom3_name', $_POST['custom3_name'], 25, 25);
text_cells(null, 'custom3_value', $_POST['custom3_value'], 30, 30);
end_row();

end_table(1);

submit_center('submit', _("Update"));

end_form(2);
//-------------------------------------------------------------------------------------------------

end_page();

?>
