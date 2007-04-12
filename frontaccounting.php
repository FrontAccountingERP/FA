<?php
	include_once('applications/application.php');
	include_once('applications/customers.php');
	include_once('applications/suppliers.php');
	include_once('applications/inventory.php');
	include_once('applications/manufacturing.php');
	include_once('applications/dimensions.php');
	include_once('applications/generalledger.php');
	include_once('applications/setup.php');
	include_once('themes/default/renderer.php');
	$path_to_root=".";
	include_once($path_to_root . "/includes/session.inc");

	class front_accounting 	{		var $user;
		var $settings;
		var $applications;
		var $selected_application;
		// GUI
		var $menu;
		var $renderer;
		function front_accounting() 		{			$this->renderer = new renderer();		}
		function add_application($app) 		{			$this->applications[$app->id] = &$app;		}
		function get_application($id) 		{			if (isset($this->applications[$id]))
				return $this->applications[$id];
			return null;		}
		function get_selected_application() 		{			if (isset($this->selected_application))
				return $this->applications[$this->selected_application];
			foreach ($this->applications as $application)
				return $application;
			return null;		}
		function display() 		{			$this->init();
			$this->renderer->wa_header();
			$this->renderer->menu_header($this->menu);
			$this->renderer->display_applications($this);
			$this->renderer->menu_footer($this->menu);
			$this->renderer->wa_footer();		}
		function init() 		{
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
			$this->add_application(new setup_app());		}	}
?>