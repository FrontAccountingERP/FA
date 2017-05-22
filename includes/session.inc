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

if (!isset($path_to_root))
{
	$path_to_root = ".";
}

// Prevent register_globals vulnerability
if (isset($_GET['path_to_root']) || isset($_POST['path_to_root']))
	die("Restricted access");

include_once($path_to_root . "/includes/errors.inc");
// colect all error msgs
set_error_handler('error_handler' /*, errtypes */);

include_once($path_to_root . "/includes/session_utils.inc");
include_once($path_to_root . "/includes/current_user.inc");
include_once($path_to_root . "/frontaccounting.php");
include_once($path_to_root . "/admin/db/security_db.inc");
include_once($path_to_root . "/includes/lang/language.inc");
include_once($path_to_root . "/config_db.php");
include_once($path_to_root . "/includes/ajax.inc");
include_once($path_to_root . "/includes/ui/ui_msgs.inc");
include_once($path_to_root . "/includes/prefs/sysprefs.inc");

include_once($path_to_root . "/includes/hooks.inc");
//
// include all extensions hook files.
//
foreach ($installed_extensions as $ext)
{
	if (file_exists($path_to_root.'/'.$ext['path'].'/hooks.php'))
		include_once($path_to_root.'/'.$ext['path'].'/hooks.php');
}
$Session_manager = new SessionManager();
$Session_manager->sessionStart('FA'.md5(dirname(__FILE__)));

$_SESSION['SysPrefs'] = new sys_prefs();

$SysPrefs = &$_SESSION['SysPrefs'];

//----------------------------------------------------------------------------------------
// set to reasonable values if not set in config file (pre-2.3.12 installations)

if ((!isset($SysPrefs->login_delay)) || ($SysPrefs->login_delay < 0))
    $SysPrefs->login_delay = 10;

if ((!isset($SysPrefs->login_max_attempts)) || ($SysPrefs->login_max_attempts < 0))
    $SysPrefs->login_max_attempts = 3; 

if ($SysPrefs->go_debug > 0)
	error_reporting(-1);
else
	error_reporting(E_USER_WARNING|E_USER_ERROR|E_USER_NOTICE);
ini_set("display_errors", "On");

if ($SysPrefs->error_logfile != '') {
	ini_set("error_log", $SysPrefs->error_logfile);
	ini_set("ignore_repeated_errors", "On");
	ini_set("log_errors", "On");
}

/*
	Uncomment the setting below when using FA on shared hosting
	to avoid unexpeced session timeouts.
	Make sure this directory exists and is writable!
*/
// ini_set('session.save_path', dirname(__FILE__).'/../tmp/');

ini_set('session.gc_maxlifetime', 36000); // 10hrs

hook_session_start(@$_POST["company_login_name"]);

// this is to fix the "back-do-you-want-to-refresh" issue - thanx PHPFreaks
header("Cache-control: private");

get_text_init();

if ($SysPrefs->login_delay > 0)
	@include_once($path_to_root . "/tmp/faillog.php");

// Page Initialisation
if (!isset($_SESSION['wa_current_user']) || !$_SESSION['wa_current_user']->logged_in()
	|| !isset($_SESSION['language']) || !method_exists($_SESSION['language'], 'set_language'))
{
	$l = array_search_value($dflt_lang, $installed_languages,  'code');
	$_SESSION['language'] = new language($l['name'], $l['code'], $l['encoding'],
	 (isset($l['rtl']) && $l['rtl'] === true) ? 'rtl' : 'ltr');
}

$_SESSION['language']->set_language($_SESSION['language']->code);


include_once($path_to_root . "/includes/access_levels.inc");
include_once($path_to_root . "/version.php");
include_once($path_to_root . "/includes/main.inc");
include_once($path_to_root . "/includes/app_entries.inc");

// Ajax communication object
$Ajax = new Ajax();

// js/php validation rules container
$Validate = array();
// bindings for editors
$Editors = array();
// page help. Currently help for function keys.
$Pagehelp = array();

$Refs = new references();

// intercept all output to destroy it in case of ajax call
register_shutdown_function('end_flush');
ob_start('output_html',0);

