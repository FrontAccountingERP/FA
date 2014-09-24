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

	if (!ini_get('date.timezone'))
       ini_set('date.timezone', 'Europe/Berlin');

	// Log file for error/warning messages. Should be set to any location
	// writable by www server. When set to empty string logging is switched off. 
	// Special value 'syslog' can be used for system logger usage (see php manual).
	//$error_logfile = '';
	$error_logfile = dirname(__FILE__).'/tmp/errors.log';
	$debug 			= 1;	// show sql on database errors

	$show_sql 		= 0;	// show all sql queries in page footer for debugging purposes
	$go_debug 		= 0;	// set to 1 for basic debugging, or 2 to see also backtrace after failure.
	$pdf_debug 		= 0;	// display pdf source instead reports for debugging when $go_debug!=0
	// set $sql_trail to 1 only if you want to perform bugtracking sql trail
	// Warning: this produces huge amount of data in sql_trail table.
	// Don't forget switch the option off and flush the table manually after 
	// trail, or your future backup files are overloaded with unneeded data.
	//
	$sql_trail 		= 0; // save all sql queries in sql_trail
	$select_trail 	= 0; // track also SELECT queries
	if ($go_debug > 0)
	{
		error_reporting(-1);
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

	// Build for development purposes
	$build_version 	= date("d.m.Y", filemtime("$path_to_root/CHANGELOG.txt"));

	// Powered by
	$power_by 		= "FrontAccounting";
	$power_url 		= "http://frontaccounting.com";

	/* No check on edit conflicts. Maybe needed to be set to 1 in certains Windows Servers */
	$no_check_edit_conflicts = 0;
	
	/* Do not print zero lines amount of 0.00 in Sales Documents if service item. 1 = do not */
	$no_zero_lines_amount = 1;

	/* Use icon for editkey (=true) right of combobox. 1 = use, 0 = do not use */
	$use_icon_for_editkey = 0;

	/* Creates automatic a default branch with contact. Value 0 do not create auto branch */
	$auto_create_branch = 1;

	/* Save Report selections (a value > 0 means days to save. 0 = no save) */
	$save_report_selections = 0;

	/* use popup windows for views */
	$use_popup_windows = 1;

	/* use date picker for all date fields */
	$use_date_picker = 1;

	/* use Audit Trails in GL */
	/* This variable is deprecated. Setting this to 1, will stamp the user name in the memo fields in GL */
	/* This has been superseded with built in Audit Trail */
	$use_audit_trail = 0;

	/* $show_voiced_gl_trans = 0, setting this to 1 will show the voided gl trans */
	$show_voided_gl_trans = 0;
	
	/* use old style convert (income and expense in BS, PL) */
	$use_oldstyle_convert = 0;

	/* show users online discretely in the footer */
	$show_users_online = 0;

	/* show item codes on purchase order */
	$show_po_item_codes = 0;

	/* default print destination. 0 = PDF/Printer, 1 = Excel */
	$def_print_destination = 0;

	/* default print orientation. 0 = Portrait, 1 = Landscape */
	$def_print_orientation = 0;

 	// Wiki context help configuration
 	// If your help wiki use translated page titles uncomment next line
 	// $old_style_help = 1; // this setting is depreciated and subject to removal in next FA versions
	$old_style_help = 0;
	// 	locally installed wiki module
	// $help_base_url = $path_to_root.'/modules/wiki/index.php?n='._('Help').'.';
	// 	context help feed from frontaccounting.com
	// $help_base_url = 'http://frontaccounting.com/fawiki/index.php?n=Help.';
	// 	not used
	$help_base_url = null;

	/* per user data/cache directory */
	$comp_path = $path_to_root.'/company';

	/* allow alpha characters in accounts. 0 = numeric, 1 = alpha numeric, 2 = uppercase alpha numeric */
	$accounts_alpha = 0;

	/* Date systems. 0 = traditional, 1 = Jalali used by Iran, nabour countries, Afghanistan and some other Central Asian nations,
	2 = Islamic used by other arabic nations. 3 = traditional, but where non-workday is Friday and start of week is Saturday */
	$date_system = 0;

	/* email stock location if order below reorder-level */
	/* Remember to set an email on the Location(s). */
	$loc_notification = 0;

	/* print_invoice_no. 0 = print reference number, 1 = print invoice number */
	$print_invoice_no = 0;

	/* 1 = print Subtotal tax excluded, tax and Total tax included */
	$alternative_tax_include_on_docs = 0;

	/* suppress tax rates on documents. 0 = no, 1 = yes. */
	$suppress_tax_rates = 0;
	
	$dateformats 	= array("MMDDYYYY", "DDMMYYYY", "YYYYMMDD","MmmDDYYYY", "DDMmmYYYY", "YYYYMmmDD");
	$dateseps 		= array("/", ".", "-", " ");
	$thoseps 		= array(",", ".", " ");
	$decseps 		= array(".", ",");

	/* default dateformats and dateseps indexes used before user login */
	$dflt_date_fmt = 0;
	$dflt_date_sep = 0;

	/* default PDF pagesize taken from /reporting/includes/tcpdf.php */
	$pagesizes 		= array("Letter", "A4");

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

	/* Show average costed values instead of fixed standard cost in report, Inventory Valuation Report */
	$use_costed_values = 0;	
	
	/* Allow negative prices for dummy/service items. To be moved to GL db settings */
	$allow_negative_prices = 1;

	/* Show menu category icons in core themes */
	$show_menu_category_icons = 0;
	
	// Internal configurable variables
	//-----------------------------------------------------------------------------------

	/* Whether to display the demo login and password or not */

	$allow_demo_mode = false;

    /* Whether to allow sending new password by e-mail */
    $allow_password_reset = false;

    /* Print Item Images on Sales Quotations. Set to 1 if so. */
    $print_item_images_on_quote = 0;

	/* for uploaded item pictures */
	$pic_width 		= 80;
	$pic_height 	= 50;
	$max_image_size = 500;

	/* skin for Business Graphics. 1 = Office, 2 = Matrix, or 3 = Spring. 
	   Pallete skin attributes set in reporting/includes/class.graphic.inc */
	$graph_skin 	= 1;

	/* UTF-8 font for Business Graphics. Copy it to /reporting/fonts/ folder. */
	$UTF8_fontfile	= "FreeSans.ttf";

/*	
	Before upgrade from pre-2.2 FA you have to move here your customized
	security roles definitions. If you have used standard roles, you
	can simply uncomment following two arrays. After upgrade both arrays need 
	to be deleted or commented out. You may wish to change user roles to
	new better defined in Users Setup. Old not used roles can be set inactive 
	or deleted.
*/
/* Standard FA2.1 Security Group definitions

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
*/

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

/* 
	Display a dropdown select box for choosing Company to login if false.
	Show a blank editbox only if true where the Company NickName
	will have to be manually entered. This is when privacy is needed.
*/
$text_company_selection  = false;

/*  Should FA hide menu items (Applications, Modules, and Actions) from the user if they don't have access to them? 
    0 for no       1 for yes
*/

	$hide_inaccessible_menu_items = 0;

/*
	Brute force prevention.
	$login_delay seconds delay is required between login attempts after $login_max_attemps failed logins.
	Set $login_delay to 0 to disable the feature (not recommended)
*/
$login_delay = 30;
$login_max_attempts = 10;

/*
	Choose Exchange Rate Provider
	Default is ECB for backwards compatibility
*/
$xr_providers = array("ECB", "YAHOO", "GOOGLE", "BLOOMBERG");
$dflt_xr_provider = 0;

/*
	Set to true when remote service is authoritative source of exchange rates, and can be stored automatically without
	manual edition. Otherwise exrate is stored on first new currency transaction of the day.
*/
$xr_provider_authoritative = false;

/*
	Optional sorting sales documents lines during edition according to item code
*/
$sort_sales_items = false;

/*
	Trial Balance opening balance presentation option.
	When set to true past years part of opening balance is cleared.
*/
$clear_trial_balance_opening = false;

?>
