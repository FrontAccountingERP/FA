<?php

include_once($path_to_root . "/admin/db/voiding_db.inc");

//--------------------------------------------------------------------------------------

function get_supplier_trans_view_str($type, $trans_no, $label="")
{
	global $path_to_root, $use_popup_windows;

	$viewer = "";
	if ($type == systypes::po())
		$viewer = "view_po.php";
	elseif ($type == 20)
		$viewer = "view_supp_invoice.php";
	elseif ($type == 21)
		$viewer = "view_supp_credit.php";
	elseif ($type == 22)
		$viewer = "view_supp_payment.php";
	elseif ($type == 25)
		$viewer = "view_grn.php";
	else
		return null;

	if ($label == "")
		$label = $trans_no;

	if ($viewer != "")
	{
		if ($use_popup_windows)
			$preview_str = "<a target='_blank' href='$path_to_root/purchasing/view/$viewer?trans_no=$trans_no' onclick=\"javascript:openWindow(this.href,this.target); return false;\" >$label</a>";
		else	
			$preview_str = "<a target='_blank' href='$path_to_root/purchasing/view/$viewer?trans_no=$trans_no'>$label</a>";
	}	
	else
		$preview_str = $label;

	return $preview_str;
}

//--------------------------------------------------------------------------------------

function get_gl_view_str($type, $trans_no, $label="", $force=false)
{
	global $path_to_root, $use_popup_windows;

	if (!$force && !user_show_gl_info())
		return "";

	if ($label == "")
		$label = _("GL");
	if ($use_popup_windows)
		$gl_view = "<a target='_blank' href='$path_to_root/gl/view/gl_trans_view.php?type_id=$type&trans_no=$trans_no' onclick=\"javascript:openWindow(this.href,this.target); return false;\" >$label</a>";
	else
		$gl_view = "<a target='_blank' href='$path_to_root/gl/view/gl_trans_view.php?type_id=$type&trans_no=$trans_no'>$label</a>";

	return $gl_view;
}

//--------------------------------------------------------------------------------------

function get_gl_view_str_cell($type, $trans_no, $label="")
{
	$str = get_gl_view_str($type, $trans_no, $label);
	if ($str != "")
		return "<td>$str</td>";
}

//--------------------------------------------------------------------------------------

function get_customer_trans_view_str($type, $trans_no, $label="")
{
	global $path_to_root, $use_popup_windows;

	$viewer = "";
	if ($type == 10)
		$viewer = "view_invoice.php";
	elseif ($type == 11)
		$viewer = "view_credit.php";
	elseif ($type == 12)
		$viewer = "view_receipt.php";
	elseif ($type == 30)
		$viewer = "view_sales_order.php";
	else
		return null;

	if ($label == "")
		$label = $trans_no;

	if ($viewer != "")
	{
		if ($use_popup_windows)
			$preview_str = "<a target='_blank' href='$path_to_root/sales/view/$viewer?trans_no=$trans_no' onclick=\"javascript:openWindow(this.href,this.target); return false;\" >$label</a>";
		else
			$preview_str = "<a target='_blank' href='$path_to_root/sales/view/$viewer?trans_no=$trans_no'>$label</a>";
	}		
	else
		$preview_str = $label;

	return $preview_str;
}

//--------------------------------------------------------------------------------------

function get_banking_trans_view_str($type, $trans_no, $label="")
{
	global $path_to_root, $use_popup_windows;

	$viewer = "";

	if ($type == 4)
		$viewer = "bank_transfer_view.php";
	elseif ($type == 1)
		$viewer = "gl_payment_view.php";
	elseif ($type == 2)
		$viewer = "gl_deposit_view.php";
	elseif ($type == 0)
		$viewer = "";
	else
		return null;

	if ($label == "")
		$label = $trans_no;

	if ($viewer != "")
	{
		if ($use_popup_windows)
			$preview_str = "<a target='_blank' href='$path_to_root/gl/view/$viewer?trans_no=$trans_no' onclick=\"javascript:openWindow(this.href,this.target); return false;\" >$label</a>";
		else	
			$preview_str = "<a target='_blank' href='$path_to_root/gl/view/$viewer?trans_no=$trans_no'>$label</a>";
	}		
	else
		$preview_str = $label;

	return $preview_str;
}

