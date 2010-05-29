# Built by FrontAccounting 2.1.2
# http://frontaccounting.net
# Company: Company name
# User: Administrator
# COA: en_US-new.sql


### Structure of table `0_areas` ###

DROP TABLE IF EXISTS `0_areas`;

CREATE TABLE `0_areas` (
  `area_code` int(11) NOT NULL auto_increment,
  `description` varchar(60) NOT NULL default '',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`area_code`),
  UNIQUE KEY `description` (`description`)
) TYPE=MyISAM AUTO_INCREMENT=2 ;


### Data of table `0_areas` ###

INSERT INTO `0_areas` VALUES ('1', 'Global', '0');

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

### Structure of table `0_audit_trail` ###

DROP TABLE IF EXISTS `0_audit_trail`;

CREATE TABLE `0_audit_trail` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `type` smallint(6) unsigned NOT NULL default '0',
  `trans_no` int(11) unsigned NOT NULL default '0',
  `user` smallint(6) unsigned NOT NULL default '0',
  `stamp` timestamp NOT NULL,
  `description` varchar(60) default NULL,
  `fiscal_year` int(11) NOT NULL,
  `gl_date` date NOT NULL default '0000-00-00',
  `gl_seq` int(11) unsigned default NULL,
   PRIMARY KEY (`id`),
  KEY (`fiscal_year`, `gl_seq`)
) TYPE=InnoDB  ;

### Data of table `0_audit_trail` ###



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
  `dflt_curr_act` TINYINT(1) NOT NULL default '0',
  `id` smallint(6) NOT NULL auto_increment,
  `last_reconciled_date` timestamp NOT NULL default '0000-00-00 00:00:00',
  `ending_reconcile_balance` double NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `bank_account_name` (`bank_account_name`),
  KEY `bank_account_number` (`bank_account_number`)
) TYPE=MyISAM AUTO_INCREMENT=3 ;


### Data of table `0_bank_accounts` ###

INSERT INTO `0_bank_accounts` VALUES ('1060', '0', 'Current account', 'N/A', 'N/A', '', 'USD', '1', '1', '0000-00-00 00:00:00', '0', '0');
INSERT INTO `0_bank_accounts` VALUES ('1065', '3', 'Petty Cash account', 'N/A', 'N/A', '', 'USD', '0', '2', '0000-00-00 00:00:00', '0', '0');


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
) TYPE=InnoDB  AUTO_INCREMENT=1 ;


### Data of table `0_bank_trans` ###



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
) TYPE=MyISAM  AUTO_INCREMENT=1 ;


### Data of table `0_bom` ###



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
  `ctype` tinyint(1) NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`cid`)
) TYPE=MyISAM  ;


### Data of table `0_chart_class` ###

INSERT INTO `0_chart_class` VALUES ('1', 'Assets', '1', '0');
INSERT INTO `0_chart_class` VALUES ('2', 'Liabilities', '2', '0');
INSERT INTO `0_chart_class` VALUES ('3', 'Income', '4', '0');
INSERT INTO `0_chart_class` VALUES ('4', 'Costs', '6', '0');

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

