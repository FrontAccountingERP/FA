# reliable cost change log in stock_moves
ALTER TABLE `0_stock_moves` CHANGE COLUMN `standard_cost` `unit_cost` double NOT NULL DEFAULT '0';
ALTER TABLE `0_debtor_trans_details` CHANGE COLUMN `standard_cost` `unit_cost` double NOT NULL DEFAULT '0';

# naming cleanups
ALTER TABLE `0_purch_orders` CHANGE COLUMN `requisition_no` `supp_reference` tinytext;

# cleanups in work orders
ALTER TABLE  `0_workorders` DROP INDEX `wo_ref`;
ALTER TABLE  `0_workorders` ADD KEY `wo_ref` (`wo_ref`);
ALTER TABLE  `0_workorders` DROP COLUMN `additional_costs`;

# improvements in tax systems support
ALTER TABLE `0_stock_category` ADD COLUMN  `vat_category` tinyint(1) NOT NULL DEFAULT '0' AFTER `dflt_no_purchase`;
ALTER TABLE `0_stock_master` ADD COLUMN  `vat_category` tinyint(1) NOT NULL DEFAULT '0' AFTER `fa_class_id`;
ALTER TABLE `0_trans_tax_details` ADD COLUMN  `vat_category` tinyint(1) NOT NULL DEFAULT '0' AFTER `reg_type`;
ALTER TABLE `0_trans_tax_details` ADD COLUMN `tax_group_id` int(11) DEFAULT NULL AFTER `vat_category`;

UPDATE `0_trans_tax_details` tax
	LEFT JOIN `0_supp_trans` purch ON tax.trans_no=purch.trans_no AND tax.trans_type=purch.type
	LEFT JOIN `0_suppliers` supp ON purch.supplier_id=supp.supplier_id
	LEFT JOIN `0_debtor_trans` sales ON tax.trans_no=sales.trans_no AND tax.trans_type=sales.type
	LEFT JOIN `0_cust_branch` cust ON sales.branch_code=cust.branch_code
 SET tax.tax_group_id = IFNULL(supp.tax_group_id, cust.tax_group_id);

ALTER TABLE `0_tax_groups` ADD COLUMN `tax_area` tinyint(1) NOT NULL DEFAULT '0' AFTER `name`;
