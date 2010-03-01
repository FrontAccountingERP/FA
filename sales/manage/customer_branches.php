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
$page_security = 'SA_CUSTOMER';
//$page_security = 3;
$path_to_root="../..";
include($path_to_root . "/includes/db_pager.inc");
include($path_to_root . "/includes/session.inc");

page(_($help_context = "Customer Branches"), @$_REQUEST['popup']);

include($path_to_root . "/includes/ui.inc");

//-----------------------------------------------------------------------------------------------

check_db_has_customers(_("There are no customers defined in the system. Please define a customer to add customer branches."));

check_db_has_sales_people(_("There are no sales people defined in the system. At least one sales person is required before proceeding."));

check_db_has_sales_areas(_("There are no sales areas defined in the system. At least one sales area is required before proceeding."));

check_db_has_shippers(_("There are no shipping companies defined in the system. At least one shipping company is required before proceeding."));

check_db_has_tax_groups(_("There are no tax groups defined in the system. At least one tax group is required before proceeding."));

simple_page_mode(true);
//-----------------------------------------------------------------------------------------------

if (isset($_GET['debtor_no']))
{
	$_POST['customer_id'] = strtoupper($_GET['debtor_no']);
}
$_POST['branch_code'] = $selected_id;

if (isset($_GET['SelectedBranch']))
{
	$br = get_branch($_GET['SelectedBranch']);
	$_POST['customer_id'] = $br['debtor_no'];
	$selected_id = $_POST['branch_code'] = $br['branch_code'];
	$Mode = 'Edit';
}
//-----------------------------------------------------------------------------------------------