INSERT INTO `0_chart_master` VALUES ('1060', '', 'Checking Account', '1', '0');
INSERT INTO `0_chart_master` VALUES ('1065', '', 'Petty Cash', '1', '0');
INSERT INTO `0_chart_master` VALUES ('1200', '', 'Accounts Receivables', '1', '0');
INSERT INTO `0_chart_master` VALUES ('1205', '', 'Allowance for doubtful accounts', '1', '0');
INSERT INTO `0_chart_master` VALUES ('1510', '', 'Inventory', '2', '0');
INSERT INTO `0_chart_master` VALUES ('1520', '', 'Stocks of Raw Materials', '2', '0');
INSERT INTO `0_chart_master` VALUES ('1530', '', 'Stocks of Work In Progress', '2', '0');
INSERT INTO `0_chart_master` VALUES ('1540', '', 'Stocks of Finsihed Goods', '2', '0');
INSERT INTO `0_chart_master` VALUES ('1550', '', 'Goods Received Clearing account', '2', '0');
INSERT INTO `0_chart_master` VALUES ('1820', '', 'Office Furniture &amp; Equipment', '3', '0');
INSERT INTO `0_chart_master` VALUES ('1825', '', 'Accum. Amort. -Furn. &amp; Equip.', '3', '0');
INSERT INTO `0_chart_master` VALUES ('1840', '', 'Vehicle', '3', '0');
INSERT INTO `0_chart_master` VALUES ('1845', '', 'Accum. Amort. -Vehicle', '3', '0');
INSERT INTO `0_chart_master` VALUES ('2100', '', 'Accounts Payable', '4', '0');
INSERT INTO `0_chart_master` VALUES ('2110', '', 'Accrued Income Tax - Federal', '4', '0');
INSERT INTO `0_chart_master` VALUES ('2120', '', 'Accrued Income Tax - State', '4', '0');
INSERT INTO `0_chart_master` VALUES ('2130', '', 'Accrued Franchise Tax', '4', '0');
INSERT INTO `0_chart_master` VALUES ('2140', '', 'Accrued Real &amp; Personal Prop Tax', '4', '0');
INSERT INTO `0_chart_master` VALUES ('2145', '', 'Purchase Tax', '4', '0');
INSERT INTO `0_chart_master` VALUES ('2150', '', 'Sales Tax', '4', '0');
INSERT INTO `0_chart_master` VALUES ('2160', '', 'Accrued Use Tax Payable', '4', '0');
INSERT INTO `0_chart_master` VALUES ('2210', '', 'Accrued Wages', '4', '0');
INSERT INTO `0_chart_master` VALUES ('2220', '', 'Accrued Comp Time', '4', '0');
INSERT INTO `0_chart_master` VALUES ('2230', '', 'Accrued Holiday Pay', '4', '0');
INSERT INTO `0_chart_master` VALUES ('2240', '', 'Accrued Vacation Pay', '4', '0');
INSERT INTO `0_chart_master` VALUES ('2310', '', 'Accr. Benefits - 401K', '4', '0');
INSERT INTO `0_chart_master` VALUES ('2320', '', 'Accr. Benefits - Stock Purchase', '4', '0');
INSERT INTO `0_chart_master` VALUES ('2330', '', 'Accr. Benefits - Med, Den', '4', '0');
INSERT INTO `0_chart_master` VALUES ('2340', '', 'Accr. Benefits - Payroll Taxes', '4', '0');
INSERT INTO `0_chart_master` VALUES ('2350', '', 'Accr. Benefits - Credit Union', '4', '0');
INSERT INTO `0_chart_master` VALUES ('2360', '', 'Accr. Benefits - Savings Bond', '4', '0');
INSERT INTO `0_chart_master` VALUES ('2370', '', 'Accr. Benefits - Garnish', '4', '0');
INSERT INTO `0_chart_master` VALUES ('2380', '', 'Accr. Benefits - Charity Cont.', '4', '0');
INSERT INTO `0_chart_master` VALUES ('2620', '', 'Bank Loans', '5', '0');
INSERT INTO `0_chart_master` VALUES ('2680', '', 'Loans from Shareholders', '5', '0');
INSERT INTO `0_chart_master` VALUES ('3350', '', 'Common Shares', '6', '0');
INSERT INTO `0_chart_master` VALUES ('3590', '', 'Retained Earnings - prior years', '7', '0');
INSERT INTO `0_chart_master` VALUES ('4010', '', 'Sales', '8', '0');
INSERT INTO `0_chart_master` VALUES ('4430', '', 'Shipping &amp; Handling', '9', '0');
INSERT INTO `0_chart_master` VALUES ('4440', '', 'Interest', '9', '0');
INSERT INTO `0_chart_master` VALUES ('4450', '', 'Foreign Exchange Gain', '9', '0');
INSERT INTO `0_chart_master` VALUES ('4500', '', 'Prompt Payment Discounts', '9', '0');
INSERT INTO `0_chart_master` VALUES ('4510', '', 'Discounts Given', '9', '0');
INSERT INTO `0_chart_master` VALUES ('5010', '', 'Cost of Goods Sold - Retail', '10', '0');
INSERT INTO `0_chart_master` VALUES ('5020', '', 'Material Usage Varaiance', '10', '0');
INSERT INTO `0_chart_master` VALUES ('5030', '', 'Consumable Materials', '10', '0');
INSERT INTO `0_chart_master` VALUES ('5040', '', 'Purchase price Variance', '10', '0');
INSERT INTO `0_chart_master` VALUES ('5050', '', 'Purchases of materials', '10', '0');
INSERT INTO `0_chart_master` VALUES ('5060', '', 'Discounts Received', '10', '0');
INSERT INTO `0_chart_master` VALUES ('5100', '', 'Freight', '10', '0');
INSERT INTO `0_chart_master` VALUES ('5410', '', 'Wages &amp; Salaries', '11', '0');
INSERT INTO `0_chart_master` VALUES ('5420', '', 'Wages - Overtime', '11', '0');
INSERT INTO `0_chart_master` VALUES ('5430', '', 'Benefits - Comp Time', '11', '0');
INSERT INTO `0_chart_master` VALUES ('5440', '', 'Benefits - Payroll Taxes', '11', '0');
INSERT INTO `0_chart_master` VALUES ('5450', '', 'Benefits - Workers Comp', '11', '0');
INSERT INTO `0_chart_master` VALUES ('5460', '', 'Benefits - Pension', '11', '0');
INSERT INTO `0_chart_master` VALUES ('5470', '', 'Benefits - General Benefits', '11', '0');
INSERT INTO `0_chart_master` VALUES ('5510', '', 'Inc Tax Exp - Federal', '11', '0');
INSERT INTO `0_chart_master` VALUES ('5520', '', 'Inc Tax Exp - State', '11', '0');
INSERT INTO `0_chart_master` VALUES ('5530', '', 'Taxes - Real Estate', '11', '0');
INSERT INTO `0_chart_master` VALUES ('5540', '', 'Taxes - Personal Property', '11', '0');
INSERT INTO `0_chart_master` VALUES ('5550', '', 'Taxes - Franchise', '11', '0');
INSERT INTO `0_chart_master` VALUES ('5560', '', 'Taxes - Foreign Withholding', '11', '0');
INSERT INTO `0_chart_master` VALUES ('5610', '', 'Accounting &amp; Legal', '12', '0');
INSERT INTO `0_chart_master` VALUES ('5615', '', 'Advertising &amp; Promotions', '12', '0');
INSERT INTO `0_chart_master` VALUES ('5620', '', 'Bad Debts', '12', '0');
INSERT INTO `0_chart_master` VALUES ('5660', '', 'Amortization Expense', '12', '0');
INSERT INTO `0_chart_master` VALUES ('5685', '', 'Insurance', '12', '0');
INSERT INTO `0_chart_master` VALUES ('5690', '', 'Interest &amp; Bank Charges', '12', '0');
INSERT INTO `0_chart_master` VALUES ('5700', '', 'Office Supplies', '12', '0');
INSERT INTO `0_chart_master` VALUES ('5760', '', 'Rent', '12', '0');
INSERT INTO `0_chart_master` VALUES ('5765', '', 'Repair &amp; Maintenance', '12', '0');
INSERT INTO `0_chart_master` VALUES ('5780', '', 'Telephone', '12', '0');
INSERT INTO `0_chart_master` VALUES ('5785', '', 'Travel &amp; Entertainment', '12', '0');
INSERT INTO `0_chart_master` VALUES ('5790', '', 'Utilities', '12', '0');
INSERT INTO `0_chart_master` VALUES ('5795', '', 'Registrations', '12', '0');
INSERT INTO `0_chart_master` VALUES ('5800', '', 'Licenses', '12', '0');
INSERT INTO `0_chart_master` VALUES ('5810', '', 'Foreign Exchange Loss', '12', '0');
INSERT INTO `0_chart_master` VALUES ('9990', '', 'Year Profit/Loss', '12', '0');


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
) TYPE=MyISAM AUTO_INCREMENT=13 ;


