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
    //--------------------------------------------------

	// User configurable variables
	//---------------------------------------------------

	/*Show debug messages returned from an error on the page.
	Debugging info level also determined by settings in PHP.ini
	if $debug=1 show debugging info, dont show if $debug=0 */

if (!isset($path_to_root) || isset($_GET['path_to_root']) || isset($_POST['path_to_root']))
	die("Restricted access");
	// Log file for error/warning messages. Should be set to any location
	// writable by www server. When set to empty string logging is switched off. 
	// Special value 'syslog' can be used for system logger usage (see php manual).
	$error_logfile = '';
	//$error_logfile = dirname(__FILE__).'/tmp/errors.log';
	$debug 			= 1;
	$show_sql 		= 0;
	$go_debug 		= 1;
	$pdf_debug 		= 0;
	// set $sql_trail to 1 only if you want to perform bugtracking sql trail
	// Warning: this produces huge amount of data in sql_trail table.
	// Don't forget switch the option off and flush the table manually after 
	// trail, or your future backup files are overloaded with unneeded data.
	//
	$sql_trail 		= 0; // save all sql queries in sql_trail
	$select_trail 	= 0; // track also SELECT queries
	if ($go_debug == 1)
	{
		error_reporting(E_ALL);
		ini_set("display_errors", "On");
	}
	else
	{
		error_reporting(E_USER_WARNING|E_USER_ERROR|E_USER_NOTICE);
		// ini_alter("error_reporting","E_COMPILE_ERROR|E_ERROR|E_CORE_ERROR|E_PARSE");
		ini_set("display_errors", "On");
	}

	if($error_logfile != '') {
		ini_set("error_log", $error_logfile);
		ini_set("ignore_repeated_errors", "On");
		ini_set("log_errors", "On");
	}		
	// Main Title
	$app_title = "FrontAccounting";
	// application version
	$version 		= "2.2m4 CVS";

	// Build for development purposes
	$build_version 	= date("d.m.Y", filemtime("$path_to_root/CHANGELOG.txt"));

	// Powered by
	$power_by 		= "FrontAccounting";
	$power_url 		= "http://frontaccounting.net";

	/* use popup windows for views */
	$use_popup_windows = 1;

	/* use date picker for all date fields */
	$use_date_picker = 1;

	/* use Audit Trails in GL */
	$use_audit_trail = 0;

	/* use old style convert (income and expense in BS, PL) */
	$use_oldstyle_convert = 0;

 	/* Integrated base Wiki Help URL or null if not used */
	//$help_base_url = $path_to_root.'/modules/wiki/index.php?n='._('Help').'.';
	$help_base_url = null;

	/* per user data/cache directory */
	$comp_path = $path_to_root.'/company';

	/* allow alpha characters in accounts. 0 = numeric, 1 = alpha numeric, 2 = uppercase alpha numeric */
	$accounts_alpha = 0;

	/* Date systems. 0 = traditional, 1 = Jalali used by Iran, nabour countries, Afghanistan and some other Central Asian nations,
	2 = Islamic used by other arabic nations */
	$date_system = 0;

	/* email stock location if order below reorder-level */
	$loc_notification = 0;

	/* print_invoice_no. 0 = print reference number, 1 = print invoice number */
	$print_invoice_no = 0;

	$dateformats 	= array("MMDDYYYY", "DDMMYYYY", "YYYYMMDD");
	$dateseps 		= array("/", ".", "-", " ");
	$thoseps 		= array(",", ".", " ");
	$decseps 		= array(".", ",");

	$pagesizes 		= array("Letter", "A4"); // default PDF pagesize

	/* Default border and spacing for tables */
	/* Should be moved to CSS */

	$table_style 	= "cellpadding=3 border=1 bordercolor='#8cacbb' style='border-collapse: collapse'";
	$table_style2 	= "cellpadding=3 border=1 bordercolor='#cccccc' style='border-collapse: collapse'";

	/* Accounts Payable */
	/* System check to see if quantity charged on purchase invoices exceeds the quantity received.
	If this parameter is checked the proportion by which the purchase invoice is an overcharge
	referred to before reporting an error */

	$check_qty_charged_vs_del_qty = true;

	/* System check to see if price charged on purchase invoices exceeds the purchase order price.
	If this parameter is checked the proportion by which the purchase invoice is an overcharge
	referred to before reporting an error */

	$check_price_charged_vs_order_price = True;

	$config_allocation_settled_allowance = 0.005;

	// Internal configurable variables
	//-----------------------------------------------------------------------------------

	/* Whether to display the demo login and password or not */

	$allow_demo_mode = false;

	/* for uploaded item pictures */
	$pic_width 		= 80;
	$pic_height 	= 50;
	$max_image_size = 500;

	/* skin for Business Graphics, 1, 2 or 3 */
	$graph_skin 	= 1;

	/*Security Group definitions - Depending on the AccessLevel of the user defined in the user set up
	the areas of functionality accessible can be modified.
	Each AccessLevel is associated with an array containing the security categories that the user is entitled to access
	Each script has a particular security category associated with it.
	If the security setting of the page is contained in the security group as determined by the access level then the user will be allowed access.
	Each page has a $page_security = x; variable
	This value is compared to contents of the array applicable which is based on the access level of the user.
	Access authorisation is checked in session.inc. If you wish to add more security groups
	with then you must add a new SecurityHeading to the security_headings array
	and a new array of Security categories to the Security Groups _at_the_end_ of the array
	This mechanism allows more fine grained control of access
	security_groups is an array of arrays
	The index is the order in which the array of allowed pages is defined new ones can be defined at will
	or by changing the numbers in each array the security access can be tailored. These numbers need to read
	in conjunction with the Page Security index
	Special case is security level 20 which is reserved for admins of first
	registered company (site admins). All potentially dangerous for whole FA
	site operations like installing addon modules require access level 20.
	*/

	$security_headings = array(
			_("Inquiries"),
			_("Accountant"),
			_("System Administrator"),
	);

	$security_groups = array(
			array(1,2),
			array(1,2,3,4,5,6,7,8,9,10,11,12,13,14,16),
			array(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,20),
	);

	//MySQL Backup and Restore Settings

if(isset($_SESSION["wa_current_user"])) {
	define("BACKUP_PATH", $comp_path.'/'.user_company()."/backup/");
}
	// static js files path
	$js_path = $path_to_root.'/js/';
	// standard external js scripts included in all files
	$js_static = array('JsHttpRequest.js', 'behaviour.js', 'utils.js', 'inserts.js');
	// additional js source included in header
	$js_lib = $js_userlib = array();

if (!defined('ICON_EDIT'))
{
	define("ICON_EDIT", "edit.gif");	
	define("ICON_DELETE", "delete.gif");	
	define("ICON_ADD", "ok.gif");	
	define("ICON_UPDATE", "ok.gif");	
	define("ICON_OK", "ok.gif");	
	define("ICON_CANCEL", "cancel.png");	
	define("ICON_GL", "gl.png");	
	define("ICON_PRINT", "print.png");	
	define("ICON_PDF", "pdf.gif");	
	define("ICON_DOC", "invoice.gif");	
	define("ICON_CREDIT", "credit.gif");	
	define("ICON_RECEIVE", "receive.gif");	
	define("ICON_DOWN", "download.gif");	
	define("ICON_MONEY", "money.png");	
	define("ICON_REMOVE", "remove.png");	
	define("ICON_REPORT", "report.png");	
	define("ICON_VIEW", "view.gif");	
 	define("ICON_SUBMIT", "ok.gif");
 	define("ICON_ESCAPE", "escape.png");	
}
?>