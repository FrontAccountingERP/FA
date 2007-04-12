<?php

$page_security = 3;
$path_to_root="..";

include($path_to_root . "/includes/session.inc");

page(_("Tax Groups"));

include_once($path_to_root . "/includes/data_checks.inc");
include_once($path_to_root . "/includes/ui.inc");

include_once($path_to_root . "/taxes/db/tax_groups_db.inc");
include_once($path_to_root . "/taxes/db/tax_types_db.inc");

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
	
check_db_has_tax_types(_("There are no tax types defined. Define tax types before defining tax groups."));

//-----------------------------------------------------------------------------------

if (isset($_POST['ADD_ITEM']) || isset($_POST['UPDATE_ITEM'])) 
{

	//initialise no input errors assumed initially before we test
	$input_error = 0;

	if (strlen($_POST['name']) == 0) 
	{
		$input_error = 1;
		display_error(_("The tax group name cannot be empty."));
	} 
	else 
	{
		// make sure any entered rates are valid
    	for ($i = 0; $i < 5; $i++) 
    	{
    		if (isset($_POST['tax_type_id' . $i]) && 
    			$_POST['tax_type_id' . $i] != reserved_words::get_all_numeric()	&& 
    			(!is_numeric($_POST['rate' . $i]) || $_POST['rate' . $i] < 0))
    		{
				display_error( _("An entered tax rate is invalid or less than zero."));
    			$input_error = 1;
				break;
    		}
    	}
	}

	if ($input_error != 1) 
	{

		// create an array of the taxes and array of rates
    	$taxes = array();
    	$rates = array();
    	$included = array();

    	for ($i = 0; $i < 5; $i++) 
    	{
    		if (isset($_POST['tax_type_id' . $i]) &&
   				$_POST['tax_type_id' . $i] != reserved_words::get_any_numeric()) 
   			{
        		$taxes[] = $_POST['tax_type_id' . $i];
        		$rates[] = $_POST['rate' . $i];
        		if (isset($_POST['included' . $i]))
        			$included[] = 1;
        		else	
        			$included[] = 0;
    		}
    	}

    	if ($selected_id != -1) 
    	{

    		update_tax_group($selected_id, $_POST['name'], $_POST['tax_shipping'], $taxes, 
    			$rates, $included);

    	} 
    	else 
    	{

    		add_tax_group($_POST['name'], $_POST['tax_shipping'], $taxes, $rates, $included);
    	}

		meta_forward($_SERVER['PHP_SELF']);
	}
}

//-----------------------------------------------------------------------------------

function can_delete($selected_id)
{
	if ($selected_id == -1)
		return false;
	$sql = "SELECT COUNT(*) FROM ".TB_PREF."cust_branch WHERE tax_group_id=$selected_id";
	$result = db_query($sql, "could not query customers");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_note(_("Cannot delete this tax group because customer branches been created referring to it."));
		return false;
	}

	$sql = "SELECT COUNT(*) FROM ".TB_PREF."suppliers WHERE tax_group_id=$selected_id";
	$result = db_query($sql, "could not query suppliers");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		display_note(_("Cannot delete this tax group because suppliers been created referring to it."));
		return false;
	}


	return true;
}


//-----------------------------------------------------------------------------------

if (isset($_GET['delete'])) 
{

	if (can_delete($selected_id))
	{
		delete_tax_group($selected_id);
		meta_forward($_SERVER['PHP_SELF']);
	}
}

//-----------------------------------------------------------------------------------

$result = get_all_tax_groups();

start_table($table_style);
$th = array(_("Description"), _("Tax Shipping"), "", "");
table_header($th);

$k = 0;
while ($myrow = db_fetch($result)) 
{

	alt_table_row_color($k);

	label_cell($myrow["name"]);
	if ($myrow["tax_shipping"])
		label_cell(_("Yes"));
	else
		label_cell(_("No"));

	/*for ($i=0; $i< 5; $i++)
		if ($myrow["type" . $i] != reserved_words::get_all_numeric())
			echo "<td>" . $myrow["type" . $i] . "</td>";*/

	edit_link_cell("selected_id=" . $myrow["id"]);
	delete_link_cell("selected_id=" . $myrow["id"]. "&delete=1");
	end_row();;
}

end_table();

//-----------------------------------------------------------------------------------

hyperlink_no_params($_SERVER['PHP_SELF'], _("New Tax Group"));

start_form();

start_table($table_style2);

if ($selected_id != -1) 
{
	//editing an existing status code

	if (!isset($_POST['name']))
	{
    	$group = get_tax_group($selected_id);

    	$_POST['name']  = $group["name"];
    	$_POST['tax_shipping'] = $group["tax_shipping"];

    	$items = get_tax_group_items($selected_id);

    	$i = 0;
    	while ($tax_item = db_fetch($items)) 
    	{
    		$_POST['tax_type_id' . $i]  = $tax_item["tax_type_id"];
    		$_POST['rate' . $i]  = $tax_item["rate"];
    		$_POST['included' . $i]  = $tax_item["included_in_price"];
    		$i ++;
    	}
	}

	hidden('selected_id', $selected_id);
}
text_row_ex(_("Description:"), 'name', 40);
yesno_list_row(_("Tax Shipping:"), 'tax_shipping', null, "", "", true);

end_table();

display_note(_("Select the taxes that are included in this group."), 1);

start_table($table_style2);
$th = array(_("Tax"), _("Default Rate (%)"), _("Rate (%)"), _("Include in Price"));
table_header($th);
for ($i = 0; $i < 5; $i++) 
{
	start_row();
	if (!isset($_POST['tax_type_id' . $i]))
		$_POST['tax_type_id' . $i] = 0;
	if (!isset($_POST['included' . $i]))
		$_POST['included' . $i] = 0;
	tax_types_list_cells(null, 'tax_type_id' . $i, $_POST['tax_type_id' . $i], true, _("None"), true);

	if ($_POST['tax_type_id' . $i] != 0 && $_POST['tax_type_id' . $i] != reserved_words::get_all_numeric()) 
	{

		$default_rate = get_tax_type_default_rate($_POST['tax_type_id' . $i]);
		label_cell(number_format2($default_rate, user_percent_dec()), "nowrap align=right");

		if (!isset($_POST['rate' . $i]) || $_POST['rate' . $i] == "")
			$_POST['rate' . $i] = $default_rate;
		text_cells(null, 'rate' . $i, $_POST['rate' . $i], 10, 10);
		check_cells(null, 'included' . $i, $_POST['included' . $i]);
	}
	end_row();
}

end_table(1);

submit_add_or_update_center(!isset($selected_id));

end_form();

//------------------------------------------------------------------------------------

end_page();

?>
