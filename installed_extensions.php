<?php

/* List of installed additional modules and plugins. If adding extensions at the beginning 
	of the list, make sure it's index is set to 0 (it has ' 0 => ');
	
	'name' - name for identification purposes;
	'type' - type of extension: 'module' or 'plugin'
	'path' - FA root based installation path
	'filename' - name of module menu file, or plugin filename; related to path.
	'tab' - index of the module tab (new for module, or one of standard module names for plugin);
	'title' - is the menu text (for plugin) or new tab name
	'active' - current status of extension
	'acc_file' - (optional) file name with $security_areas/$security_sections extensions; 
		related to 'path'.
*/

$installed_extensions = array (
	);
?>