//--------------------------------------------------------------------------------------

function get_inventory_trans_view_str($type, $trans_no, $label="")
{
	global $path_to_root, $use_popup_windows;

	$viewer = "";

	if ($type == systypes::inventory_adjustment())
		$viewer = "view_adjustment.php";
	elseif ($type == systypes::location_transfer())
		$viewer = "view_transfer.php";
	else
		return null;

	if ($label == "")
		$label = $trans_no;

	if ($viewer != "")
	{
		if ($use_popup_windows)
			$preview_str = "<a target='_blank' href='$path_to_root/inventory/view/$viewer?trans_no=$trans_no' onclick=\"javascript:openWindow(this.href,this.target); return false;\" >$label</a>";
		else	
			$preview_str = "<a target='_blank' href='$path_to_root/inventory/view/$viewer?trans_no=$trans_no'>$label</a>";
	}		
	else
		$preview_str = $label;

	return $preview_str;
}

//--------------------------------------------------------------------------------------

function get_manufacturing_trans_view_str($type, $trans_no, $label="")
{
	global $path_to_root, $use_popup_windows;

	$viewer = "";

	if ($type == 28)
		$viewer = "wo_issue_view.php";
	elseif ($type == 29)
		$viewer = "wo_production_view.php";
	elseif ($type == systypes::work_order())
		$viewer = "work_order_view.php";
	else
		return null;

	if ($label == "")
		$label = $trans_no;

	if ($viewer != "")
	{
		if ($use_popup_windows)
			$preview_str = "<a target='_blank' href='$path_to_root/manufacturing/view/$viewer?trans_no=$trans_no' onclick=\"javascript:openWindow(this.href,this.target); return false;\" >$label</a>";
		else	
			$preview_str = "<a target='_blank' href='$path_to_root/manufacturing/view/$viewer?trans_no=$trans_no'>$label</a>";
	}
	else
		$preview_str = $label;

	return $preview_str;
}

//--------------------------------------------------------------------------------------

function get_dimensions_trans_view_str($type, $trans_no, $label="")
{
	global $path_to_root, $use_popup_windows;

	$viewer = "";

	if ($type == 40)
		$viewer = "view_dimension.php";
	else
		return null;

	if ($label == "")
		$label = $trans_no;

	if ($viewer != "")
	{
		if ($use_popup_windows)
			$preview_str = "<a target='_blank' href='$path_to_root/dimensions/view/$viewer?trans_no=$trans_no' onclick=\"javascript:openWindow(this.href,this.target); return false;\" >$label</a>";
		else	
			$preview_str = "<a target='_blank' href='$path_to_root/dimensions/view/$viewer?trans_no=$trans_no'>$label</a>";
	}		
	else
		$preview_str = $label;

	return $preview_str;
}

//--------------------------------------------------------------------------------------

function get_trans_view_str($type, $trans_no, $label="")
{
	$view_str = get_customer_trans_view_str($type, $trans_no, $label);
	if ($view_str != null)
		return $view_str;

	$view_str = get_supplier_trans_view_str($type, $trans_no, $label);
	if ($view_str != null)
		return $view_str;

	$view_str = get_banking_trans_view_str($type, $trans_no, $label);
	if ($view_str != null)
		return $view_str;

	$view_str = get_inventory_trans_view_str($type, $trans_no, $label);
	if ($view_str != null)
		return $view_str;

	$view_str = get_manufacturing_trans_view_str($type, $trans_no, $label);
	if ($view_str != null)
		return $view_str;

	$view_str = get_dimensions_trans_view_str($type, $trans_no, $label);
	if ($view_str != null)
		return $view_str;

	return null;
}

//--------------------------------------------------------------------------------------

