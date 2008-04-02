<?php

$page_security = 3;
$path_to_root="..";
include_once($path_to_root . "/includes/ui/items_cart.inc");

include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/inventory/includes/item_adjustments_ui.inc");
include_once($path_to_root . "/inventory/includes/inventory_db.inc");
$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(800, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();
page(_("Item Adjustments Note"), false, false, "", $js);

//-----------------------------------------------------------------------------------------------

check_db_has_costable_items(_("There are no inventory items defined in the system which can be adjusted (Purchased or Manufactured)."));

check_db_has_movement_types(_("There are no inventory movement types defined in the system. Please define at least one inventory adjustment type."));

//-----------------------------------------------------------------------------------------------

if (isset($_GET['AddedID'])) 
{
	$trans_no = $_GET['AddedID'];
	$trans_type = systypes::inventory_adjustment();

	display_notification_centered(_("Items adjustment has been processed"));
	display_note(get_trans_view_str($trans_type, $trans_no, _("View this adjustment")));

	display_note(get_gl_view_str($trans_type, $trans_no, _("View the GL Postings for this Adjustment")), 1, 0);

	hyperlink_no_params($_SERVER['PHP_SELF'], _("Enter Another Adjustment"));

	display_footer_exit();
}

//--------------------------------------------------------------------------------------------------

function copy_to_st()
{
	$_SESSION['adj_items']->from_loc = $_POST['StockLocation'];
	$_SESSION['adj_items']->tran_date = $_POST['AdjDate'];
	$_SESSION['adj_items']->transfer_type = $_POST['type'];
	$_SESSION['adj_items']->increase = $_POST['Increase'];
	$_SESSION['adj_items']->memo_ = $_POST['memo_'];
}

//--------------------------------------------------------------------------------------------------

function copy_from_st()
{
	$_POST['StockLocation'] = $_SESSION['adj_items']->from_loc;
	$_POST['AdjDate'] = $_SESSION['adj_items']->tran_date;
	$_POST['type'] = $_SESSION['adj_items']->transfer_type;
	$_POST['Increase'] = $_SESSION['adj_items']->increase;
	$_POST['memo_'] = $_SESSION['adj_items']->memo_;
}

//-----------------------------------------------------------------------------------------------

function handle_new_order()
{
	if (isset($_SESSION['adj_items']))
	{
		$_SESSION['adj_items']->clear_items();
		unset ($_SESSION['adj_items']);
	}

    session_register("adj_items");

    $_SESSION['adj_items'] = new items_cart;
	$_POST['AdjDate'] = Today();
	if (!is_date_in_fiscalyear($_POST['AdjDate']))
		$_POST['AdjDate'] = end_fiscalyear();
	$_SESSION['adj_items']->tran_date = $_POST['AdjDate'];	
}

//-----------------------------------------------------------------------------------------------

function can_process()
{
	if (!references::is_valid($_POST['ref'])) 
	{
		display_error( _("You must enter a reference."));
		set_focus('ref');
		return false;
	}

	if (!is_new_reference($_POST['ref'], systypes::inventory_adjustment())) 
	{
		display_error( _("The entered reference is already in use."));
		set_focus('ref');
		return false;
	}

	if (!is_date($_POST['AdjDate'])) 
	{
		display_error(_("The entered date for the adjustment is invalid."));
		set_focus('AdjDate');
		return false;
	} 
	elseif (!is_date_in_fiscalyear($_POST['AdjDate'])) 
	{
		display_error(_("The entered date is not in fiscal year."));
		set_focus('AdjDate');
		return false;
	}
	$failed_item = $_SESSION['adj_items']->check_qoh($_POST['StockLocation'], $_POST['AdjDate'], !$_POST['Increase']);
	if ($failed_item != null) 
	{
    	display_error(_("The adjustment cannot be processed because an adjustment item would cause a negative inventory balance :") .
    		" " . $failed_item->stock_id . " - " .  $failed_item->item_description);
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------

if (isset($_POST['Process']) && can_process()){

	$trans_no = add_stock_adjustment($_SESSION['adj_items']->line_items,
		$_POST['StockLocation'], $_POST['AdjDate'],	$_POST['type'], $_POST['Increase'],
		$_POST['ref'], $_POST['memo_']);

	$_SESSION['adj_items']->clear_items();
	unset($_SESSION['adj_items']);
   	meta_forward($_SERVER['PHP_SELF'], "AddedID=$trans_no");
} /*end of process credit note */

//-----------------------------------------------------------------------------------------------

function check_item_data()
{
	if (!check_num('qty',0))
	{
		display_error(_("The quantity entered is negative or invalid."));
		set_focus('qty');
		return false;
	}

	if (!check_num('std_cost', 0))
	{
		display_error(_("The entered standard cost is negative or invalid."));
		set_focus('std_cost');
		return false;
	}

   	return true;
}

//-----------------------------------------------------------------------------------------------

function handle_update_item()
{
    if($_POST['UpdateItem'] != "" && check_item_data())
    {
    	$_SESSION['adj_items']->update_cart_item($_POST['stock_id'], 
		  input_num('qty'), input_num('std_cost'));
    }
}

//-----------------------------------------------------------------------------------------------

function handle_delete_item()
{
	$_SESSION['adj_items']->remove_from_cart($_GET['Delete']);
}

//-----------------------------------------------------------------------------------------------

function handle_new_item()
{
	if (!check_item_data())
		return;

	add_to_order($_SESSION['adj_items'], $_POST['stock_id'], 
	  input_num('qty'), input_num('std_cost'));
}

//-----------------------------------------------------------------------------------------------

if (isset($_GET['Delete']) || isset($_GET['Edit']))
	copy_from_st();

if (isset($_GET['Delete']))
	handle_delete_item();

if (isset($_POST['AddItem']) || isset($_POST['UpdateItem']))
	copy_to_st();
	
if (isset($_POST['AddItem']))
	handle_new_item();

if (isset($_POST['UpdateItem']))
	handle_update_item();

//-----------------------------------------------------------------------------------------------

if (isset($_GET['NewAdjustment']) || !isset($_SESSION['adj_items']))
{
	handle_new_order();
}

//-----------------------------------------------------------------------------------------------

start_form(false, true);

display_order_header($_SESSION['adj_items']);

start_table("$table_style width=70%", 10);
start_row();
echo "<TD>";
display_adjustment_items(_("Adjustment Items"), $_SESSION['adj_items']);
adjustment_options_controls();
echo "</TD>";
end_row();
end_table(1);

start_table();
start_row();
submit_cells('Update', _("Update"));
if ($_SESSION['adj_items']->count_items() >= 1)
{
    submit_cells('Process', _("Process Adjustment"));
}
end_row();
end_table();

end_form();
end_page();

?>
