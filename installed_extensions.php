<?php

/* List of installed additional modules and plugins. If adding extensions manually 
	to the list make sure they have unique, so far not used extension_ids as a keys,
	and $next_extension_id is also updated.
	
	'name' - name for identification purposes;
	'type' - type of extension: 'module' or 'plugin'
	'path' - FA root based installation path
	'filename' - name of module menu file, or plugin filename; related to path.
	'tab' - index of the module tab (new for module, or one of standard module names for plugin);
	'title' - is the menu text (for plugin) or new tab name
	'active' - current status of extension
	'acc_file' - (optional) file name with $security_areas/$security_sections extensions; 
		related to 'path'
	'access' - security area code in string form
*/

$next_extension_id = 1; // unique id for next installed extension

$installed_extensions = array (
	);
?>