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

// Standard report classess
define('RC_CUSTOMER', 0);
define('RC_SUPPLIER', 1);
define('RC_INVENTORY', 2);
define('RC_MANUFACTURE', 3);
define('RC_DIMENSIONS', 4);
define('RC_BANKING', 5);
define('RC_GL', 6);

class BoxReports
{
	var $ar_classes; // report class names
	var $ar_reports;
	var $ctrl_handlers = array();

	function ReportClasses()
	{
		$this->ar_classes = array();
	}

	function addReportClass($class_name, $class_id=null)
	{
		if (!$class_id)
			$class_id = count($this->ar_classes);
		$this->ar_classes[$class_id] = $class_name;

		return $class_id;
	}

	function addReport($class, $id, $rep_name, $params=null)
	{
		unset($this->ar_reports[$class][$id]); // unset std report if any
		$this->ar_reports[$class][$id] = new Report($id, $rep_name, $params);
	}

	function getDisplay($class=null)
	{
		global $path_to_root, $pdf_debug, $Ajax;

		if (find_submit('Rep') != -1) {
			include($path_to_root.'/reporting/prn_redirect.php');
			return;
		}

		$temp = array_values($this->ar_classes);
		$display_class = $class==null ? $temp[0] : $this->ar_classes[$class];
		$st_reports = "";
		$st_params = "";
		$st_classes = "<b>" . _("Report Classes:") . "</b><br>";

		if (isset($_GET['Class']))
			set_focus('class'.$_GET['Class']);

		$cur_class = $_REQUEST['Class'];

		foreach($this->ar_classes as $class_id => $name)
		{
			if (!isset($this->ar_reports[$class_id]))
				continue; // e.g. no dimensions

			$acc = access_string($name);
			$st_classes .= "<a href='"
				.$_SERVER['PHP_SELF']."?Class=$class_id'"
				." style='font-weight:". ($cur_class == $class_id ? 'bold' : 'normal')."'"
				." class='repclass_link' id='".'class'.$class_id."'"
				."$acc[1]>$acc[0]</a> <br>";

			$style = $class_id==$cur_class ? '' : "style='display:none'";
			$st_reports .= "<table class='repclass' id='TAB_" . $class_id ."' $style cellpadding=0 cellspacing=0 border=0 width='100%'><tr><td><b>" . _("Reports For Class: ") . "&nbsp;$name</b></td></tr>\n";
			foreach($this->ar_reports[$class_id] as $rep_id => $report)
			{
				$acc = access_string($report->name);
				$st_reports .= "<tr><td><a class='repopts_link'"
					." href='".$_SERVER['PHP_SELF']."?Class=$class_id&REP_ID=$report->id'"
					." style='font-weight:". (@$_REQUEST['REP_ID'] == $report->id ? 'bold' : 'normal')."'"
					." id='".$id = default_focus()."'"
					."$acc[1]>$acc[0]</a><tr><td>\n";

				if (@$_REQUEST['REP_ID'] == $report->id) {
					$Ajax->activate($id);
				}

				if (isset($_REQUEST['REP_ID']) && $_REQUEST['REP_ID']==$report->id) {
					// parameters form
					$action = $_SERVER['PHP_SELF'];
					$st_params = "<table><tr><td>\n"
						. "<form method='POST' action='$action' target='_blank'>\n";
					$st_params .= submit('Rep'.$report->id,  
						_("Display: ") . access_string($report->name, true),
						false, '', $pdf_debug ? false : 'default process') . hidden('REP_ID', $report->id, false).'<br><br>';
					$st_params .= $this->getOptions($report->get_controls(), $report->id);
					$st_params .= "\n<input type=hidden name='Class' value=".$cur_class.">"
						."\n</form></td></tr></table>\n";
					set_focus('Rep'.$report->id);
					$Ajax->addUpdate(true, 'rep_form', $st_params);
				}
			}
			$st_reports .= "</table>";
		}

		$st_params = "<div id='rep_form'>".
			"$st_params</div>";
		
		$st = "<table align='center' width='80%' style='border:1px solid #cccccc;'><tr valign='top'>";
		$st .= "<td width='30%'>$st_classes</td>";
		$st .= "<td width='35%' style='border-left:1px solid #cccccc;border-right:1px solid #cccccc;padding-left:3px;'>$st_reports</td>";
		$st .= "<td width='35%'>$st_params</td>";
		$st .= "</tr></table><br>";

		return $st;
	}

