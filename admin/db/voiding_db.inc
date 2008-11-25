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
include_once($path_to_root . "/sales/includes/sales_db.inc");
include_once($path_to_root . "/purchasing/includes/purchasing_db.inc");
include_once($path_to_root . "/inventory/includes/inventory_db.inc");
include_once($path_to_root . "/manufacturing/includes/manufacturing_db.inc");

function void_transaction($type, $type_no, $date_, $memo_)
{
	$void_entry = get_voided_entry($type, $type_no);

	if ($void_entry != null)
		return false;

	switch ($type) {
		case 0 : // it's a journal entry
			if (!exists_gl_trans($type, $type_no))
				return false;
			void_gl_trans($type, $type_no);
			if (exists_bank_trans($type, $type_no))
				void_bank_trans($type, $type_no);
			break;

		case 1 : // it's a payment
		case 2 : // it's a deposit
		case 4 : // it's a transfer
			if (!exists_bank_trans($type, $type_no))
				return false;
			void_bank_trans($type, $type_no);
			break;

		case 10 : // it's a customer invoice
		case 11 : // it's a customer credit note
		case 12 : // it's a customer payment
		case 13 : // it's a customer dispatch
			if (!exists_customer_trans($type, $type_no))
				return false;
			if ($type == 13)	// added 04 Oct 2008 by Joe Hunt. If delivery note has a not voided invoice, then NO.
			{
				$delivery = get_customer_trans($type_no, $type);
				if ($delivery['trans_link'] != 0)
				{
					if (get_voided_entry(10, $delivery['trans_link']) === false)
						return false;
				}
			}	
			post_void_customer_trans($type, $type_no);
			break;

		case systypes::location_transfer() : // it's a stock transfer
			if (get_stock_transfer_items($type_no) == null)
				return false;
			void_stock_transfer($type_no);
			break;

		case systypes::inventory_adjustment() : // it's a stock adjustment
			if (get_stock_adjustment_items($type_no) == null)
				return false;
			void_stock_adjustment($type_no);
			break;

		case 25 : // it's a GRN
			return false;
		case 20 : // it's a suppler invoice
		case 21 : // it's a supplier credit note
		case 22 : // it's a supplier payment
			if (!exists_supp_trans($type, $type_no))
				return false;
			if (!post_void_supp_trans($type, $type_no))
				return false;
			break;

		case systypes::work_order() : // it's a work order
			if (!get_work_order($type_no, true))
				return false;
			void_work_order($type_no);
			break;

		case 28 : // it's a work order issue
			if (!exists_work_order_issue($type_no))
				return false;
			void_work_order_issue($type_no);
			break;

		case 29 : // it's a work order production
			if (!exists_work_order_produce($type_no))
				return false;
			void_work_order_produce($type_no);
			break;

		case systypes::cost_update() : // it's a stock cost update
			return false;
			break;
	}

	// only add an entry if it's actually been voided
	add_voided_entry($type, $type_no, $date_, $memo_);

	return true;
}

//--------------------------------------------------------------------------------------------------

function get_voided_entry($type, $type_no)
{
	$sql = "SELECT * FROM ".TB_PREF."voided WHERE type=$type AND id=$type_no";

	$result = db_query($sql, "could not query voided transaction table");

	return db_fetch($result);
}

//--------------------------------------------------------------------------------------------------

function add_voided_entry($type, $type_no, $date_, $memo_)
{
	$date = date2sql($date_);
	$sql = "INSERT INTO ".TB_PREF."voided (type, id, date_, memo_)
		VALUES ($type, $type_no, ".db_escape($date).", ".db_escape($memo_).")";

	db_query($sql, "could not add voided transaction entry");
}

//--------------------------------------------------------------------------------------------------

?>