<?php
if (isset($header2type))
{
	$doc_Cust_no = "Cust no";
	$doc_Date = "Date";
	$doc_Charge_To = "Charge To";
	$doc_Delivered_To = "Delivered To";
	$doc_Shipping_Company = "Shipping Company";
	if ($doctype == 9)
		$doc_Due_Date = "Delivery Date";
	else
		$doc_Due_Date = "Due Date";
	$doc_Your_Ref = "Your Ref";
	$doc_Our_Ref = "Our Ref.";
	$doc_Your_VAT_no = "Your VAT No.";
	$doc_Our_VAT_no = "Our VAT No.";
	$doc_Payment_Terms = "Payment Terms";
	$doc_Customers_Ref = "Customers Reference";
	$doc_Our_Order_No = "Our Order No";
	$doc_Domicile = "Domicile";
	if($doctype == 13 || $doctype == 8 || $doctype == 9) {
	 $doc_Bank_Account = '';
	 $doc_Please_Quote = "All amounts stated in";
  } else {
	 $doc_Please_Quote = "Please quote Invoice no. when paying. All amounts stated in";
	 $doc_Bank_Account = "Bank Account";
	}
	$doc_Address = "Address";
	$doc_Phone_Fax_Email = "Phone/Fax/Email";
	$doc_Bank = "Bank";
	$doc_Bank_Account = "Bank Account";
	$doc_Payment_Link = "You can pay through";
	if ($doctype == 8 || $doctype == 9 || $doctype == 10 || $doctype == 11 || $doctype == 13)
	{
		if ($doctype == 8)
			$this->title = "PURCHASE ORDER";
		else if ($doctype == 9)
			$this->title = ($print_as_quote==1 ? "QUOTE" : "SALES ORDER");
		else if ($doctype == 10)
			$this->title = "INVOICE";
		else if ($doctype == 13)
			$this->title = "DELIVERY NOTE";
		else
			$this->title = "CREDIT NOTE";
		if ($doctype == 8)
			$this->headers = array('Item Code', 'Item Description',	'Delivery Date',
				'Quantity', 'Unit',	'Price', 'Total');
		else
			$this->headers = array('Item Code', 'Item Description',	'Quantity', 'Unit',
				'Price', 'Discount %', 'Total');
	}
	else if ($doctype == 12)
	{
		$this->title = "STATEMENT";
		$this->headers = array('Trans Type', '#', 'Date', 'Due Date', 'Charges',
			'Credits', 'Allocated', 'Outstanding');
	}
}
if (isset($emailtype))
{
	$doc_Dear_Sirs = "Dear Sirs";
	$doc_AttachedFile = "Attached you will find ";
	$doc_Kindest_regards = "Kindest regards";
	$doc_Payment_Link = "You can pay through";
}
if (isset($header2type) || isset($linetype))
{
	$doc_Invoice_no = ($doctype==13 ? "Delivery Note No." : (($doctype==8 || $doctype==9) ? "Order no." : "Invoice No."));
	$doc_Order_no = "Order no.";
}
if (isset($linetype))
{
	if ($doctype == 8 || $doctype == 9 || $doctype == 10 || $doctype == 11 || $doctype == 13)
	{
		$doc_Sub_total = "Sub-total";
		$doc_Shipping = "Shipping";
		$doc_Included = "Included";
		$doc_Amount = "amount";
		$doc_TOTAL_INVOICE = "TOTAL INVOICE";
		$doc_TOTAL_ORDER = "TOTAL ORDER EX VAT";
		$doc_TOTAL_PO = "TOTAL PO EX VAT";
		$doc_TOTAL_DELIVERY = "TOTAL DELIVERY INCL. VAT";
	}
	else if ($doctype == 12)
	{
		$doc_Outstanding = "Outstanding Transactions";
		$doc_Current = "Current";
		$doc_Total_Balance = "Total Balance";
		$doc_Statement = "Statement";
		$doc_as_of = "as of";
		$doc_Days = "Days";
		$doc_Over = "Over";
	}
}
?>