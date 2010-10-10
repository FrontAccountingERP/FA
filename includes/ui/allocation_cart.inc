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
/*
	Class for supplier/customer payment/credit allocations edition
	and related helpers.
*/
//-----------------------------------------------------------------------------------

class allocation 
{

	var $trans_no; 
	var $type;
	var $person_id = '';
	var $person_name = '';
	var $person_type;	// true - supplier, otherwise customer
	var $date_;
	var $amount = 0; /*Total amount of the transaction in FX */
	
	var $allocs; /*array of transactions allocated to */	

	function allocation($type, $trans_no)
	{
		$this->allocs = array();
		
		$this->trans_no = $trans_no;
		$this->type = $type;
		$this->read(); // read payment or credit
	}

	function add_item($type, $type_no, $date_, $due_date, $amount, $amount_allocated, 
		$current_allocated, $ref)
	{
		if ($amount > 0)
		{
			$this->allocs[count($this->allocs)] = new allocation_item($type, $type_no, 
				$date_, $due_date, $amount, $amount_allocated, $current_allocated, $ref);
			return true;
		} 
		else 
		{
			return false;
		}
	}
	
	function update_item($index, $type, $type_no, $date_, $due_date, 
		$amount, $amount_allocated, $current_allocated, $ref)
	{
		if ($amount > 0)
		{
			$this->allocs[$index] = new allocation_item($type, $type_no, 
				$date_, $due_date, $amount, $amount_allocated, $current_allocated, $ref);
			return true;
		} 
		else 
		{
			return false;
		}
	}	
	
	function add_or_update_item($type, $type_no, $date_, $due_date, 
		$amount, $amount_allocated, $current_allocated, $ref)
	{
		for ($i = 0; $i < count($this->allocs); $i++) 
		{
			$item = $this->allocs[$i];
			if (($item->type == $type) && ($item->type_no == $type_no)) 
			{
				return $this->update_item($i, $type, $type_no, $date_, $due_date, 
					$amount, $amount_allocated, $current_allocated, $ref);
			}  
		}
        return $this->add_item($type, $type_no, $date_, $due_date, 
        	$amount, $amount_allocated, $current_allocated, $ref);
	}

