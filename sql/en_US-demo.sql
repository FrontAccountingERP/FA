# MySQL dump of database 'fa' on host 'localhost'
# Backup Date and Time: 2009-01-30 10:42
# Built by FrontAccounting 2.1.0 CVS
# http://frontaccounting.net
# Company: Training Co.
# User: Administrator
# Database: en_US-demo.sql



### Structure of table `0_areas` ###

DROP TABLE IF EXISTS `0_areas`;

CREATE TABLE `0_areas` (
  `area_code` int(11) NOT NULL auto_increment,
  `description` varchar(60) NOT NULL default '',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`area_code`),
  UNIQUE KEY `description` (`description`)
) TYPE=MyISAM AUTO_INCREMENT=5  AUTO_INCREMENT=5 ;


### Data of table `0_areas` ###

INSERT INTO `0_areas` VALUES ('1', 'USA', '0');
INSERT INTO `0_areas` VALUES ('2', 'Far East', '0');
INSERT INTO `0_areas` VALUES ('3', 'Africa', '0');
INSERT INTO `0_areas` VALUES ('4', 'Europe', '0');


### Structure of table `0_attachments` ###

DROP TABLE IF EXISTS `0_attachments`;

CREATE TABLE `0_attachments` (
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
) TYPE=MyISAM  AUTO_INCREMENT=1 ;


### Data of table `0_attachments` ###



### Structure of table `0_bank_accounts` ###

DROP TABLE IF EXISTS `0_bank_accounts`;

CREATE TABLE `0_bank_accounts` (
  `account_code` varchar(11) NOT NULL default '',
  `account_type` smallint(6) NOT NULL default '0',
  `bank_account_name` varchar(60) NOT NULL default '',
  `bank_account_number` varchar(100) NOT NULL default '',
  `bank_name` varchar(60) NOT NULL default '',
  `bank_address` tinytext,
  `bank_curr_code` char(3) NOT NULL default '',
  `id` smallint(6) NOT NULL auto_increment,
  `last_reconciled_date` timestamp NOT NULL default '0000-00-00 00:00:00',
  `ending_reconcile_balance` double NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `bank_account_name` (`bank_account_name`),
  KEY `bank_account_number` (`bank_account_number`)
) TYPE=MyISAM AUTO_INCREMENT=4  AUTO_INCREMENT=4 ;


### Data of table `0_bank_accounts` ###

INSERT INTO `0_bank_accounts` VALUES ('1700', '1', 'Current account', 'N/A', 'N/A', '', 'USD', '1', '0000-00-00 00:00:00', '0', '0');
INSERT INTO `0_bank_accounts` VALUES ('1705', '3', 'Petty Cash account', 'N/A', 'N/A', '', 'USD', '2', '0000-00-00 00:00:00', '0', '0');
INSERT INTO `0_bank_accounts` VALUES ('1710', '0', 'Saving account', '10001000', 'Saving Bank', '', 'GBP', '3', '0000-00-00 00:00:00', '0', '0');


### Structure of table `0_bank_trans` ###

DROP TABLE IF EXISTS `0_bank_trans`;

CREATE TABLE `0_bank_trans` (
  `id` int(11) NOT NULL auto_increment,
  `type` smallint(6) default NULL,
  `trans_no` int(11) default NULL,
  `bank_act` varchar(11) default NULL,
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
  KEY `type` (`type`,`trans_no`)
) TYPE=InnoDB AUTO_INCREMENT=30  AUTO_INCREMENT=30 ;


### Data of table `0_bank_trans` ###

INSERT INTO `0_bank_trans` VALUES ('21', '12', '7', '1', '6', '2008-03-06', '100', '0', '0', '2', '1', NULL);
INSERT INTO `0_bank_trans` VALUES ('22', '12', '8', '1', '7', '2008-03-06', '100', '0', '0', '2', '4', NULL);
INSERT INTO `0_bank_trans` VALUES ('23', '12', '9', '1', '8', '2008-03-07', '2000', '0', '0', '2', '1', NULL);
INSERT INTO `0_bank_trans` VALUES ('24', '12', '10', '1', '9', '2009-01-10', '1000', '0', '0', '2', '1', NULL);
INSERT INTO `0_bank_trans` VALUES ('25', '12', '11', '1', '10', '2009-01-10', '183.62116393597', '0', '0', '2', '3', NULL);
INSERT INTO `0_bank_trans` VALUES ('26', '22', '4', '1', '3', '2009-01-10', '-200', '0', '0', '3', '1', NULL);
INSERT INTO `0_bank_trans` VALUES ('27', '2', '5', '1', '14', '2009-01-29','200', '0', '0', '4', '3', NULL);
INSERT INTO `0_bank_trans` VALUES ('28', '1', '8', '1', '7', '2009-01-29', '-50', '0', '0', '4', '1', NULL);
INSERT INTO `0_bank_trans` VALUES ('29', '2', '6', '1', '15', '2009-01-29','50', '0', '0', '4', '3', NULL);


### Structure of table `0_bom` ###

DROP TABLE IF EXISTS `0_bom`;

CREATE TABLE `0_bom` (
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
  KEY `Parent_2` (`parent`),
  KEY `workcentre_added` (`workcentre_added`)
) TYPE=MyISAM AUTO_INCREMENT=6  AUTO_INCREMENT=6 ;


### Data of table `0_bom` ###

INSERT INTO `0_bom` VALUES ('1', '3400', '102', '1', 'DEF', '1');
INSERT INTO `0_bom` VALUES ('2', '3400', '103', '1', 'DEF', '1');
INSERT INTO `0_bom` VALUES ('3', '3400', '104', '1', 'DEF', '1');
INSERT INTO `0_bom` VALUES ('4', '3400', '201', '1', 'DEF', '1');
INSERT INTO `0_bom` VALUES ('5', '3400', '103', '1', 'CWA', '1');


### Structure of table `0_budget_trans` ###

DROP TABLE IF EXISTS `0_budget_trans`;

CREATE TABLE `0_budget_trans` (
  `counter` int(11) NOT NULL auto_increment,
  `type` smallint(6) NOT NULL default '0',
  `type_no` bigint(16) NOT NULL default '1',
  `tran_date` date NOT NULL default '0000-00-00',
  `account` varchar(11) NOT NULL default '',
  `memo_` tinytext NOT NULL,
  `amount` double NOT NULL default '0',
  `dimension_id` int(11) default '0',
  `dimension2_id` int(11) default '0',
  `person_type_id` int(11) default NULL,
  `person_id` tinyblob,
  PRIMARY KEY  (`counter`),
  KEY `Type_and_Number` (`type`,`type_no`)
) TYPE=InnoDB  AUTO_INCREMENT=1 ;


### Data of table `0_budget_trans` ###



### Structure of table `0_chart_class` ###

DROP TABLE IF EXISTS `0_chart_class`;

CREATE TABLE `0_chart_class` (
  `cid` int(11) NOT NULL default '0',
  `class_name` varchar(60) NOT NULL default '',
  `balance_sheet` tinyint(1) NOT NULL default '0',
  `sign_convert` tinyint(1) NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`cid`)
) TYPE=MyISAM  ;


### Data of table `0_chart_class` ###

INSERT INTO `0_chart_class` VALUES ('1', 'Assets', '1', '0', '0');
INSERT INTO `0_chart_class` VALUES ('2', 'Liabilities', '1', '0', '0');
INSERT INTO `0_chart_class` VALUES ('3', 'Income', '0', '1', '0');
INSERT INTO `0_chart_class` VALUES ('4', 'Costs', '0', '1', '0');
INSERT INTO `0_chart_class` VALUES ('5', 'Gross', '0', '1', '0');


### Structure of table `0_chart_master` ###

DROP TABLE IF EXISTS `0_chart_master`;

CREATE TABLE `0_chart_master` (
  `account_code` varchar(11) NOT NULL default '',
  `account_code2` varchar(11) default '',
  `account_name` varchar(60) NOT NULL default '',
  `account_type` int(11) NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`account_code`),
  KEY `account_code` (`account_code`),
  KEY `account_name` (`account_name`)
) TYPE=MyISAM  ;


### Data of table `0_chart_master` ###

INSERT INTO `0_chart_master` VALUES ('3000', '', 'Sales', '1', '0');
INSERT INTO `0_chart_master` VALUES ('3010', '', 'Sales  - Wholesale', '1', '0');
INSERT INTO `0_chart_master` VALUES ('3020', '', 'Sales of Other items', '1', '0');
INSERT INTO `0_chart_master` VALUES ('3400', '', 'Difference On Exchange', '1', '0');
INSERT INTO `0_chart_master` VALUES ('5000', '', 'Direct Labour', '2', '0');
INSERT INTO `0_chart_master` VALUES ('5050', '', 'Direct Labour Recovery', '2', '0');
INSERT INTO `0_chart_master` VALUES ('4200', '', 'Material Usage Varaiance', '2', '0');
INSERT INTO `0_chart_master` VALUES ('4210', '', 'Consumable Materials', '2', '0');
INSERT INTO `0_chart_master` VALUES ('4220', '', 'Purchase price Variance', '2', '0');
INSERT INTO `0_chart_master` VALUES ('4000', '', 'Purchases of materials', '2', '0');
INSERT INTO `0_chart_master` VALUES ('4250', '', 'Discounts Received', '2', '0');
INSERT INTO `0_chart_master` VALUES ('4260', '', 'Exchange Variation', '2', '0');
INSERT INTO `0_chart_master` VALUES ('4300', '', 'Freight Inwards', '2', '0');
INSERT INTO `0_chart_master` VALUES ('4010', '', 'Cost of Goods Sold - Retail', '2', '0');
INSERT INTO `0_chart_master` VALUES ('6790', '', 'Bank Charges', '5', '0');
INSERT INTO `0_chart_master` VALUES ('6800', '', 'Entertainments', '5', '0');
INSERT INTO `0_chart_master` VALUES ('6810', '', 'Legal Expenses', '5', '0');
INSERT INTO `0_chart_master` VALUES ('6600', '', 'Repairs and Maintenance Office', '5', '0');
INSERT INTO `0_chart_master` VALUES ('6730', '', 'phone', '5', '0');
INSERT INTO `0_chart_master` VALUES ('8200', '', 'Bank Interest', '52', '0');
INSERT INTO `0_chart_master` VALUES ('6840', '', 'Credit Control', '5', '0');
INSERT INTO `0_chart_master` VALUES ('7040', '', 'Depreciation Office Equipment', '51', '0');
INSERT INTO `0_chart_master` VALUES ('3800', '', 'Freight Outwards', '5', '0');
INSERT INTO `0_chart_master` VALUES ('4500', '', 'Packaging', '5', '0');
INSERT INTO `0_chart_master` VALUES ('6400', '', 'Commissions', '5', '0');
INSERT INTO `0_chart_master` VALUES ('3200', '', 'Prompt Payment Discounts', '1', '0');
INSERT INTO `0_chart_master` VALUES ('6700', '', 'General Expenses', '5', '0');
INSERT INTO `0_chart_master` VALUES ('5200', '', 'Indirect Labour', '2', '0');
INSERT INTO `0_chart_master` VALUES ('5210', '', 'Overhead Recovery', '5', '0');
INSERT INTO `0_chart_master` VALUES ('1700', '', 'Bank account', '10', '0');
INSERT INTO `0_chart_master` VALUES ('1705', '', 'Petty Cash', '10', '0');
INSERT INTO `0_chart_master` VALUES ('1710', '', 'Foreign currency account', '10', '0');
INSERT INTO `0_chart_master` VALUES ('1500', '', 'Accounts Receivable', '20', '0');
INSERT INTO `0_chart_master` VALUES ('1400', '', 'Stocks of Raw Materials', '45', '0');
INSERT INTO `0_chart_master` VALUES ('1410', '', 'Stocks of Work In Progress', '45', '0');
INSERT INTO `0_chart_master` VALUES ('1420', '', 'Stocks of Finsihed Goods', '45', '0');
INSERT INTO `0_chart_master` VALUES ('1430', '', 'Goods Received Clearing account', '30', '0');
INSERT INTO `0_chart_master` VALUES ('2630', '', 'Accounts Payable', '30', '0');
INSERT INTO `0_chart_master` VALUES ('2660', '', 'VAT out 5', '30', '0');
INSERT INTO `0_chart_master` VALUES ('2662', '', 'VAT out 1', '30', '0');
INSERT INTO `0_chart_master` VALUES ('2664', '', 'VAT out 25', '30', '0');
INSERT INTO `0_chart_master` VALUES ('2680', '', 'VAT In 5', '30', '0');
INSERT INTO `0_chart_master` VALUES ('2682', '', 'VAT In 25', '30', '0');
INSERT INTO `0_chart_master` VALUES ('2050', '', 'Retained Earnings', '50', '0');
INSERT INTO `0_chart_master` VALUES ('2000', '', 'Share Capital', '50', '0');


### Structure of table `0_chart_types` ###

DROP TABLE IF EXISTS `0_chart_types`;

CREATE TABLE `0_chart_types` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(60) NOT NULL default '',
  `class_id` tinyint(1) NOT NULL default '0',
  `parent` int(11) NOT NULL default '-1',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `name` (`name`)
) TYPE=MyISAM AUTO_INCREMENT=53  AUTO_INCREMENT=53 ;


### Data of table `0_chart_types` ###