if ($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM')
{

	//initialise no input errors assumed initially before we test
	$input_error = 0;

	//first off validate inputs sensible

	if (strlen($_POST['br_name']) == 0)
	{
		$input_error = 1;
		display_error(_("The Branch name cannot be empty."));
		set_focus('br_name');
	}

	if (strlen($_POST['br_ref']) == 0)
	{
		$input_error = 1;
		display_error(_("The Branch short name cannot be empty."));
		set_focus('br_ref');
	}

	if ($input_error != 1)
	{

    	if ($selected_id != -1)
		{
			/*SelectedBranch could also exist if submit had not been clicked this code would not run in this case cos submit is false of course see the 	delete code below*/

			$sql = "UPDATE ".TB_PREF."cust_branch SET br_name = " . db_escape($_POST['br_name']) . ",
				branch_ref = " . db_escape($_POST['br_ref']) . ",
				br_address = ".db_escape($_POST['br_address']). ",
    	        phone=".db_escape($_POST['phone']). ",
    	        phone2=".db_escape($_POST['phone2']). ",
    	        fax=".db_escape($_POST['fax']).",
    	        contact_name=".db_escape($_POST['contact_name']) . ",
    	        salesman= ".db_escape($_POST['salesman']) . ",
    	        area=".db_escape($_POST['area']) . ",
    	        email=".db_escape($_POST['email']) . ",
    	        tax_group_id=".db_escape($_POST['tax_group_id']). ",
				sales_account=".db_escape($_POST['sales_account']) . ",
				sales_discount_account=".db_escape($_POST['sales_discount_account']) . ",
				receivables_account=".db_escape($_POST['receivables_account']) . ",
				payment_discount_account=".db_escape($_POST['payment_discount_account']) . ",
    	        default_location=".db_escape($_POST['default_location']) . ",
    	        br_post_address =".db_escape($_POST['br_post_address']) . ",
    	        disable_trans=".db_escape($_POST['disable_trans']) . ",
				group_no=".db_escape($_POST['group_no']) . ", 
    	        default_ship_via=".db_escape($_POST['default_ship_via']) . ",
                notes=".db_escape($_POST['notes']) . "
    	        WHERE branch_code =".db_escape($_POST['branch_code']) . "
    	        AND debtor_no=".db_escape($_POST['customer_id']);

			$note =_('Selected customer branch has been updated');
		}
		else
		{
			/*Selected branch is null cos no item selected on first time round so must be adding a	record must be submitting new entries in the new Customer Branches form */
			$sql = "INSERT INTO ".TB_PREF."cust_branch (debtor_no, br_name, branch_ref, br_address,
				salesman, phone, phone2, fax,
				contact_name, area, email, tax_group_id, sales_account, receivables_account, payment_discount_account, sales_discount_account, default_location,
				br_post_address, disable_trans, group_no, default_ship_via, notes)
				VALUES (".db_escape($_POST['customer_id']). ",".db_escape($_POST['br_name']) . ", "
					.db_escape($_POST['br_ref']) . ", "
					.db_escape($_POST['br_address']) . ", ".db_escape($_POST['salesman']) . ", "
					.db_escape($_POST['phone']) . ", ".db_escape($_POST['phone2']) . ", "
					.db_escape($_POST['fax']) . ","
					.db_escape($_POST['contact_name']) . ", ".db_escape($_POST['area']) . ","
					.db_escape($_POST['email']) . ", ".db_escape($_POST['tax_group_id']) . ", "
					.db_escape($_POST['sales_account']) . ", "
					.db_escape($_POST['receivables_account']) . ", "
					.db_escape($_POST['payment_discount_account']) . ", "
					.db_escape($_POST['sales_discount_account']) . ", "
					.db_escape($_POST['default_location']) . ", "
					.db_escape($_POST['br_post_address']) . ","
					.db_escape($_POST['disable_trans']) . ", "
					.db_escape($_POST['group_no']) . ", "
					.db_escape($_POST['default_ship_via']). ", "
					.db_escape($_POST['notes']) . ")";

			$note = _('New customer branch has been added');
		}
		//run the sql from either of the above possibilites
		db_query($sql,"The branch record could not be inserted or updated");
		display_notification($note);
		$Mode = 'RESET';
		if (@$_REQUEST['popup']) {
			set_focus("Select".($_POST['branch_code'] == -1 
				? db_insert_id(): $_POST['branch_code']));
		}
	}

}
elseif ($Mode == 'Delete')
{
	//the link to delete a selected record was clicked instead of the submit button

	// PREVENT DELETES IF DEPENDENT RECORDS IN 'debtor_trans'

	$sql= "SELECT COUNT(*) FROM ".TB_PREF."debtor_trans WHERE branch_code=".db_escape($_POST['branch_code'])." AND debtor_no = ".db_escape($_POST['customer_id']);
	$result = db_query($sql,"could not query debtortrans");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0)
	{
		display_error(_("Cannot delete this branch because customer transactions have been created to this branch."));

	}
	else
	{
		$sql= "SELECT COUNT(*) FROM ".TB_PREF."sales_orders WHERE branch_code=".db_escape($_POST['branch_code'])." AND debtor_no = ".db_escape($_POST['customer_id']);
		$result = db_query($sql,"could not query sales orders");

		$myrow = db_fetch_row($result);
		if ($myrow[0] > 0)
		{
			display_error(_("Cannot delete this branch because sales orders exist for it. Purge old sales orders first."));
		}
		else
		{
			$sql="DELETE FROM ".TB_PREF."cust_branch WHERE branch_code=".db_escape($_POST['branch_code'])." AND debtor_no=".db_escape($_POST['customer_id']);
			db_query($sql,"could not delete branch");
			display_notification(_('Selected customer branch has been deleted'));
		}
	} //end ifs to test if the branch can be deleted
	$Mode = 'RESET';
}

if ($Mode == 'RESET' || get_post('_customer_id_update'))
{
	$selected_id = -1;
	$cust_id = $_POST['customer_id'];
	$inact = get_post('show_inactive');
	unset($_POST);
	$_POST['show_inactive'] = $inact;
	$_POST['customer_id'] = $cust_id;
	$Ajax->activate('_page_body');
}

function branch_email($row) {
	return	'<a href = "mailto:'.$row["email"].'">'.$row["email"].'</a>';
}

