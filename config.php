<?php
	/*--------------------------------------------------\
	| 		|               | config.php        		|
	|---------------------------------------------------|
    | FrontAccounting 									|
    | http://frontaccounting.com/  						|
    | by FrontAccounting                       			|
	|---------------------------------------------------|
	|                                                   |
	\--------------------------------------------------*/

/*
	// Make sure this directory exists and is writable!
    $session_save_path = dirname(__FILE__).'/tmp/';

*/
	$session_save_path = session_save_path();
	if (strpos($session_save_path, ";") !== false)
  		$session_save_path = substr($session_save_path, strpos($session_save_path, ";") + 1);

	if (isset($session_save_path))
	{
		session_save_path($session_save_path);
		unset($session_save_path);
    }
	if (!isset($path_to_root) || isset($_GET['path_to_root']) || isset($_POST['path_to_root']))
		die("Restricted access");
    include_once($path_to_root . "/config_db.php");
    include_once($path_to_root . "/includes/lang/language.php");

	//--------------------------------------------------

	// User configurable variables
    //---------------------------------------------------

    /*Show debug messages returned from an error on the page.
    Debugging info level also determined by settings in PHP.ini
    if $debug=1 show debugging info, dont show if $debug=0 */

    $debug 			= 1;
	$show_sql 		= 0;
	$go_debug 		= 0;
	if ($go_debug == 1)
	{
		error_reporting(E_ALL);
		ini_set("display_errors", "On");
	}
	else
	{
		// ini_alter("error_reporting","E_COMPILE_ERROR|E_ERROR|E_CORE_ERROR|E_PARSE");
		ini_set("display_errors", "Off");
	}
	// Main Title
	$app_title = "FrontAccounting";
    // application version
    $version 		= "1.14";

    // Build for development purposes
    $build_version 	= "79";

	// Powered by
	$power_by 		= "FrontAccounting";
	$power_url 		= "http://frontaccounting.net";

    /*Stock units array*/
	$stock_units 	= array(_("ea."), _("m"), _("kgg"), _("tons"), _("l"), _("lbs"), _("dozen"), _("pack"), _("hrs"));

	/* use popup windows for views */
	$use_popup_windows = 1;

	/* use date picker for all date fields */
	$use_date_picker = 1;

	/* use Audit Trails in GL */
	$use_audit_trail = 0;

	/* allow alpha characters in accounts. 0 = numeric, 1 = alpha numeric, 2 = uppercase alpha numeric */
	$accounts_alpha = 0;

	/* default start-up tab (orders/AP/stock/manuf/proj/GL/system) */
	$def_app = "orders";

	/* Date systems. 0 = traditional, 1 = Jalali used by Iran, nabour countries, Afghanistan and some other Central Asian nations,
	   2 = Islamic used by other arabic nations */
	$date_system = 0;

	$dateformats 	= array("MMDDYYYY", "DDMMYYYY", "YYYYMMDD");
	$dateseps 		= array("/", ".", "-", " ");
	$thoseps 		= array(",", ".", " ");
	$decseps 		= array(".", ",");
    // CSS theme
    $themes 		= array("default","aqua","cool");
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
    Access authorisation is checked in header.inc this is where _SESSION["AccessLevel"] is the index of the security_groups array. If you wish to add more security groups with then you must add a new SecurityHeading to the security_headings array
    and a new array of Security categories to the Security Groups array
    This mechanism allows more fine grained control of access
    security_groups is an array of arrays
    The index is the order in which the array of allowed pages is defined new ones can be defined at will
    or by changing the numbers in each array the security access can be tailored. These numbers need to read
    in conjunction with the Page Security index
    */

    $security_headings = array(
      		_("Inquiries"),
      		_("Accountant"),
      		_("System Administrator")
    );

    $security_groups = array(
    		array(1,2),
    		array(1,2,3,4,5,6,7,8,9,10,11,12,13,14),
    		array(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15)
    );

	//MySQL Backup and Restore Settings

    define("BACKUP_PATH", "/admin/backup/");

?>