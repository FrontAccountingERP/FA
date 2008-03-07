<?php

$path_to_root = "..";
$page_security = 3;

include($path_to_root . "/includes/session.inc");

page(_("Item Tax Types")); 

include_once($path_to_root . "/taxes/db/item_tax_types_db.inc");
include_once($path_to_root . "/taxes/db/tax_types_db.inc");

include($path_to_root . "/includes/ui.inc");

if (isset($_GET['selected_id']))
{
	$selected_id = $_GET['selected_id'];
} 
elseif(isset($_POST['selected_id']))
{
	$selected_id = $_POST['selected_id'];
}

//-----------------------------------------------------------------------------------

if (isset($_POST['ADD_ITEM']) || isset($_POST['UPDATE_ITEM'])) 
{

	$input_error = 0;

	if (strlen($_POST['name']) == 0) 
	{
		$input_error = 1;
		display_error(_("The item tax type description cannot be empty."));
	}

	if ($input_error != 1) 
	{
		
		// create an array of the exemptions
    	$exempt_from = array();
    	
        $tax_types = get_all_tax_types_simple();
        $i = 0;    	
        
        while ($myrow = db_fetch($tax_types)) 
        {
        	if (check_value('ExemptTax' . $myrow["id"]))
        	{
        		$exempt_from[$i] = $myrow["id"];
        		$i++;
        	}
        }  
        
    	if (isset($selected_id)) 
    	{
    		
    		update_item_tax_type($selected_id, $_POST['name'], $_POST['exempt'], $exempt_from);
    	} 
    	else 
    	{
    
    		add_item_tax_type($_POST['name'], $_POST['exempt'], $exempt_from);
    	}
		meta_forward($_SERVER['PHP_SELF']);     	
	}
} 

//-----------------------------------------------------------------------------------

function can_delete($selected_id)
{
	$sql= "SELECT COUNT(*) FROM ".TB_PREF."stock_master WHERE tax_type_id=$selected_id";
	$result = db_query($sql, "could not query stock master");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_error(_("Cannot delete this item tax type because items have been created referring to it."));
		return false;
	}
	
	return true;
}


//-----------------------------------------------------------------------------------

if (isset($_GET['delete'])) 
{

	if (can_delete($selected_id))
	{
		delete_item_tax_type($selected_id);
		meta_forward($_SERVER['PHP_SELF']); 		
	}
}

//-----------------------------------------------------------------------------------


$result2 = $result = get_all_item_tax_types();
start_table("$table_style width=30%");
$th = array(_("Name"), _("Tax exempt"),'','');

table_header($th);

$k = 0;
while ($myrow = db_fetch($result2)) 
{
	
	alt_table_row_color($k);	

	if ($myrow["exempt"] == 0) 
	{
		$disallow_text = _("No");
	} 
	else 
	{
		$disallow_text = _("Yes");
	}
	
	label_cell($myrow["name"]);
	label_cell($disallow_text);
	edit_link_cell("selected_id=" . $myrow["id"]);
	delete_link_cell("selected_id=" . $myrow["id"]. "&delete=1");
	end_row();
}

end_table();

//-----------------------------------------------------------------------------------

hyperlink_no_params($_SERVER['PHP_SELF'], _("New Item Tax type"));

start_form();

start_table($table_style2);

if (isset($selected_id)) 
{
	
	if (!isset($_POST['name'])) 
	{
    	$myrow = get_item_tax_type($selected_id);
    
    	$_POST['name']  = $myrow["name"];
    	$_POST['exempt']  = $myrow["exempt"];
    	
    	// read the exemptions and check the ones that are on
    	$exemptions = get_item_tax_type_exemptions($selected_id);
    	
    	if (db_num_rows($exemptions) > 0)
    	{
    		while ($exmp = db_fetch($exemptions)) 
    		{
    			$_POST['ExemptTax' . $exmp["tax_type_id"]] = 1;
    		}
    	}	
	}

	hidden('selected_id', $selected_id);
} 

text_row_ex(_("Description:"), 'name', 50);

yesno_list_row(_("Is Fully Tax-exempt:"), 'exempt', null, "", "", true);

end_table(1);

if (!isset($_POST['exempt']) || $_POST['exempt'] == 0) 
{

    display_note(_("Select which taxes this item tax type is exempt from."), 0, 1);
    
    start_table($table_style2);
    $th = array(_("Tax Name"), _("Rate"), _("Is exempt"));
    table_header($th);
    	
    $tax_types = get_all_tax_types_simple();    	
    
    while ($myrow = db_fetch($tax_types)) 
    {
    	
    	alt_table_row_color($k);	
    
    	label_cell($myrow["name"]);
	percent_cell($myrow["rate"]);
    	check_cells("", 'ExemptTax' . $myrow["id"], null);
    	end_row();
    }
    
    end_table(1);
}

submit_add_or_update_center(!isset($selected_id));

end_form();

//------------------------------------------------------------------------------------

end_page();

?>
