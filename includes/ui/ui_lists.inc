<?php

include_once($path_to_root . "/includes/banking.inc");

define("DEFVAL", "--");
$search_button = "<input type='submit' class='inputsubmit' style='border:0;background:url($path_to_root/themes/default/images/locate.png) no-repeat;' name='%s' value='?' title='"._("Search")."'> ";
//$search_button = "<input type='submit' class='inputsubmit' name='%s' value='?'> ";

$all_items = reserved_words::get_all();

// TDB for all list functions : if there is no data, display a link to the releveant
// page to add an item, eg. for locations, if no locations, link to Add Location

function supplier_list($name, $selected_id, $all_option=false, $submit_on_change=false)
{
	global $all_items;

	if ($selected_id == null)
		$selected_id = ((!isset($_POST[$name]) || $_POST[$name] == "") ? "" : $_POST[$name]);
	$no_supplier_list = get_company_pref('no_supplier_list');
	if ($no_supplier_list)
	{
		global $search_button;
		$edit_name = $name."_edit";
		$edit_button = $name."_button";

		$val = (isset($_POST[$edit_name]) && $_POST[$edit_name] != "" ? $_POST[$edit_name] : "");
		if (isset($_POST[$edit_button]))
		{
			$selected_id = $_POST[$name] = "";
		}

		if ($selected_id != "")
		{
			$val = DEFVAL;
			$supplier_sql = "SELECT supplier_id, supp_name, curr_code FROM ".TB_PREF."suppliers WHERE supplier_id=$selected_id";
			$supplier_result = db_query($supplier_sql);
		}
		else
		{
			if ($val != "" && $val != DEFVAL)
			{
				$supplier_sql = "SELECT supplier_id, supp_name, curr_code FROM ".TB_PREF."suppliers WHERE supp_name LIKE '%{$val}%' ORDER BY supp_name";
				$supplier_result = db_query($supplier_sql);
			}
			else
				$supplier_result = false;
		}

		echo "<input type='text' name='$edit_name' size='8' maxlength='50' value='$val' onblur='this.form.$edit_button.click();'> ";

		echo sprintf($search_button, $edit_button);
	}
	else
	{
		$supplier_sql = "SELECT supplier_id, supp_name, curr_code FROM ".TB_PREF."suppliers ORDER BY supp_name";
		$supplier_result = db_query($supplier_sql);
	}

	if ($submit_on_change==true)
		echo "<select name='$name' onchange='this.form.submit();'>";
	else
		echo "<select name='$name'>";

	$company_currency = get_company_currency();

	if ($all_option == true)
	{
    	if ($selected_id == $all_items)
    	{
    	     echo "<option selected value='$all_items'>" . _("All Suppliers") . "</option>\n";
    	}
    	else
    	{
    	     echo "<option value='$all_items'>" . _("All Suppliers") . "</option>\n";
    	}
		if ($selected_id == "")
		{
			$selected_id = $all_items;
		}
	}

	while ($supplier_row = db_fetch_row($supplier_result))
	{
		if ($selected_id==$supplier_row[0])
		{
			echo "<option selected value='" . $supplier_row[0] . "'>";
		}
		else
		{
			echo "<option value='" . $supplier_row[0] . "'>";
		}
		echo $supplier_row[1];

		if ($supplier_row[2] != $company_currency)
			echo "&nbsp;-&nbsp;" . $supplier_row[2];
		echo  "</option>\n";
		if ($selected_id == "")
		{
			$selected_id = $supplier_row[0];
			$_POST[$name] = $selected_id;
		}
	}

	echo "</select>";

	db_free_result($supplier_result);
}

function supplier_list_cells($label, $name, $selected_id, $all_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td nowrap>";
	supplier_list($name, $selected_id, $all_option, $submit_on_change);
	echo "</td>\n";
}

function supplier_list_row($label, $name, $selected_id, $all_option=false, $submit_on_change=false)
{
	echo "<tr>\n";
	supplier_list_cells($label, $name, $selected_id, $all_option, $submit_on_change);
	echo "</tr>\n";
}

//----------------------------------------------------------------------------------------------

function customer_list($name, $selected_id, $all_option=false, $submit_on_change=false)
{
	global $all_items;

	if ($selected_id == null)
		$selected_id = ((!isset($_POST[$name]) || $_POST[$name] == "") ? "" : $_POST[$name]);
	$no_customer_list = get_company_pref('no_customer_list');
	if ($no_customer_list)
	{
		global $search_button;
		$edit_name = $name."_edit";
		$edit_button = $name."_button";

		$val = (isset($_POST[$edit_name]) && $_POST[$edit_name] != "" ? $_POST[$edit_name] : "");
		if (isset($_POST[$edit_button]))
		{
			$selected_id = $_POST[$name] = "";
			$_POST['branch_id'] = "";
		}

		if ($selected_id != "")
		{
			$val = DEFVAL;
			$customer_sql = "SELECT debtor_no, name, curr_code FROM ".TB_PREF."debtors_master WHERE debtor_no=$selected_id";
			$customer_result = db_query($customer_sql);
		}
		else
		{
			if ($val != "" && $val != DEFVAL)
			{
				$customer_sql = "SELECT debtor_no, name, curr_code FROM ".TB_PREF."debtors_master WHERE name LIKE '%{$val}%' ORDER BY name";
				$customer_result = db_query($customer_sql);
			}
			else
				$customer_result = false;
		}

		echo "<input type='text' name='$edit_name' size='8' maxlength='50' value='$val' onblur='this.form.$edit_button.click();'> ";

		echo sprintf($search_button, $edit_button);
	}
	else
	{
		$customer_sql = "SELECT debtor_no, name, curr_code FROM ".TB_PREF."debtors_master ORDER BY name";
		$customer_result = db_query($customer_sql);
	}
	if ($submit_on_change==true)
		echo "<select name='$name' onchange='this.form.submit();'>";
	else
		echo "<select name='$name'>";

	$company_currency = get_company_currency();

	if (($all_option == true))
	{
    	if ($selected_id == $all_items)
    	{
    	     echo "<option selected value='$all_items'>" . _("All Customers") . "</option>\n";
    	}
    	else
    	{
    	     echo "<option value='$all_items'>" . _("All Customers") . "</option>\n";
    	}
		if ($selected_id == "")
		{
			$selected_id =  $all_items;
		}
	}
	while ($customer_row = db_fetch_row($customer_result))
	{
		if ($selected_id == $customer_row[0])
		{
			echo "<option selected value='" . $customer_row[0] . "'>";
		}
		else
		{
			echo "<option value='" . $customer_row[0] . "'>";
		}

		echo $customer_row[1];
		if ($customer_row[2] != $company_currency)
			echo "&nbsp;-&nbsp;" . $customer_row[2];

		echo  "</option>\n";
		// if no initial selection - set the first item
		// do we want to do this for all lists ???? probably
		if ($selected_id == "")
		{
			$selected_id = $customer_row[0];
			$_POST[$name] = $selected_id;
		}
	}

	echo "</select>";
	db_free_result($customer_result);
}

function customer_list_cells($label, $name, $selected_id, $all_option = false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td nowrap>";
	customer_list($name, $selected_id, $all_option, $submit_on_change);
	echo "</td>\n";
}

