<?php
function printTransaction($link_text, $form_id, $form_type, $trans_type, $trans_no, $ref, $param1, $param2) {
	$ar = array(
				'COMPANY'	=> $_SESSION["wa_current_user"]->company,
				'FORMID'	=> $form_id,
				'FORMTYPE'	=> $form_type,
				'TRANSNO'	=> $trans_no,
				'TRANSTYPE'	=> $trans_type,
				'REF'		=> $ref,
				'PARAM1'	=> $param1,
				'PARAM2'	=> $param2
				);	
	$pdf_href =	getPDFOpenScript($link_text, $ar);
	return "<a href=\"javascript:$pdf_href\">" . _($link_text) . "</a>"; 	
}

function getHiddenFieldScript() {
	$ar = Array('COMPANY','LANG','FORMID','FORMTYPE','TRANSNO','TRANSTYPE','REF','PARAM1','PARAM2');
	foreach ($ar as $value)	$st.= "<input type='hidden' name='$value'>";
	
	$st .= "<input type='hidden' name='QTY_DEC' 	value='" . user_qty_dec() 	 . "'>
			<input type='hidden' name='PRICE_DEC' 	value='" . user_price_dec()	 . "'>
			<input type='hidden' name='PER_DEC' 	value='" . user_percent_dec(). "'>";	
	
	$st = "<form method='post' name='pdf_form'>" . $st . "</form>";	
	return $st;
}

function getPDFOpenScript($link_text, $ar_params) {
	$st = "document.pdf_form.LANG.value='" . $_SESSION['language']->code . "';";
	foreach ($ar_params as $key => $value) {
		$st .= "document.pdf_form.{$key}.value = '{$value}';";
	}
	$st.= "window.open('','REP_WINDOW','toolbar=no,scrollbar=no,resizable=yes,menubar=no');";	
	$st.= "document.pdf_form.target='REP_WINDOW';";
	$st.= "document.pdf_form.action= '" . C_JSP_ROOT . "/reporting/gen_pdf_form.jsp';";
	$st.= "document.pdf_form.submit();";
	
	return $st;
}

?>