function exchange_rate_display($from_currency, $to_currency, $date_, $buttons=true)
{
	if ($from_currency != $to_currency)
	{
		if ($buttons && isset($_POST['get_rate']))	
		{
			$comp_currency = get_company_currency();
			if ($from_currency == $comp_currency)
				$currency = $to_currency;
			else
				$currency = $from_currency;
			$rate = get_ecb_rate($currency);
			if (get_date_exchange_rate($currency, $date_))
				update_exchange_rate($currency, $date_, $rate, $rate);
			else	
				add_exchange_rate($currency, $date_, $rate, $rate);
			if ($from_currency == $comp_currency)
				$rate = 1 / $rate;
		}	
		else
			$rate = get_exchange_rate_from_to($to_currency, $from_currency, $date_);
		$rate = number_format2($rate, user_exrate_dec());
    	label_row(_("Exchange Rate:"),"1 " . $from_currency . " = " .  $rate . " " . $to_currency .
			($buttons?"  " . submit('get_rate',_("Get"), false):""));
	}
}

//--------------------------------------------------------------------------------------

function is_voided_display($type, $id, $label)
{
	global $table_style;
	$void_entry = get_voided_entry($type, $id);

	if ($void_entry == null)
		return false;

	start_table("width=50% $table_style");
	echo "<tr><td align=center><font color=red>$label</font><br>";
	echo "<font color=red>" . _("Date Voided:") . " " . sql2date($void_entry["date_"]) . "</font><br>";
	if (strlen($void_entry["memo_"]) > 0)
		echo "<center><font color=red>" . _("Memo:") . " " . $void_entry["memo_"] . "</font><br>";
	echo "</td></tr>";
	end_table(1);

	return true;
}

//--------------------------------------------------------------------------------------

function comments_display_row($type, $id)
{
	$comments = get_comments($type, $id);
	if ($comments and db_num_rows($comments)) 
	{
		echo "<tr><td colspan=15>";
    	while ($comment = db_fetch($comments)) 
    	{
    		echo $comment["memo_"] . "<br>";
    	}
		echo "</td></tr>";
	}
}

//--------------------------------------------------------------------------------------

function get_comments_string($type, $type_no)
{
	$str_return = "";
	$result = get_comments($type, $type_no);
	while ($comment = db_fetch($result)) 
	{
		if (strlen($str_return))
			$str_return = $str_return . " \n";
		$str_return = $str_return . $comment["memo_"];
	}
	return $str_return;
}

//--------------------------------------------------------------------------------------

function view_stock_status($stock_id, $description=null)
{
	global $path_to_root;
	if ($description)
		//hyperlink_params_separate($path_to_root . "/inventory/inquiry/stock_status.php", (user_show_codes()?$stock_id . " - ":"") . $description, "stock_id=$stock_id");
		$preview_str = "<a target='_blank' href='$path_to_root/inventory/inquiry/stock_status.php?stock_id=$stock_id' onclick=\"javascript:openWindow(this.href,this.target); return false;\" >". (user_show_codes()?$stock_id . " - ":"") . $description."</a>";
	else
		//hyperlink_params_separate($path_to_root . "/inventory/inquiry/stock_status.php", $stock_id, "stock_id=$stock_id");
		$preview_str = "<a target='_blank' href='$path_to_root/inventory/inquiry/stock_status.php?stock_id=$stock_id' onclick=\"javascript:openWindow(this.href,this.target); return false;\" >$stock_id</a>";
	echo $preview_str;	
}

function view_stock_status_cell($stock_id, $description=null)
{
	echo "<td>";
	view_stock_status($stock_id, $description);
	echo "</td>";
}

//--------------------------------------------------------------------------------------

function display_debit_or_credit_cells($value)
{
	if ($value > 0)
	{
		amount_cell($value);
		label_cell("");
	}	
	elseif ($value < 0)
	{
		label_cell("");
		amount_cell(abs($value));
	}	
	else
	{
		//label_cell("");
		amount_cell(0);
		label_cell("");
	}
}

//--------------------------------------------------------------------------------------

function display_customer_trans_tax_details($tax_items, $columns)
{
    while ($tax_item = db_fetch($tax_items)) 
    {
    	$tax = number_format2($tax_item['amount'],user_price_dec());
    	if ($tax_item['included_in_price'])
        	label_row(_("Included") . " " . $tax_item['tax_type_name'] . " (" . $tax_item['rate'] . "%) " .
        		_("Amount") . ": $tax", "", "colspan=$columns align=right", "align=right");
    	else
        	label_row($tax_item['tax_type_name'] . " (" . $tax_item['rate'] . "%)",
        		$tax, "colspan=$columns align=right", "align=right");
    }
}