	function getOptions($controls, $id = 0)
	{
		$st = '';
		if ($controls == null)
			return "";

		$cnt = 0;
		foreach($controls as $title => $type)
		{
			$ctrl = '';
			foreach($this->ctrl_handlers as $fun) { // first check for non-standard controls
				$ctrl = $fun('PARAM_'.$cnt, $type);
				if ($ctrl) break;
			}
			if ($ctrl == '') {
				if (isset($_COOKIE['select'][$id][$cnt])) // saved parameters 2010-10-06 Joe Hunt
					$_POST['PARAM_'.$cnt] = $_COOKIE['select'][$id][$cnt];
				$ctrl = $this->get_ctrl('PARAM_'.$cnt, $type);
			}
			if ($ctrl != '')  {
				$st .= $title . ':<br>';
				$st .= $ctrl;
				$st .= "<br><br>";
			} else {
				display_error(_('Unknown report parameter type:').$type);
			}
			$cnt++;
		}
		return $st;
	}

	//
	//	Register additional control handler
	// $handle - name of global function f($name, $type) returning html code for control
	function register_controls($handler) {
		$this->ctrl_handlers[] = $handler;
	}
	//
	//	Returns html code for input control $name of type $type 
	//
	function get_ctrl($name, $type)
	{
		global $path_to_root, $use_date_picker, $pdf_debug, $print_invoice_no,
			$def_print_destination, $def_print_orientation, $type_shortcuts;

		$st = '';
			switch ($type)
			{
				case 'CURRENCY':
					$sql = "SELECT curr_abrev, concat(curr_abrev,' - ', currency) AS name FROM ".TB_PREF."currencies";
					return combo_input($name, '', $sql, 'curr_abrev', 'name',array('spec_option'=>_("No Currency Filter"),'spec_id'=>ALL_TEXT,'order'=>false));

				case 'DATE':
				case 'DATEBEGIN':
				case 'DATEEND':
				case 'DATEBEGINM':
				case 'DATEENDM':
				case 'DATEBEGINTAX':
				case 'DATEENDTAX':
					if (!isset($_POST[$name]))
					{
						if ($type == 'DATEBEGIN')
							$date = begin_fiscalyear();
						elseif ($type == 'DATEEND')
							$date = end_fiscalyear();
						else
							$date = Today();
						if ($type == 'DATEBEGINM')
							$date = begin_month($date);
						elseif ($type == 'DATEENDM')
							$date = end_month($date);
						elseif ($type == 'DATEBEGINTAX' || $type == 'DATEENDTAX')
						{
							$row = get_company_prefs();
							$edate = add_months($date, -$row['tax_last']);
							$edate = end_month($edate);
							if ($type == 'DATEENDTAX')
								$date = $edate;
							else
							{
								$bdate = begin_month($edate);
								$bdate = add_months($bdate, -$row['tax_prd'] + 1);
								$date = $bdate;
							}
						}
					}
					else // saved parameters 2010-10-16 Joe Hunt
						$date = $_POST[$name];
					$st = "<input type='text' name='$name' value='$date'>";
					if ($use_date_picker)
					{
						$calc_image = (file_exists("$path_to_root/themes/".user_theme()."/images/cal.gif")) ? 
							"$path_to_root/themes/".user_theme()."/images/cal.gif" : "$path_to_root/themes/default/images/cal.gif";
						$st .= "<a href=\"javascript:date_picker(document.forms[0].$name);\">"
						. "	<img src='$calc_image' width='16' height='16' border='0' alt='"._('Click Here to Pick up the date')."'></a>\n";
					}	
					return $st;
					break;

				case 'YES_NO':
					return yesno_list($name);

				case 'PAYMENT_LINK':
					return payment_services($name, null);

				case 'DESTINATION':
					$sel = array(_("PDF/Printer"), "Excel");
					$def = 0;
					if (isset($def_print_destination) && $def_print_destination == 1)
						$def = 1;
					return array_selector($name, $def, $sel);

				case 'ORIENTATION':
					$sel = array(_("Portrait"), _("Landscape"));
					$def = 0;
					if (isset($def_print_orientation) && $def_print_orientation == 1)
						$def = 1;
					return array_selector($name, $def, $sel);

				case 'COMPARE':
					$sel = array(_("Accumulated"), _("Period Y-1"), _("Budget"));
					return array_selector($name, null, $sel);

				case 'GRAPHIC':
					$sel = array(_("No Graphics"), _("Vertical bars"), _("Horizontal bars"), 
								_("Dots"), _("Lines"), _("Pie"), _("Donut"));
					return array_selector($name, null, $sel);

				case 'SYS_TYPES':
					return gl_systypes_list($name, null, _("No Type Filter"));

				case 'SYS_TYPES_ALL':
					return systypes_list($name, null, _("No Type Filter"));

				case 'TEXT':
					return "<input type='text' name='$name'>";

				case 'TEXTBOX':
					$value = (isset($_POST[$name]) ? $_POST[$name] : "");
					return "<textarea rows=4 cols=30 name='$name'>$value</textarea>";

				case 'ACCOUNTS': // not used
					return gl_account_types_list($name, null, _("No Account Group Filter"), true);

				case 'ACCOUNTS_NO_FILTER': // not used
					return gl_account_types_list($name);

				case 'GL_ACCOUNTS':
					return gl_all_accounts_list($name);

				case 'BANK_ACCOUNTS':
					return bank_accounts_list($name);
					
				case 'DIMENSION':
					return dimensions_list($name, null, false, ' ', false, true, 0);
					
				case 'DIMENSIONS':
					return dimensions_list($name, null, true, _("No Dimension Filter"), false, true, 0);

				case 'DIMENSION1':
					return dimensions_list($name, null, false, ' ', false, true, 1);

				case 'DIMENSIONS1':
					return dimensions_list($name, null, true, _("No Dimension Filter"), false, true, 1);

				case 'DIMENSION2':
					return dimensions_list($name, null, false, ' ', false, true, 2);

				case 'DIMENSIONS2':
					return dimensions_list($name, null, true, _("No Dimension Filter"), false, true, 2);

				case 'CUSTOMERS_NO_FILTER':
				case 'CUSTOMERS':
					$sql = "SELECT debtor_no, name FROM ".TB_PREF."debtors_master";
					if ($type == 'CUSTOMERS_NO_FILTER')
						return customer_list($name, null, _("No Customer Filter"));
					else
						return customer_list($name);

				case 'SUPPLIERS_NO_FILTER':
				case 'SUPPLIERS':
					$sql = "SELECT supplier_id, supp_name FROM ".TB_PREF."suppliers";
					if ($type == 'SUPPLIERS_NO_FILTER')
						return supplier_list($name, null, _("No Supplier Filter"));
					else
						return supplier_list($name);

				case 'INVOICE':
					$IV = $type_shortcuts[ST_SALESINVOICE];
					$ref = ($print_invoice_no == 1 ? "trans_no" : "reference");
					$sql = "SELECT concat(".TB_PREF."debtor_trans.trans_no, '-',
						".TB_PREF."debtor_trans.type) AS TNO, concat('$IV ', ".TB_PREF."debtor_trans.$ref,' ', ".TB_PREF."debtors_master.name) as IName
						FROM ".TB_PREF."debtors_master, ".TB_PREF."debtor_trans WHERE type=".ST_SALESINVOICE." AND ".TB_PREF."debtors_master.debtor_no=".TB_PREF."debtor_trans.debtor_no ORDER BY ".TB_PREF."debtor_trans.trans_no DESC";
					return combo_input($name, '', $sql, 'TNO', 'IName',array('order'=>false));

				case 'CREDIT':
					$CN = $type_shortcuts[ST_CUSTCREDIT];
					$ref = ($print_invoice_no == 1 ? "trans_no" : "reference");
					$sql = "SELECT concat(".TB_PREF."debtor_trans.trans_no, '-',
						".TB_PREF."debtor_trans.type) AS TNO, concat('$CN ', ".TB_PREF."debtor_trans.$ref,' ', ".TB_PREF."debtors_master.name) as IName
						FROM ".TB_PREF."debtors_master, ".TB_PREF."debtor_trans WHERE type=".ST_CUSTCREDIT." AND ".TB_PREF."debtors_master.debtor_no=".TB_PREF."debtor_trans.debtor_no ORDER BY ".TB_PREF."debtor_trans.trans_no DESC";
					return combo_input($name, '', $sql, 'TNO', 'IName',array('order'=>false));

