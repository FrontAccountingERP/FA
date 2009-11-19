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
if (!isset($path_to_root) || isset($_GET['path_to_root']) || isset($_POST['path_to_root']))
	die("Restricted access");
	include_once($path_to_root . '/applications/application.php');
	include_once($path_to_root . '/applications/customers.php');
	include_once($path_to_root . '/applications/suppliers.php');
	include_once($path_to_root . '/applications/inventory.php');
	include_once($path_to_root . '/applications/manufacturing.php');
	include_once($path_to_root . '/applications/dimensions.php');
	include_once($path_to_root . '/applications/generalledger.php');
	include_once($path_to_root . '/applications/setup.php');
	include_once($path_to_root . '/installed_extensions.php');
	if (count($installed_extensions) > 0)
	{
		foreach ($installed_extensions as $ext)
		{
			if ($ext['type'] == 'module')
				include_once($path_to_root."/".$ext['path']."/".$ext['filename']);
		}
	}	

	class front_accounting
		{
		var $user;
		var $settings;
		var $applications;
		var $selected_application;
		// GUI
		var $menu;
		//var $renderer;
		function front_accounting()
		{
			//$this->renderer =& new renderer();
		}
		function add_application(&$app)
				{	
					if ($app->enabled) // skip inactive modules
						$this->applications[$app->id] = &$app;
				}
		function get_application($id)
				{
				 if (isset($this->applications[$id]))
					return $this->applications[$id];
				 return null;
				}
		function get_selected_application()
		{
			if (isset($this->selected_application))
				 return $this->applications[$this->selected_application];
			foreach ($this->applications as $application)
				return $application;
			return null;
		}
		function display()
		{
			global $path_to_root;
			include($path_to_root . "/themes/".user_theme()."/renderer.php");
			$this->init();
			$rend = new renderer();
			$rend->wa_header();
			//$rend->menu_header($this->menu);
			$rend->display_applications($this);
			//$rend->menu_footer($this->menu);
			$rend->wa_footer();
		}
		function init()
		{
			global $installed_extensions, $path_to_root;

			$this->menu = new menu(_("Main  Menu"));
			$this->menu->add_item(_("Main  Menu"), "index.php");
			$this->menu->add_item(_("Logout"), "/account/access/logout.php");
			$this->applications = array();
			$this->add_application(new customers_app());
			$this->add_application(new suppliers_app());
			$this->add_application(new inventory_app());
			$this->add_application(new manufacturing_app());
			$this->add_application(new dimensions_app());
			$this->add_application(new general_ledger_app());
			if (count($installed_extensions) > 0)
			{
				// Do not use global array directly here, or you suffer 
				// from buggy php behaviour (unexpected loop break 
				// because of same var usage in class constructor).
				$extensions = $installed_extensions;
				foreach ($extensions as $ext)
				{
					if (@($ext['active'] && $ext['type'] == 'module')) // supressed warnings before 2.2 upgrade
					{ 
						$_SESSION['get_text']->add_domain($_SESSION['language']->code, 
							$ext['path']."/lang");
						$class = $ext['tab']."_app";
						if (class_exists($class))
							$this->add_application(new $class());
						$_SESSION['get_text']->add_domain($_SESSION['language']->code, 
							$path_to_root."/lang");
					}
				}
			}	
			
			$this->add_application(new setup_app());
		}
}
?>