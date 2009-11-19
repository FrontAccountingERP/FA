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
		
		function app_function($label,$link,$access='SA_OPEN') 
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
		
		function add_lapp_function($label,$link="",$access='SA_OPEN') 
		{
			$appfunction = new app_function($label,$link,$access);
			//array_push($this->lappfunctions,$appfunction);
			$this->lappfunctions[] = $appfunction;
			return $appfunction;
		}

		function add_rapp_function($label,$link="",$access='SA_OPEN') 
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
		var $help_context;
		var $modules;
		var $enabled;
		
		function application($id, $name, $enabled=true) 
		{
			$this->id = $id;
			$this->name = $name;
			$this->enabled = $enabled;
			$this->modules = array();
		}
		
		function add_module($name, $icon = null) 
		{
			$module = new module($name,$icon);
			//array_push($this->modules,$module);
			$this->modules[] = $module;
			return $module;
		}
		
		function add_lapp_function($level, $label,$link="",$access='SA_OPEN') 
		{
			$this->modules[$level]->lappfunctions[] = new app_function($label, $link, $access);
		}	
			
		function add_rapp_function($level, $label,$link="",$access='SA_OPEN') 
		{
			$this->modules[$level]->rappfunctions[] = new app_function($label, $link, $access);
		}	
	}


?>