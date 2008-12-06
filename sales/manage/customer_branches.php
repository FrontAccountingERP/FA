<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU Affero General Public License,
	AGPL, as published by the Free Software Foundation, either version 
	3 of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/agpl-3.0.html>.
***********************************************************************/
$page_security = 3;
$path_to_root="../..";
include($path_to_root . "/includes/session.inc");

page(_("Customer Branches"));

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
	$_POST['branch_code'] = strtoupper($_GET['SelectedBranch']);
	$selected_id = $_GET['SelectedBranch'];
}

$id = find_submit('Select');
if ($id != -1)
{
	context_return(array('customer_id' => $_POST['customer_id'],
		'branch_id' => $id)); // return to sales document
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

	if ($input_error != 1)
	{

    	if ($selected_id != -1)
		{
			/*SelectedBranch could also exist if submit had not been clicked this code would not run in this case cos submit is false of course see the 	delete code below*/

			$sql = "UPDATE ".TB_PREF."cust_branch SET br_name = " . db_escape($_POST['br_name']) . ",
				br_address = ".db_escape($_POST['br_address']). ",
    	        phone=".db_escape($_POST['phone']). ",
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
    	        default_ship_via=".db_escape($_POST['default_ship_via']) . "
    	        WHERE branch_code =".db_escape($_POST['branch_code']) . "
    	        AND debtor_no=".db_escape($_POST['customer_id']);

			$note =_('Selected customer branch has been updated');
		}
		else
		{
			/*Selected branch is null cos no item selected on first time round so must be adding a	record must be submitting new entries in the new Customer Branches form */
			$sql = "INSERT INTO ".TB_PREF."cust_branch (debtor_no, br_name, br_address,
				salesman, phone, fax,
				contact_name, area, email, tax_group_id, sales_account, receivables_account, payment_discount_account, sales_discount_account, default_location,
				br_post_address, disable_trans, group_no, default_ship_via)
				VALUES (".db_escape($_POST['customer_id']). ",".db_escape($_POST['br_name']) . ", "
					.db_escape($_POST['br_address']) . ", ".db_escape($_POST['salesman']) . ", "
					.db_escape($_POST['phone']) . ", ".db_escape($_POST['fax']) . ","
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
					.db_escape($_POST['default_ship_via']) . ")";

			$note = _('New customer branch has been added');
		}

		//run the sql from either of the above possibilites
		db_query($sql,"The branch record could not be inserted or updated");
		display_notification($note);
		$Mode = 'RESET';
	}

}
elseif ($Mode == 'Delete')
{
	//the link to delete a selected record was clicked instead of the submit button

	// PREVENT DELETES IF DEPENDENT RECORDS IN 'debtor_trans'

	$sql= "SELECT COUNT(*) FROM ".TB_PREF."debtor_trans WHERE branch_code='" . $_POST['branch_code']. "' AND debtor_no = '" . $_POST['customer_id']. "'";
	$result = db_query($sql,"could not query debtortrans");
	$myrow = db_fetch_row($result);
	if ($myrow[0] > 0)
	{
		display_error(_("Cannot delete this branch because customer transactions have been created to this branch."));

	}
	else
	{
		$sql= "SELECT COUNT(*) FROM ".TB_PREF."sales_orders WHERE branch_code='" . $_POST['branch_code']. "' AND debtor_no = '" . $_POST['customer_id']. "'";
		$result = db_query($sql,"could not query sales orders");

		$myrow = db_fetch_row($result);
		if ($myrow[0] > 0)
		{
			display_error(_("Cannot delete this branch because sales orders exist for it. Purge old sales orders first."));
		}
		else
		{
			$sql="DELETE FROM ".TB_PREF."cust_branch WHERE branch_code='" . $_POST['branch_code']. "' AND debtor_no='" . $_POST['customer_id']. "'";
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
	unset($_POST);
	$_POST['customer_id'] = $cust_id;
	$Ajax->activate('_page_body');
}
start_form();

echo "<center>" . _("Select a customer: ") . "&nbsp;&nbsp;";
customer_list('customer_id', null, false, true);
echo "</center><br><br>";

$num_branches = db_customer_has_branches($_POST['customer_id']);

if ($num_branches)
{
	$sql = "SELECT ".TB_PREF."debtors_master.name, ".TB_PREF."cust_branch.*, ".TB_PREF."salesman.salesman_name,
		".TB_PREF."areas.description, ".TB_PREF."tax_groups.name AS tax_group_name
		FROM ".TB_PREF."cust_branch, ".TB_PREF."debtors_master, ".TB_PREF."areas, ".TB_PREF."salesman, ".TB_PREF."tax_groups
		WHERE ".TB_PREF."cust_branch.debtor_no=".TB_PREF."debtors_master.debtor_no
		AND ".TB_PREF."cust_branch.tax_group_id=".TB_PREF."tax_groups.id
		AND ".TB_PREF."cust_branch.area=".TB_PREF."areas.area_code
		AND ".TB_PREF."cust_branch.salesman=".TB_PREF."salesman.salesman_code
		AND ".TB_PREF."cust_branch.debtor_no = '" . $_POST['customer_id']. "'";

	$result = db_query($sql,"could not get customer branches");

	start_table("$table_style width=60%");

	$th = array(_("Name"), _("Contact"), _("Sales Person"), _("Area"),
		_("Phone No"), _("Fax No"), _("E-mail"), _("Tax Group"), "", "");
	if (count($_SESSION['Context'])) $th[] = '';
	table_header($th);

	while ($myrow = db_fetch($result))
	{
		start_row();
		label_cell($myrow["br_name"]);
		label_cell($myrow["contact_name"]);
		label_cell($myrow["salesman_name"]);
		label_cell($myrow["description"]);
		label_cell($myrow["phone"]);
		label_cell($myrow["fax"]);
		label_cell("<a href=mailto:" . $myrow["email"]. ">" . $myrow["email"]. "</a>");
		label_cell($myrow["tax_group_name"]);
		if (count($_SESSION['Context']))
 			edit_button_cell("Select".$myrow["branch_code"], _("Select"));
 		edit_button_cell("Edit".$myrow["branch_code"], _("Edit"));
 		delete_button_cell("Delete".$myrow["branch_code"], _("Delete"));
		end_row();
	}
	end_table();
	//END WHILE LIST LOOP
}
else
	display_note(_("The selected customer does not have any branches. Please create at least one branch."));

echo "<br>";
start_table("$table_style2 width=70%", 5);
echo "<tr valign=top><td>"; // outer table

echo "<table>";


$_POST['email'] = "";
if ($selected_id != -1)
{
 	if ($Mode == 'Edit') {

		//editing an existing branch
    	$sql = "SELECT * FROM ".TB_PREF."cust_branch
			WHERE branch_code='" . $_POST['branch_code'] . "'
			AND debtor_no='" . $_POST['customer_id'] . "'";
		$result = db_query($sql,"check failed");
	    $myrow = db_fetch($result);
		set_focus('br_name');
    	$_POST['branch_code'] = $myrow["branch_code"];
	    $_POST['br_name']  = $myrow["br_name"];
	    $_POST['br_address']  = $myrow["br_address"];
	    $_POST['br_post_address']  = $myrow["br_post_address"];
	    $_POST['contact_name'] = $myrow["contact_name"];
	    $_POST['salesman'] =$myrow["salesman"];
	    $_POST['area'] =$myrow["area"];
	    $_POST['phone'] =$myrow["phone"];
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
	}
}
elseif ($Mode != 'ADD_ITEM')
{ //end of if $SelectedBranch only do the else when a new record is being entered
	if(!$num_branches) {
		$sql = "SELECT name, address, email
			FROM ".TB_PREF."debtors_master WHERE debtor_no = '" . $_POST['customer_id']. "'";
		$result = db_query($sql,"check failed");
		$myrow = db_fetch($result);
		$_POST['br_name'] = $myrow["name"];
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
	    $_POST['sales_account'] = "";
	    $_POST['sales_discount_account'] = $company_record['default_sales_discount_act'];
	    $_POST['receivables_account'] = $company_record['debtors_act'];
	    $_POST['payment_discount_account'] = $company_record['default_prompt_payment_act'];

	}

}
hidden('selected_id', $selected_id);
hidden('branch_code');

table_section_title(_("Name and Contact"));

text_row(_("Branch Name:"), 'br_name', null, 35, 40);
text_row(_("Contact Person:"), 'contact_name', null, 35, 40);

text_row(_("Phone Number:"), 'phone', null, 20, 20);
text_row(_("Fax Number:"), 'fax', null, 20, 20);

text_row("<a href='Mailto:".$_POST['email']."'>" . _("E-mail:") . "</a>", 'email', null, 35, 55);

table_section_title(_("Sales"));

sales_persons_list_row( _("Sales Person:"), 'salesman', null);

sales_areas_list_row( _("Sales Area:"), 'area', null);

locations_list_row(_("Default Inventory Location:"), 'default_location', null);

shippers_list_row(_("Default Shipping Company:"), 'default_ship_via', null);

tax_groups_list_row(_("Tax Group:"), 'tax_group_id', null);

yesno_list_row(_("Disable this Branch:"), 'disable_trans', null);

sales_groups_list_row(_("Sales Group:"), 'group_no', null, true);

echo "</table>";

echo "</td><td  class='tableseparator'>"; // outer table

echo"<table>";

table_section_title(_("GL Accounts"));

// 2006-06-14. Changed gl_al_accounts_list to have an optional all_option 'Use Item Sales Accounts'
gl_all_accounts_list_row(_("Sales Account:"), 'sales_account', null, false,	false, false, true);

gl_all_accounts_list_row(_("Sales Discount Account:"), 'sales_discount_account');

gl_all_accounts_list_row(_("Accounts Receivable Account:"), 'receivables_account');

gl_all_accounts_list_row(_("Prompt Payment Discount Account:"), 'payment_discount_account');

table_section_title(_("Addresses"));

textarea_row(_("Mailing Address:"), 'br_post_address', null, 35, 5);

textarea_row(_("Billing Address:"), 'br_address', null, 35, 5);

end_table();

end_table(1); // outer table

submit_add_or_update_center($selected_id == -1, '', true);

end_form();

end_page();

?>
