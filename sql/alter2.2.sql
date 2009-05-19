ALTER TABLE `0_company` DROP COLUMN `custom1_name`;
ALTER TABLE `0_company` DROP COLUMN `custom2_name`;
ALTER TABLE `0_company` DROP COLUMN `custom3_name`;
ALTER TABLE `0_company` DROP COLUMN `custom1_value`;
ALTER TABLE `0_company` DROP COLUMN `custom2_value`;
ALTER TABLE `0_company` DROP COLUMN `custom3_value`;

ALTER TABLE `0_company` ADD COLUMN `default_delivery_required` SMALLINT(6) NULL DEFAULT '1';
ALTER TABLE `0_company` ADD COLUMN `version_id` VARCHAR(11) NOT NULL DEFAULT '';
ALTER TABLE `0_company` DROP COLUMN `purch_exchange_diff_act`;
ALTER TABLE `0_company` ADD COLUMN`profit_loss_year_act` VARCHAR(11) NOT NULL DEFAULT '' AFTER `exchange_diff_act`;
ALTER TABLE `0_company` ADD COLUMN `time_zone` TINYINT(1) NOT NULL DEFAULT '0';
#INSERT INTO `0_chart_master` VALUES ('8900', '', 'Profit and Loss this year', '52', '0');
UPDATE `0_company` SET `profit_loss_year_act`='8900', `version_id`='2.2' WHERE `coy_code`=1; 

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
ALTER TABLE `0_stock_category` ADD COLUMN `dflt_no_sale` tinyint(1) NOT NULL default '0';

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

ALTER TABLE `0_users` DROP PRIMARY KEY;
ALTER TABLE `0_users` ADD `id` SMALLINT(6) AUTO_INCREMENT PRIMARY KEY FIRST;
ALTER TABLE `0_users` ADD UNIQUE KEY (`user_id`);
ALTER TABLE `0_users` ADD COLUMN `inactive` tinyint(1) NOT NULL default '0';

DROP TABLE IF EXISTS `0_audit_trail`;
# fiscal_year, gl_date, gl_seq - journal sequence data
CREATE TABLE `0_audit_trail` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `type` smallint(6) unsigned NOT NULL default '0',
  `trans_no` int(11) unsigned NOT NULL default '0',
  `user` smallint(6) unsigned NOT NULL default '0',
  `stamp` timestamp NOT NULL default CURRENT_TIMESTAMP,
  `description` varchar(60) default NULL,
  `fiscal_year` int(11) NOT NULL,
  `gl_date` date NOT NULL default '0000-00-00',
  `gl_seq` int(11) unsigned default NULL,
   PRIMARY KEY (`id`),
  KEY (`fiscal_year`, `gl_seq`)
) TYPE=InnoDB  ;

ALTER TABLE `0_stock_master` ADD COLUMN `no_sale` tinyint(1) NOT NULL default '0';
ALTER TABLE `0_currencies` ADD COLUMN `auto_update` tinyint(1) NOT NULL default '1';
