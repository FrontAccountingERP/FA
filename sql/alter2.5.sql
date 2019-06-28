# reliable cost change log in stock_moves
ALTER TABLE `0_stock_moves` CHANGE COLUMN `standard_cost` `unit_cost` double NOT NULL DEFAULT '0';
ALTER TABLE `0_debtor_trans_details` CHANGE COLUMN `standard_cost` `unit_cost` double NOT NULL DEFAULT '0';

