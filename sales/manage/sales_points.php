<?php

$page_security = 15;
$path_to_root="../..";
include_once($path_to_root . "/includes/session.inc");

page(_("POS settings"));

include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/sales/includes/db/sales_points_db.inc");

simple_page_mode(true);
//----------------------------------------------------------------------------------------------------

function can_process()
{
	if (strlen($_POST['name']) == 0)
	{
		display_error(_("The POS name cannot be empty."));
		set_focus('pos_name');
		return false;
	}
	if (!check_value('cash') && !check_value('credit'))
	{
		display_error(_("You must allow cash or credit sale."));
		set_focus('credit');
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------------------

if ($Mode=='ADD_ITEM' && can_process())
{
	add_sales_point($_POST['name'], $_POST['location'], $_POST['account'],
		check_value('cash'), check_value('credit'));
	display_notification(_('New point of sale has been added'));
	$Mode = 'RESET';
}

//----------------------------------------------------------------------------------------------------

if ($Mode=='UPDATE_ITEM' && can_process())
{

	update_sales_point($selected_id, $_POST['name'], $_POST['location'],
		$_POST['account'], check_value('cash'), check_value('credit'));
	display_notification(_('Selected point of sale has been updated'));
	$Mode = 'RESET';
}

//----------------------------------------------------------------------------------------------------

if ($Mode == 'Delete')
{
	delete_sales_point($selected_id);
	display_notification(_('Selected point of sale has been deleted'));
	$Mode = 'RESET';
}

if ($Mode == 'RESET')
{
	$selected_id = -1;
	unset($_POST);
}
//----------------------------------------------------------------------------------------------------

$result = get_all_sales_points();

start_form();
start_table("$table_style");

$th = array (_('POS Name'), _('Credit sale'), _('Cash sale'), _('Location'), _('Default account'), 
	 '','');
table_header($th);
$k = 0;

while ($myrow = db_fetch($result))
{
    alt_table_row_color($k);
	label_cell($myrow["pos_name"], "nowrap");
	label_cell($myrow['credit_sale'] ? _('Yes') : _('No'));
	label_cell($myrow['cash_sale'] ? _('Yes') : _('No'));
	label_cell($myrow["location_name"], "");
	label_cell($myrow["bank_account_name"], "");
 	edit_button_cell("Edit".$myrow['id'], _("Edit"));
 	edit_button_cell("Delete".$myrow['id'], _("Delete"));
	end_row();
}

end_table();
end_form();
echo '<br>';
//----------------------------------------------------------------------------------------------------

start_form();

start_table("$table_style2 width=30%");

if ($selected_id != -1)
{

 	if ($Mode == 'Edit') {
		$myrow = get_sales_point($selected_id);

		$_POST['name']  = $myrow["pos_name"];
		$_POST['location']  = $myrow["pos_location"];
		$_POST['account']  = $myrow["pos_account"];
		if ($myrow["credit_sale"]) $_POST['credit_sale']  = 1;
		if ($myrow["cash_sale"]) $_POST['cash_sale'] = 1;
	}
	hidden('selected_id', $selected_id);
} 

text_row_ex(_("Point of Sale Name").':', 'name', 20, 30);
check_row(_('Allowed credit sale'), 'credit', check_value('credit_sale'));
check_row(_('Allowed cash sale'), 'cash',  check_value('cash_sale'));
locations_list_row(_("POS location").':', 'location');
cash_accounts_list_row(_("Default cash account").':', 'account');

end_table(1);

submit_add_or_update_center($selected_id == -1, '', true);

end_form();

end_page();

?>
