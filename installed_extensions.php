<?php

/* How to make new entries here

-- if adding extensions at the beginning of the list, make sure it's index is set to 0 (it has ' 0 => ')
-- 'app_file' is the application file name to be put into folder applications
-- 'name' is the name of the extension module. Will become the index of the application
-- 'title' is the Menu Title
-- 'folder' is the folder where the extension files exist
-- 'acc_file' is path inside extension folder to optional file with $security_areas/$security_sections extensions 
*/

$installed_extensions = array ();

// example
/*
$installed_extensions = array (
	0 => array ('app_file' => 'organizer.php', 'name' => 'organizer', 'title' => 'Organizer', 'folder' => 'organizer',
	'acc_file'=>'')),
	array ('app_file' => 'payroll.php', 'name' => 'payroll', 'title' => 'Payroll', 'folder' => 'payroll',
		'acc_file'=>'includes/access_exts.inc'));
*/	
?>