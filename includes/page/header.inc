<?php

function page_header($title, $no_menu=false, $is_index=false, $onload="", $js="")
{
	// titles and screen header
	global $db_connections, $path_to_root, $def_app;
	$theme = user_theme();
	$local_path_to_root = $path_to_root;

	if (!isset($no_menu)) 
	{
		$no_menu = false;
	}
	if (!isset($_SESSION["sel_app"]))
		session_register("sel_app");	
	if (isset($_SESSION["App"]) && is_object($_SESSION["App"]) && isset($_SESSION["App"]->selected_application) && 
		$_SESSION["App"]->selected_application != "")
	 	$sel_app = $_SESSION["App"]->selected_application;
	elseif (isset($_SESSION["sel_app"]) && $_SESSION["sel_app"] != "")
		$sel_app = $_SESSION["sel_app"];
	else
		$sel_app = $def_app;
	$_SESSION["sel_app"] = $sel_app;

    echo "<html dir='" . $_SESSION['language']->dir . "' >";
    echo "<head><title>$title</title>";
    $encoding = $_SESSION['language']->encoding;
	echo "<meta http-equiv='Content-type' content='text/html'; charset='$encoding'>";
	echo "<link href='$local_path_to_root/themes/$theme/default.css' rel='stylesheet' type='text/css' /> \n";
	echo "<link href='$local_path_to_root/" . $_SESSION['language']->get_stylesheet() . "' rel='stylesheet' type='text/css' /> \n";
	if ($js != "")
		echo $js;
	echo "</head> \n";
	if ($onload == "")
		echo "<body>";
	else
		echo "<body onload=\"$onload\">";
    echo "<table class='callout_main' border='0' cellpadding='0' cellspacing='0'>\n";
	echo "<tr>\n";
    echo "<td colspan='2' rowspan='2'>\n";

	echo "<table class='main_page' border='0' cellpadding='0' cellspacing='0'>\n";
	echo "<tr>\n";
	echo "<td>\n";
    echo "<table width='100%' border='0' cellpadding='0' cellspacing='0'>\n";
	echo "<tr>\n";
    echo "<td class='quick_menu'>\n";
	if (!$no_menu) 
	{
		echo "<table cellpadding=0 cellspacing=0 width='100%'><tr><td>";
		echo "<div class=tabs>";
		echo "<a ".($sel_app == "orders" ? "class='selected' " : "")."href='$local_path_to_root/index.php?application=orders". SID ."'>" . _("Sales") . "</a>";
		echo "<a ".($sel_app == "AP" ? "class='selected' " : "")."href='$local_path_to_root/index.php?application=AP". SID ."'>" . _("Purchases") . "</a>";
		echo "<a ".($sel_app == "stock" ? "class='selected' " : "")."href='$local_path_to_root/index.php?application=stock". SID ."'>" . _("Items and Inventory") . "</a>";
		echo "<a ".($sel_app == "manuf" ? "class='selected' " : "")."href='$local_path_to_root/index.php?application=manuf". SID ."'>" . _("Manufacturing") . "</a>";
		echo "<a ".($sel_app == "proj" ? "class='selected' " : "")."href='$local_path_to_root/index.php?application=proj". SID ."'>" . _("Dimensions") . "</a>";
		echo "<a ".($sel_app == "GL" ? "class='selected' " : "")."href='$local_path_to_root/index.php?application=GL". SID ."'>" . _("Banking and General Ledger") . "</a>";
		echo "<a ".($sel_app == "system" ? "class='selected' " : "")."href='$local_path_to_root/index.php?application=system". SID ."'>" . _("Setup") . "</a>";
		echo "</div>";

		echo "</td></tr></table>";

		echo "<table class=logoutBar>";
		echo "<tr><td class=headingtext3>" . $db_connections[$_SESSION["wa_current_user"]->company]["name"] . " | " . $_SERVER['SERVER_NAME'] . " | " . $_SESSION["wa_current_user"]->name . "</td>";
		echo "<td class=logoutBarRight><a href='$local_path_to_root/access/logout.php?'>" . _("Logout") . "</a>";
		echo "</td></tr></table>";
	}

	echo "</td></tr></table>";

	if ($title && !$no_menu && !$is_index) 
	{
		echo "<center><table width='100%'><tr><td width='100%' class='titletext'>$title</td></tr></table></center>";
	}

	if (!$is_index)
		echo "<br>";
}

?>