ALTER TABLE `0_company` DROP COLUMN `custom1_name`;
ALTER TABLE `0_company` DROP COLUMN `custom2_name`;
ALTER TABLE `0_company` DROP COLUMN `custom3_name`;
ALTER TABLE `0_company` DROP COLUMN `custom1_value`;
ALTER TABLE `0_company` DROP COLUMN `custom2_value`;
ALTER TABLE `0_company` DROP COLUMN `custom3_value`;

ALTER TABLE `0_company` ADD COLUMN `default_delivery_required` SMALLINT(6) NULL DEFAULT '1';

ALTER TABLE `0_stock_category` DROP COLUMN `stock_act`;
ALTER TABLE `0_stock_category` DROP COLUMN `cogs_act`;
ALTER TABLE `0_stock_category` DROP COLUMN `adj_gl_act`;
ALTER TABLE `0_stock_category` DROP COLUMN `purch_price_var_act`;

ALTER TABLE `0_stock_category` ADD COLUMN `dflt_tax_type` int(11) NOT NULL default '1';
ALTER TABLE `0_stock_category` ADD COLUMN `dflt_units` varchar(20) NOT NULL default 'each';
ALTER TABLE `0_stock_category` ADD COLUMN `dflt_mb_flag` char(1) NOT NULL default 'B';
ALTER TABLE `0_stock_category` ADD COLUMN `dflt_sales_act` varchar(11) NOT NULL default '';
ALTER TABLE `0_stock_category` ADD COLUMN `dflt_cogs_act` varchar(11) NOT NULL default '';
ALTER TABLE `0_stock_category` ADD COLUMN `dflt_inventory_act` varchar(11) NOT NULL default '';
ALTER TABLE `0_stock_category` ADD COLUMN `dflt_adjustment_act` varchar(11) NOT NULL default '';
ALTER TABLE `0_stock_category` ADD COLUMN `dflt_assembly_act` varchar(11) NOT NULL default '';
ALTER TABLE `0_stock_category` ADD COLUMN `dflt_dim1` int(11) default NULL;
ALTER TABLE `0_stock_category` ADD COLUMN `dflt_dim2` int(11) default NULL;

ALTER TABLE `0_users` ADD `sticky_doc_date` TINYINT(1) DEFAULT '0';

ALTER TABLE `0_debtors_master` MODIFY COLUMN `name` varchar(100) NOT NULL default '';
ALTER TABLE `0_cust_branch` ADD `inactive` tinyint(1) NOT NULL default '0';
ALTER TABLE `0_chart_class` ADD `sign_convert` tinyint(1) NOT NULL default '0';
UPDATE `0_chart_class` SET sign_convert=1 WHERE cid=3 OR cid=4 OR cid=5;
