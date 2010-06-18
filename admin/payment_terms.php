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

	$inpug_error = 0;

	if (!is_numeric($_POST['DayNumber']))
	{
		$inpug_error = 1;
		display_error( _("The number of days or the day in the following month must be numeric."));
		set_focus('DayNumber');
	} 
	elseif (strlen($_POST['terms']) == 0) 
	{
		$inpug_error = 1;
		display_error( _("The Terms description must be entered."));
		set_focus('terms');
	} // there should be no limits by 30 here if they want longer payment terms. Joe Hunt 2010-05-31
	//elseif ($_POST['DayNumber'] > 30 && !check_value('DaysOrFoll')) 
	//{
	//	$inpug_error = 1;
	//	display_error( _("When the check box to indicate a day in the following month is the due date, the due date cannot be a day after the 30th. A number between 1 and 30 is expected."));
	//	set_focus('DayNumber');
	//} 
	// No constrain on day values, Joe Hunt 2010-06-18.
	//elseif ($_POST['DayNumber'] > 500 && check_value('DaysOrFoll')) 
	//{
	//	$inpug_error = 1;
	//	display_error( _("When the check box is not checked to indicate that the term expects a number of days after which accounts are due, the number entered should be less than 500 days."));
	//	set_focus('DayNumber');
	//}

	if ($_POST['DayNumber'] == '')
		$_POST['DayNumber'] = 0;

	if ($inpug_error != 1)
	{
    	if ($selected_id != -1) 
    	{
    		if (check_value('DaysOrFoll')) 
    		{
    			$sql = "UPDATE ".TB_PREF."payment_terms SET terms=" . db_escape($_POST['terms']) . ",
					day_in_following_month=0,
					days_before_due=" . db_escape($_POST['DayNumber']) . "
					WHERE terms_indicator = " .db_escape($selected_id);
    		} 
    		else 
    		{
    			$sql = "UPDATE ".TB_PREF."payment_terms SET terms=" . db_escape($_POST['terms']) . ",
					day_in_following_month=" . db_escape($_POST['DayNumber']) . ",
					days_before_due=0
					WHERE terms_indicator = " .db_escape( $selected_id );
    		}
 			$note = _('Selected payment terms have been updated');
    	} 
    	else 
    	{

    		if (check_value('DaysOrFoll')) 
    		{
    			$sql = "INSERT INTO ".TB_PREF."payment_terms (terms,
					days_before_due, day_in_following_month)
					VALUES (" .
					db_escape($_POST['terms']) . ", " . db_escape($_POST['DayNumber']) . ", 0)";
    		} 
    		else 
    		{
    			$sql = "INSERT INTO ".TB_PREF."payment_terms (terms,
					days_before_due, day_in_following_month)
					VALUES (" . db_escape($_POST['terms']) . ",
					0, " . db_escape($_POST['DayNumber']) . ")";
    		}
			$note = _('New payment terms have been added');
    	}
    	//run the sql from either of the above possibilites
    	db_query($sql,"The payment term could not be added or updated");
		display_notification($note);
 		$Mode = 'RESET';
	}
}

if ($Mode == 'Delete')
{
	// PREVENT DELETES IF DEPENDENT RECORDS IN debtors_master

	$sql= "SELECT COUNT(*) FROM ".TB_PREF."debtors_master WHERE payment_terms = ".db_escape($selected_id);
	$result = db_query($sql,"check failed");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_("Cannot delete this payment term, because customer accounts have been created referring to this term."));
	} 
	else 
	{
		$sql= "SELECT COUNT(*) FROM ".TB_PREF."suppliers WHERE payment_terms = ".db_escape($selected_id);
		$result = db_query($sql,"check failed");
		$myrow = db_fetch_row($result);
		if ($myrow[0] > 0) 
		{
			display_error(_("Cannot delete this payment term, because supplier accounts have been created referring to this term"));
		} 
		else 
		{
			//only delete if used in neither customer or supplier accounts

			$sql="DELETE FROM ".TB_PREF."payment_terms WHERE terms_indicator=".db_escape($selected_id);
			db_query($sql,"could not delete a payment terms");
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

$sql = "SELECT * FROM ".TB_PREF."payment_terms";
if (!check_value('show_inactive')) $sql .= " WHERE !inactive";
$result = db_query($sql,"could not get payment terms");

start_form();
start_table($table_style);
$th = array(_("Description"), _("Following Month On"), _("Due After (Days)"), "", "");
inactive_control_column($th);
table_header($th);

$k = 0; //row colour counter
while ($myrow = db_fetch($result)) 
{
	if ($myrow["day_in_following_month"] == 0) 
	{
		$full_text = _("N/A");
	} 
	else 
	{
		$full_text = $myrow["day_in_following_month"];
	}

	if ($myrow["days_before_due"] == 0) 
	{
		$after_text = _("N/A");
	} 
	else 
	{
		$after_text = $myrow["days_before_due"] . " " . _("days");
	}

	alt_table_row_color($k);

    label_cell($myrow["terms"]);
    label_cell($full_text);
    label_cell($after_text);
	inactive_control_cell($myrow["terms_indicator"], $myrow["inactive"], 'payment_terms', "terms_indicator");
 	edit_button_cell("Edit".$myrow["terms_indicator"], _("Edit"));
 	delete_button_cell("Delete".$myrow["terms_indicator"], _("Delete"));
    end_row();


} //END WHILE LIST LOOP

inactive_control_row($th);
end_table(1);

//-------------------------------------------------------------------------------------------------

start_table($table_style2);

$day_in_following_month = $days_before_due = 0;
if ($selected_id != -1) 
{
	if ($Mode == 'Edit') {
		//editing an existing payment terms
		$sql = "SELECT * FROM ".TB_PREF."payment_terms
			WHERE terms_indicator=".db_escape($selected_id);

		$result = db_query($sql,"could not get payment term");
		$myrow = db_fetch($result);

		$_POST['terms']  = $myrow["terms"];
		$days_before_due  = $myrow["days_before_due"];
		$day_in_following_month  = $myrow["day_in_following_month"];
		unset($_POST['DayNumber']);
	}
	hidden('selected_id', $selected_id);
}
text_row(_("Terms Description:"), 'terms', null, 40, 40);

check_row(_("Due After A Given No. Of Days:"), 'DaysOrFoll', $day_in_following_month == 0);

if (!isset($_POST['DayNumber'])) 
{
    if ($days_before_due != 0)
    	$_POST['DayNumber'] = $days_before_due;
    else
    	$_POST['DayNumber'] = $day_in_following_month;
}

text_row_ex(_("Days (Or Day In Following Month):"), 'DayNumber', 3);

end_table(1);

submit_add_or_update_center($selected_id == -1, '', 'both');

end_form();

end_page();

?>
