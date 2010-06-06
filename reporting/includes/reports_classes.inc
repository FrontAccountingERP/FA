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
class BoxReports
{
	var $ar_classes;
	var $ctrl_handlers = array();

	function ReportClasses()
	{
		$this->ar_classes = array();
	}

	function addReportClass($class_name)
	{
		$this->ar_classes[$class_name] = array();
	}

	function addReport($class_name, $id, $rep_name, $params=null)
	{
		unset($this->ar_classes[$class_name][$id]); // unset std report if any
		$this->ar_classes[$class_name][$id] = new Report($id, $rep_name, $params);
	}

	function getDisplay($class=null)
	{
		global $table_style, $comp_path, $path_to_root, $pdf_debug, $Ajax;


		$temp = array_values($this->ar_classes);
		$display_class = $class==null ? $temp[0] : $this->ar_classes[$class];
		$class_counter = 0;
		$rep_counter = 0;
		$st_reports = "";
		$st_params = "";
		$st_classes = "<b>" . _("Report Classes:") . "</b><br>";
		foreach($this->ar_classes as $key=>$value)
		{
			$style = $class_counter==$_REQUEST['Class'] ? '' : "style='display:none'";
			$acc = access_string($key);
			$st_classes .= "<a href='"
				.$_SERVER['PHP_SELF']."?Class=$class_counter'"
				." class='menu_option' id='".default_focus()."'"
				." onclick='return showClass($class_counter);'$acc[1]>$acc[0]</a> <br>";
			$st_reports .= "<table id='TAB_" . $class_counter ."' $style cellpadding=0 cellspacing=0 width='100%'><tr><td><b>" . _("Reports For Class: ") . "&nbsp;$key</b></td></tr>\n";
			foreach($value as $report)
			{
				$acc = access_string($report->name);
				$st_reports .= "<tr><td><a class='printlink' href='"
					.$_SERVER['PHP_SELF']."?Class=$class_counter&rep_id=$report->id'"
					." id='".default_focus()."'"
					."$acc[1]>$acc[0]</a><tr><td>\n";
				if (isset($_REQUEST['rep_id']) && $_REQUEST['rep_id']==$report->id) {

					$action = $path_to_root.'/reporting/prn_redirect.php';
	
					$st_params = "<table><tr><td>\n"
						. "<form method='POST' action='$action' target='_blank'>\n";
					$st_params .= submit('Rep'.$report->id,  
						_("Display: ") . access_string($report->name, true),
						false, '', $pdf_debug ? false : 'default process') . hidden('REP_ID', $report->id, false).'<br><br>';
					$st_params .= $this->getOptions($report->get_controls());
					$st_params .= "\n</form></td></tr></table>\n";
					set_focus('Rep'.$report->id);
					$Ajax->addUpdate(true, 'rep_form', $st_params);
				}
			}
			$st_reports .= "</table>";
			$class_counter++;
		}

		$st_params = "<div id='rep_form'>".
			"$st_params</div>";
		

		$st = 	"<script language='javascript'>
					function showClass(pClass) {
						for(i=0; i<$class_counter; i++) {
							document.getElementById(\"TAB_\" + i).style.display=
							i==pClass ? \"block\" : \"none\";
						}
						document.getElementById('rep_form').innerHTML = '';
//						document.getElementById('rep_form').style.display = 'none';
						return false;
					}
					function checkDate(pObj) {
						var re = /^(3[01]|0[1-9]|[12]\d)\/(0[1-9]|1[012])\/\d{4}/;
						if (re.test(pObj.value)==false) {
							alert('" . _("Invalid date format") . "')
						}
					}
				</script>
				";
		$st .= "<table align='center' width='80%' style='border:1px solid #cccccc;'><tr valign='top'>";
		$st .= "<td width='30%'>$st_classes</td>";
		$st .= "<td width='35%' style='border-left:1px solid #cccccc;border-right:1px solid #cccccc;padding-left:3px;'>$st_reports</td>";
		$st .= "<td width='35%'>$st_params</td>";
		$st .= "</tr></table><br>";

		return $st;
	}

	function getOptions($controls)
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
		global $path_to_root, $use_date_picker, $pdf_debug, $print_invoice_no, $def_print_destination;

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

					$st = "<input type='text' name='$name' value='$date'>";
					if ($use_date_picker)
						$st .= "<a href=\"javascript:date_picker(document.forms[0].$name);\">"
						. "	<img src='$path_to_root/themes/default/images/cal.gif' width='16' height='16' border='0' alt='"._('Click Here to Pick up the date')."'></a>\n";
					return $st;
					break;

				case 'YES_NO':
					return yesno_list($name);

				case 'PAYMENT_LINK':
					$sel = array(_("No payment Link"), "PayPal");
					return array_selector($name, null, $sel);

				case 'DESTINATION':
					$sel = array(_("PDF/Printer"), "Excel");
					$def = 0;
					if (isset($def_print_destination) && $def_print_destination == 1)
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
					return "<textarea rows=4 cols=30 name='$name'></textarea>";

				case 'ACCOUNTS': // not used
//					$sql = "SELECT id, name FROM ".TB_PREF."chart_types";
//					return combo_input($name, '', $sql, 'id', 'name',array('spec_option'=>_("No Account Group Filter"),'spec_id'=>ALL_NUMERIC));
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
						return combo_input($name, '', $sql, 'debtor_no', 'name',array('spec_option'=>_("No Customer Filter"),'spec_id'=>ALL_NUMERIC));
// FIX allitems numeric!
//						return customer_list($name, null, _("No Customer Filter"));
					else
						return combo_input($name, '', $sql, 'debtor_no', 'name', null);
//						return customer_list($name);

				case 'SUPPLIERS_NO_FILTER':
				case 'SUPPLIERS':
					$sql = "SELECT supplier_id, supp_name FROM ".TB_PREF."suppliers";
					if ($type == 'SUPPLIERS_NO_FILTER')
						return combo_input($name, '', $sql, 'supplier_id', 'supp_name',array('spec_option'=>_("No Supplier Filter"),'spec_id'=>ALL_NUMERIC));
// FIX allitems numeric!
//						return supplier_list($name, null, _("No Supplier Filter"));
					else
						return combo_input($name, '', $sql, 'supplier_id', 'supp_name', null);
//						return supplier_list($name);

				case 'INVOICE':
					$IV = _("IV");
					$CN = _("CN");
					$ref = ($print_invoice_no == 1 ? "trans_no" : "reference");
					$sql = "SELECT concat(".TB_PREF."debtor_trans.trans_no, '-',
						".TB_PREF."debtor_trans.type) AS TNO, concat(".TB_PREF."debtor_trans.$ref, if (type=".ST_SALESINVOICE.", ' $IV ', ' $CN '), ".TB_PREF."debtors_master.name) as IName
						FROM ".TB_PREF."debtors_master, ".TB_PREF."debtor_trans WHERE (type=".ST_SALESINVOICE." OR type=".ST_CUSTCREDIT.") AND ".TB_PREF."debtors_master.debtor_no=".TB_PREF."debtor_trans.debtor_no ORDER BY ".TB_PREF."debtor_trans.trans_no DESC";
					return combo_input($name, '', $sql, 'TNO', 'IName',array('order'=>false));

