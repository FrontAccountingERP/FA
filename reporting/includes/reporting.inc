<?php

// Put this on the page only once, e.g. print_hidden_script(systypes::sales_order());

function print_hidden_script($type_no)
{
	global $path_to_root;
	include_once($path_to_root . "/includes/types.inc");
	$action = "";
	$_POST['PARAM_0'] = $_POST['PARAM_1'] = 0; // the document no. is updated in print_document_link().

	switch ($type_no)
	{
		case systypes::sales_order() :
			$action = "$path_to_root/reporting/rep109.php";
			$_POST['PARAM_2'] = ""; // currency
			$_POST['PARAM_3'] = ""; // bank account
			$_POST['PARAM_4'] = 0; // email
			$_POST['PARAM_5'] = 0; // quote
			$_POST['PARAM_6'] = ""; // comments
			$params = 7;
			break;
		case systypes::cust_dispatch() :
			$action = "$path_to_root/reporting/rep110.php";
			$_POST['PARAM_2'] = 0; // email
			$_POST['PARAM_3'] = ""; // comments
			$params = 4;
			break;
		case 10 : // Sales Invoice
		case 11 : // Customer Credit Note
			$action = "$path_to_root/reporting/rep107.php";
			$_POST['PARAM_2'] = ""; // currency
			$_POST['PARAM_3'] = ""; // bank account
			$_POST['PARAM_4'] = 0; // email
			$_POST['PARAM_5'] = ""; // paylink
			$_POST['PARAM_6'] = ""; // comments
			$params = 7;
			break;
		case systypes::po() :
			$action = "$path_to_root/reporting/rep209.php";
			$_POST['PARAM_2'] = ""; // currency
			$_POST['PARAM_3'] = ""; // bank account
			$_POST['PARAM_4'] = 0; // email
			$_POST['PARAM_5'] = ""; // comments
			$params = 6;
			break;
	}
	$st = "";
	for ($i = 0; $i < $params; $i++)
	{
		$st .= "<input type='hidden' name='PARAM_".$i."' value='".$_POST['PARAM_'.$i]."' />";
	}
	$st = "<form method='post' name='pdf_form' action='$action'>" . $st . "</form>\n";
	echo $st;
}

// Ex. label_cell(print_document_link($myrow['order_no'], _("Print")));
// or display_note(print_document_link($order_no, _("Print this order")));
// or if a button
// echo "<input type='button' name='button' value='"._("Print this order")."' onclick=\"".
// 		print_document_link($order_no, "", false)."\" class='inputsubmit' />\n";

function print_document_link($doc_no, $link_text, $link=true)
{
	$st = "document.pdf_form.PARAM_0.value = '$doc_no';";
	$st .= "document.pdf_form.PARAM_1.value = '$doc_no';";
	$st .= "window.open('','REP_WINDOW','toolbar=no,scrollbar=no,resizable=yes,menubar=no');
	  	document.pdf_form.target='REP_WINDOW';
		document.pdf_form.submit();\n";
	if ($link)
		return "<a href=\"javascript:$st\">" . $link_text . "</a>";
	else
		return "javascript:$st";
}
?>