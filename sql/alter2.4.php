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
	function install($company, $force) 
	{
		global $db_version;
		
		if (get_company_pref('grn_clearing_act') === null) { // available form 2.3.1, can be not defined on pre-2.4 installations
			set_company_pref('grn_clearing_act', 'glsetup.purchase', 'varchar', 15, 0);
		}
		$result = $this->update_workorders();
		if ($result)
			$result = $this->do_cleanup();
//		return  update_company_prefs(array('version_id'=>$db_version));
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
	function installed($pref)
	{
		$n = 2; // number of patches to be installed
		$patchcnt = 0;

		if (!check_table($pref, 'suppliers', 'tax_algorithm')) $patchcnt++;
		if (!check_table($pref, 'wo_costing')) $patchcnt++;
		return $n == $patchcnt ? true : ($patchcnt ? ($patchcnt.'/'. $n) : 0);
	}

	function update_workorders()
	{
		global $db;

		$sql = "SELECT DISTINCT type, type_no, tran_date, person_id FROM ".TB_PREF."gl_trans WHERE `type`=".ST_WORKORDER
		." AND person_type_id=1";
		$res = db_query($sql);
		if (!$res)
		{
			display_error("Cannot update work orders costs"
				.':<br>'. db_error_msg($db));
			return false;
		}
		while ($row = db_fetch($res))
		{
			$journal_id = get_next_trans_no(ST_JOURNAL);

			$sql1 = "UPDATE ".TB_PREF."gl_trans SET `type`=".ST_JOURNAL.", type_no={$journal_id},
				person_type_id=NULL, person_id=0
				WHERE `type`=".ST_WORKORDER." AND type_no={$row['type_no']} AND tran_date='{$row['tran_date']}'
				AND person_id='{$row['person_id']}'";
			if (!db_query($sql1)) return false;
			
			$sql2 = "INSERT INTO ".TB_PREF."wo_costing (workorder_id, cost_type, trans_no) 
				VALUES ({$row['type_no']}, {$row['person_id']}, {$journal_id})";
			if (!db_query($sql2)) return false;
		}
		return true;
	}

	function do_cleanup()
	{
		$sql = "ALTER TABLE `".TB_PREF."tax_group_items` DROP COLUMN `rate`";
		return db_query($sql);
	}
}

$install = new fa2_4;
