-- phpMyAdmin SQL Dump
-- version 2.9.0.2
-- http://www.phpmyadmin.net
-- 
-- Host: localhost
-- Generation Time: Mar 20, 2007 at 11:10 AM
-- Server version: 4.1.21
-- PHP Version: 4.4.2
-- 
-- Database: `frontacc_frontacc`
-- 

-- --------------------------------------------------------

-- 
-- ALTER TABLE for `0_company`
-- 

ALTER TABLE `0_supp_invoice_items` CHANGE `gl_code` `gl_code` VARCHAR(11) NOT NULL DEFAULT '0';

ALTER TABLE `0_sales_order_details` DROP PRIMARY KEY;
ALTER TABLE `0_sales_order_details` ADD `id` INTEGER(11) NOT NULL AUTO_INCREMENT FIRST, ADD PRIMARY KEY (`id`);

ALTER TABLE `0_company` ADD `no_item_list` TINYINT(1) NOT NULL DEFAULT '0' AFTER `f_year`;
ALTER TABLE `0_company` ADD `no_customer_list` TINYINT(1) NOT NULL DEFAULT '0' AFTER `no_item_list`;
ALTER TABLE `0_company` ADD `no_supplier_list` TINYINT(1) NOT NULL DEFAULT '0' AFTER `no_customer_list`;
  
ALTER TABLE `0_salesman` ADD `provision` DOUBLE NOT NULL DEFAULT '0' AFTER `salesman_email`;
ALTER TABLE `0_salesman` ADD `break_pt` DOUBLE NOT NULL DEFAULT '0' AFTER `provision`;
ALTER TABLE `0_salesman` ADD `provision2` DOUBLE NOT NULL DEFAULT '0' AFTER `break_pt`;

