<?php
//==========================================================================================
//
// Settings in this file can be automatically updated at any time during software update.
//

// Internal database version compatibility check. Do not change.
$db_version = "2.3rc";

// application version - can be overriden in config.php
if (!isset($version))
	$version 		= "2.3.15";

//======================================================================
// Extension packages repository settings 
//
// Extensions repository. Can be overriden in config.php

if (!isset($repo_auth))
	$repo_auth = array(
		 'login' => 'anonymous',
		 'pass' => 'password',
		 'host' => 'repo.frontaccounting.eu', // repo server address
		 'branch' => '2.3'	// Repository branch for current sources version
);
