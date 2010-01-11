ALTER TABLE 0_comments ADD KEY `type_and_id` (`type`, `id`);
ALTER TABLE 0_quick_entries ADD COLUMN `bal_type` TINYINT(1) NOT NULL default '0'; 

# Key optimizations
ALTER TABLE 0_fiscal_year ADD UNIQUE KEY(`begin`), ADD UNIQUE KEY(`end`);
ALTER TABLE 0_useronline ADD KEY(`ip`);
ALTER TABLE 0_dimensions ADD KEY(`date_`), ADD KEY(`due_date`), ADD KEY(`type_`);
ALTER TABLE 0_gl_trans ADD KEY (`dimension_id`), ADD KEY (`dimension2_id`), ADD KEY (`tran_date`), ADD KEY `account_and_tran_date` (`account`, `tran_date`);
ALTER TABLE 0_chart_master DROP KEY `account_code`;
ALTER TABLE 0_chart_types ADD KEY(`class_id`);
ALTER TABLE 0_bank_accounts ADD KEY (`account_code`);
ALTER TABLE 0_bank_trans ADD KEY (`bank_act`,`reconciled`), ADD KEY (`bank_act`,`trans_date`);
ALTER TABLE 0_budget_trans ADD KEY `Account` (`account`, `tran_date`, `dimension_id`, `dimension2_id`);
ALTER TABLE 0_trans_tax_details ADD KEY `Type_and_Number` (`trans_type`,`trans_no`), ADD KEY (`tran_date`);
ALTER TABLE 0_audit_trail DROP KEY `fiscal_year`, ADD KEY `Seq` (`fiscal_year`, `gl_date`, `gl_seq`), ADD KEY `Type_and_Number` (`type`,`trans_no`);
ALTER TABLE 0_item_codes ADD KEY (`item_code`);
ALTER TABLE 0_stock_moves ADD KEY `Move` (`stock_id`,`loc_code`, `tran_date`);
ALTER TABLE 0_wo_issues ADD KEY (`workorder_id`);
ALTER TABLE 0_wo_manufacture ADD KEY (`workorder_id`);
ALTER TABLE 0_wo_requirements ADD KEY (`workorder_id`);
ALTER TABLE 0_bom DROP KEY `Parent_2`;
ALTER TABLE 0_refs ADD KEY `Type_and_Reference` (`type`,`reference`);
ALTER TABLE 0_grn_items ADD KEY (`grn_batch_id`);
ALTER TABLE 0_grn_batch ADD KEY (`delivery_date`), ADD KEY (`purch_order_no`);
ALTER TABLE 0_supp_invoice_items ADD KEY `Transaction` (`supp_trans_type`, `supp_trans_no`, `stock_id`);
ALTER TABLE 0_purch_order_details ADD KEY `order` (`order_no`, `po_detail_item`);
ALTER TABLE 0_purch_orders ADD KEY (`ord_date`);
ALTER TABLE 0_supp_trans ADD KEY (`tran_date`), DROP PRIMARY KEY, ADD PRIMARY KEY (`type`, `trans_no`);
ALTER TABLE 0_suppliers ADD KEY (`supp_ref`);
ALTER TABLE 0_supp_allocations ADD KEY `From` (`trans_type_from`, `trans_no_from`), ADD KEY `To` (`trans_type_to`, `trans_no_to`);
ALTER TABLE 0_cust_branch DROP KEY `br_name`, ADD KEY (`branch_ref`), ADD KEY (`group_no`);
ALTER TABLE 0_debtors_master ADD KEY (`debtor_ref`);
ALTER TABLE 0_debtor_trans DROP PRIMARY KEY, ADD PRIMARY KEY (`type`, `trans_no`), ADD KEY (`tran_date`);
ALTER TABLE 0_debtor_trans_details ADD KEY `Transaction` (`debtor_trans_type`, `debtor_trans_no`);
ALTER TABLE 0_cust_allocations ADD KEY `From` (`trans_type_from`, `trans_no_from`), ADD KEY `To` (`trans_type_to`, `trans_no_to`);
ALTER TABLE 0_sales_order_details ADD KEY `sorder` (`trans_type`, `order_no`);
ALTER TABLE 0_chart_master ADD KEY `accounts_by_type` (`account_type`, `account_code`);
# fix invalid constraint on databases generated from 2.2 version on en_US-new.sql
#ALTER TABLE `0_tax_types` DROP KEY `name`;
