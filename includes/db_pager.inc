<?php
//
//	Controler part of database table pager with column sort.
//	To display actual html object call display_db_pager($name) inside
//  any form.
//
//	View definition you will find in the following file:
include_once($path_to_root."/includes/ui/db_pager_view.inc");

class db_pager {
	var $sql;
	var $name;
	var $columns;		// column definitions (head, type, order)

	var $marker;		// marker check function
	var $marker_txt;	
	var $marker_class;
	var $notice_class;
			
	var $data = array();

	var $curr_page,
		$max_page,
	    $last_page, 
	    $prev_page, 
	    $next_page,
	    $first_page;
	    
	var $page_len,
	    $rec_count;
	
	var $select,
		$where,
	    $from,
		$group;
	var	$extra_where;
	
	var $ready = false;

	//  db_pager constructor
	//  accepts $sql like 'SELECT .... FROM .... [WHERE ...] [GROUP ...]'
	//	$name is base name for pager controls
	function db_pager($sql, $name, $page_len=0) 
	{
		if ($page_len == 0) $page_len = user_query_size();
		$this->name = $name;
		$this->page_len = $page_len;
	 	$this->set_sql($sql);
	}
	//
	//	Parse base sql select query.
	//
	function set_sql($sql)
	{
		if ($sql != $this->sql) {
		    $this->sql = $sql;
		    $this->ready = false;
			$select = $sql;
			$from = $where = $group = '';
	    	$parts = preg_split('/\sFROM\s/si', $sql, 2);
			if(count($parts) == 2) {
				$select = $parts[0];
				$from = $parts[1];
		    	$parts = preg_split('/\sWHERE\s/si', $from, 2);
				if(count($parts) == 2) {
					$from = $parts[0];
					$where = $parts[1];
		    		$parts = preg_split('/\sGROUP\s*BY\s/si', $where, 2);
					if(count($parts) == 2) {
						$where = $parts[0];
						$group = $parts[1];
					}
				}
			}
			$this->select = $select;
			$this->from = $from;
			$this->where = $where;
			$this->group = $group;
/*
		display_error("sql: $sql");
		display_error($select);
		display_error("FROM $from");
		display_error("WHERE $where");
		display_error("GROUP BY $group");
*/
		}
	}
	//
	//	Set additional constraint on record set
	//
	function set_where($where) 
	{
	    if (!is_array($where))
		  $where = array($where);

	    if (count($where) != count($this->extra_where) || 
			count(array_diff($this->extra_where, $where))) {
				$this->extra_where = $where;
				$this->ready = false;
	    }
	}
	//
	//	Set query result page
	//
	function change_page($page=null) 
	{
	    $this->set_page($page);
	    $this->query();
	    return true;
	}
	//
	//	Change sort column direction 
	//	in order asc->desc->none->asc
	//
	function sort_table($col=null) 
	{
	    $ord = $this->columns[$col]['ord'];
	    $ord = ($ord == '') ? 'asc' : (($ord == 'asc') ? 'desc' : '');
	    $this->columns[$col]['ord'] = $ord;
	    $this->set_page(1);
	    $this->query();
	    return true;
	}
	//
	// Query database
	//
	function query() 
	{
		global $Ajax;

		$Ajax->activate("_{$this->name}_span");
	    $this->data = array();
	    if (!$this->_init()) 
		  return false;

	    if ($this->rec_count == 0) return true;

	    $sql = $this->_sql_gen(false);

	    $result = db_query($sql, 'Error browsing database: '.$sql );

	    if ($result) {
		// setting field names for subsequent queries
			$c = 0;
		  // add result field names to column defs for 
		  // col value retrieve and sort purposes 
			for ($c = $i = 0; $c < count($this->columns); $c++) {
				if ($this->columns[$c]['type'] != 'insert')
			 		$this->columns[$c]['name']= mysql_field_name($result, $i++);
		  	}
		  
		  	while ($row = db_fetch_assoc($result))
				$this->data[] = $row;
		  
		} else 
		  return false;
		return true;
	}	    
	//
	//	Calculates page numbers for html controls.
	//
	function set_page($to) 
	{
	    switch($to) {
		case 'next':
		    $page = $this->curr_page+1; break;
		case 'prev':
		    $page = $this->curr_page-1; break;
		case 'last':
		    $page = $this->last_page; break;
		default:
		    if (is_numeric($to)) {
			 $page = $to; break;
		    }
		case 'first':
		    $page = 1; break;
	    }
	  if ($page < 1) 
	    $page = 1;
	  $max = $this->max_page;
	  if ($page > $max) 
	    $page = $max;
	  $this->curr_page = $page;
	  $this->next_page = ($page < $max) ? $page+1 : null;
	  $this->prev_page = ($page > 1) ? ($page-1) : null;
	  $this->last_page = ($page < $max) ? $max : null;
	  $this->first_page = ($page != 1) ? 1: null;
	}
	//
	//	Set column definitions
	//  $flds: array( fldname1, fldname2=>type,...)
	function set_columns($flds)
	{
		$this->columns = array();
		if (!is_array($flds)) {
			$flds = array($flds);
		}
		foreach ($flds as $colnum=>$coldef) {
			if (!is_numeric($colnum)) {	// 'colname'=>params
			  $h = $colnum;
			  $c = $coldef;
			} else {			//  n=>params
				if (is_array($coldef)) {
					$h = '';
					$c = $coldef;
				} else {
			  		$h = $coldef;
					$c = 'text';
				}
			  $c = '';
			}
			if (!is_array($c)) 			// params is simple column type
			  $c = array('type'=>$c);

			if (!isset($c['type']))
			  $c['type'] = 'text';

			switch($c['type']) {
			  case 'insert':
			  default:
				$c['head'] = $h; break;
			  case 'skip':		// skip the column (no header)
				unset($c['head']);	// paranoid code
	  		}
			$this->columns[] = $c;	
		}
	}
	//
	// Generate db query from base sql
	// $count==false - for current page data retrieval 
	// $count==true  - for total records count
	//
	function _sql_gen($count=false) 
	{
		$select = $this->select;
		$from = $this->from;
		$where = $this->where;
		$group = $this->group;		

		if(count($this->extra_where)) {
		    $wherw .= ($where=='' ? '' : ' AND ')
				.implode( $this->extra_where, ' AND ');
		}
		if ($where) $where = " WHERE($where)";

		if ($count) {
			$group = $group == '' ? "*" : "DISTINCT $group";

			return "SELECT COUNT($group) FROM $from $where";
		}

		$sql = "$select FROM $from $where GROUP BY $group";
	    $ord = array();

	    foreach( $this->columns as $col) {
	       	if (isset($col['ord'])) {
	        	if ( $col['ord'] != '' && isset($col['name'])) {
		    	    $ord[] = $col['name'] .' '. $col['ord'];
			    }
			}
	    }
				
	    if (count($ord)) 
			$sql .= " ORDER BY " . implode($ord, ',');

	    $page_len = $this->page_len;
	    $offset = ($this->curr_page - 1) * $page_len;

	    $sql .= " LIMIT $offset, $page_len";

		return $sql;
		
	}
	//
	//	Initialization after changing record set
	//
	function _init() 
	{
	    if ($this->ready == false ) {
			$sql = $this->_sql_gen(true);
			$result = db_query($sql, 'Error reading record set');
			if ($result == false) 
				return false;
			$row = db_fetch_row($result);
			$this->rec_count = $row[0];
	  		$this->max_page = ceil($this->rec_count/$this->page_len);
	  		$this->set_page(1);
	  		$this->ready = true;
	    }
    	return true;
	}
	//
	//	Set current page in response to user control.
	//
	function select_records() 
	{
		global $Ajax;
		
		$page = find_submit($this->name.'_page_', false);
		$sort = find_submit($this->name.'_sort_', false);
		if ($page) {
			$this->change_page($page);
			if ($page == 'next' && !$this->next_page ||
				$page == 'last' && !$this->last_page)
					set_focus($this->name.'_page_prev');
			if ($page == 'prev' && !$this->prev_page ||
				$page == 'first' && !$this->first_page)
					set_focus($this->name.'_page_next');
		} elseif ($sort) {
			$this->sort_table($sort);
		} else
			$this->query();
	}
	//
	//	Set check function to mark some rows.
	//	
	function set_marker($func, $notice='', $markercl='overduebg', $msgclass='overduefg' )
	{
		$this->marker = $func;
		$this->marker_txt = $notice;
		$this->marker_class = $markercl;
		$this->notice_class = $msgclass;
	}
};
//-----------------------------------------------------------------------------
//	Creates new db_pager $_SESSION object on first page call.
//  Retrieves from $_SESSION var on subsequent $_POST calls
//
//  $name - base name for pager controls and $_SESSION object name
//  $sql  - base sql for data inquiry. Order of fields implies
//		pager columns order.
//	$coldef - array of column definitions. Example definitions
//		Text column with title 'User name':
//				'User name'
//		Skipped field from sql query. Data for the field is not displayed:
//				'dummy' => 'skip'
//		Column without title, formated with function func(). Field value 
// is passed as parameter:
//	 			array('type'=>'spec', 'fun'=>'func')
// 		Inserted column with title 'Some', formated with function rowfun(). Row
//  		values are passed as parameter array:
//				'Some' => array('type'=>'insert', 'fun'=>'rowfun')
// 		Column with name 'Another', formatted as date, 
// sortable with ascending start order (available orders: asc,desc, '').
//				'Another' => array('type'=>'date', 'ord'=>'asc')
//
//	All available column format types you will find in db_pager_view.inc file.
//		If query result has more fields than count($coldef), rest of data is ignored
//  during display, but can be used in format handlers for 'spec' and 'insert' 
//	type columns.

function &new_db_pager($name, $sql, $coldef, $page_len = 0)  {

    if (isset($name) && isset($_SESSION[$name])) {
		 // kill old pager if any on first page call
	    if ($_SERVER['REQUEST_METHOD'] == 'GET')
			unset($_SESSION[$name]);
	    else
    		return $_SESSION[$name];
    }
    $ret =& new db_pager($sql, $name, $page_len);
    $ret->set_columns($coldef);

    if (isset($name)) $_SESSION[$name] = &$ret;

    return $ret;
}

?>