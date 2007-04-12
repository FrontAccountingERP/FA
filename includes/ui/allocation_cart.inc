<?php

/* definition of the Debtor Receipt/Credit note allocation class */

//-----------------------------------------------------------------------------------

class allocation 
{

	var $trans_no; 
	var $type;
	var $person_id;
	var $person_name;
	var $date_;
	var $amount; /*Total amount of the transaction in FX */
	
	var $allocs; /*array of transactions allocated to */	

	function allocation($trans_no, $type, $person_id, $person_name,	$amount, $date_)
	{
		$this->allocs = array();
		
		$this->trans_no = $trans_no;
		$this->type = $type;
		$this->person_id = $person_id;
		$this->person_name = $person_name;
		$this->amount = $amount;
		$this->date_ = $date_;
	}

	function add_item($type, $type_no, $date_, $due_date, $amount, $amount_allocated, 
		$current_allocated)
	{
		if ($amount > 0)
		{
			$this->allocs[count($this->allocs)] = new allocation_item($type, $type_no, 
				$date_, $due_date, $amount, $amount_allocated, $current_allocated);
			return true;
		} 
		else 
		{
			return false;
		}
	}
	
	function update_item($index, $type, $type_no, $date_, $due_date, 
		$amount, $amount_allocated, $current_allocated)
	{
		if ($amount > 0)
		{
			$this->allocs[$index] = new allocation_item($type, $type_no, 
				$date_, $due_date, $amount, $amount_allocated, $current_allocated);
			return true;
		} 
		else 
		{
			return false;
		}
	}	
	
	function add_or_update_item($type, $type_no, $date_, $due_date, 
		$amount, $amount_allocated, $current_allocated)
	{
		for ($i = 0; $i < count($this->allocs); $i++) 
		{
			$item = $this->allocs[$i];
			if (($item->type == $type) && ($item->type_no == $type_no)) 
			{
				return $this->update_item($i, $type, $type_no, $date_, $due_date, 
					$amount, $amount_allocated, $current_allocated);
			}  
		}
        return $this->add_item($type, $type_no, $date_, $due_date, 
        	$amount, $amount_allocated, $current_allocated);				
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
	
	var $current_allocated;
	
	function allocation_item ($type, $type_no, $date_, $due_date, $amount, 
		$amount_allocated, $current_allocated)
	{

		$this->type = $type;
		$this->type_no = $type_no;
		
		$this->date_ = $date_;
		$this->due_date = $due_date;
		
		$this->amount = $amount;
		$this->amount_allocated = $amount_allocated;
		$this->current_allocated = $current_allocated;
	}
}

//-----------------------------------------------------------------------------------

?>
