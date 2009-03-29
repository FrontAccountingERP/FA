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
		global $path_to_root, $use_date_picker, $pdf_debug;

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
					$sql = "SELECT curr_abrev, concat(curr_abrev,' - ', currency) FROM ".TB_PREF."currencies";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy, true, _("No Currency Filter"));
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
							$bdate = add_months($edate, -$row['tax_prd'] + 1);
							$date = begin_month($bdate);
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
					$st .= dup_systypes_list("PARAM_$index", $dummy, true, _("No Type Filter"), true);
					break;
				case 'TEXT':
					$st .= "<input type='text' name='PARAM_$index'>";
					break;
				case 'TEXTBOX':
					$st .= "<textarea rows=4 cols=30 name='PARAM_$index'></textarea>";
					break;
				case 'ACCOUNTS':
					$sql = "SELECT id, name FROM ".TB_PREF."chart_types ORDER BY name";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy, true, _("No Account Group Filter"), true);
					break;
				case 'GL_ACCOUNTS':
					$sql = "SELECT account_code, concat(account_code, ' - ', account_name) as account_name FROM ".TB_PREF."chart_master ORDER BY account_code";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy);
					break;
				case 'BANK_ACCOUNTS':
					$sql = "SELECT ".TB_PREF."bank_accounts.id, concat(bank_account_name, if (bank_curr_code=curr_default,'', concat(' - ', bank_curr_code))) FROM ".TB_PREF."bank_accounts, ".TB_PREF."company";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy);
					break;
				case 'DIMENSION':
					$sql = "SELECT reference, concat(reference, ' - ', name) AS DimName FROM ".TB_PREF."dimensions ORDER BY reference";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy);
					break;
				case 'DIMENSIONS':
					$sql = "SELECT reference, concat(reference, ' - ', name) as DimName FROM ".TB_PREF."dimensions ORDER BY reference";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy, true, _("No Dimension Filter"), true);
					break;
				case 'DIMENSION1':
					$sql = "SELECT reference, concat(reference, ' - ', name) AS DimName FROM ".TB_PREF."dimensions WHERE type_=1 ORDER BY reference";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index". "_$index", $dummy);
					break;
				case 'DIMENSIONS1':
					$sql = "SELECT reference, concat(reference, ' - ', name) as DimName FROM ".TB_PREF."dimensions WHERE type_=1 ORDER BY reference";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy, true, _("No Dimension Filter"), true);
					break;
				case 'DIMENSION2':
					$sql = "SELECT reference, concat(reference, ' - ', name) AS DimName FROM ".TB_PREF."dimensions WHERE type_=2 ORDER BY reference";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy);
					break;
				case 'DIMENSIONS2':
					$sql = "SELECT reference, concat(reference, ' - ', name) as DimName FROM ".TB_PREF."dimensions WHERE type_=2 ORDER BY reference";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy, true, _("No Dimension Filter"), true);
					break;
				case 'CUSTOMERS_NO_FILTER':
				case 'CUSTOMERS':
					$sql = "SELECT debtor_no, name FROM ".TB_PREF."debtors_master ORDER BY name";
					if ($param->param_type == 'CUSTOMERS_NO_FILTER')
						$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy, true, _("No Customer Filter"), true);
					else
						$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy);
					break;
				case 'SUPPLIERS_NO_FILTER':
				case 'SUPPLIERS':
					$sql = "SELECT supplier_id, supp_name FROM ".TB_PREF."suppliers ORDER BY supp_name";
					if ($param->param_type == 'SUPPLIERS_NO_FILTER')
						$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy, true, _("No Supplier Filter"), true);
					else
						$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy);
					break;
				case 'INVOICE':
					$IV = _("IV");
					$CN = _("CN");
					$sql = "SELECT concat(".TB_PREF."debtor_trans.trans_no, '-',
						".TB_PREF."debtor_trans.type) AS TNO, concat(".TB_PREF."debtor_trans.trans_no, if (type=10, ' $IV ', ' $CN '), ".TB_PREF."debtors_master.name) as IName
						FROM ".TB_PREF."debtors_master, ".TB_PREF."debtor_trans WHERE (type=10 OR type=11) AND ".TB_PREF."debtors_master.debtor_no=".TB_PREF."debtor_trans.debtor_no ORDER BY ".TB_PREF."debtor_trans.trans_no DESC";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy);
					break;
				case 'DELIVERY':
					$DN = _("DN");
					$sql = "SELECT
					concat(".TB_PREF."debtor_trans.trans_no, '-', ".TB_PREF."debtor_trans.type) AS TNO, concat(".TB_PREF."debtor_trans.trans_no, ' $DN ',
					 ".TB_PREF."debtors_master.name) as IName
						FROM ".TB_PREF."debtors_master, ".TB_PREF."debtor_trans
						WHERE type=13 AND ".TB_PREF."debtors_master.debtor_no=".
						TB_PREF."debtor_trans.debtor_no ORDER BY ".TB_PREF."debtor_trans.trans_no DESC";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy);
					break;
				case 'ORDERS':
					$sql = "SELECT ".TB_PREF."sales_orders.order_no, concat(".TB_PREF."sales_orders.order_no, '-',
						".TB_PREF."debtors_master.name) as IName
						FROM ".TB_PREF."debtors_master, ".TB_PREF."sales_orders WHERE ".TB_PREF."debtors_master.debtor_no=".TB_PREF."sales_orders.debtor_no ORDER BY ".TB_PREF."sales_orders.order_no DESC";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy);
					break;
				case 'PO':
					$sql = "SELECT ".TB_PREF."purch_orders.order_no, concat(".TB_PREF."purch_orders.order_no, '-',
						".TB_PREF."suppliers.supp_name) as IName
						FROM ".TB_PREF."suppliers, ".TB_PREF."purch_orders WHERE ".TB_PREF."suppliers.supplier_id=".TB_PREF."purch_orders.supplier_id ORDER BY ".TB_PREF."purch_orders.order_no DESC";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy);
					break;
				case 'ITEMS':
					$sql = "SELECT stock_id, concat(stock_id, '-', description) as name FROM ".TB_PREF."stock_master WHERE (mb_flag='A' OR mb_flag='M') ORDER BY stock_id";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy);
					break;
				case 'LOCATIONS':
					$sql = "SELECT loc_code, location_name FROM ".TB_PREF."locations ORDER BY location_name";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy, true, _("No Location Filter"), false);
					break;
				case 'CATEGORIES':
					$sql = "SELECT category_id, description FROM ".TB_PREF."stock_category ORDER BY description";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy, true, _("No Category Filter"), true);
					break;
				case 'SALESTYPES':
					$sql = "SELECT id, sales_type FROM ".TB_PREF."sales_types ORDER BY sales_type";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy);
					break;
				case 'AREAS':
					$sql = "SELECT area_code, description FROM ".TB_PREF."areas ORDER BY description";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy, true, _("No Area Filter"), true);
					break;
				case 'SALESMEN':
					$sql = "SELECT salesman_code, salesman_name FROM ".TB_PREF."salesman ORDER BY salesman_name";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy, true, _("No Sales Folk Filter"), true);
					break;
				case 'TRANS_YEARS':
					$sql = "SELECT DISTINCT YEAR(tran_date), YEAR(tran_date) FROM ".TB_PREF."gl_trans";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy);
					break;
				case 'ACCOUNTS_NO_FILTER':
					$sql = "SELECT id, name FROM ".TB_PREF."chart_types ORDER BY name";
					$st .= dup_simple_codeandname_list($sql, "PARAM_$index", $dummy);
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