//--------------------------------------------------------------------------------------

function display_supp_trans_tax_details($tax_items, $columns)
{
    while ($tax_item = db_fetch($tax_items)) 
    {
    	$tax = number_format2(abs($tax_item['amount']),user_price_dec());
    	if ($tax_item['included_in_price'])
        	label_row(_("Included") . " " . $tax_item['tax_type_name'] . " (" . $tax_item['rate'] . "%) " .
        		_("Amount:") . ": $tax", "colspan=$columns align=right", "align=right");
    	else
        	label_row($tax_item['tax_type_name'] . " (" . $tax_item['rate'] . "%)",
        		$tax, "colspan=$columns align=right", "align=right");
    }
}

//--------------------------------------------------------------------------------------

function display_edit_tax_items($taxes, $columns)
{
	$total = 0;

    foreach ($taxes as $taxitem) 
    {
    	if ($taxitem['included_in_price']) 
    	{
        	label_row(_("Included") . " " . $taxitem['tax_type_name'] . " (" . $taxitem['rate'] . "%) " .
        		_("Amount:") . " " . number_format2($taxitem['Value'],user_price_dec()), "", "colspan=$columns align=right", "align=right");
    	} 
    	else 
    	{
        	label_row($taxitem['tax_type_name'] . " (" . $taxitem['rate'] . "%)",
        		number_format2($taxitem['Value'],user_price_dec()), "colspan=$columns align=right", "align=right");
        	$total +=  $taxitem['Value'];
    	}
    }

    return $total;
}

//--------------------------------------------------------------------------------------

function display_footer_exit()
{
	global $path_to_root;
	br(2);
	end_page();
	exit;
}

//--------------------------------------------------------------------------------------

function display_allocations($alloc_result, $total)
{
	global $table_style;

	if (!$alloc_result || db_num_rows($alloc_result) == 0)
		return;

    display_heading2(_("Allocations"));

    start_table("$table_style width=80%");
    
    $th = array( _("Type"), _("Number"), _("Date"), _("Total Amount"),
    	_("Left to Allocate"), _("This Allocation"));
	table_header($th);
    $k = $total_allocated = 0;

    while ($alloc_row = db_fetch($alloc_result))
    {

    	alt_table_row_color($k);

    	label_cell(systypes::name($alloc_row['type']));
    	label_cell(get_trans_view_str($alloc_row['type'],$alloc_row['trans_no']));
    	label_cell(sql2date($alloc_row['tran_date']));
    	amount_cell($alloc_row['Total']);
    	//amount_cell($alloc_row['Total'] - $alloc_row['PrevAllocs'] - $alloc_row['amt']);
    	amount_cell($alloc_row['Total'] - $alloc_row['amt']);
    	amount_cell($alloc_row['amt']);
    	end_row();

    	$total_allocated += $alloc_row['amt'];
    }
    start_row();
   	label_cell(_("Total Allocated:"), "align=right colspan=5");
	amount_cell($total_allocated);
	end_row();
	start_row();
    label_cell(_("Left to Allocate:"), "align=right colspan=5");
    amount_cell($total - $total_allocated);
    end_row();

    end_table(1);
}

//--------------------------------------------------------------------------------------

function display_allocations_from($person_type, $person_id, $type, $type_no, $total)
{
	switch ($person_type) 
	{
		case payment_person_types::customer() :
			$alloc_result = get_allocatable_to_cust_transactions($person_id, $type_no, $type);
			display_allocations($alloc_result, $total);
			return;
		case payment_person_types::supplier() :
			$alloc_result = get_allocatable_to_supp_transactions($person_id, $type_no, $type);
			display_allocations($alloc_result, $total);
			return;
	}
}