function customer_list_row($label, $name, $selected_id, $all_option = false, $submit_on_change=false)
{
	echo "<tr>\n";
	customer_list_cells($label, $name, $selected_id, $all_option, $submit_on_change);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------------------

function customer_branches_list($customer_id, $name, $selected_id,
	$all_option = true, $enabled=true, $submit_on_change=false)
{
	global $all_items;

	if ($submit_on_change==true)
		echo "<select name='$name' onchange='this.form.submit();'>";
	else
		echo "<select name='$name'>";

	$sql = "SELECT branch_code, br_name FROM ".TB_PREF."cust_branch
		WHERE debtor_no='" . $customer_id . "'";
	if ($enabled)
		$sql .= " AND disable_trans = 0";
	$result = db_query($sql);

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	if ($all_option == true)
	{
		echo "<option ";
		if ($selected_id == $all_items)
			echo " selected ";
		echo "value='$all_items'>" . _("All Branches") . "</option>\n";

		if ($selected_id == "")
		{
			$selected_id =  $all_items;
		}
	}

	while ($row = db_fetch_row($result))
	{
		if ($selected_id == $row[0])
		{
			echo "<option selected value='" . $row[0] . "'>";
		}
		else
		{
			echo "<option value='" . $row[0] . "'>";
		}
		echo $row[1] . "</option>\n";

		if ($selected_id == "")
		{
			$selected_id = $row[0];
			$_POST[$name] = $selected_id;
		}
	}

	echo "</select>";
	db_free_result($result);
}

function customer_branches_list_cells($label,$customer_id, $name, $selected_id, $all_option = true, $enabled=true, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	customer_branches_list($customer_id, $name, $selected_id, $all_option, $enabled, $submit_on_change);
	echo "</td>\n";
}

function customer_branches_list_row($label,$customer_id, $name, $selected_id, $all_option = true, $enabled=true, $submit_on_change=false)
{
	echo "<tr>";
	customer_branches_list_cells($label, $customer_id, $name, $selected_id, $all_option, $enabled, $submit_on_change);
	echo "</tr>";
}

//------------------------------------------------------------------------------------------------

function locations_list($name, $selected_id, $all_option=false, $submit_on_change=false)
{
	global $all_items;

	if ($submit_on_change == true)
		echo "<select name='$name' onchange='this.form.submit();'>";
	else
		echo "<select name='$name'>";

//	if ($selected_id =="" AND isset($_SESSION['UserStockLocation']) AND $_SESSION['UserStockLocation'] !="") {
//	  	$selected_id = $_SESSION['UserStockLocation'];
//	}

	$sql = "SELECT loc_code, location_name FROM ".TB_PREF."locations";
	$result = db_query($sql);

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	if ($all_option == true)
	{
		echo "<option ";
    	if ($selected_id == $all_items)
    	     echo " selected ";
		echo " value='$all_items'>" . _("All Locations") . "</option>\n";

		if ($selected_id == "")
		{
			$selected_id = $all_items;
		}
	}

	while ($row = db_fetch_row($result))
	{
		if ($selected_id == $row[0])
		{
			echo "<option selected value='" . $row[0] . "'>";
		}
		else
		{
			echo "<option value='" . $row[0] . "'>";
		}
		echo $row[1] . "</option>\n";

		if ($selected_id == "")
		{
			$selected_id = $row[0];
			$_POST[$name] = $selected_id;
		}
	}

	echo "</select>";

	db_free_result($result);
}

function locations_list_cells($label, $name, $selected_id, $all_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	locations_list($name, $selected_id, $all_option, $submit_on_change);
	echo "</td>\n";
}

function locations_list_row($label, $name, $selected_id, $all_option=false, $submit_on_change=false)
{
	echo "<tr>";
	locations_list_cells($label, $name, $selected_id, $all_option, $submit_on_change);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------

function currencies_list($name, &$selected_id, $submit_on_change=false)
{
	if ($submit_on_change==true)
		echo "<select name='$name' onchange='this.form.submit();'>";
	else
		echo "<select name='$name'>";

	$company_currency = get_company_currency();

	$sql = "SELECT curr_abrev, currency FROM ".TB_PREF."currencies";
	$result = db_query($sql);

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	while ($row = db_fetch_row($result))
	{
		// default to the company currency
		if ($selected_id == "" && ($row[0] == $company_currency))
		{
			$selected_id = $row[0];
			$_POST[$name] = $selected_id;
		}

		if ($selected_id == $row[0])
		{
			echo "<option selected value='" . $row[0] . "'>";
		}
		else
		{
			echo "<option value='" . $row[0] . "'>";
		}
		echo $row[0] . "&nbsp;-&nbsp;" . $row[1] . "</option>\n";
	}

	echo "</select>";
	db_free_result($result);
}

function currencies_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	currencies_list($name, $selected_id);
	echo "</td>\n";
}

function currencies_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	currencies_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//---------------------------------------------------------------------------------------------------

function fiscalyears_list($name, &$selected_id, $submit_on_change=false)
{
	if ($submit_on_change == true)
		echo "<select name='$name' onchange='this.form.submit();'>";
	else
		echo "<select name='$name'>";

	$company_year = get_company_pref('f_year');

	$sql = "SELECT * FROM ".TB_PREF."fiscal_year ORDER BY begin";
	$result = db_query($sql);

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	while ($row = db_fetch_row($result))
	{
		// default to the company current fiscal year
		if ($selected_id == "" && ($row[0] == $company_year))
		{
			$selected_id = $row[0];
			$_POST[$name] = $selected_id;
		}

		if ($selected_id == $row[0])
		{
			echo "<option selected value='" . $row[0] . "'>";
		}
		else
		{
			echo "<option value='" . $row[0] . "'>";
		}
		if ($row[3] == 0)
			$how = _('Active');
		else
			$how = _('Closed');
		$row[1] = sql2date($row[1]);
		$row[2] = sql2date($row[2]);
		echo $row[1] . "&nbsp;-&nbsp;" . $row[2] . "&nbsp;&nbsp;" . $how . "</option>\n";
	}

	echo "</select>";
	db_free_result($result);
}

function fiscalyears_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	fiscalyears_list($name, $selected_id);
	echo "</td>\n";
}

function fiscalyears_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	fiscalyears_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//---------------------------------------------------------------------------------------------------

function simple_codeandname_list($sql, $name, &$selected_id,
	$all_option=false, $all_option_name=null, $all_option_numeric=false,
	$submit_on_change=false, $returnzero=false)
{
	global $all_items;

	if ($submit_on_change == true)
		echo "<select name='$name' onchange='this.form.submit();'>";
	else
		echo "<select name='$name'>";

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	if ($all_option == true)
	{
		if ($returnzero)
			$reserved_word = 0;
		elseif ($all_option_numeric)
			$reserved_word = reserved_words::get_all_numeric();
		else
			$reserved_word = $all_items;

    	if ($selected_id == $reserved_word)
    	{
    	     echo "<option selected value='$reserved_word'>$all_option_name</option>\n";
    	}
    	else
    	{
    	     echo "<option value='$reserved_word'>$all_option_name</option>\n";
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
			echo "<option selected value='" . $row[0] . "'>";
		} else {
			echo "<option value='" . $row[0] . "'>";
		}
		echo $row[1] . "</option>\n";

		if (!$returnzero && $selected_id == "")
		{
			$selected_id = $row[0];
			$_POST[$name] = $selected_id;
		}
	}

	echo "</select>";
	db_free_result($result);
}

