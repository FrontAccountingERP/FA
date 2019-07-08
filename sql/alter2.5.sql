# reliable cost change log in stock_moves
ALTER TABLE `0_stock_moves` CHANGE COLUMN `standard_cost` `unit_cost` double NOT NULL DEFAULT '0';
ALTER TABLE `0_debtor_trans_details` CHANGE COLUMN `standard_cost` `unit_cost` double NOT NULL DEFAULT '0';

# naming cleanups
ALTER TABLE `0_purch_orders` CHANGE COLUMN `requisition_no` `supp_reference` tinytext;

# cleanups in work orders
ALTER TABLE  `0_workorders` DROP INDEX `wo_ref`;
ALTER TABLE  `0_workorders` ADD KEY `wo_ref` (`wo_ref`);
ALTER TABLE  `0_workorders` DROP COLUMN `additional_costs`;