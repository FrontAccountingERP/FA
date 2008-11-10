DROP TABLE IF EXISTS `0_bank_trans_types`;

ALTER TABLE `0_bank_accounts` DROP PRIMARY KEY;
ALTER TABLE `0_bank_accounts` ADD `id` SMALLINT(6) AUTO_INCREMENT PRIMARY KEY;
ALTER TABLE `0_bank_accounts` ADD KEY (`account_code`);

# Version for any MySQL but usable with digital only gl account codes:
# UPDATE 0_bank_accounts SET id = account_code;
# For any Applicable only to  MySQL >=4.0.4 :
UPDATE `0_bank_trans`, `0_bank_accounts` SET 0_bank_trans.bank_act=0_bank_accounts.id 
	WHERE 0_bank_trans.bank_act=0_bank_accounts.account_code; 


ALTER TABLE `0_users` ADD `query_size` TINYINT(1) DEFAULT '10';

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

ALTER TABLE `0_users` ADD `print_profile` VARCHAR(30) DEFAULT '' AFTER `show_hints` ;
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
