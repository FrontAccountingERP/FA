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
$page_security = 'SA_SRECURRENT';
$path_to_root = "../..";
include($path_to_root . "/includes/session.inc");
include($path_to_root . "/includes/ui.inc");

$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 600);
if ($use_date_picker)
	$js .= get_js_date_picker();

page(_($help_context = "Recurrent Invoices"), false, false, "", $js);

simple_page_mode(true);

if ($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM') 
{

	$input_error = 0;

	if (strlen($_POST['description']) == 0) 
	{
		$input_error = 1;
		display_error(_("The area description cannot be empty."));
		set_focus('description');
	}

	if ($input_error != 1)
	{
    	if ($selected_id != -1) 
    	{
    		$sql = "UPDATE ".TB_PREF."recurrent_invoices SET 
    			description=".db_escape($_POST['description']).", 
    			order_no=".db_escape($_POST['order_no']).", 
    			debtor_no=".db_escape($_POST['debtor_no']).", 
    			group_no=".db_escape($_POST['group_no']).", 
    			days=".input_num('days', 0).", 
    			monthly=".input_num('monthly', 0).", 
    			begin='".date2sql($_POST['begin'])."', 
    			end='".date2sql($_POST['end'])."' 
    			WHERE id = ".db_escape($selected_id);
			$note = _('Selected recurrent invoice has been updated');
    	} 
    	else 
    	{
    		$sql = "INSERT INTO ".TB_PREF."recurrent_invoices (description, order_no, debtor_no,
    			group_no, days, monthly, begin, end, last_sent) VALUES (".db_escape($_POST['description']) . ", "
    			.db_escape($_POST['order_no']).", ".db_escape($_POST['debtor_no']).", "
    			.db_escape($_POST['group_no']).", ".input_num('days', 0).", ".input_num('monthly', 0).", '"
    			.date2sql($_POST['begin'])."', '".date2sql($_POST['end'])."', '".date2sql(Add_Years($_POST['begin'], -5))."')";
			$note = _('New recurrent invoice has been added');
    	}
    
    	db_query($sql,"The recurrent invoice could not be updated or added");
		display_notification($note);    	
		$Mode = 'RESET';
	}
} 

if ($Mode == 'Delete')
{

	$cancel_delete = 0;

	if ($cancel_delete == 0) 
	{
		$sql="DELETE FROM ".TB_PREF."recurrent_invoices WHERE id=".db_escape($selected_id);
		db_query($sql,"could not delete recurrent invoice");

		display_notification(_('Selected recurrent invoice has been deleted'));
	} //end if Delete area
	$Mode = 'RESET';
} 

if ($Mode == 'RESET')
{
	$selected_id = -1;
	unset($_POST);
}
//-------------------------------------------------------------------------------------------------
function get_sales_group_name($group_no)
{
	$sql = "SELECT description FROM ".TB_PREF."groups WHERE id = ".db_escape($group_no);
	$result = db_query($sql, "could not get group");
	$row = db_fetch($result);
	return $row[0];
}

$sql = "SELECT * FROM ".TB_PREF."recurrent_invoices ORDER BY description, group_no, debtor_no";
$result = db_query($sql,"could not get recurrent invoices");

start_form();
start_table("$table_style width=70%");
$th = array(_("Description"), _("Template No"),_("Customer"),_("Branch")."/"._("Group"),_("Days"),_("Monthly"),_("Begin"),_("End"),_("Last Created"),"", "");
table_header($th);
$k = 0;
while ($myrow = db_fetch($result)) 
{
	$begin = sql2date($myrow["begin"]);
	$end = sql2date($myrow["end"]);
	$last_sent = sql2date($myrow["last_sent"]);
	
	alt_table_row_color($k);
		
	label_cell($myrow["description"]);
	label_cell(get_customer_trans_view_str(ST_SALESORDER, $myrow["order_no"]));
	if ($myrow["debtor_no"] == 0)
	{
		label_cell("");
		label_cell(get_sales_group_name($myrow["group_no"]));
	}	
	else
	{
		label_cell(get_customer_name($myrow["debtor_no"]));
		label_cell(get_branch_name($myrow['group_no']));
	}	
	label_cell($myrow["days"]);
	label_cell($myrow['monthly']);
	label_cell($begin);
	label_cell($end);
	label_cell($last_sent);
 	edit_button_cell("Edit".$myrow["id"], _("Edit"));
 	delete_button_cell("Delete".$myrow["id"], _("Delete"));
 	end_row();
}
end_table();

end_form();
echo '<br>';

//-------------------------------------------------------------------------------------------------

start_form();

start_table($table_style2);

if ($selected_id != -1) 
{
 	if ($Mode == 'Edit') {
		//editing an existing area
		$sql = "SELECT * FROM ".TB_PREF."recurrent_invoices WHERE id=".db_escape($selected_id);

		$result = db_query($sql,"could not get recurrent invoice");
		$myrow = db_fetch($result);

		$_POST['description']  = $myrow["description"];
		$_POST['order_no']  = $myrow["order_no"];
		$_POST['debtor_no']  = $myrow["debtor_no"];
		$_POST['group_no']  = $myrow["group_no"];
		$_POST['days']  = $myrow["days"];
		$_POST['monthly']  = $myrow["monthly"];
		$_POST['begin']  = sql2date($myrow["begin"]);
		$_POST['end']  = sql2date($myrow["end"]);
	} 
	hidden("selected_id", $selected_id);
}


text_row_ex(_("Description:"), 'description', 50); 

templates_list_row(_("Template:"), 'order_no');

customer_list_row(_("Customer:"), 'debtor_no', null, " ", true);

if ($_POST['debtor_no'] > 0)
	customer_branches_list_row(_("Branch:"), $_POST['debtor_no'], 'group_no', null, false);
else	
	sales_groups_list_row(_("Sales Group:"), 'group_no', null, " ");

small_amount_row(_("Days:"), 'days', 0, null, null, 0);

small_amount_row(_("Monthly:"), 'monthly', 0, null, null, 0);

date_row(_("Begin:"), 'begin');

date_row(_("End:"), 'end', null, null, 0, 0, 5);

end_table(1);

submit_add_or_update_center($selected_id == -1, '', 'both');

end_form();

end_page();
?>
