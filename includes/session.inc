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
	function output_html($text)
	{
  	  global $before_box, $Ajax, $messages;
		// Fatal errors are not send to error_handler, 
		// so we must check the output
	  if ($text && preg_match('/\bFatal error(<.*?>)?:(.*)/i', $text, $m)) {
       	$Ajax->aCommands = array();  // Don't update page via ajax on errors
		$text = preg_replace('/\bFatal error(<.*?>)?:(.*)/i','', $text);
		$messages[] = array(E_ERROR, $m[2], null, null);
	  }
	  $Ajax->run();
  	  return  in_ajax() ? fmt_errors() : ($before_box.fmt_errors().$text);
	}

	if (!isset($path_to_root))
	{
		$path_to_root = ".";
	}

    include_once($path_to_root . "/includes/current_user.inc");

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
	include_once($path_to_root . "/includes/lang/language.php");
	include_once($path_to_root . "/config_db.php");
	include_once($path_to_root . "/config.php");
	include_once($path_to_root . "/includes/ajax.inc");
	$Ajax =& new Ajax();

	include_once($path_to_root . "/includes/main.inc");

    //----------------------------------------------------------------------------------------

	function kill_login()
	{
        session_unset();
		session_destroy();
	}

	//----------------------------------------------------------------------------------------

	function login_fail()
	{
        echo "<center><br><br><font size='5' color='red'><b>" . _("Incorrect Password") . "<b></font><br><br>";
        echo "<b>" . _("The user and password combination is not valid for the system.") . "<b><br><br>";

        echo _("If you are not an authorized user, please contact your system administrator to obtain an account to enable you to use the system.");
		echo "<br><a href='javascript:history.go(-1)'>" . _("Back") . "</a>";
        echo "</center>";

        kill_login();
        die();
	}

	//----------------------------------------------------------------------------------------

	function check_page_security($page_security)
	{
		if (!$_SESSION["wa_current_user"]->check_user_access())
		{
			echo "<br><br><br><center>";
			echo "<b>" . _("Security settings have not been defined for your user account.");
			echo "<br>" . _("Please contact your system administrator.") . "</b>";

			kill_login();
			exit;
		}

		if (!$_SESSION["wa_current_user"]->can_access_page($page_security))
		{
			page(_("Access denied"));
			echo "<center><br><br><br><b>";
			echo _("The security settings on your account do not permit you to access this function");
			echo "</b>";
			echo "<br><br><br><br></center>";
			//echo '<script type="text/javascript">';
			//echo 'alert("' . _("The security settings on your account do not permit you to access this function") . '");';
			//echo 'history.go(-1)';
			//echo '</script>'
			end_page();
			//kill_login();
			exit;
		}
	}

	//-----------------------------------------------------------------------------
	//	Removing magic quotes from nested arrays/variables
	//
	function strip_quotes($data)
	{
		if(get_magic_quotes_gpc()) {
			if(is_array($data)) {
				foreach($data as $k => $v) {
					$data[$k] = strip_quotes($data[$k]);
				}
			} else
				return stripslashes($data);
		}
		return $data;
	}


	//----------------------------------------------------------------------------------------
	if (!isset($_SESSION["wa_current_user"]) ||
		(isset($_SESSION["wa_current_user"]) && !$_SESSION["wa_current_user"]->logged_in()))
	{

		$_SESSION["wa_current_user"] = new current_user();

        // Show login screen
        if (!isset($_POST["user_name_entry_field"]) or $_POST["user_name_entry_field"] == "")
        {
	        include($path_to_root . "/access/login.php");
			$Ajax->redirect($path_to_root . "/access/login.php");
            exit;
        }
   	}
	include_once($path_to_root . "/includes/ui/ui_msgs.inc");
	// intercept all output to destroy it in case of ajax call
	register_shutdown_function('ob_end_flush');
	ob_start('output_html',0);
	// colect all error msgs
	set_error_handler('error_handler' /*, errtypes */);

   	if (isset($_POST["user_name_entry_field"]))
   	{
		$succeed = $_SESSION["wa_current_user"]->login($_POST["company_login_name"],
			$_POST["user_name_entry_field"],
			md5($_POST["password"]));
		// select full vs fallback ui mode on login
		$_SESSION["wa_current_user"]->ui_mode = $_POST['ui_mode'];
		if (!$succeed)
		{
			// Incorrect password
			login_fail();
		}
   	}

    // Run with debugging messages for the system administrator(s) but not anyone else
    /*if (in_array(15, $security_groups[$_SESSION["AccessLevel"]])) {
        $debug = 1;
    } else {
        $debug = 0;
    }*/

	//----------------------------------------------------------------------------------------

   	check_page_security($page_security);

// POST vars cleanup needed for direct reuse.
// We quote all values later with db_escape() before db update.
	$_POST = strip_quotes($_POST);

?>