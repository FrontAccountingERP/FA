-- phpMyAdmin SQL Dump
-- version 2.7.0-pl2
-- http://www.phpmyadmin.net
-- 
-- Host: localhost
-- Created: 09 februar 2007 at 11:03
-- Server Version: 4.1.11
-- PHP-version: 4.4.1
-- 
-- Database: `en_US-new`
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
-- Data in table `bank_accounts`
-- 

INSERT INTO `0_bank_accounts` VALUES ('1700', 0, 'Current account', 'N/A', 'N/A', NULL, 'USD');
INSERT INTO `0_bank_accounts` VALUES ('1705', 0, 'Petty Cash account', 'N/A', 'N/A', NULL, 'USD');

-- 
-- Data in table `bank_trans_types`
-- 

INSERT INTO `0_bank_trans_types` VALUES (1, 'Cash');
INSERT INTO `0_bank_trans_types` VALUES (2, 'Transfer');

-- 
-- Data in table `chart_class`
-- 

INSERT INTO `0_chart_class` VALUES (1, 'Assets', 1);
INSERT INTO `0_chart_class` VALUES (2, 'Liabilities', 1);
INSERT INTO `0_chart_class` VALUES (3, 'Income', 0);
INSERT INTO `0_chart_class` VALUES (4, 'Costs', 0);

-- 
-- Data in table `chart_master`
-- 

INSERT INTO `0_chart_master` VALUES ('3000', NULL, 'Sales', 1, 1);
INSERT INTO `0_chart_master` VALUES ('3010', NULL, 'Sales  - Wholesale', 1, 1);
INSERT INTO `0_chart_master` VALUES ('3020', NULL, 'Sales of Other items', 1, 1);
INSERT INTO `0_chart_master` VALUES ('3400', NULL, 'Difference On Exchange', 1, 0);
INSERT INTO `0_chart_master` VALUES ('5000', NULL, 'Direct Labour', 2, 0);
INSERT INTO `0_chart_master` VALUES ('5050', NULL, 'Direct Labour Recovery', 2, 0);
INSERT INTO `0_chart_master` VALUES ('4200', NULL, 'Material Usage Varaiance', 2, 4);
INSERT INTO `0_chart_master` VALUES ('4210', NULL, 'Consumable Materials', 2, 4);
INSERT INTO `0_chart_master` VALUES ('4220', NULL, 'Purchase price Variance', 2, 0);
INSERT INTO `0_chart_master` VALUES ('4000', NULL, 'Purchases of materials', 2, 4);
INSERT INTO `0_chart_master` VALUES ('4250', NULL, 'Discounts Received', 2, 0);
INSERT INTO `0_chart_master` VALUES ('4260', NULL, 'Exchange Variation', 2, 0);
INSERT INTO `0_chart_master` VALUES ('4300', NULL, 'Freight Inwards', 2, 4);
INSERT INTO `0_chart_master` VALUES ('4010', NULL, 'Cost of Goods Sold - Retail', 2, 4);
INSERT INTO `0_chart_master` VALUES ('6790', NULL, 'Bank Charges', 5, 4);
INSERT INTO `0_chart_master` VALUES ('6800', NULL, 'Entertainments', 5, 4);
INSERT INTO `0_chart_master` VALUES ('6810', NULL, 'Legal Expenses', 5, 4);
INSERT INTO `0_chart_master` VALUES ('6600', NULL, 'Repairs and Maintenance Office', 5, 4);
INSERT INTO `0_chart_master` VALUES ('6730', NULL, 'phone', 5, 4);
INSERT INTO `0_chart_master` VALUES ('8200', NULL, 'Bank Interest', 52, 0);
INSERT INTO `0_chart_master` VALUES ('6840', NULL, 'Credit Control', 5, 0);
INSERT INTO `0_chart_master` VALUES ('7040', NULL, 'Depreciation Office Equipment', 51, 0);
INSERT INTO `0_chart_master` VALUES ('3800', NULL, 'Freight Outwards', 5, 4);
INSERT INTO `0_chart_master` VALUES ('4500', NULL, 'Packaging', 5, 4);
INSERT INTO `0_chart_master` VALUES ('6400', NULL, 'Commissions', 5, 0);
INSERT INTO `0_chart_master` VALUES ('3200', NULL, 'Prompt Payment Discounts', 1, 0);
INSERT INTO `0_chart_master` VALUES ('6700', NULL, 'General Expenses', 5, 4);
INSERT INTO `0_chart_master` VALUES ('5200', NULL, 'Indirect Labour', 2, 0);
INSERT INTO `0_chart_master` VALUES ('5210', NULL, 'Overhead Recovery', 5, 0);
INSERT INTO `0_chart_master` VALUES ('1700', NULL, 'Bank account', 10, 0);
INSERT INTO `0_chart_master` VALUES ('1705', NULL, 'Petty Cash', 10, 0);
INSERT INTO `0_chart_master` VALUES ('1710', NULL, 'Foreign currency account', 10, 0);
INSERT INTO `0_chart_master` VALUES ('1500', NULL, 'Accounts Receivable', 20, 0);
INSERT INTO `0_chart_master` VALUES ('1400', NULL, 'Stocks of Raw Materials', 45, 0);
INSERT INTO `0_chart_master` VALUES ('1410', NULL, 'Stocks of Work In Progress', 45, 0);
INSERT INTO `0_chart_master` VALUES ('1420', NULL, 'Stocks of Finsihed Goods', 45, 0);
INSERT INTO `0_chart_master` VALUES ('1430', NULL, 'Goods Received Clearing account', 30, 0);
INSERT INTO `0_chart_master` VALUES ('2630', NULL, 'Accounts Payable', 30, 0);
INSERT INTO `0_chart_master` VALUES ('2660', NULL, 'VAT out 5', 30, 0);
INSERT INTO `0_chart_master` VALUES ('2662', NULL, 'VAT out 1', 30, 0);
INSERT INTO `0_chart_master` VALUES ('2664', NULL, 'VAT out 25', 30, 0);
INSERT INTO `0_chart_master` VALUES ('2680', NULL, 'VAT In 5', 30, 0);
INSERT INTO `0_chart_master` VALUES ('2682', NULL, 'VAT In 25', 30, 0);
INSERT INTO `0_chart_master` VALUES ('2050', NULL, 'Retained Earnings', 50, 0);
INSERT INTO `0_chart_master` VALUES ('2000', NULL, 'Share Capital', 50, 0);

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
-- Data in table `company`
-- 

