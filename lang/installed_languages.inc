<?php

/* How to make new entries here

-- if adding languages at the beginning of the list, make sure it's index is set to 0 (it has ' 0 => ')
-- 'code' should match the name of the directory for the language under \lang
-- 'name' is the name that will be displayed in the language selection list (in Users and Display Setup)
-- 'rtl' only needs to be set for right-to-left languages like Arabic and Hebrew

*/


$installed_languages = array (
	0 => array ('code' => 'en_GB', 'name' => 'English', 'encoding' => 'iso-8859-1'),
		array ('code' => 'en_US', 'name' => 'English (US)', 'encoding' => 'iso-8859-1'),
	);

$dflt_lang = 'en_GB';
?>