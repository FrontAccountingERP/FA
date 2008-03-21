-- phpMyAdmin SQL Dump
-- version 2.7.0-pl2
-- http://www.phpmyadmin.net
-- 
-- Host: localhost
-- Created: 09 februar 2007 at 10:54
-- Server Version: 4.1.11
-- PHP-version: 4.4.1
-- 
-- Database: `en_US-demo`
-- 

-- --------------------------------------------------------

-- 
-- Structure for table `areas`
-- 

DROP TABLE IF EXISTS `0_areas`;
CREATE TABLE IF NOT EXISTS `0_areas` (
  `area_code` int(11) NOT NULL auto_increment,
  `description` varchar(60) NOT NULL default '',
  PRIMARY KEY  (`area_code`),
  UNIQUE KEY `description` (`description`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `bank_accounts`
-- 

DROP TABLE IF EXISTS `0_bank_accounts`;
CREATE TABLE IF NOT EXISTS `0_bank_accounts` (
  `account_code` varchar(11) NOT NULL default '',
  `account_type` smallint(6) NOT NULL default '0',
  `bank_account_name` varchar(60) NOT NULL default '',
  `bank_account_number` varchar(100) NOT NULL default '',
  `bank_name` varchar(60) NOT NULL default '',
  `bank_address` tinytext,
  `bank_curr_code` char(3) NOT NULL default '',
  PRIMARY KEY  (`account_code`),
  KEY `bank_account_name` (`bank_account_name`),
  KEY `bank_account_number` (`bank_account_number`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `bank_trans`
-- 

DROP TABLE IF EXISTS `0_bank_trans`;
CREATE TABLE IF NOT EXISTS `0_bank_trans` (
  `id` int(11) NOT NULL auto_increment,
  `type` smallint(6) default NULL,
  `trans_no` int(11) default NULL,
  `bank_act` varchar(11) default NULL,
  `ref` varchar(40) default NULL,
  `trans_date` date NOT NULL default '0000-00-00',
  `bank_trans_type_id` int(10) unsigned default NULL,
  `amount` double default NULL,
  `dimension_id` int(11) NOT NULL default '0',
  `dimension2_id` int(11) NOT NULL default '0',
  `person_type_id` int(11) NOT NULL default '0',
  `person_id` tinyblob,
  PRIMARY KEY  (`id`),
  KEY `bank_act` (`bank_act`,`ref`),
  KEY `type` (`type`,`trans_no`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `bank_trans_types`
-- 

DROP TABLE IF EXISTS `0_bank_trans_types`;
CREATE TABLE IF NOT EXISTS `0_bank_trans_types` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(60) NOT NULL default '',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `bom`
-- 

DROP TABLE IF EXISTS `0_bom`;
CREATE TABLE IF NOT EXISTS `0_bom` (
  `id` int(11) NOT NULL auto_increment,
  `parent` char(20) NOT NULL default '',
  `component` char(20) NOT NULL default '',
  `workcentre_added` char(5) NOT NULL default '',
  `loc_code` char(5) NOT NULL default '',
  `quantity` double NOT NULL default '1',
  PRIMARY KEY  (`parent`,`component`,`workcentre_added`,`loc_code`),
  KEY `component` (`component`),
  KEY `id` (`id`),
  KEY `loc_code` (`loc_code`),
  KEY `parent` (`parent`,`loc_code`),
  KEY `Parent_2` (`parent`),
  KEY `workcentre_added` (`workcentre_added`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `budget_trans`
-- 

DROP TABLE IF EXISTS `0_budget_trans`;
CREATE TABLE IF NOT EXISTS `0_budget_trans` (
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
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `chart_class`
-- 

DROP TABLE IF EXISTS `0_chart_class`;
CREATE TABLE IF NOT EXISTS `0_chart_class` (
  `cid` int(11) NOT NULL default '0',
  `class_name` varchar(60) NOT NULL default '',
  `balance_sheet` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`cid`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `chart_master`
-- 

DROP TABLE IF EXISTS `0_chart_master`;
CREATE TABLE IF NOT EXISTS `0_chart_master` (
  `account_code` varchar(11) NOT NULL default '',
  `account_code2` varchar(11) default '',
  `account_name` varchar(60) NOT NULL default '',
  `account_type` int(11) NOT NULL default '0',
  `tax_code` int(11) NOT NULL default '0',
  PRIMARY KEY  (`account_code`),
  KEY `account_code` (`account_code`),
  KEY `account_name` (`account_name`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `chart_types`
-- 

DROP TABLE IF EXISTS `0_chart_types`;
CREATE TABLE IF NOT EXISTS `0_chart_types` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(60) NOT NULL default '',
  `class_id` tinyint(1) NOT NULL default '0',
  `parent` int(11) NOT NULL default '-1',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `comments`
-- 

DROP TABLE IF EXISTS `0_comments`;
CREATE TABLE IF NOT EXISTS `0_comments` (
  `type` int(11) NOT NULL default '0',
  `id` int(11) NOT NULL default '0',
  `date_` date default '0000-00-00',
  `memo_` tinytext
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `company`
-- 

DROP TABLE IF EXISTS `0_company`;
CREATE TABLE IF NOT EXISTS `0_company` (
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
  `custom1_name` varchar(60) NOT NULL default '',
  `custom2_name` varchar(60) NOT NULL default '',
  `custom3_name` varchar(60) NOT NULL default '',
  `custom1_value` varchar(100) NOT NULL default '',
  `custom2_value` varchar(100) NOT NULL default '',
  `custom3_value` varchar(100) NOT NULL default '',
  `allow_negative_stock` tinyint(1) NOT NULL default '0',
  `po_over_receive` int(11) NOT NULL default '10',
  `po_over_charge` int(11) NOT NULL default '10',
  `default_credit_limit` int(11) NOT NULL default '1000',
  `default_workorder_required` int(11) NOT NULL default '20',
  `default_dim_required` int(11) NOT NULL default '20',
  `past_due_days` int(11) NOT NULL default '30',
  `use_dimension` tinyint(1) default '0',
  `f_year` int(11) NOT NULL default '1',
  `no_item_list` TINYINT(1) NOT NULL DEFAULT '0',
  `no_customer_list` TINYINT(1) NOT NULL DEFAULT '0',
  `no_supplier_list` TINYINT(1) NOT NULL DEFAULT '0',
  PRIMARY KEY  (`coy_code`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `credit_status`
-- 

DROP TABLE IF EXISTS `0_credit_status`;
CREATE TABLE IF NOT EXISTS `0_credit_status` (
  `id` int(11) NOT NULL auto_increment,
  `reason_description` char(100) NOT NULL default '',
  `dissallow_invoices` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `reason_description` (`reason_description`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `currencies`
-- 

DROP TABLE IF EXISTS `0_currencies`;
CREATE TABLE IF NOT EXISTS `0_currencies` (
  `currency` varchar(60) NOT NULL default '',
  `curr_abrev` char(3) NOT NULL default '',
  `curr_symbol` varchar(10) NOT NULL default '',
  `country` varchar(100) NOT NULL default '',
  `hundreds_name` varchar(15) NOT NULL default '',
  PRIMARY KEY  (`curr_abrev`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `cust_allocations`
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
  PRIMARY KEY  (`id`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `cust_branch`
-- 

DROP TABLE IF EXISTS `0_cust_branch`;
CREATE TABLE IF NOT EXISTS `0_cust_branch` (
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
  PRIMARY KEY  (`branch_code`,`debtor_no`),
  KEY `branch_code` (`branch_code`),
  KEY `br_name` (`br_name`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `debtor_trans`
-- 

DROP TABLE IF EXISTS `0_debtor_trans`;
CREATE TABLE IF NOT EXISTS `0_debtor_trans` (
  `trans_no` int(11) unsigned NOT NULL default '0',
  `type` smallint(6) unsigned NOT NULL default '0',
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
  `ov_discount` double NOT NULL default '0',
  `alloc` double NOT NULL default '0',
  `rate` double NOT NULL default '1',
  `ship_via` int(11) default NULL,
  PRIMARY KEY  (`trans_no`,`type`),
  KEY `debtor_no` (`debtor_no`,`branch_code`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `debtor_trans_details`
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
  PRIMARY KEY  (`id`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `debtor_trans_tax_details`
-- 

DROP TABLE IF EXISTS `0_debtor_trans_tax_details`;
CREATE TABLE IF NOT EXISTS `0_debtor_trans_tax_details` (
  `id` int(11) NOT NULL auto_increment,
  `debtor_trans_no` int(11) default NULL,
  `debtor_trans_type` int(11) default NULL,
  `tax_type_id` int(11) NOT NULL default '0',
  `tax_type_name` varchar(60) default NULL,
  `rate` double NOT NULL default '0',
  `included_in_price` tinyint(1) NOT NULL default '0',
  `amount` double NOT NULL default '0',
  PRIMARY KEY  (`id`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `debtors_master`
-- 

DROP TABLE IF EXISTS `0_debtors_master`;
CREATE TABLE IF NOT EXISTS `0_debtors_master` (
  `debtor_no` int(11) NOT NULL auto_increment,
  `name` varchar(60) NOT NULL default '',
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
  PRIMARY KEY  (`debtor_no`),
  UNIQUE KEY `name` (`name`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `dimensions`
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
  UNIQUE KEY `reference` (`reference`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `exchange_rates`
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
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `fiscal_year`
-- 

DROP TABLE IF EXISTS `0_fiscal_year`;
CREATE TABLE IF NOT EXISTS `0_fiscal_year` (
  `id` int(11) NOT NULL auto_increment,
  `begin` date default '0000-00-00',
  `end` date default '0000-00-00',
  `closed` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `form_items`
-- 

DROP TABLE IF EXISTS `0_form_items`;
CREATE TABLE IF NOT EXISTS `0_form_items` (
  `form_id` int(11) NOT NULL default '0',
  `form_type` int(11) NOT NULL default '0',
  `trans_type` int(11) NOT NULL default '0',
  `trans_id` int(11) NOT NULL default '0',
  `param1` varchar(100) NOT NULL default '',
  `param2` varchar(100) default NULL,
  PRIMARY KEY  (`form_id`,`form_type`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `gl_trans`
-- 

DROP TABLE IF EXISTS `0_gl_trans`;
CREATE TABLE IF NOT EXISTS `0_gl_trans` (
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
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `grn_batch`
-- 

DROP TABLE IF EXISTS `0_grn_batch`;
CREATE TABLE IF NOT EXISTS `0_grn_batch` (
  `id` int(11) NOT NULL auto_increment,
  `supplier_id` int(11) NOT NULL default '0',
  `purch_order_no` int(11) default NULL,
  `reference` varchar(60) NOT NULL default '',
  `delivery_date` date NOT NULL default '0000-00-00',
  `loc_code` varchar(5) default NULL,
  PRIMARY KEY  (`id`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `grn_items`
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
  PRIMARY KEY  (`id`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `item_tax_type_exemptions`
-- 

DROP TABLE IF EXISTS `0_item_tax_type_exemptions`;
CREATE TABLE IF NOT EXISTS `0_item_tax_type_exemptions` (
  `item_tax_type_id` int(11) NOT NULL default '0',
  `tax_type_id` int(11) NOT NULL default '0',
  PRIMARY KEY  (`item_tax_type_id`,`tax_type_id`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `item_tax_types`
-- 

DROP TABLE IF EXISTS `0_item_tax_types`;
CREATE TABLE IF NOT EXISTS `0_item_tax_types` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(60) NOT NULL default '',
  `exempt` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `loc_stock`
-- 

DROP TABLE IF EXISTS `0_loc_stock`;
CREATE TABLE IF NOT EXISTS `0_loc_stock` (
  `loc_code` char(5) NOT NULL default '',
  `stock_id` char(20) NOT NULL default '',
  `reorder_level` bigint(20) NOT NULL default '0',
  PRIMARY KEY  (`loc_code`,`stock_id`),
  KEY `stock_id` (`stock_id`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `locations`
-- 

DROP TABLE IF EXISTS `0_locations`;
CREATE TABLE IF NOT EXISTS `0_locations` (
  `loc_code` varchar(5) NOT NULL default '',
  `location_name` varchar(60) NOT NULL default '',
  `delivery_address` tinytext NOT NULL,
  `phone` varchar(30) NOT NULL default '',
  `fax` varchar(30) NOT NULL default '',
  `email` varchar(100) NOT NULL default '',
  `contact` varchar(30) NOT NULL default '',
  PRIMARY KEY  (`loc_code`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `movement_types`
-- 

DROP TABLE IF EXISTS `0_movement_types`;
CREATE TABLE IF NOT EXISTS `0_movement_types` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(60) NOT NULL default '',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `payment_terms`
-- 

DROP TABLE IF EXISTS `0_payment_terms`;
CREATE TABLE IF NOT EXISTS `0_payment_terms` (
  `terms_indicator` int(11) NOT NULL auto_increment,
  `terms` char(80) NOT NULL default '',
  `days_before_due` smallint(6) NOT NULL default '0',
  `day_in_following_month` smallint(6) NOT NULL default '0',
  PRIMARY KEY  (`terms_indicator`),
  UNIQUE KEY `terms` (`terms`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `prices`
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
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `purch_data`
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
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `purch_order_details`
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
  PRIMARY KEY  (`po_detail_item`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `purch_orders`
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
  PRIMARY KEY  (`order_no`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `refs`
-- 

DROP TABLE IF EXISTS `0_refs`;
CREATE TABLE IF NOT EXISTS `0_refs` (
  `id` int(11) NOT NULL default '0',
  `type` int(11) NOT NULL default '0',
  `reference` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`id`,`type`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `sales_order_details`
-- 

DROP TABLE IF EXISTS `0_sales_order_details`;
CREATE TABLE IF NOT EXISTS `0_sales_order_details` (
  `order_no` int(11) NOT NULL default '0',
  `stk_code` varchar(20) NOT NULL default '',
  `description` tinytext,
  `qty_invoiced` double NOT NULL default '0',
  `unit_price` double NOT NULL default '0',
  `quantity` double NOT NULL default '0',
  `discount_percent` double NOT NULL default '0',
  PRIMARY KEY  (`order_no`,`stk_code`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `sales_orders`
-- 

DROP TABLE IF EXISTS `0_sales_orders`;
CREATE TABLE IF NOT EXISTS `0_sales_orders` (
  `order_no` int(11) NOT NULL auto_increment,
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
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `sales_types`
-- 

DROP TABLE IF EXISTS `0_sales_types`;
CREATE TABLE IF NOT EXISTS `0_sales_types` (
  `id` int(11) NOT NULL auto_increment,
  `sales_type` char(50) NOT NULL default '',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `sales_type` (`sales_type`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `salesman`
-- 

DROP TABLE IF EXISTS `0_salesman`;
CREATE TABLE IF NOT EXISTS `0_salesman` (
  `salesman_code` int(11) NOT NULL auto_increment,
  `salesman_name` char(60) NOT NULL default '',
  `salesman_phone` char(30) NOT NULL default '',
  `salesman_fax` char(30) NOT NULL default '',
  `salesman_email` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`salesman_code`),
  UNIQUE KEY `salesman_name` (`salesman_name`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `shippers`
-- 

DROP TABLE IF EXISTS `0_shippers`;
CREATE TABLE IF NOT EXISTS `0_shippers` (
  `shipper_id` int(11) NOT NULL auto_increment,
  `shipper_name` varchar(60) NOT NULL default '',
  `phone` varchar(30) NOT NULL default '',
  `contact` tinytext NOT NULL,
  `address` tinytext NOT NULL,
  PRIMARY KEY  (`shipper_id`),
  UNIQUE KEY `name` (`shipper_name`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `stock_category`
-- 

DROP TABLE IF EXISTS `0_stock_category`;
CREATE TABLE IF NOT EXISTS `0_stock_category` (
  `category_id` int(11) NOT NULL auto_increment,
  `description` varchar(60) NOT NULL default '',
  `stock_act` varchar(11) default NULL,
  `cogs_act` varchar(11) default NULL,
  `adj_gl_act` varchar(11) default NULL,
  `purch_price_var_act` varchar(11) default NULL,
  PRIMARY KEY  (`category_id`),
  UNIQUE KEY `description` (`description`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `stock_master`
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
  PRIMARY KEY  (`stock_id`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `stock_moves`
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
  KEY `type` (`type`,`trans_no`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `supp_allocations`
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
  PRIMARY KEY  (`id`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `supp_invoice_items`
-- 

DROP TABLE IF EXISTS `0_supp_invoice_items`;
CREATE TABLE IF NOT EXISTS `0_supp_invoice_items` (
  `id` int(11) NOT NULL auto_increment,
  `supp_trans_no` int(11) default NULL,
  `supp_trans_type` int(11) default NULL,
  `gl_code` int(11) NOT NULL default '0',
  `grn_item_id` int(11) default NULL,
  `po_detail_item_id` int(11) default NULL,
  `stock_id` varchar(20) NOT NULL default '',
  `description` tinytext,
  `quantity` double NOT NULL default '0',
  `unit_price` double NOT NULL default '0',
  `unit_tax` double NOT NULL default '0',
  `memo_` tinytext,
  PRIMARY KEY  (`id`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `supp_invoice_tax_items`
-- 

DROP TABLE IF EXISTS `0_supp_invoice_tax_items`;
CREATE TABLE IF NOT EXISTS `0_supp_invoice_tax_items` (
  `id` int(11) NOT NULL auto_increment,
  `supp_trans_no` int(11) default NULL,
  `supp_trans_type` int(11) default NULL,
  `tax_type_id` int(11) NOT NULL default '0',
  `tax_type_name` varchar(60) default NULL,
  `rate` double NOT NULL default '0',
  `included_in_price` tinyint(1) NOT NULL default '0',
  `amount` double NOT NULL default '0',
  PRIMARY KEY  (`id`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `supp_trans`
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
  PRIMARY KEY  (`trans_no`,`type`),
  KEY `supplier_id` (`supplier_id`),
  KEY `SupplierID_2` (`supplier_id`,`supp_reference`),
  KEY `type` (`type`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `suppliers`
-- 

DROP TABLE IF EXISTS `0_suppliers`;
CREATE TABLE IF NOT EXISTS `0_suppliers` (
  `supplier_id` int(11) NOT NULL auto_increment,
  `supp_name` varchar(60) NOT NULL default '',
  `address` tinytext NOT NULL,
  `email` varchar(100) NOT NULL default '',
  `bank_account` varchar(60) NOT NULL default '',
  `curr_code` char(3) default NULL,
  `payment_terms` int(11) default NULL,
  `dimension_id` int(11) default '0',
  `dimension2_id` int(11) default '0',
  `tax_group_id` int(11) default NULL,
  `purchase_account` varchar(11) default NULL,
  `payable_account` varchar(11) default NULL,
  `payment_discount_account` varchar(11) default NULL,
  PRIMARY KEY  (`supplier_id`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `sys_types`
-- 

DROP TABLE IF EXISTS `0_sys_types`;
CREATE TABLE IF NOT EXISTS `0_sys_types` (
  `type_id` smallint(6) NOT NULL default '0',
  `type_name` varchar(60) NOT NULL default '',
  `type_no` int(11) NOT NULL default '1',
  `next_reference` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`type_id`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `tax_group_items`
-- 

DROP TABLE IF EXISTS `0_tax_group_items`;
CREATE TABLE IF NOT EXISTS `0_tax_group_items` (
  `tax_group_id` int(11) NOT NULL default '0',
  `tax_type_id` int(11) NOT NULL default '0',
  `rate` double NOT NULL default '0',
  `included_in_price` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`tax_group_id`,`tax_type_id`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `tax_groups`
-- 

DROP TABLE IF EXISTS `0_tax_groups`;
CREATE TABLE IF NOT EXISTS `0_tax_groups` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(60) NOT NULL default '',
  `tax_shipping` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `tax_types`
-- 

DROP TABLE IF EXISTS `0_tax_types`;
CREATE TABLE IF NOT EXISTS `0_tax_types` (
  `id` int(11) NOT NULL auto_increment,
  `rate` double NOT NULL default '0',
  `sales_gl_code` varchar(11) NOT NULL default '',
  `purchasing_gl_code` varchar(11) NOT NULL default '',
  `name` varchar(60) NOT NULL default '',
  `out` tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `users`
-- 

DROP TABLE IF EXISTS `0_users`;
CREATE TABLE IF NOT EXISTS `0_users` (
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
  `last_visit_date` datetime default NULL,
  PRIMARY KEY  (`user_id`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `voided`
-- 

DROP TABLE IF EXISTS `0_voided`;
CREATE TABLE IF NOT EXISTS `0_voided` (
  `type` int(11) NOT NULL default '0',
  `id` int(11) NOT NULL default '0',
  `date_` date NOT NULL default '0000-00-00',
  `memo_` tinytext NOT NULL,
  UNIQUE KEY `id` (`type`,`id`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `wo_issue_items`
-- 

DROP TABLE IF EXISTS `0_wo_issue_items`;
CREATE TABLE IF NOT EXISTS `0_wo_issue_items` (
  `id` int(11) NOT NULL auto_increment,
  `stock_id` varchar(40) default NULL,
  `issue_id` int(11) default NULL,
  `qty_issued` double default NULL,
  PRIMARY KEY  (`id`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `wo_issues`
-- 

DROP TABLE IF EXISTS `0_wo_issues`;
CREATE TABLE IF NOT EXISTS `0_wo_issues` (
  `issue_no` int(11) NOT NULL auto_increment,
  `workorder_id` int(11) NOT NULL default '0',
  `reference` varchar(100) default NULL,
  `issue_date` date default NULL,
  `loc_code` varchar(5) default NULL,
  `workcentre_id` int(11) default NULL,
  PRIMARY KEY  (`issue_no`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `wo_manufacture`
-- 

DROP TABLE IF EXISTS `0_wo_manufacture`;
CREATE TABLE IF NOT EXISTS `0_wo_manufacture` (
  `id` int(11) NOT NULL auto_increment,
  `reference` varchar(100) default NULL,
  `workorder_id` int(11) NOT NULL default '0',
  `quantity` double NOT NULL default '0',
  `date_` date NOT NULL default '0000-00-00',
  PRIMARY KEY  (`id`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `wo_requirements`
-- 

DROP TABLE IF EXISTS `0_wo_requirements`;
CREATE TABLE IF NOT EXISTS `0_wo_requirements` (
  `id` int(11) NOT NULL auto_increment,
  `workorder_id` int(11) NOT NULL default '0',
  `stock_id` char(20) NOT NULL default '',
  `workcentre` char(5) NOT NULL default '',
  `units_req` double NOT NULL default '1',
  `std_cost` double NOT NULL default '0',
  `loc_code` char(5) NOT NULL default '',
  `units_issued` double NOT NULL default '0',
  PRIMARY KEY  (`id`)
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Structure for table `workcentres`
-- 

DROP TABLE IF EXISTS `0_workcentres`;
CREATE TABLE IF NOT EXISTS `0_workcentres` (
  `id` int(11) NOT NULL auto_increment,
  `name` char(40) NOT NULL default '',
  `description` char(50) NOT NULL default '',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Structure for table `workorders`
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
) TYPE=InnoDB;

-- --------------------------------------------------------

-- 
-- Data in table `areas`
-- 

INSERT INTO `0_areas` VALUES (1, 'USA');
INSERT INTO `0_areas` VALUES (2, 'Far East');
INSERT INTO `0_areas` VALUES (3, 'Africa');
INSERT INTO `0_areas` VALUES (4, 'Europe');

-- 
-- Data in table `bank_accounts`
-- 

INSERT INTO `0_bank_accounts` VALUES ('1700', 0, 'Current account', 'N/A', 'N/A', '', 'USD');
INSERT INTO `0_bank_accounts` VALUES ('1705', 0, 'Petty Cash account', 'N/A', 'N/A', '', 'USD');
INSERT INTO `0_bank_accounts` VALUES ('1710', 0, 'Saving account', '10001000', 'Saving Bank', '', 'GBP');

-- 
-- Data in table `bank_trans`
-- 

INSERT INTO `0_bank_trans` VALUES (1, 12, 2, '1700', '111', '2006-01-18', 1, 5000, 0, 0, 2, 0x31);
INSERT INTO `0_bank_trans` VALUES (2, 12, 3, '1700', '112', '2006-01-18', 1, 240, 0, 0, 2, 0x32);
INSERT INTO `0_bank_trans` VALUES (3, 12, 4, '1700', '113', '2006-01-18', 1, 360, 0, 0, 2, 0x32);
INSERT INTO `0_bank_trans` VALUES (4, 12, 5, '1700', '114', '2006-01-18', 1, 500, 0, 0, 2, 0x31);
INSERT INTO `0_bank_trans` VALUES (5, 1, 2, '1700', '1', '2006-01-18', 1, -25, 0, 0, 0, '');
INSERT INTO `0_bank_trans` VALUES (6, 1, 3, '1705', '2', '2006-01-18', 1, -250, 0, 0, 0, '');
INSERT INTO `0_bank_trans` VALUES (7, 1, 4, '1700', '3', '2006-01-18', 1, -555, 0, 0, 4, 0x31);
INSERT INTO `0_bank_trans` VALUES (8, 4, 2, '1700', '4', '2006-01-18', 1, -300, 0, 0, 0, '');
INSERT INTO `0_bank_trans` VALUES (9, 4, 2, '1710', '4', '2006-01-18', 1, 250, 0, 0, 0, '');
INSERT INTO `0_bank_trans` VALUES (10, 22, 2, '1700', '1', '2006-01-18', 1, -5000, 0, 0, 3, 0x31);
INSERT INTO `0_bank_trans` VALUES (11, 22, 3, '1710', '2', '2006-01-18', 1, -3300, 0, 0, 3, 0x32);
INSERT INTO `0_bank_trans` VALUES (12, 2, 2, '1700', '11', '2006-01-20', 1, 1050, 0, 0, 0, '');
INSERT INTO `0_bank_trans` VALUES (13, 12, 6, '1700', '115', '2007-01-30', 1, 200, 0, 0, 2, 0x31);
INSERT INTO `0_bank_trans` VALUES (14, 1, 5, '1700', '4', '2007-01-30', 1, -200, 0, 0, 4, 0x31);
INSERT INTO `0_bank_trans` VALUES (15, 2, 3, '1700', '12', '2007-01-30', 3, 70, 0, 0, 4, 0x32);
INSERT INTO `0_bank_trans` VALUES (16, 4, 3, '1700', '5', '2007-03-09', 1, -222, 0, 0, 0, '');
INSERT INTO `0_bank_trans` VALUES (17, 4, 3, '1705', '5', '2007-03-09', 1, 222, 0, 0, 0, '');
INSERT INTO `0_bank_trans` VALUES (18, 2, 4, '1700', '13', '2007-03-09', 3, 200, 0, 0, 2, 0x31);
INSERT INTO `0_bank_trans` VALUES (19, 1, 6, '1700', '5', '2007-03-22', 1, -200, 0, 0, 3, 0x31);
INSERT INTO `0_bank_trans` VALUES (20, 1, 7, '1700', '6', '2007-03-22', 1, -125, 0, 0, 0, 0x67796c6c657472616e73706f7274);

-- 
-- Data in table `bank_trans_types`
-- 

INSERT INTO `0_bank_trans_types` VALUES (1, 'Cash');
INSERT INTO `0_bank_trans_types` VALUES (2, 'Cheque');
INSERT INTO `0_bank_trans_types` VALUES (3, 'Transfer');

-- 
-- Data in table `bom`
-- 

INSERT INTO `0_bom` VALUES (1, '3400', '102', '1', 'DEF', 1);
INSERT INTO `0_bom` VALUES (2, '3400', '103', '1', 'DEF', 1);
INSERT INTO `0_bom` VALUES (3, '3400', '104', '1', 'DEF', 1);
INSERT INTO `0_bom` VALUES (4, '3400', '201', '1', 'DEF', 1);

-- 
-- Data in table `chart_class`
-- 

INSERT INTO `0_chart_class` VALUES (1, 'Assets', 1);
INSERT INTO `0_chart_class` VALUES (2, 'Liabilities', 1);
INSERT INTO `0_chart_class` VALUES (3, 'Income', 0);
INSERT INTO `0_chart_class` VALUES (4, 'Costs', 0);
INSERT INTO `0_chart_class` VALUES (5, 'Gross', 0);

-- 
-- Data in table `chart_master`
-- 

INSERT INTO `0_chart_master` VALUES ('3000', '', 'Sales', 1, 1);
INSERT INTO `0_chart_master` VALUES ('3010', '', 'Sales  - Wholesale', 1, 1);
INSERT INTO `0_chart_master` VALUES ('3020', '', 'Sales of Other items', 1, 1);
INSERT INTO `0_chart_master` VALUES ('3400', '', 'Difference On Exchange', 1, 0);
INSERT INTO `0_chart_master` VALUES ('5000', '', 'Direct Labour', 2, 0);
INSERT INTO `0_chart_master` VALUES ('5050', '', 'Direct Labour Recovery', 2, 0);
INSERT INTO `0_chart_master` VALUES ('4200', '', 'Material Usage Varaiance', 2, 4);
INSERT INTO `0_chart_master` VALUES ('4210', '', 'Consumable Materials', 2, 4);
INSERT INTO `0_chart_master` VALUES ('4220', '', 'Purchase price Variance', 2, 0);
INSERT INTO `0_chart_master` VALUES ('4000', '', 'Purchases of materials', 2, 4);
INSERT INTO `0_chart_master` VALUES ('4250', '', 'Discounts Received', 2, 0);
INSERT INTO `0_chart_master` VALUES ('4260', '', 'Exchange Variation', 2, 0);
INSERT INTO `0_chart_master` VALUES ('4300', '', 'Freight Inwards', 2, 4);
INSERT INTO `0_chart_master` VALUES ('4010', '', 'Cost of Goods Sold - Retail', 2, 4);
INSERT INTO `0_chart_master` VALUES ('6790', '', 'Bank Charges', 5, 4);
INSERT INTO `0_chart_master` VALUES ('6800', '', 'Entertainments', 5, 4);
INSERT INTO `0_chart_master` VALUES ('6810', '', 'Legal Expenses', 5, 4);
INSERT INTO `0_chart_master` VALUES ('6600', '', 'Repairs and Maintenance Office', 5, 4);
INSERT INTO `0_chart_master` VALUES ('6730', '', 'phone', 5, 4);
INSERT INTO `0_chart_master` VALUES ('8200', '', 'Bank Interest', 52, 0);
INSERT INTO `0_chart_master` VALUES ('6840', '', 'Credit Control', 5, 0);
INSERT INTO `0_chart_master` VALUES ('7040', '', 'Depreciation Office Equipment', 51, 0);
INSERT INTO `0_chart_master` VALUES ('3800', '', 'Freight Outwards', 5, 4);
INSERT INTO `0_chart_master` VALUES ('4500', '', 'Packaging', 5, 4);
INSERT INTO `0_chart_master` VALUES ('6400', '', 'Commissions', 5, 0);
INSERT INTO `0_chart_master` VALUES ('3200', '', 'Prompt Payment Discounts', 1, 0);
INSERT INTO `0_chart_master` VALUES ('6700', '', 'General Expenses', 5, 4);
INSERT INTO `0_chart_master` VALUES ('5200', '', 'Indirect Labour', 2, 0);
INSERT INTO `0_chart_master` VALUES ('5210', '', 'Overhead Recovery', 5, 0);
INSERT INTO `0_chart_master` VALUES ('1700', '', 'Bank account', 10, 0);
INSERT INTO `0_chart_master` VALUES ('1705', '', 'Petty Cash', 10, 0);
INSERT INTO `0_chart_master` VALUES ('1710', '', 'Foreign currency account', 10, 0);
INSERT INTO `0_chart_master` VALUES ('1500', '', 'Accounts Receivable', 20, 0);
INSERT INTO `0_chart_master` VALUES ('1400', '', 'Stocks of Raw Materials', 45, 0);
INSERT INTO `0_chart_master` VALUES ('1410', '', 'Stocks of Work In Progress', 45, 0);
INSERT INTO `0_chart_master` VALUES ('1420', '', 'Stocks of Finsihed Goods', 45, 0);
INSERT INTO `0_chart_master` VALUES ('1430', '', 'Goods Received Clearing account', 30, 0);
INSERT INTO `0_chart_master` VALUES ('2630', '', 'Accounts Payable', 30, 0);
INSERT INTO `0_chart_master` VALUES ('2660', '', 'VAT out 5', 30, 0);
INSERT INTO `0_chart_master` VALUES ('2662', '', 'VAT out 1', 30, 0);
INSERT INTO `0_chart_master` VALUES ('2664', '', 'VAT out 25', 30, 0);
INSERT INTO `0_chart_master` VALUES ('2680', '', 'VAT In 5', 30, 0);
INSERT INTO `0_chart_master` VALUES ('2682', '', 'VAT In 25', 30, 0);
INSERT INTO `0_chart_master` VALUES ('2050', '', 'Retained Earnings', 50, 0);
INSERT INTO `0_chart_master` VALUES ('2000', '', 'Share Capital', 50, 0);

-- 
-- Data in table `chart_types`
-- 

INSERT INTO `0_chart_types` VALUES (1, 'Sales', 3, -1);
INSERT INTO `0_chart_types` VALUES (2, 'Cost of Sales', 4, -1);
INSERT INTO `0_chart_types` VALUES (5, 'Expenses', 4, -1);
INSERT INTO `0_chart_types` VALUES (10, 'Cash/Bank', 1, -1);
INSERT INTO `0_chart_types` VALUES (20, 'Accounts Receivable', 1, -1);
INSERT INTO `0_chart_types` VALUES (30, 'Accounts Payable', 2, -1);
INSERT INTO `0_chart_types` VALUES (40, 'Fixed Assets', 1, -1);
INSERT INTO `0_chart_types` VALUES (45, 'Inventory', 1, -1);
INSERT INTO `0_chart_types` VALUES (50, 'Equity', 2, -1);
INSERT INTO `0_chart_types` VALUES (51, 'Depreciations', 4, -1);
INSERT INTO `0_chart_types` VALUES (52, 'Financials', 4, -1);

-- 
-- Data in table `comments`
-- 

INSERT INTO `0_comments` VALUES (17, 2, '2006-01-18', 'initial balances');
INSERT INTO `0_comments` VALUES (10, 6, '2007-01-30', 'Hi there you got it!');
INSERT INTO `0_comments` VALUES (12, 6, '2007-01-30', 'This is good');
INSERT INTO `0_comments` VALUES (1, 5, '2007-01-30', 'Totalgylle');
INSERT INTO `0_comments` VALUES (40, 2, '0000-00-00', 'Gylle projevt');
INSERT INTO `0_comments` VALUES (0, 6, '2007-02-02', 'A big memo');
INSERT INTO `0_comments` VALUES (10, 7, '2007-02-03', 'Another big memo, which looks good.');
INSERT INTO `0_comments` VALUES (4, 3, '2007-03-09', 'A little cash up front.');

-- 
-- Data in table `company`
-- 

INSERT INTO `0_company` VALUES (1, 'Drill Company Inc.', '987654321', '123123123', 1, 1, 'N/A', '202-122320', '202-18889123', 'delta@delta.com', 'logo_frontaccounting.jpg', 'DownTown', 'USD', '1500', '4250', '2630', '1430', '4260', '4220', '2050', '3800', '3000', '3000', '3200', '1420', '4010', '4210', '3000', '1410', '5000', '', '', '', '', '', '', 0, 10, 10, 1000, 20, 20, 30, 1, 2, 0, 0, 0);

-- 
-- Data in table `credit_status`
-- 

INSERT INTO `0_credit_status` VALUES (1, 'Good History', 0);
INSERT INTO `0_credit_status` VALUES (3, 'No more work until payment received', 1);
INSERT INTO `0_credit_status` VALUES (4, 'In liquidation', 1);

-- 
-- Data in table `currencies`
-- 

INSERT INTO `0_currencies` VALUES ('Kronor', 'SEK', 'kr', 'Sweden', '?ren');
INSERT INTO `0_currencies` VALUES ('Kroner', 'DKK', 'kr.', 'Denmark', '?re');
INSERT INTO `0_currencies` VALUES ('Euro', 'EUR', '?', 'Europe', 'Cents');
INSERT INTO `0_currencies` VALUES ('Pounds', 'GBP', '?', 'England', 'Pence');
INSERT INTO `0_currencies` VALUES ('US Dollars', 'USD', '$', 'United States', 'Cents');

-- 
-- Data in table `cust_allocations`
-- 

INSERT INTO `0_cust_allocations` VALUES (1, 200, '2007-01-30', 6, 12, 6, 10);
INSERT INTO `0_cust_allocations` VALUES (4, 133, '2007-03-09', 4, 2, 6, 10);

-- 
-- Data in table `cust_branch`
-- 

INSERT INTO `0_cust_branch` VALUES (1, 1, 'Main', '', 1, 1, '', '', 'Lucky Luke Inc.', 'joe@frontaccounting.com', 'DEF', 2, '3000', '3000', '1500', '3200', 1, 0, 'The Road');
INSERT INTO `0_cust_branch` VALUES (2, 1, 'Branch 2', '', 1, 1, '', '', '', '', 'DEF', 3, '3000', '3000', '1500', '3200', 1, 0, 'Another Road');
INSERT INTO `0_cust_branch` VALUES (3, 2, 'Main', '', 1, 1, '', '', 'Money Makers Ltd.', '', 'DEF', 3, '3000', '3000', '1500', '3200', 1, 0, '');
INSERT INTO `0_cust_branch` VALUES (4, 2, 'Main', '', 1, 1, '', '', 'Money Makers Ltd.', '', 'DEF', 3, '3000', '3000', '1500', '3200', 1, 0, 'UK,UK');
INSERT INTO `0_cust_branch` VALUES (5, 3, 'Main', '', 1, 1, '', '', 'Junk Beer ApS', '', 'DEF', 3, '3000', '3000', '1500', '3200', 1, 0, '');

-- 
-- Data in table `debtor_trans`
-- 

INSERT INTO `0_debtor_trans` VALUES (2, 10, 1, 1, '2006-01-18', '2006-01-18', '1', 1, 1, 1750, 0, 0, 0, 0, 1, 1);
INSERT INTO `0_debtor_trans` VALUES (2, 11, 2, 3, '2006-01-18', '0000-00-00', '33', 1, 0, -1050, 0, 0, 0, 0, 1.2, 1);
INSERT INTO `0_debtor_trans` VALUES (2, 12, 1, 1, '2006-01-18', '0000-00-00', '111', 0, 0, -10000, 0, 0, 0, 0, 1, 0);
INSERT INTO `0_debtor_trans` VALUES (3, 10, 1, 1, '2006-01-18', '2006-01-18', '2', 1, 1, 1000, 0, 0, 0, 0, 1, 1);
INSERT INTO `0_debtor_trans` VALUES (3, 12, 2, 3, '2006-01-18', '0000-00-00', '112', 0, 0, -200, 0, 0, 0, 0, 1.2, 0);
INSERT INTO `0_debtor_trans` VALUES (4, 2, 1, 1, '2007-03-09', '0000-00-00', '13', 0, 0, -200, 0, 0, 0, 133, 1, 0);
INSERT INTO `0_debtor_trans` VALUES (4, 10, 2, 3, '2006-01-18', '2006-01-18', '3', 1, 2, 1057.14285714, 52.8571428571, 0, 0, 0, 1.2, 1);
INSERT INTO `0_debtor_trans` VALUES (4, 12, 2, 3, '2006-01-18', '0000-00-00', '113', 0, 0, -300, 0, 0, 0, 0, 1.2, 0);
INSERT INTO `0_debtor_trans` VALUES (5, 10, 1, 1, '2007-01-28', '2007-01-28', '4', 1, 1, 93050, 0, 0, 0, 0, 1, 1);
INSERT INTO `0_debtor_trans` VALUES (5, 12, 1, 1, '2006-01-18', '0000-00-00', '114', 0, 0, -500, 0, 0, 0, 0, 1, 0);
INSERT INTO `0_debtor_trans` VALUES (6, 10, 1, 1, '2007-01-30', '2007-01-30', '5', 1, 3, 333, 0, 0, 0, 333, 1, 1);
INSERT INTO `0_debtor_trans` VALUES (6, 12, 1, 1, '2007-01-30', '0000-00-00', '115', 0, 0, -200, 0, 0, 0, 200, 1, 0);
INSERT INTO `0_debtor_trans` VALUES (7, 10, 1, 1, '2007-02-03', '2007-02-03', '6', 1, 4, 333, 0, 0, 0, 0, 1, 1);
INSERT INTO `0_debtor_trans` VALUES (8, 10, 1, 1, '2007-02-03', '2007-02-03', '7', 1, 5, 333, 0, 0, 0, 0, 1, 1);
INSERT INTO `0_debtor_trans` VALUES (9, 10, 2, 3, '2007-02-03', '2007-02-03', '8', 1, 6, 2857.1428571429, 142.85714285714, 0, 0, 0, 1.2, 1);
INSERT INTO `0_debtor_trans` VALUES (10, 10, 1, 1, '2007-02-04', '2007-02-04', '9', 1, 9, 333, 0, 0, 0, 0, 1, 1);
INSERT INTO `0_debtor_trans` VALUES (11, 10, 1, 1, '2007-02-04', '2007-03-22', '10', 1, 10, 333, 0, 0, 0, 0, 1, 1);
INSERT INTO `0_debtor_trans` VALUES (12, 10, 1, 1, '2007-02-06', '2007-03-22', '11', 1, 11, 333, 0, 0, 0, 0, 1, 1);
INSERT INTO `0_debtor_trans` VALUES (13, 10, 1, 1, '2007-02-25', '2007-03-22', '12', 1, 12, 333, 0, 0, 0, 0, 1, 1);
INSERT INTO `0_debtor_trans` VALUES (14, 10, 1, 1, '2007-03-04', '2007-03-22', '13', 1, 13, 333, 0, 0, 0, 0, 1, 1);
INSERT INTO `0_debtor_trans` VALUES (15, 10, 1, 1, '2007-03-05', '2007-03-22', '14', 1, 14, 333, 0, 25, 0, 0, 1, 1);
INSERT INTO `0_debtor_trans` VALUES (16, 10, 1, 1, '2007-03-05', '2007-03-22', '15', 1, 15, 3000, 0, 44, 0, 0, 1, 1);

-- 
-- Data in table `debtor_trans_details`
-- 

INSERT INTO `0_debtor_trans_details` VALUES (1, 2, 10, '102', '17 inch VGA Monitor', 250, 0, -2, 0, 0);
INSERT INTO `0_debtor_trans_details` VALUES (2, 2, 10, '103', '32MB VGA Card', 3000, 0, -10, 0, 0);
INSERT INTO `0_debtor_trans_details` VALUES (3, 2, 10, '104', '52x CD Drive', 50, 0, -5, 0, 0);
INSERT INTO `0_debtor_trans_details` VALUES (4, 3, 10, '103', '32MB VGA Card', 3000, 0, -10, 0, 0);
INSERT INTO `0_debtor_trans_details` VALUES (5, 2, 11, '102', '17 inch VGA Monitor', 210, 0, 5, 0, 0);
INSERT INTO `0_debtor_trans_details` VALUES (6, 4, 10, '102', '17 inch VGA Monitor', 211.428571429, 10.5714285714, -5, 0, 0);
INSERT INTO `0_debtor_trans_details` VALUES (7, 5, 10, '102', '17 inch VGA Monitor', 250, 0, -8, 0, 0);
INSERT INTO `0_debtor_trans_details` VALUES (8, 5, 10, '103', '32MB VGA Card', 3000, 0, -30, 0, 0);
INSERT INTO `0_debtor_trans_details` VALUES (9, 5, 10, '104', '52x CD Drive', 50, 0, -21, 0, 0);
INSERT INTO `0_debtor_trans_details` VALUES (10, 6, 10, '102', '17 inch VGA Monitor', 333, 0, -1, 0, 0);
INSERT INTO `0_debtor_trans_details` VALUES (11, 7, 10, '102', '17 inch VGA Monitor', 333, 0, -1, 0, 0);
INSERT INTO `0_debtor_trans_details` VALUES (12, 8, 10, '102', '17 inch VGA Monitor', 333, 0, -1, 0, 0);
INSERT INTO `0_debtor_trans_details` VALUES (13, 9, 10, '3400', 'P4 Business System', 2857.1428571429, 142.85714285714, -1, 0, 0);
INSERT INTO `0_debtor_trans_details` VALUES (14, 10, 10, '102', '17 inch VGA Monitor', 333, 0, -1, 0, 0);
INSERT INTO `0_debtor_trans_details` VALUES (15, 11, 10, '102', '17 inch VGA Monitor', 333, 0, -1, 0, 0);
INSERT INTO `0_debtor_trans_details` VALUES (16, 12, 10, '102', '17 inch VGA Monitor', 333, 0, -1, 0, 0);
INSERT INTO `0_debtor_trans_details` VALUES (17, 13, 10, '102', '17 inch VGA Monitor', 333, 0, -1, 0, 0);
INSERT INTO `0_debtor_trans_details` VALUES (18, 14, 10, '102', '17 inch VGA Monitor', 333, 0, -1, 0, 0);
INSERT INTO `0_debtor_trans_details` VALUES (19, 15, 10, '102', '17 inch VGA Monitor', 333, 0, -1, 0, 0);
INSERT INTO `0_debtor_trans_details` VALUES (20, 16, 10, '103', '32MB VGA Card', 3000, 0, -1, 0, 0);

-- 
-- Data in table `debtor_trans_tax_details`
-- 

INSERT INTO `0_debtor_trans_tax_details` VALUES (1, 4, 10, 1, NULL, 5, 1, 52.8571428571);
INSERT INTO `0_debtor_trans_tax_details` VALUES (2, 9, 10, 1, NULL, 5, 1, 142.85714285714);

-- 
-- Data in table `debtors_master`
-- 

INSERT INTO `0_debtors_master` VALUES (1, 'Lucky Luke Inc.', '35 Waldorf Street\r\nTown 19358, AR', 'joe@frontaccounting.com', '12311231', 'USD', 1, 0, 0, 1, 1, 0, 0, 1000);
INSERT INTO `0_debtors_master` VALUES (2, 'Money Makers Ltd.', 'N/A', '', '9876543', 'GBP', 1, 0, 0, 1, 1, 0, 0, 1000);
INSERT INTO `0_debtors_master` VALUES (3, 'Junk Beer ApS', 'N/A', '', '123321123', 'DKK', 1, 0, 0, 1, 1, 0, 0, 1000);

-- 
-- Data in table `dimensions`
-- 

INSERT INTO `0_dimensions` VALUES (1, '1', 'Development', 1, 0, '2006-01-18', '2006-02-07');
INSERT INTO `0_dimensions` VALUES (2, '2', 'Support', 1, 0, '2006-01-18', '2007-03-07');
INSERT INTO `0_dimensions` VALUES (3, '3', 'Training', 2, 0, '2006-01-18', '2007-03-07');

-- 
-- Data in table `exchange_rates`
-- 

INSERT INTO `0_exchange_rates` VALUES (1, 'LE', 0.149, 0.149, '2006-01-18');
INSERT INTO `0_exchange_rates` VALUES (2, 'GBP', 1.2, 1.2, '2006-01-18');
INSERT INTO `0_exchange_rates` VALUES (3, 'SEK', 0.1667, 0.1667, '2007-01-29');
INSERT INTO `0_exchange_rates` VALUES (4, 'DKK', 0.2, 0.2, '2007-03-05');
INSERT INTO `0_exchange_rates` VALUES (5, 'EUR', 1.1, 1.1, '2007-03-05');

-- 
-- Data in table `fiscal_year`
-- 

INSERT INTO `0_fiscal_year` VALUES (1, '2006-01-01', '2006-12-31', 0);
INSERT INTO `0_fiscal_year` VALUES (2, '2007-01-01', '2007-12-31', 0);
INSERT INTO `0_fiscal_year` VALUES (5, '2005-01-01', '2005-12-31', 1);

-- 
-- Data in table `form_items`
-- 

INSERT INTO `0_form_items` VALUES (1, 10, 10, 2, '', NULL);
INSERT INTO `0_form_items` VALUES (1, 11, 11, 2, '', NULL);
INSERT INTO `0_form_items` VALUES (1, 18, 18, 1, '', NULL);
INSERT INTO `0_form_items` VALUES (1, 26, 26, 1, '', NULL);
INSERT INTO `0_form_items` VALUES (1, 30, 30, 1, '', NULL);
INSERT INTO `0_form_items` VALUES (1, 50, 1, 2, '', NULL);
INSERT INTO `0_form_items` VALUES (1, 51, 12, 2, '', NULL);
INSERT INTO `0_form_items` VALUES (1, 60, 16, 2, 'DEF', NULL);
INSERT INTO `0_form_items` VALUES (1, 61, 17, 2, 'DEF', NULL);
INSERT INTO `0_form_items` VALUES (1, 62, 11, 2, 'DEF', NULL);
INSERT INTO `0_form_items` VALUES (2, 10, 10, 3, '', NULL);
INSERT INTO `0_form_items` VALUES (2, 18, 18, 2, '', NULL);
INSERT INTO `0_form_items` VALUES (2, 26, 26, 2, '', NULL);
INSERT INTO `0_form_items` VALUES (2, 30, 30, 2, '', NULL);
INSERT INTO `0_form_items` VALUES (2, 50, 1, 3, '', NULL);
INSERT INTO `0_form_items` VALUES (2, 51, 12, 3, '', NULL);
INSERT INTO `0_form_items` VALUES (2, 60, 10, 2, 'DEF', NULL);
INSERT INTO `0_form_items` VALUES (2, 61, 16, 2, 'CWA', NULL);
INSERT INTO `0_form_items` VALUES (3, 10, 10, 4, '', NULL);
INSERT INTO `0_form_items` VALUES (3, 18, 18, 3, '', NULL);
INSERT INTO `0_form_items` VALUES (3, 26, 26, 3, '', NULL);
INSERT INTO `0_form_items` VALUES (3, 30, 30, 3, '', '');
INSERT INTO `0_form_items` VALUES (3, 50, 1, 4, '', NULL);
INSERT INTO `0_form_items` VALUES (3, 51, 12, 4, '', NULL);
INSERT INTO `0_form_items` VALUES (3, 60, 10, 3, 'DEF', NULL);
INSERT INTO `0_form_items` VALUES (3, 61, 25, 1, 'DEF', NULL);
INSERT INTO `0_form_items` VALUES (4, 10, 10, 5, '', '');
INSERT INTO `0_form_items` VALUES (4, 18, 18, 4, '', NULL);
INSERT INTO `0_form_items` VALUES (4, 26, 26, 4, '', '');
INSERT INTO `0_form_items` VALUES (4, 30, 30, 4, '', '');
INSERT INTO `0_form_items` VALUES (4, 50, 4, 2, '1700', NULL);
INSERT INTO `0_form_items` VALUES (4, 51, 12, 5, '', NULL);
INSERT INTO `0_form_items` VALUES (4, 60, 10, 4, 'DEF', NULL);
INSERT INTO `0_form_items` VALUES (4, 61, 25, 2, 'DEF', NULL);
INSERT INTO `0_form_items` VALUES (5, 10, 10, 6, '', '');
INSERT INTO `0_form_items` VALUES (5, 18, 18, 5, '', NULL);
INSERT INTO `0_form_items` VALUES (5, 26, 26, 5, '', '');
INSERT INTO `0_form_items` VALUES (5, 30, 30, 5, '', '');
INSERT INTO `0_form_items` VALUES (5, 50, 22, 2, '', NULL);
INSERT INTO `0_form_items` VALUES (5, 51, 4, 2, '1710', NULL);
INSERT INTO `0_form_items` VALUES (5, 60, 28, 1, 'DEF', '');
INSERT INTO `0_form_items` VALUES (5, 61, 25, 3, 'DEF', NULL);
INSERT INTO `0_form_items` VALUES (6, 10, 10, 7, '', '');
INSERT INTO `0_form_items` VALUES (6, 30, 30, 6, '', '');
INSERT INTO `0_form_items` VALUES (6, 50, 22, 3, '', NULL);
INSERT INTO `0_form_items` VALUES (6, 51, 2, 2, '0', 'mr. mgoo');
INSERT INTO `0_form_items` VALUES (6, 60, 10, 5, 'DEF', '');
INSERT INTO `0_form_items` VALUES (6, 61, 29, 1, 'DEF', '');
INSERT INTO `0_form_items` VALUES (7, 10, 10, 8, '', '');
INSERT INTO `0_form_items` VALUES (7, 30, 30, 7, '', '');
INSERT INTO `0_form_items` VALUES (7, 50, 1, 5, '4', '1');
INSERT INTO `0_form_items` VALUES (7, 51, 12, 6, '2', '1');
INSERT INTO `0_form_items` VALUES (7, 60, 10, 6, 'DEF', '');
INSERT INTO `0_form_items` VALUES (7, 61, 29, 2, 'DEF', '');
INSERT INTO `0_form_items` VALUES (8, 10, 10, 9, '', '');
INSERT INTO `0_form_items` VALUES (8, 30, 30, 8, '', '');
INSERT INTO `0_form_items` VALUES (8, 50, 4, 3, '1700', '1705');
INSERT INTO `0_form_items` VALUES (8, 51, 2, 3, '4', '2');
INSERT INTO `0_form_items` VALUES (8, 60, 10, 7, 'DEF', '');
INSERT INTO `0_form_items` VALUES (9, 10, 10, 10, '', '');
INSERT INTO `0_form_items` VALUES (9, 30, 30, 9, '', '');
INSERT INTO `0_form_items` VALUES (9, 50, 1, 6, '3', '1');
INSERT INTO `0_form_items` VALUES (9, 51, 4, 3, '1705', '1700');
INSERT INTO `0_form_items` VALUES (9, 60, 10, 8, 'DEF', '');
INSERT INTO `0_form_items` VALUES (10, 10, 10, 11, '', '');
INSERT INTO `0_form_items` VALUES (10, 30, 30, 10, '', '');
INSERT INTO `0_form_items` VALUES (10, 50, 1, 7, '0', 'gylle transport');
INSERT INTO `0_form_items` VALUES (10, 51, 2, 4, '2', '1');
INSERT INTO `0_form_items` VALUES (10, 60, 10, 9, 'DEF', '');
INSERT INTO `0_form_items` VALUES (11, 10, 10, 12, '', '');
INSERT INTO `0_form_items` VALUES (11, 30, 30, 11, '', '');
INSERT INTO `0_form_items` VALUES (11, 60, 10, 10, 'DEF', '');
INSERT INTO `0_form_items` VALUES (12, 10, 10, 13, '', '');
INSERT INTO `0_form_items` VALUES (12, 30, 30, 12, '', '');
INSERT INTO `0_form_items` VALUES (12, 60, 10, 11, 'DEF', '');
INSERT INTO `0_form_items` VALUES (13, 10, 10, 14, '', '');
INSERT INTO `0_form_items` VALUES (13, 30, 30, 13, '', '');
INSERT INTO `0_form_items` VALUES (13, 60, 10, 12, 'DEF', '');
INSERT INTO `0_form_items` VALUES (14, 10, 10, 15, '', '');
INSERT INTO `0_form_items` VALUES (14, 30, 30, 14, '', '');
INSERT INTO `0_form_items` VALUES (14, 60, 10, 13, 'DEF', '');
INSERT INTO `0_form_items` VALUES (15, 10, 10, 16, '', '');
INSERT INTO `0_form_items` VALUES (15, 30, 30, 15, '', '');
INSERT INTO `0_form_items` VALUES (15, 60, 10, 14, 'DEF', '');
INSERT INTO `0_form_items` VALUES (16, 60, 10, 15, 'DEF', '');
INSERT INTO `0_form_items` VALUES (17, 60, 10, 16, 'DEF', '');

-- 
-- Data in table `gl_trans`
-- 

INSERT INTO `0_gl_trans` VALUES (1, 10, 2, '2006-01-18', '3000', '', -500, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (2, 10, 2, '2006-01-18', '3000', '', -1000, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (3, 10, 2, '2006-01-18', '3000', '', -250, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (4, 10, 2, '2006-01-18', '1500', '', 1750, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (5, 10, 3, '2006-01-18', '3000', '', -1000, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (6, 10, 3, '2006-01-18', '1500', '', 1000, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (7, 12, 2, '2006-01-18', '1700', '', 5000, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (8, 12, 2, '2006-01-18', '1500', '', -10000, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (9, 12, 3, '2006-01-18', '1700', '', 240, 0, 0, 2, 0x32);
INSERT INTO `0_gl_trans` VALUES (10, 12, 3, '2006-01-18', '1500', '', -240, 0, 0, 2, 0x32);
INSERT INTO `0_gl_trans` VALUES (11, 12, 4, '2006-01-18', '1700', '', 360, 0, 0, 2, 0x32);
INSERT INTO `0_gl_trans` VALUES (12, 12, 4, '2006-01-18', '1500', '', -360, 0, 0, 2, 0x32);
INSERT INTO `0_gl_trans` VALUES (13, 12, 5, '2006-01-18', '1700', '', 500, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (14, 12, 5, '2006-01-18', '1500', '', -500, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (15, 11, 2, '2006-01-18', '3000', '', 1260, 0, 0, 2, 0x32);
INSERT INTO `0_gl_trans` VALUES (16, 11, 2, '2006-01-18', '1500', '', -1260, 0, 0, 2, 0x32);
INSERT INTO `0_gl_trans` VALUES (17, 10, 4, '2006-01-18', '3000', '', -1268.57142857, 0, 0, 2, 0x32);
INSERT INTO `0_gl_trans` VALUES (18, 10, 4, '2006-01-18', '1500', '', 1332, 0, 0, 2, 0x32);
INSERT INTO `0_gl_trans` VALUES (19, 10, 4, '2006-01-18', '2660', '', -63.4285714286, 0, 0, 2, 0x32);
INSERT INTO `0_gl_trans` VALUES (20, 1, 2, '2006-01-18', '1700', '', -25, 0, 0, 0, NULL);
INSERT INTO `0_gl_trans` VALUES (21, 1, 2, '2006-01-18', '6600', '', 10, 0, 0, 0, NULL);
INSERT INTO `0_gl_trans` VALUES (22, 1, 2, '2006-01-18', '6730', '', 15, 0, 0, 0, NULL);
INSERT INTO `0_gl_trans` VALUES (23, 1, 3, '2006-01-18', '1705', '', -250, 0, 0, 0, NULL);
INSERT INTO `0_gl_trans` VALUES (24, 1, 3, '2006-01-18', '6810', '', 3000, 0, 0, 0, NULL);
INSERT INTO `0_gl_trans` VALUES (25, 1, 3, '2006-01-18', '6700', '', 150, 0, 0, 0, NULL);
INSERT INTO `0_gl_trans` VALUES (26, 1, 4, '2006-01-18', '1700', '', -555, 0, 0, 4, 0x31);
INSERT INTO `0_gl_trans` VALUES (27, 1, 4, '2006-01-18', '4500', '', 555, 0, 0, 4, 0x31);
INSERT INTO `0_gl_trans` VALUES (28, 4, 2, '2006-01-18', '1700', '', -300, 0, 0, 0, NULL);
INSERT INTO `0_gl_trans` VALUES (29, 4, 2, '2006-01-18', '1710', '', 300, 0, 0, 0, NULL);
INSERT INTO `0_gl_trans` VALUES (30, 22, 2, '2006-01-18', '2630', '', 5000, 0, 0, 3, 0x31);
INSERT INTO `0_gl_trans` VALUES (31, 22, 2, '2006-01-18', '1700', '', -5000, 0, 0, 3, 0x31);
INSERT INTO `0_gl_trans` VALUES (32, 22, 3, '2006-01-18', '2630', '', 3960, 0, 0, 3, 0x32);
INSERT INTO `0_gl_trans` VALUES (33, 22, 3, '2006-01-18', '1710', '', -3960, 0, 0, 3, 0x32);
INSERT INTO `0_gl_trans` VALUES (34, 20, 2, '2006-01-18', '2630', '', -3445, 0, 0, 3, 0x31);
INSERT INTO `0_gl_trans` VALUES (35, 20, 2, '2006-01-18', '1420', '', 1000, 0, 0, 3, 0x31);
INSERT INTO `0_gl_trans` VALUES (36, 20, 2, '2006-01-18', '1420', '', 2250, 0, 0, 3, 0x31);
INSERT INTO `0_gl_trans` VALUES (37, 20, 2, '2006-01-18', '2660', '', 162.5, 0, 0, 3, 0x31);
INSERT INTO `0_gl_trans` VALUES (38, 20, 2, '2006-01-18', '2660', '', 32.5, 0, 0, 3, 0x31);
INSERT INTO `0_gl_trans` VALUES (39, 20, 3, '2006-01-18', '2630', '', -26, 0, 0, 3, 0x31);
INSERT INTO `0_gl_trans` VALUES (40, 20, 3, '2006-01-18', '1420', '', 26, 0, 0, 3, 0x31);
INSERT INTO `0_gl_trans` VALUES (41, 2, 2, '2006-01-20', '1700', '', 1050, 0, 0, 0, NULL);
INSERT INTO `0_gl_trans` VALUES (42, 2, 2, '2006-01-20', '4500', '', -1000, 0, 0, 0, NULL);
INSERT INTO `0_gl_trans` VALUES (43, 2, 2, '2006-01-20', '6400', '', -50, 0, 0, 0, NULL);
INSERT INTO `0_gl_trans` VALUES (44, 10, 5, '2007-01-28', '3000', '', -2000, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (45, 10, 5, '2007-01-28', '3000', '', -90000, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (46, 10, 5, '2007-01-28', '3000', '', -1050, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (47, 10, 5, '2007-01-28', '1500', '', 93050, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (48, 10, 6, '2007-01-30', '3000', '', -333, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (49, 10, 6, '2007-01-30', '1500', '', 333, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (50, 12, 6, '2007-01-30', '1700', '', 200, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (51, 12, 6, '2007-01-30', '1500', '', -200, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (52, 1, 5, '2007-01-30', '1700', '', -200, 0, 0, 4, 0x31);
INSERT INTO `0_gl_trans` VALUES (53, 1, 5, '2007-01-30', '1400', 'Gylle', 200, 0, 0, 4, 0x31);
INSERT INTO `0_gl_trans` VALUES (54, 2, 3, '2007-01-30', '1700', '', 70, 0, 0, 4, 0x32);
INSERT INTO `0_gl_trans` VALUES (55, 2, 3, '2007-01-30', '4500', 'Packing', -50, 0, 0, 4, 0x32);
INSERT INTO `0_gl_trans` VALUES (56, 2, 3, '2007-01-30', '6400', '', -20, 0, 0, 4, 0x32);
INSERT INTO `0_gl_trans` VALUES (57, 20, 4, '2007-01-30', '2630', '', -17350, 0, 0, 3, 0x31);
INSERT INTO `0_gl_trans` VALUES (58, 20, 4, '2007-01-30', '1420', '', 15100, 0, 0, 3, 0x31);
INSERT INTO `0_gl_trans` VALUES (59, 20, 4, '2007-01-30', '1420', '', 2250, 0, 0, 3, 0x31);
INSERT INTO `0_gl_trans` VALUES (60, 0, 2, '2007-02-02', '3000', 'nana', -100, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (61, 0, 2, '2007-02-02', '4010', 'jojo', 100, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (62, 0, 3, '2007-02-02', '3000', '', -25, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (63, 0, 3, '2007-02-02', '4000', '', 25, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (64, 0, 4, '2007-03-01', '3000', '', 25, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (65, 0, 4, '2007-03-01', '4000', '', -25, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (66, 0, 5, '2007-02-02', '3000', '', -50, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (67, 0, 5, '2007-02-02', '4000', '', 50, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (68, 0, 6, '2007-02-02', '3020', 'Til Ejnar', -400, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (69, 0, 6, '2007-02-02', '1500', 'Opdate forrige linje', 400, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (70, 20, 5, '2007-02-03', '2630', '', -6644, 0, 0, 3, 0x31);
INSERT INTO `0_gl_trans` VALUES (71, 20, 5, '2007-02-03', '1420', '', 6644, 0, 0, 3, 0x31);
INSERT INTO `0_gl_trans` VALUES (72, 10, 7, '2007-02-03', '3000', '', -333, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (73, 10, 7, '2007-02-03', '1500', '', 333, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (74, 10, 8, '2007-02-03', '3000', '', -333, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (75, 10, 8, '2007-02-03', '1500', '', 333, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (76, 10, 9, '2007-02-03', '3000', '', -3428.5714285714, 0, 0, 2, 0x32);
INSERT INTO `0_gl_trans` VALUES (77, 10, 9, '2007-02-03', '1500', '', 3600, 0, 0, 2, 0x32);
INSERT INTO `0_gl_trans` VALUES (78, 10, 9, '2007-02-03', '2660', '', -171.42857142857, 0, 0, 2, 0x32);
INSERT INTO `0_gl_trans` VALUES (79, 10, 10, '2007-02-04', '3000', '', -333, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (80, 10, 10, '2007-02-04', '1500', '', 333, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (81, 10, 11, '2007-02-04', '3000', '', -333, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (82, 10, 11, '2007-02-04', '1500', '', 333, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (83, 10, 12, '2007-02-06', '3000', '', -333, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (84, 10, 12, '2007-02-06', '1500', '', 333, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (85, 0, 7, '2007-02-06', '1400', '', 100, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (86, 0, 7, '2007-02-06', '1500', '', -100, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (87, 0, 8, '2007-02-06', '1400', '', 100, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (88, 0, 8, '2007-02-06', '2000', '', -100, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (89, 0, 9, '2006-04-06', '1400', '', 100, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (90, 0, 9, '2006-04-06', '2000', '', -100, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (91, 0, 10, '2007-02-06', '1400', '', 100, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (92, 0, 10, '2007-02-06', '2050', '', -100, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (93, 0, 11, '2006-04-06', '1400', '', 100, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (94, 0, 11, '2006-04-06', '3000', '', -100, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (95, 0, 12, '2007-02-06', '1400', '', 100, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (96, 0, 12, '2007-02-06', '3000', '', -100, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (97, 0, 13, '2007-02-13', '1400', '', 10, 1, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (98, 0, 13, '2007-02-13', '1400', '', -10, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (99, 10, 13, '2007-02-25', '3000', '', -333, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (100, 10, 13, '2007-02-25', '1500', '', 333, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (101, 0, 14, '2007-02-25', '1400', '', 10, 1, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (102, 0, 14, '2007-02-25', '3000', '', -10, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (103, 0, 15, '2007-03-02', '1400', '', 10, 1, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (104, 0, 15, '2007-03-02', '1400', '', -10, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (105, 0, 16, '2007-03-02', '3000', '', 100, 1, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (106, 0, 16, '2007-03-02', '1430', '', -100, 1, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (107, 10, 14, '2007-03-04', '3000', '', -333, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (108, 10, 14, '2007-03-04', '1500', '', 333, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (109, 10, 15, '2007-03-05', '3000', '', -333, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (110, 10, 15, '2007-03-05', '1500', '', 358, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (111, 10, 15, '2007-03-05', '3800', '', -25, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (112, 10, 16, '2007-03-05', '3000', '', -3000, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (113, 10, 16, '2007-03-05', '1500', '', 3044, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (114, 10, 16, '2007-03-05', '3800', '', -44, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (115, 20, 6, '2007-03-05', '2630', '', -33.34, 0, 0, 3, 0x33);
INSERT INTO `0_gl_trans` VALUES (116, 20, 6, '2007-03-05', '6730', '', 33.34, 1, 0, 3, 0x33);
INSERT INTO `0_gl_trans` VALUES (117, 4, 3, '2007-03-09', '1700', '', -222, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (118, 4, 3, '2007-03-09', '1705', '', 222, 0, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (119, 2, 4, '2007-03-09', '1700', '', 200, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (120, 2, 4, '2007-03-09', '1400', '', -200, 0, 0, 2, 0x31);
INSERT INTO `0_gl_trans` VALUES (121, 1, 6, '2007-03-22', '1700', '', -200, 0, 0, 3, 0x31);
INSERT INTO `0_gl_trans` VALUES (122, 1, 6, '2007-03-22', '2630', '', 200, 0, 0, 3, 0x31);
INSERT INTO `0_gl_trans` VALUES (123, 1, 7, '2007-03-22', '1700', '', -125, 0, 0, 0, 0x67796c6c657472616e73706f7274);
INSERT INTO `0_gl_trans` VALUES (124, 1, 7, '2007-03-22', '6700', '', 100, 0, 0, 0, 0x67796c6c657472616e73706f7274);
INSERT INTO `0_gl_trans` VALUES (125, 1, 7, '2007-03-22', '2682', '', 25, 0, 0, 0, 0x67796c6c657472616e73706f7274);
INSERT INTO `0_gl_trans` VALUES (126, 0, 17, '2007-03-25', '3000', '', -300, 1, 0, NULL, NULL);
INSERT INTO `0_gl_trans` VALUES (127, 0, 17, '2007-03-25', '4200', '', 300, 0, 0, NULL, NULL);

-- 
-- Data in table `grn_batch`
-- 

INSERT INTO `0_grn_batch` VALUES (1, 1, 1, '1', '2006-01-18', 'DEF');
INSERT INTO `0_grn_batch` VALUES (2, 1, 2, '2', '2006-01-18', 'DEF');
INSERT INTO `0_grn_batch` VALUES (3, 1, 5, '3', '2006-01-18', 'DEF');

-- 
-- Data in table `grn_items`
-- 

INSERT INTO `0_grn_items` VALUES (1, 1, 1, '102', '17 inch VGA Monitor', 10, 10);
INSERT INTO `0_grn_items` VALUES (2, 1, 2, '103', '32MB VGA Card', 50, 50);
INSERT INTO `0_grn_items` VALUES (3, 2, 3, '104', '52x CD Drive', 1, 1);
INSERT INTO `0_grn_items` VALUES (4, 3, 6, '104', '52x CD Drive (upgraded)', 3020, 302);

-- 
-- Data in table `item_tax_types`
-- 

INSERT INTO `0_item_tax_types` VALUES (1, 'Regular', 0);

-- 
-- Data in table `loc_stock`
-- 

INSERT INTO `0_loc_stock` VALUES ('CWA', '102', 0);
INSERT INTO `0_loc_stock` VALUES ('CWA', '103', 0);
INSERT INTO `0_loc_stock` VALUES ('CWA', '104', 0);
INSERT INTO `0_loc_stock` VALUES ('CWA', '201', 0);
INSERT INTO `0_loc_stock` VALUES ('CWA', '3400', 0);
INSERT INTO `0_loc_stock` VALUES ('DEF', '102', 0);
INSERT INTO `0_loc_stock` VALUES ('DEF', '103', 0);
INSERT INTO `0_loc_stock` VALUES ('DEF', '104', 0);
INSERT INTO `0_loc_stock` VALUES ('DEF', '201', 0);
INSERT INTO `0_loc_stock` VALUES ('DEF', '3400', 0);

-- 
-- Data in table `locations`
-- 

INSERT INTO `0_locations` VALUES ('DEF', 'Default', 'N/A', '', '', '', '');
INSERT INTO `0_locations` VALUES ('CWA', 'Cool Warehouse', '', '', '', '', '');

-- 
-- Data in table `movement_types`
-- 

INSERT INTO `0_movement_types` VALUES (1, 'Adjustment');

-- 
-- Data in table `payment_terms`
-- 

INSERT INTO `0_payment_terms` VALUES (1, 'Due 15th Of the Following Month', 0, 17);
INSERT INTO `0_payment_terms` VALUES (2, 'Due By End Of The Following Month', 0, 30);
INSERT INTO `0_payment_terms` VALUES (3, 'Payment due within 10 days', 10, 0);
INSERT INTO `0_payment_terms` VALUES (4, 'Cash Only', 1, 0);

-- 
-- Data in table `prices`
-- 

INSERT INTO `0_prices` VALUES (1, '102', 1, 'USD', 333);
INSERT INTO `0_prices` VALUES (2, '103', 1, 'USD', 3000);
INSERT INTO `0_prices` VALUES (3, '104', 1, 'USD', 34);
INSERT INTO `0_prices` VALUES (4, '201', 1, 'USD', 40);
INSERT INTO `0_prices` VALUES (5, '3400', 1, 'USD', 600);

-- 
-- Data in table `purch_order_details`
-- 

INSERT INTO `0_purch_order_details` VALUES (1, 1, '102', '17 inch VGA Monitor', '2006-01-28', 10, 3020, 3020, 0, 3000, 10);
INSERT INTO `0_purch_order_details` VALUES (2, 1, '103', '32MB VGA Card', '2006-01-28', 50, 90, 90, 0, 300, 50);
INSERT INTO `0_purch_order_details` VALUES (3, 2, '104', '52x CD Drive', '2006-01-28', 1, 26, 26, 0, 1, 1);
INSERT INTO `0_purch_order_details` VALUES (4, 3, '104', '52x CD Drive', '2006-01-28', 0, 22, 0, 0, 1, 0);
INSERT INTO `0_purch_order_details` VALUES (6, 5, '104', '52x CD Drive', '2006-01-28', 302, 22, 22, 0, 330, 3020);

-- 
-- Data in table `purch_orders`
-- 

INSERT INTO `0_purch_orders` VALUES (1, 1, '', '2006-01-18', '3', '333', 'DEF', 'N/A');
INSERT INTO `0_purch_orders` VALUES (2, 1, '', '2006-01-18', '4', '44', 'DEF', 'N/A');
INSERT INTO `0_purch_orders` VALUES (3, 1, '', '2006-01-18', '5', '', 'DEF', 'N/A');
INSERT INTO `0_purch_orders` VALUES (5, 1, '', '2006-01-18', '7', '', 'DEF', 'N/A');

-- 
-- Data in table `refs`
-- 

INSERT INTO `0_refs` VALUES (2, 0, 'Joe');
INSERT INTO `0_refs` VALUES (3, 0, '19');
INSERT INTO `0_refs` VALUES (4, 0, '19');
INSERT INTO `0_refs` VALUES (5, 0, '20');
INSERT INTO `0_refs` VALUES (6, 0, '21');
INSERT INTO `0_refs` VALUES (7, 0, '22');
INSERT INTO `0_refs` VALUES (8, 0, '23');
INSERT INTO `0_refs` VALUES (9, 0, '24');
INSERT INTO `0_refs` VALUES (10, 0, '25');
INSERT INTO `0_refs` VALUES (11, 0, '26');
INSERT INTO `0_refs` VALUES (12, 0, '27');
INSERT INTO `0_refs` VALUES (13, 0, '28');
INSERT INTO `0_refs` VALUES (14, 0, '29');
INSERT INTO `0_refs` VALUES (15, 0, '30');
INSERT INTO `0_refs` VALUES (16, 0, '31');
INSERT INTO `0_refs` VALUES (17, 0, '32');

-- 
-- Data in table `sales_order_details`
-- 

INSERT INTO `0_sales_order_details` VALUES (1, '102', '17 inch VGA Monitor', 10, 250, 10, 0);
INSERT INTO `0_sales_order_details` VALUES (1, '103', '32MB VGA Card', 50, 3000, 50, 0);
INSERT INTO `0_sales_order_details` VALUES (1, '104', '52x CD Drive', 26, 50, 26, 0);
INSERT INTO `0_sales_order_details` VALUES (2, '102', '17 inch VGA Monitor', 5, 222, 25, 0);
INSERT INTO `0_sales_order_details` VALUES (3, '102', '17 inch VGA Monitor', 1, 333, 1, 0);
INSERT INTO `0_sales_order_details` VALUES (4, '102', '17 inch VGA Monitor', 1, 333, 1, 0);
INSERT INTO `0_sales_order_details` VALUES (5, '102', '17 inch VGA Monitor', 1, 333, 1, 0);
INSERT INTO `0_sales_order_details` VALUES (6, '3400', 'P4 Business System', 1, 3000, 1, 0);
INSERT INTO `0_sales_order_details` VALUES (7, '102', '17 inch VGA Monitor', 0, 333, 1, 0);
INSERT INTO `0_sales_order_details` VALUES (8, '102', '17 inch VGA Monitor', 0, 333, 1, 0);
INSERT INTO `0_sales_order_details` VALUES (9, '102', '17 inch VGA Monitor', 1, 333, 1, 0);
INSERT INTO `0_sales_order_details` VALUES (10, '102', '17 inch VGA Monitor', 1, 333, 1, 0);
INSERT INTO `0_sales_order_details` VALUES (11, '102', '17 inch VGA Monitor', 1, 333, 1, 0);
INSERT INTO `0_sales_order_details` VALUES (12, '102', '17 inch VGA Monitor', 1, 333, 1, 0);
INSERT INTO `0_sales_order_details` VALUES (13, '102', '17 inch VGA Monitor', 1, 333, 1, 0);
INSERT INTO `0_sales_order_details` VALUES (14, '102', '17 inch VGA Monitor', 1, 333, 1, 0);
INSERT INTO `0_sales_order_details` VALUES (15, '103', '32MB VGA Card', 1, 3000, 1, 0);

-- 
-- Data in table `sales_orders`
-- 

INSERT INTO `0_sales_orders` VALUES (1, 1, 1, '', '', '2006-01-18', 1, 1, 'The same', '', '', 'Main', 0, 'DEF', '2006-01-18');
INSERT INTO `0_sales_orders` VALUES (2, 2, 3, '', '', '2006-01-18', 1, 1, 'His Addy', '', '', 'Main', 0, 'DEF', '2006-01-18');
INSERT INTO `0_sales_orders` VALUES (3, 1, 1, 'Oops', 'This is a lot of stuff.', '2007-01-30', 1, 1, 'The Road\r\n333 33  Downtown', '040-365045', '', 'Main', 0, 'DEF', '2007-01-30');
INSERT INTO `0_sales_orders` VALUES (4, 1, 1, '', '', '2007-02-03', 1, 1, 'The Road', '', '', 'Main', 0, 'DEF', '2007-02-03');
INSERT INTO `0_sales_orders` VALUES (5, 1, 1, '', '', '2007-02-03', 1, 1, 'The Road', '', '', 'Main', 0, 'DEF', '2007-02-03');
INSERT INTO `0_sales_orders` VALUES (6, 2, 3, '', '', '2007-02-03', 1, 1, 'Street', '', '', 'Main', 0, 'DEF', '2007-02-03');
INSERT INTO `0_sales_orders` VALUES (7, 1, 1, '', '', '2007-02-03', 1, 1, 'The Road', '', '', 'Main', 0, 'DEF', '2007-02-03');
INSERT INTO `0_sales_orders` VALUES (8, 1, 1, '', '', '2007-02-03', 1, 1, 'The Road', '', '', 'Main', 0, 'DEF', '2007-02-03');
INSERT INTO `0_sales_orders` VALUES (9, 1, 1, '', '', '2007-02-04', 1, 1, 'The Road', '', '', 'Main', 0, 'DEF', '2007-02-04');
INSERT INTO `0_sales_orders` VALUES (10, 1, 1, '', '', '2007-02-04', 1, 1, 'The Road', '', '', 'Main', 0, 'DEF', '2007-02-04');
INSERT INTO `0_sales_orders` VALUES (11, 1, 1, '', '', '2007-02-06', 1, 1, 'The Road', '', '', 'Main', 0, 'DEF', '2007-02-06');
INSERT INTO `0_sales_orders` VALUES (12, 1, 1, '', '', '2007-02-25', 1, 1, 'The Road', '', '', 'Main', 0, 'DEF', '2007-02-25');
INSERT INTO `0_sales_orders` VALUES (13, 1, 1, '', '', '2007-03-04', 1, 1, 'The Road', '', '', 'Main', 0, 'DEF', '2007-03-04');
INSERT INTO `0_sales_orders` VALUES (14, 1, 1, '', '', '2007-03-05', 1, 1, 'The Road', '', '', 'Main', 25, 'DEF', '2007-03-05');
INSERT INTO `0_sales_orders` VALUES (15, 1, 1, '', '', '2007-03-05', 1, 1, 'The Road', '', '', 'Main', 44, 'DEF', '2007-03-05');

-- 
-- Data in table `sales_types`
-- 

INSERT INTO `0_sales_types` VALUES (1, 'Retail');
INSERT INTO `0_sales_types` VALUES (2, 'Wholesale');

-- 
-- Data in table `salesman`
-- 

INSERT INTO `0_salesman` VALUES (1, 'Sparc Menser', '', '', '');
INSERT INTO `0_salesman` VALUES (2, 'Joe Hunt', '', '', '');

-- 
-- Data in table `shippers`
-- 

INSERT INTO `0_shippers` VALUES (1, 'UPS', '', '', '');
INSERT INTO `0_shippers` VALUES (2, 'Internet', '', '', '');

-- 
-- Data in table `stock_category`
-- 

INSERT INTO `0_stock_category` VALUES (1, 'Components', NULL, NULL, NULL, NULL);
INSERT INTO `0_stock_category` VALUES (2, 'Charges', NULL, NULL, NULL, NULL);
INSERT INTO `0_stock_category` VALUES (3, 'Systems', NULL, NULL, NULL, NULL);
INSERT INTO `0_stock_category` VALUES (4, 'Services', NULL, NULL, NULL, NULL);

-- 
-- Data in table `stock_master`
-- 

INSERT INTO `0_stock_master` VALUES ('102', 1, 1, '17 inch VGA Monitor', '', 'each', 'B', '3000', '4010', '1420', '4210', '0', NULL, NULL, 0, 0, 0, 0, 0);
INSERT INTO `0_stock_master` VALUES ('103', 1, 1, '32MB VGA Card', '', 'each', 'B', '3000', '4010', '1420', '4210', '0', NULL, NULL, 0, 0, 0, 0, 0);
INSERT INTO `0_stock_master` VALUES ('104', 1, 1, '52x CD Drive', '', 'each', 'B', '3000', '4010', '1420', '4210', '0', NULL, NULL, 0, 0, 0, 0, 0);
INSERT INTO `0_stock_master` VALUES ('201', 2, 1, 'Assembly Labour', '', 'each', 'D', '3000', '4010', '1420', '4210', '0', NULL, NULL, 0, 0, 0, 0, 0);
INSERT INTO `0_stock_master` VALUES ('3400', 3, 1, 'P4 Business System', '', 'each', 'M', '3000', '4010', '1420', '4210', '1400', NULL, NULL, 0, 0, 0, 0, 0);

-- 
-- Data in table `stock_moves`
-- 

INSERT INTO `0_stock_moves` VALUES (1, 2, '102', 17, 'DEF', '2006-01-18', 1, 0, '1', 3000, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (2, 2, '103', 17, 'DEF', '2006-01-18', 1, 0, '1', 3000, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (3, 2, '104', 17, 'DEF', '2006-01-18', 1, 0, '1', 150, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (4, 2, '3400', 17, 'DEF', '2006-01-18', 1, 0, '1', 50, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (5, 2, '102', 16, 'DEF', '2006-01-18', 1, 0, '1', -25, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (6, 2, '102', 16, 'CWA', '2006-01-18', 1, 0, '1', 25, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (7, 2, '102', 10, 'DEF', '2006-01-18', 0, 250, '', -2, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (8, 2, '103', 10, 'DEF', '2006-01-18', 0, 3000, '', -10, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (9, 2, '104', 10, 'DEF', '2006-01-18', 0, 50, '', -5, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (10, 3, '103', 10, 'DEF', '2006-01-18', 0, 3000, '', -10, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (11, 2, '102', 11, 'DEF', '2006-01-18', 0, 210, '', 5, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (12, 4, '102', 10, 'DEF', '2006-01-18', 0, 222, '', -5, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (13, 1, '102', 26, 'DEF', '2006-01-18', 0, 0, '1', -20, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (14, 1, '103', 26, 'DEF', '2006-01-18', 0, 0, '1', -20, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (15, 1, '104', 26, 'DEF', '2006-01-18', 0, 0, '1', -20, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (16, 1, '3400', 26, 'DEF', '2006-01-18', 0, 0, '1', 20, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (17, 2, '102', 26, 'DEF', '2006-01-18', 0, 0, '2', -5, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (18, 2, '103', 26, 'DEF', '2006-01-18', 0, 0, '2', -5, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (19, 2, '104', 26, 'DEF', '2006-01-18', 0, 0, '2', -5, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (20, 2, '3400', 26, 'DEF', '2006-01-18', 0, 0, '2', 5, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (21, 1, '102', 25, 'DEF', '2006-01-18', 1, 3020, '', 10, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (22, 1, '103', 25, 'DEF', '2006-01-18', 1, 90, '', 50, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (23, 2, '104', 25, 'DEF', '2006-01-18', 1, 26, '', 1, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (24, 3, '104', 25, 'DEF', '2006-01-18', 1, 22, '', 3020, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (25, 1, '102', 28, 'DEF', '2006-01-20', 0, 0, '', -10, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (26, 5, '102', 10, 'DEF', '2007-01-28', 0, 250, '', -8, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (27, 5, '103', 10, 'DEF', '2007-01-28', 0, 3000, '', -30, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (28, 5, '104', 10, 'DEF', '2007-01-28', 0, 50, '', -21, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (29, 6, '102', 10, 'DEF', '2007-01-30', 0, 333, '', -1, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (30, 4, '102', 26, 'DEF', '2007-01-30', 0, 0, '4', -5, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (31, 4, '103', 26, 'DEF', '2007-01-30', 0, 0, '4', -5, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (32, 4, '104', 26, 'DEF', '2007-01-30', 0, 0, '4', -5, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (33, 4, '3400', 26, 'DEF', '2007-01-30', 0, 0, '4', 5, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (34, 1, '3400', 29, 'DEF', '2007-01-30', 0, 0, '', 50, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (35, 2, '3400', 29, 'DEF', '2007-01-30', 0, 0, '', 20, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (36, 7, '102', 10, 'DEF', '2007-02-03', 0, 333, '', -1, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (37, 8, '102', 10, 'DEF', '2007-02-03', 0, 333, '', -1, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (38, 9, '3400', 10, 'DEF', '2007-02-03', 0, 3000, '', -1, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (39, 10, '102', 10, 'DEF', '2007-02-04', 0, 333, '', -1, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (40, 11, '102', 10, 'DEF', '2007-02-04', 0, 333, '', -1, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (41, 12, '102', 10, 'DEF', '2007-02-06', 0, 333, '', -1, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (42, 13, '102', 10, 'DEF', '2007-02-25', 0, 333, '', -1, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (43, 14, '102', 10, 'DEF', '2007-03-04', 0, 333, '', -1, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (44, 15, '102', 10, 'DEF', '2007-03-05', 0, 333, '', -1, 0, 0, 1);
INSERT INTO `0_stock_moves` VALUES (45, 16, '103', 10, 'DEF', '2007-03-05', 0, 3000, '', -1, 0, 0, 1);

-- 
-- Data in table `supp_allocations`
-- 

INSERT INTO `0_supp_allocations` VALUES (7, 1529, '2007-01-30', 2, 22, 4, 20);
INSERT INTO `0_supp_allocations` VALUES (8, 3445, '2007-01-30', 2, 22, 2, 20);
INSERT INTO `0_supp_allocations` VALUES (9, 26, '2007-01-30', 2, 22, 3, 20);

-- 
-- Data in table `supp_invoice_items`
-- 

INSERT INTO `0_supp_invoice_items` VALUES (1, 2, 20, 0, 1, 1, '102', '17 inch VGA Monitor', 5, 3020, 0, '');
INSERT INTO `0_supp_invoice_items` VALUES (2, 2, 20, 0, 2, 2, '103', '32MB VGA Card', 25, 90, 0, '');
INSERT INTO `0_supp_invoice_items` VALUES (3, 3, 20, 0, 3, 3, '104', '52x CD Drive', 1, 26, 0, '');
INSERT INTO `0_supp_invoice_items` VALUES (4, 4, 20, 0, 1, 1, '102', '17 inch VGA Monitor', 5, 3020, 0, '');
INSERT INTO `0_supp_invoice_items` VALUES (5, 4, 20, 0, 2, 2, '103', '32MB VGA Card', 25, 90, 0, '');
INSERT INTO `0_supp_invoice_items` VALUES (6, 5, 20, 0, 4, 6, '104', '52x CD Drive (upgraded)', 302, 22, 0, '');
INSERT INTO `0_supp_invoice_items` VALUES (7, 6, 20, 6730, 0, 0, '', '', 0, 200, 0, 'yes');

-- 
-- Data in table `supp_invoice_tax_items`
-- 

INSERT INTO `0_supp_invoice_tax_items` VALUES (1, 2, 20, 1, NULL, 5, 0, 162.5);
INSERT INTO `0_supp_invoice_tax_items` VALUES (2, 2, 20, 2, NULL, 1, 0, 32.5);

-- 
-- Data in table `supp_trans`
-- 

INSERT INTO `0_supp_trans` VALUES (2, 20, 1, '22', '22', '2006-01-18', '2006-02-22', 3250, 0, 195, 1, 3445);
INSERT INTO `0_supp_trans` VALUES (2, 22, 1, '1', '', '2006-01-18', '2006-01-18', -5000, 0, 0, 1, 5000);
INSERT INTO `0_supp_trans` VALUES (3, 20, 1, '23', 'asdf', '2006-01-18', '2006-02-22', 26, 0, 0, 1, 26);
INSERT INTO `0_supp_trans` VALUES (3, 22, 2, '2', '', '2006-01-18', '2006-01-18', -3300, 0, 0, 1.2, 0);
INSERT INTO `0_supp_trans` VALUES (4, 20, 1, '24', 'Hamselv', '2007-01-30', '2007-02-22', 17350, 0, 0, 1, 1529);
INSERT INTO `0_supp_trans` VALUES (5, 20, 1, '25', '6789', '2007-02-03', '2007-03-22', 6644, 0, 0, 1, 0);
INSERT INTO `0_supp_trans` VALUES (6, 1, 1, '5', '', '2007-03-22', '0000-00-00', -200, 0, 0, 1, 0);
INSERT INTO `0_supp_trans` VALUES (6, 20, 3, '26', '333333', '2007-03-05', '2007-03-12', 200, 0, 0, 0.2, 0);

-- 
-- Data in table `suppliers`
-- 

INSERT INTO `0_suppliers` VALUES (1, 'Ghostbusters Corp.', '', '', '123456789', 'USD', 1, 0, 0, 2, '4000', '2630', '4250');
INSERT INTO `0_suppliers` VALUES (2, 'Beefeater Ltd.', '', '', '987654321', 'GBP', 1, 0, 0, 2, '4000', '2630', '4250');
INSERT INTO `0_suppliers` VALUES (3, 'Super Trooper AB', 'Adress', 'sven@sven.sve', '123456', 'SEK', 3, 0, 0, 2, '4000', '2630', '4250');

-- 
-- Data in table `sys_types`
-- 

INSERT INTO `0_sys_types` VALUES (0, 'Journal - GL', 17, '33');
INSERT INTO `0_sys_types` VALUES (1, 'Payment - GL', 7, '7');
INSERT INTO `0_sys_types` VALUES (2, 'Receipt - GL', 4, '14');
INSERT INTO `0_sys_types` VALUES (4, 'Funds Transfer', 3, '6');
INSERT INTO `0_sys_types` VALUES (10, 'Sales Invoice', 16, '16');
INSERT INTO `0_sys_types` VALUES (11, 'Credit Note', 2, '34');
INSERT INTO `0_sys_types` VALUES (12, 'Receipt', 6, '116');
INSERT INTO `0_sys_types` VALUES (16, 'Location Transfer', 2, '2');
INSERT INTO `0_sys_types` VALUES (17, 'Inventory Adjustment', 2, '2');
INSERT INTO `0_sys_types` VALUES (18, 'Purchase Order', 1, '8');
INSERT INTO `0_sys_types` VALUES (20, 'Supplier Invoice', 6, '27');
INSERT INTO `0_sys_types` VALUES (21, 'Supplier Credit Note', 1, '2');
INSERT INTO `0_sys_types` VALUES (22, 'Supplier Payment', 3, '3');
INSERT INTO `0_sys_types` VALUES (25, 'Purchase Order Delivery', 1, '4');
INSERT INTO `0_sys_types` VALUES (26, 'Work Order', 1, '6');
INSERT INTO `0_sys_types` VALUES (28, 'Work Order Issue', 1, '2');
INSERT INTO `0_sys_types` VALUES (29, 'Work Order Production', 1, '201');
INSERT INTO `0_sys_types` VALUES (30, 'Sales Order', 1, '1');
INSERT INTO `0_sys_types` VALUES (35, 'Cost Update', 1, '1');
INSERT INTO `0_sys_types` VALUES (40, 'Dimension', 1, '3');

-- 
-- Data in table `tax_group_items`
-- 

INSERT INTO `0_tax_group_items` VALUES (1, 1, 5, 0);
INSERT INTO `0_tax_group_items` VALUES (1, 2, 1, 0);
INSERT INTO `0_tax_group_items` VALUES (3, 1, 5, 1);
INSERT INTO `0_tax_group_items` VALUES (4, 3, 25, 0);

-- 
-- Data in table `tax_groups`
-- 

INSERT INTO `0_tax_groups` VALUES (1, 'Tax out', 0);
INSERT INTO `0_tax_groups` VALUES (2, 'Tax-Free', 0);
INSERT INTO `0_tax_groups` VALUES (3, 'Tax Included', 0);
INSERT INTO `0_tax_groups` VALUES (4, 'Tax In', 0);

-- 
-- Data in table `tax_types`
-- 

INSERT INTO `0_tax_types` VALUES (1, 5, '2660', '2680', 'VAT out 5', 1);
INSERT INTO `0_tax_types` VALUES (2, 1, '2662', '2680', 'Manufact tax 1', 1);
INSERT INTO `0_tax_types` VALUES (3, 25, '2664', '2660', 'VAT out 25', 1);
INSERT INTO `0_tax_types` VALUES (4, 5, '2660', '2680', 'VAT in 5', 0);
INSERT INTO `0_tax_types` VALUES (5, 25, '2660', '2682', 'VAT in 25', 0);

-- 
-- Data in table `users`
-- 

INSERT INTO `0_users` VALUES ('demouser', '5f4dcc3b5aa765d61d8327deb882cf99', 'Demo User', 1, '999-999-999', 'demo@demo.nu', 'en_US', 0, 0, 0, 0, 'default', 'Letter', 2, 2, 3, 1, 1, 0, '2007-02-06 19:02:35');
INSERT INTO `0_users` VALUES ('admin', '5f4dcc3b5aa765d61d8327deb882cf99', 'Administrator', 2, '', 'info@frontaccounting.com', 'en_US', 0, 0, 0, 0, 'default', 'Letter', 2, 2, 4, 1, 1, 0, '2007-03-20 10:52:46');

-- 
-- Data in table `wo_issue_items`
-- 

INSERT INTO `0_wo_issue_items` VALUES (1, '102', 1, 10);

-- 
-- Data in table `wo_issues`
-- 

INSERT INTO `0_wo_issues` VALUES (1, 3, '1', '2006-01-20', 'DEF', 1);

-- 
-- Data in table `wo_manufacture`
-- 

INSERT INTO `0_wo_manufacture` VALUES (1, 'ab200', 3, 50, '2007-01-30');
INSERT INTO `0_wo_manufacture` VALUES (2, 'ab201', 5, 20, '2007-01-30');

-- 
-- Data in table `wo_requirements`
-- 

INSERT INTO `0_wo_requirements` VALUES (1, 1, '102', '1', 1, 0, 'DEF', 20);
INSERT INTO `0_wo_requirements` VALUES (2, 1, '103', '1', 1, 0, 'DEF', 20);
INSERT INTO `0_wo_requirements` VALUES (3, 1, '104', '1', 1, 0, 'DEF', 20);
INSERT INTO `0_wo_requirements` VALUES (4, 1, '201', '1', 1, 0, 'DEF', 20);
INSERT INTO `0_wo_requirements` VALUES (5, 2, '102', '1', 1, 0, 'DEF', 5);
INSERT INTO `0_wo_requirements` VALUES (6, 2, '103', '1', 1, 0, 'DEF', 5);
INSERT INTO `0_wo_requirements` VALUES (7, 2, '104', '1', 1, 0, 'DEF', 5);
INSERT INTO `0_wo_requirements` VALUES (8, 2, '201', '1', 1, 0, 'DEF', 5);
INSERT INTO `0_wo_requirements` VALUES (9, 3, '102', '1', 1, 0, 'DEF', 0);
INSERT INTO `0_wo_requirements` VALUES (10, 3, '103', '1', 1, 0, 'DEF', 0);
INSERT INTO `0_wo_requirements` VALUES (11, 3, '104', '1', 1, 0, 'DEF', 0);
INSERT INTO `0_wo_requirements` VALUES (12, 3, '201', '1', 1, 0, 'DEF', 0);
INSERT INTO `0_wo_requirements` VALUES (13, 4, '102', '1', 1, 0, 'DEF', 5);
INSERT INTO `0_wo_requirements` VALUES (14, 4, '103', '1', 1, 0, 'DEF', 5);
INSERT INTO `0_wo_requirements` VALUES (15, 4, '104', '1', 1, 0, 'DEF', 5);
INSERT INTO `0_wo_requirements` VALUES (16, 4, '201', '1', 1, 0, 'DEF', 5);
INSERT INTO `0_wo_requirements` VALUES (17, 5, '102', '1', 1, 0, 'DEF', 0);
INSERT INTO `0_wo_requirements` VALUES (18, 5, '103', '1', 1, 0, 'DEF', 0);
INSERT INTO `0_wo_requirements` VALUES (19, 5, '104', '1', 1, 0, 'DEF', 0);
INSERT INTO `0_wo_requirements` VALUES (20, 5, '201', '1', 1, 0, 'DEF', 0);

-- 
-- Data in table `workcentres`
-- 

INSERT INTO `0_workcentres` VALUES (1, 'work centre', '');

-- 
-- Data in table `workorders`
-- 

INSERT INTO `0_workorders` VALUES (1, '1', 'DEF', 20, '3400', '2006-01-18', 0, '2006-01-18', '2006-01-18', 20, 1, 1, 0);
INSERT INTO `0_workorders` VALUES (2, '2', 'DEF', 5, '3400', '2006-01-18', 0, '2006-01-18', '2006-01-18', 5, 1, 1, 0);
INSERT INTO `0_workorders` VALUES (3, '3', 'DEF', 50, '3400', '2006-01-18', 2, '2006-02-07', '2006-01-20', 50, 1, 1, 0);
INSERT INTO `0_workorders` VALUES (4, '4', 'DEF', 5, '3400', '2007-01-30', 0, '2007-01-30', '2007-01-30', 5, 1, 1, 0);
INSERT INTO `0_workorders` VALUES (5, '5', 'DEF', 20, '3400', '2007-01-30', 2, '2007-02-19', '2007-01-30', 20, 1, 1, 0);
