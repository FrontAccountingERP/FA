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
//---------------------------------------------------------------------------------------------------

function display_grn_summary(&$po, $editable=false)
{
	global $Refs;
    start_table(TABLESTYLE2, "width=90%");
    
    start_row();
	label_cells(_("Supplier"), $po->supplier_name, "class='tableheader2'");
    
    if (!is_company_currency($po->curr_code))
    	label_cells(_("Order Currency"), $po->curr_code, "class='tableheader2'");
    
    label_cells(_("For Purchase Order"), get_trans_view_str(ST_PURCHORDER, $po->order_no),
    	"class='tableheader2'");
    
	label_cells(_("Ordered On"), $po->orig_order_date, "class='tableheader2'");
	end_row();
		
   	start_row();
    if ($editable) 
    {
    	if (!isset($_POST['ref']))
    	    $_POST['ref'] = $Refs->get_next(ST_SUPPRECEIVE);
    	ref_cells(_("Reference"), 'ref', '', null, "class='tableheader2'");
    	    	
        if (!isset($_POST['Location']))
        	$_POST['Location'] = $po->Location;
        label_cell(_("Deliver Into Location"), "class='tableheader2'");
        locations_list_cells(null, "Location", $_POST['Location']);
        
    	if (!isset($_POST['DefaultReceivedDate']))
     	  	$_POST['DefaultReceivedDate'] = new_doc_date();
    	
    	date_cells(_("Date Items Received"), 'DefaultReceivedDate', '', true, 0, 0, 0, "class='tableheader2'");    
		// currently this is related order supp reference
//		ref_cells(_("Supplier's Reference"), 'supp_ref', _("Supplier's Reference"), null, "class='tableheader2'");
		label_cells(_("Supplier's Reference"), $po->supp_ref, "class='tableheader2'");
    } 
    else 
    {
    	label_cells(_("Reference"), $po->reference, "class='tableheader2'");
    	label_cells(_("Deliver Into Location"), get_location_name($po->Location), "class='tableheader2'");
		label_cells(_("Supplier's Reference"), $po->supp_ref, "class='tableheader2'");
    }
    
    end_row();
    
    if (!$editable)
    
    	label_row(_("Delivery Address"), $po->delivery_address, "class='tableheader2'", "colspan=9");
    
    if ($po->Comments != "")
    	label_row(_("Order Comments"), $po->Comments, "class='tableheader2'", "colspan=9");
    end_table(1);	
}

?>