if (!isset($_SESSION["wa_current_user"]))
	$_SESSION["wa_current_user"] = new current_user();

html_cleanup($_GET);
html_cleanup($_POST);
html_cleanup($_REQUEST);
html_cleanup($_SERVER);

// logout.php is the only page we should have always 
// accessable regardless of access level and current login status.
if (!defined('FA_LOGOUT_PHP_FILE')){

	login_timeout();

	if (!$_SESSION["wa_current_user"]->old_db)
		include($path_to_root . '/company/'.user_company().'/installed_extensions.php');

	install_hooks();

	if (!$_SESSION["wa_current_user"]->logged_in())
	{
      if (@$SysPrefs->allow_password_reset && !$SysPrefs->allow_demo_mode
        && (isset($_GET['reset']) || isset($_POST['email_entry_field']))) {
		  if (!isset($_POST["email_entry_field"])) {
        include($path_to_root . "/access/password_reset.php");
        exit();
      }
      else {
        if (isset($_POST["company_login_nickname"]) && !isset($_POST["company_login_name"])) {
          for ($i = 0; $i < count($db_connections); $i++) {
            if ($db_connections[$i]["name"] == $_POST["company_login_nickname"]) {
              $_POST["company_login_name"] = $i;
              unset($_POST["company_login_nickname"]);
              break 1; // cannot pass variables to break from PHP v5.4 onwards
            }
          }
        }
        $_succeed = isset($db_connections[$_POST["company_login_name"]]) &&
          $_SESSION["wa_current_user"]->reset_password($_POST["company_login_name"],
          $_POST["email_entry_field"]);
        if ($_succeed)
        {
          password_reset_success();
        }

        password_reset_fail();
      }
    }
		// Show login screen
		if (!isset($_POST["user_name_entry_field"]) or $_POST["user_name_entry_field"] == "")
		{
			// strip ajax marker from uri, to force synchronous page reload
			$_SESSION['timeout'] = array( 'uri'=>preg_replace('/JsHttpRequest=(?:(\d+)-)?([^&]+)/s',
					'', html_specials_encode($_SERVER['REQUEST_URI'])),
				'post' => $_POST);

			include($path_to_root . "/access/login.php");
			if (in_ajax())
				$Ajax->activate('_page_body');
			exit;
		} else {
			if (isset($_POST["company_login_nickname"]) && !isset($_POST["company_login_name"])) {
				for ($i = 0; $i < count($db_connections); $i++) {
					if ($db_connections[$i]["name"] == $_POST["company_login_nickname"]) {
						$_POST["company_login_name"] = $i;
						unset($_POST["company_login_nickname"]);
						break 1; // cannot pass variables to break from PHP v5.4 onwards
					}
				}
			}
			$succeed = isset($db_connections[$_POST["company_login_name"]]) &&
				$_SESSION["wa_current_user"]->login($_POST["company_login_name"],
				$_POST["user_name_entry_field"], $_POST["password"]);
			// select full vs fallback ui mode on login
			$_SESSION["wa_current_user"]->ui_mode = $_POST['ui_mode'];
			if (!$succeed)
			{
			// Incorrect password
				login_fail();
			}
			elseif(isset($_SESSION['timeout']) && !$_SESSION['timeout']['post'])
			{
				// in case of GET request redirect to avoid confirmation dialog 
				// after return from menu option
				header("HTTP/1.1 303 See Other");
				header("Location: ".$_SESSION['timeout']['uri']);
				exit();
			}
			$lang = &$_SESSION['language'];
			$lang->set_language($_SESSION['language']->code);
		}
	} else
	{
		set_global_connection();

		if (db_fixed())
			db_set_encoding($_SESSION['language']->encoding);

		$SysPrefs->refresh();
	}
	if (!isset($_SESSION["App"])) {
		$_SESSION["App"] = new front_accounting();
		$_SESSION["App"]->init();
	}
}

// POST vars cleanup needed for direct reuse.
// We quote all values later with db_escape() before db update.
$_POST = strip_quotes($_POST);
