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

class fa2_4rc1 extends fa_patch {
	var $previous = '2.4.0';		// applicable database version
	var $version = '2.4.1';	// version installed
	var $description;
	var $sql = ''; // 'alter2.4rc1.sql';
	var $preconf = true;

	function fa2_4rc1() {
		parent::fa_patch();
		$this->description = _('Upgrade from version 2.4beta to 2.4rc1');
	}

	//
	//	Install procedure. All additional changes 
	//	not included in sql file should go here.
	//
	function install($company, $force=false)
	{
		// key 
		$sec_updates = array(
			'SA_SETUPCOMPANY' => array(
				'SA_ASSET', 'SA_ASSETCATEGORY', 'SA_ASSETCLASS',
				'SA_ASSETSTRANSVIEW','SA_ASSETTRANSFER', 'SA_ASSETDISPOSAL',
				'SA_DEPRECIATION', 'SA_ASSETSANALYTIC'),
		);
		$result = $this->update_security_roles($sec_updates);
		return $result;
	}

	//
	// optional procedure done after upgrade fail, before backup is restored
	//
	function post_fail($company)
	{
		$pref = $this->companies[$company]['tbpref'];
		db_query("DROP TABLE IF EXISTS " . $pref . 'stock_fa_class');
	}

}

$install = new fa2_4rc1;