INSERT INTO `0_company` VALUES (1, 'Company name', '', '', 1, 1, 'N/A', '', '', '', '', '', 'USD', '1500', '4250', '2630', '1430', '4260', '4220', '2050', '3800', '3000', '3000', '3200', '1420', '4010', '4210', '3000', '1410', '5000', '', '', '', '', '', '', 0, 10, 10, 1000, 20, 20, 30, 1, 0);

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
-- Data in table `locations`
-- 

INSERT INTO `0_locations` VALUES ('DEF', 'Default', 'N/A', '', '', '', '');

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
-- Data in table `sales_types`
-- 

INSERT INTO `0_sales_types` VALUES (1, 'Retail');
INSERT INTO `0_sales_types` VALUES (2, 'Wholesale');

-- 
-- Data in table `salesman`
-- 

INSERT INTO `0_salesman` VALUES (1, 'Sales Person', '', '', '');

-- 
-- Data in table `shippers`
-- 

INSERT INTO `0_shippers` VALUES (1, 'Default', '', '', '');

-- 
-- Data in table `stock_category`
-- 

INSERT INTO `0_stock_category` VALUES (1, 'Components', NULL, NULL, NULL, NULL);
INSERT INTO `0_stock_category` VALUES (2, 'Charges', NULL, NULL, NULL, NULL);
INSERT INTO `0_stock_category` VALUES (3, 'Systems', NULL, NULL, NULL, NULL);
INSERT INTO `0_stock_category` VALUES (4, 'Services', NULL, NULL, NULL, NULL);

-- 
-- Data in table `sys_types`
-- 

INSERT INTO `0_sys_types` VALUES (0, 'Journal - GL', 17, '0');
INSERT INTO `0_sys_types` VALUES (1, 'Payment - GL', 7, '0');
INSERT INTO `0_sys_types` VALUES (2, 'Receipt - GL', 4, '0');
INSERT INTO `0_sys_types` VALUES (4, 'Funds Transfer', 3, '0');
INSERT INTO `0_sys_types` VALUES (10, 'Sales Invoice', 16, '0');
INSERT INTO `0_sys_types` VALUES (11, 'Credit Note', 2, '0');
INSERT INTO `0_sys_types` VALUES (12, 'Receipt', 6, '0');
INSERT INTO `0_sys_types` VALUES (16, 'Location Transfer', 2, '0');
INSERT INTO `0_sys_types` VALUES (17, 'Inventory Adjustment', 2, '0');
INSERT INTO `0_sys_types` VALUES (18, 'Purchase Order', 1, '0');
INSERT INTO `0_sys_types` VALUES (20, 'Supplier Invoice', 6, '0');
INSERT INTO `0_sys_types` VALUES (21, 'Supplier Credit Note', 1, '0');
INSERT INTO `0_sys_types` VALUES (22, 'Supplier Payment', 3, '0');
INSERT INTO `0_sys_types` VALUES (25, 'Purchase Order Delivery', 1, '0');
INSERT INTO `0_sys_types` VALUES (26, 'Work Order', 1, '0');
INSERT INTO `0_sys_types` VALUES (28, 'Work Order Issue', 1, '0');
INSERT INTO `0_sys_types` VALUES (29, 'Work Order Production', 1, '0');
INSERT INTO `0_sys_types` VALUES (30, 'Sales Order', 1, '');
INSERT INTO `0_sys_types` VALUES (35, 'Cost Update', 1, '');
INSERT INTO `0_sys_types` VALUES (40, 'Dimension', 1, '3');

-- 
-- Data in table `users`
-- 

INSERT INTO `0_users` VALUES ('demouser', '5f4dcc3b5aa765d61d8327deb882cf99', 'Demo User', 1, '999-999-999', 'demo@demo.nu', 'en_US', 0, 0, 0, 0, 'default', 'Letter', 2, 2, 3, 1, 1, 0, '2007-02-06 19:02:35');
INSERT INTO `0_users` VALUES ('admin', '5f4dcc3b5aa765d61d8327deb882cf99', 'Administrator', 2, '', 'info@frontaccounting.com', 'en_US', 0, 0, 0, 0, 'default', 'Letter', 2, 2, 4, 1, 1, 0, '2007-03-20 10:52:46');

