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

function dashboard($sel_app)
{
	$selected_app = $sel_app;


	if (!$_SESSION["wa_current_user"]->check_application_access($selected_app))
		return;
	// first have a look through the directory, 
	// and remove old temporary pdfs and pngs
	$dir = company_path(). '/pdf_files';

	if ($d = @opendir($dir)) {
		while (($file = readdir($d)) !== false) {
			if (!is_file($dir.'/'.$file) || $file == 'index.php') continue;
		// then check to see if this one is too old
			$ftime = filemtime($dir.'/'.$file);
		 // seems 3 min is enough for any report download, isn't it?
			if (time()-$ftime > 180){
				unlink($dir.'/'.$file);
			}
		}
		closedir($d);
	}

	//check_for_overdue_recurrent_invoices();
	if ($selected_app == "orders")
		display_customer_topten();
	elseif ($selected_app == "AP")
		display_supplier_topten();
	elseif ($selected_app == "stock")
		display_stock_topten();
	elseif ($selected_app == "manuf")
		display_stock_topten(1);
	elseif ($selected_app == "assets")
		display_stock_topten(2);
	elseif ($selected_app == "proj")
		display_dimension_topten();
	elseif ($selected_app == "GL")
		display_gl_info();
	else	
		display_all();
}

function display_title($title, $colspan=2)
{
	echo "<tr><td colspan=$colspan class='headingtext' style='text-align:center;border:0;height:40px;'>$title</td></tr>\n";
}	

function table_one()
{
	echo "<table width='100%'>";
	echo "<tr valign=top><td style='width:50%'>\n"; // outer table
}

function table_two()
{
	echo "</td><td style='width:50%'>\n";
}

function table_end()
{
	echo "</td></tr></table>\n";
}

function display_customer_topten()
{
	$pg = new graph();

	table_one();
	$today = Today();
	$title = customer_top($today, 10, 66, $pg);
	customer_trans($today);
	table_two();
	source_graphic($today, $title, _("Customer"), $pg);
	customer_recurrent_invoices($today);
	table_end();
}

function display_supplier_topten()
{
	$pg = new graph();

	$today = Today();
	table_one();
	$title = supplier_top($today, 10, 66, $pg);
	supplier_trans($today);
	table_two();
	source_graphic($today, $title, _("Supplier"), $pg);
	table_end();
}

function display_stock_topten($type=0)
{
	$pg = new graph();

	table_one();
	$today = Today();
	$title = stock_top($today, 10, 66, $type, $pg);
	table_two();
	if ($type == 2)
		$source = _("Fixed Assets");
	elseif ($type == 1)
		$source = _("Manufacturing");
	else	
		$source = _("Items");
	source_graphic($today, $title, $source, $pg);
	table_end();
}

function display_dimension_topten()
{
	$pg = new graph();

	table_one();
	$today = Today();
	$title = dimension_top($today, 10, 66, $pg);
	table_two();
	source_graphic($today, $title, _("Dimension"), $pg, 5);
	table_end();
}	

function display_gl_info()
{
	$pg = new graph();

	table_one();
	$today = Today();
	$title = gl_top($today, 66, $pg);
	bank_balance($today, 66);
	table_two();
	source_graphic($today, $title, _("Class"), $pg, 5);
	table_end();
}	

function display_all()
{
	$today = Today();
	$pg = new graph();

	table_one();
	$title = customer_top($today, 3, 66, $pg);
	source_graphic($today, $title, _("Customer"), $pg);
	$title = supplier_top($today, 3, 66, $pg);
	source_graphic($today, $title, _("Supplier"), $pg);
	$title = stock_top($today, 3, 66, 0, $pg);
	source_graphic($today, $title, _("Stock"), $pg);
	table_two();
	dimension_top($today, 3, 66);
	$title = gl_top($today, 66, $pg);
	source_graphic($today, $title, _("Class"), $pg, 5);
	stock_top($today, 3, 66, 2);
	stock_top($today, 3, 66, 1);
	table_end();
}