				case 'DELIVERY':
					$DN = $type_shortcuts[ST_CUSTDELIVERY];
					$sql = "SELECT
					concat(".TB_PREF."debtor_trans.trans_no, '-', ".TB_PREF."debtor_trans.type) AS TNO, concat(".TB_PREF."debtor_trans.trans_no, ' $DN ',
					 ".TB_PREF."debtors_master.name) as IName
						FROM ".TB_PREF."debtors_master, ".TB_PREF."debtor_trans
						WHERE type=".ST_CUSTDELIVERY." AND ".TB_PREF."debtors_master.debtor_no=".
						TB_PREF."debtor_trans.debtor_no ORDER BY ".TB_PREF."debtor_trans.trans_no DESC";
					return combo_input($name, '', $sql, 'TNO', 'IName',array('order'=>false));

				case 'ORDERS':
					$ref = ($print_invoice_no == 1 ? "order_no" : "reference");
					$sql = "SELECT ".TB_PREF."sales_orders.order_no, concat(".TB_PREF."sales_orders.$ref, '-',
						".TB_PREF."debtors_master.name) as IName
						FROM ".TB_PREF."debtors_master, ".TB_PREF."sales_orders WHERE ".TB_PREF."debtors_master.debtor_no=".TB_PREF."sales_orders.debtor_no 
						AND ".TB_PREF."sales_orders.trans_type=".ST_SALESORDER." ORDER BY ".TB_PREF."sales_orders.order_no DESC";
					return combo_input($name, '', $sql, 'order_no', 'IName',array('order'=>false));

				case 'QUOTATIONS':
					$ref = ($print_invoice_no == 1 ? "order_no" : "reference");
					$sql = "SELECT ".TB_PREF."sales_orders.order_no, concat(".TB_PREF."sales_orders.$ref, '-',
						".TB_PREF."debtors_master.name) as IName
						FROM ".TB_PREF."debtors_master, ".TB_PREF."sales_orders WHERE ".TB_PREF."debtors_master.debtor_no=".TB_PREF."sales_orders.debtor_no 
						AND ".TB_PREF."sales_orders.trans_type=".ST_SALESQUOTE." ORDER BY ".TB_PREF."sales_orders.order_no DESC";
					return combo_input($name, '', $sql, 'order_no', 'IName',array('order'=>false));

				case 'PO':
					$ref = ($print_invoice_no == 1 ? "order_no" : "reference");
					$sql = "SELECT ".TB_PREF."purch_orders.order_no, concat(".TB_PREF."purch_orders.$ref, '-',
						".TB_PREF."suppliers.supp_name) as IName
						FROM ".TB_PREF."suppliers, ".TB_PREF."purch_orders WHERE ".TB_PREF."suppliers.supplier_id=".TB_PREF."purch_orders.supplier_id ORDER BY ".TB_PREF."purch_orders.order_no DESC";
					return combo_input($name, '', $sql, 'order_no', 'IName',array('order'=>false));

				case 'REMITTANCE':
					$BP = $type_shortcuts[ST_BANKPAYMENT];
					$SP = $type_shortcuts[ST_SUPPAYMENT];
					$CN = $type_shortcuts[ST_SUPPCREDIT];
					$ref = ($print_invoice_no == 1 ? "trans_no" : "reference");
					$sql = "SELECT concat(".TB_PREF."supp_trans.trans_no, '-',
						".TB_PREF."supp_trans.type) AS TNO, concat(".TB_PREF."supp_trans.$ref, if (type=".ST_BANKPAYMENT.", ' $BP ', if (type=".ST_SUPPAYMENT.", ' $SP ', ' $CN ')), ".TB_PREF."suppliers.supp_name) as IName
						FROM ".TB_PREF."suppliers, ".TB_PREF."supp_trans WHERE (type=".ST_BANKPAYMENT." OR type=".ST_SUPPAYMENT." OR type=".ST_SUPPCREDIT.") AND ".TB_PREF."suppliers.supplier_id=".TB_PREF."supp_trans.supplier_id ORDER BY ".TB_PREF."supp_trans.trans_no DESC";
					return combo_input($name, '', $sql, 'TNO', 'IName',array('order'=>false));

				case 'RECEIPT':
					$BD = $type_shortcuts[ST_BANKDEPOSIT];
					$CP = $type_shortcuts[ST_CUSTPAYMENT];
					$ref = ($print_invoice_no == 1 ? "trans_no" : "reference");
					$sql = "SELECT concat(".TB_PREF."debtor_trans.trans_no, '-',
						".TB_PREF."debtor_trans.type) AS TNO, concat(".TB_PREF."debtor_trans.$ref, if (type=".ST_BANKDEPOSIT.", ' $BD ', ' $CP '), ".TB_PREF."debtors_master.name) as IName
						FROM ".TB_PREF."debtors_master, ".TB_PREF."debtor_trans WHERE (type=".ST_BANKDEPOSIT." OR type=".ST_CUSTPAYMENT." OR type=".ST_CUSTCREDIT.") AND ".TB_PREF."debtors_master.debtor_no=".TB_PREF."debtor_trans.debtor_no ORDER BY ".TB_PREF."debtor_trans.trans_no DESC";
					return combo_input($name, '', $sql, 'TNO', 'IName',array('order'=>false));

				case 'ITEMS':
					return stock_manufactured_items_list($name);

				case 'ITEMS_P':
					return stock_purchasable_items_list($name, null, true);
				
				case 'WORKORDER':
					$sql = "SELECT ".TB_PREF."workorders.id, concat(".TB_PREF."workorders.id, '-',
						".TB_PREF."stock_master.description) as IName
						FROM ".TB_PREF."stock_master, ".TB_PREF."workorders WHERE ".TB_PREF."stock_master.stock_id=".TB_PREF."workorders.stock_id ORDER BY ".TB_PREF."workorders.id DESC";
					return combo_input($name, '', $sql, 'id', 'IName',array('order'=>false));

				case 'LOCATIONS':
					return  locations_list($name, null, _("No Location Filter"));

				case 'CATEGORIES':
					return stock_categories_list($name, null, _("No Category Filter"));

				case 'SALESTYPES':
					return sales_types_list($name);

				case 'AREAS':
					return sales_areas_list($name);

				case 'SALESMEN':
					return sales_persons_list($name, null, _("No Sales Folk Filter"));
					
				case 'TRANS_YEARS':
					return fiscalyears_list($name);

				case 'USERS':
					$sql = "SELECT id, user_id FROM ".TB_PREF."users";
					return combo_input($name, '', $sql, 'id', 'user_id',array('spec_option'=>_("No Users Filter"),'spec_id'=>ALL_NUMERIC));

				case 'ACCOUNTTAGS':
				case 'DIMENSIONTAGS':
					if ($type == 'ACCOUNTTAGS')
						$tag_type = TAG_ACCOUNT;
					else
						$tag_type = TAG_DIMENSION;
					//return tag_list($name, 5, $tag_type, true, false, _("No tags"));
					return tag_list($name, 5, $tag_type, true);

			}
		return '';
	}
}

