ALTER TABLE `0_locations` ADD COLUMN `fixed_asset` tinyint(1) NOT NULL DEFAULT '0' after `contact`;

DROP TABLE IF EXISTS `0_stock_fa_class`;
CREATE TABLE `0_stock_fa_class` (
  `fa_class_id` varchar(20) NOT NULL DEFAULT '',
  `parent_id` varchar(20) NOT NULL DEFAULT '',
  `description` varchar(200) NOT NULL DEFAULT '',
  `long_description` tinytext NOT NULL,
  `depreciation_rate` double NOT NULL DEFAULT '0',
  `inactive` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`fa_class_id`)
) ENGINE=InnoDB;

ALTER TABLE `0_stock_master` ADD COLUMN `depreciation_method` char(1) NOT NULL DEFAULT 'S' AFTER `editable`;
ALTER TABLE `0_stock_master` ADD COLUMN `depreciation_rate` double NOT NULL DEFAULT '0' AFTER `depreciation_method`;
ALTER TABLE `0_stock_master` ADD COLUMN `depreciation_factor` double NOT NULL DEFAULT '0' AFTER `depreciation_rate`;
ALTER TABLE `0_stock_master` ADD COLUMN `depreciation_start` date NOT NULL DEFAULT '0000-00-00' AFTER `depreciation_factor`;
ALTER TABLE `0_stock_master` ADD COLUMN `depreciation_date` date NOT NULL DEFAULT '0000-00-00' AFTER `depreciation_start`;
ALTER TABLE `0_stock_master` ADD COLUMN `fa_class_id` varchar(20) NOT NULL DEFAULT '' AFTER `depreciation_date`;
ALTER TABLE `0_stock_master` CHANGE `actual_cost` `purchase_cost` double NOT NULL default 0;

INSERT INTO `0_sys_prefs` VALUES ('default_loss_on_asset_disposal_act', 'glsetup.items', 'varchar', '15', '5660');
INSERT INTO `0_sys_prefs` VALUES ('depreciation_period', 'glsetup.company', 'tinyint', '1', '1');

INSERT INTO `0_sys_prefs` VALUES ('use_manufacturing','setup.company', 'tinyint', 1, '1');
INSERT INTO `0_sys_prefs` VALUES ('use_fixed_assets','setup.company', 'tinyint', 1, '1');