function dup_simple_codeandname_list($sql, $name, &$selected_id,
	$all_option=false, $all_option_name=null, $all_option_numeric=false,
	$submit_on_change=false)
{
	if ($submit_on_change == true)
		$st = "<select name='$name' onchange='this.form.submit();'>";
	else
		$st = "<select name='$name'>";

	if ($all_option == true)
	{
		if ($all_option_numeric)
			$reserved_word = reserved_words::get_all_numeric();
		else
			$reserved_word = reserved_words::get_all();

    	if ($reserved_word == $selected_id)
    	{
    	     $st .= "<option selected value='" . $reserved_word . "'>" . $all_option_name . "</option>\n";
    	}
    	else
    	{
    	     $st .= "<option value='" . $reserved_word . "'>" . $all_option_name . "</option>\n";
    	}
		if ($selected_id == "")
		{
			$selected_id =  $reserved_word;
		}
	}

	$result = db_query($sql);

	while ($row = db_fetch_row($result))
	{
		if ($selected_id == $row[0])
		{
			$st .= "<option selected value='" . $row[0] . "'>";
		}
		else
		{
			$st .= "<option value='" . $row[0] . "'>";
		}
		$st .= $row[1] . "</option>\n";

		if ($selected_id == "")
		{
			$selected_id = $row[0];
		}
	}

	$st .= "</select>";
	db_free_result($result);

	return $st;
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

function dup_systypes_list($name, &$selected_id, $all_option=false, $all_option_name=null)
{
	global $systypes_array;

   	$st = "<select name='$name'>";
	if ($all_option == true)
	{
		$reserved_word = reserved_words::get_all_numeric();
    	if ($reserved_word == $selected_id)
    	     $st .= "<option selected value='" . $reserved_word . "'>" . $all_option_name . "</option>\n";
    	else
    	     $st .= "<option value='" . $reserved_word . "'>" . $all_option_name . "</option>\n";
		if ($selected_id == "")
			$selected_id = $reserved_word;
	}

	foreach ($systypes_array as $i => $value)
	{
		if ($i == 16 || $i == 18 || $i == 25 || ($i >= 28 && $i <= 30) || $i >= 40)
			continue;
    	$st .= "<option" . ($selected_id == $i ? " selected" : "") . " value='$i'>" . $value['name'] . "</option>\n";
	}
    $st .= "</select>";
	return $st;
}


?>