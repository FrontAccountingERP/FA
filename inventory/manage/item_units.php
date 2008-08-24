<?php
$page_security = 11;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("Units of Measure"));

include_once($path_to_root . "/includes/ui.inc");

include_once($path_to_root . "/inventory/includes/db/items_units_db.inc");

simple_page_mode(false);
//----------------------------------------------------------------------------------

if ($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM') 
{

	//initialise no input errors assumed initially before we test
	$input_error = 0;

	if (strlen($_POST['abbr']) == 0)
	{
		$input_error = 1;
		display_error(_("The unit of measure code cannot be empty."));
		set_focus('abbr');
	}
	if (strlen($_POST['description']) == 0)
	{
		$input_error = 1;
		display_error(_("The unit of measure description cannot be empty."));
		set_focus('description');
	}

	if ($input_error !=1) {
    	write_item_unit(htmlentities($selected_id), $_POST['abbr'], $_POST['description'], $_POST['decimals'] );
		if($selected_id != '')
			display_notification(_('Selected unit has been updated'));
		else
			display_notification(_('New unit has been added'));
		$Mode = 'RESET';
	}
}

//----------------------------------------------------------------------------------

if ($Mode == 'Delete')
{

	// PREVENT DELETES IF DEPENDENT RECORDS IN 'stock_master'

	if (item_unit_used($selected_id))
	{
		display_error(_("Cannot delete this unit of measure because items have been created using this unit."));

	}
	else
	{
		delete_item_unit($selected_id);
		display_notification(_('Selected unit has been deleted'));
	}
	$Mode = 'RESET';
}

if ($Mode == 'RESET')
{
	$selected_id = '';
	unset($_POST);
}

//----------------------------------------------------------------------------------

$result = get_all_item_units();
start_form();
start_table("$table_style width=50%");
$th = array(_('Unit'), _('Description'), _('Decimals'), "", "");

table_header($th);
$k = 0; //row colour counter

while ($myrow = db_fetch($result))
{

	alt_table_row_color($k);

	label_cell($myrow["abbr"]);
	label_cell($myrow["name"]);
	label_cell(($myrow["decimals"]==-1?_("User Quantity Decimals"):$myrow["decimals"]));

 	edit_button_cell("Edit".$myrow[0], _("Edit"));
 	edit_button_cell("Delete".$myrow[0], _("Delete"));
	end_row();
}

end_table();
end_form();
echo '<br>';

//----------------------------------------------------------------------------------

start_form();

start_table("class='tablestyle_noborder'");

if ($selected_id != '') 
{
 	if ($Mode == 'Edit') {
		//editing an existing item category

		$myrow = get_item_unit($selected_id);

		$_POST['abbr'] = $myrow["abbr"];
		$_POST['description']  = $myrow["name"];
		$_POST['decimals']  = $myrow["decimals"];
	}
	hidden('selected_id', $selected_id);
}
if ($selected_id != '' && item_unit_used($selected_id)) {
    label_row(_("Unit Abbreviation:"), $_POST['abbr']);
    hidden('abbr', $_POST['abbr']);
} else
    text_row(_("Unit Abbreviation:"), 'abbr', null, 20, 20);
text_row(_("Descriptive Name:"), 'description', null, 40, 40);

number_list_row(_("Decimal Places:"), 'decimals', null, 0, 6, _("User Quantity Decimals"));

end_table(1);

submit_add_or_update_center($selected_id == '', '', true);

end_form();

end_page();

?>