//------------------------------------------------------------------------------------

function dimensions_list($name, &$selected_id, $no_option=false, $showname=null,
	$submit_on_change=false, $showclosed=false, $showtype=1)
{
	$sql = "SELECT id, CONCAT(reference,'  ',name) FROM ".TB_PREF."dimensions";
	if ($showclosed || $showtype)
	{
		$sql .= " WHERE";
		if ($showclosed)
			$sql .= " closed=0";
		if ($showclosed && $showtype)
			$sql .= " AND type_=$showtype";
		else if ($showtype)
			$sql .= " type_=$showtype";
	}
	$sql .= " ORDER BY reference";
	simple_codeandname_list($sql, $name, $selected_id, $no_option, $showname,
		true, $submit_on_change, true);
}

function dimensions_list_cells($label, $name, $selected_id, $no_option=false, $showname=null,
	$showclosed=false, $showtype=0)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	dimensions_list($name, $selected_id, $no_option, $showname, false, $showclosed, $showtype);
	echo "</td>\n";
}

function dimensions_list_row($label, $name, $selected_id, $no_option=false, $showname=null,
	$showclosed=false, $showtype=0)
{
	echo "<tr>\n";
	dimensions_list_cells($label, $name, $selected_id, $no_option, $showname,
		$showclosed, $showtype);
	echo "</tr>\n";
}

//---------------------------------------------------------------------------------------------------

function stock_items_list($name, $selected_id, $all_option=false, $submit_on_change=false, $extra="")
{
	global $all_items;

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	$no_item_list = get_company_pref('no_item_list');
	if ($no_item_list)
	{
		global $search_button;
		$edit_name = $name."_edit";
		$edit_button = $name."_button";

		$val = (isset($_POST[$edit_name]) && $_POST[$edit_name] != "" ? $_POST[$edit_name] : "");

		if (isset($_POST[$edit_button]))
		{
			$selected_id = $_POST[$name] = "";
		}

		if ($selected_id != "")
		{
			$val = DEFVAL;
			$sql = "SELECT stock_id, ".TB_PREF."stock_master.description, ".TB_PREF."stock_category.description
				FROM ".TB_PREF."stock_master,".TB_PREF."stock_category WHERE ".TB_PREF."stock_master.category_id=".TB_PREF."stock_category.category_id
				AND stock_id='$selected_id'";
			$result = db_query($sql);
		}
		else
		{
			if ($val != "" && $val != DEFVAL)
			{
				$sql = "SELECT stock_id, ".TB_PREF."stock_master.description, ".TB_PREF."stock_category.description
					FROM ".TB_PREF."stock_master,".TB_PREF."stock_category WHERE "
					.TB_PREF."stock_master.category_id=".TB_PREF."stock_category.category_id AND
					(stock_id LIKE '%{$val}%' OR ".TB_PREF."stock_category.description LIKE '%{$val}%' OR "
					.TB_PREF."stock_master.description LIKE '%{$val}%')";
				$result = db_query($sql);
			}
			else
				$result = false;
		}

		echo "<input type='text' name='$edit_name' size='8' maxlength='50' value='$val' onblur='this.form.$edit_button.click();'> ";

		echo sprintf($search_button, $edit_button);
	}
	else
	{
		$sql = "SELECT stock_id, ".TB_PREF."stock_master.description, ".TB_PREF."stock_category.description
			FROM ".TB_PREF."stock_master,".TB_PREF."stock_category WHERE ".TB_PREF."stock_master.category_id=".TB_PREF."stock_category.category_id";
		$result = db_query($sql);
	}

	if ($submit_on_change==true)
		echo "<select name='$name' onchange='this.form.submit();'>";
	else if ($extra != "")
		echo "<select name='$name' $extra>";
	else
		echo "<select name='$name'>";

	if (($all_option == true))
	{
    	if ($selected_id == $all_items)
    	{
    	     echo "<option selected value='$all_items'>" . _("All Items") . "</option>\n";
    	}
    	else
    	{
    	     echo "<option value='$all_items'>" . _("All Items") . "</option>\n";
    	}
		if ($selected_id == "")
		{
			$selected_id = $all_items;
		}
	}

	while ($row = db_fetch_row($result))
	{
		if ($selected_id == $row[0])
		{
			echo "<option selected value='" . $row[0] . "'>";
		}
		else
		{
			echo "<option value='" . $row[0] . "'>";
		}
		echo (user_show_codes()?$row[0] . "&nbsp;-&nbsp;":"") . $row[2] . "&nbsp;-&nbsp;" . $row[1] . "</option>\n";

		// if no initial selection - set the first item
		// do we want to do this for all lists ???? probably
		if ($selected_id == "")
		{
			$selected_id = $row[0];
			$_POST[$name] = $selected_id;
		}
	}
	echo "</select>";
	db_free_result($result);
}

function stock_items_list_cells($label, $name, $selected_id, $all_option=false, $submit_on_change=false, $extra="")
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td nowrap>";
	stock_items_list($name, $selected_id, $all_option, $submit_on_change, $extra);
	echo "</td>\n";
}

