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

class fa2_3 {
	var $version = '2.3';	// version installed
	var $description;
	var $sql = 'alter2.3.sql';
	var $preconf = true;
	
	function fa2_3() {
		$this->description = _('Upgrade from version 2.2 to 2.3');
	}
	
	//
	//	Install procedure. All additional changes 
	//	not included in sql file should go here.
	//
	function install($pref, $force) 
	{
		return true;
	}
	//
	//	Checking before install
	//
	function pre_check($pref, $force)
	{
		return true;
	}
	//
	//	Test if patch was applied before.
	//
	function installed($pref) {
		$n = 1; // number of patches to be installed
		$patchcnt = 0;

		if (!check_table($pref, 'comments', 'type', array('Key'=>'MUL'))) $patchcnt++;

		$n -= $patchcnt;
		return $n == 0 ? true : $patchcnt;
	}
};

$install = new fa2_3;
?>