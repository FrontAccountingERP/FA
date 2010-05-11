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
//
//	Template for simple table editors
//
class simple_crud {
	var $name;
	var $Mode;
	var $selected_id = '';
	var $handlers;
	var $selector;
	//
	//
	function simple_crud($name, $options = array())
	{
		$this->name = $name;
		$this->handlers = array(
			'Edit' => '_edit',
			'Delete' => '_delete',
			'ADD' => '_add',
			'UPDATE' => '_update',
			'RESET' => '_cancel',
			'CLONE' => '_cloning'
			);
		// selector type: 'button' (default),  or 'list'
		$this->selector = @$options['selector']; // TODO
		
	}
	
	function _check_mode($numeric_id = true)
	{
		global $Ajax;

		$sel_name = $this->name.'_id';

		$this->Mode = 'Edit';
		foreach (array('Edit', 'Delete') as $m) {
			foreach ($_POST as $p => $pvar) {
				if (strpos($p, $this->name.$m) === 0) {
					unset($_POST['_focus']); // focus on first form entry
					$this->selected_id = quoted_printable_decode(substr($p, strlen($this->name.$m)));
					$Ajax->activate($this->name.'_div'); 
					$this->Mode = $m;
					return;
				}
			}
		}
		$default = '';
		$this->selected_id = get_post($sel_name, $default);
		if ($this->selected_id === $default) {
			$this->Mode = '';
			return;
		}
		foreach (array('ADD', 'UPDATE', 'RESET', 'CLONE') as $m) {
			if (isset($_POST[$this->name.$m])) {
				$Ajax->activate($this->name.'_div');
				unset($_POST['_focus']);
				$this->Mode = $m;
				return;
			}
		}
	}
	
	//
	//	Set record for edition
	//
	function _edit()
	{

		if ($this->selected_id !== '0') {
			$this->data = $this->db_read();
/*			foreach($this->data as $name => $value) {
				if (!is_numeric($name)) {
					$_POST[$name] = $value;
				}
			}
*/		}
	}
	//
	//	Update record after edition
	//
	function _update()
	{
		if ($this->insert_check()) {
			if ($this->db_update())
				$this->selected_id = '';
		}
	}
	//
	//	Add new record
	//
	function _add()
	{
		if ($this->insert_check()) {
			$this->db_insert();
			$this->selected_id = '';
		}
	}
	//
	//	Delete selected  record
	//
	function _delete()
	{
		if ($this->delete_check())
			$this->db_delete();
		$this->selected_id = '';
	}
	//
	//	Return to listing view
	//
	function _cancel()
	{
		$this->selected_id = '';
	}
	//
	// Clone record for new edition
	//
	function _cloning()
	{
		$this->selected_id = '0';
	}
	/*
		Generate form controls
	*/
	function _bottom_controls()
	{
		$clone = $this->selected_id == '0';

		$title=false;
		$async='both';
		$base=$this->name;

		$cancel = $async;

		if ($async === 'both') {
			$async = 'default'; $cancel = 'cancel';
		} 
		else if ($async === 'default')
			$cancel = true;
		else if ($async === 'cancel')
			$async = true;
		echo "<center>";

		if ($this->selected_id == '')
			submit("{$base}Edit0", _("Add new"), true, $title, $async);
		else {
			if ($this->selected_id == '0')
				submit("{$base}ADD", _("Add"), true, $title, $async);
			else {
				submit("{$base}UPDATE", _("Update"), true, _('Submit changes'), $async);
				if ($clone) 
					submit("${base}CLONE", _("Clone"), true, _('Edit new record with current data'), $async);
			}
			submit("{$base}RESET", _("Cancel"), true, _('Cancel edition'), $cancel);
		}
		echo "</center>";
	}
	//===========================================================================
	// Public functions
	//
	
	//	Main function - display current CRUD editor content
	//
	function show()
	{
		$this->_check_mode(true);
		div_start($this->name.'_div');
		if (array_key_exists($this->Mode, $this->handlers)) {
			$fun = $this->handlers[$this->Mode];
			$this->$fun();
		}
		if ($this->selected_id != '')
			$this->editor();
		else
			$this->listing();
		$this->_bottom_controls();
		hidden($this->name.'_id', $this->selected_id);
		div_end();
	}
	
	//===========================================================================
	//	Database functions placeholders
	
	//
	//	Read record from db for edition
	//
	function db_read() {
		display_notification('read sql...');
	}
	//
	//	Update record in db after edition
	//
	function db_update() {
		display_notification('update sql...');
	}
	//
	//	Delete record
	//
	function db_delete() {
		display_notification('delete sql...');
	}

	function delete_check() {
		display_notification('check delete ...');
		return true; 
	}
	//
	//	Insert record
	//
	function db_insert() {}

	function insert_check() { return true; }
	//
	//	Show database content in pager/table
	//
	function listing() {
		display_notification('listing ...');
	}

	//
	//	Show record editor screen content
	//
	function editor() {
		display_notification('editor ...');
	}
};

?>