function stock_items_list_row($label, $name, $selected_id, $all_option=false, $submit_on_change=false)
{
	echo "<tr>\n";
	stock_items_list_cells($label, $name, $selected_id, $all_option, $submit_on_change);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function stock_bom_items_list($name, $selected_id, $all_option=false, $submit_on_change=false)
{
	global $all_items;

	if ($submit_on_change==true)
		echo "<select name='$name' onchange='this.form.submit();'>";
	else
		echo "<select name='$name'>";

	$sql = "SELECT stock_id, ".TB_PREF."stock_master.description, ".TB_PREF."stock_category.description
		FROM ".TB_PREF."stock_master,".TB_PREF."stock_category WHERE ".TB_PREF."stock_master.category_id=".TB_PREF."stock_category.category_id
		AND (".TB_PREF."stock_master.mb_flag='M' OR ".TB_PREF."stock_master.mb_flag='K')";
	$result = db_query($sql);

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	if (($all_option == true))
	{
    	if ($selected_id == $all_items)
    	{
    	     echo "<option selected value='$all_items'>" . _("All Items") . "</option>\n";
    	}
    	else
    	{
    	     echo "<option value='$all_items'>" . _("All Items") . "</option>\n";
    	}
		if ($selected_id == "")
		{
			$selected_id = $all_items;
		}
	}

	while ($row = db_fetch_row($result))
	{
		if ($selected_id==$row[0])
		{
			echo "<option selected value='" . $row[0] . "'>";
		}
		else
		{
			echo "<option value='" . $row[0] . "'>";
		}
		echo (user_show_codes()?$row[0] . "&nbsp;-&nbsp;":"") . $row[2] . "&nbsp;-&nbsp;" . $row[1] . "</option>\n";

		if ($selected_id == "")
		{
			$selected_id = $row[0];
			$_POST[$name] = $selected_id;
		}
	}

	echo "</select>";
	db_free_result($result);
}

function stock_bom_items_list_cells($label, $name, $selected_id, $all_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	stock_bom_items_list($name, $selected_id, $all_option, $submit_on_change);
	echo "</td>\n";
}

function stock_bom_items_list_row($label, $name, $selected_id, $all_option=false, $submit_on_change=false)
{
	echo "<tr>\n";
	stock_bom_items_list_cells($label, $name, $selected_id, $all_option, $submit_on_change);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function base_stock_items_list($sql, $name, &$selected_id,
	$all_option=false, $all_option_name="", $submit_on_change=false, $extra="")
{
	global $all_items;

	if ($submit_on_change==true)
		echo "<select name='$name' onchange='this.form.submit();'>";
	else if ($extra != "")
		echo "<select name='$name' $extra>";
	else
		echo "<select name='$name'>";

	$result = db_query($sql);

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	if ($all_option == true)
	{
    	if ($selected_id == $all_items)
    	{
    	     echo "<option selected value='$all_items'>$all_option_name</option>\n";
    	}
    	else
    	{
    	     echo "<option value='$all_items'>$all_option_name</option>\n";
    	}
		if ($selected_id == "")
		{
			$selected_id = $all_items;
		}
	}

	while ($row = db_fetch_row($result))
	{
		if ($selected_id == $row[0])
		{
			echo "<option selected value='" . $row[0] . "'>";
		}
		else
		{
			echo "<option value='" . $row[0] . "'>";
		}
		echo (user_show_codes()?$row[0] . "&nbsp;-&nbsp;":"") . $row[2] . "&nbsp;-&nbsp;" . $row[1] . "</option>\n";

		if ($selected_id == "")
		{
			$selected_id = $row[0];
			$_POST[$name] = $selected_id;
		}
	}

	echo "</select>";
	db_free_result($result);
}

//------------------------------------------------------------------------------------

function stock_manufactured_items_list($name, &$selected_id,
	$all_option=false, $submit_on_change=false)
{
	$sql = "SELECT stock_id, ".TB_PREF."stock_master.description, ".TB_PREF."stock_category.description
		FROM ".TB_PREF."stock_master,".TB_PREF."stock_category WHERE ".TB_PREF."stock_master.category_id=".TB_PREF."stock_category.category_id
		AND (".TB_PREF."stock_master.mb_flag='M')";

	base_stock_items_list($sql, $name, $selected_id,	$all_option, _("All Items"),
		$submit_on_change);
}

function stock_manufactured_items_list_cells($label, $name, $selected_id, $all_option=false, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	stock_manufactured_items_list($name, $selected_id, $all_option, $submit_on_change);
	echo "</td>\n";
}

function stock_manufactured_items_list_row($label, $name, $selected_id, $all_option=false, $submit_on_change=false)
{
	echo "<tr>\n";
	stock_manufactured_items_list_cells($label, $name, $selected_id, $all_option, $submit_on_change);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function stock_component_items_list($name, $parent_stock_id, &$selected_id,
	$all_option=false, $submit_on_change=false)
{
	$sql = "SELECT stock_id, ".TB_PREF."stock_master.description, ".TB_PREF."stock_category.description
		FROM ".TB_PREF."stock_master,".TB_PREF."stock_category WHERE
		".TB_PREF."stock_master.category_id=".TB_PREF."stock_category.category_id
		AND stock_id != '$parent_stock_id'";

	base_stock_items_list($sql, $name, $selected_id,
		$all_option, _("All Items"), $submit_on_change);
}

//------------------------------------------------------------------------------------

function stock_purchasable_items_list($name, &$selected_id,	$all_option=false, $submit_on_change=false, $extra="")
{
	global $all_items;

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	$no_item_list = get_company_pref('no_item_list');
	if ($no_item_list)
	{
		$edit_name = $name."_edit";
		$edit_button = $name."_button";

		$val = (isset($_POST[$edit_name]) && $_POST[$edit_name] != "" ? $_POST[$edit_name] : "");
		if (isset($_POST[$edit_button]))
		{
			$selected_id = $_POST[$name] = "";
		}

		if ($selected_id != "")
		{
			$val = DEFVAL;
			$sql = "SELECT stock_id, ".TB_PREF."stock_master.description, ".TB_PREF."stock_category.description
				FROM ".TB_PREF."stock_master,".TB_PREF."stock_category WHERE ".TB_PREF."stock_master.category_id=".TB_PREF."stock_category.category_id
				AND mb_flag !='M' AND stock_id='$selected_id'";
			$result = db_query($sql);
		}
		else
		{
			if ($val != "" && $val != DEFVAL)
			{
				$sql = "SELECT stock_id, ".TB_PREF."stock_master.description, ".TB_PREF."stock_category.description
					FROM ".TB_PREF."stock_master,".TB_PREF."stock_category WHERE "
					.TB_PREF."stock_master.category_id=".TB_PREF."stock_category.category_id AND mb_flag !='M' AND
					(stock_id LIKE '%{$val}%' OR ".TB_PREF."stock_category.description LIKE '%{$val}%' OR "
					.TB_PREF."stock_master.description LIKE '%{$val}%')";
				$result = db_query($sql);
			}
			else
				$result = false;
		}

		echo "<input type='text' name='$edit_name' size='8' maxlength='50' value='$val' > ";

		echo "<input type='submit' class='inputsubmit' name='$edit_button' value='?'> ";
	}
	else
	{
		$sql = "SELECT stock_id, ".TB_PREF."stock_master.description, ".TB_PREF."stock_category.description
			FROM ".TB_PREF."stock_master,".TB_PREF."stock_category WHERE ".TB_PREF."stock_master.category_id="
			.TB_PREF."stock_category.category_id AND mb_flag !='M'";
		$result = db_query($sql);
	}

	if ($submit_on_change==true)
		echo "<select name='$name' onchange='this.form.submit();'>";
	else if ($extra != "")
		echo "<select name='$name' $extra>";
	else
		echo "<select name='$name'>";

	if ($all_option == true)
	{
    	if ($selected_id == $all_items)
    	{
    	     echo "<option selected value='$all_items'>"._("All Items")."</option>\n";
    	}
    	else
    	{
    	     echo "<option value='$all_items'>"._("All Items")."</option>\n";
    	}
		if ($selected_id == "")
		{
			$selected_id = $all_items;
		}
	}

	while ($row = db_fetch_row($result))
	{
		if ($selected_id == $row[0])
		{
			echo "<option selected value='" . $row[0] . "'>";
		}
		else
		{
			echo "<option value='" . $row[0] . "'>";
		}
		echo (user_show_codes()?$row[0] . "&nbsp;-&nbsp;":"") . $row[2] . "&nbsp;-&nbsp;" . $row[1] . "</option>\n";

		if ($selected_id == "")
		{
			$selected_id = $row[0];
			$_POST[$name] = $selected_id;
		}
	}

	echo "</select>";
	db_free_result($result);
}

function stock_purchasable_items_list_cells($label, $name, &$selected_id, $all_option=false, $submit_on_change=false, $extra = "")
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	stock_purchasable_items_list($name, $selected_id, $all_option, $submit_on_change, $extra);
	echo "</td>\n";
}

function stock_purchasable_items_list_row($label, $name, &$selected_id, $all_option=false, $submit_on_change=false)
{
	echo "<tr>\n";
	stock_purchasable_items_list_cells($label, $name, $selected_id, $all_option, $submit_on_change);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function stock_costable_items_list($name, &$selected_id,
	$all_option=false, $submit_on_change=false)
{
	$sql = "SELECT stock_id, ".TB_PREF."stock_master.description, ".TB_PREF."stock_category.description
		FROM ".TB_PREF."stock_master,".TB_PREF."stock_category WHERE ".TB_PREF."stock_master.category_id=".TB_PREF."stock_category.category_id
		AND mb_flag !='D'";

	base_stock_items_list($sql, $name, $selected_id,
		$all_option, _("All Items"), $submit_on_change);
}

//------------------------------------------------------------------------------------

function stock_item_types_list_row($label, $name, $selected_id, $enabled=true)
{
	echo "<tr>";
	if ($label != NULL)
		echo "<td>$label</td>\n";
	echo "<td>";
	if ($enabled)
		echo "<select name='$name' onchange='this.form.submit();'>\n";
	else
		echo "<select disabled name='$name'>\n";
	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	if ($selected_id == "")
		$_POST[$name] = $selected_id = "B";
	echo "<option " . ($selected_id == 'M'?" selected ":"") . " value='M'>" . _("Manufactured"). "</option>\n";
	echo "<option " . ($selected_id == 'B'?" selected ":"") . " value='B'>" . _("Purchased"). "</option>\n";
	echo "<option " . ($selected_id == 'D'?" selected ":"") . " value='D'>" . _("Service"). "</option>\n";
	echo "</select></td></tr>\n";
}

function stock_units_list_row($label, $name, $value, $enabled=true)
{
  	global $stock_units;

	$result = get_all_item_units();
	echo "<tr><td>$label</td>\n";
	if ($enabled)
		echo "<td><select name='$name'>";
	else
		echo "<td><select disabled name='$name'>";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? "" : $_POST[$name]);

	while($unit = db_fetch($result))
	{
		if ($value == "") {
			$_POST[$name] = $unit['abbr'];
		}
		$descr = $unit['name'];
		if ($value==$unit['abbr'])
		{
			echo "<option selected value='".$unit['abbr']."'>$descr</option>\n";
		}
		else
		{
			echo "<option value='".$unit['abbr']."'>$descr</option>\n";
		}
	}
	echo "</select></td></tr>\n";
}

//------------------------------------------------------------------------------------

function tax_types_list($name, $selected_id,
	$none_option=false, $none_option_name=null, $submit_on_change=false)
{
	simple_codeandname_list("SELECT id, CONCAT(name, ' (',rate,'%)') as name FROM ".TB_PREF."tax_types",
		$name, $selected_id, $none_option, $none_option_name, true, $submit_on_change);
}

function tax_types_list_cells($label, $name, $selected_id, $none_option=false,
	$none_option_name=null, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	tax_types_list($name, $selected_id, $none_option, $none_option_name, $submit_on_change);
	echo "</td>\n";
}

function tax_types_list_row($label, $name, $selected_id, $none_option=false,
	$none_option_name=null, $submit_on_change=false)
{
	echo "<tr>\n";
	tax_types_list_cells($label, $name, $selected_id, $none_option, $none_option_name, $submit_on_change);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function tax_groups_list($name, $selected_id,
	$none_option=false, $none_option_name=null, $submit_on_change=false)
{
	simple_codeandname_list("SELECT id, name FROM ".TB_PREF."tax_groups ORDER BY id",
		$name, $selected_id, $none_option, $none_option_name, true, $submit_on_change);
}

function tax_groups_list_cells($label, $name, $selected_id, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	tax_groups_list($name, $selected_id, false, null, $submit_on_change);
	echo "</td>\n";
}

function tax_groups_list_row($label, $name, $selected_id, $submit_on_change=false)
{
	echo "<tr>\n";
	tax_groups_list_cells($label, $name, $selected_id, false, null, $submit_on_change);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function item_tax_types_list($name, $selected_id)
{
	simple_codeandname_list("SELECT id, name FROM ".TB_PREF."item_tax_types ORDER BY id",
		$name, $selected_id);
}

function item_tax_types_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	item_tax_types_list($name, $selected_id);
	echo "</td>\n";
}

function item_tax_types_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	item_tax_types_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function shippers_list($name, $selected_id)
{
	simple_codeandname_list("SELECT shipper_id, shipper_name FROM ".TB_PREF."shippers",
		$name, $selected_id);
}

function shippers_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	shippers_list($name, $selected_id);
	echo "</td>\n";
}

function shippers_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	shippers_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//-------------------------------------------------------------------------------------

function sales_persons_list($name, $selected_id)
{
	simple_codeandname_list("SELECT salesman_code, salesman_name FROM ".TB_PREF."salesman",
		$name, $selected_id);
}

function sales_persons_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>\n";
	sales_persons_list($name, $selected_id);
	echo "</td>\n";
}

function sales_persons_list_row($label, $name, $selected_id, $submit_on_change=false)
{
	echo "<tr>\n";
	sales_persons_list_cells($label, $name, $selected_id, $submit_on_change=false);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function sales_areas_list($name, $selected_id)
{
	simple_codeandname_list("SELECT area_code, description FROM ".TB_PREF."areas",
		$name, $selected_id);
}

function sales_areas_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	sales_areas_list($name, $selected_id);
	echo "</td>\n";
}

function sales_areas_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	sales_areas_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function workorders_list($name, $selected_id)
{
	simple_codeandname_list("SELECT id, wo_ref FROM ".TB_PREF."workorders WHERE closed=0",
		$name, $selected_id);
}

function workorders_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	workorders_list($name, $selected_id);
	echo "</td>\n";
}

function workorders_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	workorders_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function payment_terms_list($name, $selected_id)
{
	simple_codeandname_list("SELECT terms_indicator, terms FROM ".TB_PREF."payment_terms",
		$name, $selected_id);
}

function payment_terms_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	payment_terms_list($name, $selected_id);
	echo "</td>\n";
}

