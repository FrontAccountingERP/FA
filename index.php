<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU Affero General Public License,
	AGPL, as published by the Free Software Foundation, either version 
	3 of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/agpl-3.0.html>.
***********************************************************************/
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
	context_reset();
?>