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
if (isset($header2type))
{
	$doc_Cust_no = "Cust no";
	$doc_Date = "Date";
	if ($doctype == 8) // Purchase Order
	{
		$doc_Charge_To = "Order To";
		$doc_Delivered_To = "Charge To";
	}
	else
	{
		$doc_Charge_To = "Charge To";
		$doc_Delivered_To = "Delivered To";
	}	
	$doc_Shipping_Company = "Shipping Company";
	if ($doctype == 7)
		$doc_Due_Date = "Valid until";
	if ($doctype == 9)
		$doc_Due_Date = "Delivery Date";
	else
		$doc_Due_Date = "Due Date";
	$doc_Your_Ref = "Your Ref";
	if ($doctype == 26)
	{
		$doc_Our_Ref = "Type";
		$doc_Your_VAT_no = "Manufactured Item";
		$doc_Payment_Terms = "Required By";
		$doc_Customers_Ref = "Reference";
		$doc_Our_Order_No = "Into Location";
		$doc_Due_Date = "Quantity";
	}	
	else
	{
		$doc_Our_Ref = "Our Ref.";
		$doc_Your_VAT_no = "Your VAT no.";
		$doc_Payment_Terms = "Payment Terms";
		$doc_Customers_Ref = "Customers Reference";
		$doc_Our_Order_No = "Our Order No";
	}
	$doc_Our_VAT_no = "Our VAT No.";
	$doc_Domicile = "Domicile";
	if($doctype == 13 || $doctype == 7 || $doctype == 8 || $doctype == 9) {
	 $doc_Bank_Account = '';
	 $doc_Please_Quote = "All amounts stated in";
  } else {
	 $doc_Please_Quote = "Please quote ".($doctype==10 ? "Invoice" : "Credit")
	 	." no. when paying. All amounts stated in";
	 $doc_Bank_Account = "Bank Account";
	}
	$doc_Address = "Address";
	$doc_Phone_Fax_Email = "Phone/Fax/Email";
	$doc_Bank = "Bank";
	$doc_Bank_Account = "Bank Account";
	$doc_Payment_Link = "You can pay through";
	if ($doctype == 7 || $doctype == 8 || $doctype == 9 || $doctype == 10 || $doctype == 11 || $doctype == 13 || $doctype == 26)
	{
		if ($doctype == 7)
			$this->title = "SALES QUOTATION";
		elseif ($doctype == 8)
			$this->title = "PURCHASE ORDER";
		elseif ($doctype == 9)
			$this->title = ($print_as_quote==1 ? "QUOTE" : "SALES ORDER");
		elseif ($doctype == 10)
			$this->title = "INVOICE";
		elseif ($doctype == 13)
			$this->title = ($packing_slip==1 ? "PACKING SLIP" : "DELIVERY NOTE");
		elseif ($doctype == 26)
			$this->title = "WORK ORDER";
		else
			$this->title = "CREDIT NOTE";
		if ($doctype == 8)
			$this->headers = array('Item Code', 'Item Description',	'Delivery Date',
				'Quantity', 'Unit',	'Price', 'Total');
		elseif ($doctype == 26)
			$this->headers = array("Item Code", "Item Description",
				"From Location", "Work Centre", 	"Unit Quantity", "Total Quantity", "Units Issued");
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
	$doc_Dear_Sirs = "Dear";
	$doc_AttachedFile = "Attached you will find ";
	$doc_Kindest_regards = "Kindest regards";
	$doc_Payment_Link = "You can pay through";
}
if (isset($header2type) || isset($linetype))
{
	$doc_Invoice_no = ($doctype==13 ? "Delivery Note No." : (($doctype == 7 || $doctype==8 || $doctype==9) ? "Order no." : 
	($doctype==10 ? "Invoice No." : ($doctype == 26 ? "Work Order No" : "Credit No."))));
	$doc_Delivery_no = "Delivery Note No.";
	$doc_Order_no = "Order no.";
}
if (isset($linetype))
{
	if ($doctype == 7 || $doctype == 8 || $doctype == 9 || $doctype == 10 || $doctype == 11 || $doctype == 13)
	{
		$doc_Sub_total = "Sub-total";
		$doc_Shipping = "Shipping";
		$doc_Included = "Included";
		$doc_Amount = "amount";
		$doc_TOTAL_INVOICE = $doctype==10 ? "TOTAL INVOICE" : "TOTAL CREDIT";
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