function payment_terms_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	payment_terms_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------

function credit_status_list($name, $selected_id)
{
	simple_codeandname_list("SELECT id, reason_description FROM ".TB_PREF."credit_status",
		$name, $selected_id);
}

function credit_status_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	credit_status_list($name, $selected_id);
	echo "</td>\n";
}

function credit_status_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	credit_status_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------

function sales_types_list($name, $selected_id, $submit_on_change=false)
{
	simple_codeandname_list("SELECT id, sales_type FROM ".TB_PREF."sales_types",
		$name, $selected_id, false, null, false, $submit_on_change);
}

function sales_types_list_cells($label, $name, $selected_id, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	sales_types_list($name, $selected_id, $submit_on_change);
	echo "</td>\n";
}

function sales_types_list_row($label, $name, $selected_id, $submit_on_change=false)
{
	echo "<tr>\n";
	sales_types_list_cells($label, $name, $selected_id, $submit_on_change);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------

function movement_types_list($name, $selected_id)
{
	simple_codeandname_list("SELECT id, name FROM ".TB_PREF."movement_types",
		$name, $selected_id);
}

function movement_types_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	movement_types_list($name, $selected_id);
	echo "</td>\n";
}

function movement_types_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	movement_types_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------

function bank_trans_types_list($name, $selected_id)
{
	simple_codeandname_list("SELECT id, name FROM ".TB_PREF."bank_trans_types",
		$name, $selected_id);
}

function bank_trans_types_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	bank_trans_types_list($name, $selected_id);
	echo "</td>\n";
}

function bank_trans_types_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	bank_trans_types_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------