### Data of table `0_chart_types` ###

INSERT INTO `0_chart_types` VALUES ('1', 'Current Assets', '1', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('2', 'Inventory Assets', '1', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('3', 'Capital Assets', '1', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('4', 'Current Liabilities', '2', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('5', 'Long Term Liabilities', '2', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('6', 'Share Capital', '2', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('7', 'Retained Earnings', '2', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('8', 'Sales Revenue', '3', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('9', 'Other Revenue', '3', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('10', 'Cost of Goods Sold', '4', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('11', 'Payroll Expenses', '4', '-1', '0');
INSERT INTO `0_chart_types` VALUES ('12', 'General &amp; Administrative expenses', '4', -1, '0');


### Structure of table `0_comments` ###

DROP TABLE IF EXISTS `0_comments`;

CREATE TABLE `0_comments` (
  `type` int(11) NOT NULL default '0',
  `id` int(11) NOT NULL default '0',
  `date_` date default '0000-00-00',
  `memo_` tinytext
) TYPE=InnoDB  ;


### Data of table `0_comments` ###



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
  `bank_charge_act` varchar(11) NOT NULL default '',
  `exchange_diff_act` varchar(11) NOT NULL default '',
  `profit_loss_year_act` varchar(11) NOT NULL default '',
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
  `version_id` varchar(11) NOT NULL default '',
  `time_zone` tinyint(1) NOT NULL default '0',
  `add_pct` int(5) NOT NULL default '-1',
  `round_to` int(5) NOT NULL default '1',
  `login_tout` SMALLINT(6) NOT NULL DEFAULT '600',
  PRIMARY KEY  (`coy_code`)
) TYPE=MyISAM  ;

### Data of table `0_company` ###

INSERT INTO `0_company` VALUES ('1', 'Company name', '', '', '1', '1', 'N/A', '', '', '', '', '', 'USD', '1200', '5060', '2100', '5690', '4450', '9990', '3590', '4430', '4010', '4510', '4500', '1510', '5010', '5040', '4010', '1530', '5000', '0', '10', '10', '1000', '20', '20', '30', '1', '3', '0', '0', '0', '1', '0', '0', '', '1', '2.2', '0', '-1', '1', '600');

### Structure of table `0_credit_status` ###

DROP TABLE IF EXISTS `0_credit_status`;

CREATE TABLE `0_credit_status` (
  `id` int(11) NOT NULL auto_increment,
  `reason_description` char(100) NOT NULL default '',
  `dissallow_invoices` tinyint(1) NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `reason_description` (`reason_description`)
) TYPE=MyISAM AUTO_INCREMENT=5 ;


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
  `auto_update` tinyint(1) NOT NULL default '1',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`curr_abrev`)
) TYPE=MyISAM  ;


### Data of table `0_currencies` ###

INSERT INTO `0_currencies` VALUES ('US Dollars', 'USD', '$', 'United States', 'Cents', '1', '0');
INSERT INTO `0_currencies` VALUES ('CA Dollars', 'CAD', '$', 'Canada', 'Cents', '1', '0');
INSERT INTO `0_currencies` VALUES ('Euro', 'EUR', '?', 'Europe', 'Cents', '1', '0');
INSERT INTO `0_currencies` VALUES ('Pounds', 'GBP', '?', 'England', 'Pence', '1', '0');

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
) TYPE=InnoDB  AUTO_INCREMENT=1 ;


### Data of table `0_cust_allocations` ###



### Structure of table `0_cust_branch` ###

DROP TABLE IF EXISTS `0_cust_branch`;

CREATE TABLE `0_cust_branch` (
  `branch_code` int(11) NOT NULL auto_increment,
  `debtor_no` int(11) NOT NULL default '0',
  `br_name` varchar(60) NOT NULL default '',
  `branch_ref` 	varchar(30) NOT NULL default '',
  `br_address` tinytext NOT NULL,
  `area` int(11) default NULL,
  `salesman` int(11) NOT NULL default '0',
  `phone` varchar(30) NOT NULL default '',
  `phone2` varchar(30) NOT NULL default '',
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
  `notes` tinytext NOT NULL,
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`branch_code`,`debtor_no`),
  KEY `branch_code` (`branch_code`),
  KEY `br_name` (`br_name`)
) TYPE=MyISAM  AUTO_INCREMENT=1 ;


### Data of table `0_cust_branch` ###



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
) TYPE=InnoDB  AUTO_INCREMENT=1 ;


### Data of table `0_debtor_trans_details` ###



### Structure of table `0_debtors_master` ###

DROP TABLE IF EXISTS `0_debtors_master`;

CREATE TABLE `0_debtors_master` (
  `debtor_no` int(11) NOT NULL auto_increment,
  `name` varchar(100) NOT NULL default '',
  `debtor_ref` varchar(30) NOT NULL,
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
  `notes` tinytext NOT NULL,
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`debtor_no`),
  UNIQUE KEY `name` (`name`)
) TYPE=MyISAM  AUTO_INCREMENT=1 ;


### Data of table `0_debtors_master` ###



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
) TYPE=InnoDB  AUTO_INCREMENT=1 ;


### Data of table `0_dimensions` ###



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
) TYPE=MyISAM  AUTO_INCREMENT=1 ;


### Data of table `0_exchange_rates` ###



### Structure of table `0_fiscal_year` ###

DROP TABLE IF EXISTS `0_fiscal_year`;

CREATE TABLE `0_fiscal_year` (
  `id` int(11) NOT NULL auto_increment,
  `begin` date default '0000-00-00',
  `end` date default '0000-00-00',
  `closed` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`)
) TYPE=InnoDB AUTO_INCREMENT=2 ;


### Data of table `0_fiscal_year` ###

INSERT INTO `0_fiscal_year` VALUES ('1', '2008-01-01', '2008-12-31', '0');
INSERT INTO `0_fiscal_year` VALUES ('2', '2009-01-01', '2009-12-31', '0');
INSERT INTO `0_fiscal_year` VALUES ('3', '2010-01-01', '2010-12-31', '0');


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
) TYPE=InnoDB  AUTO_INCREMENT=1 ;


### Data of table `0_gl_trans` ###



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
) TYPE=InnoDB  AUTO_INCREMENT=1 ;


### Data of table `0_grn_batch` ###



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
) TYPE=InnoDB  AUTO_INCREMENT=1 ;


### Data of table `0_grn_items` ###



### Structure of table `0_groups` ###

DROP TABLE IF EXISTS `0_groups`;

CREATE TABLE `0_groups` (
  `id` smallint(6) unsigned NOT NULL auto_increment,
  `description` varchar(60) NOT NULL default '',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `description` (`description`)
) TYPE=MyISAM AUTO_INCREMENT=4 ;


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
) TYPE=MyISAM  AUTO_INCREMENT=1 ;


### Data of table `0_item_codes` ###



### Structure of table `0_item_tax_type_exemptions` ###

DROP TABLE IF EXISTS `0_item_tax_type_exemptions`;

CREATE TABLE `0_item_tax_type_exemptions` (
  `item_tax_type_id` int(11) NOT NULL default '0',
  `tax_type_id` int(11) NOT NULL default '0',
  PRIMARY KEY  (`item_tax_type_id`,`tax_type_id`)
) TYPE=InnoDB  ;


### Data of table `0_item_tax_type_exemptions` ###



### Structure of table `0_item_tax_types` ###

DROP TABLE IF EXISTS `0_item_tax_types`;

CREATE TABLE `0_item_tax_types` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(60) NOT NULL default '',
  `exempt` tinyint(1) NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) TYPE=InnoDB AUTO_INCREMENT=2 ;


### Data of table `0_item_tax_types` ###

INSERT INTO `0_item_tax_types` VALUES ('1', 'Regular', '0', '0');


### Structure of table `0_item_units` ###

DROP TABLE IF EXISTS `0_item_units`;

CREATE TABLE `0_item_units` (
  `abbr` varchar(20) NOT NULL,
  `name` varchar(40) NOT NULL,
  `decimals` tinyint(2) NOT NULL,
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`abbr`),
  UNIQUE KEY `name` (`name`)
) TYPE=MyISAM  ;


### Data of table `0_item_units` ###

INSERT INTO `0_item_units` VALUES ('ea.', 'Each', '0', '0');


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



### Structure of table `0_locations` ###

DROP TABLE IF EXISTS `0_locations`;

CREATE TABLE `0_locations` (
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
) TYPE=MyISAM  ;


### Data of table `0_locations` ###

INSERT INTO `0_locations` VALUES ('DEF', 'Default', 'N/A', '', '', '', '', '', '0');


### Structure of table `0_movement_types` ###

DROP TABLE IF EXISTS `0_movement_types`;

CREATE TABLE `0_movement_types` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(60) NOT NULL default '',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) TYPE=MyISAM AUTO_INCREMENT=2 ;


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
) TYPE=MyISAM AUTO_INCREMENT=5 ;


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
) TYPE=MyISAM  AUTO_INCREMENT=1 ;


### Data of table `0_prices` ###



### Structure of table `0_print_profiles` ###

DROP TABLE IF EXISTS `0_print_profiles`;

CREATE TABLE `0_print_profiles` (
  `id` smallint(6) unsigned NOT NULL auto_increment,
  `profile` varchar(30) NOT NULL,
  `report` varchar(5) default NULL,
  `printer` tinyint(3) unsigned default NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `profile` (`profile`,`report`)
) TYPE=MyISAM AUTO_INCREMENT=10 ;


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
) TYPE=MyISAM AUTO_INCREMENT=4 ;


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
) TYPE=InnoDB  AUTO_INCREMENT=1 ;


### Data of table `0_purch_order_details` ###



### Structure of table `0_purch_orders` ###

DROP TABLE IF EXISTS `0_purch_orders`;

CREATE TABLE `0_purch_orders` (
  `order_no` int(11) NOT NULL auto_increment,
  `supplier_id` int(11) NOT NULL default '0',
  `comments` tinytext,
  `ord_date` date NOT NULL default '0000-00-00',
  `reference` tinytext NOT NULL,
  `requisition_no` tinytext,
  `into_stock_location` varchar(5) NOT NULL default '',
  `delivery_address` tinytext NOT NULL,
  PRIMARY KEY  (`order_no`)
) TYPE=InnoDB  AUTO_INCREMENT=1 ;


### Data of table `0_purch_orders` ###



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
) TYPE=MyISAM AUTO_INCREMENT=4 ;


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
) TYPE=MyISAM AUTO_INCREMENT=7 ;


### Data of table `0_quick_entry_lines` ###

INSERT INTO `0_quick_entry_lines` VALUES ('1', '1', '0', 't-', '1', '0', '0');
INSERT INTO `0_quick_entry_lines` VALUES ('2', '2', '0', 't-', '1', '0', '0');
INSERT INTO `0_quick_entry_lines` VALUES ('3', '3', '0', 't-', '1', '0', '0');
INSERT INTO `0_quick_entry_lines` VALUES ('4', '3', '0', '=', '4010', '0', '0');
INSERT INTO `0_quick_entry_lines` VALUES ('5', '1', '0', '=', '5765', '0', '0');
INSERT INTO `0_quick_entry_lines` VALUES ('6', '2', '0', '=', '5780', '0', '0');


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



### Structure of table `0_sales_order_details` ###

DROP TABLE IF EXISTS `0_sales_order_details`;

CREATE TABLE `0_sales_order_details` (
  `id` int(11) NOT NULL auto_increment,
  `order_no` int(11) NOT NULL default '0',
  `trans_type` smallint(6) NOT NULL default '30',
  `stk_code` varchar(20) NOT NULL default '',
  `description` tinytext,
  `qty_sent` double NOT NULL default '0',
  `unit_price` double NOT NULL default '0',
  `quantity` double NOT NULL default '0',
  `discount_percent` double NOT NULL default '0',
  PRIMARY KEY  (`id`)
) TYPE=InnoDB  AUTO_INCREMENT=1 ;


### Data of table `0_sales_order_details` ###



### Structure of table `0_sales_orders` ###

DROP TABLE IF EXISTS `0_sales_orders`;

CREATE TABLE `0_sales_orders` (
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
  PRIMARY KEY  (`trans_type`, `order_no`)
) TYPE=InnoDB;


### Data of table `0_sales_orders` ###



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
) TYPE=MyISAM AUTO_INCREMENT=2 ;


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
) TYPE=MyISAM AUTO_INCREMENT=3 ;


### Data of table `0_sales_types` ###

INSERT INTO `0_sales_types` VALUES ('1', 'Retail', '1', '1', '0');
INSERT INTO `0_sales_types` VALUES ('2', 'Wholesale', '0', '0.7', '0');


### Structure of table `0_salesman` ###

DROP TABLE IF EXISTS `0_salesman`;

CREATE TABLE `0_salesman` (
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
) TYPE=MyISAM AUTO_INCREMENT=2 ;


### Data of table `0_salesman` ###

INSERT INTO `0_salesman` VALUES ('1', 'Sales Person', '', '', '', '5', '1000', '4', '0');

### Structure of table `0_security_roles` ###

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
) TYPE=MyISAM  AUTO_INCREMENT=11 ;

--
-- Data of table `0_security_roles`
--

INSERT INTO `0_security_roles` VALUES (1, 'Inquiries', 'Inquiries', '768;2816;3072;3328;5632;5888;8192;8448;10752;11008;13312;15872;16128', '257;258;259;260;513;514;515;516;517;518;519;520;521;522;523;524;525;773;774;2822;3073;3075;3076;3077;3329;3330;3331;3332;3333;3334;3335;5377;5633;5640;5889;5890;5891;7937;7938;7939;7940;8193;8194;8450;8451;10497;10753;11009;11010;11012;13313;13315;15617;15618;15619;15620;15621;15622;15623;15624;15625;15626;15873;15882;16129;16130;16131;16132', 0);
INSERT INTO `0_security_roles` VALUES (2, 'System Administrator', 'System Administrator', '256;512;768;2816;3072;3328;5376;5632;5888;7936;8192;8448;10496;10752;11008;13056;13312;15616;15872;16128', '257;258;259;260;513;514;515;516;517;518;519;520;521;522;523;524;525;769;770;771;772;773;774;2817;2818;2819;2820;2821;2822;2823;3073;3074;3082;3075;3076;3077;3078;3079;3080;3081;3329;3330;3331;3332;3333;3334;3335;5377;5633;5634;5635;5636;5637;5641;5638;5639;5640;5889;5890;5891;7937;7938;7939;7940;8193;8194;8195;8196;8197;8449;8450;8451;10497;10753;10754;10755;10756;10757;11009;11010;11011;11012;13057;13313;13314;13315;15617;15618;15619;15620;15621;15622;15623;15624;15628;15625;15626;15627;15873;15874;15875;15876;15877;15878;15879;15880;15883;15881;15882;16129;16130;16131;16132', 0);
INSERT INTO `0_security_roles` VALUES (3, 'Salesman', 'Salesman', '768;3072;5632;8192;15872', '773;774;3073;3075;3081;5633;8194;15873', 0);
INSERT INTO `0_security_roles` VALUES (4, 'Stock Manager', 'Stock Manager', '2816;3072;3328;5632;5888;8192;8448;10752;11008;13312;15872;16128', '2818;2822;3073;3076;3077;3329;3330;3330;3330;3331;3331;3332;3333;3334;3335;5633;5640;5889;5890;5891;8193;8194;8450;8451;10753;11009;11010;11012;13313;13315;15882;16129;16130;16131;16132', 0);
INSERT INTO `0_security_roles` VALUES (5, 'Production Manager', 'Production Manager', '512;2816;3072;3328;5632;5888;8192;8448;10752;11008;13312;15616;15872;16128', '521;523;524;2818;2819;2820;2821;2822;2823;3073;3074;3076;3077;3078;3079;3080;3081;3329;3330;3330;3330;3331;3331;3332;3333;3334;3335;5633;5640;5640;5889;5890;5891;8193;8194;8196;8197;8450;8451;10753;10755;11009;11010;11012;13313;13315;15617;15619;15620;15621;15624;15624;15876;15877;15880;15882;16129;16130;16131;16132', 0);
INSERT INTO `0_security_roles` VALUES (6, 'Purchase Officer', 'Purchase Officer', '512;2816;3072;3328;5376;5632;5888;8192;8448;10752;11008;13312;15616;15872;16128', '521;523;524;2818;2819;2820;2821;2822;2823;3073;3074;3076;3077;3078;3079;3080;3081;3329;3330;3330;3330;3331;3331;3332;3333;3334;3335;5377;5633;5635;5640;5640;5889;5890;5891;8193;8194;8196;8197;8449;8450;8451;10753;10755;11009;11010;11012;13313;13315;15617;15619;15620;15621;15624;15624;15876;15877;15880;15882;16129;16130;16131;16132', 0);
INSERT INTO `0_security_roles` VALUES (7, 'AR Officer', 'AR Officer', '512;768;2816;3072;3328;5632;5888;8192;8448;10752;11008;13312;15616;15872;16128', '521;523;524;771;773;774;2818;2819;2820;2821;2822;2823;3073;3073;3074;3075;3076;3077;3078;3079;3080;3081;3081;3329;3330;3330;3330;3331;3331;3332;3333;3334;3335;5633;5633;5634;5637;5638;5639;5640;5640;5889;5890;5891;8193;8194;8194;8196;8197;8450;8451;10753;10755;11009;11010;11012;13313;13315;15617;15619;15620;15621;15624;15624;15873;15876;15877;15878;15880;15882;16129;16130;16131;16132', 0);
INSERT INTO `0_security_roles` VALUES (8, 'AP Officer', 'AP Officer', '512;2816;3072;3328;5376;5632;5888;8192;8448;10752;11008;13312;15616;15872;16128', '257;258;259;260;521;523;524;769;770;771;772;773;774;2818;2819;2820;2821;2822;2823;3073;3074;3082;3076;3077;3078;3079;3080;3081;3329;3330;3331;3332;3333;3334;3335;5377;5633;5635;5640;5889;5890;5891;7937;7938;7939;7940;8193;8194;8196;8197;8449;8450;8451;10497;10753;10755;11009;11010;11012;13057;13313;13315;15617;15619;15620;15621;15624;15876;15877;15880;15882;16129;16130;16131;16132', 0);
INSERT INTO `0_security_roles` VALUES (9, 'Accountant', 'New Accountant', '512;768;2816;3072;3328;5376;5632;5888;8192;8448;10752;11008;13312;15616;15872;16128', '257;258;259;260;521;523;524;771;772;773;774;2818;2819;2820;2821;2822;2823;3073;3074;3075;3076;3077;3078;3079;3080;3081;3329;3330;3331;3332;3333;3334;3335;5377;5633;5634;5635;5637;5638;5639;5640;5889;5890;5891;7937;7938;7939;7940;8193;8194;8196;8197;8449;8450;8451;10497;10753;10755;11009;11010;11012;13313;13315;15617;15618;15619;15620;15621;15624;15873;15876;15877;15878;15880;15882;16129;16130;16131;16132', 0);
INSERT INTO `0_security_roles` VALUES (10, 'Sub Admin', 'Sub Admin', '512;768;2816;3072;3328;5376;5632;5888;8192;8448;10752;11008;13312;15616;15872;16128', '257;258;259;260;521;523;524;771;772;773;774;2818;2819;2820;2821;2822;2823;3073;3074;3082;3075;3076;3077;3078;3079;3080;3081;3329;3330;3331;3332;3333;3334;3335;5377;5633;5634;5635;5637;5638;5639;5640;5889;5890;5891;7937;7938;7939;7940;8193;8194;8196;8197;8449;8450;8451;10497;10753;10755;11009;11010;11012;13057;13313;13315;15617;15619;15620;15621;15624;15873;15874;15876;15877;15878;15879;15880;15882;16129;16130;16131;16132', 0);

### Structure of table `0_shippers` ###

DROP TABLE IF EXISTS `0_shippers`;

CREATE TABLE `0_shippers` (
  `shipper_id` int(11) NOT NULL auto_increment,
  `shipper_name` varchar(60) NOT NULL default '',
  `phone` varchar(30) NOT NULL default '',
  `phone2` varchar(30) NOT NULL default '',
  `contact` tinytext NOT NULL,
  `address` tinytext NOT NULL,
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`shipper_id`),
  UNIQUE KEY `name` (`shipper_name`)
) TYPE=MyISAM AUTO_INCREMENT=2 ;


### Data of table `0_shippers` ###

INSERT INTO `0_shippers` VALUES ('1', 'Default', '', '', '', '', '0');


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
  `dflt_no_sale` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`category_id`),
  UNIQUE KEY `description` (`description`)
) TYPE=MyISAM AUTO_INCREMENT=5 ;


### Data of table `0_stock_category` ###

INSERT INTO `0_stock_category` VALUES ('1', 'Components', '1', 'each', 'B', '4010', '5010', '1510', '5040', '1530', '0', '0', '0', '0');
INSERT INTO `0_stock_category` VALUES ('2', 'Charges', '1', 'each', 'D', '4010', '5010', '1510', '5040', '1530', '0', '0', '0', '0');
INSERT INTO `0_stock_category` VALUES ('3', 'Systems', '1', 'each', 'M', '4010', '5010', '1510', '5040', '1530', '0', '0', '0', '0');
INSERT INTO `0_stock_category` VALUES ('4', 'Services', '1', 'hrs', 'D', '4010', '5010', '1510', '5040', '1530', '0', '0', '0', '0');


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
  `no_sale` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`stock_id`)
) TYPE=InnoDB  ;


### Data of table `0_stock_master` ###



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
) TYPE=InnoDB  AUTO_INCREMENT=1 ;


### Data of table `0_stock_moves` ###



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
) TYPE=InnoDB  AUTO_INCREMENT=1 ;


### Data of table `0_supp_allocations` ###



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
) TYPE=InnoDB  AUTO_INCREMENT=1 ;


### Data of table `0_supp_invoice_items` ###



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



### Structure of table `0_suppliers` ###

DROP TABLE IF EXISTS `0_suppliers`;

CREATE TABLE `0_suppliers` (
  `supplier_id` int(11) NOT NULL auto_increment,
  `supp_name` varchar(60) NOT NULL default '',
  `supp_ref` varchar(30) NOT NULL default '',
  `address` tinytext NOT NULL,
  `supp_address` tinytext NOT NULL,
  `phone` varchar(30) NOT NULL default '',
  `phone2` varchar(30) NOT NULL default '',
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
) TYPE=MyISAM  AUTO_INCREMENT=1 ;


### Data of table `0_suppliers` ###



### Structure of table `0_sys_types` ###

DROP TABLE IF EXISTS `0_sys_types`;

CREATE TABLE `0_sys_types` (
  `type_id` smallint(6) NOT NULL default '0',
  `type_no` int(11) NOT NULL default '1',
  `next_reference` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`type_id`)
) TYPE=InnoDB  ;


### Data of table `0_sys_types` ###

INSERT INTO `0_sys_types` VALUES ('0', '17', '1');
INSERT INTO `0_sys_types` VALUES ('1', '7', '1');
INSERT INTO `0_sys_types` VALUES ('2', '4', '1');
INSERT INTO `0_sys_types` VALUES ('4', '3', '1');
INSERT INTO `0_sys_types` VALUES ('10', '16', '1');
INSERT INTO `0_sys_types` VALUES ('11', '2', '1');
INSERT INTO `0_sys_types` VALUES ('12', '6', '1');
INSERT INTO `0_sys_types` VALUES ('13', '1', '1');
INSERT INTO `0_sys_types` VALUES ('16', '2', '1');
INSERT INTO `0_sys_types` VALUES ('17', '2', '1');
INSERT INTO `0_sys_types` VALUES ('18', '1', '1');
INSERT INTO `0_sys_types` VALUES ('20', '6', '1');
INSERT INTO `0_sys_types` VALUES ('21', '1', '1');
INSERT INTO `0_sys_types` VALUES ('22', '3', '1');
INSERT INTO `0_sys_types` VALUES ('25', '1', '1');
INSERT INTO `0_sys_types` VALUES ('26', '1', '1');
INSERT INTO `0_sys_types` VALUES ('28', '1', '1');
INSERT INTO `0_sys_types` VALUES ('29', '1', '1');
INSERT INTO `0_sys_types` VALUES ('30', '0', '1');
INSERT INTO `0_sys_types` VALUES ('32', '0', '1');
INSERT INTO `0_sys_types` VALUES ('35', '1', '1');
INSERT INTO `0_sys_types` VALUES ('40', '1', '1');


### Structure of table `0_tax_group_items` ###

DROP TABLE IF EXISTS `0_tax_group_items`;

CREATE TABLE `0_tax_group_items` (
  `tax_group_id` int(11) NOT NULL default '0',
  `tax_type_id` int(11) NOT NULL default '0',
  `rate` double NOT NULL default '0',
  PRIMARY KEY  (`tax_group_id`,`tax_type_id`)
) TYPE=InnoDB  ;


### Data of table `0_tax_group_items` ###

INSERT INTO `0_tax_group_items` VALUES ('1', '1', '5');


### Structure of table `0_tax_groups` ###

DROP TABLE IF EXISTS `0_tax_groups`;

CREATE TABLE `0_tax_groups` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(60) NOT NULL default '',
  `tax_shipping` tinyint(1) NOT NULL default '0',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) TYPE=InnoDB AUTO_INCREMENT=3 ;


### Data of table `0_tax_groups` ###

INSERT INTO `0_tax_groups` VALUES ('1', 'Tax', '0', '0');
INSERT INTO `0_tax_groups` VALUES ('2', 'Tax Exempt', '0', '0');


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
) TYPE=InnoDB AUTO_INCREMENT=2 ;


### Data of table `0_tax_types` ###

INSERT INTO `0_tax_types` VALUES ('1', '5', '2150', '2145', 'Tax', '0');


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
) TYPE=InnoDB AUTO_INCREMENT=1 ;


### Data of table `0_trans_tax_details` ###



### Structure of table `0_users` ###

DROP TABLE IF EXISTS `0_users`;

CREATE TABLE `0_users` (
  `id` SMALLINT(6) AUTO_INCREMENT,
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
  `query_size` tinyint(1) default '10',
  `graphic_links` tinyint(1) default '1',
  `pos` smallint(6) default '1',
  `print_profile` varchar(30) NOT NULL default '1',
  `rep_popup` tinyint(1) default '1',
  `sticky_doc_date` tinyint(1) default '0',
  `startup_tab` varchar(20) NOT NULL default '',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY  (`user_id`)
) TYPE=MyISAM  AUTO_INCREMENT=2;


### Data of table `0_users` ###

INSERT INTO `0_users` VALUES ('1', 'admin', '5f4dcc3b5aa765d61d8327deb882cf99', 'Administrator', '2', '', 'adm@adm.com', 'en_US', '0', '0', '0', '0', 'default', 'Letter', '2', '2', '4', '1', '1', '0', '0', '2008-04-04 12:34:29', '10', '1', '1', '1', '1', '0', 'orders', '0');

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



### Structure of table `0_wo_issue_items` ###

DROP TABLE IF EXISTS `0_wo_issue_items`;

CREATE TABLE `0_wo_issue_items` (
  `id` int(11) NOT NULL auto_increment,
  `stock_id` varchar(40) default NULL,
  `issue_id` int(11) default NULL,
  `qty_issued` double default NULL,
  PRIMARY KEY  (`id`)
) TYPE=InnoDB  AUTO_INCREMENT=1 ;


### Data of table `0_wo_issue_items` ###



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
) TYPE=InnoDB  AUTO_INCREMENT=1 ;


### Data of table `0_wo_issues` ###



### Structure of table `0_wo_manufacture` ###

DROP TABLE IF EXISTS `0_wo_manufacture`;

CREATE TABLE `0_wo_manufacture` (
  `id` int(11) NOT NULL auto_increment,
  `reference` varchar(100) default NULL,
  `workorder_id` int(11) NOT NULL default '0',
  `quantity` double NOT NULL default '0',
  `date_` date NOT NULL default '0000-00-00',
  PRIMARY KEY  (`id`)
) TYPE=InnoDB  AUTO_INCREMENT=1 ;


### Data of table `0_wo_manufacture` ###



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
) TYPE=InnoDB  AUTO_INCREMENT=1 ;


### Data of table `0_wo_requirements` ###



### Structure of table `0_workcentres` ###

DROP TABLE IF EXISTS `0_workcentres`;

CREATE TABLE `0_workcentres` (
  `id` int(11) NOT NULL auto_increment,
  `name` char(40) NOT NULL default '',
  `description` char(50) NOT NULL default '',
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) TYPE=MyISAM  AUTO_INCREMENT=1 ;


### Data of table `0_workcentres` ###



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
) TYPE=InnoDB  AUTO_INCREMENT=1 ;


### Data of table `0_workorders` ###



### Structure of table `0_tags` ###

DROP TABLE IF EXISTS `0_tags`;

CREATE TABLE `0_tags` (
  `id` int(11) NOT NULL auto_increment,
  `type` smallint(6) NOT NULL,
  `name` varchar(30) NOT NULL,
  `description` varchar(60) default NULL,
  `inactive` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY(`type`,`name`)
) TYPE=MyISAM AUTO_INCREMENT=1;


### Data of table `0_chart_tags` ###



### Structure of table `0_tag_associations` ###

DROP TABLE IF EXISTS `0_tag_associations`;

CREATE TABLE `0_tag_associations` (
  `record_id` varchar(11) NOT NULL,
  `tag_id` int(11) NOT NULL,
  UNIQUE KEY(`record_id`,`tag_id`)
) TYPE=MyISAM;


### Data of table `0_tag_associations` ###

DROP TABLE IF EXISTS `0_useronline` ;

CREATE TABLE `0_useronline` (
	`id` int(11) NOT NULL AUTO_INCREMENT ,
	`timestamp` int(15) NOT NULL default '0',
	`ip` varchar(40) NOT NULL default '',
	`file` varchar(100) NOT NULL default '',
	PRIMARY KEY `id` (`id`) ,
	KEY (`timestamp`) 
) TYPE=MYISAM AUTO_INCREMENT=1;

