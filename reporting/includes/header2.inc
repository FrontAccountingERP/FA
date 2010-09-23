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
		// New version (without vertical lines)

	global $wo_types_array, $systypes_array;
	
		$this->row = $this->pageHeight - $this->topMargin;

		$upper = $this->row - 2 * $this->lineHeight;
		$lower = $this->bottomMargin + 8 * $this->lineHeight;
		$iline1 = $upper - 7.5 * $this->lineHeight;
		$iline2 = $iline1 - 8 * $this->lineHeight;
		$iline3 = $iline2 - 1.5 * $this->lineHeight;
		$iline4 = $iline3 - 1.5 * $this->lineHeight;
		$iline5 = $iline4 - 3 * $this->lineHeight;
		$iline6 = $iline5 - 1.5 * $this->lineHeight;
		$iline7 = $lower;
		$right = $this->pageWidth - $this->rightMargin;
		$width = ($right - $this->leftMargin) / 5;
		$icol = $this->pageWidth / 2;
		$ccol = $this->cols[0] + 4;
		$c2col = $ccol + 60;
		$ccol2 = $icol / 2;
		$mcol = $icol + 8;
		$mcol2 = $this->pageWidth - $ccol2;
		$cols = count($this->cols);
		$this->SetDrawColor(205, 205, 205);
		$this->Line($iline1, 3);
		$this->SetDrawColor(128, 128, 128);
		$this->Line($iline1);
		$this->rectangle($this->leftMargin, $iline2, $right - $this->leftMargin, $iline2 - $iline3, "F", null, array(222, 231, 236));   
		$this->Line($iline2);
		$this->Line($iline3);
		$this->Line($iline4);
		$this->rectangle($this->leftMargin, $iline5, $right - $this->leftMargin, $iline5 - $iline6, "F", null, array(222, 231, 236));   
		$this->Line($iline5);
		$this->Line($iline6);
		$this->Line($iline7);
		$this->LineTo($this->leftMargin, $iline2 ,$this->leftMargin, $iline4);
		$col = $this->leftMargin;
		for ($i = 0; $i < 5; $i++)
		{
			$this->LineTo($col += $width, $iline2,$col, $iline4);
		}	
		$this->LineTo($right, $iline2 ,$right, $iline4);
		$this->LineTo($this->leftMargin, $iline5 ,$this->leftMargin, $iline7);
		$this->LineTo($this->cols[$cols - 2] + 4, $iline5 ,$this->cols[$cols - 2] + 4, $iline7);
		$this->LineTo($right, $iline5 ,$right, $iline7);

		$this->NewLine();
		if ($this->company['coy_logo'] != '')
		{
			$logo = company_path() . "/images/" . $this->company['coy_logo'];
			$this->AddImage($logo, $ccol, $this->row, 0, 40);
		}
		else
		{
			$this->fontSize += 4;
			$this->Font('bold');
			$this->Text($ccol, $this->company['coy_name'], $icol);
			$this->Font();
			$this->fontSize -= 4;
		}
		$this->SetTextColor(190, 190, 190);
		$this->fontSize += 10;
		$this->Font('bold');
		$this->TextWrap($mcol, $this->row, $this->pageWidth - $this->rightMargin - $mcol - 20, $this->title, 'right');
		$this->Font();
		$this->fontSize -= 10;
		$this->NewLine();
		$this->SetTextColor(0, 0, 0);
		$adrline = $this->row;
		$this->TextWrapLines($ccol, $icol, $this->company['postal_address']);
		$this->Font('italic');
		if ($this->company['phone'] != "")
		{
			$this->Text($ccol, _("Phone"), $c2col);
			$this->Text($c2col, $this->company['phone'], $mcol);
			$this->NewLine();
		}	
		if ($this->company['fax'] != "")
		{
			$this->Text($ccol, _("Fax"), $c2col);
			$this->Text($c2col, $this->company['fax'], $mcol);
			$this->NewLine();
		}	
		if ($this->company['email'] != "")
		{
			$this->Text($ccol, _("Email"), $c2col);

			$url = "mailto:" . $this->company['email'];
			$this->SetTextColor(0, 0, 255);
			$this->Text($c2col, $this->company['email'], $mcol);
			$this->SetTextColor(0, 0, 0);
			$this->addLink($url, $c2col, $this->row, $mcol, $this->row + $this->lineHeight);

			$this->NewLine();
		}	
		if ($this->company['gst_no'] != "")
		{
			$this->Text($ccol, $doc_Our_VAT_no, $c2col);
			$this->Text($c2col, $this->company['gst_no'], $mcol);
			$this->NewLine();
		}
		if (($doctype == ST_SALESINVOICE || $doctype == ST_STATEMENT) && $this->company['domicile'] != "")
		{
			$this->Text($ccol, $doc_Domicile, $c2col);
			$this->Text($c2col, $this->company['domicile'], $mcol);
			$this->NewLine();
		}
		$this->Font();
		$this->row = $adrline;
		$this->NewLine(3);
		$this->Text($mcol + 100, $doc_Date);
		if ($doctype == ST_SALESQUOTE || $doctype == ST_PURCHORDER || $doctype == ST_SALESORDER)
			$this->Text($mcol + 180, sql2date($this->formData['ord_date']));
		elseif ($doctype == ST_WORKORDER)	
			$this->Text($mcol + 180, sql2date($this->formData['date_']));
		else
			$this->Text($mcol + 180, sql2date($this->formData['tran_date']));
		$this->NewLine();
		$this->Text($mcol + 100, $doc_Invoice_no);
		if ($doctype == ST_SALESQUOTE || $doctype == ST_PURCHORDER || $doctype == ST_SALESORDER) // QUOTE, PO or SO
		{
			if ($print_invoice_no == 1)
				$this->Text($mcol + 180, $this->formData['order_no']);
			else	
				$this->Text($mcol + 180, $this->formData['reference']);
		}		
		elseif ($doctype == ST_WORKORDER)	
			$this->Text($mcol + 180, $this->formData['id']);
		else if (isset($this->formData['trans_no']) && isset($this->formData['reference'])) // INV/CRE/STA
		{
			if ($print_invoice_no == 1)
				$this->Text($mcol + 180, $this->formData['trans_no']);
			else
				$this->Text($mcol + 180, $this->formData['reference']);
		}

		if ($this->pageNumber > 1 && !strstr($this->filename, "Bulk"))
			$this->Text($this->endLine - 35, _("Page") . ' ' . $this->pageNumber);
		$this->row = $iline1 - $this->lineHeight;
		$this->fontSize -= 4;
		$this->Text($ccol, $doc_Charge_To, $icol);
		$this->Text($mcol, $doc_Delivered_To);
		$this->fontSize += 4;

		$this->row = $this->row - $this->lineHeight - 5;

		$temp = $this->row;
		if ($doctype == ST_SALESQUOTE || $doctype == ST_SALESORDER)
		{
			$this->Text($ccol, $this->formData['name'], $icol);
		}
		elseif ($doctype == ST_WORKORDER)
		{
			$this->Text($ccol, $this->formData['location_name'], $icol);
			$this->NewLine();
			$this->TextWrapLines($ccol, $icol - $ccol, $this->formData['delivery_address']);
		}
		else
		{
			if ($doctype == ST_PURCHORDER || $doctype == ST_SUPPAYMENT) {
				$name = $this->formData['supp_name'];
				$addr = $this->formData['address'];
			} else {
				if (isset($this->formData['br_address']) && 
						trim($this->formData['br_address']) != '') {
					$name = $this->formData['br_name'];
					$addr = $this->formData['br_address'];
				} else {
					$name = $this->formData['DebtorName'];
					$addr = $this->formData['address'];
				}
			}
			$this->Text($ccol, $name, $icol);
			$this->NewLine();
			$this->TextWrapLines($ccol, $icol - $ccol, $addr);
		}
		$this->row = $temp;
		if (isset($this->formData['deliver_to']))
		{
			if ($doctype == ST_PURCHORDER)
				$this->Text($mcol, $this->company['coy_name']);
			elseif ($doctype != ST_SUPPAYMENT && isset($this->formData['deliver_to']))
			{
				$this->Text($mcol, $this->formData['deliver_to']);
				$this->NewLine();
			}	
			if ($doctype != ST_SUPPAYMENT && isset($this->formData['delivery_address']))
				$this->TextWrapLines($mcol, $this->rightMargin - $mcol, $this->formData['delivery_address']);
		}	// if you need the company name in purchase order then write it as first line in location addresss.	
		elseif ($doctype == ST_PURCHORDER && isset($this->formData['delivery_address']))
			$this->TextWrapLines($mcol, $this->rightMargin - $mcol, $this->formData['delivery_address']);
		$this->row = $iline2 - $this->lineHeight - 1;
		$col = $this->leftMargin;
		$this->TextWrap($col, $this->row, $width, $doc_Customers_Ref, 'C');
		$col += $width;
		$this->TextWrap($col, $this->row, $width, $doc_Our_Ref, 'C');
		$col += $width;
		$this->TextWrap($col, $this->row, $width, $doc_Your_VAT_no, 'C');
		$col += $width;
		if ($doctype == ST_SALESINVOICE)
			$this->TextWrap($col, $this->row, $width, $doc_Delivery_no, 'C');
		else
			$this->TextWrap($col, $this->row, $width, $doc_Our_Order_No, 'C');
		$col += $width;
		$this->TextWrap($col, $this->row, $width, $doc_Due_Date, 'C');
		$this->row = $iline3 - $this->lineHeight - 1;
		$col = $this->leftMargin;
		if ($doctype == ST_PURCHORDER || $doctype == ST_SUPPAYMENT)
			$this->TextWrap($col, $this->row, $width, $this->formData['supp_account_no'], 'C');
		elseif ($doctype == ST_WORKORDER)
			$this->TextWrap($col, $this->row, $width, $this->formData['wo_ref'], 'C');
		elseif (isset($this->formData["customer_ref"]))
			$this->TextWrap($col, $this->row, $width, $this->formData["customer_ref"], 'C');
		elseif (isset($this->formData["debtor_ref"]))
			$this->TextWrap($col, $this->row, $width, $this->formData["debtor_ref"], 'C');
		$col += $width;	
		if (isset($this->formData['salesman']))
		{
			$id = $this->formData['salesman'];
			//$sql = "SELECT salesman_name  FROM ".TB_PREF."salesman WHERE salesman_code='$id'";
			//$result = db_query($sql,"could not get sales person");
			//$row = db_fetch($result);
			//$this->TextWrap($col, $this->row, $width, $row['salesman_name'], 'C');
			$this->TextWrap($col, $this->row, $width, get_salesman_name($id), 'C');
		}		
		elseif ($doctype == ST_SUPPAYMENT || $doctype == ST_CUSTPAYMENT)
			$this->TextWrap($col, $this->row, $width, $systypes_array[$this->formData["type"]], 'C');
		elseif ($doctype == ST_WORKORDER)
			$this->TextWrap($col, $this->row, $width, $wo_types_array[$this->formData["type"]], 'C');
		$col += $width;	
		if ($doctype == ST_WORKORDER)
			$this->TextWrap($col, $this->row, $width, $this->formData["StockItemName"], 'C');
		elseif ($doctype != ST_PURCHORDER && isset($this->formData['tax_id']))
			$this->TextWrap($col, $this->row, $width, $this->formData['tax_id'], 'C');
		$col += $width;	
		if ($doctype == ST_SALESINVOICE)
		{
			$deliveries = get_sales_parent_numbers(ST_SALESINVOICE, $this->formData['trans_no']);
			if ($print_invoice_no == 0)
			{
				foreach($deliveries as $n => $delivery) {
					$deliveries[$n] = get_reference(ST_CUSTDELIVERY, $delivery);
				}
			}
			$this->TextWrap($col, $this->row, $width, implode(',', $deliveries), 'C');
		}
		elseif ($doctype == ST_CUSTDELIVERY)
		{
			$ref = $this->formData['order_'];
			if ($print_invoice_no == 0)
			{
				$ref = get_reference(ST_SALESORDER, $this->formData['order_']);
				if (!$ref)
					$ref = $this->formData['order_'];
			}		
			$this->TextWrap($col, $this->row, $width, $ref, 'C');
		}
		elseif ($doctype == ST_WORKORDER)
			$this->TextWrap($col, $this->row, $width, $this->formData["location_name"], 'C');
		elseif (isset($this->formData['order_']) && $this->formData['order_'] != 0)
			$this->TextWrap($col, $this->row, $width, $this->formData['order_'], 'C');
		$col += $width;	
		if ($doctype == ST_SALESORDER || $doctype == ST_SALESQUOTE)
			$this->TextWrap($col, $this->row, $width, sql2date($this->formData['delivery_date']), 'C');
		elseif ($doctype == ST_WORKORDER)	
			$this->TextWrap($col, $this->row, $width, $this->formData["units_issued"], 'C');
		elseif ($doctype != ST_PURCHORDER && $doctype != ST_CUSTCREDIT && $doctype != ST_CUSTPAYMENT &&
			$doctype != ST_SUPPAYMENT && isset($this->formData['due_date']))
			$this->TextWrap($col, $this->row, $width, sql2date($this->formData['due_date']), 'C');
		if ((!isset($packing_slip) || $packing_slip == 0) && ($doctype != ST_STATEMENT)) //!
		{
			$this->row -= (2 * $this->lineHeight);
			if ($doctype == ST_WORKORDER)
				$str = sql2date($this->formData["required_by"]);
			else
			{
				$id = $this->formData['payment_terms'];
				$sql = "SELECT terms FROM ".TB_PREF."payment_terms WHERE terms_indicator='$id'";
				$result = db_query($sql,"could not get paymentterms");
				$row = db_fetch($result);
				$str = $row["terms"];
			}	
			$this->Font('italic');
			$this->TextWrap($ccol, $this->row, $right-$ccol, $doc_Payment_Terms . ":  " . $str);
			$this->Font();
		}
		$this->row = $iline5 - $this->lineHeight - 1;
		$this->Font('bold');
		$count = count($this->headers);
		$this->cols[$count] = $right - 3;
		for ($i = 0; $i < $count; $i++)
			$this->TextCol($i, $i + 1, $this->headers[$i], -2);
		$this->Font();
		$this->Font('italic');
		$this->row = $iline7 - $this->lineHeight - 6;
		if ($doctype != ST_WORKORDER)
			$this->TextWrap($ccol, $this->row, $right - $ccol, $doc_Please_Quote . " - " . $this->formData['curr_code'], 'C');
		$this->row -= $this->lineHeight;

		if (isset($this->formData['bank_name']))
		{
			$txt = $doc_Bank . ": ".$this->formData['bank_name']. ", " . $doc_Bank_Account . ": " . $this->formData['bank_account_number'];
			$this->TextWrap($ccol, $this->row, $right - $ccol, $txt, 'C');
			$this->row -= $this->lineHeight;
		}	
		if ($doctype == ST_SALESINVOICE && $this->formData['disable_branch'] > 0) // payment links
		{
			if ($this->formData['disable_branch'] == 1)
			{
				$amt = number_format($this->formData["ov_freight"] + $this->formData["ov_gst"] +	$this->formData["ov_amount"], user_price_dec());
				$txt = $doc_Payment_Link . " PayPal: ";
				$name = urlencode($this->title . " " . $this->formData['reference']);
				$url = "https://www.paypal.com/xclick/business=" . $this->company['email'] . "&item_name=" .
					$name . "&amount=" . $amt . "&currency_code=" . $this->formData['curr_code'];
				$this->fontSize -= 2;
				$this->TextWrap($ccol, $this->row, $right - $ccol, $txt, 'C');
				$this->row -= $this->lineHeight;
				$this->SetTextColor(0, 0, 255);
				$this->TextWrap($ccol, $this->row, $right - $ccol, $url, 'C');
				$this->SetTextColor(0, 0, 0);
				$this->addLink($url, $ccol, $this->row, $this->pageWidth - $this->rightMargin, $this->row + $this->lineHeight);
				$this->fontSize += 2;
				$this->row -= $this->lineHeight;
			}
		}
		if ($doc_Extra != "")
		{
			$this->TextWrap($ccol, $this->row, $right - $ccol, $doc_Extra, 'C');
			$this->row -= $this->lineHeight;
		}
		if ($this->params['comments'] != '')
		{
			$this->TextWrap($ccol, $this->row, $right - $ccol, $this->params['comments'], 'C');
			$this->row -= $this->lineHeight;
		}
		if (($doctype == ST_SALESINVOICE || $doctype == ST_STATEMENT) && $this->company['legal_text'] != "") 
		{
			$this->TextWrapLines($ccol, $right - $ccol, $this->company['legal_text'], 'C');
		}
		$this->Font();
		$temp = $iline6 - $this->lineHeight - 2;
?>