function customer_top($today, $limit=10, $width="33", &$pg=null)
{
	$begin = begin_fiscalyear();
	$begin1 = date2sql($begin);
	$today1 = date2sql($today);
	$sql = "SELECT SUM((ov_amount + ov_discount) * rate * IF(trans.type = ".ST_CUSTCREDIT.", -1, 1)) AS total,d.debtor_no, d.name FROM
		".TB_PREF."debtor_trans AS trans, ".TB_PREF."debtors_master AS d WHERE trans.debtor_no=d.debtor_no
		AND (trans.type = ".ST_SALESINVOICE." OR trans.type = ".ST_CUSTCREDIT.")
		AND tran_date >= '$begin1' AND tran_date <= '$today1' GROUP by d.debtor_no ORDER BY total DESC, d.debtor_no 
		LIMIT $limit";
	$result = db_query($sql);
	$title = _("Top $limit customers in fiscal year");
	$th = array(_("Customer"), _("Amount"));
	start_table(TABLESTYLE, "width='$width%'");
	display_title($title, count($th));
	table_header($th);
	check_page_security('SA_SALESTRANSVIEW');
	$k = 0; //row colour counter
	$i = 0;
	while ($myrow = db_fetch($result))
	{
		alt_table_row_color($k);
		$name = $myrow["debtor_no"]." ".$myrow["name"];
		label_cell($name);
		amount_cell($myrow['total']);
		if ($pg != null)
		{
			$pg->x[$i] = $name; 
			$pg->y[$i] = $myrow['total'];
		}	
		$i++;
		end_row();
	}
	end_table(2);
	return $title;
}

function supplier_top($today, $limit=10, $width="33", &$pg=null)
{
	$begin = begin_fiscalyear();
	$begin1 = date2sql($begin);
	$today1 = date2sql($today);
	$sql = "SELECT SUM((trans.ov_amount + trans.ov_discount) * rate) AS total, s.supplier_id, s.supp_name FROM
		".TB_PREF."supp_trans AS trans, ".TB_PREF."suppliers AS s WHERE trans.supplier_id=s.supplier_id
		AND (trans.type = ".ST_SUPPINVOICE." OR trans.type = ".ST_SUPPCREDIT.")
		AND tran_date >= '$begin1' AND tran_date <= '$today1' GROUP by s.supplier_id ORDER BY total DESC, s.supplier_id 
		LIMIT $limit";
	$result = db_query($sql);
	$title = _("Top $limit suppliers in fiscal year");
	$th = array(_("Supplier"), _("Amount"));
	start_table(TABLESTYLE, "width='$width%'");
	display_title($title, count($th));
	table_header($th);
	check_page_security('SA_SUPPTRANSVIEW');
	$k = 0; //row colour counter
	$i = 0;
	while ($myrow = db_fetch($result))
	{
		alt_table_row_color($k);
		$name = $myrow["supplier_id"]." ".$myrow["supp_name"];
		label_cell($name);
		amount_cell($myrow['total']);
		if ($pg != null)
		{
			$pg->x[$i] = $name; 
			$pg->y[$i] = $myrow['total'];
		}	
		$i++;
		end_row();
	}
	end_table(2);
	return $title;
}

