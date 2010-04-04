<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU General Public License, GPL, 
	as published by the Free Software Foundation, either version 3 
	of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/gpl-3.0.html>.
***********************************************************************/
/*
	Print request redirector. This file is fired via print link or 
	print button in reporting module. 
*/
$path_to_root = "..";
$page_security = 'SA_OPEN';	// this level is later overriden in rep file
include_once($path_to_root . "/includes/session.inc");

if (isset($_GET['xls']))
{
	$filename = $_GET['filename'];
	$unique_name = $_GET['unique'];
	$path =  company_path(). '/pdf_files/';
	header("Content-type: application/vnd.ms-excel");
	header("Content-Disposition: attachment; filename=$filename" );
	header("Expires: 0");
	header("Cache-Control: must-revalidate, post-check=0,pre-check=0");
	header("Pragma: public");
	echo file_get_contents($path.$unique_name);
	exit();
}
elseif (isset($_GET['xml']))
{
	$filename = $_GET['filename'];
	$unique_name = $_GET['unique'];
	$path =  company_path(). '/pdf_files/';
	header("content-type: text/xml");
	header("Content-Disposition: attachment; filename=$filename");
	header("Expires: 0");
	header("Cache-Control: must-revalidate, post-check=0,pre-check=0");
	header("Pragma: public");
	echo file_get_contents($path.$unique_name);
	exit();
}
	
if (!isset($_POST['REP_ID'])) {	// print link clicked
	$def_pars = array(0, 0, '', '', 0, '', '', 0); //default values
	$rep = $_POST['REP_ID'] = $_GET['REP_ID'];
	for($i=0; $i<8; $i++) {
		$_POST['PARAM_'.$i] = isset($_GET['PARAM_'.$i]) 
			? $_GET['PARAM_'.$i] : $def_pars[$i];
	}
}

$rep = $_POST['REP_ID'];

$rep_file = find_custom_file("/reporting/rep$rep.php");

if ($rep_file) {
	chdir(dirname($rep_file));
	require(basename($rep_file));
} else
	display_error("Cannot find report file '$rep'");
exit();

?>