#
#	Database upgrade script Front Accounting 
#	Source version: 2.0.x
#	Target version:	2.1.0
#	
#	To make upgrades clean and failsafe:
#	* Precede all CREATE TABLE statment with DROP TABLE IF EXISTS
#	* Precede all ALTER TABLE statements using ADD column with respective
#		ALTER TABLE with DROP column
#	* Move all other DROP queries (e.g. removing obsolete tables) to installer
#		- they are not executed during non-forced upgrade.
#

DROP TABLE IF EXISTS `0_attachments`;

CREATE TABLE `0_attachments` (
  `id` int(11) NOT NULL auto_increment,
  `description` varchar(60) NOT NULL default '',
  `type_no` int(11) NOT NULL default '0',
  `trans_no` int(11) NOT NULL default '0',
  `bin_data` mediumblob NOT NULL,
  `tran_date` date NOT NULL default '0000-00-00',
  `filename` varchar(60) NOT NULL default '',
  `filesize` int(11) NOT NULL default '0',
  `filetype` varchar(60) NOT NULL default '',
  PRIMARY KEY  (`id`),
  KEY `type_no` (`type_no`,`trans_no`)
) TYPE=MyISAM AUTO_INCREMENT=1 ;

DROP TABLE IF EXISTS `0_groups`;

CREATE TABLE `0_groups` (
  `id` int(11) NOT NULL auto_increment,
  `description` varchar(60) NOT NULL default '',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `description` (`description`)
) TYPE=MyISAM AUTO_INCREMENT=1 ;

INSERT INTO `0_groups` VALUES ('1', 'Small');
INSERT INTO `0_groups` VALUES ('2', 'Medium');
INSERT INTO `0_groups` VALUES ('3', 'Large');

DROP TABLE IF EXISTS `0_recurrent_invoices`;

CREATE TABLE `0_recurrent_invoices` (
  `id` int(11) NOT NULL auto_increment,
  `description` varchar(60) NOT NULL default '',
  `order_no` int(11) NOT NULL default '0',
  `debtor_no` int(11) NOT NULL default '0',
  `group_no` int(11) NOT NULL default '0',
  `days` int(11) NOT NULL default '0',
  `monthly` int(11) NOT NULL default '0',
  `begin` date NOT NULL default '0000-00-00',
  `end` date NOT NULL default '0000-00-00',
  `last_sent` date NOT NULL default '0000-00-00',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `description` (`description`)
) TYPE=InnoDB AUTO_INCREMENT=1 ;

ALTER TABLE `0_cust_branch` DROP COLUMN `group_no`;
ALTER TABLE `0_cust_branch` ADD `group_no` int(11) NOT NULL default '0';

ALTER TABLE `0_debtor_trans` DROP COLUMN `dimension_id`;
ALTER TABLE `0_debtor_trans` ADD `dimension_id` int(11) NOT NULL default '0';
ALTER TABLE `0_debtor_trans` DROP COLUMN `dimension2_id`;
ALTER TABLE `0_debtor_trans` ADD `dimension2_id` int(11) NOT NULL default '0';

ALTER TABLE `0_bank_accounts` DROP COLUMN `id`;
ALTER TABLE `0_bank_accounts` DROP PRIMARY KEY;
ALTER TABLE `0_bank_accounts` ADD `id` SMALLINT(6) AUTO_INCREMENT PRIMARY KEY;

ALTER TABLE `0_users` DROP COLUMN `query_size`;
ALTER TABLE `0_users` ADD `query_size` TINYINT(1) DEFAULT '10';

ALTER TABLE `0_users` DROP COLUMN `graphic_links`;
ALTER TABLE `0_users` ADD `graphic_links` TINYINT(1) DEFAULT '1';

DROP TABLE IF EXISTS `0_sales_pos`;

CREATE TABLE `0_sales_pos` (
  `id` smallint(6) NOT NULL auto_increment,
  `pos_name` varchar(30) NOT NULL,
  `cash_sale` tinyint(1) NOT NULL,
  `credit_sale` tinyint(1) NOT NULL,
  `pos_location` varchar(5) NOT NULL,
  `pos_account` varchar(11) NOT NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY(`pos_name`)
) TYPE=MyISAM AUTO_INCREMENT=1;

INSERT INTO `0_sales_pos` VALUES ('1', 'Default', '1', '1', 'DEF', '1');

ALTER TABLE `0_users` DROP COLUMN `pos`;
ALTER TABLE `0_users` ADD `pos` SMALLINT(6) DEFAULT '1';

DROP TABLE IF EXISTS `0_quick_entries`;

CREATE TABLE `0_quick_entries` (
  `id` smallint(6) NOT NULL auto_increment,
  `description` varchar(60) NOT NULL,
  `deposit` tinyint(1) NOT NULL default '0',
  `bank_only` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `description` (`description`)
) TYPE=MyISAM AUTO_INCREMENT=1;

INSERT INTO `0_quick_entries` VALUES ('1', 'Maintenance', '0', '1');
INSERT INTO `0_quick_entries` VALUES ('2', 'Phone', '0', '1');
INSERT INTO `0_quick_entries` VALUES ('3', 'Cash Sales', '1', '1');

DROP TABLE IF EXISTS `0_quick_entry_lines`;

