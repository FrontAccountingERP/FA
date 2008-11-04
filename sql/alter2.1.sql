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
) ENGINE=MyISAM AUTO_INCREMENT=3;

INSERT INTO `0_sales_pos` VALUES ('1', 'Default', '1', '1', 'DEF', '1705');
INSERT INTO `0_sales_pos` VALUES ('2', 'Cash sale', '1', '0', 'CWA', '1705');

ALTER TABLE `0_users` ADD `pos` SMALLINT(6) DEFAULT '1';

DROP TABLE IF EXISTS `0_quick_entries`;

CREATE TABLE `0_quick_entries` (
  `id` smallint(6) NOT NULL auto_increment,
  `description` varchar(60) NOT NULL,
  `account` varchar(11) NOT NULL,
  `deposit` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `description` (`description`)
) ENGINE=MyISAM AUTO_INCREMENT=1;

INSERT INTO `0_quick_entries` VALUES ('1', 'Maintenance', '6600', '0');
INSERT INTO `0_quick_entries` VALUES ('2', 'Phone', '6730', '0');
INSERT INTO `0_quick_entries` VALUES ('3', 'Cash Sales', '3000', '1');

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
) ENGINE=MyISAM AUTO_INCREMENT=10;

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
) ENGINE=MyISAM AUTO_INCREMENT=4;

INSERT INTO `0_printers` VALUES ('1', 'QL500', 'Label printer', 'QL500', 'server', '127', '20');
INSERT INTO `0_printers` VALUES ('2', 'Samsung', 'Main network printer', 'scx4521F', 'server', '515', '5');
INSERT INTO `0_printers` VALUES ('3', 'Local', 'Local print server at user IP', 'lp', '', '515', '10');
