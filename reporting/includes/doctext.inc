<?php
if (isset($header2type))
{
	$doc_Cust_no = _("Cust no");
	$doc_Date = _("Date");
	$doc_Charge_To = _("Charge To");
	$doc_Delivered_To = _("Delivered To");
	$doc_Shipping_Company = _("Shipping Company");
	if ($doctype == 9)
		$doc_Due_Date = _("Delivery Date");
  else
		$doc_Due_Date = _("Due Date");
	$doc_Your_Ref = _("Your Ref");
	$doc_Our_Ref = _("Our Ref.");
	$doc_Your_VAT_no = _("Your VAT no.");
	$doc_Our_VAT_no = _("Our VAT No.");
	$doc_Payment_Terms = _("Payment Terms");
	$doc_Our_Order_No = _("Our Order No");
	$doc_Domicile = _("Domicile");
	if($doctype == 13) {
	 $doc_Bank_Account = '';
	 $doc_Please_Quote = _("All amounts stated in");
  } else {
	 $doc_Bank_Account = _("Bank Account");
	 $doc_Please_Quote = _("Please quote Invoice no. when paying. All amounts stated in");
	}
	$doc_Address = _("Address");
	$doc_Phone_Fax_Email = _("Phone/Fax/Email");
	$doc_Bank = _("Bank");
	$doc_Bank_Account = _("Bank Account");
	$doc_Payment_Link = _("You can pay through");
	if ($doctype == 8 || $doctype == 9 || $doctype == 10 || $doctype == 11 || $doctype == 11)
	{
		if ($doctype == 8)
			$this->title = _("PURCHASE ORDER");
		else if ($doctype == 13)
			$this->title = _("DELIVERY NOTE");
		else if ($doctype == 9)
			$this->title = ($print_as_quote==1 ? _("QUOTE") : _("SALES ORDER"));
		else if ($doctype == 10)
			$this->title = _("INVOICE");
		else
			$this->title = _("CREDIT NOTE");
		if ($doctype == 8)	
			$this->headers = array(_("Item Code"), _("Item Description"),	
				_("Delivery Date"), _("Quantity"), 	_("Unit"), _("Price"), _("Total"));	
		else		
			$this->headers = array(_("Item Code"), _("Item Description"),	_("Quantity"), 
				_("Unit"), _("Price"), _("Discount %"), _("Total"));	
	}		
	else if ($doctype == 12)
	{
		$this->title = _("STATEMENT");
		$this->headers = array(_("Trans Type"), _("#"), _("Date"), _("DueDate"), _("Charges"),
			_("Credits"), _("Allocated"), _("Outstanding"));
	}		
}
if (isset($emailtype))
{
	$doc_Dear_Sirs = _("Dear Sirs");
	$doc_AttachedFile = _("Attached you will find ");
	$doc_Kindest_regards = _("Kindest regards");
	$doc_Payment_Link = _("You can pay through");
}
if (isset($header2type) || isset($linetype))
{
	$doc_Document_no = $doctype==13 ? _("Delivery Note No.") : _("Invoice No.");
	$doc_Delivery_no = "Delivery Note No.";
	$doc_Order_no = _("Order no.");
}	
if (isset($linetype))
{
	if ($doctype == 8 || $doctype == 9 || $doctype == 10 || $doctype == 11 || $doctype == 13)
	{
		$doc_Sub_total = _("Sub-total");
		$doc_Shipping = _("Shipping");
		$doc_Included = _("Included");
		$doc_Amount = _("Amount");
		$doc_TOTAL_INVOICE = _("TOTAL INVOICE");
		$doc_TOTAL_ORDER = _("TOTAL ORDER EX VAT");
		$doc_TOTAL_PO = _("TOTAL PO EX VAT");
		$doc_TOTAL_DELIVERY = _("TOTAL DELIVERY INCL. VAT");
	}
	else if ($doctype == 12)
	{
		$doc_Outstanding = _("Outstanding Transactions");
		$doc_Current = _("Current");
		$doc_Total_Balance = _("Total Balance");
		$doc_Statement = _("Statement");
		$doc_as_of = _("as of");
		$doc_Days = _("Days");
		$doc_Over = _("Over");
	}	
}
?>