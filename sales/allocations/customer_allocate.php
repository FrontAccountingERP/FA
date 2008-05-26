<?php

$path_to_root="../..";
$page_security = 3;

include($path_to_root . "/includes/ui/allocation_cart.inc");
include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/data_checks.inc");
include_once($path_to_root . "/sales/includes/sales_ui.inc");
include_once($path_to_root . "/sales/includes/sales_db.inc");

$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(900, 500);

add_js_file('allocate.js');

page(_("Allocate Customer Payment or Credit Note"), false, false, "", $js);

//--------------------------------------------------------------------------------

function clear_allocations()
{
	if (isset($_SESSION['alloc']))
	{
		unset($_SESSION['alloc']->allocs);
		unset($_SESSION['alloc']);
	}
	session_register('alloc');
}

//--------------------------------------------------------------------------------

function check_data()
{
	$total_allocated = 0;

	for ($counter = 0; $counter < $_POST["TotalNumberOfAllocs"]; $counter++)
	{

		if (!check_num('amount' . $counter))
		{
			display_error(_("The entry for one or more amounts is invalid."));
			set_focus('amount'.$counter);
			return false;
		}

		if (!check_num('amount' . $counter,0))
		{
			display_error(_("The entry for an amount to allocate was negative. A positive allocation amount is expected."));
		set_focus('amount'.$counter);
			return false;
		}

		  /*Now check to see that the AllocAmt is no greater than the
		amount left to be allocated against the transaction under review */
		if (input_num('amount' . $counter) > $_POST['un_allocated' . $counter])
		{
		    //$_POST['amount' . $counter] = $_POST['un_allocated' . $counter];
		}

		$_SESSION['alloc']->allocs[$counter]->current_allocated = input_num('amount' . $counter);

		$total_allocated += input_num('amount' . $counter);
	}

	if ($total_allocated - $_SESSION['alloc']->amount > sys_prefs::allocation_settled_allowance())
	{
		display_error(_("These allocations cannot be processed because the amount allocated is more than the total amount left to allocate."));
	   	//echo  _("Total allocated:") . " " . $total_allocated ;
	   	//echo "  " . _("Total amount that can be allocated:") . " " . -$_SESSION['alloc']->TransAmt . "<BR>";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------------

function handle_process()
{
	begin_transaction();

	// clear all the allocations for this payment/credit
	clear_cust_alloctions($_SESSION['alloc']->type,	$_SESSION['alloc']->trans_no);

	// now add the new allocations
	$total_allocated = 0;
	foreach ($_SESSION['alloc']->allocs as $allocn_item)
	{
		if ($allocn_item->current_allocated > 0)
		{
			add_cust_allocation($allocn_item->current_allocated,
				$_SESSION['alloc']->type, $_SESSION['alloc']->trans_no,
		     	$allocn_item->type, $allocn_item->type_no, $_SESSION['alloc']->date_);

			update_debtor_trans_allocation($allocn_item->type, $allocn_item->type_no,
				$allocn_item->current_allocated);
			$total_allocated += $allocn_item->current_allocated;

		}

	}  /*end of the loop through the array of allocations made */

	update_debtor_trans_allocation($_SESSION['alloc']->type,
		$_SESSION['alloc']->trans_no, $total_allocated);

	commit_transaction();

	clear_allocations();
}

//--------------------------------------------------------------------------------

if (isset($_POST['Process']))
{
	if (check_data())
	{
		handle_process();
		$_POST['Cancel'] = 1;
	}
}

//--------------------------------------------------------------------------------

if (isset($_POST['Cancel']))
{
	clear_allocations();
	meta_forward($path_to_root . "/sales/allocations/customer_allocation_main.php");
	exit;
}

//--------------------------------------------------------------------------------

function get_allocations_for_transaction($type, $trans_no)
{
	clear_allocations();

	$debtor = get_customer_trans($trans_no, $type);

	$_SESSION['alloc'] = new allocation($trans_no, $type, $debtor["debtor_no"],
		$debtor["DebtorName"], $debtor["Total"], sql2date($debtor["tran_date"]));

	/* Now populate the array of possible (and previous actual) allocations for this customer */
	/*First get the transactions that have outstanding balances ie Total-alloc >0 */

	$trans_items = get_allocatable_to_cust_transactions($_SESSION['alloc']->person_id);

	while ($myrow = db_fetch($trans_items))
	{
		$_SESSION['alloc']->add_item($myrow["type"], $myrow["trans_no"],
			sql2date($myrow["tran_date"]), sql2date($myrow["due_date"]),
			$myrow["Total"], // trans total
			$myrow["alloc"], // trans total allocated
			0); // this allocation
	}


	/* Now get trans that might have previously been allocated to by this trans
	NB existing entries where still some of the trans outstanding entered from
	above logic will be overwritten with the prev alloc detail below */

	$trans_items = get_allocatable_to_cust_transactions($_SESSION['alloc']->person_id, $trans_no, $type);

	while ($myrow = db_fetch($trans_items))
	{
		$_SESSION['alloc']->add_or_update_item ($myrow["type"], $myrow["trans_no"],
			sql2date($myrow["tran_date"]), sql2date($myrow["due_date"]),
			$myrow["Total"], $myrow["alloc"] - $myrow["amt"], $myrow["amt"]);
	}
}

//--------------------------------------------------------------------------------

function edit_allocations_for_transaction($type, $trans_no)
{
	global $table_style;

    display_heading(sprintf(_("Allocation of %s # %d"), systypes::name($_SESSION['alloc']->type),$_SESSION['alloc']->trans_no));

    display_heading($_SESSION['alloc']->person_name);

    display_heading2(_("Date:") . " <b>" . $_SESSION['alloc']->date_ . "</b>");
    display_heading2(_("Total:") . " <b>" . price_format($_SESSION['alloc']->amount) . "</b>");

    echo "<br>";

	start_form(false, true);

    if (count($_SESSION['alloc']->allocs) > 0)
    {
		start_table($table_style);

   		$th = array(_("Transaction Type"), _("#"), _("Date"), _("Due Date"), _("Amount"),
   			_("Other Allocations"), _("This Allocation"), _("Left to Allocate"), "", "");

		table_header($th);

        $k = $counter = $total_allocated = 0;

        foreach ($_SESSION['alloc']->allocs as $allocn_item)
        {
    		alt_table_row_color($k);

    	    label_cell(systypes::name($allocn_item->type));
    		label_cell(get_trans_view_str($allocn_item->type, $allocn_item->type_no));
    		label_cell($allocn_item->date_, "align=right");
    		label_cell($allocn_item->due_date, "align=right");
    		amount_cell($allocn_item->amount);
			amount_cell($allocn_item->amount_allocated);

    	    if (!check_num('amount' . $counter))
    	    	$_POST['amount' . $counter] = price_format($allocn_item->current_allocated);
    	    amount_cells(null, 'amount' . $counter, $_POST['amount' . $counter]);

    		$un_allocated = round($allocn_item->amount - $allocn_item->amount_allocated, 6);
    		//hidden("un_allocated" . $counter, $un_allocated);
    		amount_cell($un_allocated);

			label_cell("<a href='#' name='Alloc$counter' onclick='allocate_all(this.name.substr(5));return true;'>"
					 . _("All") . "</a>");
			//label_cell("<a href='#' name='DeAll$counter' onclick='allocate_none(this.name.substr(5));return true;'>"
			//		 . _("None") . "</a>");
			label_cell("<a href='#' name='DeAll$counter' onclick='allocate_none(this.name.substr(5));return true;'>"
					 . _("None") . "</a>".hidden("un_allocated" . $counter, $un_allocated, false));
			end_row();

    	    $total_allocated += input_num('amount' . $counter);
    	    $counter++;
       	}

       	label_row(_("Total Allocated"), price_format($total_allocated),
       		"colspan=6 align=right", "nowrap align=right id='total_allocated'");
        if ($_SESSION['alloc']->amount - $total_allocated < 0)
        {
        	$font1 = "<font color=red>";
        	$font2 = "</font>";
        }
        else
        	$font1 = $font2 = "";
		$left_to_allocate = $_SESSION['alloc']->amount - $total_allocated;
		$left_to_allocate = price_format($left_to_allocate);
        label_row(_("Left to Allocate"), $font1 . $left_to_allocate . $font2,
	    	"colspan=6 align=right ", "nowrap align=right id='left_to_allocate'");
        end_table(1);

       	hidden('TotalNumberOfAllocs', $counter);
//		hidden('left_to_allocate', $left_to_allocate);
       	submit_center_first('UpdateDisplay', _("Update"));
       	submit('Process', _("Process"));
   		submit_center_last('Cancel', _("Back to Allocations"));
	}
	else
	{
    	display_note(_("There are no unsettled transactions to allocate."), 0, 1);
   		submit_center('Cancel', _("Back to Allocations"));
    }

  	end_form();
}

//--------------------------------------------------------------------------------

if (isset($_GET['trans_no']) && isset($_GET['trans_type']))
{
	get_allocations_for_transaction($_GET['trans_type'], $_GET['trans_no']);
}

if (isset($_SESSION['alloc']))
{
	edit_allocations_for_transaction($_SESSION['alloc']->type, $_SESSION['alloc']->trans_no);
}

//--------------------------------------------------------------------------------

end_page();

?>