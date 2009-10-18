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
//-------------------------------------------------------------------------------------------------------------

function add_supp_invoice_item($supp_trans_type, $supp_trans_no, $stock_id, $description,
	$gl_code, $unit_price, $unit_tax, $quantity, $grn_item_id, $po_detail_item_id, $memo_,
	$err_msg="")
{
	$sql = "INSERT INTO ".TB_PREF."supp_invoice_items (supp_trans_type, supp_trans_no, stock_id, description, gl_code, unit_price, unit_tax, quantity,
	  	grn_item_id, po_detail_item_id, memo_) ";
	$sql .= "VALUES (".db_escape($supp_trans_type).", ".db_escape($supp_trans_no).", "
		.db_escape($stock_id).
	", ".db_escape($description).", ".db_escape($gl_code).", ".db_escape($unit_price)
	.", ".db_escape($unit_tax).", ".db_escape($quantity).",
		".db_escape($grn_item_id).", ".db_escape($po_detail_item_id).", ".db_escape($memo_).")";

	if ($err_msg == "")
		$err_msg = "Cannot insert a supplier transaction detail record";

	db_query($sql, $err_msg);

	return db_insert_id();
}

//-------------------------------------------------------------------------------------------------------------

function add_supp_invoice_gl_item($supp_trans_type, $supp_trans_no, $gl_code, $amount, $memo_, $err_msg="")
{
	return add_supp_invoice_item($supp_trans_type, $supp_trans_no,  "", "", $gl_code, $amount,
		0, 0, /*$grn_item_id*/0, /*$po_detail_item_id*/0, $memo_, $err_msg);
}


//----------------------------------------------------------------------------------------

function get_supp_invoice_items($supp_trans_type, $supp_trans_no)
{
	$sql = "SELECT *, unit_price AS FullUnitPrice FROM ".TB_PREF."supp_invoice_items
		WHERE supp_trans_type = ".db_escape($supp_trans_type)."
		AND supp_trans_no = ".db_escape($supp_trans_no)." ORDER BY id";
	return db_query($sql, "Cannot retreive supplier transaction detail records");
}

//----------------------------------------------------------------------------------------

function void_supp_invoice_items($type, $type_no)
{
	$sql = "UPDATE ".TB_PREF."supp_invoice_items SET quantity=0, unit_price=0
		WHERE supp_trans_type = ".db_escape($type)." AND supp_trans_no=".db_escape($type_no);
	db_query($sql, "could not void supptrans details");
}

?>