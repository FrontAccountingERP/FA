<?php

$page_security = 3;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("Credit Status")); 

include($path_to_root . "/sales/includes/db/credit_status_db.inc");

include($path_to_root . "/includes/ui.inc");

if (isset($_GET['selected_id']))
{
	$selected_id = $_GET['selected_id'];
} 
elseif (isset($_POST['selected_id']))
{
	$selected_id = $_POST['selected_id'];
}

//-----------------------------------------------------------------------------------

function can_process() 
{
	
	if (strlen($_POST['reason_description']) == 0) 
	{
		display_error(_("The credit status description cannot be empty."));
		return false;
	}	
	
	return true;
}

//-----------------------------------------------------------------------------------

if (isset($_POST['ADD_ITEM']) && can_process()) 
{

	add_credit_status($_POST['reason_description'], $_POST['DisallowInvoices']);
	meta_forward($_SERVER['PHP_SELF']);    	
} 

//-----------------------------------------------------------------------------------

if (isset($_POST['UPDATE_ITEM']) && can_process()) 
{

	update_credit_status($selected_id, $_POST['reason_description'], $_POST['DisallowInvoices']);
	meta_forward($_SERVER['PHP_SELF']);    	
}

//-----------------------------------------------------------------------------------

function can_delete($selected_id)
{
	$sql= "SELECT COUNT(*) FROM ".TB_PREF."debtors_master 
		WHERE credit_status=$selected_id";
	$result = db_query($sql, "could not query customers");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_("Cannot delete this credit status because customer accounts have been created referring to it."));
		return false;
	}
	
	return true;
}


//-----------------------------------------------------------------------------------

if (isset($_GET['delete'])) 
{

	if (can_delete($selected_id))
	{
		delete_credit_status($selected_id);
		meta_forward($_SERVER['PHP_SELF']);		
	}
}

//-----------------------------------------------------------------------------------

$result = get_all_credit_status();

start_table("$table_style width=40%");
$th = array(_("Description"), _("Dissallow Invoices"));
table_header($th);

$k = 0;
while ($myrow = db_fetch($result)) 
{
	
	alt_table_row_color($k);	

	if ($myrow["dissallow_invoices"] == 0) 
	{
		$disallow_text = _("Invoice OK");
	} 
	else 
	{
		$disallow_text = "<b>" . _("NO INVOICING") . "</b>";
	}
	
	label_cell($myrow["reason_description"]);
	label_cell($disallow_text);
	edit_link_cell("selected_id=" . $myrow["id"]);
	delete_link_cell("selected_id=" . $myrow["id"]. "&delete=1");
	end_row();
}

end_table();

//-----------------------------------------------------------------------------------

hyperlink_no_params($_SERVER['PHP_SELF'], _("New Credit Status"));

start_form();

start_table("$table_style2 width=40%");

if (isset($selected_id)) 
{
	//editing an existing status code

	$myrow = get_credit_status($selected_id);

	$_POST['reason_description']  = $myrow["reason_description"];
	$_POST['DisallowInvoices']  = $myrow["dissallow_invoices"];

	hidden('selected_id', $selected_id);
} 

text_row_ex(_("Description:"), 'reason_description', 50);

yesno_list_row(_("Dissallow invoicing ?"), 'DisallowInvoices', null); 

end_table(1);

submit_add_or_update_center(!isset($selected_id));

end_form();

//------------------------------------------------------------------------------------

end_page();

?>
