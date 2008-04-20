<?php

if (!isset($path_to_root) || isset($_GET['path_to_root']) || isset($_POST['path_to_root']))
	die("Restricted access");
include_once($path_to_root . "/lang/installed_languages.inc");
include_once($path_to_root . "/includes/lang/gettext.php");

class language 
{
	var $name;
	var $code;			// eg. ar_EG, en_GB
	var $encoding;		// eg. UTF-8, CP1256, ISO8859-1
	var	$dir;			// Currently support for Left-to-Right (ltr) and
						// Right-To-Left (rtl)

	function language($name, $code, $encoding) 
	{
		$this->name = $name;
		$this->code = $code;
		$this->encoding = $encoding;
		$this->dir = "ltr";
	}

	function get_language_dir() 
	{
		return "lang/" . $this->code;
	}


	function get_current_language_dir() 
	{
		$lang = $_SESSION['language'];
		return "lang/" . $lang->code;
	}

	function set_language($code) 
	{
		if (isset($_SESSION['languages'][$code]) &&
			$_SESSION['language'] != $_SESSION['languages'][$code]) 
		{
			$_SESSION['language'] = $_SESSION['languages'][$code];
			reload_page("");
		}
	}

	function get_stylesheet() 
	{
		return 'lang/' . $_SESSION['language']->code . '/stylesheet.css';
	}

	/**
	 * This method loads an array of language objects into a session variable
     * called $_SESSIONS['languages']. Only supported languages are added.
     */
	function load_languages() 
	{
		global $installed_languages;

		$_SESSION['languages'] = array();

        foreach ($installed_languages as $lang) 
        {
			$l = new language($lang['name'],$lang['code'],$lang['encoding']);
			if (isset($lang['rtl']))
				$l->dir = "rtl";
			$_SESSION['languages'][$l->code] = $l;
        }

		if (!isset($_SESSION['language']))
			$_SESSION['language'] = $_SESSION['languages']['en_GB'];
	}

}

session_start();
// this is to fix the "back-do-you-want-to-refresh" issue - thanx PHPFreaks
header("Cache-control: private");

// Page Initialisation
if (!isset($_SESSION['languages'])) 
{
	language::load_languages();
}

$lang = $_SESSION['language'];

// get_text support
get_text::init();
get_text::set_language($lang->code, $lang->encoding);
//get_text::add_domain("wa", $path_to_root . "/lang");
get_text::add_domain($lang->code, $path_to_root . "/lang");
// Unnecessary for ajax calls. 
// Due to bug in php 4.3.10 for this version set globally in php4.ini
ini_set('default_charset', $_SESSION['language']->encoding);

if (!function_exists("_")) 
{
	function _($text) 
	{
		$retVal = get_text::gettext($text);
		if ($retVal == "")
			return $text;
		return $retVal;
	}
}

function _set($key,$value) 
{
	get_text::set_var($key,$value);
}

function reload_page($msg) 
{
//	header("Location: $_SERVER['PHP_SELF']."");
//	exit;
	echo "<html>";
	echo "<head>";
    echo "<title>Changing Languages</title>";
	echo '<meta http-equiv="refresh" content="0;url=' . $_SERVER['PHP_SELF'] . '">';
	echo '</head>';
	echo '<body>';
	echo '<div>';
	if ($msg != "")
		echo $msg . " " . $_SERVER['PHP_SELF'];
	echo "</div>";	
	echo "</body>";
	echo "</html>";
}



?>