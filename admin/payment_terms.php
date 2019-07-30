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
$page_security = 'SA_PAYTERMS';
$path_to_root="..";
include($path_to_root . "/includes/session.inc");

page(_($help_context = "Payment Terms"));

include($path_to_root . "/includes/ui.inc");

simple_page_mode(true);

//-------------------------------------------------------------------------------------------

if ($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM') 
{

	$input_error = 0;

	if (!is_numeric($_POST['DayNumber']))
	{
		$input_error = 1;
		display_error( _("The number of days or the day in the following month must be numeric."));
		set_focus('DayNumber');
	} 
	elseif (strlen($_POST['terms']) == 0) 
	{
		$input_error = 1;
		display_error( _("The Terms description must be entered."));
		set_focus('terms');
	}
	$early_days = input_num('early_days', 0);
	if ($early_days) {
		if ($early_days >= $_POST['DayNumber']) {
			$input_error = 1;
			display_error(_("Early payment days option should be shorter that payment term days."));
			set_focus('early_days');
		} else if (!check_num('early_discount', 0, 100) or input_num('early_discount') == 0)
		{
			$input_error = 1;
			display_error(_("The payment discount must be numeric and is expected to be less than 100% and greater than or equal to 0."));
			set_focus('early_discount');
		} 
	} else {
		if (input_num('early_discount', 0)) {
			$input_error = 1;
			display_error(_("Early payment days option should be positive and less than payment term days."));
			set_focus('early_days');
		}
	}

	if ($input_error != 1)
	{
    	if ($selected_id != -1) 
    	{
    		update_payment_terms($selected_id, get_post('terms'), get_post('type'), input_num('DayNumber', 0), input_num('early_discount')/100, $early_days); 
 			$note = _('Selected payment terms have been updated');
    	} 
    	else 
    	{
			add_payment_terms(get_post('terms'), get_post('type'), input_num('DayNumber', 0), input_num('early_discount')/100, $early_days);
			$note = _('New payment terms have been added');
    	}
    	//run the sql from either of the above possibilites
		display_notification($note);
 		$Mode = 'RESET';
	}
}

if ($Mode == 'Delete')
{
	// PREVENT DELETES IF DEPENDENT RECORDS IN debtors_master
	if (key_in_foreign_table($selected_id, 'debtors_master', 'payment_terms'))
	{
		display_error(_("Cannot delete this payment term, because customer accounts have been created referring to this term."));
	} 
	else 
	{
		if (key_in_foreign_table($selected_id, 'suppliers', 'payment_terms'))
		{
			display_error(_("Cannot delete this payment term, because supplier accounts have been created referring to this term"));
		} 
		else 
		{
			//only delete if used in neither customer or supplier accounts
			delete_payment_terms($selected_id);
			display_notification(_('Selected payment terms have been deleted'));
		}
	}
	//end if payment terms used in customer or supplier accounts
	$Mode = 'RESET';
}

if ($Mode == 'RESET')
{
	$selected_id = -1;
	$sav = get_post('show_inactive');
	unset($_POST);
	$_POST['show_inactive'] = $sav;
}
//-------------------------------------------------------------------------------------------------

$result = get_payment_terms_all(check_value('show_inactive'));

start_form();
start_table(TABLESTYLE);
$th = array(_("Description"), _("Type"), _("Due After/Days"), _("Early payment discount"),"", "");
inactive_control_column($th);
table_header($th);

$k = 0; //row colour counter
while ($myrow = db_fetch($result)) 
{
	$days = $myrow['days'];
	$percent = $myrow['early_discount'];

	alt_table_row_color($k);
    label_cell($myrow["terms"]);
    label_cell($pterm_types[$myrow['type']]);
    label_cell($myrow['type'] == PTT_DAYS ? "$days "._("days") : ($myrow['type'] == PTT_FOLLOWING ? $days : _("N/A")));
    label_cell(in_array($myrow['type'], array(PTT_FOLLOWING, PTT_DAYS)) ? ($percent==0 ? _("None") : ($percent*100).'/'.$myrow['early_days']) : _("N/A"));
	inactive_control_cell($myrow["id"], $myrow["inactive"], 'payment_terms', "id");
 	edit_button_cell("Edit".$myrow["id"], _("Edit"));
 	delete_button_cell("Delete".$myrow["id"], _("Delete"));
    end_row();

}

inactive_control_row($th);
end_table(1);

//-------------------------------------------------------------------------------------------------
if (list_updated('type')) {
	$Ajax->activate('edits');
}

div_start('edits');

start_table(TABLESTYLE2);

if ($selected_id != -1) 
{
	if ($Mode == 'Edit') {
		//editing an existing payment terms
		$myrow = get_payment_terms($selected_id);

		$_POST['terms']  = $myrow["terms"];
		$_POST['type'] = $myrow['type'];
		$_POST['DayNumber'] = $myrow['days'];
		$_POST['early_discount'] = $myrow['early_discount']*100;
		$_POST['early_days'] = $myrow['early_days'];
	}
	hidden('selected_id', $selected_id);
}

text_row(_("Terms Description:"), 'terms', null, 40, 40);

payment_type_list_row(_("Payment type:"), 'type', null, true);

if ( in_array(get_post('type'), array(PTT_FOLLOWING, PTT_DAYS))) {
	text_row_ex(_("Days (Or Day In Following Month):"), 'DayNumber', 3);
	small_amount_row(_("Days of early payment discount option:"), 'early_days', null, null, _('days'), 0);
	small_amount_row(_("Early payment discount percent:"), 'early_discount', null, null, _('%'), 1);
} else
	hidden('DayNumber', 0);

end_table(1);
div_end();

submit_add_or_update_center($selected_id == -1, '', 'both');

end_form();

end_page();