				case 'DELIVERY':
					$DN = _("DN");
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
					$BP = _("BP");
					$SP = _("SP");
					$CN = _("CN");
					$ref = ($print_invoice_no == 1 ? "trans_no" : "reference");
					$sql = "SELECT concat(".TB_PREF."supp_trans.trans_no, '-',
						".TB_PREF."supp_trans.type) AS TNO, concat(".TB_PREF."supp_trans.$ref, if (type=".ST_BANKPAYMENT.", ' $BP ', if (type=".ST_SUPPAYMENT.", ' $SP ', ' $CN ')), ".TB_PREF."suppliers.supp_name) as IName
						FROM ".TB_PREF."suppliers, ".TB_PREF."supp_trans WHERE (type=".ST_BANKPAYMENT." OR type=".ST_SUPPAYMENT." OR type=".ST_SUPPCREDIT.") AND ".TB_PREF."suppliers.supplier_id=".TB_PREF."supp_trans.supplier_id ORDER BY ".TB_PREF."supp_trans.trans_no DESC";
					return combo_input($name, '', $sql, 'TNO', 'IName',array('order'=>false));

				case 'RECEIPT':
					$BD = _("BD");
					$CP = _("CP");
					$CN = _("CN");
					$ref = ($print_invoice_no == 1 ? "trans_no" : "reference");
					$sql = "SELECT concat(".TB_PREF."debtor_trans.trans_no, '-',
						".TB_PREF."debtor_trans.type) AS TNO, concat(".TB_PREF."debtor_trans.$ref, if (type=".ST_BANKDEPOSIT.", ' $BD ', if (type=".ST_CUSTPAYMENT.", ' $CP ', ' $CN ')), ".TB_PREF."debtors_master.name) as IName
						FROM ".TB_PREF."debtors_master, ".TB_PREF."debtor_trans WHERE (type=".ST_BANKDEPOSIT." OR type=".ST_CUSTPAYMENT." OR type=".ST_CUSTCREDIT.") AND ".TB_PREF."debtors_master.debtor_no=".TB_PREF."debtor_trans.debtor_no ORDER BY ".TB_PREF."debtor_trans.trans_no DESC";
					return combo_input($name, '', $sql, 'TNO', 'IName',array('order'=>false));

				case 'ITEMS':
					return stock_manufactured_items_list($name);

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
					return tag_list($name, 5, $tag_type, true, _("No tags"));

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

function add_custom_reports(&$reports)
{
	global $installed_extensions, $path_to_root, $comp_path;
	
	// include reports installed inside extension modules
	if (count($installed_extensions) > 0)
	{
		$extensions = $installed_extensions;
		foreach ($extensions as $ext)
			if (($ext['active'] && $ext['type'] == 'module')) {
				$file = $path_to_root.'/'.$ext['path']."/reporting/reports_custom.php";
				if (file_exists($file))
					include_once($file);
			}
	}
	$file = $comp_path.'/'.user_company()."/reporting/reports_custom.php";
	if (file_exists($file))
		include_once($file);
}

?>