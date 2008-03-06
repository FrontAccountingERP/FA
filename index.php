<?php
	$path_to_root=".";
	
	$page_security = 1;
	ini_set('xdebug.auto_trace',1);
	include_once("frontaccounting.php");
	include_once("includes/session.inc");
	if (!isset($_SESSION["App"]))
		$_SESSION["App"] = new front_accounting();
	$app = &$_SESSION["App"];
	if (isset($_GET['application']))
		$app->selected_application = $_GET['application'];
	$app->display();
?>