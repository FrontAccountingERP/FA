<?php

$form_types = array (
				
				10=>  array('code' => 'si', 'name' => _("Sales Invoice")),
				11=>  array('code' => 'si', 'name' => _("Sales Credit Note")),
				30=>  array('code' => 'so', 'name' => _("Sales Order")),
				
				18=>  array('code' => 'po', 'name' => _("Purchase Order")),
				
				26=>  array('code' => 'wo', 'name' => _("Work Order")),
				28=>  array('code' => 'wo', 'name' => _("Work Order Issue")),
				29=>  array('code' => 'wo', 'name' => _("Work Order Production")),
				
				40=>  array('code' => 'qw', 'name' => _("Cheque Withdrawal")),
				41=>  array('code' => 'qd', 'name' => _("Cheque Deposit")),
				
				50=>  array('code' => 'cw', 'name' => _("Cash Withdrawal")),
				51=>  array('code' => 'cd', 'name' => _("Cash Deposit")),
				
				60=>  array('code' => 'ir', 'name' => _("Inventory Release")),
				61=>  array('code' => 'ia', 'name' => _("Inventory Add")),
				62=>  array('code' => 'it', 'name' => _("Inventory Return"))								
					);
					
function getFormTypeName($type)
{
	global $form_types;
	return $form_types[$type]['name'];
}

function add_forms_for_sys_type($trans_type, $trans_id, $ParamFrom=null, $ParamTo=null)
{
	switch ($trans_type) {
        case 1 : // bank payment
		case 22 : // payment to supplier 
			add_form_entry(50, $trans_type, $trans_id, $ParamFrom, $ParamTo); 
			break;
		
		case 12: // payment from customer 			
        case 2 : // bank deposit
			add_form_entry(51, $trans_type, $trans_id, $ParamFrom, $ParamTo);
			break;
        
        case 4 : // bank transfer - withdraw and deposit
			add_form_entry(50, $trans_type, $trans_id, $ParamFrom, $ParamTo);
			add_form_entry(51, $trans_type, $trans_id, $ParamTo, $ParamFrom);
			break;		
		
		case 10 : // sales invoice - inventory release and invoice forms
			add_form_entry(60, $trans_type, $trans_id, $ParamFrom); 
			add_form_entry(10, $trans_type, $trans_id);
			break;
		
		case 11 : // sales credit - inventory return and credit note forms
			add_form_entry(62, $trans_type, $trans_id, $ParamFrom); 
			add_form_entry(11, $trans_type, $trans_id);
			break;
		
		case 16 : // location transfer
			add_form_entry(60, $trans_type, $trans_id, $ParamFrom);
			add_form_entry(61, $trans_type, $trans_id, $ParamTo);
			break;
		
		case systypes::inventory_adjustment() : // inventory adjustment

			// $ParamFrom : if 1 inventory add, 0 inventory release
			if ($ParamFrom)
				add_form_entry(61, $trans_type, $trans_id, $ParamTo);
			else
				add_form_entry(60, $trans_type, $trans_id, $ParamTo);
			break;	
			
		case systypes::po() : // purchase order
			add_form_entry(18, $trans_type, $trans_id);
			break; 
			
		case 25 : // purchase order delivery
			add_form_entry(61, $trans_type, $trans_id, $ParamFrom);
			break;	

		case systypes::work_order() : // work order
			add_form_entry(26, $trans_type, $trans_id);
			break;	
			
		case 28 : // work order issue
			// $ParamFrom : if 1 inventory add, 0 inventory release
			if ($ParamFrom)
				add_form_entry(61, $trans_type, $trans_id, $ParamTo);
			else
				add_form_entry(60, $trans_type, $trans_id, $ParamTo);
			break;	
			
		case 29 : // work order production
			// $ParamFrom is the quantity - if +ve inventory add, -ve inventory release
			if ($ParamFrom >= 0)
				add_form_entry(61, $trans_type, $trans_id, $ParamTo);
			else
				add_form_entry(60, $trans_type, $trans_id, $ParamTo);
			break;							
		
		case 30 : // sales order - sales order form
			add_form_entry(30, $trans_type, $trans_id);
			break;
	}
}	

function add_form_entry($form_type, $trans_type, $trans_id, $param1=null, $param2=null)	
{
	$sql = "SELECT max(form_id) FROM ".TB_PREF."form_items WHERE form_type=$form_type";
	$result = db_query($sql, "could not get next form_id");
	$row = db_fetch_row($result);
	$form_id = $row[0] + 1;
	
	$sql = "INSERT INTO ".TB_PREF."form_items (form_id, form_type, trans_type, trans_id, param1, param2)
		VALUES ($form_id, $form_type, $trans_type, $trans_id, '$param1', '$param2')";
	$result = db_query($sql, "could not add form entry for $trans_type, $trans_id");
}			

function delete_forms_for_systype($trans_type, $trans_id)
{
	$sql = "DELETE FROM ".TB_PREF."form_items WHERE trans_type=$trans_type AND trans_id=$trans_id";
	$result = db_query($sql, "could not delete form entry for $trans_type, $trans_id");
}

function get_form_entries($trans_type, $trans_id)
{
	$sql = "SELECT * FROM ".TB_PREF."form_items WHERE trans_type=$trans_type AND trans_id=$trans_id";
	$result = db_query($sql, "could not query form entry for $trans_type, $trans_id");
	
	return $result;
}

?>