function workcenter_list($name, $selected_id, $all_option=false)
{
	global $all_items;
	echo "<select name='$name'>";

	$sql = "SELECT id, name FROM ".TB_PREF."workcentres";
	$result = db_query($sql);

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	if ($all_option == true)
	{
    	if ($selected_id == $all_items)
    	{
    	     echo "<option selected value='$all_items'>" . _("All Work Centres") . "</option>\n";
    	}
    	else
    	{
    	     echo "<option value='$all_items'>" . _("All Work Centres") . "</option>\n";
    	}
	}

	while ($row = db_fetch_row($result))
	{
		if ($selected_id == $row[0])
		{
			echo "<option selected value='" . $row[0] . "'>";
		}
		else
		{
			echo "<option value='" . $row[0] . "'>";
		}
		echo $row[1] . "</option>\n";
	}

	echo "</select>";
	db_free_result($result);
}

function workcenter_list_cells($label, $name, $selected_id, $all_option=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	workcenter_list($name, $selected_id, $all_option);
	echo "</td>\n";
}

function workcenter_list_row($label, $name, $selected_id, $all_option=false)
{
	echo "<tr>\n";
	workcenter_list_cells($label, $name, $selected_id, $all_option);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------

function bank_accounts_list($name, $selected_id, $submit_on_change=false)
{
	if ($submit_on_change==true)
		echo "<select name='$name' onchange='this.form.submit();'>";
	else
		echo "<select name='$name'>";

	$company_currency = get_company_currency();

	$sql = "SELECT ".TB_PREF."bank_accounts.account_code, bank_account_name, bank_curr_code
		FROM ".TB_PREF."bank_accounts, ".TB_PREF."chart_master
		WHERE ".TB_PREF."bank_accounts.account_code=".TB_PREF."chart_master.account_code";
	$result = db_query($sql);

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	while ($row = db_fetch_row($result))
	{
		if ($selected_id == $row[0])
		{
			echo "<option selected value='" . $row[0] . "'>";
		}
		else
		{
			echo "<option value='" . $row[0] . "'>";
		}
		echo $row[1];
		if ($company_currency != $row[2])
			echo "&nbsp;-&nbsp;" . $row[2];
		echo  "</option>\n";
		if ($selected_id == "")
		{
			$selected_id = $row[0];
			$_POST[$name] = $selected_id;
		}
	}

	echo "</select>";
	db_free_result($result);
}

function bank_accounts_list_cells($label, $name, $selected_id, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	bank_accounts_list($name, $selected_id, $submit_on_change);
	echo "</td>\n";
}

function bank_accounts_list_row($label, $name, $selected_id, $submit_on_change=false)
{
	echo "<tr>\n";
	bank_accounts_list_cells($label, $name, $selected_id, $submit_on_change);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------

function class_list($name, $selected_id, $submit_on_change=false)
{
	if ($submit_on_change==true)
		echo "<select name='$name' onchange='this.form.submit();'>";
	else
		echo "<select name='$name'>";

	$sql = "SELECT cid, class_name FROM ".TB_PREF."chart_class";
	$result = db_query($sql);

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	while ($row = db_fetch_row($result))
	{
		if ($selected_id == $row[0])
		{
			echo "<option selected value='" . $row[0] . "'>";
		}
		else
		{
			echo "<option value='" . $row[0] . "'>";
		}
		echo $row[1] . "</option>\n";
		if ($selected_id == "")
		{
			$selected_id = $row[0];
			$_POST[$name] = $selected_id;
		}
	}

	echo "</select>";
	db_free_result($result);
}

function class_list_cells($label, $name, $selected_id, $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	class_list($name, $selected_id, $submit_on_change);
	echo "</td>\n";
}

function class_list_row($label, $name, $selected_id, $submit_on_change=false)
{
	echo "<tr>\n";
	class_list_cells($label, $name, $selected_id, $submit_on_change);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------

function stock_categories_list($name, $selected_id)
{
	simple_codeandname_list("SELECT category_id, description FROM ".TB_PREF."stock_category
		ORDER BY category_id", $name, $selected_id);
}

function stock_categories_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	stock_categories_list($name, $selected_id);
	echo "</td>\n";
}

function stock_categories_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	stock_categories_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------

function gl_account_types_list($name, $selected_id, $all_option, $all_option_name,
	$all_option_numeric)
{
	simple_codeandname_list("SELECT id, name FROM ".TB_PREF."chart_types ORDER BY id",
		$name, $selected_id, $all_option, $all_option_name, $all_option_numeric);
}

function gl_account_types_list_cells($label, $name, $selected_id, $all_option=false, $all_option_name=null,
	$all_option_numeric=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	gl_account_types_list($name, $selected_id, $all_option, $all_option_name, $all_option_numeric);
	echo "</td>\n";
}

function gl_account_types_list_row($label, $name, $selected_id, $all_option=false, $all_option_name=null,
	$all_option_numeric=false)
{
	echo "<tr>\n";
	gl_account_types_list_cells($label, $name, $selected_id, $all_option,
		$all_option_name, $all_option_numeric);
	echo "</tr>\n";
}

//-----------------------------------------------------------------------------------------------

function gl_all_accounts_list($name, $selected_id, $skip_bank_accounts=false,
	$show_group=false, $onchange="")
{
	echo "<select name='$name'";
	if ($onchange != "")
		echo " onchange='$onchange'";
	echo ">";

	if ($skip_bank_accounts)
		$sql = "SELECT ".TB_PREF."chart_master.account_code, ".TB_PREF."chart_master.account_name, ".TB_PREF."chart_types.name
			FROM (".TB_PREF."chart_master,".TB_PREF."chart_types) LEFT JOIN ".TB_PREF."bank_accounts ON ".TB_PREF."chart_master.account_code=".TB_PREF."bank_accounts.account_code
            WHERE ".TB_PREF."bank_accounts.account_code IS NULL
			AND ".TB_PREF."chart_master.account_type=".TB_PREF."chart_types.id
			ORDER BY account_code;";
	else
		$sql = "SELECT account_code, account_name,".TB_PREF."chart_types.name
			FROM ".TB_PREF."chart_master, ".TB_PREF."chart_types
			WHERE ".TB_PREF."chart_master.account_type=".TB_PREF."chart_types.id
			ORDER BY account_code";

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	$result = db_query($sql, "query chart master");

	while ($row = db_fetch_row($result))
	{
		if ($selected_id == $row[0])
		{
			echo "<option selected value='" . $row[0] . "'>";
		}
		else
		{
			echo "<option value='" . $row[0] . "'>";
		}
		//echo str_pad($row[0],6,'0', STR_PAD_LEFT) .  "&nbsp;&nbsp;&nbsp;&nbsp;" . $row[2] . "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" . $row[1];
		if ($show_group)
			echo $row[0] .  "&nbsp;&nbsp;&nbsp;&nbsp;" . $row[2] . "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" . $row[1] . "</option>\n";
		else
			echo $row[0] .  "&nbsp;&nbsp;&nbsp;&nbsp;" . $row[1] . "</option>\n";

		if ($selected_id == "")
		{
			$selected_id = $row[0];
			$_POST[$name] = $selected_id;
		}
	}

	echo "</select>";
	db_free_result($result);
}

function gl_all_accounts_list_cells($label, $name, $selected_id, $skip_bank_accounts=false,
	$show_group=false, $onchange="")
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	gl_all_accounts_list($name, $selected_id, $skip_bank_accounts, $show_group, $onchange);
	echo "</td>\n";
}

function gl_all_accounts_list_row($label, $name, $selected_id, $skip_bank_accounts=false,
	$show_group=false, $onchange="")
{
	echo "<tr>\n";
	gl_all_accounts_list_cells($label, $name, $selected_id, $skip_bank_accounts,
		$show_group, $onchange);
	echo "</tr>\n";
}

function yesno_list($name, $selected_id, $name_yes="", $name_no="", $submit_on_change=false)
{
	if ($submit_on_change == true)
		echo "<select name='$name' onchange='this.form.submit();'>";
	else
		echo "<select name='$name'>";

	if (strlen($name_yes) == 0)
	{
		unset($name_yes);
	}
	if (strlen($name_no) == 0)
	{
		unset($name_no);
	}

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? 0 : $_POST[$name]);
    if ($selected_id == 0)
    	echo "<option value=1>";
    else
    	echo "<option selected value=1>";
	if (!isset($name_yes))
		echo _("Yes") . "</option>\n";
	else
		echo $name_yes . "</option>\n";
	if ($selected_id == 0)
		echo "<option selected value=0>";
	else
    	echo "<option value=0>";
	if (!isset($name_no))
		echo _("No") . "</option>\n";
	else
		echo $name_no . "</option>\n";
	echo "</select>";
}

function yesno_list_cells($label, $name, $selected_id, $name_yes="", $name_no="", $submit_on_change=false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	yesno_list($name, $selected_id, $name_yes, $name_no, $submit_on_change);
	echo "</td>\n";
}

function yesno_list_row($label, $name, $selected_id, $name_yes="", $name_no="", $submit_on_change=false)
{
	echo "<tr>\n";
	yesno_list_cells($label, $name, $selected_id, $name_yes, $name_no, $submit_on_change);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------------------

function languages_list($name, &$selected_id)
{
  	global $installed_languages;

	echo "<select name='$name'>";

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	foreach ($installed_languages as $lang)
	{
	  	echo "<option ";
		if ($selected_id == $lang['code'])
			echo "selected ";
		echo "value='" . $lang['code'] . "'>" . $lang['name'] . "</option>\n";
	}
}

function languages_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	languages_list($name, $selected_id);
	echo "</td>\n";
}

function languages_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	languages_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------------------

function bank_account_types_list($name, &$selected_id)
{
  	$bank_account_types = bank_account_types::get_all();

  	echo "<select name='$name'>";

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
  	foreach ($bank_account_types as $type)
  	{
      	echo "<option ";
      	if ($selected_id == "" || $selected_id == $type['id'])
      		echo "selected ";
      	echo "value='" . $type['id'] . "'>" . $type['name'] . "</option>\n";
  	}
}

function bank_account_types_list_cells($label, $name, $selected_id)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	bank_account_types_list($name, $selected_id);
	echo "</td>\n";
}

