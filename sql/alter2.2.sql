ALTER TABLE `0_company` DROP COLUMN `custom1_name`;
ALTER TABLE `0_company` DROP COLUMN `custom2_name`;
ALTER TABLE `0_company` DROP COLUMN `custom3_name`;
ALTER TABLE `0_company` DROP COLUMN `custom1_value`;
ALTER TABLE `0_company` DROP COLUMN `custom2_value`;
ALTER TABLE `0_company` DROP COLUMN `custom3_value`;

ALTER TABLE `0_company` ADD COLUMN `default_delivery_required` SMALLINT(6) DEFAULT '1';
