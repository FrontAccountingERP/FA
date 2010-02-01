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
$page_security = 'SA_GLANALYTIC';
$path_to_root="../..";

include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/gl/includes/gl_db.inc");

$js = "";
if ($use_date_picker)
	$js = get_js_date_picker();

page(_($help_context = "Balance Sheet"), false, false, "", $js);

//----------------------------------------------------------------------------------------------------
// Ajax updates

if (get_post('Show')) 
{
	$Ajax->activate('balance_tbl');
}

if (isset($_GET["TransFromDate"]))
	$_POST["TransFromDate"] = $_GET["TransFromDate"];	
if (isset($_GET["TransToDate"]))
	$_POST["TransToDate"] = $_GET["TransToDate"];
if (isset($_GET["AccGrp"]))
	$_POST["AccGrp"] = $_GET["AccGrp"];	

//----------------------------------------------------------------------------------------------------

function is_of_account_type($accttype,$typeslist)
{
	return in_array($accttype, $typeslist);
}

function get_child_account_types($acctype)
{
	global $parentsarr;
	$parentsarr = array();
	$childernsarr = array();
	$list = '';
	array_push($parentsarr, $acctype);
    while (sizeof($parentsarr)>0)
    {
		$parent = array_pop($parentsarr);
		array_push($childernsarr,$parent);
		pushchilds($parent);
	}
	$list = substr($list,0,-1);
	return $childernsarr;
}

function pushchilds($parent)
{
	global $parentsarr;

	$sql = "SELECT id FROM  ".TB_PREF."chart_types WHERE parent=".$parent;
	$result = db_query($sql,"Query failed");   
	while ($myrow=db_fetch($result))
	{
		array_push($parentsarr, $myrow['id']);
	}
}

function inquiry_controls()
{
   
    start_table("class='tablestyle_noborder'");
	date_cells(_("As at:"), 'TransToDate');
	submit_cells('Show',_("Show"),'','', 'default');
    end_table();

	hidden('TransFromDate');
	hidden('AccGrp');

}

