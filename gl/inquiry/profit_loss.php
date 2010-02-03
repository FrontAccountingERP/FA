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

page(_($help_context = "Profit & Loss Drilldown"), false, false, "", $js);

//----------------------------------------------------------------------------------------------------
// Ajax updates

if (get_post('Show')) 
{
	$Ajax->activate('pl_tbl');
}

if (isset($_GET["TransFromDate"]))
	$_POST["TransFromDate"] = $_GET["TransFromDate"];	
if (isset($_GET["TransToDate"]))
	$_POST["TransToDate"] = $_GET["TransToDate"];
if (isset($_GET["Compare"]))
	$_POST["Compare"] = $_GET["Compare"];	
if (isset($_GET["AccGrp"]))
	$_POST["AccGrp"] = $_GET["AccGrp"];	
if (isset($_GET["DrillLevel"]))
	$_POST["DrillLevel"] = $_GET["DrillLevel"];	
else 	
	$_POST["DrillLevel"] = 1; // Root level
	
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
function Achieve($d1, $d2)
{
	if ($d1 == 0 && $d2 == 0)
		return 0;
	elseif ($d2 == 0)
		return 999;
	$ret = ($d1 / $d2 * 100.0);
	if ($ret > 999)
		$ret = 999;
	return $ret;
}

function inquiry_controls()
{  
    start_table("class='tablestyle_noborder'");
    
    date_cells(_("From:"), 'TransFromDate', '', null, -30);
	date_cells(_("To:"), 'TransToDate');
	
	//Compare Combo
	global $sel;
	$sel = array(_("Accumulated"), _("Period Y-1"), _("Budget"));	
	echo "<td>Comapre To:</td>\n";
	echo "<td>";
	echo array_selector('Compare', null, $sel);
	echo "</td>\n";	
	
	submit_cells('Show',_("Show"),'','', 'default');
    end_table();

	hidden('AccGrp');
}

//----------------------------------------------------------------------------------------------------

