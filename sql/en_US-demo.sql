-- phpMyAdmin SQL Dump
-- version 2.11.8.1deb5
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Sep 26, 2014 at 11:32 AM
-- Server version: 5.0.51
-- PHP Version: 5.2.6-1+lenny2

--
-- Database: `fatest`
--

-- --------------------------------------------------------

--
-- Table structure for table `0_areas`
--

DROP TABLE IF EXISTS `0_areas`;
CREATE TABLE IF NOT EXISTS `0_areas` (
  `area_code` int(11) NOT NULL auto_increment,
  `description` varchar(60) NOT NULL default '',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`area_code`),
  UNIQUE KEY `description` (`description`)
) ENGINE=MyISAM  AUTO_INCREMENT=2 ;

--
-- Dumping data for table `0_areas`
--

INSERT INTO `0_areas` VALUES(1, 'Global', 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_attachments`
--

DROP TABLE IF EXISTS `0_attachments`;
CREATE TABLE IF NOT EXISTS `0_attachments` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `description` varchar(60) NOT NULL default '',
  `type_no` int(11) NOT NULL default '0',
  `trans_no` int(11) NOT NULL default '0',
  `unique_name` varchar(60) NOT NULL default '',
  `tran_date` date NOT NULL default '0000-00-00',
  `filename` varchar(60) NOT NULL default '',
  `filesize` int(11) NOT NULL default '0',
  `filetype` varchar(60) NOT NULL default '',
  PRIMARY KEY  (`id`),
  KEY `type_no` (`type_no`,`trans_no`)
) ENGINE=MyISAM AUTO_INCREMENT=1 ;

--
-- Dumping data for table `0_attachments`
--


-- --------------------------------------------------------

--
-- Table structure for table `0_audit_trail`
--

DROP TABLE IF EXISTS `0_audit_trail`;
CREATE TABLE IF NOT EXISTS `0_audit_trail` (
  `id` int(11) NOT NULL auto_increment,
  `type` smallint(6) unsigned NOT NULL default '0',
  `trans_no` int(11) unsigned NOT NULL default '0',
  `user` smallint(6) unsigned NOT NULL default '0',
  `stamp` timestamp NOT NULL,
  `description` varchar(60) default NULL,
  `fiscal_year` int(11) NOT NULL,
  `gl_date` date NOT NULL default '0000-00-00',
  `gl_seq` int(11) unsigned default NULL,
  PRIMARY KEY  (`id`),
  KEY `Seq` (`fiscal_year`,`gl_date`,`gl_seq`),
  KEY `Type_and_Number` (`type`,`trans_no`)
) ENGINE=InnoDB AUTO_INCREMENT=1 ;

--
-- Dumping data for table `0_audit_trail`
--


-- --------------------------------------------------------

--
-- Table structure for table `0_bank_accounts`
--

DROP TABLE IF EXISTS `0_bank_accounts`;
CREATE TABLE IF NOT EXISTS `0_bank_accounts` (
  `account_code` varchar(15) NOT NULL default '',
  `account_type` smallint(6) NOT NULL default '0',
  `bank_account_name` varchar(60) NOT NULL default '',
  `bank_account_number` varchar(100) NOT NULL default '',
  `bank_name` varchar(60) NOT NULL default '',
  `bank_address` tinytext,
  `bank_curr_code` char(3) NOT NULL default '',
  `dflt_curr_act` tinyint(1) NOT NULL default '0',
  `id` smallint(6) NOT NULL auto_increment,
  `last_reconciled_date` timestamp NOT NULL default '0000-00-00 00:00:00',
  `ending_reconcile_balance` double NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `bank_account_name` (`bank_account_name`),
  KEY `bank_account_number` (`bank_account_number`),
  KEY `account_code` (`account_code`)
) ENGINE=MyISAM  AUTO_INCREMENT=3 ;

--
-- Dumping data for table `0_bank_accounts`
--

INSERT INTO `0_bank_accounts` VALUES('1060', 0, 'Current account', '9999999999', 'Wachovia Bank', '', 'USD', 1, 1, '0000-00-00 00:00:00', 0, 0);
INSERT INTO `0_bank_accounts` VALUES('1065', 3, 'Petty Cash account', 'N/A', 'N/A', '', 'USD', 0, 2, '0000-00-00 00:00:00', 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_bank_trans`
--

DROP TABLE IF EXISTS `0_bank_trans`;
CREATE TABLE IF NOT EXISTS `0_bank_trans` (
  `id` int(11) NOT NULL auto_increment,
  `type` smallint(6) default NULL,
  `trans_no` int(11) default NULL,
  `bank_act` varchar(15) NOT NULL default '',
  `ref` varchar(40) default NULL,
  `trans_date` date NOT NULL default '0000-00-00',
  `amount` double default NULL,
  `dimension_id` int(11) NOT NULL default '0',
  `dimension2_id` int(11) NOT NULL default '0',
  `person_type_id` int(11) NOT NULL default '0',
  `person_id` tinyblob,
  `reconciled` date default NULL,
  PRIMARY KEY  (`id`),
  KEY `bank_act` (`bank_act`,`ref`),
  KEY `type` (`type`,`trans_no`),
  KEY `bank_act_2` (`bank_act`,`reconciled`),
  KEY `bank_act_3` (`bank_act`,`trans_date`)
) ENGINE=InnoDB  AUTO_INCREMENT=12 ;

--
-- Dumping data for table `0_bank_trans`
--

INSERT INTO `0_bank_trans` VALUES(1, 22, 4, '1', '1', '2014-06-21', -3465, 0, 0, 3, '2', NULL);
INSERT INTO `0_bank_trans` VALUES(2, 26, 1, '1', '', '2014-06-21', -10, 0, 0, 1, '1', NULL);
INSERT INTO `0_bank_trans` VALUES(3, 26, 1, '1', '', '2014-06-21', -20, 0, 0, 1, '0', NULL);
INSERT INTO `0_bank_trans` VALUES(4, 0, 18, '1', '1', '2014-02-20', 1000, 0, 0, 0, '', NULL);
INSERT INTO `0_bank_trans` VALUES(5, 0, 19, '1', '2', '2014-02-21', 4000, 0, 0, 0, '', NULL);
INSERT INTO `0_bank_trans` VALUES(6, 2, 5, '1', '1', '2014-06-21', 100, 0, 0, 4, '3', NULL);
INSERT INTO `0_bank_trans` VALUES(7, 1, 8, '1', '1', '2014-06-21', -50, 0, 0, 4, '1', NULL);
INSERT INTO `0_bank_trans` VALUES(8, 26, 5, '1', '', '2014-06-21', -10, 0, 0, 1, '1', NULL);
INSERT INTO `0_bank_trans` VALUES(9, 26, 5, '1', '', '2014-06-21', -20, 0, 0, 1, '0', NULL);
INSERT INTO `0_bank_trans` VALUES(10, 26, 7, '1', '', '2014-06-21', -10, 0, 0, 1, '1', NULL);
INSERT INTO `0_bank_trans` VALUES(11, 26, 7, '1', '', '2014-06-21', -20, 0, 0, 1, '0', NULL);

-- --------------------------------------------------------

--
-- Table structure for table `0_bom`
--

DROP TABLE IF EXISTS `0_bom`;
CREATE TABLE IF NOT EXISTS `0_bom` (
  `id` int(11) NOT NULL auto_increment,
  `parent` char(20) NOT NULL default '',
  `component` char(20) NOT NULL default '',
  `workcentre_added` int(11) NOT NULL default '0',
  `loc_code` char(5) NOT NULL default '',
  `quantity` double NOT NULL default '1',
  PRIMARY KEY  (`parent`,`component`,`workcentre_added`,`loc_code`),
  KEY `component` (`component`),
  KEY `id` (`id`),
  KEY `loc_code` (`loc_code`),
  KEY `parent` (`parent`,`loc_code`),
  KEY `workcentre_added` (`workcentre_added`)
) ENGINE=MyISAM  AUTO_INCREMENT=4 ;

--
-- Dumping data for table `0_bom`
--

INSERT INTO `0_bom` VALUES(1, '3400', '102', 1, 'DEF', 1);
INSERT INTO `0_bom` VALUES(2, '3400', '103', 1, 'DEF', 1);
INSERT INTO `0_bom` VALUES(3, '3400', '104', 1, 'DEF', 1);

-- --------------------------------------------------------

--
-- Table structure for table `0_budget_trans`
--

DROP TABLE IF EXISTS `0_budget_trans`;
CREATE TABLE IF NOT EXISTS `0_budget_trans` (
  `counter` int(11) NOT NULL auto_increment,
  `type` smallint(6) NOT NULL default '0',
  `type_no` bigint(16) NOT NULL default '1',
  `tran_date` date NOT NULL default '0000-00-00',
  `account` varchar(15) NOT NULL default '',
  `memo_` tinytext NOT NULL,
  `amount` double NOT NULL default '0',
  `dimension_id` int(11) default '0',
  `dimension2_id` int(11) default '0',
  `person_type_id` int(11) default NULL,
  `person_id` tinyblob,
  PRIMARY KEY  (`counter`),
  KEY `Type_and_Number` (`type`,`type_no`),
  KEY `Account` (`account`,`tran_date`,`dimension_id`,`dimension2_id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 ;

--
-- Dumping data for table `0_budget_trans`
--


-- --------------------------------------------------------

--
-- Table structure for table `0_chart_class`
--

DROP TABLE IF EXISTS `0_chart_class`;
CREATE TABLE IF NOT EXISTS `0_chart_class` (
  `cid` varchar(3) NOT NULL,
  `class_name` varchar(60) NOT NULL default '',
  `ctype` tinyint(1) NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`cid`)
) ENGINE=MyISAM;

--
-- Dumping data for table `0_chart_class`
--

INSERT INTO `0_chart_class` VALUES('1', 'Assets', 1, 0);
INSERT INTO `0_chart_class` VALUES('2', 'Liabilities', 2, 0);
INSERT INTO `0_chart_class` VALUES('3', 'Income', 4, 0);
INSERT INTO `0_chart_class` VALUES('4', 'Costs', 6, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_chart_master`
--

DROP TABLE IF EXISTS `0_chart_master`;
CREATE TABLE IF NOT EXISTS `0_chart_master` (
  `account_code` varchar(15) NOT NULL default '',
  `account_code2` varchar(15) NOT NULL default '',
  `account_name` varchar(60) NOT NULL default '',
  `account_type` varchar(10) NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`account_code`),
  KEY `account_name` (`account_name`),
  KEY `accounts_by_type` (`account_type`,`account_code`)
) ENGINE=MyISAM;

--
-- Dumping data for table `0_chart_master`
--

INSERT INTO `0_chart_master` VALUES('1060', '', 'Checking Account', '1', 0);
INSERT INTO `0_chart_master` VALUES('1065', '', 'Petty Cash', '1', 0);
INSERT INTO `0_chart_master` VALUES('1200', '', 'Accounts Receivables', '1', 0);
INSERT INTO `0_chart_master` VALUES('1205', '', 'Allowance for doubtful accounts', '1', 0);
INSERT INTO `0_chart_master` VALUES('1510', '', 'Inventory', '2', 0);
INSERT INTO `0_chart_master` VALUES('1520', '', 'Stocks of Raw Materials', '2', 0);
INSERT INTO `0_chart_master` VALUES('1530', '', 'Stocks of Work In Progress', '2', 0);
INSERT INTO `0_chart_master` VALUES('1540', '', 'Stocks of Finsihed Goods', '2', 0);
INSERT INTO `0_chart_master` VALUES('1550', '', 'Goods Received Clearing account', '2', 0);
INSERT INTO `0_chart_master` VALUES('1820', '', 'Office Furniture &amp; Equipment', '3', 0);
INSERT INTO `0_chart_master` VALUES('1825', '', 'Accum. Amort. -Furn. &amp; Equip.', '3', 0);
INSERT INTO `0_chart_master` VALUES('1840', '', 'Vehicle', '3', 0);
INSERT INTO `0_chart_master` VALUES('1845', '', 'Accum. Amort. -Vehicle', '3', 0);
INSERT INTO `0_chart_master` VALUES('2100', '', 'Accounts Payable', '4', 0);
INSERT INTO `0_chart_master` VALUES('2110', '', 'Accrued Income Tax - Federal', '4', 0);
INSERT INTO `0_chart_master` VALUES('2120', '', 'Accrued Income Tax - State', '4', 0);
INSERT INTO `0_chart_master` VALUES('2130', '', 'Accrued Franchise Tax', '4', 0);
INSERT INTO `0_chart_master` VALUES('2140', '', 'Accrued Real &amp; Personal Prop Tax', '4', 0);
INSERT INTO `0_chart_master` VALUES('2150', '', 'Sales Tax', '4', 0);
INSERT INTO `0_chart_master` VALUES('2160', '', 'Accrued Use Tax Payable', '4', 0);
INSERT INTO `0_chart_master` VALUES('2210', '', 'Accrued Wages', '4', 0);
INSERT INTO `0_chart_master` VALUES('2220', '', 'Accrued Comp Time', '4', 0);
INSERT INTO `0_chart_master` VALUES('2230', '', 'Accrued Holiday Pay', '4', 0);
INSERT INTO `0_chart_master` VALUES('2240', '', 'Accrued Vacation Pay', '4', 0);
INSERT INTO `0_chart_master` VALUES('2310', '', 'Accr. Benefits - 401K', '4', 0);
INSERT INTO `0_chart_master` VALUES('2320', '', 'Accr. Benefits - Stock Purchase', '4', 0);
INSERT INTO `0_chart_master` VALUES('2330', '', 'Accr. Benefits - Med, Den', '4', 0);
INSERT INTO `0_chart_master` VALUES('2340', '', 'Accr. Benefits - Payroll Taxes', '4', 0);
INSERT INTO `0_chart_master` VALUES('2350', '', 'Accr. Benefits - Credit Union', '4', 0);
INSERT INTO `0_chart_master` VALUES('2360', '', 'Accr. Benefits - Savings Bond', '4', 0);
INSERT INTO `0_chart_master` VALUES('2370', '', 'Accr. Benefits - Garnish', '4', 0);
INSERT INTO `0_chart_master` VALUES('2380', '', 'Accr. Benefits - Charity Cont.', '4', 0);
INSERT INTO `0_chart_master` VALUES('2620', '', 'Bank Loans', '5', 0);
INSERT INTO `0_chart_master` VALUES('2680', '', 'Loans from Shareholders', '5', 0);
INSERT INTO `0_chart_master` VALUES('3350', '', 'Common Shares', '6', 0);
INSERT INTO `0_chart_master` VALUES('3590', '', 'Retained Earnings - prior years', '7', 0);
INSERT INTO `0_chart_master` VALUES('4010', '', 'Sales', '8', 0);
INSERT INTO `0_chart_master` VALUES('4430', '', 'Shipping &amp; Handling', '9', 0);
INSERT INTO `0_chart_master` VALUES('4440', '', 'Interest', '9', 0);
INSERT INTO `0_chart_master` VALUES('4450', '', 'Foreign Exchange Gain', '9', 0);
INSERT INTO `0_chart_master` VALUES('4500', '', 'Prompt Payment Discounts', '9', 0);
INSERT INTO `0_chart_master` VALUES('4510', '', 'Discounts Given', '9', 0);
INSERT INTO `0_chart_master` VALUES('5010', '', 'Cost of Goods Sold - Retail', '10', 0);
INSERT INTO `0_chart_master` VALUES('5020', '', 'Material Usage Varaiance', '10', 0);
INSERT INTO `0_chart_master` VALUES('5030', '', 'Consumable Materials', '10', 0);
INSERT INTO `0_chart_master` VALUES('5040', '', 'Purchase price Variance', '10', 0);
INSERT INTO `0_chart_master` VALUES('5050', '', 'Purchases of materials', '10', 0);
INSERT INTO `0_chart_master` VALUES('5060', '', 'Discounts Received', '10', 0);
INSERT INTO `0_chart_master` VALUES('5100', '', 'Freight', '10', 0);
INSERT INTO `0_chart_master` VALUES('5410', '', 'Wages &amp; Salaries', '11', 0);
INSERT INTO `0_chart_master` VALUES('5420', '', 'Wages - Overtime', '11', 0);
INSERT INTO `0_chart_master` VALUES('5430', '', 'Benefits - Comp Time', '11', 0);
INSERT INTO `0_chart_master` VALUES('5440', '', 'Benefits - Payroll Taxes', '11', 0);
INSERT INTO `0_chart_master` VALUES('5450', '', 'Benefits - Workers Comp', '11', 0);
INSERT INTO `0_chart_master` VALUES('5460', '', 'Benefits - Pension', '11', 0);
INSERT INTO `0_chart_master` VALUES('5470', '', 'Benefits - General Benefits', '11', 0);
INSERT INTO `0_chart_master` VALUES('5510', '', 'Inc Tax Exp - Federal', '11', 0);
INSERT INTO `0_chart_master` VALUES('5520', '', 'Inc Tax Exp - State', '11', 0);
INSERT INTO `0_chart_master` VALUES('5530', '', 'Taxes - Real Estate', '11', 0);
INSERT INTO `0_chart_master` VALUES('5540', '', 'Taxes - Personal Property', '11', 0);
INSERT INTO `0_chart_master` VALUES('5550', '', 'Taxes - Franchise', '11', 0);
INSERT INTO `0_chart_master` VALUES('5560', '', 'Taxes - Foreign Withholding', '11', 0);
INSERT INTO `0_chart_master` VALUES('5610', '', 'Accounting &amp; Legal', '12', 0);
INSERT INTO `0_chart_master` VALUES('5615', '', 'Advertising &amp; Promotions', '12', 0);
INSERT INTO `0_chart_master` VALUES('5620', '', 'Bad Debts', '12', 0);
INSERT INTO `0_chart_master` VALUES('5660', '', 'Amortization Expense', '12', 0);
INSERT INTO `0_chart_master` VALUES('5685', '', 'Insurance', '12', 0);
INSERT INTO `0_chart_master` VALUES('5690', '', 'Interest &amp; Bank Charges', '12', 0);
INSERT INTO `0_chart_master` VALUES('5700', '', 'Office Supplies', '12', 0);
INSERT INTO `0_chart_master` VALUES('5760', '', 'Rent', '12', 0);
INSERT INTO `0_chart_master` VALUES('5765', '', 'Repair &amp; Maintenance', '12', 0);
INSERT INTO `0_chart_master` VALUES('5780', '', 'Telephone', '12', 0);
INSERT INTO `0_chart_master` VALUES('5785', '', 'Travel &amp; Entertainment', '12', 0);
INSERT INTO `0_chart_master` VALUES('5790', '', 'Utilities', '12', 0);
INSERT INTO `0_chart_master` VALUES('5795', '', 'Registrations', '12', 0);
INSERT INTO `0_chart_master` VALUES('5800', '', 'Licenses', '12', 0);
INSERT INTO `0_chart_master` VALUES('5810', '', 'Foreign Exchange Loss', '12', 0);
INSERT INTO `0_chart_master` VALUES('9990', '', 'Year Profit/Loss', '12', 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_chart_types`
--

DROP TABLE IF EXISTS `0_chart_types`;
CREATE TABLE IF NOT EXISTS `0_chart_types` (
  `id` varchar(10) NOT NULL,
  `name` varchar(60) NOT NULL default '',
  `class_id` varchar(3) NOT NULL default '',
  `parent` varchar(10) NOT NULL default '-1',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `name` (`name`),
  KEY `class_id` (`class_id`)
) ENGINE=MyISAM ;

--
-- Dumping data for table `0_chart_types`
--

INSERT INTO `0_chart_types` VALUES('1', 'Current Assets', '1', '', 0);
INSERT INTO `0_chart_types` VALUES('2', 'Inventory Assets', '1', '', 0);
INSERT INTO `0_chart_types` VALUES('3', 'Capital Assets', '1', '', 0);
INSERT INTO `0_chart_types` VALUES('4', 'Current Liabilities', '2', '', 0);
INSERT INTO `0_chart_types` VALUES('5', 'Long Term Liabilities', '2', '', 0);
INSERT INTO `0_chart_types` VALUES('6', 'Share Capital', '2', '', 0);
INSERT INTO `0_chart_types` VALUES('7', 'Retained Earnings', '2', '', 0);
INSERT INTO `0_chart_types` VALUES('8', 'Sales Revenue', '3', '', 0);
INSERT INTO `0_chart_types` VALUES('9', 'Other Revenue', '3', '', 0);
INSERT INTO `0_chart_types` VALUES('10', 'Cost of Goods Sold', '4', '', 0);
INSERT INTO `0_chart_types` VALUES('11', 'Payroll Expenses', '4', '', 0);
INSERT INTO `0_chart_types` VALUES('12', 'General &amp; Administrative expenses', '4', '', 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_comments`
--

DROP TABLE IF EXISTS `0_comments`;
CREATE TABLE IF NOT EXISTS `0_comments` (
  `type` int(11) NOT NULL default '0',
  `id` int(11) NOT NULL default '0',
  `date_` date default '0000-00-00',
  `memo_` tinytext,
  KEY `type_and_id` (`type`,`id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `0_comments`
--


-- --------------------------------------------------------

--
-- Table structure for table `0_credit_status`
--

DROP TABLE IF EXISTS `0_credit_status`;
CREATE TABLE IF NOT EXISTS `0_credit_status` (
  `id` int(11) NOT NULL auto_increment,
  `reason_description` char(100) NOT NULL default '',
  `dissallow_invoices` tinyint(1) NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `reason_description` (`reason_description`)
) ENGINE=MyISAM  AUTO_INCREMENT=5 ;

--
-- Dumping data for table `0_credit_status`
--

INSERT INTO `0_credit_status` VALUES(1, 'Good History', 0, 0);
INSERT INTO `0_credit_status` VALUES(3, 'No more work until payment received', 1, 0);
INSERT INTO `0_credit_status` VALUES(4, 'In liquidation', 1, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_crm_categories`
--

DROP TABLE IF EXISTS `0_crm_categories`;
CREATE TABLE IF NOT EXISTS `0_crm_categories` (
  `id` int(11) NOT NULL auto_increment COMMENT 'pure technical key',
  `type` varchar(20) NOT NULL COMMENT 'contact type e.g. customer',
  `action` varchar(20) NOT NULL COMMENT 'detailed usage e.g. department',
  `name` varchar(30) NOT NULL COMMENT 'for category selector',
  `description` tinytext NOT NULL COMMENT 'usage description',
  `system` tinyint(1) NOT NULL default '0' COMMENT 'nonzero for core system usage',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `type` (`type`,`action`),
  UNIQUE KEY `type_2` (`type`,`name`)
) ENGINE=InnoDB  AUTO_INCREMENT=13 ;

--
-- Dumping data for table `0_crm_categories`
--

INSERT INTO `0_crm_categories` VALUES(1, 'cust_branch', 'general', 'General', 'General contact data for customer branch (overrides company setting)', 1, 0);
INSERT INTO `0_crm_categories` VALUES(2, 'cust_branch', 'invoice', 'Invoices', 'Invoice posting (overrides company setting)', 1, 0);
INSERT INTO `0_crm_categories` VALUES(3, 'cust_branch', 'order', 'Orders', 'Order confirmation (overrides company setting)', 1, 0);
INSERT INTO `0_crm_categories` VALUES(4, 'cust_branch', 'delivery', 'Deliveries', 'Delivery coordination (overrides company setting)', 1, 0);
INSERT INTO `0_crm_categories` VALUES(5, 'customer', 'general', 'General', 'General contact data for customer', 1, 0);
INSERT INTO `0_crm_categories` VALUES(6, 'customer', 'order', 'Orders', 'Order confirmation', 1, 0);
INSERT INTO `0_crm_categories` VALUES(7, 'customer', 'delivery', 'Deliveries', 'Delivery coordination', 1, 0);
INSERT INTO `0_crm_categories` VALUES(8, 'customer', 'invoice', 'Invoices', 'Invoice posting', 1, 0);
INSERT INTO `0_crm_categories` VALUES(9, 'supplier', 'general', 'General', 'General contact data for supplier', 1, 0);
INSERT INTO `0_crm_categories` VALUES(10, 'supplier', 'order', 'Orders', 'Order confirmation', 1, 0);
INSERT INTO `0_crm_categories` VALUES(11, 'supplier', 'delivery', 'Deliveries', 'Delivery coordination', 1, 0);
INSERT INTO `0_crm_categories` VALUES(12, 'supplier', 'invoice', 'Invoices', 'Invoice posting', 1, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_crm_contacts`
--

DROP TABLE IF EXISTS `0_crm_contacts`;
CREATE TABLE IF NOT EXISTS `0_crm_contacts` (
  `id` int(11) NOT NULL auto_increment,
  `person_id` int(11) NOT NULL default '0' COMMENT 'foreign key to crm_contacts',
  `type` varchar(20) NOT NULL COMMENT 'foreign key to crm_categories',
  `action` varchar(20) NOT NULL COMMENT 'foreign key to crm_categories',
  `entity_id` varchar(11) default NULL COMMENT 'entity id in related class table',
  PRIMARY KEY  (`id`),
  KEY `type` (`type`,`action`)
) ENGINE=InnoDB  AUTO_INCREMENT=10 ;

--
-- Dumping data for table `0_crm_contacts`
--

INSERT INTO `0_crm_contacts` VALUES(1, 1, 'customer', 'general', '1');
INSERT INTO `0_crm_contacts` VALUES(2, 2, 'customer', 'general', '2');
INSERT INTO `0_crm_contacts` VALUES(3, 3, 'customer', 'general', '3');
INSERT INTO `0_crm_contacts` VALUES(4, 4, 'cust_branch', 'general', '1');
INSERT INTO `0_crm_contacts` VALUES(5, 5, 'cust_branch', 'general', '2');
INSERT INTO `0_crm_contacts` VALUES(6, 6, 'cust_branch', 'general', '3');
INSERT INTO `0_crm_contacts` VALUES(7, 7, 'supplier', 'general', '1');
INSERT INTO `0_crm_contacts` VALUES(8, 8, 'supplier', 'general', '2');
INSERT INTO `0_crm_contacts` VALUES(9, 9, 'supplier', 'general', '3');

-- --------------------------------------------------------

--
-- Table structure for table `0_crm_persons`
--

DROP TABLE IF EXISTS `0_crm_persons`;
CREATE TABLE IF NOT EXISTS `0_crm_persons` (
  `id` int(11) NOT NULL auto_increment,
  `ref` varchar(30) NOT NULL,
  `name` varchar(60) NOT NULL,
  `name2` varchar(60) default NULL,
  `address` tinytext,
  `phone` varchar(30) default NULL,
  `phone2` varchar(30) default NULL,
  `fax` varchar(30) default NULL,
  `email` varchar(100) default NULL,
  `lang` char(5) default NULL,
  `notes` tinytext NOT NULL,
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `ref` (`ref`)
) ENGINE=InnoDB  AUTO_INCREMENT=10 ;

--
-- Dumping data for table `0_crm_persons`
--

INSERT INTO `0_crm_persons` VALUES(1, 'Beefeater', '', NULL, NULL, NULL, NULL, NULL, '', '', '', 0);
INSERT INTO `0_crm_persons` VALUES(2, 'Ghostbusters', '', NULL, NULL, NULL, NULL, NULL, '', NULL, '', 0);
INSERT INTO `0_crm_persons` VALUES(3, 'Brezan', '', NULL, NULL, NULL, NULL, NULL, '', '', '', 0);
INSERT INTO `0_crm_persons` VALUES(4, 'Beefeater', 'Main Branch', NULL, '', '', '', '', '', NULL, '', 0);
INSERT INTO `0_crm_persons` VALUES(5, 'Ghostbusters', 'Main Branch', NULL, 'Address 1\nAddress 2\nAddress 3', '', '', '', '', NULL, '', 0);
INSERT INTO `0_crm_persons` VALUES(6, 'Brezan', 'Main Branch', NULL, 'Address 1\nAddress 2\nAddress 3', '', '', '', '', NULL, '', 0);
INSERT INTO `0_crm_persons` VALUES(7, 'Junk Beer', 'Contact', NULL, 'Address 1\nAddress 2\nAddress 3', '+45 55667788', '', '', '', '', '', 0);
INSERT INTO `0_crm_persons` VALUES(8, 'Lucky Luke', 'Luke', NULL, 'Address 1\nAddress 2\nAddress 3', '(111) 222.333.444', '', '', '', NULL, '', 0);
INSERT INTO `0_crm_persons` VALUES(9, 'Money Makers', 'Makers', NULL, 'Address 1\nAddress 2\nAddress 3', '+44 444 555 666', '', '', '', '', '', 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_currencies`
--

DROP TABLE IF EXISTS `0_currencies`;
CREATE TABLE IF NOT EXISTS `0_currencies` (
  `currency` varchar(60) NOT NULL default '',
  `curr_abrev` char(3) NOT NULL default '',
  `curr_symbol` varchar(10) NOT NULL default '',
  `country` varchar(100) NOT NULL default '',
  `hundreds_name` varchar(15) NOT NULL default '',
  `auto_update` tinyint(1) NOT NULL default '1',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`curr_abrev`)
) ENGINE=MyISAM;

--
-- Dumping data for table `0_currencies`
--

INSERT INTO `0_currencies` VALUES('US Dollars', 'USD', '$', 'United States', 'Cents', 1, 0);
INSERT INTO `0_currencies` VALUES('CA Dollars', 'CAD', '$', 'Canada', 'Cents', 1, 0);
INSERT INTO `0_currencies` VALUES('Euro', 'EUR', '?', 'Europe', 'Cents', 1, 0);
INSERT INTO `0_currencies` VALUES('Pounds', 'GBP', '?', 'England', 'Pence', 1, 0);
INSERT INTO `0_currencies` VALUES('DK Kroner', 'DKK', 'kr', 'Denmark', 'Ore', 1, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_cust_allocations`
--

DROP TABLE IF EXISTS `0_cust_allocations`;
CREATE TABLE IF NOT EXISTS `0_cust_allocations` (
  `id` int(11) NOT NULL auto_increment,
  `amt` double unsigned default NULL,
  `date_alloc` date NOT NULL default '0000-00-00',
  `trans_no_from` int(11) default NULL,
  `trans_type_from` int(11) default NULL,
  `trans_no_to` int(11) default NULL,
  `trans_type_to` int(11) default NULL,
  PRIMARY KEY  (`id`),
  KEY `From` (`trans_type_from`,`trans_no_from`),
  KEY `To` (`trans_type_to`,`trans_no_to`)
) ENGINE=InnoDB  AUTO_INCREMENT=2 ;

--
-- Dumping data for table `0_cust_allocations`
--

INSERT INTO `0_cust_allocations` VALUES(1, 37.68, '2014-06-21', 3, 11, 18, 10);

-- --------------------------------------------------------

--
-- Table structure for table `0_cust_branch`
--

DROP TABLE IF EXISTS `0_cust_branch`;
CREATE TABLE IF NOT EXISTS `0_cust_branch` (
  `branch_code` int(11) NOT NULL auto_increment,
  `debtor_no` int(11) NOT NULL default '0',
  `br_name` varchar(60) NOT NULL default '',
  `branch_ref` varchar(30) NOT NULL default '',
  `br_address` tinytext NOT NULL,
  `area` int(11) default NULL,
  `salesman` int(11) NOT NULL default '0',
  `contact_name` varchar(60) NOT NULL default '',
  `default_location` varchar(5) NOT NULL default '',
  `tax_group_id` int(11) default NULL,
  `sales_account` varchar(15) NOT NULL default '',
  `sales_discount_account` varchar(15) NOT NULL default '',
  `receivables_account` varchar(15) NOT NULL default '',
  `payment_discount_account` varchar(15) NOT NULL default '',
  `default_ship_via` int(11) NOT NULL default '1',
  `disable_trans` tinyint(4) NOT NULL default '0',
  `br_post_address` tinytext NOT NULL,
  `group_no` int(11) NOT NULL default '0',
  `notes` tinytext NOT NULL,
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`branch_code`,`debtor_no`),
  KEY `branch_code` (`branch_code`),
  KEY `branch_ref` (`branch_ref`),
  KEY `group_no` (`group_no`)
) ENGINE=MyISAM  AUTO_INCREMENT=4 ;

--
-- Dumping data for table `0_cust_branch`
--

INSERT INTO `0_cust_branch` VALUES(1, 1, 'Beefeater Ltd.', 'Beefeater', '', 1, 1, 'Main Branch', 'DEF', 2, '', '4510', '1200', '4500', 1, 0, 'Address 1\nAddress 2\nAddress 3', 0, '', 0);
INSERT INTO `0_cust_branch` VALUES(2, 2, 'Ghostbusters Corp.', 'Ghostbusters', 'Address 1\nAddress 2\nAddress 3', 1, 1, 'Main Branch', 'DEF', 1, '', '4510', '1200', '4500', 1, 0, 'Address 1\nAddress 2\nAddress 3', 0, '', 0);
INSERT INTO `0_cust_branch` VALUES(3, 3, 'Brezan', 'Brezan', 'Address 1\nAddress 2\nAddress 3', 1, 1, 'Main Branch', 'DEF', 1, '', '4510', '1200', '4500', 1, 0, 'Address 1\nAddress 2\nAddress 3', 0, '', 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_debtors_master`
--

DROP TABLE IF EXISTS `0_debtors_master`;
CREATE TABLE IF NOT EXISTS `0_debtors_master` (
  `debtor_no` int(11) NOT NULL auto_increment,
  `name` varchar(100) NOT NULL default '',
  `debtor_ref` varchar(30) NOT NULL,
  `address` tinytext,
  `tax_id` varchar(55) NOT NULL default '',
  `curr_code` char(3) NOT NULL default '',
  `sales_type` int(11) NOT NULL default '1',
  `dimension_id` int(11) NOT NULL default '0',
  `dimension2_id` int(11) NOT NULL default '0',
  `credit_status` int(11) NOT NULL default '0',
  `payment_terms` int(11) default NULL,
  `discount` double NOT NULL default '0',
  `pymt_discount` double NOT NULL default '0',
  `credit_limit` float NOT NULL default '1000',
  `notes` tinytext NOT NULL,
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`debtor_no`),
  KEY `name` (`name`),
  UNIQUE KEY `debtor_ref` (`debtor_ref`)
) ENGINE=MyISAM  AUTO_INCREMENT=4 ;

--
-- Dumping data for table `0_debtors_master`
--

INSERT INTO `0_debtors_master` VALUES(1, 'Beefeater Ltd.', 'Beefeater', 'Addr 1\nAddr 2\nAddr 3', '345678', 'GBP', 2, 0, 0, 1, 3, 0, 0, 1000, '', 0);
INSERT INTO `0_debtors_master` VALUES(2, 'Ghostbusters Corp.', 'Ghostbusters', 'Address 1\nAddress 2\nAddress 3', '2222222', 'USD', 1, 0, 0, 1, 4, 0, 0, 1000, '', 0);
INSERT INTO `0_debtors_master` VALUES(3, 'Brezan', 'Brezan', 'Address 1\nAddress 2\nAddress 3', '7777777', 'EUR', 2, 0, 0, 1, 3, 0, 0, 1000, '', 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_debtor_trans`
--

DROP TABLE IF EXISTS `0_debtor_trans`;
CREATE TABLE IF NOT EXISTS `0_debtor_trans` (
  `trans_no` int(11) unsigned NOT NULL default '0',
  `type` smallint(6) unsigned NOT NULL default '0',
  `version` tinyint(1) unsigned NOT NULL default '0',
  `debtor_no` int(11) unsigned default NULL,
  `branch_code` int(11) NOT NULL default '-1',
  `tran_date` date NOT NULL default '0000-00-00',
  `due_date` date NOT NULL default '0000-00-00',
  `reference` varchar(60) NOT NULL default '',
  `tpe` int(11) NOT NULL default '0',
  `order_` int(11) NOT NULL default '0',
  `ov_amount` double NOT NULL default '0',
  `ov_gst` double NOT NULL default '0',
  `ov_freight` double NOT NULL default '0',
  `ov_freight_tax` double NOT NULL default '0',
  `ov_discount` double NOT NULL default '0',
  `alloc` double NOT NULL default '0',
  `rate` double NOT NULL default '1',
  `ship_via` int(11) default NULL,
  `dimension_id` int(11) NOT NULL default '0',
  `dimension2_id` int(11) NOT NULL default '0',
  `payment_terms` int(11) default NULL,
  PRIMARY KEY  (`type`,`trans_no`),
  KEY `debtor_no` (`debtor_no`,`branch_code`),
  KEY `tran_date` (`tran_date`),
  KEY `order_` (`order_`)
) ENGINE=InnoDB;

--
-- Dumping data for table `0_debtor_trans`
--

INSERT INTO `0_debtor_trans` VALUES(17, 10, 0, 2, 2, '2014-06-21', '2014-06-22', '1', 1, 2, 50, 2.5, 0, 0, 0, 0, 1, 1, 0, 0, 4);
INSERT INTO `0_debtor_trans` VALUES(18, 10, 1, 3, 3, '2014-06-21', '2014-07-01', '2', 2, 3, 35.89, 1.79, 0, 0, 0, 37.68, 1.3932, 1, 2, 0, 3);
INSERT INTO `0_debtor_trans` VALUES(19, 10, 0, 2, 2, '2014-06-21', '2014-06-22', '3', 1, 5, 50, 0, 5, 0, 0, 0, 1, 1, 0, 0, 4);
INSERT INTO `0_debtor_trans` VALUES(3, 11, 0, 3, 3, '2014-06-21', '0000-00-00', '1', 2, 3, 35.89, 1.79, 0, 0, 0, 37.68, 1.3932, 1, 2, 0, 3);
INSERT INTO `0_debtor_trans` VALUES(2, 13, 0, 1, 1, '2014-06-21', '2014-06-22', '1', 2, 1, 60.8, 0, 10, 0, 0, 0, 1.6445729799917, 1, 0, 0, 3);
INSERT INTO `0_debtor_trans` VALUES(3, 13, 1, 2, 2, '2014-06-21', '2014-06-22', 'auto', 1, 2, 50, 2.5, 0, 0, 0, 0, 1, 1, 0, 0, 4);
INSERT INTO `0_debtor_trans` VALUES(4, 13, 1, 3, 3, '2014-06-21', '2014-07-01', 'auto', 2, 3, 35.89, 1.79, 0, 0, 0, 0, 1.3932, 1, 2, 0, 3);
INSERT INTO `0_debtor_trans` VALUES(5, 13, 1, 2, 2, '2014-06-21', '2014-06-22', 'auto', 1, 5, 50, 0, 5, 0, 0, 0, 1, 1, 0, 0, 4);

-- --------------------------------------------------------

--
-- Table structure for table `0_debtor_trans_details`
--

DROP TABLE IF EXISTS `0_debtor_trans_details`;
CREATE TABLE IF NOT EXISTS `0_debtor_trans_details` (
  `id` int(11) NOT NULL auto_increment,
  `debtor_trans_no` int(11) default NULL,
  `debtor_trans_type` int(11) default NULL,
  `stock_id` varchar(20) NOT NULL default '',
  `description` tinytext,
  `unit_price` double NOT NULL default '0',
  `unit_tax` double NOT NULL default '0',
  `quantity` double NOT NULL default '0',
  `discount_percent` double NOT NULL default '0',
  `standard_cost` double NOT NULL default '0',
  `qty_done` double NOT NULL default '0',
  `src_id` int(11) NOT NULL,
  PRIMARY KEY  (`id`),
  KEY `Transaction` (`debtor_trans_type`,`debtor_trans_no`),
  KEY (`src_id`)
) ENGINE=InnoDB  AUTO_INCREMENT=9 ;

--
-- Dumping data for table `0_debtor_trans_details`
--

INSERT INTO `0_debtor_trans_details` VALUES(1, 2, 13, '102', '17inch VGA Monitor', 30.4, 0, 2, 0, 10, 0, 1);
INSERT INTO `0_debtor_trans_details` VALUES(2, 3, 13, '102', '17inch VGA Monitor', 50, 2.5, 1, 0, 10, 1, 2);
INSERT INTO `0_debtor_trans_details` VALUES(3, 17, 10, '102', '17inch VGA Monitor', 50, 2.5, 1, 0, 10, 2, 2);
INSERT INTO `0_debtor_trans_details` VALUES(4, 4, 13, '102', '17inch VGA Monitor', 35.89, 1.79, 1, 0, 10, 1,3);
INSERT INTO `0_debtor_trans_details` VALUES(5, 18, 10, '102', '17inch VGA Monitor', 35.89, 1.79, 1, 0, 10, 1,4);
INSERT INTO `0_debtor_trans_details` VALUES(6, 5, 13, '102', '17inch VGA Monitor', 50, 2.38, 1, 0, 10, 1,5);
INSERT INTO `0_debtor_trans_details` VALUES(7, 19, 10, '102', '17inch VGA Monitor', 50, 2.38, 1, 0, 10, 0,6);
INSERT INTO `0_debtor_trans_details` VALUES(8, 3, 11, '102', '17inch VGA Monitor', 35.89, 1.79, 1, 0, 10, 0,5);

-- --------------------------------------------------------

--
-- Table structure for table `0_dimensions`
--

DROP TABLE IF EXISTS `0_dimensions`;
CREATE TABLE IF NOT EXISTS `0_dimensions` (
  `id` int(11) NOT NULL auto_increment,
  `reference` varchar(60) NOT NULL default '',
  `name` varchar(60) NOT NULL default '',
  `type_` tinyint(1) NOT NULL default '1',
  `closed` tinyint(1) NOT NULL default '0',
  `date_` date NOT NULL default '0000-00-00',
  `due_date` date NOT NULL default '0000-00-00',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `reference` (`reference`),
  KEY `date_` (`date_`),
  KEY `due_date` (`due_date`),
  KEY `type_` (`type_`)
) ENGINE=InnoDB  AUTO_INCREMENT=3 ;

--
-- Dumping data for table `0_dimensions`
--

INSERT INTO `0_dimensions` VALUES(1, '1', 'Support', 1, 0, '2014-06-21', '2020-07-11');
INSERT INTO `0_dimensions` VALUES(2, '2', 'Development', 1, 0, '2014-06-21', '2020-07-11');

-- --------------------------------------------------------

--
-- Table structure for table `0_exchange_rates`
--

DROP TABLE IF EXISTS `0_exchange_rates`;
CREATE TABLE IF NOT EXISTS `0_exchange_rates` (
  `id` int(11) NOT NULL auto_increment,
  `curr_code` char(3) NOT NULL default '',
  `rate_buy` double NOT NULL default '0',
  `rate_sell` double NOT NULL default '0',
  `date_` date NOT NULL default '0000-00-00',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `curr_code` (`curr_code`,`date_`)
) ENGINE=MyISAM  AUTO_INCREMENT=4 ;

--
-- Dumping data for table `0_exchange_rates`
--

INSERT INTO `0_exchange_rates` VALUES(1, 'DKK', 0.18717252868313, 0.18717252868313, '2014-06-21');
INSERT INTO `0_exchange_rates` VALUES(2, 'GBP', 1.6445729799917, 1.6445729799917, '2014-06-21');
INSERT INTO `0_exchange_rates` VALUES(3, 'EUR', 1.3932, 1.3932, '2014-06-21');

-- --------------------------------------------------------

--
-- Table structure for table `0_fiscal_year`
--

DROP TABLE IF EXISTS `0_fiscal_year`;
CREATE TABLE IF NOT EXISTS `0_fiscal_year` (
  `id` int(11) NOT NULL auto_increment,
  `begin` date default '0000-00-00',
  `end` date default '0000-00-00',
  `closed` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `begin` (`begin`),
  UNIQUE KEY `end` (`end`)
) ENGINE=InnoDB  AUTO_INCREMENT=5 ;

--
-- Dumping data for table `0_fiscal_year`
--

INSERT INTO `0_fiscal_year` VALUES(1, '2013-01-01', '2013-12-31', 0);
INSERT INTO `0_fiscal_year` VALUES(2, '2014-01-01', '2014-12-31', 0);
INSERT INTO `0_fiscal_year` VALUES(3, '2015-01-01', '2015-12-31', 0);
INSERT INTO `0_fiscal_year` VALUES(4, '2016-01-01', '2016-12-31', 0);

--
-- Table structure for table `0_gl_trans`
--

DROP TABLE IF EXISTS `0_gl_trans`;
CREATE TABLE IF NOT EXISTS `0_gl_trans` (
  `counter` int(11) NOT NULL auto_increment,
  `type` smallint(6) NOT NULL default '0',
  `type_no` bigint(16) NOT NULL default '1',
  `tran_date` date NOT NULL default '0000-00-00',
  `account` varchar(15) NOT NULL default '',
  `memo_` tinytext NOT NULL,
  `amount` double NOT NULL default '0',
  `dimension_id` int(11) NOT NULL default '0',
  `dimension2_id` int(11) NOT NULL default '0',
  `person_type_id` int(11) default NULL,
  `person_id` tinyblob,
  PRIMARY KEY  (`counter`),
  KEY `Type_and_Number` (`type`,`type_no`),
  KEY `dimension_id` (`dimension_id`),
  KEY `dimension2_id` (`dimension2_id`),
  KEY `tran_date` (`tran_date`),
  KEY `account_and_tran_date` (`account`,`tran_date`)
) ENGINE=InnoDB  AUTO_INCREMENT=84 ;

--
-- Dumping data for table `0_gl_trans`
--

INSERT INTO `0_gl_trans` VALUES(1, 20, 7, '2014-06-21', '2100', '', -3465, 0, 0, 3, '2');
INSERT INTO `0_gl_trans` VALUES(2, 20, 7, '2014-06-21', '1510', '', 1000, 0, 0, 3, '2');
INSERT INTO `0_gl_trans` VALUES(3, 20, 7, '2014-06-21', '1510', '', 1100, 0, 0, 3, '2');
INSERT INTO `0_gl_trans` VALUES(4, 20, 7, '2014-06-21', '1510', '', 1200, 0, 0, 3, '2');
INSERT INTO `0_gl_trans` VALUES(5, 20, 7, '2014-06-21', '2150', '', 165, 0, 0, 3, '2');
INSERT INTO `0_gl_trans` VALUES(6, 22, 4, '2014-06-21', '2100', '', 3465, 0, 0, 3, '2');
INSERT INTO `0_gl_trans` VALUES(7, 22, 4, '2014-06-21', '1060', '', -3465, 0, 0, 3, '2');
INSERT INTO `0_gl_trans` VALUES(8, 26, 1, '2014-06-21', '1510', '', -100, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(9, 26, 1, '2014-06-21', '1510', '', -110, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(10, 26, 1, '2014-06-21', '1510', '', -120, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(11, 26, 1, '2014-06-21', '1060', 'Overhead Cost', -10, 0, 0, 1, '1');
INSERT INTO `0_gl_trans` VALUES(12, 26, 1, '2014-06-21', '1530', 'Overhead Cost', 10, 0, 0, 1, '1');
INSERT INTO `0_gl_trans` VALUES(13, 26, 1, '2014-06-21', '1060', 'Labour Cost', -20, 0, 0, 1, '0');
INSERT INTO `0_gl_trans` VALUES(14, 26, 1, '2014-06-21', '1530', 'Labour Cost', 20, 0, 0, 1, '0');
INSERT INTO `0_gl_trans` VALUES(15, 26, 1, '2014-06-21', '1510', '', 330, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(16, 13, 2, '2014-06-21', '5010', '', 20, 0, 0, 2, '1');
INSERT INTO `0_gl_trans` VALUES(17, 13, 2, '2014-06-21', '1510', '', -20, 0, 0, 2, '1');
INSERT INTO `0_gl_trans` VALUES(18, 13, 3, '2014-06-21', '5010', '', 10, 0, 0, 2, '2');
INSERT INTO `0_gl_trans` VALUES(19, 13, 3, '2014-06-21', '1510', '', -10, 0, 0, 2, '2');
INSERT INTO `0_gl_trans` VALUES(20, 10, 17, '2014-06-21', '4010', '', -50, 0, 0, 2, '2');
INSERT INTO `0_gl_trans` VALUES(21, 10, 17, '2014-06-21', '1200', '', 52.5, 0, 0, 2, '2');
INSERT INTO `0_gl_trans` VALUES(22, 10, 17, '2014-06-21', '2150', '', -2.5, 0, 0, 2, '2');
INSERT INTO `0_gl_trans` VALUES(23, 13, 4, '2014-06-21', '5010', '', 10, 2, 0, 2, '3');
INSERT INTO `0_gl_trans` VALUES(24, 13, 4, '2014-06-21', '1510', '', -10, 0, 0, 2, '3');
INSERT INTO `0_gl_trans` VALUES(25, 10, 18, '2014-06-21', '4010', '', -50, 2, 0, 2, '3');
INSERT INTO `0_gl_trans` VALUES(26, 10, 18, '2014-06-21', '1200', '', 52.5, 0, 0, 2, '3');
INSERT INTO `0_gl_trans` VALUES(27, 10, 18, '2014-06-21', '2150', '', -2.5, 0, 0, 2, '3');
INSERT INTO `0_gl_trans` VALUES(28, 0, 18, '2014-02-20', '1060', '', 1000, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(29, 0, 18, '2014-02-20', '3350', '', -1000, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(30, 0, 19, '2014-02-21', '1060', '', 4000, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(31, 0, 19, '2014-02-21', '3350', '', -4000, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(32, 26, 3, '2014-06-21', '1510', '', -20, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(33, 26, 3, '2014-06-21', '1510', '', -22, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(34, 26, 3, '2014-06-21', '1510', '', -24, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(35, 26, 3, '2014-06-21', '1540', '', 66, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(36, 2, 5, '2014-06-21', '2150', 'Cash Sales', -4.76, 0, 0, 4, '3');
INSERT INTO `0_gl_trans` VALUES(37, 2, 5, '2014-06-21', '4010', 'Cash Sales', -95.24, 0, 0, 4, '3');
INSERT INTO `0_gl_trans` VALUES(38, 2, 5, '2014-06-21', '1060', '', 100, 0, 0, 4, '3');
INSERT INTO `0_gl_trans` VALUES(39, 1, 8, '2014-06-21', '2150', 'Maintenance', 2.38, 0, 0, 4, '1');
INSERT INTO `0_gl_trans` VALUES(40, 1, 8, '2014-06-21', '5765', 'Maintenance', 47.62, 0, 0, 4, '1');
INSERT INTO `0_gl_trans` VALUES(41, 1, 8, '2014-06-21', '1060', '', -50, 0, 0, 4, '1');
INSERT INTO `0_gl_trans` VALUES(42, 20, 8, '2014-06-21', '2100', '', -20, 0, 0, 3, '2');
INSERT INTO `0_gl_trans` VALUES(43, 20, 8, '2014-06-21', '2150', '', 0.95, 0, 0, 3, '2');
INSERT INTO `0_gl_trans` VALUES(44, 20, 8, '2014-06-21', '5780', '', 19.05, 0, 0, 3, '2');
INSERT INTO `0_gl_trans` VALUES(45, 26, 4, '2014-06-21', '1510', '', -40, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(46, 26, 4, '2014-06-21', '1510', '', -44, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(47, 26, 4, '2014-06-21', '1510', '', -48, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(48, 26, 4, '2014-06-21', '1540', '', 132, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(49, 26, 2, '2014-06-21', '1510', '', -20, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(50, 26, 2, '2014-06-21', '1510', '', -22, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(51, 26, 2, '2014-06-21', '1510', '', -24, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(52, 26, 2, '2014-06-21', '1540', '', 66, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(53, 26, 5, '2014-06-21', '1510', '', -50, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(54, 26, 5, '2014-06-21', '1510', '', -55, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(55, 26, 5, '2014-06-21', '1510', '', -60, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(56, 26, 5, '2014-06-21', '1060', 'Overhead Cost', -10, 0, 0, 1, '1');
INSERT INTO `0_gl_trans` VALUES(57, 26, 5, '2014-06-21', '1530', 'Overhead Cost', 10, 0, 0, 1, '1');
INSERT INTO `0_gl_trans` VALUES(58, 26, 5, '2014-06-21', '1060', 'Labour Cost', -20, 0, 0, 1, '0');
INSERT INTO `0_gl_trans` VALUES(59, 26, 5, '2014-06-21', '1530', 'Labour Cost', 20, 0, 0, 1, '0');
INSERT INTO `0_gl_trans` VALUES(60, 26, 5, '2014-06-21', '1540', '', 165, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(61, 26, 6, '2014-06-21', '1510', '', 50, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(62, 26, 6, '2014-06-21', '1510', '', 55, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(63, 26, 6, '2014-06-21', '1510', '', 60, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(64, 26, 6, '2014-06-21', '1540', '', -165, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(65, 26, 7, '2014-06-21', '1510', '', 20, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(66, 26, 7, '2014-06-21', '1510', '', 22, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(67, 26, 7, '2014-06-21', '1510', '', 24, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(68, 26, 7, '2014-06-21', '1060', 'Overhead Cost', -10, 0, 0, 1, '1');
INSERT INTO `0_gl_trans` VALUES(69, 26, 7, '2014-06-21', '1530', 'Overhead Cost', 10, 0, 0, 1, '1');
INSERT INTO `0_gl_trans` VALUES(70, 26, 7, '2014-06-21', '1060', 'Labour Cost', -20, 0, 0, 1, '0');
INSERT INTO `0_gl_trans` VALUES(71, 26, 7, '2014-06-21', '1530', 'Labour Cost', 20, 0, 0, 1, '0');
INSERT INTO `0_gl_trans` VALUES(72, 26, 7, '2014-06-21', '1540', '', -66, 0, 0, NULL, '');
INSERT INTO `0_gl_trans` VALUES(73, 13, 5, '2014-06-21', '5010', '', 10, 0, 0, 2, '2');
INSERT INTO `0_gl_trans` VALUES(74, 13, 5, '2014-06-21', '1510', '', -10, 0, 0, 2, '2');
INSERT INTO `0_gl_trans` VALUES(75, 10, 19, '2014-06-21', '4010', '', -47.62, 0, 0, 2, '2');
INSERT INTO `0_gl_trans` VALUES(76, 10, 19, '2014-06-21', '1200', '', 55, 0, 0, 2, '2');
INSERT INTO `0_gl_trans` VALUES(77, 10, 19, '2014-06-21', '4430', '', -5, 0, 0, 2, '2');
INSERT INTO `0_gl_trans` VALUES(78, 10, 19, '2014-06-21', '2150', '', -2.38, 0, 0, 2, '2');
INSERT INTO `0_gl_trans` VALUES(79, 11, 3, '2014-06-21', '5010', '', -10, 2, 0, 2, '3');
INSERT INTO `0_gl_trans` VALUES(80, 11, 3, '2014-06-21', '1510', '', 10, 0, 0, 2, '3');
INSERT INTO `0_gl_trans` VALUES(81, 11, 3, '2014-06-21', '4010', '', 50, 2, 0, 2, '3');
INSERT INTO `0_gl_trans` VALUES(82, 11, 3, '2014-06-21', '1200', '', -52.5, 0, 0, 2, '3');
INSERT INTO `0_gl_trans` VALUES(83, 11, 3, '2014-06-21', '2150', '', 2.5, 0, 0, 2, '3');

-- --------------------------------------------------------

--
-- Table structure for table `0_grn_batch`
--

DROP TABLE IF EXISTS `0_grn_batch`;
CREATE TABLE IF NOT EXISTS `0_grn_batch` (
  `id` int(11) NOT NULL auto_increment,
  `supplier_id` int(11) NOT NULL default '0',
  `purch_order_no` int(11) default NULL,
  `reference` varchar(60) NOT NULL default '',
  `delivery_date` date NOT NULL default '0000-00-00',
  `loc_code` varchar(5) default NULL,
  PRIMARY KEY  (`id`),
  KEY `delivery_date` (`delivery_date`),
  KEY `purch_order_no` (`purch_order_no`)
) ENGINE=InnoDB  AUTO_INCREMENT=2 ;

--
-- Dumping data for table `0_grn_batch`
--

INSERT INTO `0_grn_batch` VALUES(1, 2, 1, '1', '2014-06-21', 'DEF');

-- --------------------------------------------------------

--
-- Table structure for table `0_grn_items`
--

DROP TABLE IF EXISTS `0_grn_items`;
CREATE TABLE IF NOT EXISTS `0_grn_items` (
  `id` int(11) NOT NULL auto_increment,
  `grn_batch_id` int(11) default NULL,
  `po_detail_item` int(11) NOT NULL default '0',
  `item_code` varchar(20) NOT NULL default '',
  `description` tinytext,
  `qty_recd` double NOT NULL default '0',
  `quantity_inv` double NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `grn_batch_id` (`grn_batch_id`)
) ENGINE=InnoDB  AUTO_INCREMENT=4 ;

--
-- Dumping data for table `0_grn_items`
--

INSERT INTO `0_grn_items` VALUES(1, 1, 1, '102', '17inch VGA Monitor', 100, 100);
INSERT INTO `0_grn_items` VALUES(2, 1, 2, '103', '32MB VGA Card', 100, 100);
INSERT INTO `0_grn_items` VALUES(3, 1, 3, '104', '52x CD Drive', 100, 100);

-- --------------------------------------------------------

--
-- Table structure for table `0_groups`
--

DROP TABLE IF EXISTS `0_groups`;
CREATE TABLE IF NOT EXISTS `0_groups` (
  `id` smallint(6) unsigned NOT NULL auto_increment,
  `description` varchar(60) NOT NULL default '',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `description` (`description`)
) ENGINE=MyISAM  AUTO_INCREMENT=4 ;

--
-- Dumping data for table `0_groups`
--

INSERT INTO `0_groups` VALUES(1, 'Small', 0);
INSERT INTO `0_groups` VALUES(2, 'Medium', 0);
INSERT INTO `0_groups` VALUES(3, 'Large', 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_item_codes`
--

DROP TABLE IF EXISTS `0_item_codes`;
CREATE TABLE IF NOT EXISTS `0_item_codes` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `item_code` varchar(20) NOT NULL,
  `stock_id` varchar(20) NOT NULL,
  `description` varchar(200) NOT NULL default '',
  `category_id` smallint(6) unsigned NOT NULL,
  `quantity` double NOT NULL default '1',
  `is_foreign` tinyint(1) NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `stock_id` (`stock_id`,`item_code`),
  KEY `item_code` (`item_code`)
) ENGINE=MyISAM  AUTO_INCREMENT=6 ;

--
-- Dumping data for table `0_item_codes`
--

INSERT INTO `0_item_codes` VALUES(1, '102', '102', '17inch VGA Monitor', 1, 1, 0, 0);
INSERT INTO `0_item_codes` VALUES(2, '103', '103', '32MB VGA Card', 1, 1, 0, 0);
INSERT INTO `0_item_codes` VALUES(3, '104', '104', '52x CD Drive', 1, 1, 0, 0);
INSERT INTO `0_item_codes` VALUES(4, '3400', '3400', 'P4 Business System', 3, 1, 0, 0);
INSERT INTO `0_item_codes` VALUES(5, '201', '201', 'Assembly Labour', 4, 1, 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_item_tax_types`
--

DROP TABLE IF EXISTS `0_item_tax_types`;
CREATE TABLE IF NOT EXISTS `0_item_tax_types` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(60) NOT NULL default '',
  `exempt` tinyint(1) NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB  AUTO_INCREMENT=2 ;

--
-- Dumping data for table `0_item_tax_types`
--

INSERT INTO `0_item_tax_types` VALUES(1, 'Regular', 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_item_tax_type_exemptions`
--

DROP TABLE IF EXISTS `0_item_tax_type_exemptions`;
CREATE TABLE IF NOT EXISTS `0_item_tax_type_exemptions` (
  `item_tax_type_id` int(11) NOT NULL default '0',
  `tax_type_id` int(11) NOT NULL default '0',
  PRIMARY KEY  (`item_tax_type_id`,`tax_type_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `0_item_tax_type_exemptions`
--


-- --------------------------------------------------------

--
-- Table structure for table `0_item_units`
--

DROP TABLE IF EXISTS `0_item_units`;
CREATE TABLE IF NOT EXISTS `0_item_units` (
  `abbr` varchar(20) NOT NULL,
  `name` varchar(40) NOT NULL,
  `decimals` tinyint(2) NOT NULL,
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`abbr`),
  UNIQUE KEY `name` (`name`)
) ENGINE=MyISAM;

--
-- Dumping data for table `0_item_units`
--

INSERT INTO `0_item_units` VALUES('each', 'Each', 0, 0);
INSERT INTO `0_item_units` VALUES('hr', 'Hours', 1, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_locations`
--

DROP TABLE IF EXISTS `0_locations`;
CREATE TABLE IF NOT EXISTS `0_locations` (
  `loc_code` varchar(5) NOT NULL default '',
  `location_name` varchar(60) NOT NULL default '',
  `delivery_address` tinytext NOT NULL,
  `phone` varchar(30) NOT NULL default '',
  `phone2` varchar(30) NOT NULL default '',
  `fax` varchar(30) NOT NULL default '',
  `email` varchar(100) NOT NULL default '',
  `contact` varchar(30) NOT NULL default '',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`loc_code`)
) ENGINE=MyISAM;

--
-- Dumping data for table `0_locations`
--

INSERT INTO `0_locations` VALUES('DEF', 'Default', 'Delivery 1\nDelivery 2\nDelivery 3', '', '', '', '', '', 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_loc_stock`
--

DROP TABLE IF EXISTS `0_loc_stock`;
CREATE TABLE IF NOT EXISTS `0_loc_stock` (
  `loc_code` char(5) NOT NULL default '',
  `stock_id` char(20) NOT NULL default '',
  `reorder_level` bigint(20) NOT NULL default '0',
  PRIMARY KEY  (`loc_code`,`stock_id`),
  KEY `stock_id` (`stock_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `0_loc_stock`
--

INSERT INTO `0_loc_stock` VALUES('DEF', '102', 0);
INSERT INTO `0_loc_stock` VALUES('DEF', '103', 0);
INSERT INTO `0_loc_stock` VALUES('DEF', '104', 0);
INSERT INTO `0_loc_stock` VALUES('DEF', '201', 0);
INSERT INTO `0_loc_stock` VALUES('DEF', '3400', 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_movement_types`
--

DROP TABLE IF EXISTS `0_movement_types`;
CREATE TABLE IF NOT EXISTS `0_movement_types` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(60) NOT NULL default '',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=MyISAM  AUTO_INCREMENT=2 ;

--
-- Dumping data for table `0_movement_types`
--

INSERT INTO `0_movement_types` VALUES(1, 'Adjustment', 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_payment_terms`
--

DROP TABLE IF EXISTS `0_payment_terms`;
CREATE TABLE IF NOT EXISTS `0_payment_terms` (
  `terms_indicator` int(11) NOT NULL auto_increment,
  `terms` char(80) NOT NULL default '',
  `days_before_due` smallint(6) NOT NULL default '0',
  `day_in_following_month` smallint(6) NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`terms_indicator`),
  UNIQUE KEY `terms` (`terms`)
) ENGINE=MyISAM  AUTO_INCREMENT=5 ;

--
-- Dumping data for table `0_payment_terms`
--

INSERT INTO `0_payment_terms` VALUES(1, 'Due 15th Of the Following Month', 0, 17, 0);
INSERT INTO `0_payment_terms` VALUES(2, 'Due By End Of The Following Month', 0, 30, 0);
INSERT INTO `0_payment_terms` VALUES(3, 'Payment due within 10 days', 10, 0, 0);
INSERT INTO `0_payment_terms` VALUES(4, 'Cash Only', 0, 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_prices`
--

DROP TABLE IF EXISTS `0_prices`;
CREATE TABLE IF NOT EXISTS `0_prices` (
  `id` int(11) NOT NULL auto_increment,
  `stock_id` varchar(20) NOT NULL default '',
  `sales_type_id` int(11) NOT NULL default '0',
  `curr_abrev` char(3) NOT NULL default '',
  `price` double NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `price` (`stock_id`,`sales_type_id`,`curr_abrev`)
) ENGINE=MyISAM  AUTO_INCREMENT=5 ;

--
-- Dumping data for table `0_prices`
--

INSERT INTO `0_prices` VALUES(1, '3400', 1, 'USD', 100);
INSERT INTO `0_prices` VALUES(2, '102', 1, 'USD', 50);
INSERT INTO `0_prices` VALUES(3, '103', 1, 'USD', 55);
INSERT INTO `0_prices` VALUES(4, '104', 1, 'USD', 60);

-- --------------------------------------------------------

--
-- Table structure for table `0_printers`
--

DROP TABLE IF EXISTS `0_printers`;
CREATE TABLE IF NOT EXISTS `0_printers` (
  `id` tinyint(3) unsigned NOT NULL auto_increment,
  `name` varchar(20) NOT NULL,
  `description` varchar(60) NOT NULL,
  `queue` varchar(20) NOT NULL,
  `host` varchar(40) NOT NULL,
  `port` smallint(11) unsigned NOT NULL,
  `timeout` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=MyISAM  AUTO_INCREMENT=4 ;

--
-- Dumping data for table `0_printers`
--

INSERT INTO `0_printers` VALUES(1, 'QL500', 'Label printer', 'QL500', 'server', 127, 20);
INSERT INTO `0_printers` VALUES(2, 'Samsung', 'Main network printer', 'scx4521F', 'server', 515, 5);
INSERT INTO `0_printers` VALUES(3, 'Local', 'Local print server at user IP', 'lp', '', 515, 10);

-- --------------------------------------------------------

--
-- Table structure for table `0_print_profiles`
--

DROP TABLE IF EXISTS `0_print_profiles`;
CREATE TABLE IF NOT EXISTS `0_print_profiles` (
  `id` smallint(6) unsigned NOT NULL auto_increment,
  `profile` varchar(30) NOT NULL,
  `report` varchar(5) default NULL,
  `printer` tinyint(3) unsigned default NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `profile` (`profile`,`report`)
) ENGINE=MyISAM  AUTO_INCREMENT=10 ;

--
-- Dumping data for table `0_print_profiles`
--

INSERT INTO `0_print_profiles` VALUES(1, 'Out of office', '', 0);
INSERT INTO `0_print_profiles` VALUES(2, 'Sales Department', '', 0);
INSERT INTO `0_print_profiles` VALUES(3, 'Central', '', 2);
INSERT INTO `0_print_profiles` VALUES(4, 'Sales Department', '104', 2);
INSERT INTO `0_print_profiles` VALUES(5, 'Sales Department', '105', 2);
INSERT INTO `0_print_profiles` VALUES(6, 'Sales Department', '107', 2);
INSERT INTO `0_print_profiles` VALUES(7, 'Sales Department', '109', 2);
INSERT INTO `0_print_profiles` VALUES(8, 'Sales Department', '110', 2);
INSERT INTO `0_print_profiles` VALUES(9, 'Sales Department', '201', 2);

-- --------------------------------------------------------

--
-- Table structure for table `0_purch_data`
--

DROP TABLE IF EXISTS `0_purch_data`;
CREATE TABLE IF NOT EXISTS `0_purch_data` (
  `supplier_id` int(11) NOT NULL default '0',
  `stock_id` char(20) NOT NULL default '',
  `price` double NOT NULL default '0',
  `suppliers_uom` char(50) NOT NULL default '',
  `conversion_factor` double NOT NULL default '1',
  `supplier_description` char(50) NOT NULL default '',
  PRIMARY KEY  (`supplier_id`,`stock_id`)
) ENGINE=MyISAM;

--
-- Dumping data for table `0_purch_data`
--

INSERT INTO `0_purch_data` VALUES(2, '102', 10, '', 1, '17inch VGA Monitor');
INSERT INTO `0_purch_data` VALUES(2, '103', 11, '', 1, '32MB VGA Card');
INSERT INTO `0_purch_data` VALUES(2, '104', 12, '', 1, '52x CD Drive');

-- --------------------------------------------------------

--
-- Table structure for table `0_purch_orders`
--

DROP TABLE IF EXISTS `0_purch_orders`;
CREATE TABLE IF NOT EXISTS `0_purch_orders` (
  `order_no` int(11) NOT NULL auto_increment,
  `supplier_id` int(11) NOT NULL default '0',
  `comments` tinytext,
  `ord_date` date NOT NULL default '0000-00-00',
  `reference` tinytext NOT NULL,
  `requisition_no` tinytext,
  `into_stock_location` varchar(5) NOT NULL default '',
  `delivery_address` tinytext NOT NULL,
  `total` double NOT NULL default '0',
  `tax_included` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`order_no`),
  KEY `ord_date` (`ord_date`)
) ENGINE=InnoDB  AUTO_INCREMENT=3 ;

--
-- Dumping data for table `0_purch_orders`
--

INSERT INTO `0_purch_orders` VALUES(1, 2, '', '2014-06-01', '1', '', 'DEF', 'Delivery 1\nDelivery 2\nDelivery 3', 0, 0);
INSERT INTO `0_purch_orders` VALUES(2, 3, '', '2014-06-21', '2', '', 'DEF', 'Delivery 1\nDelivery 2\nDelivery 3', 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_purch_order_details`
--

DROP TABLE IF EXISTS `0_purch_order_details`;
CREATE TABLE IF NOT EXISTS `0_purch_order_details` (
  `po_detail_item` int(11) NOT NULL auto_increment,
  `order_no` int(11) NOT NULL default '0',
  `item_code` varchar(20) NOT NULL default '',
  `description` tinytext,
  `delivery_date` date NOT NULL default '0000-00-00',
  `qty_invoiced` double NOT NULL default '0',
  `unit_price` double NOT NULL default '0',
  `act_price` double NOT NULL default '0',
  `std_cost_unit` double NOT NULL default '0',
  `quantity_ordered` double NOT NULL default '0',
  `quantity_received` double NOT NULL default '0',
  PRIMARY KEY  (`po_detail_item`),
  KEY `order` (`order_no`,`po_detail_item`)
) ENGINE=InnoDB  AUTO_INCREMENT=6 ;

--
-- Dumping data for table `0_purch_order_details`
--

INSERT INTO `0_purch_order_details` VALUES(1, 1, '102', '17inch VGA Monitor', '2014-07-01', 100, 10, 10, 10, 100, 100);
INSERT INTO `0_purch_order_details` VALUES(2, 1, '103', '32MB VGA Card', '2014-07-01', 100, 11, 11, 11, 100, 100);
INSERT INTO `0_purch_order_details` VALUES(3, 1, '104', '52x CD Drive', '2014-07-01', 100, 12, 12, 12, 100, 100);
INSERT INTO `0_purch_order_details` VALUES(4, 2, '102', '17inch VGA Monitor', '2014-07-01', 0, 5, 0, 0, 1, 0);
INSERT INTO `0_purch_order_details` VALUES(5, 2, '103', '32MB VGA Card', '2014-07-01', 0, 5, 0, 0, 1, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_quick_entries`
--

DROP TABLE IF EXISTS `0_quick_entries`;
CREATE TABLE IF NOT EXISTS `0_quick_entries` (
  `id` smallint(6) unsigned NOT NULL auto_increment,
  `type` tinyint(1) NOT NULL default '0',
  `description` varchar(60) NOT NULL,
  `base_amount` double NOT NULL default '0',
  `base_desc` varchar(60) default NULL,
  `bal_type` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `description` (`description`)
) ENGINE=MyISAM  AUTO_INCREMENT=4 ;

--
-- Dumping data for table `0_quick_entries`
--

INSERT INTO `0_quick_entries` VALUES(1, 1, 'Maintenance', 0, 'Amount', 0);
INSERT INTO `0_quick_entries` VALUES(2, 4, 'Phone', 0, 'Amount', 0);
INSERT INTO `0_quick_entries` VALUES(3, 2, 'Cash Sales', 0, 'Amount', 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_quick_entry_lines`
--

DROP TABLE IF EXISTS `0_quick_entry_lines`;
CREATE TABLE IF NOT EXISTS `0_quick_entry_lines` (
  `id` smallint(6) unsigned NOT NULL auto_increment,
  `qid` smallint(6) unsigned NOT NULL,
  `amount` double default '0',
  `action` varchar(2) NOT NULL,
  `dest_id` varchar(15) NOT NULL default '',
  `dimension_id` smallint(6) unsigned default NULL,
  `dimension2_id` smallint(6) unsigned default NULL,
  PRIMARY KEY  (`id`),
  KEY `qid` (`qid`)
) ENGINE=MyISAM  AUTO_INCREMENT=8 ;

--
-- Dumping data for table `0_quick_entry_lines`
--

INSERT INTO `0_quick_entry_lines` VALUES(1, 1, 0, 't-', '1', 0, 0);
INSERT INTO `0_quick_entry_lines` VALUES(2, 2, 0, 't-', '1', 0, 0);
INSERT INTO `0_quick_entry_lines` VALUES(3, 3, 0, 't-', '1', 0, 0);
INSERT INTO `0_quick_entry_lines` VALUES(4, 3, 0, '=', '4010', 0, 0);
INSERT INTO `0_quick_entry_lines` VALUES(5, 1, 0, '=', '5765', 0, 0);
INSERT INTO `0_quick_entry_lines` VALUES(6, 2, 0, '=', '5780', 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_recurrent_invoices`
--

DROP TABLE IF EXISTS `0_recurrent_invoices`;
CREATE TABLE IF NOT EXISTS `0_recurrent_invoices` (
  `id` smallint(6) unsigned NOT NULL auto_increment,
  `description` varchar(60) NOT NULL default '',
  `order_no` int(11) unsigned NOT NULL,
  `debtor_no` int(11) unsigned default NULL,
  `group_no` smallint(6) unsigned default NULL,
  `days` int(11) NOT NULL default '0',
  `monthly` int(11) NOT NULL default '0',
  `begin` date NOT NULL default '0000-00-00',
  `end` date NOT NULL default '0000-00-00',
  `last_sent` date NOT NULL default '0000-00-00',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `description` (`description`)
) ENGINE=InnoDB AUTO_INCREMENT=1 ;

--
-- Dumping data for table `0_recurrent_invoices`
--


-- --------------------------------------------------------

--
-- Table structure for table `0_refs`
--

DROP TABLE IF EXISTS `0_refs`;
CREATE TABLE IF NOT EXISTS `0_refs` (
  `id` int(11) NOT NULL default '0',
  `type` int(11) NOT NULL default '0',
  `reference` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`id`,`type`),
  KEY `Type_and_Reference` (`type`,`reference`)
) ENGINE=InnoDB;

--
-- Dumping data for table `0_refs`
--

INSERT INTO `0_refs` VALUES(18, 0, '1');
INSERT INTO `0_refs` VALUES(19, 0, '2');

-- --------------------------------------------------------

--
-- Table structure for table `0_salesman`
--

DROP TABLE IF EXISTS `0_salesman`;
CREATE TABLE IF NOT EXISTS `0_salesman` (
  `salesman_code` int(11) NOT NULL auto_increment,
  `salesman_name` char(60) NOT NULL default '',
  `salesman_phone` char(30) NOT NULL default '',
  `salesman_fax` char(30) NOT NULL default '',
  `salesman_email` varchar(100) NOT NULL default '',
  `provision` double NOT NULL default '0',
  `break_pt` double NOT NULL default '0',
  `provision2` double NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`salesman_code`),
  UNIQUE KEY `salesman_name` (`salesman_name`)
) ENGINE=MyISAM  AUTO_INCREMENT=2 ;

--
-- Dumping data for table `0_salesman`
--

INSERT INTO `0_salesman` VALUES(1, 'Sales Person', '', '', '', 5, 1000, 4, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_sales_orders`
--

DROP TABLE IF EXISTS `0_sales_orders`;
CREATE TABLE IF NOT EXISTS `0_sales_orders` (
  `order_no` int(11) NOT NULL,
  `trans_type` smallint(6) NOT NULL default '30',
  `version` tinyint(1) unsigned NOT NULL default '0',
  `type` tinyint(1) NOT NULL default '0',
  `debtor_no` int(11) NOT NULL default '0',
  `branch_code` int(11) NOT NULL default '0',
  `reference` varchar(100) NOT NULL default '',
  `customer_ref` tinytext NOT NULL,
  `comments` tinytext,
  `ord_date` date NOT NULL default '0000-00-00',
  `order_type` int(11) NOT NULL default '0',
  `ship_via` int(11) NOT NULL default '0',
  `delivery_address` tinytext NOT NULL,
  `contact_phone` varchar(30) default NULL,
  `contact_email` varchar(100) default NULL,
  `deliver_to` tinytext NOT NULL,
  `freight_cost` double NOT NULL default '0',
  `from_stk_loc` varchar(5) NOT NULL default '',
  `delivery_date` date NOT NULL default '0000-00-00',
  `payment_terms` int(11) default NULL,
  `total` double NOT NULL default '0',
  PRIMARY KEY  (`trans_type`,`order_no`)
) ENGINE=InnoDB;

--
-- Dumping data for table `0_sales_orders`
--

INSERT INTO `0_sales_orders` VALUES(1, 30, 1, 0, 1, 1, '1', '', '', '2014-06-21', 2, 1, 'Address 1\nAddress 2\nAddress 3', '', '', 'Beefeater Ltd.', 10, 'DEF', '2014-06-22', 3, 0);
INSERT INTO `0_sales_orders` VALUES(2, 30, 1, 0, 2, 2, '2', '', '', '2014-06-21', 1, 1, 'Address 1\nAddress 2\nAddress 3', '', '', 'Ghostbusters Corp.', 0, 'DEF', '2014-06-22', 4, 0);
INSERT INTO `0_sales_orders` VALUES(3, 30, 1, 0, 3, 3, '3', '', '', '2014-06-21', 2, 1, 'Address 1\nAddress 2\nAddress 3', '', '', 'Brezan', 0, 'DEF', '2014-07-01', 3, 0);
INSERT INTO `0_sales_orders` VALUES(4, 30, 0, 0, 1, 1, '4', '', '', '2014-06-21', 2, 1, 'Address 1\nAddress 2\nAddress 3', '', '', 'Beefeater Ltd.', 0, 'DEF', '2014-06-22', 3, 0);
INSERT INTO `0_sales_orders` VALUES(5, 30, 1, 0, 2, 2, '5', '', '', '2014-06-21', 1, 1, 'Address 1\nAddress 2\nAddress 3', '', '', 'Ghostbusters Corp.', 5, 'DEF', '2014-06-22', 4, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_sales_order_details`
--

DROP TABLE IF EXISTS `0_sales_order_details`;
CREATE TABLE IF NOT EXISTS `0_sales_order_details` (
  `id` int(11) NOT NULL auto_increment,
  `order_no` int(11) NOT NULL default '0',
  `trans_type` smallint(6) NOT NULL default '30',
  `stk_code` varchar(20) NOT NULL default '',
  `description` tinytext,
  `qty_sent` double NOT NULL default '0',
  `unit_price` double NOT NULL default '0',
  `quantity` double NOT NULL default '0',
  `discount_percent` double NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `sorder` (`trans_type`,`order_no`)
) ENGINE=InnoDB  AUTO_INCREMENT=6 ;

--
-- Dumping data for table `0_sales_order_details`
--

INSERT INTO `0_sales_order_details` VALUES(1, 1, 30, '102', '17inch VGA Monitor', 2, 30.4, 2, 0);
INSERT INTO `0_sales_order_details` VALUES(2, 2, 30, '102', '17inch VGA Monitor', 1, 50, 1, 0);
INSERT INTO `0_sales_order_details` VALUES(3, 3, 30, '102', '17inch VGA Monitor', 1, 35.89, 1, 0);
INSERT INTO `0_sales_order_details` VALUES(4, 4, 30, '102', '17inch VGA Monitor', 0, 21.28, 2, 0);
INSERT INTO `0_sales_order_details` VALUES(5, 5, 30, '102', '17inch VGA Monitor', 1, 50, 1, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_sales_pos`
--

DROP TABLE IF EXISTS `0_sales_pos`;
CREATE TABLE IF NOT EXISTS `0_sales_pos` (
  `id` smallint(6) unsigned NOT NULL auto_increment,
  `pos_name` varchar(30) NOT NULL,
  `cash_sale` tinyint(1) NOT NULL,
  `credit_sale` tinyint(1) NOT NULL,
  `pos_location` varchar(5) NOT NULL,
  `pos_account` smallint(6) unsigned NOT NULL,
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `pos_name` (`pos_name`)
) ENGINE=MyISAM  AUTO_INCREMENT=2 ;

--
-- Dumping data for table `0_sales_pos`
--

INSERT INTO `0_sales_pos` VALUES(1, 'Default', 1, 1, 'DEF', 2, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_sales_types`
--

DROP TABLE IF EXISTS `0_sales_types`;
CREATE TABLE IF NOT EXISTS `0_sales_types` (
  `id` int(11) NOT NULL auto_increment,
  `sales_type` char(50) NOT NULL default '',
  `tax_included` int(1) NOT NULL default '0',
  `factor` double NOT NULL default '1',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `sales_type` (`sales_type`)
) ENGINE=MyISAM  AUTO_INCREMENT=3 ;

--
-- Dumping data for table `0_sales_types`
--

INSERT INTO `0_sales_types` VALUES(1, 'Retail', 1, 1, 0);
INSERT INTO `0_sales_types` VALUES(2, 'Wholesale', 0, 0.7, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_security_roles`
--

DROP TABLE IF EXISTS `0_security_roles`;
CREATE TABLE IF NOT EXISTS `0_security_roles` (
  `id` int(11) NOT NULL auto_increment,
  `role` varchar(30) NOT NULL,
  `description` varchar(50) default NULL,
  `sections` text,
  `areas` text,
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `role` (`role`)
) ENGINE=MyISAM  AUTO_INCREMENT=11 ;

--
-- Dumping data for table `0_security_roles`
--

INSERT INTO `0_security_roles` VALUES(1, 'Inquiries', 'Inquiries', '768;2816;3072;3328;5632;5888;8192;8448;10752;11008;13312;15872;16128', '257;258;259;260;513;514;515;516;517;518;519;520;521;522;523;524;525;773;774;2822;3073;3075;3076;3077;3329;3330;3331;3332;3333;3334;3335;5377;5633;5640;5889;5890;5891;7937;7938;7939;7940;8193;8194;8450;8451;10497;10753;11009;11010;11012;13313;13315;15617;15618;15619;15620;15621;15622;15623;15624;15625;15626;15873;15882;16129;16130;16131;16132', 0);
INSERT INTO `0_security_roles` VALUES(2, 'System Administrator', 'System Administrator', '256;512;768;2816;3072;3328;5376;5632;5888;7936;8192;8448;10496;10752;11008;13056;13312;15616;15872;16128', '257;258;259;260;513;514;515;516;517;518;519;520;521;522;523;524;525;526;769;770;771;772;773;774;2817;2818;2819;2820;2821;2822;2823;3073;3074;3082;3075;3076;3077;3078;3079;3080;3081;3329;3330;3331;3332;3333;3334;3335;5377;5633;5634;5635;5636;5637;5641;5638;5639;5640;5889;5890;5891;7937;7938;7939;7940;8193;8194;8195;8196;8197;8449;8450;8451;10497;10753;10754;10755;10756;10757;11009;11010;11011;11012;13057;13313;13314;13315;15617;15618;15619;15620;15621;15622;15623;15624;15628;15625;15626;15627;15873;15874;15875;15876;15877;15878;15879;15880;15883;15881;15882;16129;16130;16131;16132', 0);
INSERT INTO `0_security_roles` VALUES(3, 'Salesman', 'Salesman', '768;3072;5632;8192;15872', '773;774;3073;3075;3081;5633;8194;15873', 0);
INSERT INTO `0_security_roles` VALUES(4, 'Stock Manager', 'Stock Manager', '2816;3072;3328;5632;5888;8192;8448;10752;11008;13312;15872;16128', '2818;2822;3073;3076;3077;3329;3330;3330;3330;3331;3331;3332;3333;3334;3335;5633;5640;5889;5890;5891;8193;8194;8450;8451;10753;11009;11010;11012;13313;13315;15882;16129;16130;16131;16132', 0);
INSERT INTO `0_security_roles` VALUES(5, 'Production Manager', 'Production Manager', '512;2816;3072;3328;5632;5888;8192;8448;10752;11008;13312;15616;15872;16128', '521;523;524;2818;2819;2820;2821;2822;2823;3073;3074;3076;3077;3078;3079;3080;3081;3329;3330;3330;3330;3331;3331;3332;3333;3334;3335;5633;5640;5640;5889;5890;5891;8193;8194;8196;8197;8450;8451;10753;10755;11009;11010;11012;13313;13315;15617;15619;15620;15621;15624;15624;15876;15877;15880;15882;16129;16130;16131;16132', 0);
INSERT INTO `0_security_roles` VALUES(6, 'Purchase Officer', 'Purchase Officer', '512;2816;3072;3328;5376;5632;5888;8192;8448;10752;11008;13312;15616;15872;16128', '521;523;524;2818;2819;2820;2821;2822;2823;3073;3074;3076;3077;3078;3079;3080;3081;3329;3330;3330;3330;3331;3331;3332;3333;3334;3335;5377;5633;5635;5640;5640;5889;5890;5891;8193;8194;8196;8197;8449;8450;8451;10753;10755;11009;11010;11012;13313;13315;15617;15619;15620;15621;15624;15624;15876;15877;15880;15882;16129;16130;16131;16132', 0);
INSERT INTO `0_security_roles` VALUES(7, 'AR Officer', 'AR Officer', '512;768;2816;3072;3328;5632;5888;8192;8448;10752;11008;13312;15616;15872;16128', '521;523;524;771;773;774;2818;2819;2820;2821;2822;2823;3073;3073;3074;3075;3076;3077;3078;3079;3080;3081;3081;3329;3330;3330;3330;3331;3331;3332;3333;3334;3335;5633;5633;5634;5637;5638;5639;5640;5640;5889;5890;5891;8193;8194;8194;8196;8197;8450;8451;10753;10755;11009;11010;11012;13313;13315;15617;15619;15620;15621;15624;15624;15873;15876;15877;15878;15880;15882;16129;16130;16131;16132', 0);
INSERT INTO `0_security_roles` VALUES(8, 'AP Officer', 'AP Officer', '512;2816;3072;3328;5376;5632;5888;8192;8448;10752;11008;13312;15616;15872;16128', '257;258;259;260;521;523;524;769;770;771;772;773;774;2818;2819;2820;2821;2822;2823;3073;3074;3082;3076;3077;3078;3079;3080;3081;3329;3330;3331;3332;3333;3334;3335;5377;5633;5635;5640;5889;5890;5891;7937;7938;7939;7940;8193;8194;8196;8197;8449;8450;8451;10497;10753;10755;11009;11010;11012;13057;13313;13315;15617;15619;15620;15621;15624;15876;15877;15880;15882;16129;16130;16131;16132', 0);
INSERT INTO `0_security_roles` VALUES(9, 'Accountant', 'New Accountant', '512;768;2816;3072;3328;5376;5632;5888;8192;8448;10752;11008;13312;15616;15872;16128', '257;258;259;260;521;523;524;771;772;773;774;2818;2819;2820;2821;2822;2823;3073;3074;3075;3076;3077;3078;3079;3080;3081;3329;3330;3331;3332;3333;3334;3335;5377;5633;5634;5635;5637;5638;5639;5640;5889;5890;5891;7937;7938;7939;7940;8193;8194;8196;8197;8449;8450;8451;10497;10753;10755;11009;11010;11012;13313;13315;15617;15618;15619;15620;15621;15624;15873;15876;15877;15878;15880;15882;16129;16130;16131;16132', 0);
INSERT INTO `0_security_roles` VALUES(10, 'Sub Admin', 'Sub Admin', '512;768;2816;3072;3328;5376;5632;5888;8192;8448;10752;11008;13312;15616;15872;16128', '257;258;259;260;521;523;524;771;772;773;774;2818;2819;2820;2821;2822;2823;3073;3074;3082;3075;3076;3077;3078;3079;3080;3081;3329;3330;3331;3332;3333;3334;3335;5377;5633;5634;5635;5637;5638;5639;5640;5889;5890;5891;7937;7938;7939;7940;8193;8194;8196;8197;8449;8450;8451;10497;10753;10755;11009;11010;11012;13057;13313;13315;15617;15619;15620;15621;15624;15873;15874;15876;15877;15878;15879;15880;15882;16129;16130;16131;16132', 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_shippers`
--

DROP TABLE IF EXISTS `0_shippers`;
CREATE TABLE IF NOT EXISTS `0_shippers` (
  `shipper_id` int(11) NOT NULL auto_increment,
  `shipper_name` varchar(60) NOT NULL default '',
  `phone` varchar(30) NOT NULL default '',
  `phone2` varchar(30) NOT NULL default '',
  `contact` tinytext NOT NULL,
  `address` tinytext NOT NULL,
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`shipper_id`),
  UNIQUE KEY `name` (`shipper_name`)
) ENGINE=MyISAM  AUTO_INCREMENT=2 ;

--
-- Dumping data for table `0_shippers`
--

INSERT INTO `0_shippers` VALUES(1, 'Default', '', '', '', '', 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_sql_trail`
--

DROP TABLE IF EXISTS `0_sql_trail`;
CREATE TABLE IF NOT EXISTS `0_sql_trail` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `sql` text NOT NULL,
  `result` tinyint(1) NOT NULL,
  `msg` varchar(255) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=1 ;

--
-- Dumping data for table `0_sql_trail`
--


-- --------------------------------------------------------

--
-- Table structure for table `0_stock_category`
--

DROP TABLE IF EXISTS `0_stock_category`;
CREATE TABLE IF NOT EXISTS `0_stock_category` (
  `category_id` int(11) NOT NULL auto_increment,
  `description` varchar(60) NOT NULL default '',
  `dflt_tax_type` int(11) NOT NULL default '1',
  `dflt_units` varchar(20) NOT NULL default 'each',
  `dflt_mb_flag` char(1) NOT NULL default 'B',
  `dflt_sales_act` varchar(15) NOT NULL default '',
  `dflt_cogs_act` varchar(15) NOT NULL default '',
  `dflt_inventory_act` varchar(15) NOT NULL default '',
  `dflt_adjustment_act` varchar(15) NOT NULL default '',
  `dflt_assembly_act` varchar(15) NOT NULL default '',
  `dflt_dim1` int(11) default NULL,
  `dflt_dim2` int(11) default NULL,
  `inactive` tinyint(1) NOT NULL default '0',
  `dflt_no_sale` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`category_id`),
  UNIQUE KEY `description` (`description`)
) ENGINE=MyISAM  AUTO_INCREMENT=5 ;

--
-- Dumping data for table `0_stock_category`
--

INSERT INTO `0_stock_category` VALUES(1, 'Components', 1, 'each', 'B', '4010', '5010', '1510', '5040', '1530', 0, 0, 0, 0);
INSERT INTO `0_stock_category` VALUES(2, 'Charges', 1, 'each', 'D', '4010', '5010', '1510', '5040', '1530', 0, 0, 0, 0);
INSERT INTO `0_stock_category` VALUES(3, 'Systems', 1, 'each', 'M', '4010', '5010', '1510', '5040', '1530', 0, 0, 0, 0);
INSERT INTO `0_stock_category` VALUES(4, 'Services', 1, 'hr', 'D', '4010', '5010', '1510', '5040', '1530', 0, 0, 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_stock_master`
--

DROP TABLE IF EXISTS `0_stock_master`;
CREATE TABLE IF NOT EXISTS `0_stock_master` (
  `stock_id` varchar(20) NOT NULL default '',
  `category_id` int(11) NOT NULL default '0',
  `tax_type_id` int(11) NOT NULL default '0',
  `description` varchar(200) NOT NULL default '',
  `long_description` tinytext NOT NULL,
  `units` varchar(20) NOT NULL default 'each',
  `mb_flag` char(1) NOT NULL default 'B',
  `sales_account` varchar(15) NOT NULL default '',
  `cogs_account` varchar(15) NOT NULL default '',
  `inventory_account` varchar(15) NOT NULL default '',
  `adjustment_account` varchar(15) NOT NULL default '',
  `assembly_account` varchar(15) NOT NULL default '',
  `dimension_id` int(11) default NULL,
  `dimension2_id` int(11) default NULL,
  `actual_cost` double NOT NULL default '0',
  `last_cost` double NOT NULL default '0',
  `material_cost` double NOT NULL default '0',
  `labour_cost` double NOT NULL default '0',
  `overhead_cost` double NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  `no_sale` tinyint(1) NOT NULL default '0',
  `editable` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`stock_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `0_stock_master`
--

INSERT INTO `0_stock_master` VALUES('102', 1, 1, '17inch VGA Monitor', '', 'each', 'B', '4010', '5010', '1510', '5040', '1530', 0, 0, 0, 0, 10, 0, 0, 0, 0, 0);
INSERT INTO `0_stock_master` VALUES('103', 1, 1, '32MB VGA Card', '', 'each', 'B', '4010', '5010', '1510', '5040', '1530', 0, 0, 0, 0, 11, 0, 0, 0, 0, 0);
INSERT INTO `0_stock_master` VALUES('104', 1, 1, '52x CD Drive', '', 'each', 'B', '4010', '5010', '1510', '5040', '1530', 0, 0, 0, 0, 12, 0, 0, 0, 0, 0);
INSERT INTO `0_stock_master` VALUES('201', 4, 1, 'Assembly Labour', '', 'hr', 'D', '4010', '5010', '5010', '5040', '1530', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `0_stock_master` VALUES('3400', 3, 1, 'P4 Business System', '', 'each', 'M', '4010', '5010', '1540', '5040', '1530', 0, 0, 0, 0, 33, 3.9999999999999, 2, 0, 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_stock_moves`
--

DROP TABLE IF EXISTS `0_stock_moves`;
CREATE TABLE IF NOT EXISTS `0_stock_moves` (
  `trans_id` int(11) NOT NULL auto_increment,
  `trans_no` int(11) NOT NULL default '0',
  `stock_id` char(20) NOT NULL default '',
  `type` smallint(6) NOT NULL default '0',
  `loc_code` char(5) NOT NULL default '',
  `tran_date` date NOT NULL default '0000-00-00',
  `person_id` int(11) default NULL,
  `price` double NOT NULL default '0',
  `reference` char(40) NOT NULL default '',
  `qty` double NOT NULL default '1',
  `discount_percent` double NOT NULL default '0',
  `standard_cost` double NOT NULL default '0',
  `visible` tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (`trans_id`),
  KEY `type` (`type`,`trans_no`),
  KEY `Move` (`stock_id`,`loc_code`,`tran_date`)
) ENGINE=InnoDB  AUTO_INCREMENT=37 ;

--
-- Dumping data for table `0_stock_moves`
--

INSERT INTO `0_stock_moves` VALUES(1, 1, '102', 25, 'DEF', '2014-06-21', 2, 10, '', 100, 0, 10, 1);
INSERT INTO `0_stock_moves` VALUES(2, 1, '103', 25, 'DEF', '2014-06-21', 2, 11, '', 100, 0, 11, 1);
INSERT INTO `0_stock_moves` VALUES(3, 1, '104', 25, 'DEF', '2014-06-21', 2, 12, '', 100, 0, 12, 1);
INSERT INTO `0_stock_moves` VALUES(4, 1, '102', 26, 'DEF', '2014-06-21', 0, 0, '1', -10, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(5, 1, '103', 26, 'DEF', '2014-06-21', 0, 0, '1', -10, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(6, 1, '104', 26, 'DEF', '2014-06-21', 0, 0, '1', -10, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(7, 1, '3400', 26, 'DEF', '2014-06-21', 0, 0, '1', 10, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(8, 2, '102', 13, 'DEF', '2014-06-21', 0, 30.4, '1', -2, 0, 10, 1);
INSERT INTO `0_stock_moves` VALUES(9, 3, '102', 13, 'DEF', '2014-06-21', 0, 50, 'auto', -1, 0, 10, 1);
INSERT INTO `0_stock_moves` VALUES(10, 4, '102', 13, 'DEF', '2014-06-21', 0, 35.89, 'auto', -1, 0, 10, 1);
INSERT INTO `0_stock_moves` VALUES(11, 3, '102', 26, 'DEF', '2014-06-21', 0, 0, '3', -2, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(12, 3, '103', 26, 'DEF', '2014-06-21', 0, 0, '3', -2, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(13, 3, '104', 26, 'DEF', '2014-06-21', 0, 0, '3', -2, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(14, 3, '3400', 26, 'DEF', '2014-06-21', 0, 0, '3', 2, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(15, 4, '102', 26, 'DEF', '2014-06-21', 0, 0, '4', -4, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(16, 4, '103', 26, 'DEF', '2014-06-21', 0, 0, '4', -4, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(17, 4, '104', 26, 'DEF', '2014-06-21', 0, 0, '4', -4, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(18, 4, '3400', 26, 'DEF', '2014-06-21', 0, 0, '4', 4, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(19, 2, '102', 26, 'DEF', '2014-06-21', 0, 0, '', -2, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(20, 2, '103', 26, 'DEF', '2014-06-21', 0, 0, '', -2, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(21, 2, '104', 26, 'DEF', '2014-06-21', 0, 0, '', -2, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(22, 1, '3400', 29, 'DEF', '2014-06-21', 0, 0, '', 2, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(23, 5, '102', 26, 'DEF', '2014-06-21', 0, 0, '5', -5, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(24, 5, '103', 26, 'DEF', '2014-06-21', 0, 0, '5', -5, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(25, 5, '104', 26, 'DEF', '2014-06-21', 0, 0, '5', -5, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(26, 5, '3400', 26, 'DEF', '2014-06-21', 0, 0, '5', 5, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(27, 6, '102', 26, 'DEF', '2014-06-21', 0, 0, '6', 5, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(28, 6, '103', 26, 'DEF', '2014-06-21', 0, 0, '6', 5, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(29, 6, '104', 26, 'DEF', '2014-06-21', 0, 0, '6', 5, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(30, 6, '3400', 26, 'DEF', '2014-06-21', 0, 0, '6', -5, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(31, 7, '102', 26, 'DEF', '2014-06-21', 0, 0, '7', 2, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(32, 7, '103', 26, 'DEF', '2014-06-21', 0, 0, '7', 2, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(33, 7, '104', 26, 'DEF', '2014-06-21', 0, 0, '7', 2, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(34, 7, '3400', 26, 'DEF', '2014-06-21', 0, 0, '7', -2, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES(35, 5, '102', 13, 'DEF', '2014-06-21', 0, 50, 'auto', -1, 0, 10, 1);
INSERT INTO `0_stock_moves` VALUES(36, 3, '102', 11, 'DEF', '2014-06-21', 0, 37.68, 'Return Ex Inv: 18', 1, 0, 10, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_suppliers`
--

DROP TABLE IF EXISTS `0_suppliers`;
CREATE TABLE IF NOT EXISTS `0_suppliers` (
  `supplier_id` int(11) NOT NULL auto_increment,
  `supp_name` varchar(60) NOT NULL default '',
  `supp_ref` varchar(30) NOT NULL default '',
  `address` tinytext NOT NULL,
  `supp_address` tinytext NOT NULL,
  `gst_no` varchar(25) NOT NULL default '',
  `contact` varchar(60) NOT NULL default '',
  `supp_account_no` varchar(40) NOT NULL default '',
  `website` varchar(100) NOT NULL default '',
  `bank_account` varchar(60) NOT NULL default '',
  `curr_code` char(3) default NULL,
  `payment_terms` int(11) default NULL,
  `tax_included` tinyint(1) NOT NULL default '0',
  `dimension_id` int(11) default '0',
  `dimension2_id` int(11) default '0',
  `tax_group_id` int(11) default NULL,
  `credit_limit` double NOT NULL default '0',
  `purchase_account` varchar(15) NOT NULL default '',
  `payable_account` varchar(15) NOT NULL default '',
  `payment_discount_account` varchar(15) NOT NULL default '',
  `notes` tinytext NOT NULL,
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`supplier_id`),
  KEY `supp_ref` (`supp_ref`)
) ENGINE=MyISAM  AUTO_INCREMENT=4 ;

--
-- Dumping data for table `0_suppliers`
--

INSERT INTO `0_suppliers` VALUES(1, 'Junk Beer ApS', 'Junk Beer', 'Mailing 1\nMailing 2\nMailing 3', 'Address 1\nAddress 2\nAddress 3', '123456', 'Contact', '111', '', '', 'DKK', 3, 0, 1, 0, 2, 1000, '', '2100', '5060', 'A supplier with junk beers.', 0);
INSERT INTO `0_suppliers` VALUES(2, 'Lucky Luke Inc.', 'Lucky Luke', 'Mailing 1\nMailing 2\nMailing 3', 'Address 1\nAddress 2\nAddress 3', '654321', 'Luke', '333', '', '', 'USD', 3, 0, 0, 0, 1, 500, '', '2100', '5060', '', 0);
INSERT INTO `0_suppliers` VALUES(3, 'Money Makers Ltd.', 'Money Makers', 'Mailing 1\nMailing 2\nMailing 3', 'Address 1\nAddress 2\nAddress 3', '987654', 'Makers', '222', '', '', 'GBP', 3, 0, 0, 0, 2, 300, '', '2100', '5060', '', 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_supp_allocations`
--

DROP TABLE IF EXISTS `0_supp_allocations`;
CREATE TABLE IF NOT EXISTS `0_supp_allocations` (
  `id` int(11) NOT NULL auto_increment,
  `amt` double unsigned default NULL,
  `date_alloc` date NOT NULL default '0000-00-00',
  `trans_no_from` int(11) default NULL,
  `trans_type_from` int(11) default NULL,
  `trans_no_to` int(11) default NULL,
  `trans_type_to` int(11) default NULL,
  PRIMARY KEY  (`id`),
  KEY `From` (`trans_type_from`,`trans_no_from`),
  KEY `To` (`trans_type_to`,`trans_no_to`)
) ENGINE=InnoDB  AUTO_INCREMENT=2 ;

--
-- Dumping data for table `0_supp_allocations`
--

INSERT INTO `0_supp_allocations` VALUES(1, 3465, '2014-06-21', 4, 22, 7, 20);

-- --------------------------------------------------------

--
-- Table structure for table `0_supp_invoice_items`
--

DROP TABLE IF EXISTS `0_supp_invoice_items`;
CREATE TABLE IF NOT EXISTS `0_supp_invoice_items` (
  `id` int(11) NOT NULL auto_increment,
  `supp_trans_no` int(11) default NULL,
  `supp_trans_type` int(11) default NULL,
  `gl_code` varchar(15) NOT NULL default '',
  `grn_item_id` int(11) default NULL,
  `po_detail_item_id` int(11) default NULL,
  `stock_id` varchar(20) NOT NULL default '',
  `description` tinytext,
  `quantity` double NOT NULL default '0',
  `unit_price` double NOT NULL default '0',
  `unit_tax` double NOT NULL default '0',
  `memo_` tinytext,
  PRIMARY KEY  (`id`),
  KEY `Transaction` (`supp_trans_type`,`supp_trans_no`,`stock_id`)
) ENGINE=InnoDB  AUTO_INCREMENT=6 ;

--
-- Dumping data for table `0_supp_invoice_items`
--

INSERT INTO `0_supp_invoice_items` VALUES(1, 7, 20, '0', 1, 1, '102', '17inch VGA Monitor', 100, 10, 0.5, '');
INSERT INTO `0_supp_invoice_items` VALUES(2, 7, 20, '0', 2, 2, '103', '32MB VGA Card', 100, 11, 0.55, '');
INSERT INTO `0_supp_invoice_items` VALUES(3, 7, 20, '0', 3, 3, '104', '52x CD Drive', 100, 12, 0.6, '');
INSERT INTO `0_supp_invoice_items` VALUES(4, 8, 20, '2150', 0, 0, '', '', 0, 0.95, 0, 'Phone');
INSERT INTO `0_supp_invoice_items` VALUES(5, 8, 20, '5780', 0, 0, '', '', 0, 19.05, 0, 'Phone');

-- --------------------------------------------------------

--
-- Table structure for table `0_supp_trans`
--

DROP TABLE IF EXISTS `0_supp_trans`;
CREATE TABLE IF NOT EXISTS `0_supp_trans` (
  `trans_no` int(11) unsigned NOT NULL default '0',
  `type` smallint(6) unsigned NOT NULL default '0',
  `supplier_id` int(11) unsigned default NULL,
  `reference` tinytext NOT NULL,
  `supp_reference` varchar(60) NOT NULL default '',
  `tran_date` date NOT NULL default '0000-00-00',
  `due_date` date NOT NULL default '0000-00-00',
  `ov_amount` double NOT NULL default '0',
  `ov_discount` double NOT NULL default '0',
  `ov_gst` double NOT NULL default '0',
  `rate` double NOT NULL default '1',
  `alloc` double NOT NULL default '0',
  `tax_included` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`type`,`trans_no`),
  KEY `supplier_id` (`supplier_id`),
  KEY `SupplierID_2` (`supplier_id`,`supp_reference`),
  KEY `type` (`type`),
  KEY `tran_date` (`tran_date`)
) ENGINE=InnoDB;

--
-- Dumping data for table `0_supp_trans`
--

INSERT INTO `0_supp_trans` VALUES(7, 20, 2, '1', '5t', '2014-06-21', '2014-07-01', 3300, 0, 165, 1, 3465, 0);
INSERT INTO `0_supp_trans` VALUES(8, 20, 2, '2', 'cc', '2014-06-21', '2014-07-01', 20, 0, 0, 1, 0, 0);
INSERT INTO `0_supp_trans` VALUES(4, 22, 2, '1', '', '2014-06-21', '2014-06-21', -3465, 0, 0, 1, 3465, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_sys_prefs`
--

DROP TABLE IF EXISTS `0_sys_prefs`;
CREATE TABLE IF NOT EXISTS `0_sys_prefs` (
  `name` varchar(35) NOT NULL default '',
  `category` varchar(30) default NULL,
  `type` varchar(20) NOT NULL default '',
  `length` smallint(6) default NULL,
  `value` tinytext,
  PRIMARY KEY  (`name`),
  KEY `category` (`category`)
) ENGINE=MyISAM;

--
-- Dumping data for table `0_sys_prefs`
--

INSERT INTO `0_sys_prefs` VALUES('coy_name', 'setup.company', 'varchar', 60, 'Training Co.');
INSERT INTO `0_sys_prefs` VALUES('gst_no', 'setup.company', 'varchar', 25, '9876543');
INSERT INTO `0_sys_prefs` VALUES('coy_no', 'setup.company', 'varchar', 25, '123456789');
INSERT INTO `0_sys_prefs` VALUES('tax_prd', 'setup.company', 'int', 11, '1');
INSERT INTO `0_sys_prefs` VALUES('tax_last', 'setup.company', 'int', 11, '1');
INSERT INTO `0_sys_prefs` VALUES('postal_address', 'setup.company', 'tinytext', 0, 'Address 1\nAddress 2\nAddress 3');
INSERT INTO `0_sys_prefs` VALUES('phone', 'setup.company', 'varchar', 30, '(222) 111.222.333');
INSERT INTO `0_sys_prefs` VALUES('fax', 'setup.company', 'varchar', 30, '');
INSERT INTO `0_sys_prefs` VALUES('email', 'setup.company', 'varchar', 100, 'delta@delta.com');
INSERT INTO `0_sys_prefs` VALUES('coy_logo', 'setup.company', 'varchar', 100, 'logo_frontaccounting.jpg');
INSERT INTO `0_sys_prefs` VALUES('domicile', 'setup.company', 'varchar', 55, '');
INSERT INTO `0_sys_prefs` VALUES('curr_default', 'setup.company', 'char', 3, 'USD');
INSERT INTO `0_sys_prefs` VALUES('use_dimension', 'setup.company', 'tinyint', 1, '1');
INSERT INTO `0_sys_prefs` VALUES('f_year', 'setup.company', 'int', 11, '2');
INSERT INTO `0_sys_prefs` VALUES('no_item_list', 'setup.company', 'tinyint', 1, '0');
INSERT INTO `0_sys_prefs` VALUES('no_customer_list', 'setup.company', 'tinyint', 1, '0');
INSERT INTO `0_sys_prefs` VALUES('no_supplier_list', 'setup.company', 'tinyint', 1, '0');
INSERT INTO `0_sys_prefs` VALUES('base_sales', 'setup.company', 'int', 11, '1');
INSERT INTO `0_sys_prefs` VALUES('time_zone', 'setup.company', 'tinyint', 1, '0');
INSERT INTO `0_sys_prefs` VALUES('add_pct', 'setup.company', 'int', 5, '-1');
INSERT INTO `0_sys_prefs` VALUES('round_to', 'setup.company', 'int', 5, '1');
INSERT INTO `0_sys_prefs` VALUES('login_tout', 'setup.company', 'smallint', 6, '600');
INSERT INTO `0_sys_prefs` VALUES('past_due_days', 'glsetup.general', 'int', 11, '30');
INSERT INTO `0_sys_prefs` VALUES('profit_loss_year_act', 'glsetup.general', 'varchar', 15, '9990');
INSERT INTO `0_sys_prefs` VALUES('retained_earnings_act', 'glsetup.general', 'varchar', 15, '3590');
INSERT INTO `0_sys_prefs` VALUES('bank_charge_act', 'glsetup.general', 'varchar', 15, '5690');
INSERT INTO `0_sys_prefs` VALUES('exchange_diff_act', 'glsetup.general', 'varchar', 15, '4450');
INSERT INTO `0_sys_prefs` VALUES('default_credit_limit', 'glsetup.customer', 'int', 11, '1000');
INSERT INTO `0_sys_prefs` VALUES('accumulate_shipping', 'glsetup.customer', 'tinyint', 1, '0');
INSERT INTO `0_sys_prefs` VALUES('legal_text', 'glsetup.customer', 'tinytext', 0, '');
INSERT INTO `0_sys_prefs` VALUES('freight_act', 'glsetup.customer', 'varchar', 15, '4430');
INSERT INTO `0_sys_prefs` VALUES('debtors_act', 'glsetup.sales', 'varchar', 15, '1200');
INSERT INTO `0_sys_prefs` VALUES('default_sales_act', 'glsetup.sales', 'varchar', 15, '4010');
INSERT INTO `0_sys_prefs` VALUES('default_sales_discount_act', 'glsetup.sales', 'varchar', 15, '4510');
INSERT INTO `0_sys_prefs` VALUES('default_prompt_payment_act', 'glsetup.sales', 'varchar', 15, '4500');
INSERT INTO `0_sys_prefs` VALUES('default_delivery_required', 'glsetup.sales', 'smallint', 6, '1');
INSERT INTO `0_sys_prefs` VALUES('default_dim_required', 'glsetup.dims', 'int', 11, '20');
INSERT INTO `0_sys_prefs` VALUES('pyt_discount_act', 'glsetup.purchase', 'varchar', 15, '5060');
INSERT INTO `0_sys_prefs` VALUES('creditors_act', 'glsetup.purchase', 'varchar', 15, '2100');
INSERT INTO `0_sys_prefs` VALUES('po_over_receive', 'glsetup.purchase', 'int', 11, '10');
INSERT INTO `0_sys_prefs` VALUES('po_over_charge', 'glsetup.purchase', 'int', 11, '10');
INSERT INTO `0_sys_prefs` VALUES('allow_negative_stock', 'glsetup.inventory', 'tinyint', 1, '0');
INSERT INTO `0_sys_prefs` VALUES('default_inventory_act', 'glsetup.items', 'varchar', 15, '1510');
INSERT INTO `0_sys_prefs` VALUES('default_cogs_act', 'glsetup.items', 'varchar', 15, '5010');
INSERT INTO `0_sys_prefs` VALUES('default_adj_act', 'glsetup.items', 'varchar', 15, '5040');
INSERT INTO `0_sys_prefs` VALUES('default_inv_sales_act', 'glsetup.items', 'varchar', 15, '4010');
INSERT INTO `0_sys_prefs` VALUES('default_assembly_act', 'glsetup.items', 'varchar', 15, '1530');
INSERT INTO `0_sys_prefs` VALUES('default_workorder_required', 'glsetup.manuf', 'int', 11, '20');
INSERT INTO `0_sys_prefs` VALUES('version_id', 'system', 'varchar', 11, '2.3rc');
INSERT INTO `0_sys_prefs` VALUES('auto_curr_reval', 'setup.company', 'smallint', 6, '1');
INSERT INTO `0_sys_prefs` VALUES('grn_clearing_act', 'glsetup.purchase', 'varchar', 15, '1550');
INSERT INTO `0_sys_prefs` VALUES('bcc_email', 'setup.company', 'varchar', 100, '');

-- --------------------------------------------------------

--
-- Table structure for table `0_sys_types`
--

DROP TABLE IF EXISTS `0_sys_types`;
CREATE TABLE IF NOT EXISTS `0_sys_types` (
  `type_id` smallint(6) NOT NULL default '0',
  `type_no` int(11) NOT NULL default '1',
  `next_reference` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`type_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `0_sys_types`
--

INSERT INTO `0_sys_types` VALUES(0, 19, '3');
INSERT INTO `0_sys_types` VALUES(1, 8, '2');
INSERT INTO `0_sys_types` VALUES(2, 5, '2');
INSERT INTO `0_sys_types` VALUES(4, 3, '1');
INSERT INTO `0_sys_types` VALUES(10, 19, '4');
INSERT INTO `0_sys_types` VALUES(11, 3, '2');
INSERT INTO `0_sys_types` VALUES(12, 6, '1');
INSERT INTO `0_sys_types` VALUES(13, 5, '2');
INSERT INTO `0_sys_types` VALUES(16, 2, '1');
INSERT INTO `0_sys_types` VALUES(17, 2, '1');
INSERT INTO `0_sys_types` VALUES(18, 1, '3');
INSERT INTO `0_sys_types` VALUES(20, 8, '3');
INSERT INTO `0_sys_types` VALUES(21, 1, '1');
INSERT INTO `0_sys_types` VALUES(22, 4, '2');
INSERT INTO `0_sys_types` VALUES(25, 1, '2');
INSERT INTO `0_sys_types` VALUES(26, 1, '8');
INSERT INTO `0_sys_types` VALUES(28, 1, '1');
INSERT INTO `0_sys_types` VALUES(29, 1, '2');
INSERT INTO `0_sys_types` VALUES(30, 5, '6');
INSERT INTO `0_sys_types` VALUES(32, 0, '1');
INSERT INTO `0_sys_types` VALUES(35, 1, '1');
INSERT INTO `0_sys_types` VALUES(40, 1, '3');

-- --------------------------------------------------------

--
-- Table structure for table `0_tags`
--

DROP TABLE IF EXISTS `0_tags`;
CREATE TABLE IF NOT EXISTS `0_tags` (
  `id` int(11) NOT NULL auto_increment,
  `type` smallint(6) NOT NULL,
  `name` varchar(30) NOT NULL,
  `description` varchar(60) default NULL,
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `type` (`type`,`name`)
) ENGINE=MyISAM AUTO_INCREMENT=1 ;

--
-- Dumping data for table `0_tags`
--


-- --------------------------------------------------------

--
-- Table structure for table `0_tag_associations`
--

DROP TABLE IF EXISTS `0_tag_associations`;
CREATE TABLE IF NOT EXISTS `0_tag_associations` (
  `record_id` varchar(15) NOT NULL,
  `tag_id` int(11) NOT NULL,
  UNIQUE KEY `record_id` (`record_id`,`tag_id`)
) ENGINE=MyISAM;

--
-- Dumping data for table `0_tag_associations`
--


-- --------------------------------------------------------

--
-- Table structure for table `0_tax_groups`
--

DROP TABLE IF EXISTS `0_tax_groups`;
CREATE TABLE IF NOT EXISTS `0_tax_groups` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(60) NOT NULL default '',
  `tax_shipping` tinyint(1) NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB  AUTO_INCREMENT=3 ;

--
-- Dumping data for table `0_tax_groups`
--

INSERT INTO `0_tax_groups` VALUES(1, 'Tax', 0, 0);
INSERT INTO `0_tax_groups` VALUES(2, 'Tax Exempt', 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_tax_group_items`
--

DROP TABLE IF EXISTS `0_tax_group_items`;
CREATE TABLE IF NOT EXISTS `0_tax_group_items` (
  `tax_group_id` int(11) NOT NULL default '0',
  `tax_type_id` int(11) NOT NULL default '0',
  `rate` double NOT NULL default '0',
  PRIMARY KEY  (`tax_group_id`,`tax_type_id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `0_tax_group_items`
--

INSERT INTO `0_tax_group_items` VALUES(1, 1, 5);

-- --------------------------------------------------------

--
-- Table structure for table `0_tax_types`
--

DROP TABLE IF EXISTS `0_tax_types`;
CREATE TABLE IF NOT EXISTS `0_tax_types` (
  `id` int(11) NOT NULL auto_increment,
  `rate` double NOT NULL default '0',
  `sales_gl_code` varchar(15) NOT NULL default '',
  `purchasing_gl_code` varchar(15) NOT NULL default '',
  `name` varchar(60) NOT NULL default '',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB  AUTO_INCREMENT=2 ;

--
-- Dumping data for table `0_tax_types`
--

INSERT INTO `0_tax_types` VALUES(1, 5, '2150', '2150', 'Tax', 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_trans_tax_details`
--

DROP TABLE IF EXISTS `0_trans_tax_details`;
CREATE TABLE IF NOT EXISTS `0_trans_tax_details` (
  `id` int(11) NOT NULL auto_increment,
  `trans_type` smallint(6) default NULL,
  `trans_no` int(11) default NULL,
  `tran_date` date NOT NULL,
  `tax_type_id` int(11) NOT NULL default '0',
  `rate` double NOT NULL default '0',
  `ex_rate` double NOT NULL default '1',
  `included_in_price` tinyint(1) NOT NULL default '0',
  `net_amount` double NOT NULL default '0',
  `amount` double NOT NULL default '0',
  `memo` tinytext,
  PRIMARY KEY  (`id`),
  KEY `Type_and_Number` (`trans_type`,`trans_no`),
  KEY `tran_date` (`tran_date`)
) ENGINE=InnoDB  AUTO_INCREMENT=12 ;

--
-- Dumping data for table `0_trans_tax_details`
--

INSERT INTO `0_trans_tax_details` VALUES(1, 20, 7, '2014-06-21', 1, 5, 1, 0, 3300, 165, '5t');
INSERT INTO `0_trans_tax_details` VALUES(2, 13, 3, '2014-06-21', 1, 5, 1, 0, 50, 2.5, 'auto');
INSERT INTO `0_trans_tax_details` VALUES(3, 10, 17, '2014-06-21', 1, 5, 1, 0, 50, 2.5, '1');
INSERT INTO `0_trans_tax_details` VALUES(4, 13, 4, '2014-06-21', 1, 5, 1.3932, 0, 35.89, 1.7945, 'auto');
INSERT INTO `0_trans_tax_details` VALUES(5, 10, 18, '2014-06-21', 1, 5, 1.3932, 0, 35.89, 1.7945, '2');
INSERT INTO `0_trans_tax_details` VALUES(6, 2, 5, '2014-06-21', 1, 5, 1, 0, 95.2, 4.76, '');
INSERT INTO `0_trans_tax_details` VALUES(7, 1, 8, '2014-06-21', 1, 5, 1, 0, -47.6, -2.38, '');
INSERT INTO `0_trans_tax_details` VALUES(8, 20, 8, '2014-06-21', 1, 5, 1, 0, -19, -0.95, 'cc');
INSERT INTO `0_trans_tax_details` VALUES(9, 13, 5, '2014-06-21', 1, 5, 1, 1, 47.619047619048, 2.3809523809524, 'auto');
INSERT INTO `0_trans_tax_details` VALUES(10, 10, 19, '2014-06-21', 1, 5, 1, 1, 47.619047619048, 2.3809523809524, '3');
INSERT INTO `0_trans_tax_details` VALUES(11, 11, 3, '2014-06-21', 1, 5, 1.3932, 0, 35.89, 1.7945, '1');

-- --------------------------------------------------------

--
-- Table structure for table `0_useronline`
--

DROP TABLE IF EXISTS `0_useronline`;
CREATE TABLE IF NOT EXISTS `0_useronline` (
  `id` int(11) NOT NULL auto_increment,
  `timestamp` int(15) NOT NULL default '0',
  `ip` varchar(40) NOT NULL default '',
  `file` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`id`),
  KEY `timestamp` (`timestamp`),
  KEY `ip` (`ip`)
) ENGINE=MyISAM AUTO_INCREMENT=1 ;

--
-- Dumping data for table `0_useronline`
--


-- --------------------------------------------------------

--
-- Table structure for table `0_users`
--

DROP TABLE IF EXISTS `0_users`;
CREATE TABLE IF NOT EXISTS `0_users` (
  `id` smallint(6) NOT NULL auto_increment,
  `user_id` varchar(60) NOT NULL default '',
  `password` varchar(100) NOT NULL default '',
  `real_name` varchar(100) NOT NULL default '',
  `role_id` int(11) NOT NULL default '1',
  `phone` varchar(30) NOT NULL default '',
  `email` varchar(100) default NULL,
  `language` varchar(20) default NULL,
  `date_format` tinyint(1) NOT NULL default '0',
  `date_sep` tinyint(1) NOT NULL default '0',
  `tho_sep` tinyint(1) NOT NULL default '0',
  `dec_sep` tinyint(1) NOT NULL default '0',
  `theme` varchar(20) NOT NULL default 'default',
  `page_size` varchar(20) NOT NULL default 'A4',
  `prices_dec` smallint(6) NOT NULL default '2',
  `qty_dec` smallint(6) NOT NULL default '2',
  `rates_dec` smallint(6) NOT NULL default '4',
  `percent_dec` smallint(6) NOT NULL default '1',
  `show_gl` tinyint(1) NOT NULL default '1',
  `show_codes` tinyint(1) NOT NULL default '0',
  `show_hints` tinyint(1) NOT NULL default '0',
  `last_visit_date` datetime default NULL,
  `query_size` tinyint(1) unsigned NOT NULL default '10',
  `graphic_links` tinyint(1) default '1',
  `pos` smallint(6) default '1',
  `print_profile` varchar(30) NOT NULL default '',
  `rep_popup` tinyint(1) default '1',
  `sticky_doc_date` tinyint(1) default '0',
  `startup_tab` varchar(20) NOT NULL default '',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `user_id` (`user_id`)
) ENGINE=MyISAM  AUTO_INCREMENT=3 ;

--
-- Dumping data for table `0_users`
--

INSERT INTO `0_users` VALUES(1, 'admin', '5f4dcc3b5aa765d61d8327deb882cf99', 'Administrator', 2, '', 'adm@adm.com', 'en_US', 0, 0, 0, 0, 'default', 'Letter', 2, 2, 4, 1, 1, 0, 0, '2014-05-11 23:27:46', 10, 1, 1, '1', 1, 0, 'orders', 0);
INSERT INTO `0_users` VALUES(2, 'demouser', '5f4dcc3b5aa765d61d8327deb882cf99', 'Demo User', 9, '999-999-999', 'demo@demo.nu', 'en_US', 0, 0, 0, 0, 'default', 'Letter', 2, 2, 3, 1, 1, 0, 0, '2014-02-06 19:02:35', 10, 1, 1, '1', 1, 0, 'orders', 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_voided`
--

DROP TABLE IF EXISTS `0_voided`;
CREATE TABLE IF NOT EXISTS `0_voided` (
  `type` int(11) NOT NULL default '0',
  `id` int(11) NOT NULL default '0',
  `date_` date NOT NULL default '0000-00-00',
  `memo_` tinytext NOT NULL,
  UNIQUE KEY `id` (`type`,`id`)
) ENGINE=InnoDB;

--
-- Dumping data for table `0_voided`
--


-- --------------------------------------------------------

--
-- Table structure for table `0_workcentres`
--

DROP TABLE IF EXISTS `0_workcentres`;
CREATE TABLE IF NOT EXISTS `0_workcentres` (
  `id` int(11) NOT NULL auto_increment,
  `name` char(40) NOT NULL default '',
  `description` char(50) NOT NULL default '',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=MyISAM  AUTO_INCREMENT=2 ;

--
-- Dumping data for table `0_workcentres`
--

INSERT INTO `0_workcentres` VALUES(1, 'Workshop', 'Workshop in Alabama', 0);

-- --------------------------------------------------------

--
-- Table structure for table `0_workorders`
--

DROP TABLE IF EXISTS `0_workorders`;
CREATE TABLE IF NOT EXISTS `0_workorders` (
  `id` int(11) NOT NULL auto_increment,
  `wo_ref` varchar(60) NOT NULL default '',
  `loc_code` varchar(5) NOT NULL default '',
  `units_reqd` double NOT NULL default '1',
  `stock_id` varchar(20) NOT NULL default '',
  `date_` date NOT NULL default '0000-00-00',
  `type` tinyint(4) NOT NULL default '0',
  `required_by` date NOT NULL default '0000-00-00',
  `released_date` date NOT NULL default '0000-00-00',
  `units_issued` double NOT NULL default '0',
  `closed` tinyint(1) NOT NULL default '0',
  `released` tinyint(1) NOT NULL default '0',
  `additional_costs` double NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `wo_ref` (`wo_ref`)
) ENGINE=InnoDB  AUTO_INCREMENT=8 ;

--
-- Dumping data for table `0_workorders`
--

INSERT INTO `0_workorders` VALUES(1, '1', 'DEF', 10, '3400', '2014-06-21', 0, '2014-06-21', '2014-06-21', 10, 1, 1, 10);
INSERT INTO `0_workorders` VALUES(2, '2', 'DEF', 3, '3400', '2014-06-21', 2, '2014-07-11', '2014-06-21', 2, 0, 1, 0);
INSERT INTO `0_workorders` VALUES(3, '3', 'DEF', 2, '3400', '2014-06-21', 0, '2014-06-21', '2014-06-21', 2, 1, 1, 0);
INSERT INTO `0_workorders` VALUES(4, '4', 'DEF', 4, '3400', '2014-06-21', 0, '2014-06-21', '2014-06-21', 4, 1, 1, 0);
INSERT INTO `0_workorders` VALUES(5, '5', 'DEF', 5, '3400', '2014-06-21', 0, '2014-06-21', '2014-06-21', 5, 1, 1, 10);
INSERT INTO `0_workorders` VALUES(6, '6', 'DEF', -5, '3400', '2014-06-21', 1, '2014-06-21', '2014-06-21', -5, 1, 1, 0);
INSERT INTO `0_workorders` VALUES(7, '7', 'DEF', -2, '3400', '2014-06-21', 1, '2014-06-21', '2014-06-21', -2, 1, 1, 10);

-- --------------------------------------------------------

--
-- Table structure for table `0_wo_issues`
--

DROP TABLE IF EXISTS `0_wo_issues`;
CREATE TABLE IF NOT EXISTS `0_wo_issues` (
  `issue_no` int(11) NOT NULL auto_increment,
  `workorder_id` int(11) NOT NULL default '0',
  `reference` varchar(100) default NULL,
  `issue_date` date default NULL,
  `loc_code` varchar(5) default NULL,
  `workcentre_id` int(11) default NULL,
  PRIMARY KEY  (`issue_no`),
  KEY `workorder_id` (`workorder_id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 ;

--
-- Dumping data for table `0_wo_issues`
--


-- --------------------------------------------------------

--
-- Table structure for table `0_wo_issue_items`
--

DROP TABLE IF EXISTS `0_wo_issue_items`;
CREATE TABLE IF NOT EXISTS `0_wo_issue_items` (
  `id` int(11) NOT NULL auto_increment,
  `stock_id` varchar(40) default NULL,
  `issue_id` int(11) default NULL,
  `qty_issued` double default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 ;

--
-- Dumping data for table `0_wo_issue_items`
--


-- --------------------------------------------------------

--
-- Table structure for table `0_wo_manufacture`
--

DROP TABLE IF EXISTS `0_wo_manufacture`;
CREATE TABLE IF NOT EXISTS `0_wo_manufacture` (
  `id` int(11) NOT NULL auto_increment,
  `reference` varchar(100) default NULL,
  `workorder_id` int(11) NOT NULL default '0',
  `quantity` double NOT NULL default '0',
  `date_` date NOT NULL default '0000-00-00',
  PRIMARY KEY  (`id`),
  KEY `workorder_id` (`workorder_id`)
) ENGINE=InnoDB  AUTO_INCREMENT=2 ;

--
-- Dumping data for table `0_wo_manufacture`
--

INSERT INTO `0_wo_manufacture` VALUES(1, '1', 2, 2, '2014-06-21');

-- --------------------------------------------------------

--
-- Table structure for table `0_wo_requirements`
--

DROP TABLE IF EXISTS `0_wo_requirements`;
CREATE TABLE IF NOT EXISTS `0_wo_requirements` (
  `id` int(11) NOT NULL auto_increment,
  `workorder_id` int(11) NOT NULL default '0',
  `stock_id` char(20) NOT NULL default '',
  `workcentre` int(11) NOT NULL default '0',
  `units_req` double NOT NULL default '1',
  `std_cost` double NOT NULL default '0',
  `loc_code` char(5) NOT NULL default '',
  `units_issued` double NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `workorder_id` (`workorder_id`)
) ENGINE=InnoDB  AUTO_INCREMENT=22 ;

--
-- Dumping data for table `0_wo_requirements`
--

INSERT INTO `0_wo_requirements` VALUES(1, 1, '102', 1, 1, 0, 'DEF', 10);
INSERT INTO `0_wo_requirements` VALUES(2, 1, '103', 1, 1, 0, 'DEF', 10);
INSERT INTO `0_wo_requirements` VALUES(3, 1, '104', 1, 1, 0, 'DEF', 10);
INSERT INTO `0_wo_requirements` VALUES(4, 2, '102', 1, 1, 0, 'DEF', 0);
INSERT INTO `0_wo_requirements` VALUES(5, 2, '103', 1, 1, 0, 'DEF', 0);
INSERT INTO `0_wo_requirements` VALUES(6, 2, '104', 1, 1, 0, 'DEF', 0);
INSERT INTO `0_wo_requirements` VALUES(7, 3, '102', 1, 1, 0, 'DEF', 2);
INSERT INTO `0_wo_requirements` VALUES(8, 3, '103', 1, 1, 0, 'DEF', 2);
INSERT INTO `0_wo_requirements` VALUES(9, 3, '104', 1, 1, 0, 'DEF', 2);
INSERT INTO `0_wo_requirements` VALUES(10, 4, '102', 1, 1, 0, 'DEF', 4);
INSERT INTO `0_wo_requirements` VALUES(11, 4, '103', 1, 1, 0, 'DEF', 4);
INSERT INTO `0_wo_requirements` VALUES(12, 4, '104', 1, 1, 0, 'DEF', 4);
INSERT INTO `0_wo_requirements` VALUES(13, 5, '102', 1, 1, 0, 'DEF', 5);
INSERT INTO `0_wo_requirements` VALUES(14, 5, '103', 1, 1, 0, 'DEF', 5);
INSERT INTO `0_wo_requirements` VALUES(15, 5, '104', 1, 1, 0, 'DEF', 5);
INSERT INTO `0_wo_requirements` VALUES(16, 6, '102', 1, 1, 0, 'DEF', -5);
INSERT INTO `0_wo_requirements` VALUES(17, 6, '103', 1, 1, 0, 'DEF', -5);
INSERT INTO `0_wo_requirements` VALUES(18, 6, '104', 1, 1, 0, 'DEF', -5);
INSERT INTO `0_wo_requirements` VALUES(19, 7, '102', 1, 1, 0, 'DEF', -2);
INSERT INTO `0_wo_requirements` VALUES(20, 7, '103', 1, 1, 0, 'DEF', -2);
INSERT INTO `0_wo_requirements` VALUES(21, 7, '104', 1, 1, 0, 'DEF', -2);