function get_js_go_back()
{
	$js = "\n<script type=\"text/javascript\">\n"
	 	. "<!--\n"
		. "function goBack()\n"
		. "{\n"
		. "	if (window.history.length <= 1)\n"
		. "	 window.close();\n"
		. "	else\n"	
		. "	 window.history.go(-1);\n"			
		. "}\n"
		. "-->\n"
		. "</script>\n";
	return $js;
}	

function get_js_open_window($width, $height)
{
	$js = "\n<script type=\"text/javascript\">\n"
	 	. "<!--\n"
		. "function openWindow(url, title)\n"
		. "{\n"
		. " var left = (screen.width - $width) / 2;\n"
		. " var top = (screen.height - $height) / 2;\n"
		. " window.open(url, title, 'width=$width,height=$height,left='+left+',top='+top+',screenX='+left+',screenY='+top+',status=no,scrollbars=yes');\n"
		. "}\n"
		. "-->\n"
		. "</script>\n";
	return $js;
}

function get_js_form_entry($edit_name, $sel_name, $next_name)
{
	$js = "\n<script type=\"text/javascript\">\n"
		. "<!--\n"
		. "function recalcAccounts()\n"
		. "{\n"
		. "	var myForm = document.forms[0];\n"
		. "	var len = myForm.$sel_name.length;\n"
		. "	var txt;\n"
		. "	var ac = myForm.$edit_name.value;\n"
		. "	var i;\n"
		. "	myForm.$sel_name.options[myForm.$sel_name.selectedIndex].selected = false;\n"
		. "	for (i = 0; i < len; i++)\n"
		. "	{\n"
		. "		txt = myForm.$sel_name.options[i].text;\n"
		. "		if (txt.indexOf(ac) == 0)\n"
		. "		{\n"
		. "			myForm.$sel_name.options[i].selected = true;\n"
		. "			break;\n"
		. "		}\n"
		. "	}\n"
		. "}\n"
		. "function setAccount(p, s)\n"
		. "{\n"
		. "	var myForm = document.forms[0];\n"
		. " if (p == 0 && myForm.$edit_name.value == \"\")\n"
		. "		return true;\n"
		. "	myForm.$edit_name.value = myForm.$sel_name.options[myForm.$sel_name.selectedIndex].value;\n"
		. " if (s)\n"
		. "  myForm.submit();\n"
		. "	myForm.$next_name.focus();\n"
		. "	return true;\n"
		. "}\n"
		. "-->\n"
		. "</script>\n";
	return $js;	
}

function get_js_set_focus($name)
{
	$js = "\n<script type=\"text/javascript\">\n"
		. "<!--\n"
		. "function setFocus()\n"
		. "{\n"
		. "	document.forms[0].$name.focus();\n"
		. "}\n"
		. "-->\n"
		. "</script>\n";
	return $js;	
}

function get_js_png_fix()
{
	$js = "<script type=\"text/javascript\">\n"
		. "function fixPNG(myImage)\n" 
		. "{\n"
		. " var arVersion = navigator.appVersion.split(\"MSIE\")\n"
		. " var version = parseFloat(arVersion[1])\n"
    	. " if ((version >= 5.5) && (version < 7) && (document.body.filters))\n" 
    	. " {\n"
       	. "  var imgID = (myImage.id) ? \"id='\" + myImage.id + \"' \" : \"\"\n"
	   	. "  var imgClass = (myImage.className) ? \"class='\" + myImage.className + \"' \" : \"\"\n"
	   	. "  var imgTitle = (myImage.title) ?\n" 
		. "    \"title='\" + myImage.title  + \"' \" : \"title='\" + myImage.alt + \"' \"\n"
	   	. "  var imgStyle = \"display:inline-block;\" + myImage.style.cssText\n"
	   	. "  var strNewHTML = \"<span \" + imgID + imgClass + imgTitle\n"
        . "    + \" style=\\\"\" + \"width:\" + myImage.width\n" 
        . "    + \"px; height:\" + myImage.height\n" 
        . "    + \"px;\" + imgStyle + \";\"\n"
        . "    + \"filter:progid:DXImageTransform.Microsoft.AlphaImageLoader\"\n"
        . "    + \"(src=\'\" + myImage.src + \"\', sizingMethod='scale');\\\"></span>\"\n"
	   	. "  myImage.outerHTML = strNewHTML\n"	  
    	. " }\n"
		. "}\n"
		. "</script>\n";
	return $js;
}	

