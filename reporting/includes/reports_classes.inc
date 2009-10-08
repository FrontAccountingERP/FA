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
		$this->ar_classes[$class_name][] = new Report($id,$rep_name,$params);
	}

	function getDisplay($class=null)
	{
		global $table_style2, $comp_path, $path_to_root, $pdf_debug, $Ajax;

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
			$st_reports .= "<table id='TAB_" . $class_counter ."' $style cellpadding=0 cellspacing=0 border=0 width='100%'><tr><td><b>" . _("Reports For Class: ") . "&nbsp;$key</b></td></tr>\n";
			foreach($value as $report)
			{
				$acc = access_string($report->name);
				$st_reports .= "<tr><td><a class='printlink' href='"
					.$_SERVER['PHP_SELF']."?Class=$class_counter&rep_id=$report->id'"
					." id='".default_focus()."'"
					."$acc[1]>$acc[0]</a><tr><td>\n";
				if (isset($_REQUEST['rep_id']) && $_REQUEST['rep_id']==$report->id) {

					$action = $path_to_root.'/reporting/prn_redirect.php';
	
					$st_params = "<table border=0><tr><td>\n"
						. "<form method='POST' action='$action' target='_blank'>\n";
					$st_params .= hidden('REP_ID', $report->id, false);
					$st_params .= submit('Rep'.$report->id,  
						_("Display: ") . access_string($report->name, true),
						false, '', $pdf_debug ? false : 'default') . '<br><br>';
					$st_params .= $report->getDisplay() 
						. "\n</form></td></tr></table>\n";
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
		$st .= "<table align='center' width='80%' $table_style2><tr valign='top'>";
		$st .= "<td width='30%'>$st_classes</td>";
		$st .= "<td width='35%'>$st_reports</td>";
		$st .= "<td width='35%'>$st_params</td>";
		$st .= "</tr></table><br>";

		return $st;
	}
}

class Report
{
	var $id;
	var $name;
	var $ar_params;

	function Report($id, $name, $ar_params)
	{
		$this->id 			= $id;
		$this->name 		= $name;
		$this->ar_params	= $ar_params;
	}
	
	function getDisplay()
	{
		global $path_to_root, $use_date_picker, $pdf_debug, $print_invoice_no;

		$st = '';
		$dummy = "";
		if ($this->ar_params==null)
			return "";
		foreach($this->ar_params as $index=>$param)
		{
			$st .= $param->param_name . ':<br>';
			switch ($param->param_type)
			{
				case 'CURRENCY':
					$sql = "SELECT curr_abrev, concat(curr_abrev,' - ', currency) AS name FROM ".TB_PREF."currencies";
					$st .= combo_input("PARAM_$index", '', $sql, 'curr_abrev', 'name',array('spec_option'=>_("No Currency Filter"),'spec_id'=>ALL_TEXT,'order'=>false));
					break;
				case 'DATE':
				case 'DATEBEGIN':
				case 'DATEEND':
				case 'DATEBEGINM':
				case 'DATEENDM':
				case 'DATEBEGINTAX':
				case 'DATEENDTAX':
					if ($param->param_type == 'DATEBEGIN')
						$date = begin_fiscalyear();
					elseif ($param->param_type == 'DATEEND')
						$date = end_fiscalyear();
					else
						$date = Today();
					if ($param->param_type == 'DATEBEGINM')
						$date = begin_month($date);
					elseif ($param->param_type == 'DATEENDM')
						$date = end_month($date);
					elseif ($param->param_type == 'DATEBEGINTAX' || $param->param_type == 'DATEENDTAX')
					{
						$row = get_company_prefs();
						$edate = add_months($date, -$row['tax_last']);
						$edate = end_month($edate);
						if ($param->param_type == 'DATEENDTAX')
							$date = $edate;
						else
						{
							$bdate = begin_month($edate);
							$bdate = add_months($bdate, -$row['tax_prd'] + 1);
							$date = $bdate;
						}
					}
					$name = "PARAM_$index";

					$st .= "<input type='text' name='$name' value='$date'>";
					if ($use_date_picker)
						$st .= "<a href=\"javascript:date_picker(document.forms[0].$name);\">"
						. "	<img src='$path_to_root/themes/default/images/cal.gif' width='16' height='16' border='0' alt='"._('Click Here to Pick up the date')."'></a>\n";

					break;
				case 'YES_NO':
					$sel = array(_('No'), _("Yes"));
					$st .= dup_simple_name_list("PARAM_$index", $sel);
					break;
				case 'PAYMENT_LINK':
					$sel = array(_("No Payment Link"), "PayPal");
					$st .= dup_simple_name_list("PARAM_$index", $sel);
					break;
				case 'DESTINATION':
					$sel = array(_("PDF/Printer"), "Excel");
					$st .= dup_simple_name_list("PARAM_$index", $sel);
					break;
				case 'COMPARE':
					$sel = array(_("Accumulated"), _("Period Y-1"), _("Budget"));
					$st .= dup_simple_name_list("PARAM_$index", $sel);
					break;
				case 'GRAPHIC':
					$sel = array(_("No Graphics"), _("Vertical bars"), _("Horizontal bars"), _("Dots"), _("Lines"), _("Pie"), _("Donut"));
					$st .= dup_simple_name_list("PARAM_$index", $sel);
					break;
				case 'SYS_TYPES':
				case 'SYS_TYPES_ALL':
					$st .= dup_systypes_list("PARAM_$index", $dummy, true, _("No Type Filter"), ($param->param_type=='SYS_TYPES_ALL'));
					break;
				case 'TEXT':
					$st .= "<input type='text' name='PARAM_$index'>";
					break;
				case 'TEXTBOX':
					$st .= "<textarea rows=4 cols=30 name='PARAM_$index'></textarea>";
					break;
				case 'ACCOUNTS':
					$sql = "SELECT id, name FROM ".TB_PREF."chart_types";
					$st .= combo_input("PARAM_$index", '', $sql, 'id', 'name',array('spec_option'=>_("No Account Group Filter"),'spec_id'=>ALL_NUMERIC));
					break;
				case 'GL_ACCOUNTS':
					$sql = "SELECT account_code, concat(account_code, ' - ', account_name) as account_name FROM ".TB_PREF."chart_master";
					$st .= combo_input("PARAM_$index", '', $sql, 'account_code', 'account_name',array('order'=>array('account_code')));
					break;
				case 'BANK_ACCOUNTS':
					$sql = "SELECT ".TB_PREF."bank_accounts.id, concat(bank_account_name, if (bank_curr_code=curr_default,'', concat(' - ', bank_curr_code))) AS name FROM ".TB_PREF."bank_accounts, ".TB_PREF."company";
					$st .= combo_input("PARAM_$index", '', $sql, 'id', 'name',array('order'=>array('id')));
					break;
				case 'DIMENSION':
					$sql = "SELECT id, concat(reference, ' - ', name) AS DimName FROM ".TB_PREF."dimensions";
					$st .= combo_input("PARAM_$index", '', $sql, 'id', 'DimName',array('order'=>array('reference')));
					break;
				case 'DIMENSIONS':
					$sql = "SELECT id, concat(reference, ' - ', name) as DimName FROM ".TB_PREF."dimensions";
					$st .= combo_input("PARAM_$index", '', $sql, 'id', 'DimName',array('spec_option'=>_("No Dimension Filter"),'spec_id'=>ALL_NUMERIC,'order'=>array('reference')));
					break;
				case 'DIMENSION1':
					$sql = "SELECT id, concat(reference, ' - ', name) AS DimName FROM ".TB_PREF."dimensions WHERE type_=1";
					$st .= combo_input("PARAM_$index", '', $sql, 'id', 'DimName',array('order'=>array('reference')));
					break;
				case 'DIMENSIONS1':
					$sql = "SELECT id, concat(reference, ' - ', name) as DimName FROM ".TB_PREF."dimensions WHERE type_=1";
					$st .= combo_input("PARAM_$index", '', $sql, 'id', 'DimName',array('spec_option'=>_("No Dimension Filter"),'spec_id'=>ALL_NUMERIC,'order'=>array('reference')));
					break;
				case 'DIMENSION2':
					$sql = "SELECT id, concat(reference, ' - ', name) AS DimName FROM ".TB_PREF."dimensions WHERE type_=2";
					$st .= combo_input("PARAM_$index", '', $sql, 'id', 'DimName',array('order'=>array('reference')));
					break;
				case 'DIMENSIONS2':
					$sql = "SELECT id, concat(reference, ' - ', name) as DimName FROM ".TB_PREF."dimensions WHERE type_=2";
					$st .= combo_input("PARAM_$index", '', $sql, 'id', 'DimName',array('spec_option'=>_("No Dimension Filter"),'spec_id'=>ALL_NUMERIC,'order'=>array('reference')));
					break;
				case 'CUSTOMERS_NO_FILTER':
				case 'CUSTOMERS':
					$sql = "SELECT debtor_no, name FROM ".TB_PREF."debtors_master";
					if ($param->param_type == 'CUSTOMERS_NO_FILTER')
						$st .= combo_input("PARAM_$index", '', $sql, 'debtor_no', 'name',array('spec_option'=>_("No Customer Filter"),'spec_id'=>ALL_NUMERIC));
					else
						$st .= combo_input("PARAM_$index", '', $sql, 'debtor_no', 'name', null);
					break;
				case 'SUPPLIERS_NO_FILTER':
				case 'SUPPLIERS':
					$sql = "SELECT supplier_id, supp_name FROM ".TB_PREF."suppliers";
					if ($param->param_type == 'SUPPLIERS_NO_FILTER')
						$st .= combo_input("PARAM_$index", '', $sql, 'supplier_id', 'supp_name',array('spec_option'=>_("No Supplier Filter"),'spec_id'=>ALL_NUMERIC));
					else
						$st .= combo_input("PARAM_$index", '', $sql, 'supplier_id', 'supp_name', null);
					break;
				case 'INVOICE':
					$IV = _("IV");
					$CN = _("CN");
					if ($print_invoice_no == 1)
						$sql = "SELECT concat(".TB_PREF."debtor_trans.trans_no, '-',
							".TB_PREF."debtor_trans.type) AS TNO, concat(".TB_PREF."debtor_trans.trans_no, if (type=".ST_SALESINVOICE.", ' $IV ', ' $CN '), ".TB_PREF."debtors_master.name) as IName
							FROM ".TB_PREF."debtors_master, ".TB_PREF."debtor_trans WHERE (type=".ST_SALESINVOICE." OR type=".ST_CUSTCREDIT.") AND ".TB_PREF."debtors_master.debtor_no=".TB_PREF."debtor_trans.debtor_no ORDER BY ".TB_PREF."debtor_trans.trans_no DESC";
					else		
						$sql = "SELECT concat(".TB_PREF."debtor_trans.trans_no, '-',
							".TB_PREF."debtor_trans.type) AS TNO, concat(".TB_PREF."debtor_trans.reference, if (type=".ST_SALESINVOICE.", ' $IV ', ' $CN '), ".TB_PREF."debtors_master.name) as IName
							FROM ".TB_PREF."debtors_master, ".TB_PREF."debtor_trans WHERE (type=".ST_SALESINVOICE." OR type=".ST_CUSTCREDIT.") AND ".TB_PREF."debtors_master.debtor_no=".TB_PREF."debtor_trans.debtor_no ORDER BY ".TB_PREF."debtor_trans.trans_no DESC";
					$st .= combo_input("PARAM_$index", '', $sql, 'TNO', 'IName',array('order'=>false));
					break;
				case 'DELIVERY':
					$DN = _("DN");
					$sql = "SELECT
					concat(".TB_PREF."debtor_trans.trans_no, '-', ".TB_PREF."debtor_trans.type) AS TNO, concat(".TB_PREF."debtor_trans.trans_no, ' $DN ',
					 ".TB_PREF."debtors_master.name) as IName
						FROM ".TB_PREF."debtors_master, ".TB_PREF."debtor_trans
						WHERE type=".ST_CUSTDELIVERY." AND ".TB_PREF."debtors_master.debtor_no=".
						TB_PREF."debtor_trans.debtor_no ORDER BY ".TB_PREF."debtor_trans.trans_no DESC";
					$st .= combo_input("PARAM_$index", '', $sql, 'TNO', 'IName',array('order'=>false));
					break;
				case 'ORDERS':
					$sql = "SELECT ".TB_PREF."sales_orders.order_no, concat(".TB_PREF."sales_orders.order_no, '-',
						".TB_PREF."debtors_master.name) as IName
						FROM ".TB_PREF."debtors_master, ".TB_PREF."sales_orders WHERE ".TB_PREF."debtors_master.debtor_no=".TB_PREF."sales_orders.debtor_no 
						AND ".TB_PREF."sales_orders.trans_type=".ST_SALESORDER." ORDER BY ".TB_PREF."sales_orders.order_no DESC";
					$st .= combo_input("PARAM_$index", '', $sql, 'order_no', 'IName',array('order'=>false));
					break;
				case 'QUOTATIONS':
					$sql = "SELECT ".TB_PREF."sales_orders.order_no, concat(".TB_PREF."sales_orders.order_no, '-',
						".TB_PREF."debtors_master.name) as IName
						FROM ".TB_PREF."debtors_master, ".TB_PREF."sales_orders WHERE ".TB_PREF."debtors_master.debtor_no=".TB_PREF."sales_orders.debtor_no 
						AND ".TB_PREF."sales_orders.trans_type=".ST_SALESQUOTE." ORDER BY ".TB_PREF."sales_orders.order_no DESC";
					$st .= combo_input("PARAM_$index", '', $sql, 'order_no', 'IName',array('order'=>false));
					break;
				case 'PO':
					$sql = "SELECT ".TB_PREF."purch_orders.order_no, concat(".TB_PREF."purch_orders.order_no, '-',
						".TB_PREF."suppliers.supp_name) as IName
						FROM ".TB_PREF."suppliers, ".TB_PREF."purch_orders WHERE ".TB_PREF."suppliers.supplier_id=".TB_PREF."purch_orders.supplier_id ORDER BY ".TB_PREF."purch_orders.order_no DESC";
					$st .= combo_input("PARAM_$index", '', $sql, 'order_no', 'IName',array('order'=>false));
					break;
				case 'ITEMS':
					$sql = "SELECT stock_id, concat(stock_id, '-', description) as name FROM ".TB_PREF."stock_master WHERE (mb_flag='A' OR mb_flag='M')";
					$st .= combo_input("PARAM_$index", '', $sql, 'stock_id', 'name',array('order'=>array('stock_id')));
					break;
				case 'WORKORDER':
					$sql = "SELECT ".TB_PREF."workorders.id, concat(".TB_PREF."workorders.id, '-',
						".TB_PREF."stock_master.description) as IName
						FROM ".TB_PREF."stock_master, ".TB_PREF."workorders WHERE ".TB_PREF."stock_master.stock_id=".TB_PREF."workorders.stock_id ORDER BY ".TB_PREF."workorders.id DESC";
					$st .= combo_input("PARAM_$index", '', $sql, 'id', 'IName',array('order'=>false));
					break;
				case 'LOCATIONS':
					$sql = "SELECT loc_code, location_name FROM ".TB_PREF."locations";
					$st .= combo_input("PARAM_$index", '', $sql, 'loc_code', 'location_name',array('spec_option'=>_("No Location Filter"),'spec_id'=>ALL_TEXT));
					break;
				case 'CATEGORIES':
					$sql = "SELECT category_id, description FROM ".TB_PREF."stock_category";
					$st .= combo_input("PARAM_$index", '', $sql, 'category_id', 'description',array('spec_option'=>_("No Category Filter"),'spec_id'=>ALL_NUMERIC));
					break;
				case 'SALESTYPES':
					$sql = "SELECT id, sales_type FROM ".TB_PREF."sales_types";
					$st .= combo_input("PARAM_$index", '', $sql, 'id', 'sales_type', null);
					break;
				case 'AREAS':
					$sql = "SELECT area_code, description FROM ".TB_PREF."areas";
					$st .= combo_input("PARAM_$index", '', $sql, 'area_code', 'description',array('spec_option'=>_("No Area Filter"),'spec_id'=>ALL_NUMERIC));
					break;
				case 'SALESMEN':
					$sql = "SELECT salesman_code, salesman_name FROM ".TB_PREF."salesman";
					$st .= combo_input("PARAM_$index", '', $sql, 'salesman_code', 'salesman_name',array('spec_option'=>_("No Sales Folk Filter"),'spec_id'=>ALL_NUMERIC));
					break;
				case 'TRANS_YEARS':
					$sql = "SELECT DISTINCT YEAR(tran_date) AS tran_date, YEAR(tran_date) AS tran_date2 FROM ".TB_PREF."gl_trans";
					$st .= combo_input("PARAM_$index", '', $sql, 'tran_date', 'tran_date2',array('order'=>array('tran_date')));
					break;
				case 'USERS':
					$sql = "SELECT id, user_id FROM ".TB_PREF."users";
					$st .= combo_input("PARAM_$index", '', $sql, 'id', 'user_id',array('spec_option'=>_("No Users Filter"),'spec_id'=>ALL_NUMERIC));
					break;
				case 'ACCOUNTS_NO_FILTER':
					$sql = "SELECT id, name FROM ".TB_PREF."chart_types";
					$st .= combo_input("PARAM_$index", '', $sql, 'id', 'name', null);
					break;
				case 'ACCOUNTTAGS':
				case 'DIMENSIONTAGS':
					if ($param->param_type == 'ACCOUNTTAGS')
						$type = 0;
					else
						$type = 1;
					$sql = "SELECT id, name FROM ".TB_PREF."tags WHERE type=$type"; 
					$st .= combo_input("PARAM_$index", '', $sql, 'id', 'name',array('spec_option'=>_("No tags"),'spec_id' => ALL_NUMERIC,'multi'=>true,'height'=>4));
					break;

			}
			$st .= "<br><br>";
		}
		return $st;
	}
}