function stock_top($today, $limit=10, $width="33", $type=0, &$pg=null)
{
	if ($type == 2)
		$sec = 'SA_ASSETSANALYTIC';
	elseif ($type == 1)
		$sec = 'SA_WORKORDERANALYTIC';
	else
		$sec = 'SA_ITEMSTRANSVIEW';
	$begin = begin_fiscalyear();
	$begin1 = date2sql($begin);
	$today1 = date2sql($today);
	if ($type == 0)
	{
		$sql = "SELECT SUM((trans.unit_price * trans.quantity) * d.rate) AS total, s.stock_id, s.description, 
			SUM(trans.quantity) AS qty FROM
			".TB_PREF."debtor_trans_details AS trans, ".TB_PREF."stock_master AS s, ".TB_PREF."debtor_trans AS d 
			WHERE trans.stock_id=s.stock_id AND trans.debtor_trans_type=d.type AND trans.debtor_trans_no=d.trans_no
			AND (d.type = ".ST_SALESINVOICE." OR d.type = ".ST_CUSTCREDIT.") ";
	}
	else
	{
		$sql = "SELECT SUM(m.qty * (s.material_cost + s.labour_cost + s.overhead_cost)) AS total, s.stock_id, s.description, 
			SUM(qty) AS qty FROM ".TB_PREF."stock_master AS s, ".TB_PREF."stock_moves AS m 
			WHERE s.stock_id=m.stock_id ";
		if ($type == 1)
			$sql .= "AND s.mb_flag='M' ";
		elseif ($type == 2)	
			$sql .= "AND s.mb_flag='F' ";
	}
	$sql .= "AND tran_date >= '$begin1' AND tran_date <= '$today1' GROUP by s.stock_id ORDER BY total DESC, s.stock_id 
		LIMIT $limit";
	$result = db_query($sql);
	if ($type == 1)
		$title = _("Top $limit Manufactured Items in fiscal year");
	elseif ($type == 2)
		$title = _("Top $limit Fixed Assets in fiscal year");
	else	
		$title = _("Top $limit Sold Items in fiscal year");
	$th = array(_("Item"), _("Amount"), _("Quantity"));
	start_table(TABLESTYLE, "width='$width%'");
	display_title($title, count($th));	
	table_header($th);
	check_page_security($sec);
	$k = 0; //row colour counter
	$i = 0;
	while ($myrow = db_fetch($result))
	{
		alt_table_row_color($k);
		$name = $myrow["description"];
		label_cell($name);
		amount_cell($myrow['total']);
		qty_cell($myrow['qty']);
		if ($pg != NULL)
		{
			$pg->x[$i] = $name; 
			$pg->y[$i] = $myrow['total'];
		}	
		$i++;
		end_row();
	}
	end_table(2);
}

function dimension_top($today, $limit=10, $width="33", &$pg=null)
{

	$begin = begin_fiscalyear();
	$begin1 = date2sql($begin);
	$today1 = date2sql($today);
	$sql = "SELECT SUM(-t.amount) AS total, d.reference, d.name FROM
		".TB_PREF."gl_trans AS t,".TB_PREF."dimensions AS d WHERE
		(t.dimension_id = d.id OR t.dimension2_id = d.id) AND
		t.tran_date >= '$begin1' AND t.tran_date <= '$today1' GROUP BY d.id ORDER BY total DESC LIMIT $limit";
	$result = db_query($sql, "Transactions could not be calculated");
	$title = _("Top $limit Dimensions in fiscal year");
	$th = array(_("Dimension"), _("Amount"));
	start_table(TABLESTYLE, "width='$width%'");
	display_title($title, count($th));
	table_header($th);
	check_page_security('SA_DIMTRANSVIEW');
	$k = 0; //row colour counter
	$i = 0;
	while ($myrow = db_fetch($result))
	{
		alt_table_row_color($k);
		$name = $myrow['reference']." ".$myrow["name"];
		label_cell($name);
		amount_cell($myrow['total']);
		if ($pg != null)
		{
			$pg->x[$i] = $name; 
			$pg->y[$i] = abs($myrow['total']);
		}	
		$i++;
		end_row();
	}
	end_table(2);
}

function gl_top($today, $width="33", &$pg=null)
{
	$begin = begin_fiscalyear();
	$begin1 = date2sql($begin);
	$today1 = date2sql($today);
	$sql = "SELECT SUM(amount) AS total, c.class_name, c.ctype FROM
		".TB_PREF."gl_trans,".TB_PREF."chart_master AS a, ".TB_PREF."chart_types AS t, 
		".TB_PREF."chart_class AS c WHERE
		account = a.account_code AND a.account_type = t.id AND t.class_id = c.cid
		AND IF(c.ctype > 3, tran_date >= '$begin1', tran_date >= '0000-00-00') 
		AND tran_date <= '$today1' GROUP BY c.cid ORDER BY c.cid"; 
	$result = db_query($sql, "Transactions could not be calculated");
	$title = _("Class Balances");
	start_table(TABLESTYLE2, "width='$width%'");
	display_title($title);
	check_page_security('SA_GLANALYTIC');
	$i = 0;
	$total = 0;
	while ($myrow = db_fetch($result))
	{
		if ($myrow['ctype'] > 3)
		{
			$total += $myrow['total'];
			$myrow['total'] = -$myrow['total'];
			if ($pg != null)
			{
				$pg->x[$i] = $myrow['class_name']; 
				$pg->y[$i] = abs($myrow['total']);
			}	
			$i++;
		}	
		label_row($myrow['class_name'], number_format2($myrow['total'], user_price_dec()), 
			"class='label' style='font-weight:bold;'", "style='font-weight:bold;' align=right");
	}
	$calculated = _("Calculated Return");
	label_row("&nbsp;", "");
	label_row($calculated, number_format2(-$total, user_price_dec()), 
		"class='label' style='font-weight:bold;'", "style='font-weight:bold;' align=right");
	if ($pg != null)
	{
		$pg->x[$i] = $calculated; 
		$pg->y[$i] = -$total;
	}
	end_table(2);
}

