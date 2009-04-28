ALTER TABLE `0_company` DROP COLUMN `custom1_name`;
ALTER TABLE `0_company` DROP COLUMN `custom2_name`;
ALTER TABLE `0_company` DROP COLUMN `custom3_name`;
ALTER TABLE `0_company` DROP COLUMN `custom1_value`;
ALTER TABLE `0_company` DROP COLUMN `custom2_value`;
ALTER TABLE `0_company` DROP COLUMN `custom3_value`;

ALTER TABLE `0_company` ADD COLUMN `default_delivery_required` SMALLINT(6) NULL DEFAULT '1';
ALTER TABLE `0_company` ADD COLUMN `version_id` VARCHAR(11) NOT NULL DEFAULT '';
ALTER TABLE `0_company` CHANGE `purch_exchange_diff_act` `profit_loss_year_act` VARCHAR(11) NOT NULL DEFAULT '';
INSERT INTO `0_chart_master` VALUES ('8900', '', 'Profit and Loss this year', '52', '0');
UPDATE `0_company` SET `profit_loss_year_act`='8900' WHERE `coy_code`=1; 

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

ALTER TABLE `0_users` ADD COLUMN `sticky_doc_date` TINYINT(1) DEFAULT '0';

ALTER TABLE `0_debtors_master` MODIFY COLUMN `name` varchar(100) NOT NULL default '';
ALTER TABLE `0_cust_branch` ADD COLUMN `inactive` tinyint(1) NOT NULL default '0';

ALTER TABLE `0_chart_class` ADD COLUMN `sign_convert` tinyint(1) NOT NULL default '0';
UPDATE `0_chart_class` SET `sign_convert`=1 WHERE `cid`=3 OR `cid`=4 OR `cid`=5;

ALTER TABLE `0_chart_class` ADD COLUMN `inactive` tinyint(1) NOT NULL default '0';
ALTER TABLE `0_chart_types` ADD COLUMN `inactive` tinyint(1) NOT NULL default '0';
ALTER TABLE `0_movement_types` ADD COLUMN `inactive` tinyint(1) NOT NULL default '0';
ALTER TABLE `0_item_tax_types` ADD COLUMN `inactive` tinyint(1) NOT NULL default '0';
ALTER TABLE `0_tax_types` ADD COLUMN `inactive` tinyint(1) NOT NULL default '0';
ALTER TABLE `0_tax_groups` ADD COLUMN `inactive` tinyint(1) NOT NULL default '0';
ALTER TABLE `0_tax_group_items` DROP COLUMN `included_in_price`;