function edit_link($row) {
	return button("Edit".$row["branch_code"],_("Edit"), '', ICON_EDIT);
}

function del_link($row) {
	return button("Delete".$row["branch_code"],_("Delete"), '', ICON_DELETE);
}

function select_link($row) {
	return button("Select".$row["branch_code"], $row["branch_code"], '', ICON_ADD, 'selector');
}

start_form();

echo "<center>" . _("Select a customer: ") . "&nbsp;&nbsp;";
echo customer_list('customer_id', null, false, true);
echo "</center><br>";

$num_branches = db_customer_has_branches($_POST['customer_id']);

	$sql = "SELECT "
		."b.branch_code, "
		."b.branch_ref, "
		."b.br_name, "
		."b.contact_name, "
		."s.salesman_name, "
		."a.description, "
		."b.phone, "
		."b.fax, "
		."b.email, "
		."t.name AS tax_group_name, "
		."b.inactive
		FROM ".TB_PREF."cust_branch b, "
			.TB_PREF."debtors_master c, "
			.TB_PREF."areas a, "
			.TB_PREF."salesman s, "
			.TB_PREF."tax_groups t
		WHERE b.debtor_no=c.debtor_no
		AND b.tax_group_id=t.id
		AND b.area=a.area_code
		AND b.salesman=s.salesman_code
		AND b.debtor_no = ".db_escape($_POST['customer_id']);

	if (!get_post('show_inactive')) $sql .= " AND !b.inactive";
//------------------------------------------------------------------------------------------------
if ($num_branches)
{
$cols = array(
	'branch_code' => 'skip',
	_("Short Name"),
	_("Name"),
	_("Contact"),
	_("Sales Person"),
	_("Area"),
	_("Phone No"),
	_("Fax No"),
	_("E-mail") => 'email',
	_("Tax Group"),
	_("Inactive") => 'inactive',
//		array('fun'=>'inactive'),
		' '=> array('insert'=>true, 'fun'=>'select_link'),
		array('insert'=>true, 'fun'=>'edit_link'),
		array('insert'=>true, 'fun'=>'del_link')
	);

	if (!@$_REQUEST['popup']) {
		$cols[' '] = 'skip';
	}

$table =& new_db_pager('branch_tbl', $sql, $cols, 'cust_branch');
$table->set_inactive_ctrl('cust_branch', 'branch_code');

//$table->width = "85%";
display_db_pager($table);
}
else
	display_note(_("The selected customer does not have any branches. Please create at least one branch."));

start_outer_table($table_style2, 5);

table_section(1);