function print_profit_and_loss()
{
	global $comp_path, $path_to_root, $table_style, $sel;

	$dim = get_company_pref('use_dimension');
	$dimension = $dimension2 = 0;

	$from = $_POST['TransFromDate'];
	$to = $_POST['TransToDate'];
	$compare = $_POST['Compare'];
	$drilllevel = $_POST["DrillLevel"];
	$nextDrillLevel = $drilllevel + 1;

	$dec = 0;
	$pdec = user_percent_dec();

	if ($compare == 0 || $compare == 2)
	{
		$end = $to;
		if ($compare == 2)
		{
			$begin = $from;
		}
		else
			$begin = begin_fiscalyear();
	}
	elseif ($compare == 1)
	{
		$begin = add_months($from, -12);
		$end = add_months($to, -12);
	}

	$classname = '';
	
	$typeper = array(0,0,0,0,0,0,0,0,0,0);
	$typeacc = array(0,0,0,0,0,0,0,0,0,0);
	$typename = array('','','','','','','','','','');
	$acctype = array('','','','','','','','','','');	
	$closing = array(-1,-1,-1,-1,-1,-1,-1,-1,-1,-1);
	$level = 0;

	$classper = 0.0;
	$classacc = 0.0;
	$salesper = 0.0;
	$salesacc = 0.0;
	$last = -1;

	$closeclass = false;
	$convert = 1;
	$ctype = 0;

	//For drill down, prepare the list of sub account types
	if (isset($_POST['AccGrp']) && (strlen($_POST['AccGrp']) > 0) )
		$sub_types_arr = get_child_account_types($_POST['AccGrp']);	
		
	$accounts = get_gl_accounts_all(0);
	
	div_start('pl_tbl');

	start_table("width=50% $table_style");

	$tableheader =  "<tr>
        <td class='tableheader'>" . _("Group/Account Name") . "</td>
        <td class='tableheader'>" . _("Period") . "</td>
		<td class='tableheader'>" . $sel[$compare] . "</td>
		<td class='tableheader'>" . _("Achieved %") . "</td>
        </tr>";

	while ($account=db_fetch($accounts))
	{
		if ($account['account_code'] == null && $account['parent'] > 0)
			continue;			
			
		//Check for confirming the account type
		if (isset($_POST['AccGrp']) && (strlen($_POST['AccGrp']) > 0) )
		{	
			if (!is_of_account_type($account['AccountType'], $sub_types_arr))
				continue;
		}			

		if ($account['account_code'] != null)
		{
			$per_balance = get_gl_trans_from_to($from, $to, $account["account_code"], $dimension, $dimension2);

			if ($compare == 2)
				$acc_balance = get_budget_trans_from_to($begin, $end, $account["account_code"], $dimension, $dimension2);
			else
				$acc_balance = get_gl_trans_from_to($begin, $end, $account["account_code"], $dimension, $dimension2);
			if (!$per_balance && !$acc_balance)
				continue;
		}

		if ($account['AccountClassName'] != $classname)
		{
			if ($classname != '')
			{
				$closeclass = true;
			}
		}
		
		if ($account['AccountTypeName'] != $typename[$level] || $closeclass )
		{				
			if ($typename[$level] != '')
			{
				for ( ; $level >= 0, $typename[$level] != ''; $level--) 
				{
					if ($account['parent'] == $closing[$level] || $account['parent'] < $last || $account['parent'] <= 0 || $closeclass)
					{	
						// Display Groups only of Specific Drill Level
						if ($level == $drilllevel && ($typeper[$level] != 0 || $typeacc[$level] != 0 )) 
						{
							$url = "<a href='$path_to_root/gl/inquiry/profit_loss.php?TransFromDate=" 
								. $from . "&TransToDate=" . $to . "&Compare=" . $compare
								. "&AccGrp=" . $acctype[$level] . "&DrillLevel=".$nextDrillLevel."'>" . $typename[$level] ."</a>";
											
							alt_table_row_color($k);
							label_cell($url);
							amount_cell($typeper[$level] * $convert);
							amount_cell($typeacc[$level] * $convert);
							amount_cell(Achieve($typeper[$level], $typeacc[$level]));
							end_row();
						}						
						
						$typeper[$level] = $typeacc[$level] = 0.0;
					}
					else
						break;
				}
				if ($closeclass)
				{
					start_row("class='inquirybg' style='font-weight:bold'");
					label_cell(_('Total') . " " . $classname);
					amount_cell($classper * $convert);
					amount_cell($classacc * $convert);
					amount_cell(Achieve($classper, $classacc));
					end_row();					
					
					$salesper += $classper;
					$salesacc += $classacc;
					$classper = $classacc = 0.0;

					$closeclass = false;
				}
			}
			if ($account['AccountClassName'] != $classname)
			{
				if (isset($_POST['AccGrp']) && (strlen($_POST['AccGrp']) > 0))
					table_section_title($account['AccountTypeName'],4);
				else 
					table_section_title($account['AccountClassName'],4);
					
				echo $tableheader;
			}
			$level++;
			if ($account['parent'] != $last)
				$last = $account['parent'];
			$typename[$level] = $account['AccountTypeName'];
			$acctype[$level] = $account['AccountType'];
			$closing[$level] = $account['parent'];
		}
		$classname = $account['AccountClassName'];
		$ctype = $account['ClassType'];
		$convert = get_class_type_convert($ctype); 
		
		if ($account['account_code'] != null)
		{
			//$per_balance *= -1;
			//$acc_balance *= -1;
		
			for ($i = 0; $i <= $level; $i++)
			{
				$typeper[$i] += $per_balance;
				$typeacc[$i] += $acc_balance;
			}
			$classper += $per_balance;
			$classacc += $acc_balance;		
			
			//Show accounts details only for drill down and direct child of Account Group
			if ( isset($_POST['AccGrp']) && ($account['AccountType'] == $_POST['AccGrp']))
			{
				$url = "<a href='$path_to_root/gl/inquiry/gl_account_inquiry.php?TransFromDate=" 
					. $from . "&TransToDate=" . $to 
					. "&account=" . $account['account_code'] . "'>" . $account['account_code'] 
					." ". $account['account_name'] ."</a>";				
					
				start_row("class='stockmankobg'");
				label_cell($url);
				amount_cell($per_balance * $convert);
				amount_cell($acc_balance * $convert);
				amount_cell(Achieve($per_balance, $acc_balance));
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
					
					//Inside drill down, Show summary of the specific account group
					if (isset($_POST['AccGrp']) && (strlen($_POST['AccGrp']) > 0) && ($acctype[$level] == $_POST['AccGrp']))
					{
						start_row("class='inquirybg' style='font-weight:bold'");
						label_cell(_('Total') . " " .$typename[$level]);
						amount_cell($typeper[$level] * $convert);
						amount_cell($typeacc[$level] * $convert);
						amount_cell(Achieve($typeper[$level], $typeacc[$level]));
						end_row();							
					}
					 // Display Groups only of Specific Drill Level
					elseif ($level == $drilllevel  && ($typeper[$level] != 0 || $typeacc[$level] != 0 )) 
					{
						$url = "<a href='$path_to_root/gl/inquiry/profit_loss.php?TransFromDate=" 
							. $from . "&TransToDate=" . $to . "&Compare=" . $compare
							. "&AccGrp=" . $acctype[$level] . "&DrillLevel=".$nextDrillLevel."'>" . $typename[$level] ."</a>";
										
						alt_table_row_color($k);
						label_cell($url);
						amount_cell($typeper[$level] * $convert);
						amount_cell($typeacc[$level] * $convert);
						amount_cell(Achieve($typeper[$level], $typeacc[$level]));
						end_row();
					}					
					
					$typeper[$level] = $typeacc[$level] = 0.0;
				}
				else
					break;
			}

			if (($closeclass) && !(isset($_POST['AccGrp']) && (strlen($_POST['AccGrp']) > 0)) )
			{
				$calculateper = $salesper + $classper;
				$calculateacc = $salesacc + $classacc;

				start_row("class='inquirybg' style='font-weight:bold'");
				label_cell(_('Total') . " " . $classname);
				amount_cell($classper * $convert);
				amount_cell($classacc * $convert);
				amount_cell(Achieve($classper, $classacc));
				end_row();	
				
				start_row("class='inquirybg' style='font-weight:bold'");
				label_cell(_('Calculated Return'));
				amount_cell($calculateper *-1);
				amount_cell($calculateacc * -1);
				amount_cell(Achieve($calculateper, $calculateacc));
				end_row();					
				
			}
		}
	}
	end_table(1); // outer table
	div_end();
}

//----------------------------------------------------------------------------------------------------

start_form();

inquiry_controls();

print_profit_and_loss();

end_form();

end_page();

?>

