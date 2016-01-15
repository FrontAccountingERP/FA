<?php

/* List of installed additional extensions. If extensions are added to the list manually
	make sure they have unique and so far never used extension_ids as a keys,
	and $next_extension_id is also updated. More about format of this file yo will find in 
	FA extension system documentation.
*/

$next_extension_id = 1; // unique id for next installed extension

$installed_extensions = array (
  0 => 
  array (
    'name' => 'Australian COA for a service company (version 2).',
    'package' => 'chart_en_AU-service',
    'version' => '2.3.0-3',
    'type' => 'chart',
    'active' => false,
    'path' => 'sql',
    'sql' => 'en_AU-service-v2.sql',
  ),
);
?>