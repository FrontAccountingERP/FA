ALTER TABLE 0_comments ADD KEY type_and_id (`type`, `id`);
ALTER TABLE 0_quick_entries ADD COLUMN `bal_type` TINYINT(1) NOT NULL default '0'; 