	//
	//	Read payment or credit current/available allocations to cart.
	//
	// FIXME - read all transactions below twice seems to be suboptimal
	//
	function read($type = null, $trans_no = 0) 
	{
		if ($type == null) {	// re-read
			$type = $this->type;
			$trans_no = $this->trans_no;
		}
		if ($type == ST_BANKPAYMENT || $type == ST_BANKDEPOSIT) {
			$bank_trans = db_fetch(get_bank_trans($type, $trans_no));
			$this->person_type = $bank_trans['person_type_id'] == PT_SUPPLIER;
		} else
			$this->person_type = $type == ST_SUPPCREDIT || $type == ST_SUPPAYMENT;
		$this->allocs = array();

		if ($trans_no) {
			$trans = $this->person_type ? get_supp_trans($trans_no, $type) 
				: get_customer_trans($trans_no, $type);

			$this->person_id = $trans[$this->person_type ? 'supplier_id':'debtor_no'];
			$this->person_name = $trans[$this->person_type ? "supplier_name":"DebtorName"];
			$this->amount = $trans["Total"];
			$this->date_ = sql2date($trans["tran_date"]);
		} 
		else {
			$this->person_id = get_post($this->person_type ? 'supplier_id':'customer_id');
			$this->date_ = get_post($this->person_type ? 'DatePaid':'DateBanked', Today());
		}

	/* Now populate the array of possible (and previous actual) allocations 
		for this customer/supplier. First get the transactions that have 
		outstanding balances ie Total-alloc >0 */

		if ($this->person_type)
			$trans_items = get_allocatable_to_supp_transactions($this->person_id);
		else
		 	$trans_items = get_allocatable_to_cust_transactions($this->person_id);

		while ($myrow = db_fetch($trans_items))
		{
			$this->add_item($myrow["type"], $myrow["trans_no"],
				sql2date($myrow["tran_date"]),
				sql2date($myrow["due_date"]),
				$myrow["Total"], // trans total
				$myrow["alloc"], // trans total allocated
				0,
				$myrow["reference"]); // this allocation
		}
		if ($trans_no == 0) return; // this is new payment

	/* Now get trans that might have previously been allocated to by this trans
	NB existing entries where still some of the trans outstanding entered from
	above logic will be overwritten with the prev alloc detail below */

		if ($this->person_type)
			$trans_items = get_allocatable_to_supp_transactions($this->person_id, 
				$trans_no, $type);
		else
		 	$trans_items = get_allocatable_to_cust_transactions($this->person_id, 
				$trans_no, $type);

		while ($myrow = db_fetch($trans_items))
		{
			$this->add_or_update_item ($myrow["type"], $myrow["trans_no"],
				sql2date($myrow["tran_date"]),
				sql2date($myrow["due_date"]),
				$myrow["Total"],
				$myrow["alloc"] - $myrow["amt"], $myrow["amt"], $myrow["reference"]);
		}
	}
	//
	//	Update allocations in database.
	//
	function write()
	{
		begin_transaction();

		if ($this->person_type)
			clear_supp_alloctions($this->type, $this->trans_no, $this->date_);
		else
		 	clear_cust_alloctions($this->type, $this->trans_no, $this->date_);

		// now add the new allocations
		$total_allocated = 0;
		foreach ($this->allocs as $alloc_item)
		{
			if ($alloc_item->current_allocated > 0)
			{
				if ($this->person_type) {
					add_supp_allocation($alloc_item->current_allocated,
						$this->type, $this->trans_no,
			    	 	$alloc_item->type, $alloc_item->type_no, $this->date_);

					update_supp_trans_allocation($alloc_item->type, 
						$alloc_item->type_no, $alloc_item->current_allocated);
				} else {
	 				add_cust_allocation($alloc_item->current_allocated,
						$this->type, $this->trans_no,
 			     		$alloc_item->type, $alloc_item->type_no, $this->date_);
			
	 				update_debtor_trans_allocation($alloc_item->type, 
						$alloc_item->type_no, $alloc_item->current_allocated);
				}
				// Exchange Variations Joe Hunt 2008-09-20 ////////////////////
				exchange_variation($this->type, $this->trans_no,
					$alloc_item->type, $alloc_item->type_no, $this->date_,
					$alloc_item->current_allocated,
					$this->person_type ? PT_SUPPLIER : PT_CUSTOMER);
				

				//////////////////////////////////////////////////////////////
				$total_allocated += $alloc_item->current_allocated;
			}

		}  /*end of the loop through the array of allocations made */
		if ($this->person_type)
			update_supp_trans_allocation($this->type, $this->trans_no, 
				$total_allocated);
		else
		 	update_debtor_trans_allocation($this->type,	$this->trans_no, 
				$total_allocated);

		commit_transaction();

	}

} 

//-----------------------------------------------------------------------------------

class allocation_item 
{

	var $type;
	var $type_no;
	
	var $date_;
	var $due_date;
	
	var $amount_allocated;
	var $amount;
	var $ref;
	
	var $current_allocated;
	
	function allocation_item ($type, $type_no, $date_, $due_date, $amount, 
		$amount_allocated, $current_allocated, $ref)
	{

		$this->type = $type;
		$this->type_no = $type_no;

		$this->ref = $ref;

		$this->date_ = $date_;
		$this->due_date = $due_date;
		
		$this->amount = $amount;
		$this->amount_allocated = $amount_allocated;
		$this->current_allocated = $current_allocated;
	}
}

//--------------------------------------------------------------------------------

