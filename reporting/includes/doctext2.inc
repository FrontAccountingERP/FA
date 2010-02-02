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
	if ($doctype == ST_PURCHORDER || $doctype == ST_SUPPAYMENT) // Purchase Order
	{
		$doc_Charge_To = "Order To";
		$doc_Delivered_To = "Charge To";
	}
	else
	{
		if ($doctype == ST_CUSTPAYMENT)
			$doc_Charge_To = "With thanks from";
		else	
			$doc_Charge_To = "Charge To";
		$doc_Delivered_To = "Delivered To";
	}	
	$doc_Shipping_Company = "Shipping Company";
	if ($doctype == ST_SALESQUOTE)
		$doc_Due_Date = "Valid until";
	elseif ($doctype == ST_SALESORDER)
		$doc_Due_Date = "Delivery Date";
	else
		$doc_Due_Date = "Due Date";
	$doc_Your_Ref = "Your Ref";
	if ($doctype == ST_WORKORDER)
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
		if ($doctype == ST_SUPPAYMENT || $doctype == ST_CUSTPAYMENT)
			$doc_Our_Ref = "Type";
		else	
			$doc_Our_Ref = "Sales Person";
		$doc_Your_VAT_no = "Your VAT no.";
		$doc_Payment_Terms = "Payment Terms";
		$doc_Customers_Ref = "Customers Reference";
		$doc_Our_Order_No = "Our Order No";
	}
	$doc_Extra = "";
	$doc_Our_VAT_no = "Our VAT No.";
	$doc_Domicile = "Domicile";
	if($doctype == ST_CUSTDELIVERY || $doctype == ST_SALESQUOTE || $doctype == ST_PURCHORDER || $doctype == ST_SALESORDER ||
		$doctype == ST_SUPPAYMENT || $doctype == ST_CUSTPAYMENT) {
		if ($doctype == ST_CUSTPAYMENT)
			$doc_Extra = "* Subject to Realisation of the Cheque.";
		$doc_Bank_Account = '';
	 	$doc_Please_Quote = "All amounts stated in";
  } else {
	 $doc_Please_Quote = "Please quote ".($doctype==ST_SALESINVOICE ? "Invoice" : "Credit")
	 	." no. when paying. All amounts stated in";
	 $doc_Bank_Account = "Bank Account";
	}
	$doc_Address = "Address";
	$doc_Phone_Fax_Email = "Phone/Fax/Email";
	$doc_Bank = "Bank";
	$doc_Payment_Link = "You can pay through";
	if ($doctype == ST_SALESQUOTE || $doctype == ST_PURCHORDER || $doctype == ST_SALESORDER || $doctype == ST_SALESINVOICE || 
		$doctype == ST_CUSTCREDIT || $doctype == ST_CUSTDELIVERY || $doctype == ST_WORKORDER ||	$doctype == ST_SUPPAYMENT || 
		$doctype == ST_CUSTPAYMENT)
	{
		if ($doctype == ST_SALESQUOTE)
			$this->title = "SALES QUOTATION";
		elseif ($doctype == ST_PURCHORDER)
			$this->title = "PURCHASE ORDER";
		elseif ($doctype == ST_SALESORDER)
			$this->title = ($print_as_quote==1 ? "QUOTE" : "SALES ORDER");
		elseif ($doctype == ST_SALESINVOICE)
			$this->title = "INVOICE";
		elseif ($doctype == ST_CUSTDELIVERY)
			$this->title = ($packing_slip==1 ? "PACKING SLIP" : "DELIVERY NOTE");
		elseif ($doctype == ST_WORKORDER)
			$this->title = "WORK ORDER";
		elseif ($doctype == ST_SUPPAYMENT)
			$this->title = "REMITTANCE";
		elseif ($doctype == ST_CUSTPAYMENT)
			$this->title = "RECEIPT";
		else
			$this->title = "CREDIT NOTE";
		if ($doctype == ST_PURCHORDER)
			$this->headers = array('Item Code', 'Item Description',	'Delivery Date',
				'Quantity', 'Unit',	'Price', 'Total');
		elseif ($doctype == ST_WORKORDER)
			$this->headers = array("Item Code", "Item Description",
				"From Location", "Work Centre", 	"Unit Quantity", "Total Quantity", "Units Issued");
		elseif ($doctype == ST_SUPPAYMENT || $doctype == ST_CUSTPAYMENT)
			$this->headers = array('Trans Type', '#', 'Date', 'Due Date', 'Total Amount', 'Left to Allocate', 'This Allocation');
		else
			$this->headers = array('Item Code', 'Item Description',	'Quantity', 'Unit',
				'Price', 'Discount %', 'Total');
	}
	elseif ($doctype == ST_STATEMENT)
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
	$doc_Invoice_no = ($doctype==ST_CUSTDELIVERY ? "Delivery Note No." :
		($doctype == ST_CUSTPAYMENT ? "Receipt No." :
		($doctype == ST_SUPPAYMENT ? "Remittance No." :
		(($doctype == ST_SALESQUOTE || $doctype==ST_PURCHORDER || $doctype==ST_SALESORDER) ? "Order no." : 
		($doctype==ST_SALESINVOICE ? "Invoice No." : ($doctype == ST_WORKORDER ? "Work Order No" : "Credit No."))))));
	$doc_Delivery_no = "Delivery Note No.";
	$doc_Order_no = "Order no.";
}
if (isset($linetype))
{
	if ($doctype == ST_SALESQUOTE || $doctype == ST_PURCHORDER || $doctype == ST_SALESORDER || $doctype == ST_SALESINVOICE || 
		$doctype == ST_CUSTCREDIT || $doctype == ST_CUSTDELIVERY)
	{
		$doc_Sub_total = "Sub-total";
		$doc_Shipping = "Shipping";
		$doc_Included = "Included";
		$doc_Amount = "amount";
		$doc_TOTAL_INVOICE = $doctype==ST_SALESINVOICE ? "TOTAL INVOICE" : "TOTAL CREDIT";
		$doc_TOTAL_ORDER = "TOTAL ORDER EX VAT";
		$doc_TOTAL_ORDER2 = "TOTAL ORDER VAT INCL.";
		$doc_TOTAL_PO = "TOTAL PO EX VAT";
		$doc_TOTAL_DELIVERY = "TOTAL DELIVERY INCL. VAT";
	}
	elseif ($doctype == ST_SUPPAYMENT || $doctype == ST_CUSTPAYMENT)
	{
		$doc_Towards = "As advance / full / part / payment towards:";
		$doc_by_Cheque = "By Cash / Cheque* / Draft No.";
		$doc_Dated = "Dated";
		$doc_Drawn = "Drawn on Bank";
		$doc_Drawn_Branch = "Branch";
		$doc_Received = "Received / Sign";
		$doc_Total_Allocated = "Total Allocated";
		$doc_Left_To_Allocate = "Left to Allocate";
		if ($doctype == ST_CUSTPAYMENT)
			$doc_Total_Payment = "TOTAL RECEIPT";
		else	
			$doc_Total_Payment = "TOTAL REMITTANCE";
	}
	elseif ($doctype == ST_STATEMENT)
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