function source_graphic($today, $title, $source, $pg, $type=2)
{
	$pg->title     = $title;
	$pg->axis_x    = $source;
	$pg->axis_y    = _("Amount");
	$pg->graphic_1 = $today;
	$pg->type      = $type;
	$pg->skin      = 1;
	$pg->built_in  = false;
	$filename = company_path(). "/pdf_files/". uniqid("").".png";
	$pg->display($filename, true);
	start_table(TABLESTYLE);
	start_row();
	echo "<td>";
	echo "<img src='$filename' border='0' alt='$title'>";
	echo "</td>";
	end_row();
	end_table(1);
}

function customer_trans($today)
{
	$today = date2sql($today);

	$sql = "SELECT trans.trans_no, trans.reference,	trans.tran_date, trans.due_date, debtor.debtor_no, 
		debtor.name, branch.br_name, debtor.curr_code,
		(trans.ov_amount + trans.ov_gst + trans.ov_freight 
			+ trans.ov_freight_tax + trans.ov_discount)	AS total,  
		(trans.ov_amount + trans.ov_gst + trans.ov_freight 
			+ trans.ov_freight_tax + trans.ov_discount - trans.alloc) AS remainder,
		DATEDIFF('$today', trans.due_date) AS days 	
		FROM ".TB_PREF."debtor_trans as trans, ".TB_PREF."debtors_master as debtor, 
			".TB_PREF."cust_branch as branch
		WHERE debtor.debtor_no = trans.debtor_no AND trans.branch_code = branch.branch_code
			AND trans.type = ".ST_SALESINVOICE." AND (trans.ov_amount + trans.ov_gst + trans.ov_freight 
			+ trans.ov_freight_tax + trans.ov_discount - trans.alloc) > ".FLOAT_COMP_DELTA." 
			AND DATEDIFF('$today', trans.due_date) > 0 ORDER BY days DESC";
	$result = db_query($sql);
	$title = db_num_rows($result) . _(" overdue Sales Invoices");
	br(1);
	display_heading($title);
	br();
	$th = array("#", _("Ref."), _("Date"), _("Due Date"), _("Customer"), _("Branch"), _("Currency"), 
		_("Total"), _("Remainder"),	_("Days"));
	start_table(TABLESTYLE);
	table_header($th);
	$k = 0; //row colour counter
	while ($myrow = db_fetch($result))
	{
		alt_table_row_color($k);
		label_cell(get_trans_view_str(ST_SALESINVOICE, $myrow["trans_no"]));
		label_cell($myrow['reference']);
		label_cell(sql2date($myrow['tran_date']));
		label_cell(sql2date($myrow['due_date']));
		$name = $myrow["debtor_no"]." ".$myrow["name"];
		label_cell($name);
		label_cell($myrow['br_name']);
		label_cell($myrow['curr_code']);
		amount_cell($myrow['total']);
		amount_cell($myrow['remainder']);
		label_cell($myrow['days'], "align='right'");
		end_row();
	}
	end_table(2);
}

function calculate_next_invoice($myrow)
{
	if ($myrow["last_sent"] == '0000-00-00')
		$next = sql2date($myrow["begin"]);
	else
		$next = sql2date($myrow["last_sent"]);
	$next = add_months($next, $myrow['monthly']);
	$next = add_days($next, $myrow['days']);
	return add_days($next,-1);
}

