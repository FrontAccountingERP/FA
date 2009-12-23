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
// Prevent register_globals vulnerability
if (isset($_GET['path_to_root']) || isset($_POST['path_to_root']))
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
	var $is_locale_file;
	
	function language($name, $code, $encoding, $dir = 'ltr') 
	{
		$this->name = $name;
		$this->code = $code ? $code : 'en_GB';
		$this->encoding = $encoding;
		$this->dir = $dir;
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
	    global $comp_path, $path_to_root, $installed_languages;

		$changed = $this->code != $code;
		$lang = array_search_value($code, $installed_languages, 'code');

		if ($lang && $changed)
		{
		// flush cache as we can use several languages in one account
			flush_dir($comp_path.'/'.user_company().'/js_cache');

			$this->name = $lang['name'];
			$this->code = $lang['code'];
			$this->encoding = $lang['encoding'];
			$this->dir = isset($lang['rtl']) ? 'rtl' : 'ltr';
			$locale = $path_to_root . "/lang/" . $this->code . "/locale.inc";
			$this->is_locale_file = file_exists($locale);
		}

		$_SESSION['get_text']->set_language($this->code, $this->encoding);
		$_SESSION['get_text']->add_domain($this->code, $path_to_root . "/lang");

		// Necessary for ajax calls. Due to bug in php 4.3.10 for this 
		// version set globally in php.ini
		ini_set('default_charset', $this->encoding);

		if (isset($_SESSION['App']) && $changed)
			$_SESSION['App']->init(); // refresh menu
	}
}

function _set($key,$value) 
{
	$_SESSION['get_text']->set_var($key,$value);
}

if (!function_exists("_")) 
{
	function _($text) 
	{
		$retVal = $_SESSION['get_text']->gettext($text);
		if ($retVal == "")
			return $text;
		return $retVal;
	}
}
?>