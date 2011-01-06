ALTER TABLE `0_suppliers` ADD COLUMN  `tax_algorithm` tinyint(1) NOT NULL default '1' AFTER `tax_included`;
ALTER TABLE `0_supp_trans` ADD COLUMN `tax_algorithm` tinyint(1) NULL default '1' AFTER `tax_included`;
INSERT INTO `0_sys_prefs` VALUES('tax_algorithm','glsetup.customer', 'tinyint', 1, '1');

