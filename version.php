<?php
//==========================================================================================
//
// Settings in this file can be automatically updated at any time during software update.
//

// Internal data-source version compatibility check. Do not change.
$core_version = "2.3rc";

// application version - can be set also in config.php
if (!isset($version))
	$version 		= "2.3RC1";

//======================================================================
// Extension packages repository settings 
//
// Default authorization data. Can be set also in config.php

if (!isset($repo_auth))
	$repo_auth = array(
		 'login' => 'anonymous',
		 'pass' => 'password',
);

// Repository branch for current sources version
$FA_repo_version = '2.3';

// Extension packages repository url
$repository = 'http://'.$repo_auth['login'].':'.$repo_auth['pass'].'@'.'repo.frontaccounting.eu'
	.'/index.php?path=';
