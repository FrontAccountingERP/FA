<?php

// Put this on the page only once, e.g. print_hidden_script(systypes::sales_order());

function print_hidden_script($type_no)
{
	global $path_to_root;
	include_once($path_to_root . "/includes/types.inc");
	$action = "";

	switch ($type_no)
	{
		case systypes::sales_order() :
			$action = "$path_to_root/reporting/rep109.php";
			$ar = array(
				'PARAM_0' => 0,		// from - these values are updated in print_document_link()
				'PARAM_1' => 0, 	// to
				'PARAM_2' => "", 	// currency
				'PARAM_3' => "", 	// bank account
				'PARAM_4' => 0, 	// email
				'PARAM_5' => 0, 	// quote
				'PARAM_6' => ""); 	// comments
			break;
		case systypes::cust_dispatch() :
			$action = "$path_to_root/reporting/rep110.php";
			$ar = array(
				'PARAM_0' => 0,		// from - these values are updated in print_document_link()
				'PARAM_1' => 0, 	// to
				'PARAM_2' => 0, 	// email
				'PARAM_3' => ""); 	// comments
			break;
		case 10 : // Sales Invoice
		case 11 : // Customer Credit Note
			$action = "$path_to_root/reporting/rep107.php";
			$ar = array(
				'PARAM_0' => 0,		// from - these values are updated in print_document_link()
				'PARAM_1' => 0, 	// to
				'PARAM_2' => "", 	// currency
				'PARAM_3' => "", 	// bank account
				'PARAM_4' => 0, 	// email
				'PARAM_5' => "", 	// paylink
				'PARAM_6' => ""); 	// comments
			break;
		case systypes::po() :
			$action = "$path_to_root/reporting/rep209.php";
			$ar = array(
				'PARAM_0' => 0,		// from - these values are updated in print_document_link()
				'PARAM_1' => 0, 	// to
				'PARAM_2' => "", 	// currency
				'PARAM_3' => "", 	// bank account
				'PARAM_4' => 0, 	// email
				'PARAM_5' => ""); 	// comments
			break;
	}
	$st = "<form method='post' name='pdf_form' action='$action'>";
	foreach ($ar as $key => $value)
	{
		$st .= "<input type='hidden' name='$key' value='$value' />";
	}
	$st .= "</form>\n";
	$st .= "<script type='text/javascript'>
<!--
function printDocument(docno) {
  document.pdf_form.PARAM_0.value = document.pdf_form.PARAM_1.value = docno;
  window.open('','REP_WINDOW','toolbar=no,scrollbar=no,resizable=yes,menubar=no');
  document.pdf_form.target='REP_WINDOW';
  document.pdf_form.submit();
}
-->
</script>\n";

	echo $st;
}

// Ex. label_cell(print_document_link($myrow['order_no'], _("Print")));
// or display_note(print_document_link($order_no, _("Print this order")));
// or if a button
// echo "<input type='button' name='button' value='"._("Print this order")."' onclick=\"".
// 		print_document_link($order_no, "", false)."\" class='inputsubmit' />\n";

function print_document_link($doc_no, $link_text, $link=true)
{
	if ($link)
		return "<a href=\"javascript:printDocument('$doc_no');\">$link_text</a>";
	else
		return "javascript:printDocument('$doc_no');";
}
?>