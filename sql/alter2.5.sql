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

# shipment options
ALTER TABLE `0_stock_master` ADD COLUMN `shipper_id` INT(11) NOT NULL DEFAULT '0' AFTER `vat_category`;

INSERT INTO `0_stock_category` (`description`, `dflt_tax_type`, `dflt_units`, `dflt_mb_flag`, `dflt_sales_act`, `dflt_cogs_act`, `dflt_no_sale`)
	VALUES (@shipping_cat_description, @shipping_tax_type, @shipping_units, 'T', @shipping_sales_act, @shipping_cogs_act, '1');

SET @shipment_cat=LAST_INSERT_ID();

INSERT INTO `0_stock_master` (`stock_id`, `tax_type_id`, `description`, `units`, `mb_flag`, `sales_account`, `no_sale`, `no_purchase`, `vat_category`, `category_id`, `shipper_id`, `inactive`)
	SELECT shipper.shipper_name, @shipping_tax_type, shipper.shipper_name, @shipping_units, 'T', @shipping_sales_act, 1, 1, 0, @shipment_cat, shipper.shipper_id, shipper.inactive
		FROM `0_shippers` shipper;

ALTER TABLE `0_sales_orders` CHANGE COLUMN `ship_via` `ship_via` varchar(20) NOT NULL DEFAULT '';

UPDATE `0_sales_orders` ord
	LEFT JOIN `0_shippers` ship ON  ord.ship_via=ship.shipper_id
	LEFT JOIN `0_stock_master` stock ON stock.shipper_id=ship.shipper_id
	SET ord.ship_via=stock.stock_id;

ALTER TABLE `0_debtor_trans` CHANGE COLUMN `ship_via` `ship_via` varchar(20) NOT NULL DEFAULT '';

UPDATE `0_debtor_trans` trans
	LEFT JOIN `0_shippers` ship ON  trans.ship_via=ship.shipper_id
	LEFT JOIN `0_stock_master` stock ON stock.shipper_id=ship.shipper_id
	SET trans.ship_via=stock.stock_id;

ALTER TABLE `0_cust_branch` CHANGE COLUMN `default_ship_via` `default_ship_via` varchar(20) NOT NULL DEFAULT '';

UPDATE `0_cust_branch` branch
	LEFT JOIN `0_shippers` ship ON  branch.default_ship_via=ship.shipper_id
	LEFT JOIN `0_stock_master` stock ON stock.shipper_id=ship.shipper_id
	SET branch.default_ship_via=stock.stock_id;

ALTER TABLE `0_tax_group_items` DROP COLUMN `tax_shipping`;

# new debug trail
DROP TABLE `1_sql_trail`;
CREATE TABLE `1_db_trail` (
		`id` int(11) NOT NULL AUTO_INCREMENT,
		`stamp` timestamp DEFAULT CURRENT_TIMESTAMP,
		`user` tinyint(3) unsigned NOT NULL DEFAULT '0',
		`msg`  varchar(255) DEFAULT '',
		`entry`  varchar(255) DEFAULT '',
		`data` text DEFAULT NULL,
	PRIMARY KEY (`id`)
	) ENGINE=MyISAM;

# payment terms normalization, early payment support
ALTER TABLE `0_payment_terms` CHANGE COLUMN `terms_indicator` `id` tinyint(1) NOT NULL DEFAULT '0';
ALTER TABLE `0_payment_terms` ADD COLUMN `type` tinyint(1) NOT NULL DEFAULT '0' AFTER `terms`;
UPDATE `0_payment_terms` SET `type`=IF(days_before_due < 0, 1, IF(day_in_following_month>0, 4, IF(days_before_due=0, 2, 3)));
UPDATE `0_payment_terms` SET days_before_due=day_in_following_month WHERE days_before_due<=0;
ALTER TABLE `0_payment_terms` CHANGE COLUMN `days_before_due` `days` int(11) NOT NULL DEFAULT '0';
ALTER TABLE `0_payment_terms` DROP COLUMN `day_in_following_month`;
ALTER TABLE `0_payment_terms` ADD COLUMN `early_discount` double NOT NULL DEFAULT '0' AFTER `days`;
ALTER TABLE `0_payment_terms` ADD COLUMN `early_days` int(11) NOT NULL DEFAULT '0' AFTER `early_discount`;

