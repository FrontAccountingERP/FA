<?php

	class menu_item 
	{
		var $label;
		var $link;
		
		function menu_item($label, $link) 
		{
			$this->label = $label;
			$this->link = $link;
		}
	}

	class menu 
	{
		var $title;
		var $items;
		
		function menu($title) 
		{
			$this->title = $title;
			$this->items = array();
		}
		
		function add_item($label, $link) 
		{
			$item = new menu_item($label,$link);
			array_push($this->items,$item);
			return $item;
		}
		
	}

	class app_function 
	{
		var $label;
		var $link;
		var $access;
		
		function app_function($label,$link,$access=1) 
		{
			$this->label = $label;
			$this->link = $link;
			$this->access = $access;
		}
	}

	class module 
	{
		var $name;
		var $icon;
		var $lappfunctions;
		var $rappfunctions;
		
		function module($name,$icon = null) 
		{
			$this->name = $name;
			$this->icon = $icon;
			$this->lappfunctions = array();
			$this->rappfunctions = array();
		}
		
		function add_lapp_function($label,$link="",$access=1) 
		{
			$appfunction = new app_function($label,$link,$access);
			//array_push($this->lappfunctions,$appfunction);
			$this->lappfunctions[] = $appfunction;
			return $appfunction;
		}

		function add_rapp_function($label,$link="",$access=1) 
		{
			$appfunction = new app_function($label,$link,$access);
			//array_push($this->rappfunctions,$appfunction);
			$this->rappfunctions[] = $appfunction;
			return $appfunction;
		}
		
		
	}

	class application 
	{
		var $id;
		var $name;
		var $modules;
		var $enabled;
		
		function application($id, $name, $enabled=true) 
		{
			$this->id = $id;
			$this->name = $name;
			$this->enables = $enabled;
			$this->modules = array();
		}
		
		function add_module($name, $icon = null) 
		{
			$module = new module($name,$icon);
			//array_push($this->modules,$module);
			$this->modules[] = $module;
			return $module;
		}
		
		function add_lapp_function($level, $label,$link="",$access=1) 
		{
			$this->modules[$level]->lappfunctions[] = new app_function($label, $link, $access);
		}	
			
		function add_rapp_function($level, $label,$link="",$access=1) 
		{
			$this->modules[$level]->rappfunctions[] = new app_function($label, $link, $access);
		}	
	}


?>