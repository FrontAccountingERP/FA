<?php

$page_security = 10;
$path_to_root="..";
include($path_to_root . "/includes/session.inc");

page(_("Payment Terms"));

include($path_to_root . "/includes/ui.inc");


//-------------------------------------------------------------------------------------------

if (isset($_GET['selected_id']))
{
	$selected_id = $_GET['selected_id'];
} 
elseif (isset($_POST['selected_id']))
{
	$selected_id = $_POST['selected_id'];
}

//-------------------------------------------------------------------------------------------

if (isset($_POST['ADD_ITEM']) OR isset($_POST['UPDATE_ITEM'])) 
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
	} 
	elseif ($_POST['DayNumber'] > 30 && !check_value('DaysOrFoll')) 
	{
		$inpug_error = 1;
		display_error( _("When the check box to indicate a day in the following month is the due date, the due date cannot be a day after the 30th. A number between 1 and 30 is expected."));
		set_focus('DayNumber');
	} 
	elseif ($_POST['DayNumber'] > 500 && check_value('DaysOrFoll')) 
	{
		$inpug_error = 1;
		display_error( _("When the check box is not checked to indicate that the term expects a number of days after which accounts are due, the number entered should be less than 500 days."));
		set_focus('DayNumber');
	}

	if ($_POST['DayNumber'] == '')
		$_POST['DayNumber'] = 0;

	if ($inpug_error != 1)
	{
    	if (isset($selected_id)) 
    	{
    		if (check_value('DaysOrFoll')) 
    		{
    			$sql = "UPDATE ".TB_PREF."payment_terms SET terms='" . $_POST['terms'] . "',
					day_in_following_month=0,
					days_before_due=" . $_POST['DayNumber'] . "
					WHERE terms_indicator = '" . $selected_id . "'";
    		} 
    		else 
    		{
    			$sql = "UPDATE ".TB_PREF."payment_terms SET terms='" . $_POST['terms'] . "',
					day_in_following_month=" . $_POST['DayNumber'] . ",
					days_before_due=0
					WHERE terms_indicator = '" . $selected_id . "'";
    		}

    	} 
    	else 
    	{

    		if (check_value('DaysOrFoll')) 
    		{
    			$sql = "INSERT INTO ".TB_PREF."payment_terms (terms,
					days_before_due, day_in_following_month)
					VALUES ('" .
					$_POST['terms'] . "', " . $_POST['DayNumber'] . ", 0)";
    		} 
    		else 
    		{
    			$sql = "INSERT INTO ".TB_PREF."payment_terms (terms,
					days_before_due, day_in_following_month)
					VALUES ('" . $_POST['terms'] . "',
					0, " . $_POST['DayNumber'] . ")";
    		}

    	}
    	//run the sql from either of the above possibilites
    	db_query($sql,"The payment term could not be added or updated");

		meta_forward($_SERVER['PHP_SELF']);
	}
}

if (isset($_GET['delete'])) 
{
	// PREVENT DELETES IF DEPENDENT RECORDS IN debtors_master

	$sql= "SELECT COUNT(*) FROM ".TB_PREF."debtors_master WHERE payment_terms = '$selected_id'";
	$result = db_query($sql,"check failed");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_("Cannot delete this payment term, because customer accounts have been created referring to this term."));
	} 
	else 
	{
		$sql= "SELECT COUNT(*) FROM ".TB_PREF."suppliers WHERE payment_terms = '$selected_id'";
		$result = db_query($sql,"check failed");
		$myrow = db_fetch_row($result);
		if ($myrow[0] > 0) 
		{
			display_error(_("Cannot delete this payment term, because supplier accounts have been created referring to this term"));
		} 
		else 
		{
			//only delete if used in neither customer or supplier accounts

			$sql="DELETE FROM ".TB_PREF."payment_terms WHERE terms_indicator='$selected_id'";
			db_query($sql,"could not delete a payment terms");

			meta_forward($_SERVER['PHP_SELF']);
		}
	}
	//end if payment terms used in customer or supplier accounts
}

//-------------------------------------------------------------------------------------------------

$sql = "SELECT * FROM ".TB_PREF."payment_terms";
$result = db_query($sql,"could not get payment terms");

start_table($table_style);
$th = array(_("Description"), _("Following Month On"), _("Due After (Days)"), "", "");
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
    edit_link_cell("selected_id=".$myrow["terms_indicator"]);
    delete_link_cell("selected_id=".$myrow["terms_indicator"]."&delete=1");
    end_row();


} //END WHILE LIST LOOP

end_table();

hyperlink_no_params($_SERVER['PHP_SELF'], _("New Payment Term"));

//-------------------------------------------------------------------------------------------------

start_form();

start_table($table_style2);

$day_in_following_month = $days_before_due = 0;
if (isset($selected_id)) 
{
	//editing an existing payment terms
	$sql = "SELECT * FROM ".TB_PREF."payment_terms
		WHERE terms_indicator='$selected_id'";

	$result = db_query($sql,"could not get payment term");
	$myrow = db_fetch($result);

	$_POST['terms']  = $myrow["terms"];
	$days_before_due  = $myrow["days_before_due"];
	$day_in_following_month  = $myrow["day_in_following_month"];

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

submit_add_or_update_center(!isset($selected_id));

end_form();

end_page();

?>
