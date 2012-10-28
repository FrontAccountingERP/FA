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
$page_security = 'SA_SUPPLIERALLOC';
$path_to_root = "../..";

include($path_to_root . "/includes/ui/allocation_cart.inc");
include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/banking.inc");
include_once($path_to_root . "/sales/includes/sales_db.inc");
//include_once($path_to_root . "/purchasing/includes/ui/supp_alloc_ui.inc");

$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);

add_js_file('allocate.js');

page(_($help_context = "Allocate Supplier Payment or Credit Note"), false, false, "", $js);

//--------------------------------------------------------------------------------

function clear_allocations()
{
	if (isset($_SESSION['alloc']))
	{
		unset($_SESSION['alloc']->allocs);
		unset($_SESSION['alloc']);
	}
}
//--------------------------------------------------------------------------------

function edit_allocations_for_transaction($type, $trans_no)
{
	global $systypes_array;

	start_form();

    display_heading(_("Allocation of") . " " . $systypes_array[$_SESSION['alloc']->type] . " # " . $_SESSION['alloc']->trans_no);

	display_heading($_SESSION['alloc']->person_name);

    display_heading2(_("Date:") . " <b>" . $_SESSION['alloc']->date_ . "</b>");
    display_heading2(_("Total:") . " <b>" . price_format(-$_SESSION['alloc']->amount) . "</b>");

    echo "<br>";

  	div_start('alloc_tbl');
    if (count($_SESSION['alloc']->allocs) > 0)
    {
		show_allocatable(true);

     	submit_center_first('UpdateDisplay', _("Refresh"), _('Start again allocation of selected amount'), true);
       	submit('Process', _("Process"), true, _('Process allocations'), 'default');
   		submit_center_last('Cancel', _("Back to Allocations"),
			_('Abandon allocations and return to selection of allocatable amounts'), 'cancel');
	}
	else
	{
    	display_note(_("There are no unsettled transactions to allocate."), 0, 1);
   		submit_center('Cancel', _("Back to Allocations"), true,
			_('Abandon allocations and return to selection of allocatable amounts'), 'cancel');
    }

	div_end();
	end_form();
}
//--------------------------------------------------------------------------------

if (isset($_POST['Process']))
{
	if (check_allocations())
	{
		$_SESSION['alloc']->write();
		clear_allocations();
		$_POST['Cancel'] = 1;
	}
}

//--------------------------------------------------------------------------------

if (isset($_POST['Cancel']))
{
	clear_allocations();
	meta_forward($path_to_root . "/purchasing/allocations/supplier_allocation_main.php");
}

//--------------------------------------------------------------------------------

if (isset($_GET['trans_no']) && isset($_GET['trans_type']))
{
	$_SESSION['alloc'] = new allocation($_GET['trans_type'], $_GET['trans_no']);
}

if (get_post('UpdateDisplay'))
{
	$_SESSION['alloc']->read();
	$Ajax->activate('alloc_tbl');
}

if (isset($_SESSION['alloc']))
{
	edit_allocations_for_transaction($_SESSION['alloc']->type, $_SESSION['alloc']->trans_no);
}

//--------------------------------------------------------------------------------

end_page();

?>