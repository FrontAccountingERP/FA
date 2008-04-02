<?php

$page_security = 3;
$path_to_root="../..";

include($path_to_root . "/includes/session.inc");

page(_("Bank Transaction Types"));

include($path_to_root . "/gl/includes/gl_db.inc");

include($path_to_root . "/includes/ui.inc");

if (isset($_GET['selected_id']))
{
	$selected_id = $_GET['selected_id'];
} 
elseif(isset($_POST['selected_id']))
{
	$selected_id = $_POST['selected_id'];
}
else
	$selected_id = -1;
//-----------------------------------------------------------------------------------

if (isset($_POST['ADD_ITEM']) || isset($_POST['UPDATE_ITEM'])) 
{

	//initialise no input errors assumed initially before we test
	$input_error = 0;

	if (strlen($_POST['name']) == 0) 
	{
		$input_error = 1;
		display_error( _("The bank transaction type name cannot be empty."));
		set_focus('name');
	}

	if ($input_error != 1) 
	{
		
    	if ($selected_id != -1) 
    	{
    		
    		update_bank_trans_type($selected_id, $_POST['name']);
    
    	} 
    	else 
    	{
    
    		add_bank_trans_type($_POST['name']);
    	}
		
		meta_forward($_SERVER['PHP_SELF']);    	
	}
} 

//-----------------------------------------------------------------------------------

function can_delete($selected_id)
{
	if ($selected_id == -1)
		return false;
	$sql= "SELECT COUNT(*) FROM ".TB_PREF."bank_trans WHERE bank_trans_type_id=$selected_id";
	$result = db_query($sql, "could not query bank transactions");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_("Cannot delete this bank transaction type because bank transactions have been created referring to it."));
		return false;
	}
	
	return true;
}


//-----------------------------------------------------------------------------------

if (isset($_GET['delete'])) 
{

	if (can_delete($selected_id))
	{
		delete_bank_trans_type($selected_id);
		meta_forward($_SERVER['PHP_SELF']);		
	}
}

//-----------------------------------------------------------------------------------

$result = get_all_bank_trans_type();

start_table($table_style);

$th = array(_("Description"), "", "");
table_header($th);

$k = 0;
while ($myrow = db_fetch($result)) 
{
	
	alt_table_row_color($k);	

	label_cell($myrow["name"]);
	edit_link_cell("selected_id=" . $myrow["id"]);
	delete_link_cell("selected_id=" . $myrow["id"]. "&delete=1");
	end_row(); 
}

end_table();

//-----------------------------------------------------------------------------------

hyperlink_no_params($_SERVER['PHP_SELF'], _("New Bank Transaction Type"));

start_form();

start_table($table_style2);

if ($selected_id != -1) 
{
	//editing an existing status code

	$myrow = get_bank_trans_type($selected_id);

	$_POST['name']  = $myrow["name"];

	hidden('selected_id', $selected_id);
} 

text_row_ex(_("Description:"), 'name', 40);

end_table(1);

submit_add_or_update_center($selected_id == -1);

end_form();

//------------------------------------------------------------------------------------

end_page();

?>