class ReportParam
{
	var $param_name;
	var $param_type;

	function ReportParam($param_name, $param_type)
	{
		$this->param_name = $param_name;
		$this->param_type = $param_type;
	}
}

function dup_simple_name_list($name, $selections)
{
	$st = "<select name='$name'>";
	for ($i = 0; $i < count($selections); $i++)
		$st .= "<option value='" . $i . "'>" . $selections[$i] . "</option>\n";
	$st .= "</select>";
	return $st;
}

//------------------------------------------------------------------------------------------------

function dup_systypes_list($name, &$selected_id, $all_option=false, $all_option_name=null, $all_types=false)
{
	global $systypes_array;

   	$st = "<select name='$name'>";
	if ($all_option == true)
	{
		$reserved_word = ALL_NUMERIC;
    	if ($reserved_word == $selected_id)
    	     $st .= "<option selected value='" . $reserved_word . "'>" . $all_option_name . "</option>\n";
    	else
    	     $st .= "<option value='" . $reserved_word . "'>" . $all_option_name . "</option>\n";
		if ($selected_id == "")
			$selected_id = $reserved_word;
	}

	foreach ($systypes_array as $i => $value)
	{
		if (!$all_types && ($i == ST_LOCTRANSFER || $i == ST_PURCHORDER || $i == ST_SUPPRECEIVE || ($i >= ST_MANUISSUE && $i <= ST_SALESQUOTE) || 
			$i >= ST_DIMENSION))
			continue;
    	$st .= "<option" . ($selected_id == $i ? " selected" : "") . " value='$i'>" . $value . "</option>\n";
	}
    $st .= "</select>";
	return $st;
}


?>