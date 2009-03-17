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
class fa2_2 {
	var $version = '2.2';	// version installed
	var $description = 'Version 2.2';
	var $sql = 'alter2.2.sql';
	//
	//	Install procedure. All additional changes 
	//	not included in sql file should go here.
	//
	function install($pref, $force) 
	{
		global $db;
				
		return true;
	}
	//
	//	Checking before install
	//
	function pre_check($pref)
	{
		return true; // true when ok, fail otherwise
	}
	//
	//	Test if patch was applied before.
	//
	function installed($pref) {
		if (check_table($pref, 'company', 'default_delivery_required')) return false;
		return true;
	}
};

$install = new fa2_2;
?>