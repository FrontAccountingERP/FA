<?php

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
			if (!exists_customer_trans($type, $type_no))
				return false;
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

		case 20 : // it's a suppler invoice
		case 21 : // it's a supplier credit note
		case 22 : // it's a supplier payment
		case 25 : // it's a GRN
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
		VALUES ($type, $type_no, '$date', '$memo_')";

	db_query($sql, "could not add voided transaction entry");
}

//--------------------------------------------------------------------------------------------------

?>