ALTER TABLE `0_suppliers` ADD COLUMN  `tax_algorithm` tinyint(1) NOT NULL default '1' AFTER `tax_included`;
ALTER TABLE `0_supp_trans` ADD COLUMN `tax_algorithm` tinyint(1) NULL default '1' AFTER `tax_included`;
INSERT INTO `0_sys_prefs` VALUES('tax_algorithm','glsetup.customer', 'tinyint', 1, '1');
INSERT INTO `0_sys_prefs` VALUES('gl_closing_date','setup.closing_date', 'date', 8, '');
# Fix eventual invalid date/year in audit records
UPDATE `0_audit_trail` audit 
		LEFT JOIN `0_gl_trans` gl ON  gl.`type`=audit.`type` AND gl.type_no=audit.trans_no
		LEFT JOIN `0_fiscal_year` year ON year.begin<=gl.tran_date AND year.end>=gl.tran_date
		SET audit.gl_date=gl.tran_date, audit.fiscal_year=year.id
		WHERE NOT ISNULL(gl.`type`);

DROP TABLE IF EXISTS `0_wo_costing`;

CREATE TABLE `0_wo_costing` (
  `id` int(11) NOT NULL auto_increment,
  `workorder_id` int(11) NOT NULL default '0',
  `cost_type` 	tinyint(1) NOT NULL default '0',
  `trans_type` int(11) NOT NULL default '0',
  `trans_no` int(11) NOT NULL default '0',
  `factor` double NOT NULL default '1',
  PRIMARY KEY  (`id`)
) TYPE=InnoDB;

UPDATE `0_gl_trans` gl
		LEFT JOIN `0_cust_branch` br ON br.receivables_account=gl.account AND br.debtor_no=gl.person_id AND gl.person_type_id=2
		LEFT JOIN `0_suppliers` sup ON sup.payable_account=gl.account AND sup.supplier_id=gl.person_id AND gl.person_type_id=3
 SET `person_id` = IF(br.receivables_account, br.debtor_no, IF(sup.payable_account, sup.supplier_id, NULL)), 
 	`person_type_id` = IF(br.receivables_account, 2, IF(sup.payable_account, 3, NULL));

