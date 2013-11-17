ALTER TABLE `0_suppliers` ADD COLUMN  `tax_algorithm` tinyint(1) NOT NULL default '1' AFTER `tax_included`;
ALTER TABLE `0_supp_trans` ADD COLUMN `tax_algorithm` tinyint(1) NULL default '1' AFTER `tax_included`;
INSERT INTO `0_sys_prefs` VALUES('tax_algorithm','glsetup.customer', 'tinyint', 1, '1');
INSERT INTO `0_sys_prefs` VALUES('gl_closing_date','setup.closing_date', 'date', 8, '');
# Fix eventual invalid date/year in audit records
UPDATE `0_audit_trail` audit 
		LEFT JOIN `0_gl_trans` gl ON  gl.`type`=audit.`type` AND gl.type_no=audit.trans_no
		LEFT JOIN `0_fiscal_year` year ON year.begin<=gl.tran_date AND year.end>=gl.tran_date
		SET audit.gl_date=gl.tran_date, audit.fiscal_year=year.id
		WHERE NOT ISNULL(gl.`type`);

DROP TABLE IF EXISTS `0_wo_costing`;

CREATE TABLE `0_wo_costing` (
  `id` int(11) NOT NULL auto_increment,
  `workorder_id` int(11) NOT NULL default '0',
  `cost_type` 	tinyint(1) NOT NULL default '0',
  `trans_type` int(11) NOT NULL default '0',
  `trans_no` int(11) NOT NULL default '0',
  `factor` double NOT NULL default '1',
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB;

UPDATE `0_gl_trans` gl
		LEFT JOIN `0_cust_branch` br ON br.receivables_account=gl.account AND br.debtor_no=gl.person_id AND gl.person_type_id=2
		LEFT JOIN `0_suppliers` sup ON sup.payable_account=gl.account AND sup.supplier_id=gl.person_id AND gl.person_type_id=3
 SET `person_id` = IF(br.receivables_account, br.debtor_no, IF(sup.payable_account, sup.supplier_id, NULL)), 
 	`person_type_id` = IF(br.receivables_account, 2, IF(sup.payable_account, 3, NULL));

ALTER TABLE `0_tax_group_items` ADD COLUMN `tax_shipping` tinyint(1) NOT NULL default '0' AFTER `rate`;
UPDATE `0_tax_group_items` tgi
	SET tgi.tax_shipping=1
	WHERE tgi.rate=(SELECT 0_tax_types.rate FROM 0_tax_types, 0_tax_groups 
		WHERE tax_shipping=1 AND tgi.tax_group_id=0_tax_groups.id AND tgi.tax_type_id=0_tax_types.id);
ALTER TABLE `0_tax_groups` DROP COLUMN `tax_shipping`;

ALTER TABLE `0_sales_order_details` ADD KEY `stkcode` (`stk_code`);
ALTER TABLE `0_purch_order_details` ADD KEY `itemcode` (`item_code`);
ALTER TABLE `0_sys_prefs` CHANGE `value` `value` TEXT NOT NULL;
ALTER TABLE `0_cust_branch` ADD COLUMN `bank_account` varchar(60) DEFAULT NULL AFTER `notes`;

ALTER TABLE `0_debtor_trans` ADD COLUMN `tax_included` tinyint(1) unsigned NOT NULL default '0' AFTER `payment_terms`;
UPDATE `0_debtor_trans` tr, `0_trans_tax_details` td SET tr.tax_included=td.included_in_price
	WHERE tr.`type`=td.trans_type AND tr.trans_no=td.trans_no AND td.included_in_price;

ALTER TABLE `0_bank_accounts` ADD COLUMN `bank_charge_act` varchar(15) NOT NULL DEFAULT '' AFTER `id`;
UPDATE `0_bank_accounts` SET `bank_charge_act`=(SELECT `value` FROM 0_sys_prefs WHERE name='bank_charge_act');

ALTER TABLE `0_users` ADD `transaction_days` INT( 6 ) NOT NULL default '30' COMMENT 'Transaction days' AFTER `startup_tab`;

ALTER TABLE `0_purch_orders` ADD COLUMN `prep_amount` double NOT NULL DEFAULT 0 AFTER `total`;
ALTER TABLE `0_purch_orders` ADD COLUMN `alloc` double NOT NULL DEFAULT 0 AFTER `prep_amount`;

ALTER TABLE `0_sales_orders` ADD COLUMN `prep_amount` double NOT NULL DEFAULT 0 AFTER `total`;
ALTER TABLE `0_sales_orders` ADD COLUMN `alloc` double NOT NULL DEFAULT 0 AFTER `prep_amount`;

ALTER TABLE `0_cust_allocations` ADD  UNIQUE KEY(`trans_type_from`,`trans_no_from`,`trans_type_to`,`trans_no_to`);
ALTER TABLE `0_supp_allocations` ADD  UNIQUE KEY(`trans_type_from`,`trans_no_from`,`trans_type_to`,`trans_no_to`);

ALTER TABLE `0_sales_order_details` ADD COLUMN `invoiced` double NOT NULL DEFAULT 0 AFTER `quantity`;

# update sales_order_details.invoiced with sum of invoiced quantities on all related SI
UPDATE `0_sales_order_details` so
	LEFT JOIN `0_debtor_trans_details` delivery ON delivery.`debtor_trans_type`=13 AND src_id=so.id
	LEFT JOIN (SELECT src_id, sum(quantity) as qty FROM `0_debtor_trans_details` WHERE `debtor_trans_type`=10 GROUP BY src_id) inv
		ON inv.src_id=delivery.id
	SET `invoiced` = `invoiced`+inv.qty;

ALTER TABLE `0_debtor_trans` ADD COLUMN `prep_amount` double NOT NULL DEFAULT 0 AFTER `alloc`;

INSERT INTO `0_sys_prefs` VALUES ('deferred_income_act', 'glsetup.sales', 'varchar', '15', '');

# set others transactions edition for all roles for backward  compatibility
UPDATE `0_security_roles` SET `sections`=CONCAT_WS(';', `sections`, '768'), `areas`='775'
	WHERE NOT `sections` REGEXP '[^0-9]?768[^0-9]?';

UPDATE `0_security_roles` SET `areas`=CONCAT_WS(';', `areas`, '775')
	WHERE NOT `areas` REGEXP '[^0-9]?775[^0-9]?';

ALTER TABLE `0_stock_master` ADD COLUMN `no_purchase` tinyint(1) NOT NULL default '0' AFTER `no_sale`;
ALTER TABLE `0_stock_category` ADD COLUMN `dflt_no_purchase` tinyint(1) NOT NULL default '0' AFTER `dflt_no_sale`;

# added exchange rate field in grn_batch
ALTER TABLE `0_grn_batch` ADD COLUMN `rate` double NULL default '1' AFTER `loc_code`;
ALTER TABLE `0_users` CHANGE `query_size` `query_size` TINYINT(1) UNSIGNED NOT NULL DEFAULT 10; 