function print_balance_sheet()
{
	global $comp_path, $path_to_root, $table_style;
	
	$from = begin_fiscalyear();
	$to = $_POST['TransToDate'];
	
	$dim = get_company_pref('use_dimension');
	$dimension = $dimension2 = 0;	
	
	$classname = '';
	$classopen = 0.0;
	$classperiod = 0.0;
	$classclose = 0.0;
	$assetsopen = 0.0;
	$assetsperiod = 0.0;
	$assetsclose = 0.0;
	$equityopen = 0.0;
	$equityperiod = 0.0;
	$equityclose = 0.0;
	$lopen = 0.0;
	$lperiod = 0.0;
	$lclose = 0.0;
	
	$typeopen = array(0,0,0,0,0,0,0,0,0,0);
	$typeperiod = array(0,0,0,0,0,0,0,0,0,0);
	$typeclose = array(0,0,0,0,0,0,0,0,0,0);
	$typename = array('','','','','','','','','','');
	$acctype = array('','','','','','','','','','');
	$closing = array(-1,-1,-1,-1,-1,-1,-1,-1,-1,-1);
	//$parent = array(-1,-1,-1,-1,-1,-1,-1,-1,-1,-1);
	$level = 0;
	$last = -1;
	
	$closeclass = false;
	$ctype = 0;
	$convert = 1;	

	$accounts = get_gl_accounts_all(1);
	
	div_start('balance_tbl');

	start_table("width=30% $table_style");
	
	while ($account=db_fetch($accounts))
	{
		if ($account['account_code'] == null && $account['parent'] > 0)
			continue;

		//Check for confirming the account type
		if (isset($_POST['AccGrp']) && (strlen($_POST['AccGrp']) > 0) )
		{	
			$sub_types_arr = get_child_account_types($_POST['AccGrp']);
			if (!is_of_account_type($account['AccountType'], $sub_types_arr))
				continue;
		}
							
		if ($account['account_code'] != null)
		{
			$prev_balance = get_gl_balance_from_to("", $from, $account["account_code"], $dimension, $dimension2);

			$curr_balance = get_gl_trans_from_to($from, $to, $account["account_code"], $dimension, $dimension2);

			if (!$prev_balance && !$curr_balance)
				continue;
		}
		if ($account['AccountClassName'] != $classname)
		{
			if ($classname != '')
			{
				$closeclass = true;
			}
		}
		if ($account['AccountTypeName'] != $typename[$level])
		{
			if ($typename[$level] != '')
			{
				for ( ; $level >= 0, $typename[$level] != ''; $level--) 
				{
					if ($account['parent'] == $closing[$level] || $account['parent'] < $last || $account['parent'] <= 0 || $closeclass)
					{	
					
						$url = "<a href='$path_to_root/gl/inquiry/balance_sheet.php?TransFromDate=" 
							. $from . "&TransToDate=" . $to 
							. "&AccGrp=" . $acctype[$level] . "'>" . $typename[$level] . "</a>";
										
						alt_table_row_color($k);
						label_cell($url);
						amount_cell($typeclose[$level] * $convert);
						end_row();						

						$typeopen[$level] = $typeperiod[$level] = $typeclose[$level] = 0.0;
					}	
					else
						break;
				}

				if ($closeclass)
				{	
					start_row("class='inquirybg' style='font-weight:bold'");
					label_cell(_('Total') . " " . $classname);
					amount_cell($classclose * $convert, true);
					end_row();						
								
					if ($ctype == CL_EQUITY)
					{
						$equityopen += $classopen;
						$equityperiod += $classperiod;
						$equityclose += $classclose;
					}
					if ($ctype == CL_LIABILITIES)
					{
						$lopen += $classopen;
						$lperiod += $classperiod;
						$lclose += $classclose;
					}
					$assetsopen += $classopen;
					$assetsperiod += $classperiod;
					$assetsclose += $classclose;
					$classopen = $classperiod = $classclose = 0.0;

					$closeclass = false;
				}
			}
			if ($account['AccountClassName'] != $classname)
			{
				if (isset($_POST['AccGrp']) && (strlen($_POST['AccGrp']) > 0))
					table_section_title($account['AccountTypeName']);
				else 
					table_section_title($account['AccountClassName']);
			}
			$level++;
			if ($account['parent'] != $last)
				$last = $account['parent'];
			$typename[$level] = $account['AccountTypeName'];
			
			$acctype[$level] = $account['AccountType'];
			
			$closing[$level] = $account['parent'];

		}
		$classname = $account['AccountClassName'];
		$classtype = $account['AccountType'];
		$ctype = $account['ClassType'];
		$convert = get_class_type_convert($ctype); 

		if ($account['account_code'] != null)
		{
			for ($i = 0; $i <= $level; $i++)
			{
				$typeopen[$i] += $prev_balance;
				$typeperiod[$i] += $curr_balance;
				$typeclose[$i] = $typeopen[$i] + $typeperiod[$i];
			}
			$classopen += $prev_balance;
			$classperiod += $curr_balance;
			$classclose = $classopen + $classperiod;

			//Show accounts details only for drill down and direct child of Account Group
			if ( isset($_POST['AccGrp']) && ($account['AccountType'] == $_POST['AccGrp']))
			{
				$url = "<a href='$path_to_root/gl/inquiry/gl_account_inquiry.php?TransFromDate=" 
					. $from . "&TransToDate=" . $to 
					. "&account=" . $account['account_code'] . "'>" . $account['account_code'] 
					." ". $account['account_name'] ."</a>";				
					
				start_row("class='stockmankobg'");
				label_cell($url);
				amount_cell(($curr_balance + $prev_balance) * $convert);
				end_row();
			}
		}	
	}
	
	if ($account['AccountClassName'] != $classname)
	{
		if ($classname != '')
		{
			$closeclass = true;
		}
	}
	if ($account['AccountTypeName'] != $typename[$level])
	{

		if ($typename[$level] != '')
		{
			for ( ; $level >= 0, $typename[$level] != ''; $level--) 
			{
				if ($account['parent'] == $closing[$level] || $account['parent'] < $last || $account['parent'] <= 0 || $closeclass)
				{
					//Inside drill down, no hyperlink
					if (isset($_POST['AccGrp']) && (strlen($_POST['AccGrp']) > 0) && ($acctype[$level] == $_POST['AccGrp']))
					{
						start_row("class='inquirybg' style='font-weight:bold'");
						label_cell(_('Total') . " " .$typename[$level]);
						amount_cell($typeclose[$level] * $convert);
						end_row();							
					}
					else
					{
						$url = "<a href='$path_to_root/gl/inquiry/balance_sheet.php?TransFromDate=" 
							. $from . "&TransToDate=" . $to 
							. "&AccGrp=" . $acctype[$level] . "'>" . $typename[$level] . "</a>";					
						
						alt_table_row_color($k);
						label_cell($url);
						amount_cell($typeclose[$level] * $convert);
						end_row();							
					}
					
					$typeopen[$level] = $typeperiod[$level] = $typeclose[$level] = 0.0;					
					
				}
				else
					break;

			}

			if (($closeclass) && !(isset($_POST['AccGrp']) && (strlen($_POST['AccGrp']) > 0)) )
			{
				$calculateopen = -$assetsopen - $classopen;
				$calculateperiod = -$assetsperiod - $classperiod;
				$calculateclose = -$assetsclose  - $classclose;
				if ($ctype == CL_EQUITY)
				{
					$equityopen += $classopen;
					$equityperiod += $classperiod;
					$equityclose += $classclose;
				}
			
				alt_table_row_color($k);
				label_cell(_('Calculated Return'));
				amount_cell($calculateclose * $convert);
				end_row();	
							
				start_row("class='inquirybg' style='font-weight:bold'");
				label_cell(_('Total') . " " . $classname);
				amount_cell(-$assetsclose * $convert);
				end_row();					
				
				if ($equityopen != 0.0 || $equityperiod != 0.0 || $equityclose != 0.0 ||
					$lopen != 0.0 || $lperiod != 0.0 || $lclose != 0.0)
				{
					alt_table_row_color($k);
					label_cell(_('Total') . " " . _('Liabilities') . _(' and ') . _('Equities'));
					amount_cell(($lclose + $equityclose + $calculateclose) * -1);
					end_row();	
				}
			}
		}
	}
	end_table(1); // outer table
	div_end();
}

//----------------------------------------------------------------------------------------------------

start_form();

inquiry_controls();

print_balance_sheet();

end_form();

end_page();

?>