function customer_recurrent_invoices($today)
{
	$result = get_recurrent_invoices($today);
	$title = _("Overdue Recurrent Invoices");
	br(1);
	display_heading($title);
	br();
	$th = array(_("Description"), _("Template No"),_("Customer"),_("Branch")."/"._("Group"),_("Next invoice"));
	start_table(TABLESTYLE, "width=70%");
	table_header($th);
	$k = 0;
	while ($myrow = db_fetch($result)) 
	{
		if (!$myrow['overdue'])
			continue;
		alt_table_row_color($k);

		label_cell($myrow["description"]);
		label_cell(get_customer_trans_view_str(ST_SALESORDER, $myrow["order_no"]));
		if ($myrow["debtor_no"] == 0)
		{
			label_cell("");

			label_cell(get_sales_group_name($myrow["group_no"]));
		}
		else
		{
			label_cell(get_customer_name($myrow["debtor_no"]));
			label_cell(get_branch_name($myrow['group_no']));
		}
		label_cell(calculate_next_invoice($myrow),  "align='center'");
		end_row();
	}
	end_table(2);
}

function supplier_trans($today)
{
	$today = date2sql($today);
	$sql = "SELECT trans.trans_no, trans.reference, trans.tran_date, trans.due_date, s.supplier_id, 
		s.supp_name, s.curr_code,
		(trans.ov_amount + trans.ov_gst + trans.ov_discount) AS total,  
		(trans.ov_amount + trans.ov_gst + trans.ov_discount - trans.alloc) AS remainder,
		DATEDIFF('$today', trans.due_date) AS days 	
		FROM ".TB_PREF."supp_trans as trans, ".TB_PREF."suppliers as s 
		WHERE s.supplier_id = trans.supplier_id
			AND trans.type = ".ST_SUPPINVOICE." AND (ABS(trans.ov_amount + trans.ov_gst + 
				trans.ov_discount) - trans.alloc) > ".FLOAT_COMP_DELTA."
			AND DATEDIFF('$today', trans.due_date) > 0 ORDER BY days DESC";
	$result = db_query($sql);
	$title = db_num_rows($result) . _(" overdue Purchase Invoices");
	br(1);
	display_heading($title);
	br();
	$th = array("#", _("Ref."), _("Date"), _("Due Date"), _("Supplier"), _("Currency"), _("Total"), 
		_("Remainder"),	_("Days"));
	start_table(TABLESTYLE);
	table_header($th);
	$k = 0; //row colour counter
	while ($myrow = db_fetch($result))
	{
		alt_table_row_color($k);
		label_cell(get_trans_view_str(ST_SUPPINVOICE, $myrow["trans_no"]));
		label_cell($myrow['reference']);
		label_cell(sql2date($myrow['tran_date']));
		label_cell(sql2date($myrow['due_date']));
		$name = $myrow["supplier_id"]." ".$myrow["supp_name"];
		label_cell($name);
		label_cell($myrow['curr_code']);
		amount_cell($myrow['total']);
		amount_cell($myrow['remainder']);
		label_cell($myrow['days'], "align='right'");
		end_row();
	}
	end_table(2);
}

function bank_balance($today, $width)
{
	$today = date2sql($today);
	$sql = "SELECT bank_act, bank_account_name, SUM(amount) balance FROM ".TB_PREF."bank_trans bt 
	            INNER JOIN ".TB_PREF."bank_accounts ba ON bt.bank_act = ba.id
	            WHERE trans_date <= '$today'
	            AND inactive <> 1
	            GROUP BY bank_act, bank_account_name
				ORDER BY bank_account_name";
	$result = db_query($sql);
	$title = _("Bank Account Balances");
	br(1);
	display_heading($title);
	br();
	$th = array(_("Account"), _("Balance"));
	start_table(TABLESTYLE, "width='$width%'");
	table_header($th);
	$k = 0; //row colour counter
	while ($myrow = db_fetch($result))
	{
		alt_table_row_color($k);
		label_cell(viewer_link($myrow["bank_account_name"], 'gl/inquiry/bank_inquiry.php?bank_account='.$myrow["bank_act"]));
		amount_cell($myrow['balance']);
		end_row();
	}
	end_table(1);
}