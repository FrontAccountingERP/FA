<?php
	/*--------------------------------------------------\
	|               |               | session.inc       |
	|---------------------------------------------------|
    | front_accounting 									|
    | http://open-accounting.sourceforge.net/  			|
    | by KylieTech Consulting                           |
    | http://frontaccounting.com/           			|
    | by Joe Hunt Consulting         |
	\--------------------------------------------------*/

	if (!isset($path_to_root)) 
	{
		$path_to_root = ".";
	}

    include_once($path_to_root . "/includes/current_user.inc");

    include_once($path_to_root . "/config.php");

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
			echo "<br><br><a href='javascript:history.go(-1)'>" . _("Back") . "</a>";
			echo "<br><br><br><br>";
			//echo '<script type="text/javascript">';
			//echo 'alert("' . _("The security settings on your account do not permit you to access this function") . '");';
			//echo 'history.go(-1)';
			//echo '</script>'
			end_page();
			//kill_login();
			exit;
		}
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
            exit;
        }
   	}

   	if (isset($_POST["user_name_entry_field"])) 
   	{
		$succeed = $_SESSION["wa_current_user"]->login($_POST["company_login_name"],
			$_POST["user_name_entry_field"],
			md5($_POST["password"]));

		if (!$succeed) 
		{
			// Incorrect password
			login_fail();
		}
   	}

   	check_page_security($page_security);

    // Run with debugging messages for the system administrator(s) but not anyone else
    /*if (in_array(15, $security_groups[$_SESSION["AccessLevel"]])) {
        $debug = 1;
    } else {
        $debug = 0;
    }*/

	//----------------------------------------------------------------------------------------
?>