<?php
$page_security = 3;
$path_to_root="..";

include($path_to_root . "/includes/session.inc");

page(_("Tax Types"));

include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/taxes/db/tax_types_db.inc");

if (isset($_GET['selected_id']))
{
	$selected_id = $_GET['selected_id'];
} 
elseif(isset($_POST['selected_id']))
{
	$selected_id = $_POST['selected_id'];
}
//-----------------------------------------------------------------------------------

function can_process() 
{
	if (strlen($_POST['name']) == 0) 
	{
		display_error(_("The tax type name cannot be empty."));
		return false;
	} 
	elseif (!is_numeric($_POST['rate'])) 
	{
		display_error( _("The default tax rate must be numeric."));
		return false;
	} 
	elseif ($_POST['rate'] < 0) 
	{
		display_error( _("The default tax rate cannot be less than zero."));
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------------

if (isset($_POST['ADD_ITEM']) && can_process()) 
{

	add_tax_type($_POST['name'], $_POST['sales_gl_code'],
		$_POST['purchasing_gl_code'], $_POST['rate'], check_value('out'));
	meta_forward($_SERVER['PHP_SELF']);
}

//-----------------------------------------------------------------------------------

if (isset($_POST['UPDATE_ITEM']) && can_process()) 
{

	update_tax_type($selected_id, $_POST['name'],
    	$_POST['sales_gl_code'], $_POST['purchasing_gl_code'], $_POST['rate'], check_value('out'));
	meta_forward($_SERVER['PHP_SELF']);
}

//-----------------------------------------------------------------------------------

function can_delete($selected_id)
{
	$sql= "SELECT COUNT(*) FROM ".TB_PREF."tax_group_items	WHERE tax_type_id=$selected_id";
	$result = db_query($sql, "could not query tax groups");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_("Cannot delete this tax type because tax groups been created referring to it."));
		return false;
	}

	return true;
}


//-----------------------------------------------------------------------------------

if (isset($_GET['delete'])) 
{

	if (can_delete($selected_id))
	{
		delete_tax_type($selected_id);
		meta_forward($_SERVER['PHP_SELF']);
	}
}

//-----------------------------------------------------------------------------------

$result = get_all_tax_types();

start_table($table_style);

$th = array(_("Description"), _("Default Rate (%)"),
	_("Sales GL Account"), _("Purchasing GL Account"), _("Outstanding"), "", "");
table_header($th);	

$k = 0;
while ($myrow = db_fetch($result)) 
{

	if ($myrow['out'] == 1)
		$out = _("Yes");
	else
		$out = _("No");
	alt_table_row_color($k);

	label_cell($myrow["name"]);
	label_cell(number_format2($myrow["rate"],user_percent_dec()), "align=right");
	label_cell($myrow["sales_gl_code"] . "&nbsp;" . $myrow["SalesAccountName"]);
	label_cell($myrow["purchasing_gl_code"] . "&nbsp;" . $myrow["PurchasingAccountName"]);
	label_cell($out);

	edit_link_cell("selected_id=".$myrow["id"]);
	delete_link_cell("selected_id=".$myrow["id"]."&delete=1");
	end_row();
}

end_table();

//-----------------------------------------------------------------------------------

hyperlink_no_params($_SERVER['PHP_SELF'], _("New Tax Type"));

//-----------------------------------------------------------------------------------

start_form();

start_table($table_style2);

if (isset($selected_id)) 
{
	//editing an existing status code

	$myrow = get_tax_type($selected_id);

	$_POST['name']  = $myrow["name"];
	$_POST['rate']  = $myrow["rate"];
	$_POST['sales_gl_code']  = $myrow["sales_gl_code"];
	$_POST['purchasing_gl_code']  = $myrow["purchasing_gl_code"];
	$_POST['out']  = $myrow["out"];

	hidden('selected_id', $selected_id);
}
text_row_ex(_("Description:"), 'name', 50);
text_row_ex(_("Default Rate:"), 'rate', 10, 10, "", "%");

gl_all_accounts_list_row(_("Sales GL Account:"), 'sales_gl_code', null);
gl_all_accounts_list_row(_("Purchasing GL Account:"), 'purchasing_gl_code', null);

check_row(_("Outstanding:"), 'out', null);

end_table(1);

submit_add_or_update_center(!isset($selected_id));

end_form();

end_page();

?>
