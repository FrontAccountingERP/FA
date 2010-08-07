ALTER TABLE `0_supp_trans` ADD COLUMN `tax_included` tinyint(1) NOT NULL default '0';
ALTER TABLE `0_purch_orders` ADD COLUMN `tax_included` tinyint(1) NOT NULL default '0';
UPDATE `crm_persons` SET `lang`='C' WHERE `lang`='en_GB';
UPDATE `crm_users` SET `language`='C' WHERE `language`='en_GB';
