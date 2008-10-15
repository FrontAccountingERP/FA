<?php
/*
	Print request redirector. This file is fired via print link or 
	print button in reporting module. 
*/
$path_to_root = "../";
$page_security = 2;	// this level is later overriden in rep file
include_once($path_to_root . "includes/session.inc");

if (!isset($_POST['REP_ID'])) {	// print link clicked
	$def_pars = array(0, 0, '', '', 0, '', '', 0); //default values
	$rep = $_POST['REP_ID'] = $_GET['REP_ID'];
	for($i=0; $i<8; $i++) {
		$_POST['PARAM_'.$i] = isset($_GET['PARAM_'.$i]) 
			? $_GET['PARAM_'.$i] : $def_pars[$i];
	}
}
		$rep = $_POST['REP_ID'];
	    $rep_file = $comp_path.'/'.user_company().
		     "/reporting/rep$rep.php";
		if (!file_exists($rep_file)) {
		    $rep_file = $path_to_root ."/reporting/rep$rep.php";
		}
	require($rep_file);
	exit();

?>