function show_allocatable($show_totals) {

	global $systypes_array;
	
    $k = $counter = $total_allocated = 0;

	if (count($_SESSION['alloc']->allocs)) 
	{
		start_table(TABLESTYLE, "width=60%");
   		$th = array(_("Transaction Type"), _("#"), _("Ref"), _("Date"), _("Due Date"), _("Amount"),
   			_("Other Allocations"), _("This Allocation"), _("Left to Allocate"),'','');
	   	table_header($th);

		foreach ($_SESSION['alloc']->allocs as $alloc_item)
	    {
			alt_table_row_color($k);
    		label_cell($systypes_array[$alloc_item->type]);
	   		label_cell(get_trans_view_str($alloc_item->type, $alloc_item->type_no));
	   		label_cell($alloc_item->ref);
    		label_cell($alloc_item->date_, "align=right");
    		label_cell($alloc_item->due_date, "align=right");
	    	amount_cell($alloc_item->amount);
			amount_cell($alloc_item->amount_allocated);

	    	$_POST['amount' . $counter] = price_format($alloc_item->current_allocated);
    		amount_cells(null, "amount" . $counter, price_format('amount' . $counter));

	    	$un_allocated = round($alloc_item->amount - $alloc_item->amount_allocated, 6);
    		amount_cell($un_allocated, false,'', 'maxval'.$counter);
			label_cell("<a href='#' name=Alloc$counter onclick='allocate_all(this.name.substr(5));return true;'>"
				 . _("All") . "</a>");
			label_cell("<a href='#' name=DeAll$counter onclick='allocate_none(this.name.substr(5));return true;'>"
				 . _("None") . "</a>".hidden("un_allocated" . $counter, 
				 price_format($un_allocated), false));
			end_row();

   	    	$total_allocated += input_num('amount' . $counter);
	   	    $counter++;
	   	}
		if ($show_totals) {
    	   	label_row(_("Total Allocated"), price_format($total_allocated),
	    		"colspan=6 align=right", "align=right id='total_allocated'", 3);
			$amount = $_SESSION['alloc']->amount;

			if ($_SESSION['alloc']->type == ST_SUPPCREDIT
				|| $_SESSION['alloc']->type == ST_SUPPAYMENT
				||  $_SESSION['alloc']->type == ST_BANKPAYMENT)
				$amount = -$amount;
			
			if ($amount - $total_allocated < 0)
	        {
        		$font1 = "<font color=red>";
        		$font2 = "</font>";
    	    }
	        else
        		$font1 = $font2 = "";
			$left_to_allocate = price_format($amount - $total_allocated);
	        label_row(_("Left to Allocate"), $font1 . $left_to_allocate . $font2, 
				"colspan=6 align=right", "nowrap align=right id='left_to_allocate'",
				 3);
		}
		end_table(1);
	}
	hidden('TotalNumberOfAllocs', $counter);
}
//--------------------------------------------------------------------------------

function check_allocations()
{
	global $SysPrefs;

	$total_allocated = 0;

	for ($counter = 0; $counter < $_POST["TotalNumberOfAllocs"]; $counter++)
	{
		if (!check_num('amount' . $counter, 0))
		{
			display_error(_("The entry for one or more amounts is invalid or negative."));
			set_focus('amount'.$counter);
			return false;
		 }

		  /*Now check to see that the AllocAmt is no greater than the
		 amount left to be allocated against the transaction under review */
		 if (input_num('amount' . $counter) > input_num('un_allocated' . $counter))
		 {
			display_error(_("At least one transaction is overallocated."));
			set_focus('amount'.$counter);
			return false;
		 }

		 $_SESSION['alloc']->allocs[$counter]->current_allocated = input_num('amount' . $counter);

		 $total_allocated += input_num('amount' . $counter);
	}

	$amount = $_SESSION['alloc']->amount;
	

	if (in_array($_SESSION['alloc']->type, array(ST_BANKPAYMENT, ST_SUPPCREDIT, ST_SUPPAYMENT)))
		$amount = -$amount;

	if ($total_allocated - ($amount + input_num('discount'))  > $SysPrefs->allocation_settled_allowance())
	{
		display_error(_("These allocations cannot be processed because the amount allocated is more than the total amount left to allocate."));
		return false;
	}

	return true;
}

?>
