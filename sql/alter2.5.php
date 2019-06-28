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

class fa2_5 extends fa_patch {
	var $previous = '2.4.1';		// applicable database version
	var $version = '2.5.0';	// version installed
	var $description;
	var $sql = 'alter2.5.sql';
	var	$max_upgrade_time = 900;
	
	function __construct() {
		parent::__construct();
		$this->description = _('Upgrade from version 2.4 to 2.5');
	}

	//
	//	Install procedure. All additional changes 
	//	not included in sql file should go here.
	//
	function install($company, $force=false)
	{
		return true;
	}

	//
	// optional procedure done after upgrade fail, before backup is restored
	//
	function post_fail($company)
	{
	}

}

$install = new fa2_5;
