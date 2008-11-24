<?php

		$this->row = $this->pageHeight - $this->topMargin;

		$upper = $this->row - 2 * $this->lineHeight;
		$lower = $this->bottomMargin + 6 * $this->lineHeight;
		$iline1 = $upper - 4 * $this->lineHeight;
		$iline2 = $iline1 - 8 * $this->lineHeight;
		$iline3 = $iline2 - 8 * $this->lineHeight;
		$iline4 = $iline3 - 2 * $this->lineHeight;
		$iline5 = $lower + 4 * $this->lineHeight;
		$icol = $this->pageWidth / 2;
		$ccol = $this->cols[0] + 4;
		$ccol2 = $icol / 2;
		$mcol = $icol + 8;
		$mcol2 = $this->pageWidth - $ccol2;

		$this->SetDrawColor(128, 128, 128);
		$this->LineTo($this->pageWidth - $this->rightMargin, $upper ,$this->leftMargin, $upper);
		$this->LineTo($this->leftMargin, $upper ,$this->leftMargin, $lower);
		$this->LineTo($this->pageWidth - $this->rightMargin, $lower ,$this->leftMargin, $lower);
		$this->LineTo($this->pageWidth - $this->rightMargin, $lower ,$this->pageWidth - $this->rightMargin, $upper);
		$this->Line($iline1);
		$this->Line($iline2);
		$this->Line($iline3);
		$this->Line($iline4);
		$this->Line($iline5);
		$this->LineTo($icol, $upper ,$icol, $iline1);
		$this->NewLine();

		$this->fontSize += 4;
		$this->Font('bold');
		$this->Text($mcol, $this->title);
		$this->Font();
		$this->fontSize -= 4;
		if ($this->pageNumber > 1 && !strstr($this->filename, "Bulk"))
			$this->Text($this->endLine - 35, _("Page") . ' ' . $this->pageNumber);
		$this->fontSize -= 4;
		$this->row = $upper - 5;

		$this->Text($mcol, $doc_Invoice_no, $mcol + 90);
		$this->Text($mcol + 90, $doc_Cust_no, $mcol + 180);
		$this->Text($mcol + 180, $doc_Date);
		$this->fontSize += 4;

		$this->row = $upper - 2 * $this->lineHeight - 2;
		if ($this->company['coy_logo'] != '')
		{
			$logo = $comp_path .'/'. user_company() . "/images/" . $this->company['coy_logo'];
			$this->AddImage($logo, $ccol, $iline1 + 5, 0, 40);
		}
		else
		{
			$this->fontSize += 4;
			$this->Font('bold');
			$this->Text($ccol, $this->company['coy_name'], $icol);
			$this->Font();
			$this->fontSize -= 4;
		}
		if ($doctype == 8 || $doctype == 9) // PO or SO
			$this->Text($mcol, $myrow['order_no'], $mcol + 90);
		else if (isset($myrow['trans_no']) && isset($myrow['reference'])) // INV/CRE/STA
		{
			if ($print_invoice_no == 1)
				$this->Text($mcol, $myrow['trans_no'], $mcol + 90);
			else
				$this->Text($mcol, $myrow['reference'], $mcol + 90);
		}
		if (isset($myrow['debtor_no']))
			$this->Text($mcol + 90, $myrow['debtor_no'], $mcol + 180);
		if ($doctype == 8 || $doctype == 9)
			$this->Text($mcol + 180, sql2date($myrow['ord_date']));
		else
			$this->Text($mcol + 180, sql2date($myrow['tran_date']));

		$this->fontSize -= 4;
		$this->row = $iline1 - 5;
		$this->Text($ccol, $doc_Charge_To, $icol);
		$this->Text($mcol, $doc_Delivered_To);
		$this->fontSize += 4;

		$this->NewLine(2);
		$temp = $this->row;
		if ($doctype == 9)
		{
			$this->Text($ccol, $myrow['name'], $icol);
		}
		else
		{
			if ($doctype == 8)
				$this->Text($ccol, $myrow['supp_name'], $icol);
			else
				$this->Text($ccol, $myrow['DebtorName'], $icol);
			$this->NewLine();
			$this->TextWrapLines($ccol, $icol - $ccol, $myrow['address']);
		}
		if ($sales_order != NULL)
		{
			$this->row = $temp;
			if ($doctype == 8)
				$this->Text($mcol, $this->company['coy_name']);
			else
				$this->Text($mcol, $sales_order['deliver_to']);
			$this->NewLine();
			$this->TextWrapLines($mcol, $this->rightMargin - $mcol, $sales_order['delivery_address']);
		}
		$this->row = $iline2 - 2 * $this->lineHeight;
		$this->Text($ccol, $doc_Shipping_Company . ":", $ccol2);
		if ($doctype != 8 && isset($myrow['shipper_name']))
			$this->Text($ccol2, $myrow['shipper_name'], $mcol);
		$this->Text($mcol, $doc_Due_Date . ":", $mcol2);
		if ($doctype == 9)
			$this->Text($mcol2, sql2date($myrow['delivery_date']));
		else if ($doctype != 8 && $doctype != 11 && isset($myrow['due_date']))
			$this->Text($mcol2, sql2date($myrow['due_date']));
		if ($branch != null)
		{
			$this->NewLine();
			$this->Text($ccol, $doc_Your_Ref . ":", $ccol2);
			$this->Text($ccol2, $branch['contact_name'], $mcol);
			$this->Text($mcol, $doc_Our_Ref . ":", $mcol2);

			$id = $branch['salesman'];
			$sql = "SELECT salesman_name  FROM ".TB_PREF."salesman WHERE salesman_code='$id'";
			$result = db_query($sql,"could not get sales person");
			$row = db_fetch($result);

			$this->Text($mcol2, $row['salesman_name']);
		}
		$this->NewLine();
		$this->Text($ccol, $doc_Your_VAT_no . ":", $ccol2);
		if ($doctype != 8 && isset($myrow['tax_id']))
			$this->Text($ccol2, $myrow['tax_id'], $mcol);
		$this->Text($mcol, $doc_Our_VAT_no . ":", $mcol2);
		$this->Text($mcol2, $this->company['gst_no']);
		$this->NewLine();
		$this->Text($ccol, $doc_Payment_Terms . ":", $ccol2);

		$id = $myrow['payment_terms'];
		$sql = "SELECT terms FROM ".TB_PREF."payment_terms WHERE terms_indicator='$id'";
		$result = db_query($sql,"could not get paymentterms");
		$row = db_fetch($result);

		$this->Text($ccol2, $row["terms"], $mcol);
		$this->Text($mcol, $doc_Our_Order_No . ":", $mcol2);
		if (isset($myrow['order_']))
			$this->Text($mcol2, $myrow['order_']);
		// 2008-06-16. Added customer's reference
		$this->NewLine();
		$this->Text($ccol, $doc_Customers_Ref, $ccol2);
		if ($sales_order != NULL)
			$this->Text($ccol2, $sales_order["customer_ref"], $mcol);
		$locale = $path_to_root . "lang/" . $_SESSION['language']->code . "/locale.inc";
		if (file_exists($locale))
		{
			$header2include = true;
			include($locale);
		}
		$this->row = $iline3 - $this->lineHeight - 2;
		$this->Font('bold');
		$count = count($this->headers);
		for ($i = 0; $i < $count; $i++)
			$this->TextCol($i, $i + 1, $this->headers[$i], -2);
		$this->Font();
		$temp = $this->row - 2 * $this->lineHeight;
		$this->row = $iline5 - $this->lineHeight - 6;
		$this->Text($ccol, $doc_Please_Quote . " - " . $myrow['curr_code']);
		$this->NewLine();
		if ($doctype == 10 && $branch['disable_branch'] > 0) // payment links
		{
			if ($branch['disable_branch'] == 1)
			{
				$amt = number_format($myrow["ov_freight"] + $myrow["ov_gst"] +	$myrow["ov_amount"], user_price_dec());
				$txt = $doc_Payment_Link . " PayPal: ";
				$name = urlencode($this->title . " " . $myrow['reference']);
				$url = "https://www.paypal.com/xclick/business=" . $this->company['email'] . "&item_name=" .
					$name . "&amount=" . $amt . "&currency_code=" . $myrow['curr_code'];
				$this->fontSize -= 2;
				$this->Text($ccol, $txt);
				$this->NewLine();
				$this->SetTextColor(0, 0, 255);
				$this->Text($ccol, $url);
				$this->SetTextColor(0, 0, 0);
				$this->addLink($url, $ccol, $this->row, $this->pageWidth - $this->rightMargin, $this->row + $this->lineHeight);
				$this->fontSize += 2;
			}
		}
		if ($this->params['comments'] != '')
		{
			$this->NewLine();
			$this->Font('bold');
			$this->Text($ccol, $this->params['comments']);
			$this->Font();
		}

		$this->row = $lower - 5;
		$this->fontSize -= 4;

		$this->Text($ccol, $doc_Address, $ccol2 + 40);
		$this->Text($ccol2 + 30, $doc_Phone_Fax_Email, $mcol);
		$this->Text($mcol , $doc_Bank, $mcol2);
		$this->Text($mcol2, $doc_Bank_Account);
		$this->fontSize += 4;
		$this->NewLine();
		$adrline = $this->row;
		$this->TextWrapLines($ccol, $ccol2 + 40 - $ccol, $this->company['postal_address']);
		$this->row = $adrline;
		$this->Text($ccol2 + 30, $this->company['phone'], $mcol);
		$this->NewLine();
		$this->Text($ccol2 + 30, $this->company['fax'], $mcol);
		$this->NewLine();
		$this->Text($ccol2 + 30, $this->company['email'], $mcol);
		// fetch this later
		$this->row = $adrline;
		if (isset($bankaccount['bank_name']))
			$this->Text($mcol, $bankaccount['bank_name'], $mcol2);
		$this->NewLine();
		$this->TextWrapLines($mcol, $mcol2 - $mcol, $bankaccount['bank_address']);
		$this->row = $adrline;
		if (isset($bankaccount['bank_account_name']))
			$this->Text($mcol2, $bankaccount['bank_account_name']);
		$this->NewLine();
		if (isset($bankaccount['bank_account_number']))
			$this->Text($mcol2, $bankaccount['bank_account_number']);
?>