<?php


$page_security = 14;
$path_to_root="..";
include($path_to_root . "/includes/session.inc");
page(_("Shipping Company"));
include($path_to_root . "/includes/ui.inc");


if (isset($_GET['selected_id']))
{
	$selected_id = $_GET['selected_id'];
} 
else if (isset($_POST['selected_id']))
{
	$selected_id = $_POST['selected_id'];
}

//----------------------------------------------------------------------------------------------

function can_process() 
{
	if (strlen($_POST['shipper_name']) == 0) 
	{
		display_error(_("The shipping company name cannot be empty."));
		set_focus('shipper_name');
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------------

if (isset($_POST['ADD_ITEM']) && can_process()) 
{

	$sql = "INSERT INTO ".TB_PREF."shippers (shipper_name, contact, phone, address)
		VALUES (" . db_escape($_POST['shipper_name']) . ", " .
		db_escape($_POST['contact']). ", " .
		db_escape($_POST['phone']). ", " .
		db_escape($_POST['address']) . ")";

	db_query($sql,"The Shipping Company could not be added");
	meta_forward($_SERVER['PHP_SELF']);
}

//----------------------------------------------------------------------------------------------

if (isset($_POST['UPDATE_ITEM']) && can_process()) 
{

	$sql = "UPDATE ".TB_PREF."shippers SET shipper_name=" . db_escape($_POST['shipper_name']). " ,
		contact =" . db_escape($_POST['contact']). " ,
		phone =" . db_escape($_POST['phone']). " ,
		address =" . db_escape($_POST['address']). "
		WHERE shipper_id = $selected_id";

	db_query($sql,"The shipping company could not be updated");
	meta_forward($_SERVER['PHP_SELF']);
}

//----------------------------------------------------------------------------------------------

if (isset($_GET['delete']))
{
// PREVENT DELETES IF DEPENDENT RECORDS IN 'sales_orders'

	$sql= "SELECT COUNT(*) FROM ".TB_PREF."sales_orders WHERE ship_via='$selected_id'";
	$result = db_query($sql,"check failed");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0) 
	{
		$cancel_delete = 1;
		display_error(_("Cannot delete this shipping company because sales orders have been created using this shipper."));
	} 
	else 
	{
		// PREVENT DELETES IF DEPENDENT RECORDS IN 'debtor_trans'

		$sql= "SELECT COUNT(*) FROM ".TB_PREF."debtor_trans WHERE ship_via='$selected_id'";
		$result = db_query($sql,"check failed");
		$myrow = db_fetch_row($result);
		if ($myrow[0] > 0) 
		{
			$cancel_delete = 1;
			display_error(_("Cannot delete this shipping company because invoices have been created using this shipping company."));
		} 
		else 
		{
			$sql="DELETE FROM ".TB_PREF."shippers WHERE shipper_id=$selected_id";
			db_query($sql,"could not delete shipper");

			meta_forward($_SERVER['PHP_SELF']);
		}
	}
}

//----------------------------------------------------------------------------------------------

$sql = "SELECT * FROM ".TB_PREF."shippers ORDER BY shipper_id";
$result = db_query($sql,"could not get shippers");

start_table($table_style);
$th = array(_("Name"), _("Contact Person"), _("Phone Number"), _("Address"), "", "");
table_header($th);

$k = 0; //row colour counter

while ($myrow = db_fetch($result)) 
{
	alt_table_row_color($k);
	label_cell($myrow["shipper_name"]);
	label_cell($myrow["contact"]);
	label_cell($myrow["phone"]);
	label_cell($myrow["address"]);
    edit_link_cell("selected_id=".$myrow[0]);
    delete_link_cell("selected_id=".$myrow[0]."&delete=1");
	end_row();
}

end_table();

//----------------------------------------------------------------------------------------------

hyperlink_no_params($_SERVER['PHP_SELF'], _("New Shipping Company"));

start_form();

start_table($table_style2);

if (isset($selected_id)) 
{
	//editing an existing Shipper

	$sql = "SELECT * FROM ".TB_PREF."shippers WHERE shipper_id=$selected_id";

	$result = db_query($sql, "could not get shipper");
	$myrow = db_fetch($result);

	$_POST['shipper_name']	= $myrow["shipper_name"];
	$_POST['contact']	= $myrow["contact"];
	$_POST['phone']	= $myrow["phone"];
	$_POST['address'] = $myrow["address"];

	hidden('selected_id', $selected_id);
}

text_row_ex(_("Name:"), 'shipper_name', 40);

text_row_ex(_("Contact Person:"), 'contact', 30);

text_row_ex(_("Phone Number:"), 'phone', 20);

text_row_ex(_("Address:"), 'address', 50);

end_table(1);

submit_add_or_update_center(!isset($selected_id));

end_form();
end_page();
?>
