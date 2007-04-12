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

