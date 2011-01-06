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
class fa2_4 {
	var $version = '2.4';	// version installed
	var $description;
	var $sql = 'alter2.4.sql';
	var $preconf = true;
	
	function fa2_4() {
		$this->description = _('Upgrade from version 2.3 to 2.4');
	}
	
	//
	//	Install procedure. All additional changes 
	//	not included in sql file should go here.
	//
	function install($pref, $force) 
	{
		global $db_version;
		
		if (get_company_pref('grn_clearing_act') === null) { // available form 2.3.1, can be not defined on pre-2.4 installations
			set_company_pref('grn_clearing_act', 'glsetup.purchase', 'varchar', 15, 0);
			refresh_sys_prefs();
		}

//		return  update_company_prefs(array('version_id'=>$db_version), $pref);
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

		if (!check_table($pref, 'suppliers', 'tax_algorithm')) $patchcnt++;
		return $n == $patchcnt ? true : ($patchcnt ? ($patchcnt.'/'. $n) : 0);
	}

}

$install = new fa2_4;

?>