function bank_account_types_list_row($label, $name, $selected_id)
{
	echo "<tr>\n";
	bank_account_types_list_cells($label, $name, $selected_id);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------------------

function payment_person_types_list($name, $selected_id, $related=null)
{
  	$types = payment_person_types::get_all();

  	echo "<select name='$name'";
  	if ($related)
  		echo " onchange='this.form.$related.value=\"\"; this.form.submit();' ";
  	echo ">";

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
  	foreach ($types as $type)
  	{
		if (payment_person_types::has_items($type['id']))
		{
			if ($selected_id == "")
				$_POST[$name] = $selected_id = $type['id'];
  		    echo "<option ";
  	    	if ($selected_id == $type['id'])
  	    		echo "selected ";
  		    echo "value='" . $type['id'] . "'>" . $type['name'] . "</option>\n";
		}
  	}
}

function payment_person_types_list_cells($label, $name, $selected_id, $related=null)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	payment_person_types_list($name, $selected_id, $related);
	echo "</td>\n";
}

function payment_person_types_list_row($label, $name, $selected_id, $related=null)
{
	echo "<tr>\n";
	payment_person_types_list_cells($label, $name, $selected_id, $related);
	echo "</tr>\n";
}

//------------------------------------------------------------------------------------------------

function wo_types_list($name, &$selected_id)
{
  	$types = wo_types::get_all();

  	echo "<select name='$name' onchange='this.form.submit();'>";

	if ($selected_id == null)
		$selected_id = (!isset($_POST[$name]) ? "" : $_POST[$name]);
  	foreach ($types as $type)
  	{
  	    echo "<option ";
  	    if ($selected_id == $type['id'])
  	    	echo "selected ";
  	    echo "value='" . $type['id'] . "'>" . $type['name'] . "</option>\n";
  	}
	echo "</select>";
}

function wo_types_list_row($label, $name, &$selected_id)
{
	echo "<tr><td>$label</td><td>\n";
	wo_types_list($name, $selected_id);
	echo "</td></tr>\n";
}

//------------------------------------------------------------------------------------------------

function dateformats_list_row($label, $name, $value)
{
  	global $dateformats;

	echo "<tr><td>$label</td>\n";
	echo "<td><select name='$name'>";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? 0 : $_POST[$name]);
	$counter = 0;
	foreach ($dateformats as $df)
	{

		if ($value==$counter)
		{
			echo "<option selected value='$counter'>$df</option>\n";
		}
		else
		{
			echo "<option value='$counter'>$df</option>\n";
		}
		$counter++;
	}
	echo "</select></td></tr>\n";
}

function dateseps_list_row($label, $name, $value)
{
  	global $dateseps;

	echo "<tr><td>$label</td>\n";
	echo "<td><select name='$name'>";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? 0 : $_POST[$name]);
	$counter = 0;
	foreach ($dateseps as $ds)
	{

		if ($value==$counter)
		{
			echo "<option selected value='$counter'>$ds</option>\n";
		}
		else
		{
			echo "<option value='$counter'>$ds</option>\n";
		}
		$counter++;
	}
	echo "</select></td></tr>\n";
}

function thoseps_list_row($label, $name, $value)
{
  	global $thoseps;

	echo "<tr><td>$label</td>\n";
	echo "<td><select name='$name'>";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? 0 : $_POST[$name]);
	$counter = 0;
	foreach ($thoseps as $ts)
	{

		if ($value==$counter)
		{
			echo "<option selected value='$counter'>$ts</option>\n";
		}
		else
		{
			echo "<option value='$counter'>$ts</option>\n";
		}
		$counter++;
	}
	echo "</select></td></tr>\n";
}

function decseps_list_row($label, $name, $value)
{
  	global $decseps;

	echo "<tr><td>$label</td>\n";
	echo "<td><select name='$name'>";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? 0 : $_POST[$name]);
	$counter = 0;
	foreach ($decseps as $ds)
	{

		if ($value==$counter)
		{
			echo "<option selected value='$counter'>$ds</option>\n";
		}
		else
		{
			echo "<option value='$counter'>$ds</option>\n";
		}
		$counter++;
	}
	echo "</select></td></tr>\n";
}