$_POST['email'] = "";
if ($selected_id != -1)
{
 	if ($Mode == 'Edit') {

		//editing an existing branch
    	$sql = "SELECT * FROM ".TB_PREF."cust_branch
			WHERE branch_code=".db_escape($_POST['branch_code'])."
			AND debtor_no=".db_escape($_POST['customer_id']);
		$result = db_query($sql,"check failed");
	    $myrow = db_fetch($result);
		set_focus('br_name');
    	$_POST['branch_code'] = $myrow["branch_code"];
	    $_POST['br_name']  = $myrow["br_name"];
	    $_POST['br_ref']  = $myrow["branch_ref"];
	    $_POST['br_address']  = $myrow["br_address"];
	    $_POST['br_post_address']  = $myrow["br_post_address"];
	    $_POST['contact_name'] = $myrow["contact_name"];
	    $_POST['salesman'] =$myrow["salesman"];
	    $_POST['area'] =$myrow["area"];
	    $_POST['phone'] =$myrow["phone"];
	    $_POST['phone2'] =$myrow["phone2"];
	    $_POST['fax'] =$myrow["fax"];
	    $_POST['email'] =$myrow["email"];
	    $_POST['tax_group_id'] = $myrow["tax_group_id"];
	    $_POST['disable_trans'] = $myrow['disable_trans'];
	    $_POST['default_location'] = $myrow["default_location"];
	    $_POST['default_ship_via'] = $myrow['default_ship_via'];
	    $_POST['sales_account'] = $myrow["sales_account"];
	    $_POST['sales_discount_account'] = $myrow['sales_discount_account'];
	    $_POST['receivables_account'] = $myrow['receivables_account'];
	    $_POST['payment_discount_account'] = $myrow['payment_discount_account'];
		$_POST['group_no']  = $myrow["group_no"];
		$_POST['notes']  = $myrow["notes"];

	}
}
elseif ($Mode != 'ADD_ITEM')
{ //end of if $SelectedBranch only do the else when a new record is being entered
	if(!$num_branches) {
		$sql = "SELECT name, address, email, debtor_ref
			FROM ".TB_PREF."debtors_master WHERE debtor_no = ".db_escape($_POST['customer_id']);
		$result = db_query($sql,"check failed");
		$myrow = db_fetch($result);
		$_POST['br_name'] = $myrow["name"];
		$_POST['br_ref'] = $myrow["debtor_ref"];
		$_POST['contact_name'] = _('Main Branch');
		$_POST['br_address'] = $_POST['br_post_address'] = $myrow["address"];
		$_POST['email'] = $myrow['email'];
	}
	$_POST['branch_code'] = "";
	if (!isset($_POST['sales_account']) || !isset($_POST['sales_discount_account']))
	{
		$company_record = get_company_prefs();

		// We use the Item Sales Account as default!
	    // $_POST['sales_account'] = $company_record["default_sales_act"];
	    $_POST['sales_account'] = $_POST['notes']  = '';
	    $_POST['sales_discount_account'] = $company_record['default_sales_discount_act'];
	    $_POST['receivables_account'] = $company_record['debtors_act'];
	    $_POST['payment_discount_account'] = $company_record['default_prompt_payment_act'];

	}

}
hidden('selected_id', $selected_id);
hidden('branch_code');
hidden('popup', @$_REQUEST['popup']);

table_section_title(_("Name and Contact"));

text_row(_("Branch Name:"), 'br_name', null, 35, 40);
text_row(_("Branch Short Name:"), 'br_ref', null, 30, 30);
text_row(_("Contact Person:"), 'contact_name', null, 35, 40);

text_row(_("Phone Number:"), 'phone', null, 32, 30);
text_row(_("Secondary Phone Number:"), 'phone2', null, 32, 30);
text_row(_("Fax Number:"), 'fax', null, 32, 30);

email_row(_("E-mail:"), 'email', null, 35, 55);

table_section_title(_("Sales"));

sales_persons_list_row( _("Sales Person:"), 'salesman', null);

sales_areas_list_row( _("Sales Area:"), 'area', null);

sales_groups_list_row(_("Sales Group:"), 'group_no', null, true);

locations_list_row(_("Default Inventory Location:"), 'default_location', null);

shippers_list_row(_("Default Shipping Company:"), 'default_ship_via', null);

tax_groups_list_row(_("Tax Group:"), 'tax_group_id', null);

yesno_list_row(_("Disable this Branch:"), 'disable_trans', null);

table_section(2);

table_section_title(_("GL Accounts"));

// 2006-06-14. Changed gl_al_accounts_list to have an optional all_option 'Use Item Sales Accounts'
gl_all_accounts_list_row(_("Sales Account:"), 'sales_account', null, false, false, true);

gl_all_accounts_list_row(_("Sales Discount Account:"), 'sales_discount_account');

gl_all_accounts_list_row(_("Accounts Receivable Account:"), 'receivables_account');

gl_all_accounts_list_row(_("Prompt Payment Discount Account:"), 'payment_discount_account');

table_section_title(_("Addresses"));

textarea_row(_("Mailing Address:"), 'br_post_address', null, 35, 4);

textarea_row(_("Billing Address:"), 'br_address', null, 35, 4);

textarea_row(_("General Notes:"), 'notes', null, 35, 4);

end_outer_table(1);

submit_add_or_update_center($selected_id == -1, '', 'both');

end_form();

end_page();

?>