function get_js_date_picker()
{
	global $dateseps, $path_to_root;
	
	$encoding = $_SESSION['language']->encoding;	// character encoding
	$dir = $_SESSION['language']->dir;				// left to right, right to left
	$how = user_date_format();						// 0 = us/ca, 1 = eu, au, nz, 2 = jp, sw
	$sep = $dateseps[user_date_sep()];				// date separator
	$wstart = ($how != 0);							// weekstart (sun = 0, mon = 1)
	$width = 250;									// datepicker width
	$height = 180;									// datepicker height
	$path = "$path_to_root/themes/default/images/";	// path to images
	$tbgcolor = "#4682b4"; 							// title backgrond
	$tcolor = "white";								// title color
	$wbgcolor = "#87cefa";							// weekdays background
	$wcolor = "white";								// weekdays color
	$cbgcolor = "#ffb6c1";							// current day background
	$ebgcolor = "#dbeaf5";							// week-end background
	$bgcolor = "white";								// normal background
	$color = "darkblue";							// month day color
	$gcolor = "#aaaaaa";							// prev/next month day color
	$family = "tahoma,verdana";						// font-family
	
	$js = "\n<script type=\"text/javascript\">\n"
		. "<!--\n"
		. "function date_picker(nm, val) {\n"
		. " var mons = ['"._('January')."', '"._('February')."', '"._('March')."', '"._('April')."', '"._('May')."', '"._('June')."',\n"
		. "  '"._('July')."', '"._('August')."', '"._('Sebtember')."', '"._('October')."', '"._('November')."', '"._('December')."'];\n"
		. " var wdays = ['"._('Su')."', '"._('Mo')."', '"._('Tu')."', '"._('We')."', '"._('Th')."', '"._('Fr')."', '"._('Sa')."'];\n"
		. " var wstart = $wstart; // day of week starts from (normally 0 or 1)\n"
		. " var _date = (val == null || val =='' ?  new Date() : str2dt(val));\n"
		. " var _prev = new Date(_date);\n"
		. " _prev.setMonth(_date.getMonth()-1);\n"
		. " var _next = new Date(_date);\n"
		. " _next.setMonth(_date.getMonth()+1);\n"
		. " var _prevy = new Date(_date);\n"
		. " _prevy.setFullYear(_date.getFullYear()-1);\n"
		. " var _nexty = new Date(_date);\n"
		. " _nexty.setFullYear(_date.getFullYear()+1);\n"
		. " var _first = new Date(_date);\n"
		. " _first.setDate(1);\n"
		. " _first.setDate(1-(7+_first.getDay()-wstart)%7);\n"
		. " var _last = new Date(_next);\n"
		. " _last.setDate(0);\n"
		. " var left = (screen.width - $width) / 2;\n"
		. " var top = (screen.height - $height) / 2;\n"
		. "	var buff = new String (\n"
		. "\"<html dir='$dir'>\\n\"+\n"
		. "\"<head>\\n\"+\n"
		. "\"	<title>DatePicker</title>\\n\"+\n"
		. "\"   <meta http-equiv='Content-type' content='text/html'; charset='$encoding'>\\n\"+\n"
		. "\"	<style>\\n\"+\n"
		. "\"	    body { background-color:$tbgcolor; margin:0; padding:0; font-family: $family; font-size: 13px; }\\n\"+\n"
		. "\"	    select { font-size: 12px; }\\n\"+\n"
		. "\"	    a { text-decoration: none; }\\n\"+\n"
		. "\"	    a:hover { text-decoration: underline; }\\n\"+\n"
		. "\"	    .tbg { background-color: $tbgcolor; color: $tcolor; font-size: 12px; font-weight: bold; text-align: center;}\\n\"+\n"
		. "\"	    .wbg { background-color: $wbgcolor; color: $wcolor; font-size: 12px; text-align: right; }\\n\"+\n"
		. "\"	    .cbg { background-color: $cbgcolor; font-size: 12px; text-align: right; }\\n\"+\n"
		. "\"	    .ebg { background-color: $ebgcolor; font-size: 12px; text-align: right; }\\n\"+\n"
		. "\"	    .bg { background-color: $bgcolor; font-size: 12px; text-align: right; }\\n\"+\n"
		. "\"	    .c { color: $color; }\\n\"+\n"
		. "\"	    .gc { color: $gcolor; }\\n\"+\n"
		. "\"	</style>\\n\"+\n"
		. "\"</head>\\n\"+\n"
		. "\"<body>\\n\"+\n"
		. "\"<table cellspacing=\\\"0\\\" border=\\\"0\\\" style=\\\"width:100%;\\\">\\n\"+\n"
		. "\"<tr><td class=\\\"tbg\\\">\\n\"+\n"
		. "\"<table cellspacing=\\\"1\\\" cellpadding=\\\"2\\\" border=\\\"0\\\" style=\\\"width:100%;\\\">\\n\"+\n"
		. "\"<tr>\\n	<td class=\\\"tbg\\\"><a href=\\\"javascript:window.opener.date_picker('\"+\n"
		. " nm+\"', '\"+ dt2dtstr(_prev)+\"');\\\">\"+\n"
		. "\"<img src=\\\"".$path."prev.gif\\\" width=\\\"16\\\" height=\\\"16\\\" border=\\\"0\\\"\"+\n"
		. "\" alt=\\\""._('previous month')."\\\"></a></td>\\n\"+\n"
		. "\"	<td class=\\\"tbg\\\" colspan=\\\"5\\\">\"+\n"
		. " mons[_date.getMonth()]+\"</td>\\n\"+\n"
		. "\"	<td class=\\\"tbg\\\"><a href=\\\"javascript:window.opener.date_picker('\"\n"
		. " +nm+\"', '\"+dt2dtstr(_next)+\"');\\\">\"+\n"
		. "\"<img src=\\\"".$path."next.gif\\\" width=\\\"16\\\" height=\\\"16\\\" border=\\\"0\\\"\"+\n"
		. "\" alt=\\\""._('next month')."\\\"></a></td>\\n</tr>\\n\"+\n"
		. "\"<tr>\\n	<td class=\\\"tbg\\\"><a href=\\\"javascript:window.opener.date_picker('\"+\n"
		. " nm+\"', '\"+ dt2dtstr(_prevy)+\"');\\\">\"+\n"
		. "\"<img src=\\\"".$path."prev.gif\\\" width=\\\"16\\\" height=\\\"16\\\" border=\\\"0\\\"\"+\n"
		. "\" alt=\\\""._('previous year')."\\\"></a></td>\\n\"+\n"
		. "\"	<td class=\\\"tbg\\\" colspan=\\\"5\\\">\"+\n"
		. " _date.getFullYear()+\"</td>\\n\"+\n"
		. "\"	<td class=\\\"tbg\\\"><a href=\\\"javascript:window.opener.date_picker('\"\n"
		. " +nm+\"', '\"+dt2dtstr(_nexty)+\"');\\\">\"+\n"
		. "\"<img src=\\\"".$path."next.gif\\\" width=\\\"16\\\" height=\\\"16\\\" border=\\\"0\\\"\"+\n"
		. "\" alt=\\\""._('next year')."\\\"></a></td>\\n</tr>\\n\"\n"
		. ");\n"
		. " var _current = new Date(_first);\n"
		. " // weekdays titles\n"
		. " buff += \"<tr>\\n\";\n"
		. " for (var n=0; n<7; n++)\n"
		. "  buff += \"	<td class=\\\"wbg\\\">\"+\n"
		. " wdays[(wstart+n)%7]+\"</td>\\n\";\n"
		. " // calendar table\n"
		. " buff += \"</tr>\\n\";\n"
		. " for (var i=0; i<6; i++) {\n"
		. "  // row heder\n"
		. "  buff += \"<tr>\\n\";\n"
		. "  for (var n=0; n<7; n++) {\n"
		. "   if (_current.getDate() == _date.getDate() &&\n"
		. "    _current.getMonth() == _date.getMonth())\n"
		. "    // current date\n"
		. "	   buff += \"	<td class=\\\"cbg\\\">\";\n"
		. "	  else if (_current.getDay() == 0 || _current.getDay() == 6)\n"
		. "	   // weekend days\n"
		. "    buff += \"	<td class=\\\"ebg\\\">\";\n"
		. "   else\n"
		. "    // working days of current month\n"
		. "	   buff += \"	<td class=\\\"bg\\\">\";\n"
		. "	  if (_current.getMonth() == _date.getMonth())\n"
		. "    // days of current month\n"
		. "    buff += \"<a href=\\\"javascript:window.opener.\"+nm+\n"
		. "		\".value='\"+dt2dtstr(_current)+\"'; window.close();\\\">\"+\n"
		. "		\"<span class=\\\"c\\\">\";\n"
		. "   else\n" 
		. "    // days of other months\n"
		. "    buff += \"<a href=\\\"javascript:window.opener.\"+nm+\n"
		. "		\".value='\"+dt2dtstr(_current)+\"'; window.close();\\\">\"+\n"
		. "		\"<span class=\\\"gc\\\">\";\n"
		. "   buff += _current.getDate()+\"</span></a></td>\\n\";\n"
		. "   _current.setDate(_current.getDate()+1);\n"
		. "  }\n"
		. "  // row footer\n"
		. "  buff += \"</tr>\\n\";\n"
		. " }\n"
		. " // picker footer\n"
		. " buff +=\n"
		. " \"</table>\\n\" +\n"
		. " \"</tr>\\n</td>\\n</table>\\n\" +\n"
		. " \"</body>\\n\" +\n"
		. " \"</html>\\n\";\n"
		. "	var picker = window.open('', 'DatePicker',\n" 
		. "  'width=$width,height=$height,status=no,resizable=yes,top='+top+',left='+left+'');\n"
		. " picker.opener = self;\n"
		. " var _doc = picker.document;\n"
		. " _doc.write (buff);\n"
		. " _doc.close();\n"
		. "}\n"
		. "// datetime parsing and formatting routimes. modify them if you wish other datetime format\n"
		. "function str2dt (val) {\n"
		. " var re_date = /^(\\d+)\\".$sep."(\\d+)\\".$sep."(\\d+)+$/;\n"
		. " if (!re_date.exec(val))\n"
		. "  return alert('"._('Invalid Datetime format').": '+ val);\n";
	if ($how == 0)	
		$js .= " return (new Date (RegExp.$3, RegExp.$1-1, RegExp.$2));\n";
	elseif ($how == 1)		
		$js .= " return (new Date (RegExp.$3, RegExp.$2-1, RegExp.$1));\n";
	else	
		$js .= " return (new Date (RegExp.$1, RegExp.$2-1, RegExp.$3));\n";
	$js .= "}\n"
		. "function dt2dtstr (_date) {\n"
		. " var da = new String(_date.getDate());\n"
		. " var mo = new String(_date.getMonth()+1);\n"
		. " var yr = _date.getFullYear();\n"
		. " if (da.length < 2)\n"
		. "  da = '0'+da;\n"
		. " if (mo.length < 2)\n"
		. "  mo = '0'+mo;\n";
	if ($how == 0)	
		$js .= " return (new String (mo+'$sep'+da+'$sep'+yr));\n";
	elseif ($how == 1)	
		$js .= " return (new String (da+'$sep'+mo+'$sep'+yr));\n";
	else	
		$js .= " return (new String (yr+'$sep'+mo+'$sep'+da));\n";
	$js .= "}\n"
		. "-->\n"
		. "</script>\n";
	return $js;	
}
	
function alert($msg)
{
	echo "\n<script type=\"text/javascript\">\n"
		. "<!--\n"
		. "alert('$msg');\n"
		. "-->\n"
		. "</script>\n";
}		

if (!function_exists('_vd')) 
{
  	function _vd($mixed, $title = '', $exit = false) 
  	{
    	// Only the site admin is able to proceed here.
    	echo (!empty($title) ? ($title .':') : '') .'<pre>';
    	var_dump($mixed);
    	echo "</pre>\n";
    	if ($exit) 
    		exit;
  	}
}

?>