function themes_list_row($label, $name, $value)
{
	global $path_to_root;
	
	$path = $path_to_root.'/themes/';
	$themes = array();
	$themedir = opendir($path);
	while(false !== ($fname = readdir($themedir))) {
	    if($fname!='.' && $fname!='..' && $fname!='CVS' && is_dir($path.$fname)) {
			$themes[] =  $fname;
	    } 
	}
	sort($themes);
	echo "<tr><td>$label</td>\n";
	echo "<td><select name='$name'>";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	foreach ($themes as $th)
	{

		if ($value==$th)
		{
			echo "<option selected value='$th'>$th</option>\n";
		}
		else
		{
			echo "<option value='$th'>$th</option>\n";
		}
	}
	echo "</select></td></tr>\n";
}

function pagesizes_list_row($label, $name, $value)
{
  	global $pagesizes;

	echo "<tr><td>$label</td>\n";
	echo "<td><select name='$name'>";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	foreach ($pagesizes as $pz)
	{

		if ($value==$pz)
		{
			echo "<option selected value='$pz'>$pz</option>\n";
		}
		else
		{
			echo "<option value='$pz'>$pz</option>\n";
		}
	}
	echo "</select></td></tr>\n";
}

function security_headings_list_row($label, $name, $value)
{
  	global $security_headings;

	echo "<tr><td>$label</td>\n";
	echo "<td><select name='$name'>";

	if ($value == null)
		$value = (!isset($_POST[$name]) ? 0 : (int)$_POST[$name]);
	$counter=0;
	foreach ($security_headings as $sh)
	{

		if ($value==$counter)
		{
			echo "<option selected value='$counter'>$sh</option>\n";
		}
		else
		{
			echo "<option value='$counter'>$sh</option>\n";
		}
		$counter++;
	}
	echo "</select></td></tr>\n";
}

function systypes_list_cells($label, $name, $value, $submit_on_change=false)
{
  	global $systypes_array;

	if ($label != null)
		echo "<td>$label</td>\n";
    echo "<td><select name='$name'";
    if ($submit_on_change)
    	echo " onchange='this.form.submit();'>";
    else
    	echo ">";
	if ($value == null)
		$value = (!isset($_POST[$name]) ? "" : $_POST[$name]);
    foreach ($systypes_array as $key=>$type)
    {

		if ($value==$key)
		{
			echo "<option selected value='$key'>".$type['name']."</option>\n";
		}
		else
		{
			echo "<option value='$key'>".$type['name']."</option>\n";
		}
    }
    echo "</select></td>\n";
}

function systypes_list_row($label, $name, $value, $submit_on_change=false)
{
	echo "<tr>\n";
	systypes_list_cells($label, $name, $value, $submit_on_change);
	echo "</tr>\n";
}

function cust_allocations_list_cells($label, $name, $selected)
{
	global $all_items;
	if ($label != null)
		label_cell($label);
	if ($selected == null)
		$selected = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	echo "<td><select name='$name'>";
	echo "<option " . ($selected == $all_items ? " selected " : "") . " value='$all_items'>" . _("All Types"). "</option>\n";
	echo "<option " . ($selected == '1'?" selected ":"") . " value='1'>" . _("Sales Invoices"). "</option>\n";
	echo "<option " . ($selected == '2'?" selected ":"") . " value='2'>" . _("Overdue Invoices"). "</option>\n";
	echo "<option " . ($selected == '3'?" selected ":"") . " value='3'>" . _("Payments"). "</option>\n";
	echo "<option " . ($selected == '4'?" selected ":"") . " value='4'>" . _("Credit Notes"). "</option>\n";
	echo "<option " . ($selected == '5'?" selected ":"") . " value='5'>" . _("Delivery Notes"). "</option>\n";
	echo "</select></td>\n";
}

function supp_allocations_list_cells($name, $selected)
{
	global $all_items;
	if ($selected == null)
		$selected = (!isset($_POST[$name]) ? "" : $_POST[$name]);
	echo "<td><select name='$name'>";
	echo "<option " . ($selected == $all_items ? " selected " : "") . " value='$all_items'>" . _("All Types"). "</option>\n";
	echo "<option " . ($selected == '1'?" selected ":"") . " value='1'>" . _("Invoices"). "</option>\n";
	echo "<option " . ($selected == '2'?" selected ":"") . " value='2'>" . _("Overdue Invoices"). "</option>\n";
	echo "<option " . ($selected == '3'?" selected ":"") . " value='3'>" . _("Payments"). "</option>\n";
	echo "<option " . ($selected == '4'?" selected ":"") . " value='4'>" . _("Credit Notes"). "</option>\n";
	echo "<option " . ($selected == '5'?" selected ":"") . " value='5'>" . _("Overdue Credit Notes"). "</option>\n";
	echo "</select></td>\n";
}

function policy_list_cells($label, $name, $selected)
{
	if ($selected == null)
	{
		$selected = (!isset($_POST[$name]) ? "" : $_POST[$name]);
		if ($selected == "")
			$_POST[$name] = $selected;
	}
	if ($label != null)
		label_cell($label);
	echo "<td><select name='$name'>";
	echo "<option " . ($selected == ''?" selected ":"") . " value=''>" . _("Automatically put balance on back order"). "</option>\n";
	echo "<option " . ($selected == 'CAN'?" selected ":"") . " value='CAN'>" . _("Cancel any quantites not delivered"). "</option>\n";
	echo "</select></td>\n";
}

function policy_list_row($label, $name, $selected)
{
	echo "<tr>\n";
	policy_list_cells($label, $name, $selected);
	echo "</tr>\n";
}

function credit_type_list_cells($label, $name, $selected, $submit_on_change=false)
{
	if ($selected == null)
	{
		$selected = (!isset($_POST[$name]) ? "Return" : $_POST[$name]);
		if ($selected == "Return")
			$_POST[$name] = $selected;
	}
	if ($label != null)
		label_cell($label);
    echo "<td><select name='$name'";
    if ($submit_on_change)
    	echo " onchange='this.form.submit();'>";
    else
    	echo ">";
	echo "<option " . ($selected == 'Return'?" selected ":"") . " value='Return'>" . _("Items Returned to Inventory Location"). "</option>\n";
	echo "<option " . ($selected == 'WriteOff'?" selected ":"") . " value='WriteOff'>" . _("Items Written Off"). "</option>\n";
	echo "</select></td>\n";
}

function credit_type_list_row($label, $name, $selected, $submit_on_change=false)
{
	echo "<tr>\n";
	credit_type_list_cells($label, $name, $selected, $submit_on_change);
	echo "</tr>\n";
}

function number_list($name, $selected, $from, $to, $firstlabel="")
{
	if ($selected == null)
	{
		$selected = (!isset($_POST[$name]) ? $from : $_POST[$name]);
		if ($selected == $from)
			$_POST[$name] = $selected;
	}
	echo "<select name='$name'>";
	for ($i = $from; $i <= $to; $i++)
    {
		if ($i == 0 && $firstlabel != "")
			$label = $firstlabel;
		else
			$label = $i;
		if ($selected == $i)
		{
			echo "<option selected value='$i'>$label</option>\n";
		}
		else
		{
			echo "<option value='$i'>$label</option>\n";
		}
    }
	echo "</select>\n";
}

function number_list_cells($label, $name, $selected, $from, $to)
{
	if ($label != null)
		label_cell($label);
	echo "<td>\n";
	number_list($name, $selected, $from, $to);
	echo "</td>\n";
}

function number_list_row($label, $name, $selected, $from, $to)
{
	echo "<tr>\n";
	number_list_cells($label, $name, $selected, $from, $to);
	echo "</tr>\n";
}
?>