class Report
{
	var $id;
	var $name;
	var $ar_params;
	var $controls;
	
	function Report($id, $name, $ar_params = null)
	{
		$this->id = $id;
		$this->name = $name;
		if ($ar_params) $this->set_controls($ar_params);
	}
	
	function set_controls($ar_params) {
		$this->controls = $ar_params;
	}
	
	function get_controls() {
		return $this->controls;
	}
	
}

//------------------------------------------------------------------------------------------------
function gl_systypes_list($name, $value=null, $spec_opt=false)
{
	global $systypes_array;
	
	$types = $systypes_array;
	
	foreach(array(ST_LOCTRANSFER, ST_PURCHORDER, ST_SUPPRECEIVE, ST_MANUISSUE,
				ST_MANURECEIVE, ST_SALESORDER, ST_SALESQUOTE, ST_DIMENSION) as $type)
		unset($types[$type]);

	return array_selector($name, $value, $types, 
		array( 
			'spec_option'=> $spec_opt,
			'spec_id' => ALL_NUMERIC,
			'async' => false,
			)
	);
}
/*
	Add extension/custom reports to report manager.
*/
function add_custom_reports(&$reports)
{
	global $installed_extensions, $path_to_root;
	
	// include reports installed inside extension modules
	if (count($installed_extensions) > 0)
	{
		$extensions = $installed_extensions;
		foreach ($extensions as $ext)
			if (($ext['active'] && $ext['type'] == 'extension')) {
				$file = $path_to_root.'/'.$ext['path']."/reporting/reports_custom.php";
				if (file_exists($file)) {
					set_ext_domain($ext['path']);
					include_once($file);
					set_ext_domain();
				}
			}
	}
	$file = company_path()."/reporting/reports_custom.php";
	if (file_exists($file))
		include_once($file);
}

?>