INSERT INTO `0_chart_types` VALUES ('1', 'Sales', '3', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('2', 'Cost of Sales', '4', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('5', 'Expenses', '4', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('10', 'Cash/Bank', '1', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('20', 'Accounts Receivable', '1', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('30', 'Accounts Payable', '2', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('40', 'Fixed Assets', '1', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('45', 'Inventory', '1', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('50', 'Equity', '2', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('51', 'Depreciations', '4', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('52', 'Financials', '4', '-1', '0');


### Structure of table `0_comments` ###

DROP TABLE IF EXISTS `0_comments`;

CREATE TABLE `0_comments` (
  `type` int(11) NOT NULL default '0',
  `id` int(11) NOT NULL default '0',
  `date_` date default '0000-00-00',
  `memo_` tinytext
) TYPE=InnoDB  ;


### Data of table `0_comments` ###

INSERT INTO `0_comments` VALUES ('17', '2', '2006-01-18', 'initial balances');
INSERT INTO `0_comments` VALUES ('10', '6', '2007-01-30', 'Hi there you got it!');
INSERT INTO `0_comments` VALUES ('12', '6', '2007-01-30', 'This is good');
INSERT INTO `0_comments` VALUES ('1', '5', '2007-01-30', 'Total Gas');
INSERT INTO `0_comments` VALUES ('0', '6', '2007-02-02', 'A big memo');
INSERT INTO `0_comments` VALUES ('10', '7', '2007-02-03', 'Another big memo, which looks good.');
INSERT INTO `0_comments` VALUES ('4', '3', '2007-03-09', 'A little cash up front.');
INSERT INTO `0_comments` VALUES ('26', '6', '2008-02-28', 'nowe ');
INSERT INTO `0_comments` VALUES ('40', '2', '0000-00-00', 'Another project');


### Structure of table `0_company` ###

DROP TABLE IF EXISTS `0_company`;

CREATE TABLE `0_company` (
  `coy_code` int(11) NOT NULL default '1',
  `coy_name` varchar(60) NOT NULL default '',
  `gst_no` varchar(25) NOT NULL default '',
  `coy_no` varchar(25) NOT NULL default '0',
  `tax_prd` int(11) NOT NULL default '1',
  `tax_last` int(11) NOT NULL default '1',
  `postal_address` tinytext NOT NULL,
  `phone` varchar(30) NOT NULL default '',
  `fax` varchar(30) NOT NULL default '',
  `email` varchar(100) NOT NULL default '',
  `coy_logo` varchar(100) NOT NULL default '',
  `domicile` varchar(55) NOT NULL default '',
  `curr_default` char(3) NOT NULL default '',
  `debtors_act` varchar(11) NOT NULL default '',
  `pyt_discount_act` varchar(11) NOT NULL default '',
  `creditors_act` varchar(11) NOT NULL default '',
  `grn_act` varchar(11) NOT NULL default '',
  `exchange_diff_act` varchar(11) NOT NULL default '',
  `purch_exchange_diff_act` varchar(11) NOT NULL default '',
  `retained_earnings_act` varchar(11) NOT NULL default '',
  `freight_act` varchar(11) NOT NULL default '',
  `default_sales_act` varchar(11) NOT NULL default '',
  `default_sales_discount_act` varchar(11) NOT NULL default '',
  `default_prompt_payment_act` varchar(11) NOT NULL default '',
  `default_inventory_act` varchar(11) NOT NULL default '',
  `default_cogs_act` varchar(11) NOT NULL default '',
  `default_adj_act` varchar(11) NOT NULL default '',
  `default_inv_sales_act` varchar(11) NOT NULL default '',
  `default_assembly_act` varchar(11) NOT NULL default '',
  `payroll_act` varchar(11) NOT NULL default '',
  `allow_negative_stock` tinyint(1) NOT NULL default '0',
  `po_over_receive` int(11) NOT NULL default '10',
  `po_over_charge` int(11) NOT NULL default '10',
  `default_credit_limit` int(11) NOT NULL default '1000',
  `default_workorder_required` int(11) NOT NULL default '20',
  `default_dim_required` int(11) NOT NULL default '20',
  `past_due_days` int(11) NOT NULL default '30',
  `use_dimension` tinyint(1) default '0',
  `f_year` int(11) NOT NULL default '1',
  `no_item_list` tinyint(1) NOT NULL default '0',
  `no_customer_list` tinyint(1) NOT NULL default '0',
  `no_supplier_list` tinyint(1) NOT NULL default '0',
  `base_sales` int(11) NOT NULL default '-1',
  `foreign_codes` tinyint(1) NOT NULL default '0',
  `accumulate_shipping` tinyint(1) NOT NULL default '0',
  `legal_text` tinytext NOT NULL,
  `default_delivery_required` smallint(6) NOT NULL default '1',
  PRIMARY KEY  (`coy_code`)
) TYPE=MyISAM  ;


### Data of table `0_company` ###

INSERT INTO `0_company` VALUES ('1', 'Training Co.', '987654321', '123123123', '1', '1', 'N/A', '202-122320', '202-18889123', 'delta@delta.com', 'logo_frontaccounting.jpg', 'DownTown', 'USD', '1500', '4250', '2630', '1430', '4260', '4220', '2050', '3800', '3000', '3000', '3200', '1420', '4010', '4210', '3000', '1410', '5000', '0', '10', '10', '1000', '20', '20', '30', '1', '7', '0', '0', '0', '0', '0', '0', '', '1');


### Structure of table `0_credit_status` ###

DROP TABLE IF EXISTS `0_credit_status`;

CREATE TABLE `0_credit_status` (
  `id` int(11) NOT NULL auto_increment,
  `reason_description` char(100) NOT NULL default '',
  `dissallow_invoices` tinyint(1) NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `reason_description` (`reason_description`)
) TYPE=MyISAM AUTO_INCREMENT=5  AUTO_INCREMENT=5 ;


### Data of table `0_credit_status` ###

INSERT INTO `0_credit_status` VALUES ('1', 'Good History', '0', '0');
INSERT INTO `0_credit_status` VALUES ('3', 'No more work until payment received', '1', '0');
INSERT INTO `0_credit_status` VALUES ('4', 'In liquidation', '1', '0');


### Structure of table `0_currencies` ###

DROP TABLE IF EXISTS `0_currencies`;

CREATE TABLE `0_currencies` (
  `currency` varchar(60) NOT NULL default '',
  `curr_abrev` char(3) NOT NULL default '',
  `curr_symbol` varchar(10) NOT NULL default '',
  `country` varchar(100) NOT NULL default '',
  `hundreds_name` varchar(15) NOT NULL default '',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`curr_abrev`)
) TYPE=MyISAM  ;


### Data of table `0_currencies` ###

INSERT INTO `0_currencies` VALUES ('Kronor', 'SEK', 'kr', 'Sweden', '?ren', '0');
INSERT INTO `0_currencies` VALUES ('Kroner', 'DKK', 'kr.', 'Denmark', '?re', '0');
INSERT INTO `0_currencies` VALUES ('Euro', 'EUR', '?', 'Europe', 'Cents', '0');
INSERT INTO `0_currencies` VALUES ('Pounds', 'GBP', '?', 'England', 'Pence', '0');
INSERT INTO `0_currencies` VALUES ('US Dollars', 'USD', '$', 'United States', 'Cents', '0');


### Structure of table `0_cust_allocations` ###

DROP TABLE IF EXISTS `0_cust_allocations`;

CREATE TABLE `0_cust_allocations` (
  `id` int(11) NOT NULL auto_increment,
  `amt` double unsigned default NULL,
  `date_alloc` date NOT NULL default '0000-00-00',
  `trans_no_from` int(11) default NULL,
  `trans_type_from` int(11) default NULL,
  `trans_no_to` int(11) default NULL,
  `trans_type_to` int(11) default NULL,
  PRIMARY KEY  (`id`)
) TYPE=InnoDB AUTO_INCREMENT=12  AUTO_INCREMENT=12 ;


### Data of table `0_cust_allocations` ###

INSERT INTO `0_cust_allocations` VALUES ('5', '135', '2008-03-06', '1', '11', '5', '10');
INSERT INTO `0_cust_allocations` VALUES ('6', '135', '2008-03-06', '2', '11', '6', '10');
INSERT INTO `0_cust_allocations` VALUES ('7', '135', '2008-03-06', '3', '11', '4', '10');
INSERT INTO `0_cust_allocations` VALUES ('8', '125', '2008-03-07', '4', '11', '4', '11');
INSERT INTO `0_cust_allocations` VALUES ('9', '137.5', '2009-01-10', '10', '12', '9', '10');
INSERT INTO `0_cust_allocations` VALUES ('10', '125', '2009-01-10', '10', '12', '11', '10');
INSERT INTO `0_cust_allocations` VALUES ('11', '11', '2009-01-10', '11', '12', '17', '10');


### Structure of table `0_cust_branch` ###

DROP TABLE IF EXISTS `0_cust_branch`;

CREATE TABLE `0_cust_branch` (
  `branch_code` int(11) NOT NULL auto_increment,
  `debtor_no` int(11) NOT NULL default '0',
  `br_name` varchar(60) NOT NULL default '',
  `br_address` tinytext NOT NULL,
  `area` int(11) default NULL,
  `salesman` int(11) NOT NULL default '0',
  `phone` varchar(30) NOT NULL default '',
  `fax` varchar(30) NOT NULL default '',
  `contact_name` varchar(60) NOT NULL default '',
  `email` varchar(100) NOT NULL default '',
  `default_location` varchar(5) NOT NULL default '',
  `tax_group_id` int(11) default NULL,
  `sales_account` varchar(11) default NULL,
  `sales_discount_account` varchar(11) default NULL,
  `receivables_account` varchar(11) default NULL,
  `payment_discount_account` varchar(11) default NULL,
  `default_ship_via` int(11) NOT NULL default '1',
  `disable_trans` tinyint(4) NOT NULL default '0',
  `br_post_address` tinytext NOT NULL,
  `group_no` int(11) NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`branch_code`,`debtor_no`),
  KEY `branch_code` (`branch_code`),
  KEY `br_name` (`br_name`)
) TYPE=MyISAM AUTO_INCREMENT=8  AUTO_INCREMENT=8 ;


### Data of table `0_cust_branch` ###

INSERT INTO `0_cust_branch` VALUES ('1', '1', 'Main', '', '1', '1', '', '', 'Lucky Luke Inc.', 'lucky@luke.com', 'DEF', '1', '3000', '3000', '1500', '3200', '1', '0', 'The Road', '0', '0');
INSERT INTO `0_cust_branch` VALUES ('2', '1', 'Service divison', '', '4', '1', '', '', '', '', 'DEF', '2', '3000', '3000', '1500', '3200', '1', '0', 'Another Road', '0', '0');
INSERT INTO `0_cust_branch` VALUES ('3', '2', 'Main', '', '4', '2', '', '', 'Money Makers Ltd.', '', 'DEF', '2', '3000', '3000', '1500', '3200', '1', '0', '', '0', '0');
INSERT INTO `0_cust_branch` VALUES ('5', '3', 'Main', '', '4', '1', '', '', 'Junk Beer ApS', '', 'CWA', '2', '3000', '3000', '1500', '3200', '1', '0', '', '0', '0');
INSERT INTO `0_cust_branch` VALUES ('6', '4', 'Johny Bravo', 'Never Mind 13', '1', '1', '123', '', 'Johny Bravo', '', 'DEF', '1', '3000', '3000', '1500', '3200', '1', '0', 'Never Mind 13', '0', '0');
INSERT INTO `0_cust_branch` VALUES ('7', '3', 'Junk Beer ApS', 'N/A', '1', '1', '1223123', '', 'junk@junkbeer.dk', '', 'DEF', '5', '3000', '3000', '1500', '3200', '1', '0', 'N/A', '0', '0');


### Structure of table `0_debtor_trans` ###

DROP TABLE IF EXISTS `0_debtor_trans`;

CREATE TABLE `0_debtor_trans` (
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
  `trans_link` int(11) NOT NULL default '0',
  `dimension_id` int(11) NOT NULL default '0',
  `dimension2_id` int(11) NOT NULL default '0',
  PRIMARY KEY  (`trans_no`,`type`),
  KEY `debtor_no` (`debtor_no`,`branch_code`)
) TYPE=InnoDB  ;


### Data of table `0_debtor_trans` ###

INSERT INTO `0_debtor_trans` VALUES ('1', '10', '0', '4', '6', '2008-03-06', '2008-03-16', '0', '1', '1', '125', '0', '10', '0', '0', '0', '1', '1', '1', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('1', '11', '0', '4', '6', '2008-03-06', '0000-00-00', '0', '1', '5', '125', '0', '10', '0', '0', '0', '1', '1', '5', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('1', '13', '1', '4', '6', '2008-03-06', '2008-03-16', '0', '1', '1', '125', '0', '10', '0', '0', '0', '1', '1', '1', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('2', '10', '0', '4', '6', '2008-03-06', '2008-03-06', '1', '1', '2', '125', '0', '10', '0', '0', '0', '1', '1', '2', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('2', '11', '0', '4', '6', '2008-03-06', '0000-00-00', '1', '1', '6', '125', '0', '10', '0', '0', '135', '1', '1', '6', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('2', '13', '1', '4', '6', '2008-03-06', '2008-03-06', 'auto', '1', '2', '125', '0', '10', '0', '0', '0', '1', '1', '2', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('3', '10', '0', '4', '6', '2008-03-06', '2008-03-06', '2', '1', '3', '125', '0', '10', '0', '0', '0', '1', '1', '3', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('3', '11', '0', '4', '6', '2008-03-06', '0000-00-00', '2', '1', '4', '125', '0', '10', '0', '0', '135', '1', '1', '4', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('3', '13', '1', '4', '6', '2008-03-06', '2008-03-06', 'auto', '1', '3', '125', '0', '10', '0', '0', '0', '1', '1', '3', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('4', '10', '1', '4', '6', '2008-03-06', '2008-03-06', '3', '1', '4', '125', '0', '10', '0', '0', '135', '1', '1', '4', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('4', '11', '0', '1', '1', '2008-03-07', '0000-00-00', '3', '1', '0', '100', '25', '0', '0', '0', '250', '1', '1', '0', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('4', '13', '1', '4', '6', '2008-03-06', '2008-03-06', 'auto', '1', '4', '125', '0', '10', '0', '0', '0', '1', '1', '4', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('5', '10', '1', '4', '6', '2008-03-06', '2008-03-06', '4', '1', '5', '125', '0', '10', '0', '0', '135', '1', '1', '5', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('5', '13', '1', '4', '6', '2008-03-06', '2008-03-06', 'auto', '1', '5', '125', '0', '10', '0', '0', '0', '1', '1', '5', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('6', '10', '1', '4', '6', '2008-03-06', '2008-03-06', '5', '1', '6', '125', '0', '10', '0', '0', '135', '1', '1', '6', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('6', '13', '1', '4', '6', '2008-03-06', '2008-03-06', 'auto', '1', '6', '125', '0', '10', '0', '0', '0', '1', '1', '6', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('7', '10', '0', '4', '6', '2008-03-08', '2008-03-08', '6', '1', '9', '125', '0', '10', '0', '0', '0', '1', '1', '9', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('7', '12', '0', '1', '1', '2008-03-06', '0000-00-00', '6', '0', '0', '100', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('7', '13', '0', '4', '6', '2008-03-06', '2008-03-06', '1', '1', '7', '125', '0', '10', '0', '0', '0', '1', '1', '0', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('8', '10', '0', '4', '6', '2008-03-09', '2008-03-09', '7', '1', '10', '125', '0', '12.5', '0', '0', '0', '1', '1', '10', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('8', '12', '0', '4', '6', '2008-03-06', '0000-00-00', '7', '0', '0', '100', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('8', '13', '1', '4', '6', '2008-03-07', '2008-03-07', '2', '1', '8', '125', '0', '10', '0', '0', '0', '1', '1', '13', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('9', '10', '0', '1', '1', '2008-03-09', '2008-03-09', '8', '2', '11', '100', '25', '10', '2.5', '0', '137.5', '1', '1', '11', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('9', '12', '0', '1', '1', '2008-03-07', '0000-00-00', '8', '0', '0', '2000', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('9', '13', '1', '4', '6', '2008-03-08', '2008-03-08', 'auto', '1', '9', '125', '0', '10', '0', '0', '0', '1', '1', '7', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('10', '10', '0', '4', '6', '2008-03-09', '2008-03-09', '9', '1', '12', '125', '0', '10', '0', '0', '0', '1', '1', '12', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('10', '12', '0', '1', '1', '2009-01-10', '0000-00-00', '9', '0', '0', '1000', '0', '0', '0', '0', '262.5', '1', '0', '0', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('10', '13', '1', '4', '6', '2008-03-09', '2008-03-09', 'auto', '1', '10', '125', '0', '12.5', '0', '0', '0', '1', '1', '8', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('11', '10', '0', '1', '1', '2008-03-10', '2008-03-20', '10', '2', '15', '100', '25', '0', '0', '0', '125', '1', '1', '14', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('11', '12', '0', '3', '7', '2009-01-10', '0000-00-00', '10', '0', '0', '1000', '0', '0', '0', '0', '11', '0.1836', '0', '0', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('11', '13', '1', '1', '1', '2008-03-09', '2008-03-09', 'auto', '2', '11', '100', '25', '10', '2.5', '0', '0', '1', '1', '9', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('12', '10', '0', '1', '1', '2008-03-10', '2008-03-29', '11', '2', '16', '100', '25', '0', '0', '0', '0', '1', '1', '15', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('12', '13', '1', '4', '6', '2008-03-09', '2008-03-09', 'auto', '1', '12', '125', '0', '10', '0', '0', '0', '1', '1', '10', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('13', '10', '0', '4', '6', '2008-03-07', '2008-03-07', '12', '1', '8', '125', '0', '10', '0', '0', '0', '1', '1', '8', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('13', '13', '1', '4', '6', '2008-03-10', '2008-03-20', '3', '1', '14', '40', '0', '0', '0', '0', '0', '1', '1', '15', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('14', '10', '0', '4', '6', '2008-03-10', '2008-03-10', '13', '1', '18', '125', '0', '10', '0', '0', '0', '1', '1', '17', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('14', '13', '1', '1', '1', '2008-03-10', '2008-03-20', '4', '2', '15', '100', '25', '0', '0', '0', '0', '1', '1', '11', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('15', '10', '0', '4', '6', '2008-03-10', '2008-04-17', '14', '1', '14', '40', '0', '0', '0', '0', '0', '1', '1', '13', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('15', '13', '1', '1', '1', '2008-03-10', '2008-03-20', '5', '2', '16', '100', '25', '0', '0', '0', '0', '1', '1', '12', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('16', '10', '0', '4', '6', '2008-03-17', '2008-04-17', '15', '1', '19', '125', '0', '10', '0', '0', '0', '1', '1', '18', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('16', '13', '1', '3', '5', '2008-03-10', '2008-03-20', '6', '1', '17', '11', '0', '0', '0', '0', '0', '0.20563796227935', '1', '17', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('17', '10', '0', '3', '5', '2008-03-10', '2008-04-17', '16', '1', '17', '11', '0', '0', '0', '0', '11', '0.20563796227935', '1', '16', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('17', '13', '1', '4', '6', '2008-03-10', '2008-03-10', 'auto', '1', '18', '125', '0', '10', '0', '0', '0', '1', '1', '14', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('18', '10', '0', '3', '5', '2008-03-29', '2008-04-17', '17', '1', '20', '0', '0', '0', '0', '0', '0', '0.20674817019223', '1', '19', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('18', '13', '1', '4', '6', '2008-03-17', '2008-03-27', '7', '1', '19', '125', '0', '10', '0', '0', '0', '1', '1', '16', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('19', '10', '0', '4', '6', '2008-03-29', '2008-04-17', '18', '1', '21', '125', '0', '0', '0', '0', '0', '1', '1', '20', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('19', '13', '1', '3', '5', '2008-03-29', '2008-04-17', 'auto', '1', '20', '0', '0', '0', '0', '0', '0', '0.20674817019223', '1', '18', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('20', '10', '0', '1', '1', '2009-01-10', '2009-02-17', '19', '2', '22', '30', '7.5', '0', '0', '0', '0', '1', '1', '21', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('20', '13', '1', '4', '6', '2008-03-29', '2008-04-08', '8', '1', '21', '125', '0', '0', '0', '0', '0', '1', '1', '19', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('21', '10', '0', '3', '7', '2009-01-29', '2009-01-29', '20', '1', '23', '100', '0', '0', '0', '0', '0', '0.17593462333271', '1', '22', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('21', '13', '1', '1', '1', '2009-01-10', '2009-02-17', 'auto', '2', '22', '30', '7.5', '0', '0', '0', '0', '1', '1', '20', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('22', '10', '0', '3', '7', '2009-01-29', '2009-02-17', '21', '1', '24', '100', '0', '0', '0', '0', '0', '0.17593462333271', '1', '23', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('22', '13', '1', '3', '7', '2009-01-29', '2009-01-29', 'auto', '1', '23', '100', '0', '0', '0', '0', '0', '0.17593462333271', '1', '21', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('23', '10', '0', '3', '7', '2009-01-29', '2009-02-17', '22', '1', '25', '100', '0', '0', '0', '0', '0', '0.17593462333271', '1', '24', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('23', '13', '1', '3', '7', '2009-01-29', '2009-02-17', 'auto', '1', '24', '100', '0', '0', '0', '0', '0', '0.17593462333271', '1', '22', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('24', '10', '0', '3', '7', '2009-01-29', '2009-02-17', '23', '1', '26', '100', '0', '0', '0', '0', '0', '0.17593462333271', '1', '25', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('24', '13', '1', '3', '7', '2009-01-29', '2009-02-17', 'auto', '1', '25', '100', '0', '0', '0', '0', '0', '0.17593462333271', '1', '23', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('25', '10', '0', '3', '7', '2009-01-29', '2009-02-17', '24', '1', '27', '100', '0', '0', '0', '0', '0', '0.17593462333271', '1', '26', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('25', '13', '1', '3', '7', '2009-01-29', '2009-02-17', 'auto', '1', '26', '100', '0', '0', '0', '0', '0', '0.17593462333271', '1', '24', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('26', '13', '1', '3', '7', '2009-01-29', '2009-02-17', 'auto', '1', '27', '100', '0', '0', '0', '0', '0', '0.17593462333271', '1', '25', '0', '0');
INSERT INTO `0_debtor_trans` VALUES ('27', '13', '0', '3', '7', '2009-01-30', '2009-01-31', '9', '1', '28', '110', '0', '0', '0', '0', '0', '0.17593462333271', '1', '0', '0', '0');


### Structure of table `0_debtor_trans_details` ###

DROP TABLE IF EXISTS `0_debtor_trans_details`;

CREATE TABLE `0_debtor_trans_details` (
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
  PRIMARY KEY  (`id`)
) TYPE=InnoDB AUTO_INCREMENT=59  AUTO_INCREMENT=59 ;


### Data of table `0_debtor_trans_details` ###

INSERT INTO `0_debtor_trans_details` VALUES ('1', '1', '13', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('2', '1', '10', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('3', '2', '13', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('4', '2', '10', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('5', '3', '13', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('6', '3', '10', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('7', '4', '13', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('8', '4', '10', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('9', '5', '13', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('10', '5', '10', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('11', '1', '11', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('12', '6', '13', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('13', '6', '10', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('14', '7', '13', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('15', '2', '11', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('16', '3', '11', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('17', '4', '11', '102', '17 inch VGA Monitor', '100', '25', '1', '0', '0', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('18', '8', '13', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('19', '9', '13', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('20', '7', '10', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('21', '10', '13', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('22', '8', '10', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('23', '11', '13', '102', '17 inch VGA Monitor', '100', '25', '1', '0', '80', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('24', '9', '10', '102', '17 inch VGA Monitor', '100', '25', '1', '0', '80', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('25', '12', '13', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('26', '10', '10', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('27', '13', '13', '103', '32MB VGA Card', '40', '8', '1', '0', '20', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('28', '14', '13', '102', '17 inch VGA Monitor', '100', '25', '1', '0', '80', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('29', '11', '10', '102', '17 inch VGA Monitor', '100', '25', '1', '0', '80', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('30', '15', '13', '102', '17 inch VGA Monitor', '100', '25', '1', '0', '80', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('31', '12', '10', '102', '17 inch VGA Monitor', '100', '25', '1', '0', '80', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('32', '13', '10', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('33', '16', '13', '102', '17 inch VGA Monitor', '11', '0', '1', '0', '80', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('34', '17', '13', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('35', '14', '10', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '80', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('36', '15', '10', '103', '32MB VGA Card', '40', '8', '1', '0', '20', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('37', '18', '13', '102', '17 inch VGA Monitor', '125', '25', '0', '0', '-3.4114285714283', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('38', '18', '13', 'AA101', 'olie 5w40', '125', '25', '1', '0', '5.0599509174312', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('39', '16', '10', '102', '17 inch VGA Monitor', '125', '25', '0', '0', '-3.4114285714283', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('40', '16', '10', 'AA101', 'olie 5w40', '125', '25', '1', '0', '5.0599509174312', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('41', '17', '10', '102', '17 inch VGA Monitor', '11', '0', '1', '0', '80', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('42', '19', '13', '102', '17 inch VGA Monitor', '0', '0', '1', '0', '-3.4114285714283', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('43', '18', '10', '102', '17 inch VGA Monitor', '0', '0', '1', '0', '-3.4114285714283', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('44', '20', '13', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '-3.4114285714283', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('45', '19', '10', '102', '17 inch VGA Monitor', '125', '25', '1', '0', '-3.4114285714283', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('46', '21', '13', '201', 'Assembly Labour', '30', '7.5', '1', '0', '0', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('47', '20', '10', '201', 'Assembly Labour', '30', '7.5', '1', '0', '0', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('48', '22', '13', '201', 'Assembly Labour', '100', '20', '1', '0', '0', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('49', '21', '10', '201', 'Assembly Labour', '100', '20', '1', '0', '0', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('50', '23', '13', '201', 'Assembly Labour', '100', '20', '1', '0', '0', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('51', '22', '10', '201', 'Assembly Labour', '100', '20', '1', '0', '0', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('52', '24', '13', '201', 'Assembly Labour', '100', '20', '1', '0', '0', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('53', '23', '10', '201', 'Assembly Labour', '100', '20', '1', '0', '0', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('54', '25', '13', '201', 'Assembly Labour', '100', '20', '1', '0', '0', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('55', '24', '10', '201', 'Assembly Labour', '100', '20', '1', '0', '0', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('56', '26', '13', '201', 'Assembly Labour', '100', '0', '1', '0', '0', '1');
INSERT INTO `0_debtor_trans_details` VALUES ('57', '25', '10', '201', 'Assembly Labour', '100', '0', '1', '0', '0', '0');
INSERT INTO `0_debtor_trans_details` VALUES ('58', '27', '13', '201', 'Assembly Labour', '110', '0', '1', '0', '0', '0');


### Structure of table `0_debtors_master` ###

DROP TABLE IF EXISTS `0_debtors_master`;

CREATE TABLE `0_debtors_master` (
  `debtor_no` int(11) NOT NULL auto_increment,
  `name` varchar(80) NOT NULL default '',
  `address` tinytext,
  `email` varchar(100) NOT NULL default '',
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
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`debtor_no`),
  UNIQUE KEY `name` (`name`)
) TYPE=MyISAM AUTO_INCREMENT=5  AUTO_INCREMENT=5 ;


### Data of table `0_debtors_master` ###

INSERT INTO `0_debtors_master` VALUES ('1', 'Lucky Luke Inc.', '35 Waldorf Street\r\nTown 19358, AR', 'joe@home.com', '12311231', 'USD', '2', '0', '0', '1', '1', '0', '0', '1000', '0');
INSERT INTO `0_debtors_master` VALUES ('2', 'Money Makers Ltd.', 'N/A', '', '9876543', 'GBP', '2', '0', '0', '1', '1', '0', '0', '1000', '0');
INSERT INTO `0_debtors_master` VALUES ('3', 'Junk Beer ApS', 'N/A', '', '123321123', 'DKK', '1', '0', '0', '1', '1', '0', '0', '1000', '0');
INSERT INTO `0_debtors_master` VALUES ('4', 'Retail clients', 'Never Mind 13', '', '', 'USD', '1', '0', '0', '1', '1', '0', '0', '1000', '0');


### Structure of table `0_dimensions` ###

DROP TABLE IF EXISTS `0_dimensions`;

CREATE TABLE `0_dimensions` (
  `id` int(11) NOT NULL auto_increment,
  `reference` varchar(60) NOT NULL default '',
  `name` varchar(60) NOT NULL default '',
  `type_` tinyint(1) NOT NULL default '1',
  `closed` tinyint(1) NOT NULL default '0',
  `date_` date NOT NULL default '0000-00-00',
  `due_date` date NOT NULL default '0000-00-00',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `reference` (`reference`)
) TYPE=InnoDB AUTO_INCREMENT=4  AUTO_INCREMENT=4 ;


### Data of table `0_dimensions` ###

INSERT INTO `0_dimensions` VALUES ('1', '1', 'Development', '1', '0', '2006-01-18', '2006-02-07');
INSERT INTO `0_dimensions` VALUES ('2', '2', 'Support', '1', '0', '2006-01-18', '2007-03-07');
INSERT INTO `0_dimensions` VALUES ('3', '3', 'Training', '2', '0', '2006-01-18', '2007-03-07');


### Structure of table `0_exchange_rates` ###

DROP TABLE IF EXISTS `0_exchange_rates`;

CREATE TABLE `0_exchange_rates` (
  `id` int(11) NOT NULL auto_increment,
  `curr_code` char(3) NOT NULL default '',
  `rate_buy` double NOT NULL default '0',
  `rate_sell` double NOT NULL default '0',
  `date_` date NOT NULL default '0000-00-00',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `curr_code` (`curr_code`,`date_`)
) TYPE=MyISAM AUTO_INCREMENT=21  AUTO_INCREMENT=21 ;


### Data of table `0_exchange_rates` ###

INSERT INTO `0_exchange_rates` VALUES ('2', 'GBP', '1.2', '1.2', '2006-01-18');
INSERT INTO `0_exchange_rates` VALUES ('3', 'SEK', '0.1667', '0.1667', '2007-01-29');
INSERT INTO `0_exchange_rates` VALUES ('4', 'DKK', '0.2', '0.2', '2007-03-05');
INSERT INTO `0_exchange_rates` VALUES ('5', 'EUR', '1.1', '1.1', '2007-03-05');
INSERT INTO `0_exchange_rates` VALUES ('6', 'DKK', '0.20563796227935', '0.20563796227935', '2008-03-06');
INSERT INTO `0_exchange_rates` VALUES ('7', 'EUR', '1.5561', '1.5561', '2008-03-15');
INSERT INTO `0_exchange_rates` VALUES ('8', 'DKK', '0.20867920985932', '0.20867920985932', '2008-03-16');
INSERT INTO `0_exchange_rates` VALUES ('9', 'GBP', '1.9816266221251', '1.9816266221251', '2008-03-24');
INSERT INTO `0_exchange_rates` VALUES ('10', 'DKK', '0.20674817019223', '0.20674817019223', '2008-03-24');
INSERT INTO `0_exchange_rates` VALUES ('11', 'EUR', '1.5569', '1.5569', '2008-03-25');
INSERT INTO `0_exchange_rates` VALUES ('12', 'EUR', '1.5796', '1.5796', '2008-03-28');
INSERT INTO `0_exchange_rates` VALUES ('13', 'DKK', '0.18362116393597', '0.18362116393597', '2009-01-10');
INSERT INTO `0_exchange_rates` VALUES ('14', 'GBP', '1.5292802860975', '1.5292802860975', '2009-01-10');
INSERT INTO `0_exchange_rates` VALUES ('15', 'DKK', '0.17593462333271', '0.17593462333271', '2009-01-29');
INSERT INTO `0_exchange_rates` VALUES ('16', 'GBP', '1.4293813028073', '1.4293813028073', '2009-01-29');
INSERT INTO `0_exchange_rates` VALUES ('17', 'DKK', '0.17593462333271', '0.17593462333271', '2009-01-30');
INSERT INTO `0_exchange_rates` VALUES ('18', 'GBP', '1.4293813028073', '1.4293813028073', '2009-01-30');
INSERT INTO `0_exchange_rates` VALUES ('19', 'EUR', '1.3111', '1.3111', '2009-01-30');
INSERT INTO `0_exchange_rates` VALUES ('20', 'SEK', '0.12446246001082', '0.12446246001082', '2009-01-30');


### Structure of table `0_fiscal_year` ###

DROP TABLE IF EXISTS `0_fiscal_year`;

CREATE TABLE `0_fiscal_year` (
  `id` int(11) NOT NULL auto_increment,
  `begin` date default '0000-00-00',
  `end` date default '0000-00-00',
  `closed` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`)
) TYPE=InnoDB AUTO_INCREMENT=8  AUTO_INCREMENT=8 ;


### Data of table `0_fiscal_year` ###

INSERT INTO `0_fiscal_year` VALUES ('1', '2006-01-01', '2006-12-31', '0');
INSERT INTO `0_fiscal_year` VALUES ('2', '2007-01-01', '2007-12-31', '0');
INSERT INTO `0_fiscal_year` VALUES ('5', '2005-01-01', '2005-12-31', '1');
INSERT INTO `0_fiscal_year` VALUES ('6', '2008-01-01', '2008-12-31', '0');
INSERT INTO `0_fiscal_year` VALUES ('7', '2009-01-01', '2009-12-31', '0');


### Structure of table `0_gl_trans` ###

DROP TABLE IF EXISTS `0_gl_trans`;

CREATE TABLE `0_gl_trans` (
  `counter` int(11) NOT NULL auto_increment,
  `type` smallint(6) NOT NULL default '0',
  `type_no` bigint(16) NOT NULL default '1',
  `tran_date` date NOT NULL default '0000-00-00',
  `account` varchar(11) NOT NULL default '',
  `memo_` tinytext NOT NULL,
  `amount` double NOT NULL default '0',
  `dimension_id` int(11) NOT NULL default '0',
  `dimension2_id` int(11) NOT NULL default '0',
  `person_type_id` int(11) default NULL,
  `person_id` tinyblob,
  PRIMARY KEY  (`counter`),
  KEY `Type_and_Number` (`type`,`type_no`)
) TYPE=InnoDB AUTO_INCREMENT=215  AUTO_INCREMENT=215 ;


### Data of table `0_gl_trans` ###

INSERT INTO `0_gl_trans` VALUES ('1', '13', '1', '2008-03-06', '4010', '', '80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('2', '13', '1', '2008-03-06', '1420', '', '-80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('3', '10', '1', '2008-03-06', '3000', '', '-100', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('4', '10', '1', '2008-03-06', '1500', '', '135', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('5', '10', '1', '2008-03-06', '3800', '', '-8', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('6', '10', '1', '2008-03-06', '2664', '', '-27', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('7', '13', '2', '2008-03-06', '4010', '', '80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('8', '13', '2', '2008-03-06', '1420', '', '-80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('9', '10', '2', '2008-03-06', '3000', '', '-100', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('10', '10', '2', '2008-03-06', '1500', '', '135', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('11', '10', '2', '2008-03-06', '3800', '', '-8', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('12', '10', '2', '2008-03-06', '2664', '', '-27', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('13', '13', '3', '2008-03-06', '4010', '', '80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('14', '13', '3', '2008-03-06', '1420', '', '-80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('15', '10', '3', '2008-03-06', '3000', '', '-100', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('16', '10', '3', '2008-03-06', '1500', '', '135', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('17', '10', '3', '2008-03-06', '3800', '', '-8', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('18', '10', '3', '2008-03-06', '2664', '', '-27', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('19', '13', '4', '2008-03-06', '4010', '', '80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('20', '13', '4', '2008-03-06', '1420', '', '-80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('21', '10', '4', '2008-03-06', '3000', '', '-100', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('22', '10', '4', '2008-03-06', '1500', '', '135', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('23', '10', '4', '2008-03-06', '3800', '', '-8', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('24', '10', '4', '2008-03-06', '2664', '', '-27', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('25', '13', '5', '2008-03-06', '4010', '', '80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('26', '13', '5', '2008-03-06', '1420', '', '-80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('27', '10', '5', '2008-03-06', '3000', '', '-100', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('28', '10', '5', '2008-03-06', '1500', '', '135', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('29', '10', '5', '2008-03-06', '3800', '', '-8', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('30', '10', '5', '2008-03-06', '2664', '', '-27', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('31', '11', '1', '2008-03-06', '4010', '', '-80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('32', '11', '1', '2008-03-06', '1420', '', '80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('34', '11', '1', '2008-03-06', '1500', '', '-10', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('35', '11', '1', '2008-03-06', '3800', '', '8', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('36', '11', '1', '2008-03-06', '2664', '', '2', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('37', '13', '6', '2008-03-06', '4010', '', '80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('38', '13', '6', '2008-03-06', '1420', '', '-80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('39', '10', '6', '2008-03-06', '3000', '', '-100', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('40', '10', '6', '2008-03-06', '1500', '', '135', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('41', '10', '6', '2008-03-06', '3800', '', '-8', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('42', '10', '6', '2008-03-06', '2664', '', '-27', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('43', '13', '7', '2008-03-06', '4010', '', '80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('44', '13', '7', '2008-03-06', '1420', '', '-80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('45', '12', '7', '2008-03-06', '1700', '', '100', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('46', '12', '7', '2008-03-06', '1500', '', '-100', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('47', '12', '8', '2008-03-06', '1700', '', '100', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('48', '12', '8', '2008-03-06', '1500', '', '-100', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('49', '20', '7', '2008-03-06', '2630', '', '-550', '0', '0', '3', '1');
INSERT INTO `0_gl_trans` VALUES ('50', '20', '7', '2008-03-06', '1420', '', '250', '0', '0', '3', '1');
INSERT INTO `0_gl_trans` VALUES ('51', '20', '7', '2008-03-06', '1420', '', '300', '0', '0', '3', '1');
INSERT INTO `0_gl_trans` VALUES ('52', '11', '2', '2008-03-06', '4010', '', '-80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('53', '11', '2', '2008-03-06', '1420', '', '80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('54', '11', '2', '2008-03-06', '3000', '', '125', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('55', '11', '2', '2008-03-06', '1500', '', '-135', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('56', '11', '2', '2008-03-06', '3800', '', '8', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('57', '11', '2', '2008-03-06', '2664', '', '2', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('58', '11', '3', '2008-03-06', '4010', '', '-80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('59', '11', '3', '2008-03-06', '1420', '', '80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('60', '11', '3', '2008-03-06', '3000', '', '100', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('61', '11', '3', '2008-03-06', '1500', '', '-135', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('62', '11', '3', '2008-03-06', '3800', '', '8', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('63', '11', '3', '2008-03-06', '2664', '', '27', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('64', '12', '9', '2008-03-07', '1700', '', '2000', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('65', '12', '9', '2008-03-07', '1500', '', '-2000', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('66', '11', '4', '2008-03-07', '3000', '', '100', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('67', '11', '4', '2008-03-07', '1500', '', '-125', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('68', '11', '4', '2008-03-07', '2664', '', '25', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('69', '26', '7', '2008-03-07', '1420', '', '-80', '0', '0', '0', '');
INSERT INTO `0_gl_trans` VALUES ('70', '26', '7', '2008-03-07', '1420', '', '-20', '0', '0', '0', '');
INSERT INTO `0_gl_trans` VALUES ('71', '26', '7', '2008-03-07', '1420', '', '-18', '0', '0', '0', '');
INSERT INTO `0_gl_trans` VALUES ('72', '26', '7', '2008-03-07', '1420', '', '118', '0', '0', '0', '');
INSERT INTO `0_gl_trans` VALUES ('73', '0', '18', '2008-03-07', '1400', '', '20', '0', '0', '0', '');
INSERT INTO `0_gl_trans` VALUES ('74', '0', '18', '2008-03-07', '3010', '', '-20', '0', '0', '0', '');
INSERT INTO `0_gl_trans` VALUES ('75', '13', '8', '2008-03-07', '4010', '', '80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('76', '13', '8', '2008-03-07', '1420', '', '-80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('77', '13', '9', '2008-03-08', '4010', '', '80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('78', '13', '9', '2008-03-08', '1420', '', '-80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('79', '10', '7', '2008-03-08', '3000', '', '-100', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('80', '10', '7', '2008-03-08', '1500', '', '135', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('81', '10', '7', '2008-03-08', '3800', '', '-8', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('82', '10', '7', '2008-03-08', '2664', '', '-27', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('83', '13', '10', '2008-03-09', '4010', '', '80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('84', '13', '10', '2008-03-09', '1420', '', '-80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('85', '10', '8', '2008-03-09', '3000', '', '-100', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('86', '10', '8', '2008-03-09', '1500', '', '137.5', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('87', '10', '8', '2008-03-09', '3800', '', '-10', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('88', '10', '8', '2008-03-09', '2664', '', '-27.5', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('89', '13', '11', '2008-03-09', '4010', '', '80', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('90', '13', '11', '2008-03-09', '1420', '', '-80', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('91', '10', '9', '2008-03-09', '3000', '', '-100', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('92', '10', '9', '2008-03-09', '1500', '', '137.5', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('93', '10', '9', '2008-03-09', '3800', '', '-10', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('94', '10', '9', '2008-03-09', '2664', '', '-27.5', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('95', '13', '12', '2008-03-09', '4010', '', '80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('96', '13', '12', '2008-03-09', '1420', '', '-80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('97', '10', '10', '2008-03-09', '3000', '', '-100', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('98', '10', '10', '2008-03-09', '1500', '', '135', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('99', '10', '10', '2008-03-09', '3800', '', '-8', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('100', '10', '10', '2008-03-09', '2664', '', '-27', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('103', '13', '13', '2008-03-10', '4010', '', '20', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('104', '13', '13', '2008-03-10', '1420', '', '-20', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('105', '13', '14', '2008-03-10', '4010', '', '80', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('106', '13', '14', '2008-03-10', '1420', '', '-80', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('107', '10', '11', '2008-03-10', '3000', '', '-100', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('108', '10', '11', '2008-03-10', '1500', '', '125', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('109', '10', '11', '2008-03-10', '2664', '', '-25', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('110', '13', '15', '2008-03-10', '4010', '', '80', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('111', '13', '15', '2008-03-10', '1420', '', '-80', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('112', '10', '12', '2008-03-10', '3000', '', '-100', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('113', '10', '12', '2008-03-10', '1500', '', '125', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('114', '10', '12', '2008-03-10', '2664', '', '-25', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('115', '10', '13', '2008-03-07', '3000', '', '-100', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('116', '10', '13', '2008-03-07', '1500', '', '135', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('117', '10', '13', '2008-03-07', '3800', '', '-8', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('118', '10', '13', '2008-03-07', '2664', '', '-27', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('119', '13', '16', '2008-03-10', '4010', '', '80', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('120', '13', '16', '2008-03-10', '1420', '', '-80', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('121', '13', '17', '2008-03-10', '4010', '', '80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('122', '13', '17', '2008-03-10', '1420', '', '-80', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('123', '10', '14', '2008-03-10', '3000', '', '-100', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('124', '10', '14', '2008-03-10', '1500', '', '135', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('125', '10', '14', '2008-03-10', '3800', '', '-8', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('126', '10', '14', '2008-03-10', '2664', '', '-27', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('127', '10', '15', '2008-03-10', '3000', '', '-32', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('128', '10', '15', '2008-03-10', '1500', '', '40', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('129', '10', '15', '2008-03-10', '2664', '', '-8', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('133', '20', '8', '2008-03-20', '2630', '', '0', '0', '0', '3', '2');
INSERT INTO `0_gl_trans` VALUES ('134', '20', '8', '2008-03-20', '1420', '', '0', '0', '0', '3', '2');
INSERT INTO `0_gl_trans` VALUES ('135', '20', '9', '2008-03-25', '2630', '', '-19.82', '0', '0', '3', '2');
INSERT INTO `0_gl_trans` VALUES ('136', '20', '9', '2008-03-25', '1420', '', '19.82', '0', '0', '3', '2');
INSERT INTO `0_gl_trans` VALUES ('149', '20', '10', '2008-03-25', '2630', '', '-40.48', '0', '0', '3', '4');
INSERT INTO `0_gl_trans` VALUES ('150', '20', '10', '2008-03-25', '1420', '', '40.48', '0', '0', '3', '4');
INSERT INTO `0_gl_trans` VALUES ('152', '20', '11', '2008-03-28', '2630', '', '-2618.19', '0', '0', '3', '4');
INSERT INTO `0_gl_trans` VALUES ('153', '20', '11', '2008-03-28', '1420', '', '2094.55', '0', '0', '3', '4');
INSERT INTO `0_gl_trans` VALUES ('154', '20', '11', '2008-03-28', '2682', '', '523.64', '0', '0', '3', '4');
INSERT INTO `0_gl_trans` VALUES ('155', '13', '18', '2008-03-17', '4010', '', '5.0599509174312', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('156', '13', '18', '2008-03-17', '1420', '', '-5.0599509174312', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('157', '10', '16', '2008-03-17', '3000', '', '-100', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('158', '10', '16', '2008-03-17', '1500', '', '135', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('159', '10', '16', '2008-03-17', '3800', '', '-8', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('160', '10', '16', '2008-03-17', '2664', '', '-27', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('161', '10', '17', '2008-03-10', '3000', '', '-2.26', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('162', '10', '17', '2008-03-10', '1500', '', '2.26', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('163', '13', '19', '2008-03-29', '4010', '', '-3.4114285714283', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('164', '13', '19', '2008-03-29', '1420', '', '3.4114285714283', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('165', '13', '20', '2008-03-29', '4010', '', '-3.4114285714283', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('166', '13', '20', '2008-03-29', '1420', '', '3.4114285714283', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('167', '10', '19', '2008-03-29', '3000', '', '-100', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('168', '10', '19', '2008-03-29', '1500', '', '125', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('169', '10', '19', '2008-03-29', '2664', '', '-25', '0', '0', '2', '4');
INSERT INTO `0_gl_trans` VALUES ('170', '12', '10', '2009-01-10', '1700', '', '1000', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('171', '12', '10', '2009-01-10', '1500', '', '-1000', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('172', '12', '11', '2009-01-10', '1700', '', '183.6', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('173', '12', '11', '2009-01-10', '1500', '', '-183.6', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('174', '12', '11', '2009-01-10', '1500', 'Sales Invoice 17', '-0.24', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('175', '12', '11', '2009-01-10', '4260', 'Sales Invoice 17', '0.24', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('176', '10', '20', '2009-01-10', '3000', '', '-30', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('177', '10', '20', '2009-01-10', '1500', '', '37.5', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('178', '10', '20', '2009-01-10', '2664', '', '-7.5', '0', '0', '2', '1');
INSERT INTO `0_gl_trans` VALUES ('179', '35', '6', '2009-01-10', '4210', 'Cost was -3.4114285714283 changed to 10 x quantity on hand of 12', '-160.94', '0', '0', '0', '');
INSERT INTO `0_gl_trans` VALUES ('180', '35', '6', '2009-01-10', '1420', 'Cost was -3.4114285714283 changed to 10 x quantity on hand of 12', '160.94', '0', '0', '0', '');
INSERT INTO `0_gl_trans` VALUES ('181', '20', '12', '2009-01-10', '2630', '', '-775', '0', '0', '3', '1');
INSERT INTO `0_gl_trans` VALUES ('182', '20', '12', '2009-01-10', '1420', '', '200', '0', '0', '3', '1');
INSERT INTO `0_gl_trans` VALUES ('183', '20', '12', '2009-01-10', '1420', '', '500', '0', '0', '3', '1');
INSERT INTO `0_gl_trans` VALUES ('184', '20', '12', '2009-01-10', '2680', '', '25', '0', '0', '3', '1');
INSERT INTO `0_gl_trans` VALUES ('185', '20', '12', '2009-01-10', '2682', '', '50', '0', '0', '3', '1');
INSERT INTO `0_gl_trans` VALUES ('186', '22', '4', '2009-01-10', '2630', '', '200', '0', '0', '3', '1');
INSERT INTO `0_gl_trans` VALUES ('187', '22', '4', '2009-01-10', '1700', '', '-200', '0', '0', '3', '1');
INSERT INTO `0_gl_trans` VALUES ('188', '10', '21', '2009-01-29', '3000', '', '-14.07', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('189', '10', '21', '2009-01-29', '1500', '', '17.59', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('190', '10', '21', '2009-01-29', '2664', '', '-3.52', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('191', '10', '22', '2009-01-29', '3000', '', '-14.07', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('192', '10', '22', '2009-01-29', '1500', '', '17.59', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('193', '10', '22', '2009-01-29', '2664', '', '-3.52', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('194', '10', '23', '2009-01-29', '3000', '', '-14.07', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('195', '10', '23', '2009-01-29', '1500', '', '17.59', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('196', '10', '23', '2009-01-29', '2664', '', '-3.52', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('197', '10', '24', '2009-01-29', '3000', '', '-14.07', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('198', '10', '24', '2009-01-29', '1500', '', '17.59', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('199', '10', '24', '2009-01-29', '2664', '', '-3.52', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('200', '10', '25', '2009-01-29', '3000', '', '-17.59', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('201', '10', '25', '2009-01-29', '1500', '', '17.59', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('202', '10', '25', '2009-01-29', '1430', '', '0', '0', '0', '2', '3');
INSERT INTO `0_gl_trans` VALUES ('203', '2', '5', '2009-01-29', '2664', 'Cash Sales', '-40', '0', '0', '4', '3');
INSERT INTO `0_gl_trans` VALUES ('204', '2', '5', '2009-01-29', '3000', 'Cash Sales', '-160', '0', '0', '4', '3');
INSERT INTO `0_gl_trans` VALUES ('205', '2', '5', '2009-01-29', '1700', '', '200', '0', '0', '4', '3');
INSERT INTO `0_gl_trans` VALUES ('206', '1', '8', '2009-01-29', '2682', 'Maintenance', '10', '0', '0', '4', '1');
INSERT INTO `0_gl_trans` VALUES ('207', '1', '8', '2009-01-29', '6600', 'Maintenance', '40', '0', '0', '4', '1');
INSERT INTO `0_gl_trans` VALUES ('208', '1', '8', '2009-01-29', '1700', '', '-50', '0', '0', '4', '1');
INSERT INTO `0_gl_trans` VALUES ('209', '20', '13', '2009-01-29', '2630', '', '-50', '0', '0', '3', '1');
INSERT INTO `0_gl_trans` VALUES ('210', '20', '13', '2009-01-29', '2682', '', '10', '0', '0', '3', '1');
INSERT INTO `0_gl_trans` VALUES ('211', '20', '13', '2009-01-29', '6730', '', '40', '0', '0', '3', '1');
INSERT INTO `0_gl_trans` VALUES ('212', '2', '6', '2009-01-29', '2664', 'Cash Sales', '-10', '0', '0', '4', '3');
INSERT INTO `0_gl_trans` VALUES ('213', '2', '6', '2009-01-29', '3000', 'Cash Sales', '-40', '0', '0', '4', '3');
INSERT INTO `0_gl_trans` VALUES ('214', '2', '6', '2009-01-29', '1700', '', '50', '0', '0', '4', '3');


### Structure of table `0_grn_batch` ###

DROP TABLE IF EXISTS `0_grn_batch`;

CREATE TABLE `0_grn_batch` (
  `id` int(11) NOT NULL auto_increment,
  `supplier_id` int(11) NOT NULL default '0',
  `purch_order_no` int(11) default NULL,
  `reference` varchar(60) NOT NULL default '',
  `delivery_date` date NOT NULL default '0000-00-00',
  `loc_code` varchar(5) default NULL,
  PRIMARY KEY  (`id`)
) TYPE=InnoDB AUTO_INCREMENT=14  AUTO_INCREMENT=14 ;


### Data of table `0_grn_batch` ###

INSERT INTO `0_grn_batch` VALUES ('1', '1', '1', '1', '2006-01-18', 'DEF');
INSERT INTO `0_grn_batch` VALUES ('2', '1', '2', '2', '2006-01-18', 'DEF');
INSERT INTO `0_grn_batch` VALUES ('3', '1', '5', '3', '2006-01-18', 'DEF');
INSERT INTO `0_grn_batch` VALUES ('4', '1', '6', '4', '2008-03-06', 'DEF');
INSERT INTO `0_grn_batch` VALUES ('5', '4', '8', '5', '2008-03-17', 'DEF');
INSERT INTO `0_grn_batch` VALUES ('6', '4', '8', '6', '2008-03-19', 'DEF');
INSERT INTO `0_grn_batch` VALUES ('7', '2', '9', '7', '2008-03-20', 'DEF');
INSERT INTO `0_grn_batch` VALUES ('8', '2', '10', '8', '2008-03-25', 'DEF');
INSERT INTO `0_grn_batch` VALUES ('9', '4', '11', '9', '2008-03-25', 'DEF');
INSERT INTO `0_grn_batch` VALUES ('10', '4', '13', '10', '2008-03-25', 'DEF');
INSERT INTO `0_grn_batch` VALUES ('11', '4', '12', '11', '2008-03-25', 'DEF');
INSERT INTO `0_grn_batch` VALUES ('12', '4', '13', '12', '2008-03-28', 'DEF');
INSERT INTO `0_grn_batch` VALUES ('13', '1', '14', '13', '2009-01-10', 'CWA');


### Structure of table `0_grn_items` ###

DROP TABLE IF EXISTS `0_grn_items`;

CREATE TABLE `0_grn_items` (
  `id` int(11) NOT NULL auto_increment,
  `grn_batch_id` int(11) default NULL,
  `po_detail_item` int(11) NOT NULL default '0',
  `item_code` varchar(20) NOT NULL default '',
  `description` tinytext,
  `qty_recd` double NOT NULL default '0',
  `quantity_inv` double NOT NULL default '0',
  PRIMARY KEY  (`id`)
) TYPE=InnoDB AUTO_INCREMENT=18  AUTO_INCREMENT=18 ;


### Data of table `0_grn_items` ###

INSERT INTO `0_grn_items` VALUES ('1', '1', '1', '102', '17 inch VGA Monitor', '10', '10');
INSERT INTO `0_grn_items` VALUES ('2', '1', '2', '103', '32MB VGA Card', '50', '50');
INSERT INTO `0_grn_items` VALUES ('3', '2', '3', '104', '52x CD Drive', '1', '1');
INSERT INTO `0_grn_items` VALUES ('4', '3', '6', '104', '52x CD Drive (upgraded)', '3020', '302');
INSERT INTO `0_grn_items` VALUES ('5', '4', '7', '102', '17 inch VGA Monitor', '10', '10');
INSERT INTO `0_grn_items` VALUES ('6', '4', '8', '103', '32MB VGA Card', '10', '10');
INSERT INTO `0_grn_items` VALUES ('7', '4', '9', '104', '52x CD Drive', '10', '10');
INSERT INTO `0_grn_items` VALUES ('8', '4', '10', '202', 'Electric stimulator', '10', '10');
INSERT INTO `0_grn_items` VALUES ('9', '5', '12', 'AA101', 'olie 5w40', '8', '8');
INSERT INTO `0_grn_items` VALUES ('10', '6', '12', 'AA101', 'olie 5w40', '408', '408');
INSERT INTO `0_grn_items` VALUES ('11', '7', '13', '102', '17 inch VGA Monitor', '5', '5');
INSERT INTO `0_grn_items` VALUES ('12', '8', '14', '102', '17 inch VGA Monitor', '1', '1');
INSERT INTO `0_grn_items` VALUES ('13', '9', '15', 'AA101', 'olie 5w40', '20', '0');
INSERT INTO `0_grn_items` VALUES ('14', '10', '17', '102', '17 inch VGA Monitor', '5', '0');
INSERT INTO `0_grn_items` VALUES ('15', '11', '16', '102', '17 inch VGA Monitor', '1', '0');
INSERT INTO `0_grn_items` VALUES ('16', '12', '17', '102', '17 inch VGA Monitor', '5', '0');
INSERT INTO `0_grn_items` VALUES ('17', '13', '18', '102', '17 inch VGA Monitor', '1', '0');


### Structure of table `0_groups` ###

DROP TABLE IF EXISTS `0_groups`;

CREATE TABLE `0_groups` (
  `id` smallint(6) unsigned NOT NULL auto_increment,
  `description` varchar(60) NOT NULL default '',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `description` (`description`)
) TYPE=MyISAM AUTO_INCREMENT=4  AUTO_INCREMENT=4 ;


### Data of table `0_groups` ###

INSERT INTO `0_groups` VALUES ('1', 'Small', '0');
INSERT INTO `0_groups` VALUES ('2', 'Medium', '0');
INSERT INTO `0_groups` VALUES ('3', 'Large', '0');


### Structure of table `0_item_codes` ###

DROP TABLE IF EXISTS `0_item_codes`;

CREATE TABLE `0_item_codes` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `item_code` varchar(20) NOT NULL,
  `stock_id` varchar(20) NOT NULL,
  `description` varchar(200) NOT NULL default '',
  `category_id` smallint(6) unsigned NOT NULL,
  `quantity` double NOT NULL default '1',
  `is_foreign` tinyint(1) NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `stock_id` (`stock_id`,`item_code`)
) TYPE=MyISAM AUTO_INCREMENT=8  AUTO_INCREMENT=8 ;


### Data of table `0_item_codes` ###

INSERT INTO `0_item_codes` VALUES ('1', '102', '102', '17 inch VGA Monitor', '1', '1', '0', '0');
INSERT INTO `0_item_codes` VALUES ('2', '103', '103', '32MB VGA Card', '1', '1', '0', '0');
INSERT INTO `0_item_codes` VALUES ('3', '104', '104', '52x CD Drive', '1', '1', '0', '0');
INSERT INTO `0_item_codes` VALUES ('4', '201', '201', 'Assembly Labour', '2', '1', '0', '0');
INSERT INTO `0_item_codes` VALUES ('5', '202', '202', 'Electric stimulator', '1', '1', '0', '0');
INSERT INTO `0_item_codes` VALUES ('6', '3400', '3400', 'P4 Business System', '3', '1', '0', '0');
INSERT INTO `0_item_codes` VALUES ('7', 'AA101', 'AA101', 'olie 5w40', '1', '1', '0', '0');


### Structure of table `0_item_tax_type_exemptions` ###

DROP TABLE IF EXISTS `0_item_tax_type_exemptions`;

CREATE TABLE `0_item_tax_type_exemptions` (
  `item_tax_type_id` int(11) NOT NULL default '0',
  `tax_type_id` int(11) NOT NULL default '0',
  PRIMARY KEY  (`item_tax_type_id`,`tax_type_id`)
) TYPE=InnoDB  ;


### Data of table `0_item_tax_type_exemptions` ###

INSERT INTO `0_item_tax_type_exemptions` VALUES ('1', '1');
INSERT INTO `0_item_tax_type_exemptions` VALUES ('1', '2');
INSERT INTO `0_item_tax_type_exemptions` VALUES ('1', '4');
INSERT INTO `0_item_tax_type_exemptions` VALUES ('2', '2');
INSERT INTO `0_item_tax_type_exemptions` VALUES ('2', '3');


### Structure of table `0_item_tax_types` ###

DROP TABLE IF EXISTS `0_item_tax_types`;

CREATE TABLE `0_item_tax_types` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(60) [BNOT NULL default '',
  `exempt` tinyint(1) NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) TYPE=InnoDB AUTO_INCREMENT=3  AUTO_INCREMENT=3 ;


### Data of table `0_item_tax_types` ###

INSERT INTO `0_item_tax_types` VALUES ('1', 'Regular', '0');
INSERT INTO `0_item_tax_types` VALUES ('2', 'Recovery equipment', '0', '0');


### Structure of table `0_item_units` ###

DROP TABLE IF EXISTS `0_item_units`;

CREATE TABLE `0_item_units` (
  `abbr` varchar(20) NOT NULL default '',
  `name` varchar(40) NOT NULL default '',
  `decimals` tinyint(2) NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`abbr`),
  UNIQUE KEY `name` (`name`)
) TYPE=MyISAM  COMMENT='units of measure' ;


### Data of table `0_item_units` ###

INSERT INTO `0_item_units` VALUES ('each', 'each', '0', '0');
INSERT INTO `0_item_units` VALUES ('m', 'meters', '2', '0');
INSERT INTO `0_item_units` VALUES ('kgg', 'kilograms', '3', '0');
INSERT INTO `0_item_units` VALUES ('tons', 'tons', '2', '0');
INSERT INTO `0_item_units` VALUES ('lbs', 'pounds', '2', '0');
INSERT INTO `0_item_units` VALUES ('l', 'liters', '3', '0');
INSERT INTO `0_item_units` VALUES ('dozen', 'dozens', '0', '0');
INSERT INTO `0_item_units` VALUES ('pack', 'packs', '0', '0');
INSERT INTO `0_item_units` VALUES ('hrs', 'hours', '1', '0');
INSERT INTO `0_item_units` VALUES ('dz', 'dozijn', '0', '0');


### Structure of table `0_loc_stock` ###

DROP TABLE IF EXISTS `0_loc_stock`;

CREATE TABLE `0_loc_stock` (
  `loc_code` char(5) NOT NULL default '',
  `stock_id` char(20) NOT NULL default '',
  `reorder_level` bigint(20) NOT NULL default '0',
  PRIMARY KEY  (`loc_code`,`stock_id`),
  KEY `stock_id` (`stock_id`)
) TYPE=InnoDB  ;


### Data of table `0_loc_stock` ###

INSERT INTO `0_loc_stock` VALUES ('CWA', '102', '0');
INSERT INTO `0_loc_stock` VALUES ('CWA', '103', '0');
INSERT INTO `0_loc_stock` VALUES ('CWA', '104', '0');
INSERT INTO `0_loc_stock` VALUES ('CWA', '201', '0');
INSERT INTO `0_loc_stock` VALUES ('CWA', '202', '0');
INSERT INTO `0_loc_stock` VALUES ('CWA', '3400', '0');
INSERT INTO `0_loc_stock` VALUES ('CWA', 'AA101', '0');
INSERT INTO `0_loc_stock` VALUES ('DEF', '102', '0');
INSERT INTO `0_loc_stock` VALUES ('DEF', '103', '0');
INSERT INTO `0_loc_stock` VALUES ('DEF', '104', '0');
INSERT INTO `0_loc_stock` VALUES ('DEF', '201', '0');
INSERT INTO `0_loc_stock` VALUES ('DEF', '202', '0');
INSERT INTO `0_loc_stock` VALUES ('DEF', '3400', '0');
INSERT INTO `0_loc_stock` VALUES ('DEF', 'AA101', '0');


### Structure of table `0_locations` ###

DROP TABLE IF EXISTS `0_locations`;

CREATE TABLE `0_locations` (
  `loc_code` varchar(5) NOT NULL default '',
  `location_name` varchar(60) NOT NULL default '',
  `delivery_address` tinytext NOT NULL,
  `phone` varchar(30) NOT NULL default '',
  `fax` varchar(30) NOT NULL default '',
  `email` varchar(100) NOT NULL default '',
  `contact` varchar(30) NOT NULL default '',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`loc_code`)
) TYPE=MyISAM  ;


### Data of table `0_locations` ###

INSERT INTO `0_locations` VALUES ('DEF', 'Default', 'N/A', '', '', '', '', '0');
INSERT INTO `0_locations` VALUES ('CWA', 'Cool Warehouse', '', '', '', '', '', '0');


### Structure of table `0_movement_types` ###

DROP TABLE IF EXISTS `0_movement_types`;

CREATE TABLE `0_movement_types` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(60) NOT NULL default '',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) TYPE=MyISAM AUTO_INCREMENT=2  AUTO_INCREMENT=2 ;


### Data of table `0_movement_types` ###

INSERT INTO `0_movement_types` VALUES ('1', 'Adjustment', '0');


### Structure of table `0_payment_terms` ###

DROP TABLE IF EXISTS `0_payment_terms`;

CREATE TABLE `0_payment_terms` (
  `terms_indicator` int(11) NOT NULL auto_increment,
  `terms` char(80) NOT NULL default '',
  `days_before_due` smallint(6) NOT NULL default '0',
  `day_in_following_month` smallint(6) NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`terms_indicator`),
  UNIQUE KEY `terms` (`terms`)
) TYPE=MyISAM AUTO_INCREMENT=5  AUTO_INCREMENT=5 ;


### Data of table `0_payment_terms` ###

INSERT INTO `0_payment_terms` VALUES ('1', 'Due 15th Of the Following Month', '0', '17', '0');
INSERT INTO `0_payment_terms` VALUES ('2', 'Due By End Of The Following Month', '0', '30', '0');
INSERT INTO `0_payment_terms` VALUES ('3', 'Payment due within 10 days', '10', '0', '0');
INSERT INTO `0_payment_terms` VALUES ('4', 'Cash Only', '1', '0', '0');


### Structure of table `0_prices` ###

DROP TABLE IF EXISTS `0_prices`;

CREATE TABLE `0_prices` (
  `id` int(11) NOT NULL auto_increment,
  `stock_id` varchar(20) NOT NULL default '',
  `sales_type_id` int(11) NOT NULL default '0',
  `curr_abrev` char(3) NOT NULL default '',
  `price` double NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `price` (`stock_id`,`sales_type_id`,`curr_abrev`)
) TYPE=MyISAM AUTO_INCREMENT=13  AUTO_INCREMENT=13 ;


### Data of table `0_prices` ###

INSERT INTO `0_prices` VALUES ('1', '102', '1', 'USD', '125');
INSERT INTO `0_prices` VALUES ('2', '103', '1', 'USD', '40');
INSERT INTO `0_prices` VALUES ('3', '104', '1', 'USD', '34');
INSERT INTO `0_prices` VALUES ('4', '201', '1', 'USD', '40');
INSERT INTO `0_prices` VALUES ('5', '3400', '1', 'USD', '600');
INSERT INTO `0_prices` VALUES ('6', '102', '2', 'USD', '100');
INSERT INTO `0_prices` VALUES ('7', '202', '2', 'USD', '50');
INSERT INTO `0_prices` VALUES ('8', '202', '1', 'USD', '52.5');
INSERT INTO `0_prices` VALUES ('9', '103', '2', 'USD', '30');
INSERT INTO `0_prices` VALUES ('10', '104', '2', 'USD', '25');
INSERT INTO `0_prices` VALUES ('11', '201', '2', 'USD', '30');
INSERT INTO `0_prices` VALUES ('12', '3400', '2', 'USD', '450');


### Structure of table `0_print_profiles` ###

DROP TABLE IF EXISTS `0_print_profiles`;

CREATE TABLE `0_print_profiles` (
  `id` smallint(6) unsigned NOT NULL auto_increment,
  `profile` varchar(30) NOT NULL,
  `report` varchar(5) default NULL,
  `printer` tinyint(3) unsigned default NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `profile` (`profile`,`report`)
) TYPE=MyISAM AUTO_INCREMENT=10  AUTO_INCREMENT=10 ;


### Data of table `0_print_profiles` ###

INSERT INTO `0_print_profiles` VALUES ('1', 'Out of office', '', '0');
INSERT INTO `0_print_profiles` VALUES ('2', 'Sales Department', '', '0');
INSERT INTO `0_print_profiles` VALUES ('3', 'Central', '', '2');
INSERT INTO `0_print_profiles` VALUES ('4', 'Sales Department', '104', '2');
INSERT INTO `0_print_profiles` VALUES ('5', 'Sales Department', '105', '2');
INSERT INTO `0_print_profiles` VALUES ('6', 'Sales Department', '107', '2');
INSERT INTO `0_print_profiles` VALUES ('7', 'Sales Department', '109', '2');
INSERT INTO `0_print_profiles` VALUES ('8', 'Sales Department', '110', '2');
INSERT INTO `0_print_profiles` VALUES ('9', 'Sales Department', '201', '2');


### Structure of table `0_printers` ###

DROP TABLE IF EXISTS `0_printers`;

CREATE TABLE `0_printers` (
  `id` tinyint(3) unsigned NOT NULL auto_increment,
  `name` varchar(20) NOT NULL,
  `description` varchar(60) NOT NULL,
  `queue` varchar(20) NOT NULL,
  `host` varchar(40) NOT NULL,
  `port` smallint(11) unsigned NOT NULL,
  `timeout` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) TYPE=MyISAM AUTO_INCREMENT=4  AUTO_INCREMENT=4 ;


### Data of table `0_printers` ###

INSERT INTO `0_printers` VALUES ('1', 'QL500', 'Label printer', 'QL500', 'server', '127', '20');
INSERT INTO `0_printers` VALUES ('2', 'Samsung', 'Main network printer', 'scx4521F', 'server', '515', '5');
INSERT INTO `0_printers` VALUES ('3', 'Local', 'Local print server at user IP', 'lp', '', '515', '10');


### Structure of table `0_purch_data` ###

DROP TABLE IF EXISTS `0_purch_data`;

CREATE TABLE `0_purch_data` (
  `supplier_id` int(11) NOT NULL default '0',
  `stock_id` char(20) NOT NULL default '',
  `price` double NOT NULL default '0',
  `suppliers_uom` char(50) NOT NULL default '',
  `conversion_factor` double NOT NULL default '1',
  `supplier_description` char(50) NOT NULL default '',
  PRIMARY KEY  (`supplier_id`,`stock_id`)
) TYPE=MyISAM  ;


### Data of table `0_purch_data` ###

INSERT INTO `0_purch_data` VALUES ('1', '102', '10', '', '1', '17 inch VGA Monitor');
INSERT INTO `0_purch_data` VALUES ('1', '104', '20', '', '1', '');
INSERT INTO `0_purch_data` VALUES ('1', '202', '50', '', '1', '');


### Structure of table `0_purch_order_details` ###

DROP TABLE IF EXISTS `0_purch_order_details`;

CREATE TABLE `0_purch_order_details` (
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
  PRIMARY KEY  (`po_detail_item`)
) TYPE=InnoDB AUTO_INCREMENT=19  AUTO_INCREMENT=19 ;


### Data of table `0_purch_order_details` ###

INSERT INTO `0_purch_order_details` VALUES ('1', '1', '102', '17 inch VGA Monitor', '2006-01-28', '10', '3020', '3020', '0', '3000', '10');
INSERT INTO `0_purch_order_details` VALUES ('2', '1', '103', '32MB VGA Card', '2006-01-28', '50', '90', '90', '0', '300', '50');
INSERT INTO `0_purch_order_details` VALUES ('3', '2', '104', '52x CD Drive', '2006-01-28', '1', '26', '26', '0', '1', '1');
INSERT INTO `0_purch_order_details` VALUES ('4', '3', '104', '52x CD Drive', '2006-01-28', '0', '22', '0', '0', '1', '0');
INSERT INTO `0_purch_order_details` VALUES ('6', '5', '104', '52x CD Drive', '2006-01-28', '302', '22', '22', '0', '330', '3020');
INSERT INTO `0_purch_order_details` VALUES ('7', '6', '102', '17 inch VGA Monitor', '2008-03-16', '10', '25', '25', '80', '10', '10');
INSERT INTO `0_purch_order_details` VALUES ('8', '6', '103', '32MB VGA Card', '2008-03-16', '10', '30', '30', '20', '10', '10');
INSERT INTO `0_purch_order_details` VALUES ('9', '6', '104', '52x CD Drive', '2008-03-16', '10', '20', '20', '18', '10', '10');
INSERT INTO `0_purch_order_details` VALUES ('10', '6', '202', 'Electric stimulator', '2008-03-16', '10', '50', '50', '30', '10', '10');
INSERT INTO `0_purch_order_details` VALUES ('12', '8', 'AA101', 'olie 5w40', '2008-03-25', '416', '3.25', '3.25', '0', '416', '416');
INSERT INTO `0_purch_order_details` VALUES ('13', '9', '102', '17 inch VGA Monitor', '2008-03-30', '5', '0', '0', '80', '5', '5');
INSERT INTO `0_purch_order_details` VALUES ('14', '10', '102', '17 inch VGA Monitor', '2008-04-04', '1', '10', '10', '-120', '1', '1');
INSERT INTO `0_purch_order_details` VALUES ('15', '11', 'AA101', 'olie 5w40', '2008-04-04', '0', '3.25', '0', '5.0599485576923', '20', '20');
INSERT INTO `0_purch_order_details` VALUES ('16', '12', '102', '17 inch VGA Monitor', '2008-04-04', '0', '10', '0', '-17.79125', '1', '1');
INSERT INTO `0_purch_order_details` VALUES ('17', '13', '102', '17 inch VGA Monitor', '2008-04-04', '0', '10', '0', '-73.393333333333', '10', '10');
INSERT INTO `0_purch_order_details` VALUES ('18', '14', '102', '17 inch VGA Monitor', '2009-01-20', '0', '10', '10', '10', '1', '1');


### Structure of table `0_purch_orders` ###

DROP TABLE IF EXISTS `0_purch_orders`;

CREATE TABLE `0_purch_orders` (
  `order_no` int(11) NOT NULL auto_increment,
  `version` tinyint(1) unsigned NOT NULL default '0',
  `supplier_id` int(11) NOT NULL default '0',
  `comments` tinytext,
  `ord_date` date NOT NULL default '0000-00-00',
  `reference` tinytext NOT NULL,
  `requisition_no` tinytext,
  `into_stock_location` varchar(5) NOT NULL default '',
  `delivery_address` tinytext NOT NULL,
  PRIMARY KEY  (`order_no`)
) TYPE=InnoDB AUTO_INCREMENT=15  AUTO_INCREMENT=15 ;


### Data of table `0_purch_orders` ###

INSERT INTO `0_purch_orders` VALUES ('1', '0', '1', '', '2006-01-18', '3', '333', 'DEF', 'N/A');
INSERT INTO `0_purch_orders` VALUES ('2', '0', '1', '', '2006-01-18', '4', '44', 'DEF', 'N/A');
INSERT INTO `0_purch_orders` VALUES ('3', '0', '1', '', '2006-01-18', '5', '', 'DEF', 'N/A');
INSERT INTO `0_purch_orders` VALUES ('5', '0', '1', '', '2006-01-18', '7', '', 'DEF', 'N/A');
INSERT INTO `0_purch_orders` VALUES ('6', '0', '1', '', '2008-03-06', '8', '', 'DEF', 'N/A');
INSERT INTO `0_purch_orders` VALUES ('8', '0', '4', '', '2008-03-15', '9', '', 'DEF', 'N/A');
INSERT INTO `0_purch_orders` VALUES ('9', '0', '2', '', '2008-03-20', '10', '', 'DEF', 'N/A');
INSERT INTO `0_purch_orders` VALUES ('10', '0', '2', '', '2008-03-25', '11', '', 'DEF', 'N/A');
INSERT INTO `0_purch_orders` VALUES ('11', '0', '4', '', '2008-03-25', '12', '', 'DEF', 'N/A');
INSERT INTO `0_purch_orders` VALUES ('12', '0', '4', '', '2008-03-25', '13', '', 'DEF', 'N/A');
INSERT INTO `0_purch_orders` VALUES ('13', '0', '4', '', '2008-03-25', '14', '', 'DEF', 'N/A');
INSERT INTO `0_purch_orders` VALUES ('14', '0', '1', '', '2009-01-10', '15', '', 'CWA', 'address');


### Structure of table `0_quick_entries` ###

DROP TABLE IF EXISTS `0_quick_entries`;

CREATE TABLE `0_quick_entries` (
  `id` smallint(6) unsigned NOT NULL auto_increment,
  `type` tinyint(1) NOT NULL default '0',
  `description` varchar(60) NOT NULL,
  `base_amount` double NOT NULL default '0',
  `base_desc` varchar(60) default NULL,
  PRIMARY KEY  (`id`),
  KEY `description` (`description`)
) TYPE=MyISAM AUTO_INCREMENT=4  AUTO_INCREMENT=4 ;


### Data of table `0_quick_entries` ###

INSERT INTO `0_quick_entries` VALUES ('1', '1', 'Maintenance', '0', 'Amount');
INSERT INTO `0_quick_entries` VALUES ('2', '4', 'Phone', '0', 'Amount');
INSERT INTO `0_quick_entries` VALUES ('3', '2', 'Cash Sales', '0', 'Amount');


### Structure of table `0_quick_entry_lines` ###

DROP TABLE IF EXISTS `0_quick_entry_lines`;

CREATE TABLE `0_quick_entry_lines` (
  `id` smallint(6) unsigned NOT NULL auto_increment,
  `qid` smallint(6) unsigned NOT NULL,
  `amount` double default '0',
  `action` varchar(2) NOT NULL,
  `dest_id` varchar(11) NOT NULL,
  `dimension_id` smallint(6) unsigned default NULL,
  `dimension2_id` smallint(6) unsigned default NULL,
  PRIMARY KEY  (`id`),
  KEY `qid` (`qid`)
) TYPE=MyISAM AUTO_INCREMENT=7  AUTO_INCREMENT=7 ;


### Data of table `0_quick_entry_lines` ###

INSERT INTO `0_quick_entry_lines` VALUES ('1', '1', '0', 't-', '1', '0', '0');
INSERT INTO `0_quick_entry_lines` VALUES ('2', '2', '0', 't-', '1', '0', '0');
INSERT INTO `0_quick_entry_lines` VALUES ('3', '3', '0', 't-', '1', '0', '0');
INSERT INTO `0_quick_entry_lines` VALUES ('4', '3', '0', '=', '3000', '0', '0');
INSERT INTO `0_quick_entry_lines` VALUES ('5', '1', '0', '=', '6600', '0', '0');
INSERT INTO `0_quick_entry_lines` VALUES ('6', '2', '0', '=', '6730', '0', '0');


### Structure of table `0_recurrent_invoices` ###

DROP TABLE IF EXISTS `0_recurrent_invoices`;

CREATE TABLE `0_recurrent_invoices` (
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
) TYPE=InnoDB  AUTO_INCREMENT=1 ;


### Data of table `0_recurrent_invoices` ###



### Structure of table `0_refs` ###

DROP TABLE IF EXISTS `0_refs`;

CREATE TABLE `0_refs` (
  `id` int(11) NOT NULL default '0',
  `type` int(11) NOT NULL default '0',
  `reference` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`id`,`type`)
) TYPE=InnoDB  ;


### Data of table `0_refs` ###

INSERT INTO `0_refs` VALUES ('2', '0', 'Joe');
INSERT INTO `0_refs` VALUES ('3', '0', '19');
INSERT INTO `0_refs` VALUES ('4', '0', '19');
INSERT INTO `0_refs` VALUES ('5', '0', '20');
INSERT INTO `0_refs` VALUES ('6', '0', '21');
INSERT INTO `0_refs` VALUES ('7', '0', '22');
INSERT INTO `0_refs` VALUES ('8', '0', '23');
INSERT INTO `0_refs` VALUES ('9', '0', '24');
INSERT INTO `0_refs` VALUES ('10', '0', '25');
INSERT INTO `0_refs` VALUES ('11', '0', '26');
INSERT INTO `0_refs` VALUES ('12', '0', '27');
INSERT INTO `0_refs` VALUES ('13', '0', '28');
INSERT INTO `0_refs` VALUES ('14', '0', '29');
INSERT INTO `0_refs` VALUES ('15', '0', '30');
INSERT INTO `0_refs` VALUES ('16', '0', '31');
INSERT INTO `0_refs` VALUES ('17', '0', '32');
INSERT INTO `0_refs` VALUES ('18', '0', '33');


### Structure of table `0_sales_order_details` ###

DROP TABLE IF EXISTS `0_sales_order_details`;

CREATE TABLE `0_sales_order_details` (
  `id` int(11) NOT NULL auto_increment,
  `order_no` int(11) NOT NULL default '0',
  `stk_code` varchar(20) NOT NULL default '',
  `description` tinytext,
  `qty_sent` double NOT NULL default '0',
  `unit_price` double NOT NULL default '0',
  `quantity` double NOT NULL default '0',
  `discount_percent` double NOT NULL default '0',
  PRIMARY KEY  (`id`)
) TYPE=InnoDB AUTO_INCREMENT=31  AUTO_INCREMENT=31 ;


### Data of table `0_sales_order_details` ###

INSERT INTO `0_sales_order_details` VALUES ('1', '1', '102', '17 inch VGA Monitor', '1', '125', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('2', '2', '102', '17 inch VGA Monitor', '1', '125', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('3', '3', '102', '17 inch VGA Monitor', '1', '125', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('4', '4', '102', '17 inch VGA Monitor', '1', '125', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('5', '5', '102', '17 inch VGA Monitor', '1', '125', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('6', '6', '102', '17 inch VGA Monitor', '1', '125', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('7', '7', '102', '17 inch VGA Monitor', '1', '125', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('8', '8', '102', '17 inch VGA Monitor', '1', '125', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('9', '9', '102', '17 inch VGA Monitor', '1', '125', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('10', '10', '102', '17 inch VGA Monitor', '1', '125', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('11', '11', '102', '17 inch VGA Monitor', '1', '100', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('12', '12', '102', '17 inch VGA Monitor', '1', '125', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('13', '13', '102', '17 inch VGA Monitor', '0', '125', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('14', '14', '103', '32MB VGA Card', '1', '40', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('15', '15', '102', '17 inch VGA Monitor', '1', '100', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('16', '16', '102', '17 inch VGA Monitor', '1', '100', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('17', '17', '102', '17 inch VGA Monitor', '1', '11', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('18', '18', '102', '17 inch VGA Monitor', '1', '125', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('20', '19', '102', '17 inch VGA Monitor', '0', '125', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('21', '19', 'AA101', 'olie 5w40', '1', '125', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('22', '20', '102', '17 inch VGA Monitor', '1', '0', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('23', '21', '102', '17 inch VGA Monitor', '1', '125', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('24', '22', '201', 'Assembly Labour', '1', '30', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('25', '23', '201', 'Assembly Labour', '1', '100', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('26', '24', '201', 'Assembly Labour', '1', '100', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('27', '25', '201', 'Assembly Labour', '1', '100', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('28', '26', '201', 'Assembly Labour', '1', '100', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('29', '27', '201', 'Assembly Labour', '1', '100', '1', '0');
INSERT INTO `0_sales_order_details` VALUES ('30', '28', '201', 'Assembly Labour', '1', '110', '1', '0');


### Structure of table `0_sales_orders` ###

DROP TABLE IF EXISTS `0_sales_orders`;

CREATE TABLE `0_sales_orders` (
  `order_no` int(11) NOT NULL auto_increment,
  `version` tinyint(1) unsigned NOT NULL default '0',
  `type` tinyint(1) NOT NULL default '0',
  `debtor_no` int(11) NOT NULL default '0',
  `branch_code` int(11) NOT NULL default '0',
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
  PRIMARY KEY  (`order_no`)
) TYPE=InnoDB AUTO_INCREMENT=29  AUTO_INCREMENT=29 ;


### Data of table `0_sales_orders` ###

INSERT INTO `0_sales_orders` VALUES ('1', '1', '1', '4', '6', '', '', '2008-03-06', '1', '1', 'Never Mind 13', '123', '', 'Johny Bravo', '10', 'DEF', '2008-03-16');
INSERT INTO `0_sales_orders` VALUES ('2', '1', '0', '4', '6', '', '', '2008-03-06', '1', '1', 'Never Mind 13', '123', '', 'Johny Bravo', '10', 'DEF', '2008-03-06');
INSERT INTO `0_sales_orders` VALUES ('3', '1', '0', '4', '6', '', '', '2008-03-06', '1', '1', 'Never Mind 13', '123', '', 'Johny Bravo', '10', 'DEF', '2008-03-06');
INSERT INTO `0_sales_orders` VALUES ('4', '1', '0', '4', '6', '', '', '2008-03-06', '1', '1', 'Never Mind 13', '123', '', 'Johny Bravo', '10', 'DEF', '2008-03-06');
INSERT INTO `0_sales_orders` VALUES ('5', '1', '1', '4', '6', '', '', '2008-03-06', '1', '1', 'Never Mind 13', '123', '', 'Johny Bravo', '10', 'DEF', '2008-03-06');
INSERT INTO `0_sales_orders` VALUES ('6', '1', '0', '4', '6', '', '', '2008-03-06', '1', '1', 'Never Mind 13', '123', '', 'Johny Bravo', '10', 'DEF', '2008-03-06');
INSERT INTO `0_sales_orders` VALUES ('7', '1', '0', '4', '6', '', '', '2008-03-06', '1', '1', 'Never Mind 13', '123', '', 'Johny Bravo', '10', 'DEF', '2008-03-06');
INSERT INTO `0_sales_orders` VALUES ('8', '1', '0', '4', '6', '', '', '2008-03-07', '1', '1', 'Never Mind 13', '123', '', 'Johny Bravo', '10', 'DEF', '2008-03-07');
INSERT INTO `0_sales_orders` VALUES ('9', '1', '0', '4', '6', '', '', '2008-03-08', '1', '1', 'Never Mind 13', '123', '', 'Johny Bravo', '10', 'DEF', '2008-03-08');
INSERT INTO `0_sales_orders` VALUES ('10', '1', '0', '4', '6', '', '', '2008-03-09', '1', '1', 'Never Mind 13', '123', '', 'Johny Bravo', '12.5', 'DEF', '2008-03-09');
INSERT INTO `0_sales_orders` VALUES ('11', '1', '0', '1', '1', '', '', '2008-03-09', '2', '1', 'The Road', '', '', 'Main', '10', 'DEF', '2008-03-09');
INSERT INTO `0_sales_orders` VALUES ('12', '1', '0', '4', '6', '', '', '2008-03-09', '1', '1', 'Never Mind 13', '123', '', 'Johny Bravo', '10', 'DEF', '2008-03-09');
INSERT INTO `0_sales_orders` VALUES ('13', '0', '0', '4', '6', '', '', '2008-03-10', '1', '1', 'Never Mind 13', '123', '', 'Johny Bravo', '0', 'DEF', '2008-03-20');
INSERT INTO `0_sales_orders` VALUES ('14', '1', '0', '4', '6', '', '', '2008-03-10', '1', '1', 'Never Mind 13', '123', '', 'Johny Bravo', '0', 'DEF', '2008-03-20');
INSERT INTO `0_sales_orders` VALUES ('15', '1', '0', '1', '1', '', '', '2008-03-10', '2', '1', 'The Road', '', '', 'Main', '0', 'DEF', '2008-03-20');
INSERT INTO `0_sales_orders` VALUES ('16', '1', '0', '1', '1', '', '', '2008-03-10', '2', '1', 'The Road', '', '', 'Main', '0', 'DEF', '2008-03-20');
INSERT INTO `0_sales_orders` VALUES ('17', '1', '0', '3', '5', '', '', '2008-03-10', '1', '1', 'N/A', '', '', 'Main', '0', 'CWA', '2008-03-20');
INSERT INTO `0_sales_orders` VALUES ('18', '1', '0', '4', '6', '', '', '2008-03-10', '1', '1', 'Never Mind 13', '123', '', 'Johny Bravo', '10', 'DEF', '2008-03-10');
INSERT INTO `0_sales_orders` VALUES ('19', '2', '0', '4', '6', '', '', '2008-03-17', '1', '1', 'Never Mind 13', '123', '', 'Johny Bravo', '10', 'DEF', '2008-03-27');
INSERT INTO `0_sales_orders` VALUES ('20', '1', '0', '3', '5', '', '', '2008-03-29', '1', '1', 'N/A', '', '', 'Main', '0', 'CWA', '2008-04-17');
INSERT INTO `0_sales_orders` VALUES ('21', '1', '0', '4', '6', '', '', '2008-03-29', '1', '1', 'Never Mind 13', '123', '', 'Johny Bravo', '0', 'DEF', '2008-04-08');
INSERT INTO `0_sales_orders` VALUES ('22', '1', '0', '1', '1', '', '', '2009-01-10', '2', '1', 'The Road', '', '', 'Main', '0', 'DEF', '2009-02-17');
INSERT INTO `0_sales_orders` VALUES ('23', '1', '0', '3', '7', '', '', '2009-01-29', '1', '1', 'N/A', '1223123', '', 'Junk Beer ApS', '0', 'DEF', '2009-01-29');
INSERT INTO `0_sales_orders` VALUES ('24', '1', '0', '3', '7', '', '', '2009-01-29', '1', '1', 'N/A', '1223123', '', 'Junk Beer ApS', '0', 'DEF', '2009-02-17');
INSERT INTO `0_sales_orders` VALUES ('25', '1', '0', '3', '7', '', '', '2009-01-29', '1', '1', 'N/A', '1223123', '', 'Junk Beer ApS', '0', 'DEF', '2009-02-17');
INSERT INTO `0_sales_orders` VALUES ('26', '1', '0', '3', '7', '', '', '2009-01-29', '1', '1', 'N/A', '1223123', '', 'Junk Beer ApS', '0', 'DEF', '2009-02-17');
INSERT INTO `0_sales_orders` VALUES ('27', '1', '0', '3', '7', '', '', '2009-01-29', '1', '1', 'N/A', '1223123', '', 'Junk Beer ApS', '0', 'DEF', '2009-02-17');
INSERT INTO `0_sales_orders` VALUES ('28', '1', '0', '3', '7', '', '', '2009-01-30', '1', '1', 'N/A', '1223123', '', 'Junk Beer ApS', '0', 'DEF', '2009-01-31');


### Structure of table `0_sales_pos` ###

DROP TABLE IF EXISTS `0_sales_pos`;

CREATE TABLE `0_sales_pos` (
  `id` smallint(6) unsigned NOT NULL auto_increment,
  `pos_name` varchar(30) NOT NULL,
  `cash_sale` tinyint(1) NOT NULL,
  `credit_sale` tinyint(1) NOT NULL,
  `pos_location` varchar(5) NOT NULL,
  `pos_account` smallint(6) unsigned NOT NULL,
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `pos_name` (`pos_name`)
) TYPE=MyISAM AUTO_INCREMENT=2  AUTO_INCREMENT=2 ;


### Data of table `0_sales_pos` ###

INSERT INTO `0_sales_pos` VALUES ('1', 'Default', '1', '1', 'DEF', '2', '0');


### Structure of table `0_sales_types` ###

DROP TABLE IF EXISTS `0_sales_types`;

CREATE TABLE `0_sales_types` (
  `id` int(11) NOT NULL auto_increment,
  `sales_type` char(50) NOT NULL default '',
  `tax_included` int(1) NOT NULL default '0',
  `factor` double NOT NULL default '1',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `sales_type` (`sales_type`)
) TYPE=MyISAM AUTO_INCREMENT=3  AUTO_INCREMENT=3 ;


### Data of table `0_sales_types` ###

INSERT INTO `0_sales_types` VALUES ('1', 'Retail', '1', '1', '0');
INSERT INTO `0_sales_types` VALUES ('2', 'Wholesale', '0', '1', '0');


### Structure of table `0_salesman` ###

DROP TABLE IF EXISTS `0_salesman`;

CREATE TABLE `0_salesman` (
  `salesman_code` int(11) NOT NULL auto_increment,
  `salesman_name` varchar(60) NOT NULL default '',
  `salesman_phone` varchar(30) NOT NULL default '',
  `salesman_fax` varchar(30) NOT NULL default '',
  `salesman_email` varchar(100) NOT NULL default '',
  `provision` double NOT NULL default '0',
  `break_pt` double NOT NULL default '0',
  `provision2` double NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`salesman_code`),
  UNIQUE KEY `salesman_name` (`salesman_name`)
) TYPE=MyISAM AUTO_INCREMENT=3  AUTO_INCREMENT=3 ;


### Data of table `0_salesman` ###

INSERT INTO `0_salesman` VALUES ('1', 'Sparc Menser', '', '', '', '5', '1000', '4', '0');
INSERT INTO `0_salesman` VALUES ('2', 'Joe Hunt', '', '', '', '4', '500', '3', '0');


### Structure of table `0_shippers` ###

DROP TABLE IF EXISTS `0_shippers`;

CREATE TABLE `0_shippers` (
  `shipper_id` int(11) NOT NULL auto_increment,
  `shipper_name` varchar(60) NOT NULL default '',
  `phone` varchar(30) NOT NULL default '',
  `contact` tinytext NOT NULL,
  `address` tinytext NOT NULL,
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`shipper_id`),
  UNIQUE KEY `name` (`shipper_name`)
) TYPE=MyISAM AUTO_INCREMENT=3  AUTO_INCREMENT=3 ;


### Data of table `0_shippers` ###

INSERT INTO `0_shippers` VALUES ('1', 'UPS', '', '', '', '0');
INSERT INTO `0_shippers` VALUES ('2', 'Internet', '', '', '', '0');


### Structure of table `0_sql_trail` ###

DROP TABLE IF EXISTS `0_sql_trail`;

CREATE TABLE `0_sql_trail` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `sql` text NOT NULL,
  `result` tinyint(1) NOT NULL,
  `msg` varchar(255) NOT NULL,
  PRIMARY KEY  (`id`)
) TYPE=MyISAM  AUTO_INCREMENT=1 ;


### Data of table `0_sql_trail` ###



### Structure of table `0_stock_category` ###

DROP TABLE IF EXISTS `0_stock_category`;

CREATE TABLE `0_stock_category` (
  `category_id` int(11) NOT NULL auto_increment,
  `description` varchar(60) NOT NULL default '',
  `dflt_tax_type` int(11) NOT NULL default '1',
  `dflt_units` varchar(20) NOT NULL default 'each',
  `dflt_mb_flag` char(1) NOT NULL default 'B',
  `dflt_sales_act` varchar(11) NOT NULL default '',
  `dflt_cogs_act` varchar(11) NOT NULL default '',
  `dflt_inventory_act` varchar(11) NOT NULL default '',
  `dflt_adjustment_act` varchar(11) NOT NULL default '',
  `dflt_assembly_act` varchar(11) NOT NULL default '',
  `dflt_dim1` int(11) default NULL,
  `dflt_dim2` int(11) default NULL,
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`category_id`),
  UNIQUE KEY `description` (`description`)
) TYPE=MyISAM AUTO_INCREMENT=5  AUTO_INCREMENT=5 ;

### Data of table `0_stock_category` ###

INSERT INTO `0_stock_category` VALUES ('1', 'Components', '1', 'each', 'B', '3000', '4010', '1420', '4210', '1410', '1', '1', '0');
INSERT INTO `0_stock_category` VALUES ('2', 'Charges', '1', 'each', 'D', '3000', '4010', '1420', '4210', '1410', '1', '1', '0');
INSERT INTO `0_stock_category` VALUES ('3', 'Systems', '1', 'each', 'M', '3000', '4010', '1420', '4210', '1410', '1', '1', '0');
INSERT INTO `0_stock_category` VALUES ('4', 'Services', '1', 'hrs', 'D', '3000', '4010', '1420', '4210', '1410', '1', '1', '0');


### Structure of table `0_stock_master` ###

DROP TABLE IF EXISTS `0_stock_master`;

CREATE TABLE `0_stock_master` (
  `stock_id` varchar(20) NOT NULL default '',
  `category_id` int(11) NOT NULL default '0',
  `tax_type_id` int(11) NOT NULL default '0',
  `description` varchar(200) NOT NULL default '',
  `long_description` tinytext NOT NULL,
  `units` varchar(20) NOT NULL default 'each',
  `mb_flag` char(1) NOT NULL default 'B',
  `sales_account` varchar(11) NOT NULL default '',
  `cogs_account` varchar(11) NOT NULL default '',
  `inventory_account` varchar(11) NOT NULL default '',
  `adjustment_account` varchar(11) NOT NULL default '',
  `assembly_account` varchar(11) NOT NULL default '',
  `dimension_id` int(11) default NULL,
  `dimension2_id` int(11) default NULL,
  `actual_cost` double NOT NULL default '0',
  `last_cost` double NOT NULL default '0',
  `material_cost` double NOT NULL default '0',
  `labour_cost` double NOT NULL default '0',
  `overhead_cost` double NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`stock_id`)
) TYPE=InnoDB  ;


### Data of table `0_stock_master` ###

INSERT INTO `0_stock_master` VALUES ('102', '1', '1', '17 inch VGA Monitor', '', 'each', 'B', '3000', '4010', '1420', '4210', '0', '0', '0', '0', '-3.4114285714283', '10', '0', '0', '0');
INSERT INTO `0_stock_master` VALUES ('103', '1', '1', '32MB VGA Card', '', 'each', 'B', '3000', '4010', '1420', '4210', '0', '0', '0', '0', '0', '20', '0', '0', '0');
INSERT INTO `0_stock_master` VALUES ('104', '1', '1', '52x CD Drive', '', 'each', 'B', '3000', '4010', '1420', '4210', '0', '0', '0', '0', '0', '18', '0', '0', '0');
INSERT INTO `0_stock_master` VALUES ('201', '2', '1', 'Assembly Labour', '', 'each', 'D', '3000', '4010', '1420', '4210', '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `0_stock_master` VALUES ('202', '1', '2', 'Electric stimulator', '', 'each', 'B', '3000', '4010', '1420', '4210', '1410', '0', '0', '0', '0', '30', '0', '0', '0');
INSERT INTO `0_stock_master` VALUES ('3400', '3', '1', 'P4 Business System', '', 'each', 'M', '3000', '4010', '1420', '4210', '1400', '0', '0', '0', '160', '100', '30', '10', '0');
INSERT INTO `0_stock_master` VALUES ('AA101', '1', '1', 'olie 5w40', 'Shell Helix 5w40', 'l', 'B', '3000', '4010', '1420', '4210', '1410', '0', '0', '0', '0', '5.0599509174312', '0', '0', '0');


### Structure of table `0_stock_moves` ###

DROP TABLE IF EXISTS `0_stock_moves`;

CREATE TABLE `0_stock_moves` (
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
  KEY `type` (`type`,`trans_no`)
) TYPE=InnoDB AUTO_INCREMENT=43  AUTO_INCREMENT=43 ;


### Data of table `0_stock_moves` ###

INSERT INTO `0_stock_moves` VALUES ('1', '4', '102', '25', 'DEF', '2008-03-06', '1', '25', '', '10', '0', '80', '1');
INSERT INTO `0_stock_moves` VALUES ('2', '4', '103', '25', 'DEF', '2008-03-06', '1', '30', '', '10', '0', '20', '1');
INSERT INTO `0_stock_moves` VALUES ('3', '4', '104', '25', 'DEF', '2008-03-06', '1', '20', '', '10', '0', '18', '1');
INSERT INTO `0_stock_moves` VALUES ('4', '4', '202', '25', 'DEF', '2008-03-06', '1', '50', '', '10', '0', '30', '1');
INSERT INTO `0_stock_moves` VALUES ('5', '1', '102', '13', 'DEF', '2008-03-06', '0', '125', '0', '-1', '0', '80', '1');
INSERT INTO `0_stock_moves` VALUES ('6', '2', '102', '13', 'DEF', '2008-03-06', '0', '125', 'auto', '-1', '0', '80', '1');
INSERT INTO `0_stock_moves` VALUES ('7', '3', '102', '13', 'DEF', '2008-03-06', '0', '125', 'auto', '-1', '0', '80', '1');
INSERT INTO `0_stock_moves` VALUES ('8', '4', '102', '13', 'DEF', '2008-03-06', '0', '125', 'auto', '-1', '0', '80', '1');
INSERT INTO `0_stock_moves` VALUES ('9', '5', '102', '13', 'DEF', '2008-03-06', '0', '125', 'auto', '-1', '0', '80', '1');
INSERT INTO `0_stock_moves` VALUES ('10', '1', '102', '11', 'DEF', '2008-03-06', '0', '125', 'Return Ex Inv: 5', '1', '0', '80', '0');
INSERT INTO `0_stock_moves` VALUES ('11', '6', '102', '13', 'DEF', '2008-03-06', '0', '125', 'auto', '-1', '0', '80', '1');
INSERT INTO `0_stock_moves` VALUES ('12', '7', '102', '13', 'DEF', '2008-03-06', '0', '125', '1', '-1', '0', '80', '1');
INSERT INTO `0_stock_moves` VALUES ('13', '2', '102', '11', 'DEF', '2008-03-06', '0', '125', 'Return Ex Inv: 6', '1', '0', '80', '0');
INSERT INTO `0_stock_moves` VALUES ('14', '3', '102', '11', 'DEF', '2008-03-06', '0', '125', 'Return Ex Inv: 4', '1', '0', '80', '0');
INSERT INTO `0_stock_moves` VALUES ('15', '4', '102', '11', 'CWA', '2008-03-07', '0', '125', 'Return', '1', '0', '0', '0');
INSERT INTO `0_stock_moves` VALUES ('16', '7', '102', '26', 'DEF', '2008-03-07', '0', '0', '7', '-1', '0', '0', '1');
INSERT INTO `0_stock_moves` VALUES ('17', '7', '103', '26', 'DEF', '2008-03-07', '0', '0', '7', '-1', '0', '0', '1');
INSERT INTO `0_stock_moves` VALUES ('18', '7', '104', '26', 'DEF', '2008-03-07', '0', '0', '7', '-1', '0', '0', '1');
INSERT INTO `0_stock_moves` VALUES ('19', '7', '3400', '26', 'DEF', '2008-03-07', '0', '0', '7', '1', '0', '0', '1');
INSERT INTO `0_stock_moves` VALUES ('20', '8', '102', '13', 'DEF', '2008-03-07', '0', '125', '2', '-1', '0', '80', '1');
INSERT INTO `0_stock_moves` VALUES ('21', '9', '102', '13', 'DEF', '2008-03-08', '0', '125', 'auto', '-1', '0', '80', '1');
INSERT INTO `0_stock_moves` VALUES ('22', '10', '102', '13', 'DEF', '2008-03-09', '0', '125', 'auto', '-1', '0', '80', '1');
INSERT INTO `0_stock_moves` VALUES ('23', '11', '102', '13', 'DEF', '2008-03-09', '0', '100', 'auto', '-1', '0', '80', '1');
INSERT INTO `0_stock_moves` VALUES ('24', '12', '102', '13', 'DEF', '2008-03-09', '0', '125', 'auto', '-1', '0', '80', '1');
INSERT INTO `0_stock_moves` VALUES ('26', '13', '103', '13', 'DEF', '2008-03-10', '0', '40', '3', '-1', '0', '20', '1');
INSERT INTO `0_stock_moves` VALUES ('27', '14', '102', '13', 'DEF', '2008-03-10', '0', '100', '4', '-1', '0', '80', '1');
INSERT INTO `0_stock_moves` VALUES ('28', '15', '102', '13', 'DEF', '2008-03-10', '0', '100', '5', '-1', '0', '80', '1');
INSERT INTO `0_stock_moves` VALUES ('29', '16', '102', '13', 'CWA', '2008-03-10', '0', '11', '6', '-1', '0', '80', '1');
INSERT INTO `0_stock_moves` VALUES ('30', '17', '102', '13', 'DEF', '2008-03-10', '0', '125', 'auto', '-1', '0', '80', '1');
INSERT INTO `0_stock_moves` VALUES ('31', '5', 'AA101', '25', 'DEF', '2008-03-17', '4', '3.25', '', '8', '0', '0', '1');
INSERT INTO `0_stock_moves` VALUES ('32', '6', 'AA101', '25', 'DEF', '2008-03-19', '4', '3.25', '', '408', '0', '0', '1');
INSERT INTO `0_stock_moves` VALUES ('33', '7', '102', '25', 'DEF', '2008-03-20', '2', '0', '', '5', '0', '80', '1');
INSERT INTO `0_stock_moves` VALUES ('34', '8', '102', '25', 'DEF', '2008-03-25', '2', '10', '', '1', '0', '-120', '1');
INSERT INTO `0_stock_moves` VALUES ('35', '9', 'AA101', '25', 'DEF', '2008-03-25', '4', '3.25', '', '20', '0', '5.0599485576923', '1');
INSERT INTO `0_stock_moves` VALUES ('36', '10', '102', '25', 'DEF', '2008-03-25', '4', '10', '', '5', '0', '-73.393333333333', '1');
INSERT INTO `0_stock_moves` VALUES ('37', '11', '102', '25', 'DEF', '2008-03-25', '4', '10', '', '1', '0', '-17.79125', '1');
INSERT INTO `0_stock_moves` VALUES ('38', '12', '102', '25', 'DEF', '2008-03-28', '4', '10', '', '5', '0', '-73.393333333333', '1');
INSERT INTO `0_stock_moves` VALUES ('39', '18', 'AA101', '13', 'DEF', '2008-03-17', '0', '125', '7', '-1', '0', '5.0599509174312', '1');
INSERT INTO `0_stock_moves` VALUES ('40', '19', '102', '13', 'CWA', '2008-03-29', '0', '0', 'auto', '-1', '0', '-3.4114285714283', '1');
INSERT INTO `0_stock_moves` VALUES ('41', '20', '102', '13', 'DEF', '2008-03-29', '0', '125', '8', '-1', '0', '-3.4114285714283', '1');
INSERT INTO `0_stock_moves` VALUES ('42', '13', '102', '25', 'CWA', '2009-01-10', '1', '10', '', '1', '0', '10', '1');


### Structure of table `0_supp_allocations` ###

DROP TABLE IF EXISTS `0_supp_allocations`;

CREATE TABLE `0_supp_allocations` (
  `id` int(11) NOT NULL auto_increment,
  `amt` double unsigned default NULL,
  `date_alloc` date NOT NULL default '0000-00-00',
  `trans_no_from` int(11) default NULL,
  `trans_type_from` int(11) default NULL,
  `trans_no_to` int(11) default NULL,
  `trans_type_to` int(11) default NULL,
  PRIMARY KEY  (`id`)
) TYPE=InnoDB AUTO_INCREMENT=11  AUTO_INCREMENT=11 ;


### Data of table `0_supp_allocations` ###

INSERT INTO `0_supp_allocations` VALUES ('10', '200', '2009-01-10', '4', '22', '7', '20');


### Structure of table `0_supp_invoice_items` ###

DROP TABLE IF EXISTS `0_supp_invoice_items`;

CREATE TABLE `0_supp_invoice_items` (
  `id` int(11) NOT NULL auto_increment,
  `supp_trans_no` int(11) default NULL,
  `supp_trans_type` int(11) default NULL,
  `gl_code` varchar(11) NOT NULL default '0',
  `grn_item_id` int(11) default NULL,
  `po_detail_item_id` int(11) default NULL,
  `stock_id` varchar(20) NOT NULL default '',
  `description` tinytext,
  `quantity` double NOT NULL default '0',
  `unit_price` double NOT NULL default '0',
  `unit_tax` double NOT NULL default '0',
  `memo_` tinytext,
  PRIMARY KEY  (`id`)
) TYPE=InnoDB AUTO_INCREMENT=26  AUTO_INCREMENT=26 ;


### Data of table `0_supp_invoice_items` ###

INSERT INTO `0_supp_invoice_items` VALUES ('4', '4', '20', '0', '1', '1', '102', '17 inch VGA Monitor', '5', '3020', '0', '');
INSERT INTO `0_supp_invoice_items` VALUES ('5', '4', '20', '0', '2', '2', '103', '32MB VGA Card', '25', '90', '0', '');
INSERT INTO `0_supp_invoice_items` VALUES ('8', '7', '20', '0', '5', '7', '102', '17 inch VGA Monitor', '10', '25', '0', '');
INSERT INTO `0_supp_invoice_items` VALUES ('9', '7', '20', '0', '6', '8', '103', '32MB VGA Card', '10', '30', '0', '');
INSERT INTO `0_supp_invoice_items` VALUES ('12', '8', '20', '0', '11', '13', '102', '17 inch VGA Monitor', '5', '0', '0', '');
INSERT INTO `0_supp_invoice_items` VALUES ('13', '9', '20', '0', '12', '14', '102', '17 inch VGA Monitor', '1', '10', '0', '');
INSERT INTO `0_supp_invoice_items` VALUES ('20', '10', '20', '0', '9', '12', 'AA101', 'olie 5w40', '8', '3.25', '0', '');
INSERT INTO `0_supp_invoice_items` VALUES ('21', '11', '20', '0', '10', '12', 'AA101', 'olie 5w40', '408', '3.25', '0.81', '');
INSERT INTO `0_supp_invoice_items` VALUES ('22', '12', '20', '0', '7', '9', '104', '52x CD Drive', '10', '20', '5', '');
INSERT INTO `0_supp_invoice_items` VALUES ('23', '12', '20', '0', '8', '10', '202', 'Electric stimulator', '10', '50', '2.5', '');
INSERT INTO `0_supp_invoice_items` VALUES ('24', '13', '20', '2682', '0', '0', '', '', '0', '10', '0', 'Phone');
INSERT INTO `0_supp_invoice_items` VALUES ('25', '13', '20', '6730', '0', '0', '', '', '0', '40', '0', 'Phone');


### Structure of table `0_supp_trans` ###

DROP TABLE IF EXISTS `0_supp_trans`;

CREATE TABLE `0_supp_trans` (
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
  PRIMARY KEY  (`trans_no`,`type`),
  KEY `supplier_id` (`supplier_id`),
  KEY `SupplierID_2` (`supplier_id`,`supp_reference`),
  KEY `type` (`type`)
) TYPE=InnoDB  ;


### Data of table `0_supp_trans` ###

INSERT INTO `0_supp_trans` VALUES ('4', '22', '1', '3', '', '2009-01-10', '2009-01-10', '-200', '0', '0', '1', '200');
INSERT INTO `0_supp_trans` VALUES ('7', '20', '1', '27', 'eee', '2008-03-06', '2008-04-17', '550', '0', '0', '1', '200');
INSERT INTO `0_supp_trans` VALUES ('8', '20', '2', '28', '213', '2008-03-20', '2008-04-17', '0', '0', '0', '1.2', '0');
INSERT INTO `0_supp_trans` VALUES ('9', '20', '2', '29', 'aaa', '2008-03-25', '2008-04-17', '10', '0', '0', '1.9816266221251', '0');
INSERT INTO `0_supp_trans` VALUES ('10', '20', '4', '30', '12w', '2008-03-25', '2008-04-17', '20.8', '0', '5.2', '1.5569', '0');
INSERT INTO `0_supp_trans` VALUES ('11', '20', '4', '31', 'jan', '2008-03-28', '2008-04-17', '1326', '0', '331.5', '1.5796', '0');
INSERT INTO `0_supp_trans` VALUES ('12', '20', '1', '32', 'ga', '2009-01-10', '2009-02-17', '700', '0', '75', '1', '0');
INSERT INTO `0_supp_trans` VALUES ('13', '20', '1', '33', 'gg', '2009-01-29', '2009-02-17', '50', '0', '0', '1', '0');


### Structure of table `0_suppliers` ###

DROP TABLE IF EXISTS `0_suppliers`;

CREATE TABLE `0_suppliers` (
  `supplier_id` int(11) NOT NULL auto_increment,
  `supp_name` varchar(60) NOT NULL default '',
  `address` tinytext NOT NULL,
  `supp_address` tinytext NOT NULL,
  `phone` varchar(30) NOT NULL default '',
  `fax` varchar(30) NOT NULL default '',
  `gst_no` varchar(25) NOT NULL default '',
  `contact` varchar(60) NOT NULL default '',
  `supp_account_no` varchar(40) NOT NULL default '',
  `email` varchar(100) NOT NULL default '',
  `website` varchar(100) NOT NULL default '',
  `bank_account` varchar(60) NOT NULL default '',
  `curr_code` char(3) default NULL,
  `payment_terms` int(11) default NULL,
  `dimension_id` int(11) default '0',
  `dimension2_id` int(11) default '0',
  `tax_group_id` int(11) default NULL,
  `credit_limit` double NOT NULL default '0',
  `purchase_account` varchar(11) default NULL,
  `payable_account` varchar(11) default NULL,
  `payment_discount_account` varchar(11) default NULL,
  `notes` tinytext NOT NULL,
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`supplier_id`)
) TYPE=MyISAM AUTO_INCREMENT=5  AUTO_INCREMENT=5 ;


### Data of table `0_suppliers` ###

INSERT INTO `0_suppliers` VALUES ('1', 'Ghostbusters Corp.', '', '', '', '', '', '', '', '', '', '123456789', 'USD', '1', '0', '0', '1', '0', '4000', '2630', '4250', '', '0');
INSERT INTO `0_suppliers` VALUES ('2', 'Beefeater Ltd.', '', '', '', '', '', '', '', '', '', '987654321', 'GBP', '1', '0', '0', '2', '0', '4000', '2630', '4250', '', '0');
INSERT INTO `0_suppliers` VALUES ('3', 'Super Trooper AB', 'Adress', '', '', '', '', '', '', 'sven@sven.sve', '', '123456', 'SEK', '3', '0', '0', '2', '0', '4000', '2630', '4250', '', '0');
INSERT INTO `0_suppliers` VALUES ('4', 'Brezan', 'N/A', '', '', '', '', '', '', 'info@brezan.tv', '', '', 'EUR', '1', '0', '0', '1', '0', '4010', '2630', '4250', '', '0');


### Structure of table `0_sys_types` ###

DROP TABLE IF EXISTS `0_sys_types`;

CREATE TABLE `0_sys_types` (
  `type_id` smallint(6) NOT NULL default '0',
  `type_name` varchar(60) NOT NULL default '',
  `type_no` int(11) NOT NULL default '1',
  `next_reference` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`type_id`)
) TYPE=InnoDB  ;


### Data of table `0_sys_types` ###

INSERT INTO `0_sys_types` VALUES ('0', 'Journal - GL', '18', '34');
INSERT INTO `0_sys_types` VALUES ('1', 'Payment - GL', '8', '8');
INSERT INTO `0_sys_types` VALUES ('2', 'Receipt - GL', '6', '16');
INSERT INTO `0_sys_types` VALUES ('4', 'Funds Transfer', '3', '6');
INSERT INTO `0_sys_types` VALUES ('10', 'Sales Invoice', '25', '25');
INSERT INTO `0_sys_types` VALUES ('11', 'Credit Note', '4', '4');
INSERT INTO `0_sys_types` VALUES ('12', 'Receipt', '11', '11');
INSERT INTO `0_sys_types` VALUES ('13', 'Delivery', '27', '10');
INSERT INTO `0_sys_types` VALUES ('16', 'Location Transfer', '2', '2');
INSERT INTO `0_sys_types` VALUES ('17', 'Inventory Adjustment', '2', '2');
INSERT INTO `0_sys_types` VALUES ('18', 'Purchase Order', '1', '16');
INSERT INTO `0_sys_types` VALUES ('20', 'Supplier Invoice', '13', '34');
INSERT INTO `0_sys_types` VALUES ('21', 'Supplier Credit Note', '1', '2');
INSERT INTO `0_sys_types` VALUES ('22', 'Supplier Payment', '4', '4');
INSERT INTO `0_sys_types` VALUES ('25', 'Purchase Order Delivery', '1', '14');
INSERT INTO `0_sys_types` VALUES ('26', 'Work Order', '1', '8');
INSERT INTO `0_sys_types` VALUES ('28', 'Work Order Issue', '1', '2');
INSERT INTO `0_sys_types` VALUES ('29', 'Work Order Production', '1', '201');
INSERT INTO `0_sys_types` VALUES ('30', 'Sales Order', '1', '1');
INSERT INTO `0_sys_types` VALUES ('35', 'Cost Update', '6', '1');
INSERT INTO `0_sys_types` VALUES ('40', 'Dimension', '1', '3');


### Structure of table `0_tax_group_items` ###

DROP TABLE IF EXISTS `0_tax_group_items`;

CREATE TABLE `0_tax_group_items` (
  `tax_group_id` int(11) NOT NULL default '0',
  `tax_type_id` int(11) NOT NULL default '0',
  `rate` double NOT NULL default '0',
  PRIMARY KEY  (`tax_group_id`,`tax_type_id`)
) TYPE=InnoDB  ;


### Data of table `0_tax_group_items` ###

INSERT INTO `0_tax_group_items` VALUES ('1', '1', '5', '0');
INSERT INTO `0_tax_group_items` VALUES ('1', '3', '25', '0');
INSERT INTO `0_tax_group_items` VALUES ('4', '3', '25', '0');
INSERT INTO `0_tax_group_items` VALUES ('5', '4', '0', '0');


### Structure of table `0_tax_groups` ###

DROP TABLE IF EXISTS `0_tax_groups`;

CREATE TABLE `0_tax_groups` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(60) NOT NULL default '',
  `tax_shipping` tinyint(1) NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) TYPE=InnoDB AUTO_INCREMENT=6  AUTO_INCREMENT=6 ;


### Data of table `0_tax_groups` ###

INSERT INTO `0_tax_groups` VALUES ('1', 'VAT', '0', '0');
INSERT INTO `0_tax_groups` VALUES ('2', 'Tax-Free', '0', '0');
INSERT INTO `0_tax_groups` VALUES ('4', 'Shipping', '1', '0');
INSERT INTO `0_tax_groups` VALUES ('5', 'Export', '0', '0');


### Structure of table `0_tax_types` ###

DROP TABLE IF EXISTS `0_tax_types`;

CREATE TABLE `0_tax_types` (
  `id` int(11) NOT NULL auto_increment,
  `rate` double NOT NULL default '0',
  `sales_gl_code` varchar(11) NOT NULL default '',
  `purchasing_gl_code` varchar(11) NOT NULL default '',
  `name` varchar(60) NOT NULL default '',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`)
) TYPE=InnoDB AUTO_INCREMENT=5  AUTO_INCREMENT=5 ;


### Data of table `0_tax_types` ###

INSERT INTO `0_tax_types` VALUES ('1', '5', '2660', '2680', 'VAT', '0');
INSERT INTO `0_tax_types` VALUES ('2', '1', '2662', '2680', 'Manufact tax 1', '0');
INSERT INTO `0_tax_types` VALUES ('3', '25', '2664', '2682', 'VAT', '0');
INSERT INTO `0_tax_types` VALUES ('4', '0', '2660', '2680', 'Export', '0');


### Structure of table `0_trans_tax_details` ###

DROP TABLE IF EXISTS `0_trans_tax_details`;

CREATE TABLE `0_trans_tax_details` (
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
  PRIMARY KEY  (`id`)
) TYPE=InnoDB AUTO_INCREMENT=60  AUTO_INCREMENT=60 ;


### Data of table `0_trans_tax_details` ###

INSERT INTO `0_trans_tax_details` VALUES ('1', '13', '1', '2008-03-06', '3', '25', '1', '1', '81', '27', '0');
INSERT INTO `0_trans_tax_details` VALUES ('2', '10', '1', '2008-03-06', '3', '25', '1', '1', '81', '27', '0');
INSERT INTO `0_trans_tax_details` VALUES ('3', '13', '2', '2008-03-06', '3', '25', '1', '1', '81', '27', 'auto');
INSERT INTO `0_trans_tax_details` VALUES ('4', '10', '2', '2008-03-06', '3', '25', '1', '1', '81', '27', '1');
INSERT INTO `0_trans_tax_details` VALUES ('5', '13', '3', '2008-03-06', '3', '25', '1', '1', '81', '27', 'auto');
INSERT INTO `0_trans_tax_details` VALUES ('6', '10', '3', '2008-03-06', '3', '25', '1', '1', '81', '27', '2');
INSERT INTO `0_trans_tax_details` VALUES ('7', '13', '4', '2008-03-06', '3', '25', '1', '1', '81', '27', 'auto');
INSERT INTO `0_trans_tax_details` VALUES ('8', '10', '4', '2008-03-06', '3', '25', '1', '1', '81', '27', '3');
INSERT INTO `0_trans_tax_details` VALUES ('9', '13', '5', '2008-03-06', '3', '25', '1', '1', '81', '27', 'auto');
INSERT INTO `0_trans_tax_details` VALUES ('10', '10', '5', '2008-03-06', '3', '25', '1', '1', '81', '27', '4');
INSERT INTO `0_trans_tax_details` VALUES ('11', '11', '1', '2008-03-06', '3', '25', '1', '1', '81', '27', '0');
INSERT INTO `0_trans_tax_details` VALUES ('12', '13', '6', '2008-03-06', '3', '25', '1', '1', '81', '27', 'auto');
INSERT INTO `0_trans_tax_details` VALUES ('13', '10', '6', '2008-03-06', '3', '25', '1', '1', '81', '27', '5');
INSERT INTO `0_trans_tax_details` VALUES ('14', '13', '7', '2008-03-06', '3', '25', '1', '1', '81', '27', '1');
INSERT INTO `0_trans_tax_details` VALUES ('15', '11', '2', '2008-03-06', '3', '25', '1', '1', '81', '27', '1');
INSERT INTO `0_trans_tax_details` VALUES ('16', '11', '3', '2008-03-06', '3', '25', '1', '1', '81', '27', '2');
INSERT INTO `0_trans_tax_details` VALUES ('17', '11', '4', '2008-03-07', '3', '25', '1', '0', '100', '25', '3');
INSERT INTO `0_trans_tax_details` VALUES ('18', '13', '8', '2008-03-07', '3', '25', '1', '1', '81', '27', '2');
INSERT INTO `0_trans_tax_details` VALUES ('19', '13', '9', '2008-03-08', '3', '25', '1', '1', '81', '27', 'auto');
INSERT INTO `0_trans_tax_details` VALUES ('20', '10', '7', '2008-03-08', '3', '25', '1', '1', '81', '27', '6');
INSERT INTO `0_trans_tax_details` VALUES ('21', '13', '10', '2008-03-09', '3', '25', '1', '1', '82.5', '27.5', 'auto');
INSERT INTO `0_trans_tax_details` VALUES ('22', '10', '8', '2008-03-09', '3', '25', '1', '1', '82.5', '27.5', '7');
INSERT INTO `0_trans_tax_details` VALUES ('23', '13', '11', '2008-03-09', '3', '25', '1', '0', '110', '27.5', 'auto');
INSERT INTO `0_trans_tax_details` VALUES ('24', '10', '9', '2008-03-09', '3', '25', '1', '0', '110', '27.5', '8');
INSERT INTO `0_trans_tax_details` VALUES ('25', '13', '12', '2008-03-09', '3', '25', '1', '1', '81', '27', 'auto');
INSERT INTO `0_trans_tax_details` VALUES ('26', '10', '10', '2008-03-09', '3', '25', '1', '1', '81', '27', '9');
INSERT INTO `0_trans_tax_details` VALUES ('27', '13', '13', '2008-03-10', '3', '25', '1', '1', '24', '8', '3');
INSERT INTO `0_trans_tax_details` VALUES ('28', '13', '14', '2008-03-10', '3', '25', '1', '0', '100', '25', '4');
INSERT INTO `0_trans_tax_details` VALUES ('29', '10', '11', '2008-03-10', '3', '25', '1', '0', '100', '25', '10');
INSERT INTO `0_trans_tax_details` VALUES ('30', '13', '15', '2008-03-10', '3', '25', '1', '0', '100', '25', '5');
INSERT INTO `0_trans_tax_details` VALUES ('31', '10', '12', '2008-03-10', '3', '25', '1', '0', '100', '25', '11');
INSERT INTO `0_trans_tax_details` VALUES ('32', '10', '13', '2008-03-07', '3', '25', '1', '1', '81', '27', '12');
INSERT INTO `0_trans_tax_details` VALUES ('33', '13', '17', '2008-03-10', '3', '25', '1', '1', '81', '27', 'auto');
INSERT INTO `0_trans_tax_details` VALUES ('34', '10', '14', '2008-03-10', '3', '25', '1', '1', '81', '27', '13');
INSERT INTO `0_trans_tax_details` VALUES ('35', '10', '15', '2008-03-10', '3', '25', '1', '1', '24', '8', '14');
INSERT INTO `0_trans_tax_details` VALUES ('36', '13', '18', '2008-03-17', '3', '25', '1', '1', '81', '27', '7');
INSERT INTO `0_trans_tax_details` VALUES ('37', '10', '16', '2008-03-17', '3', '25', '1', '1', '81', '27', '15');
INSERT INTO `0_trans_tax_details` VALUES ('38', '13', '20', '2008-03-29', '3', '25', '1', '1', '75', '25', '8');
INSERT INTO `0_trans_tax_details` VALUES ('39', '10', '19', '2008-03-29', '3', '25', '1', '1', '75', '25', '18');
INSERT INTO `0_trans_tax_details` VALUES ('40', '13', '21', '2009-01-10', '3', '25', '1', '0', '30', '7.5', 'auto');
INSERT INTO `0_trans_tax_details` VALUES ('41', '10', '20', '2009-01-10', '3', '25', '1', '0', '30', '7.5', '19');
INSERT INTO `0_trans_tax_details` VALUES ('42', '20', '10', '2008-03-25', '3', '25', '1.5569', '0', '20.8', '5.2', '12w');
INSERT INTO `0_trans_tax_details` VALUES ('43', '20', '11', '2008-03-28', '3', '25', '1.5796', '0', '1326', '331.5', 'jan');
INSERT INTO `0_trans_tax_details` VALUES ('44', '20', '12', '2009-01-10', '1', '5', '1', '0', '500', '25', 'ga');
INSERT INTO `0_trans_tax_details` VALUES ('45', '20', '12', '2009-01-10', '3', '25', '1', '0', '200', '50', 'ga');
INSERT INTO `0_trans_tax_details` VALUES ('46', '13', '22', '0000-00-00', '3', '25', '1.7164140368342e-005', '1', '80', '20', '');
INSERT INTO `0_trans_tax_details` VALUES ('47', '10', '21', '0000-00-00', '3', '25', '1.7164140368342e-005', '1', '80', '20', '');
INSERT INTO `0_trans_tax_details` VALUES ('48', '13', '23', '0000-00-00', '3', '25', '1.7164140368342e-005', '1', '80', '20', '');
INSERT INTO `0_trans_tax_details` VALUES ('49', '10', '22', '2009-01-29', '3', '25', '0.17593462333271', '1', '80', '20', '21');
INSERT INTO `0_trans_tax_details` VALUES ('50', '13', '24', '2009-01-29', '3', '25', '1', '1', '80', '20', 'auto');
INSERT INTO `0_trans_tax_details` VALUES ('51', '10', '23', '2009-01-29', '3', '25', '0.17593462333271', '1', '80', '20', '22');
INSERT INTO `0_trans_tax_details` VALUES ('52', '13', '25', '2009-01-29', '3', '25', '0.17593462333271', '1', '80', '20', 'auto');
INSERT INTO `0_trans_tax_details` VALUES ('53', '10', '24', '2009-01-29', '3', '25', '0.17593462333271', '1', '80', '20', '23');
INSERT INTO `0_trans_tax_details` VALUES ('54', '13', '26', '2009-01-29', '4', '0', '0.17593462333271', '1', '100', '0', 'auto');
INSERT INTO `0_trans_tax_details` VALUES ('55', '10', '25', '2009-01-29', '4', '0', '0.17593462333271', '1', '100', '0', '24');
INSERT INTO `0_trans_tax_details` VALUES ('56', '2', '5', '2009-01-29', '3', '25', '1', '0', '160', '40', '');
INSERT INTO `0_trans_tax_details` VALUES ('57', '1', '8', '2009-01-29', '3', '25', '1', '0', '40', '10', '');
INSERT INTO `0_trans_tax_details` VALUES ('58', '20', '13', '2009-01-29', '3', '25', '1', '0', '40', '10', 'gg');
INSERT INTO `0_trans_tax_details` VALUES ('59', '2', '6', '2009-01-29', '3', '25', '1', '0', '40', '10', '');


### Structure of table `0_users` ###

DROP TABLE IF EXISTS `0_users`;

CREATE TABLE `0_users` (
  `user_id` varchar(60) NOT NULL default '',
  `password` varchar(100) NOT NULL default '',
  `real_name` varchar(100) NOT NULL default '',
  `full_access` int(11) NOT NULL default '1',
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
  `query_size` tinyint(1) default '10',
  `graphic_links` tinyint(1) default '1',
  `pos` smallint(6) default '1',
  `print_profile` varchar(30) NOT NULL default '1',
  `rep_popup` tinyint(1) default '1',
  `sticky_doc_date` tinyint(1) default '0',
  PRIMARY KEY  (`user_id`)
) TYPE=MyISAM  ;


### Data of table `0_users` ###

INSERT INTO `0_users` VALUES ('demouser', '5f4dcc3b5aa765d61d8327deb882cf99', 'Demo User', '1', '999-999-999', 'demo@demo.nu', 'en_US', '0', '0', '0', '0', 'default', 'Letter', '2', '2', '3', '1', '1', '0', '0', '2008-02-06 19:02:35', '10', '1', '1', '1', '1', '0');
INSERT INTO `0_users` VALUES ('admin', '5f4dcc3b5aa765d61d8327deb882cf99', 'Administrator', '2', '', 'adm@adm.com', 'en_US', '0', '0', '0', '0', 'default', 'Letter', '2', '2', '4', '1', '1', '0', '0', '2009-01-30 09:39:03', '10', '1', '1', '', '1', '0');


### Structure of table `0_voided` ###

DROP TABLE IF EXISTS `0_voided`;

CREATE TABLE `0_voided` (
  `type` int(11) NOT NULL default '0',
  `id` int(11) NOT NULL default '0',
  `date_` date NOT NULL default '0000-00-00',
  `memo_` tinytext NOT NULL,
  UNIQUE KEY `id` (`type`,`id`)
) TYPE=InnoDB  ;


### Data of table `0_voided` ###

INSERT INTO `0_voided` VALUES ('1', '2', '2009-01-10', '');
INSERT INTO `0_voided` VALUES ('1', '3', '2009-01-10', '');
INSERT INTO `0_voided` VALUES ('1', '4', '2009-01-10', '');
INSERT INTO `0_voided` VALUES ('1', '5', '2009-01-10', '');
INSERT INTO `0_voided` VALUES ('1', '6', '2009-01-10', '');


### Structure of table `0_wo_issue_items` ###

DROP TABLE IF EXISTS `0_wo_issue_items`;

CREATE TABLE `0_wo_issue_items` (
  `id` int(11) NOT NULL auto_increment,
  `stock_id` varchar(40) default NULL,
  `issue_id` int(11) default NULL,
  `qty_issued` double default NULL,
  PRIMARY KEY  (`id`)
) TYPE=InnoDB AUTO_INCREMENT=2  AUTO_INCREMENT=2 ;


### Data of table `0_wo_issue_items` ###

INSERT INTO `0_wo_issue_items` VALUES ('1', '102', '1', '10');


### Structure of table `0_wo_issues` ###

DROP TABLE IF EXISTS `0_wo_issues`;

CREATE TABLE `0_wo_issues` (
  `issue_no` int(11) NOT NULL auto_increment,
  `workorder_id` int(11) NOT NULL default '0',
  `reference` varchar(100) default NULL,
  `issue_date` date default NULL,
  `loc_code` varchar(5) default NULL,
  `workcentre_id` int(11) default NULL,
  PRIMARY KEY  (`issue_no`)
) TYPE=InnoDB AUTO_INCREMENT=2  AUTO_INCREMENT=2 ;


### Data of table `0_wo_issues` ###

INSERT INTO `0_wo_issues` VALUES ('1', '3', '1', '2006-01-20', 'DEF', '1');


### Structure of table `0_wo_manufacture` ###

DROP TABLE IF EXISTS `0_wo_manufacture`;

CREATE TABLE `0_wo_manufacture` (
  `id` int(11) NOT NULL auto_increment,
  `reference` varchar(100) default NULL,
  `workorder_id` int(11) NOT NULL default '0',
  `quantity` double NOT NULL default '0',
  `date_` date NOT NULL default '0000-00-00',
  PRIMARY KEY  (`id`)
) TYPE=InnoDB AUTO_INCREMENT=3  AUTO_INCREMENT=3 ;


### Data of table `0_wo_manufacture` ###

INSERT INTO `0_wo_manufacture` VALUES ('1', 'ab200', '3', '50', '2007-01-30');
INSERT INTO `0_wo_manufacture` VALUES ('2', 'ab201', '5', '20', '2007-01-30');


### Structure of table `0_wo_requirements` ###

DROP TABLE IF EXISTS `0_wo_requirements`;

CREATE TABLE `0_wo_requirements` (
  `id` int(11) NOT NULL auto_increment,
  `workorder_id` int(11) NOT NULL default '0',
  `stock_id` char(20) NOT NULL default '',
  `workcentre` int(11) NOT NULL default '0',
  `units_req` double NOT NULL default '1',
  `std_cost` double NOT NULL default '0',
  `loc_code` char(5) NOT NULL default '',
  `units_issued` double NOT NULL default '0',
  PRIMARY KEY  (`id`)
) TYPE=InnoDB AUTO_INCREMENT=29  AUTO_INCREMENT=29 ;


### Data of table `0_wo_requirements` ###

INSERT INTO `0_wo_requirements` VALUES ('1', '1', '102', '1', '1', '0', 'DEF', '20');
INSERT INTO `0_wo_requirements` VALUES ('2', '1', '103', '1', '1', '0', 'DEF', '20');
INSERT INTO `0_wo_requirements` VALUES ('3', '1', '104', '1', '1', '0', 'DEF', '20');
INSERT INTO `0_wo_requirements` VALUES ('4', '1', '201', '1', '1', '0', 'DEF', '20');
INSERT INTO `0_wo_requirements` VALUES ('5', '2', '102', '1', '1', '0', 'DEF', '5');
INSERT INTO `0_wo_requirements` VALUES ('6', '2', '103', '1', '1', '0', 'DEF', '5');
INSERT INTO `0_wo_requirements` VALUES ('7', '2', '104', '1', '1', '0', 'DEF', '5');
INSERT INTO `0_wo_requirements` VALUES ('8', '2', '201', '1', '1', '0', 'DEF', '5');
INSERT INTO `0_wo_requirements` VALUES ('9', '3', '102', '1', '1', '0', 'DEF', '0');
INSERT INTO `0_wo_requirements` VALUES ('10', '3', '103', '1', '1', '0', 'DEF', '0');
INSERT INTO `0_wo_requirements` VALUES ('11', '3', '104', '1', '1', '0', 'DEF', '0');
INSERT INTO `0_wo_requirements` VALUES ('12', '3', '201', '1', '1', '0', 'DEF', '0');
INSERT INTO `0_wo_requirements` VALUES ('13', '4', '102', '1', '1', '0', 'DEF', '5');
INSERT INTO `0_wo_requirements` VALUES ('14', '4', '103', '1', '1', '0', 'DEF', '5');
INSERT INTO `0_wo_requirements` VALUES ('15', '4', '104', '1', '1', '0', 'DEF', '5');
INSERT INTO `0_wo_requirements` VALUES ('16', '4', '201', '1', '1', '0', 'DEF', '5');
INSERT INTO `0_wo_requirements` VALUES ('17', '5', '102', '1', '1', '0', 'DEF', '0');
INSERT INTO `0_wo_requirements` VALUES ('18', '5', '103', '1', '1', '0', 'DEF', '0');
INSERT INTO `0_wo_requirements` VALUES ('19', '5', '104', '1', '1', '0', 'DEF', '0');
INSERT INTO `0_wo_requirements` VALUES ('20', '5', '201', '1', '1', '0', 'DEF', '0');
INSERT INTO `0_wo_requirements` VALUES ('21', '6', '102', '1', '1', '0', 'DEF', '10');
INSERT INTO `0_wo_requirements` VALUES ('22', '6', '103', '1', '1', '0', 'DEF', '10');
INSERT INTO `0_wo_requirements` VALUES ('23', '6', '104', '1', '1', '0', 'DEF', '10');
INSERT INTO `0_wo_requirements` VALUES ('24', '6', '201', '1', '1', '0', 'DEF', '10');
INSERT INTO `0_wo_requirements` VALUES ('25', '7', '102', '1', '1', '0', 'DEF', '1');
INSERT INTO `0_wo_requirements` VALUES ('26', '7', '103', '1', '1', '0', 'DEF', '1');
INSERT INTO `0_wo_requirements` VALUES ('27', '7', '104', '1', '1', '0', 'DEF', '1');
INSERT INTO `0_wo_requirements` VALUES ('28', '7', '201', '1', '1', '0', 'DEF', '1');


### Structure of table `0_workcentres` ###

DROP TABLE IF EXISTS `0_workcentres`;

CREATE TABLE `0_workcentres` (
  `id` int(11) NOT NULL auto_increment,
  `name` char(40) NOT NULL default '',
  `description` char(50) NOT NULL default '',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) TYPE=MyISAM AUTO_INCREMENT=2  AUTO_INCREMENT=2 ;


### Data of table `0_workcentres` ###

INSERT INTO `0_workcentres` VALUES ('1', 'work centre', '', '0');


### Structure of table `0_workorders` ###

DROP TABLE IF EXISTS `0_workorders`;

CREATE TABLE `0_workorders` (
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
) TYPE=InnoDB AUTO_INCREMENT=8  AUTO_INCREMENT=8 ;


### Data of table `0_workorders` ###

INSERT INTO `0_workorders` VALUES ('1', '1', 'DEF', '20', '3400', '2006-01-18', '0', '2006-01-18', '2006-01-18', '20', '1', '1', '0');
INSERT INTO `0_workorders` VALUES ('2', '2', 'DEF', '5', '3400', '2006-01-18', '0', '2006-01-18', '2006-01-18', '5', '1', '1', '0');
INSERT INTO `0_workorders` VALUES ('3', '3', 'DEF', '50', '3400', '2006-01-18', '2', '2006-02-07', '2006-01-20', '50', '1', '1', '0');
INSERT INTO `0_workorders` VALUES ('4', '4', 'DEF', '5', '3400', '2007-01-30', '0', '2007-01-30', '2007-01-30', '5', '1', '1', '0');
INSERT INTO `0_workorders` VALUES ('5', '5', 'DEF', '20', '3400', '2007-01-30', '2', '2007-02-19', '2007-01-30', '20', '1', '1', '0');
INSERT INTO `0_workorders` VALUES ('6', '6', 'DEF', '10', '3400', '2008-02-28', '0', '2008-02-28', '2008-02-28', '10', '1', '1', '234');
INSERT INTO `0_workorders` VALUES ('7', '7', 'DEF', '1', '3400', '2008-03-07', '0', '2008-03-07', '2008-03-07', '1', '1', '1', '0');