CREATE TABLE `0_quick_entry_lines` (
  `id` smallint(6) NOT NULL auto_increment,
  `qid` smallint(6) NOT NULL,
  `account` varchar(11) NOT NULL,
  `tax_acc` tinyint(1) NOT NULL default '0',
  `pct` tinyint(1) NOT NULL default '0',
  `amount` double default NULL default '0',
  `dimension_id` int(11) NOT NULL default '0',
  `dimension2_id` int(11) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `qid` (`qid`)
) TYPE=MyISAM AUTO_INCREMENT=1;

INSERT INTO `0_quick_entry_lines` VALUES ('1', '1', '6600', '1', '0', 0, '0', '0');
INSERT INTO `0_quick_entry_lines` VALUES ('2', '2', '6730', '1', '0', 0, '0', '0');
INSERT INTO `0_quick_entry_lines` VALUES ('3', '3', '3000', '1', '0', 0, '0', '0');

ALTER TABLE `0_users` DROP COLUMN `print_profile`;
ALTER TABLE `0_users` ADD `print_profile` VARCHAR(30) DEFAULT '' AFTER `show_hints` ;
ALTER TABLE `0_users` DROP COLUMN `rep_popup`;
ALTER TABLE `0_users` ADD `rep_popup` TINYINT(1) DEFAULT '1' AFTER `print_profile` ;

DROP TABLE IF EXISTS `0_print_profiles`;
CREATE TABLE `0_print_profiles` (
  `id` tinyint(11) NOT NULL auto_increment,
  `profile` varchar(30) NOT NULL,
  `report` varchar(5) NOT NULL,
  `printer` tinyint(5) NOT NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `profile` (`profile`,`report`)
) TYPE=MyISAM AUTO_INCREMENT=10;

INSERT INTO `0_print_profiles` VALUES ('1', 'Out of office', '', '0');
INSERT INTO `0_print_profiles` VALUES ('2', 'Sales Department', '', '0');
INSERT INTO `0_print_profiles` VALUES ('3', 'Central', '', '2');
INSERT INTO `0_print_profiles` VALUES ('4', 'Sales Department', '104', '2');
INSERT INTO `0_print_profiles` VALUES ('5', 'Sales Department', '105', '2');
INSERT INTO `0_print_profiles` VALUES ('6', 'Sales Department', '107', '2');
INSERT INTO `0_print_profiles` VALUES ('7', 'Sales Department', '109', '2');
INSERT INTO `0_print_profiles` VALUES ('8', 'Sales Department', '110', '2');
INSERT INTO `0_print_profiles` VALUES ('9', 'Sales Department', '201', '2');

DROP TABLE IF EXISTS `0_printers`;

CREATE TABLE `0_printers` (
  `id` smallint(6) NOT NULL auto_increment,
  `name` varchar(20) NOT NULL,
  `description` varchar(60) NOT NULL,
  `queue` varchar(20) NOT NULL,
  `host` varchar(40) NOT NULL,
  `port` smallint(11) unsigned NOT NULL,
  `timeout` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) TYPE=MyISAM AUTO_INCREMENT=4;

INSERT INTO `0_printers` VALUES ('1', 'QL500', 'Label printer', 'QL500', 'server', '127', '20');
INSERT INTO `0_printers` VALUES ('2', 'Samsung', 'Main network printer', 'scx4521F', 'server', '515', '5');
INSERT INTO `0_printers` VALUES ('3', 'Local', 'Local print server at user IP', 'lp', '', '515', '10');

DROP TABLE IF EXISTS `0_item_codes`;

CREATE TABLE `0_item_codes` (
  `id` int(11) NOT NULL auto_increment,
  `item_code` varchar(20) NOT NULL,
  `stock_id` varchar(20) NOT NULL,
  `description` varchar(200) NOT NULL default '',
  `category_id` int(11) NOT NULL,
  `quantity` double NOT NULL default '1',
  `is_foreign` tinyint(1) NOT NULL default 0,
  PRIMARY KEY  (`id`),
  UNIQUE KEY(`stock_id`, `item_code`)
) TYPE=MyISAM AUTO_INCREMENT=1;

ALTER TABLE `0_company` DROP COLUMN `foreign_codes`;
ALTER TABLE `0_company` ADD `foreign_codes` TINYINT(1) NOT NULL DEFAULT '0';

ALTER TABLE `0_suppliers` DROP COLUMN `supp_address`;
ALTER TABLE `0_suppliers` ADD `supp_address` tinytext NOT NULL DEFAULT '' AFTER `address`;

ALTER TABLE `0_suppliers` DROP COLUMN `phone`;
ALTER TABLE `0_suppliers` ADD `phone` varchar(30) NOT NULL DEFAULT '' AFTER `supp_address`;

ALTER TABLE `0_suppliers` DROP COLUMN `fax`;
ALTER TABLE `0_suppliers` ADD `fax` varchar(30) NOT NULL DEFAULT '' AFTER `phone`;

ALTER TABLE `0_suppliers` DROP COLUMN `gst_no`;
ALTER TABLE `0_suppliers` ADD `gst_no` varchar(25) NOT NULL DEFAULT '' AFTER `fax`;

ALTER TABLE `0_suppliers` DROP COLUMN `contact`;
ALTER TABLE `0_suppliers` ADD `contact` varchar(60) NOT NULL DEFAULT '' AFTER `gst_no`;

ALTER TABLE `0_suppliers` DROP COLUMN `credit_limit`;
ALTER TABLE `0_suppliers` ADD `credit_limit` double NOT NULL DEFAULT '0' AFTER `tax_group_id`;

ALTER TABLE `0_chart_types` DROP INDEX `name`, ADD INDEX `name` ( `name` );