ALTER TABLE `0_cust_allocations` ADD COLUMN `discount` double unsigned DEFAULT '0' AFTER `amt`;
ALTER TABLE `0_supp_allocations` ADD COLUMN `discount` double unsigned DEFAULT '0' AFTER `amt`;

# please define and set payment terms with discount manually per customer when needed
ALTER TABLE `0_debtors_master` DROP COLUMN `pymt_discount`;

# this update works only for single pay for invoice discounts, so may need additional manual fixes in more complex cases 
UPDATE `0_cust_allocations` ca
	LEFT JOIN `0_debtor_trans` pay ON pay.`type`=ca.`trans_type_from` AND pay.`trans_no`=ca.`trans_no_from`
	LEFT JOIN `0_debtor_trans` trans ON trans.`type`=ca.`trans_type_to` AND trans.`trans_no`=ca.`trans_no_to`
SET ca.discount=pay.ov_discount 
	WHERE pay.ov_discount != 0 AND pay.ov_amount+pay.ov_discount = trans.ov_amount+trans.ov_gst+trans.ov_freight+trans.ov_freight_tax;

UPDATE `0_supp_allocations` sa
	LEFT JOIN `0_supp_trans` pay ON pay.`type`=sa.`trans_type_from` AND pay.`trans_no`=sa.`trans_no_from`
	LEFT JOIN `0_supp_trans` trans ON trans.`type`=sa.`trans_type_to` AND trans.`trans_no`=sa.`trans_no_to`
SET sa.discount=pay.ov_discount 
	WHERE pay.ov_discount != 0 AND pay.ov_amount+pay.ov_discount = trans.ov_amount+trans.ov_gst;

# bank charge stored in bank_trans
ALTER TABLE `0_bank_trans` ADD COLUMN `charge` double DEFAULT 0 AFTER `amount`;

UPDATE `0_bank_trans` bt
	LEFT JOIN (SELECT trans.type, trans.trans_no, IF(act.bank_curr_code=home_curr.value, charge.amount,
		IF(act.bank_curr_code=debtor.curr_code, -(trans.amount-ar.ov_amount+ar.ov_discount),
		IFNULL(charge.amount*trans.amount/pmt.amount, 0))) amount
		FROM 0_bank_trans trans
			LEFT JOIN `0_bank_accounts` act ON trans.bank_act=act.id
			LEFT JOIN `0_sys_prefs` charge_act ON charge_act.name='bank_charge_act'
			LEFT JOIN `0_sys_prefs` home_curr ON home_curr.name='curr_default'
			LEFT JOIN `0_gl_trans` charge ON charge.type=trans.`type` AND charge.type_no=trans.trans_no AND charge.account=charge_act.`value`
			LEFT JOIN `0_gl_trans` pmt ON pmt.type=trans.type AND pmt.type_no=trans.trans_no AND pmt.account=act.account_code
			LEFT JOIN `0_debtors_master` debtor ON trans.person_id=debtor.debtor_no AND trans.person_type_id=2
			LEFT JOIN `0_debtor_trans` ar ON trans.type=ar.`type` AND trans.trans_no=ar.trans_no
		WHERE pmt.amount!=0 AND charge.amount!=0 AND trans.amount!=0) charges ON bt.`type`=charges.`type` AND bt.`trans_no`=charges.`trans_no`
	SET bt.charge=IFNULL(-charges.amount,0),
		bt.amount=bt.amount+IFNULL(charges.amount,0);
