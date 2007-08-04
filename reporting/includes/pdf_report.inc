<?php
/* $Revision$ */
$page_security = 8;
include_once($path_to_root . "reporting/includes/class.pdf.inc");
include_once($path_to_root . "admin/db/company_db.inc");

class FrontReport extends Cpdf
{
	var $size;
	var $company;
	var $user;
	var $host;
	var $fiscal_year;
	var $title;
	var $filename;
	var $pageWidth;
	var $pageHeight;
	var $topMargin;
	var $bottomMargin;
	var $leftMargin;
	var $rightMargin;
	var $endLine;
	var $lineHeight;
	var $rtl;

	var $cols;
	var $params;
	var $headers;
	var $aligns;
	var $headers2;
	var $aligns2;
	var $cols2;
	var $companyCol;
	var $titleCol;
	var $pageNumber;
	var $fontSize;
	var $currency;

	function FrontReport($title, $filename, $size = 'A4', $fontsize = 9)
	{
		switch ($size) 
		{

		  case 'A4':
		  case 'a4':
			  $this->pageWidth=595;
			  $this->pageHeight=842;
			  $this->topMargin=40;
			  $this->bottomMargin=30;
			  $this->leftMargin=40;
			  $this->rightMargin=30;
			  break;
		  case 'A4_Landscape':
			  $this->pageWidth=842;
			  $this->pageHeight=595;
			  $this->topMargin=30;
			  $this->bottomMargin=30;
			  $this->leftMargin=40;
			  $this->rightMargin=30;
			  break;
		   case 'A3':
			  $this->pageWidth=842;
			  $this->pageHeight=1190;
			  $this->topMargin=50;
			  $this->bottomMargin=50;
			  $this->leftMargin=50;
			  $this->rightMargin=40;
			  break;
		   case 'A3_landscape':
			  $this->pageWidth=1190;
			  $this->pageHeight=842;
			  $this->topMargin=50;
			  $this->bottomMargin=50;
			  $this->leftMargin=50;
			  $this->rightMargin=40;
			  break;
		   case 'letter':
		   case 'Letter':
			  $this->pageWidth=612;
			  $this->pageHeight=792;
			  $this->topMargin=30;
			  $this->bottomMargin=30;
			  $this->leftMargin=30;
			  $this->rightMargin=25;
			  break;
		   case 'letter_landscape':
			  $this->pageWidth=792;
			  $this->pageHeight=612;
			  $this->topMargin=30;
			  $this->bottomMargin=30;
			  $this->leftMargin=30;
			  $this->rightMargin=25;
			  break;
		   case 'legal':
			  $this->pageWidth=612;
			  $this->pageHeight=1008;
			  $this->topMargin=50;
			  $this->bottomMargin=40;
			  $this->leftMargin=30;
			  $this->rightMargin=25;
			  break;
		   case 'legal_landscape':
			  $this->pageWidth=1008;
			  $this->pageHeight=612;
			  $this->topMargin=50;
			  $this->bottomMargin=40;
			  $this->leftMargin=30;
			  $this->rightMargin=25;
			  break;
		}
		$this->size = array(0, 0, $this->pageWidth, $this->pageHeight);
		$this->title = $title;
		$this->filename = $filename;
		$this->pageNumber = 0;
		$this->endLine = $this->pageWidth - $this->rightMargin;
		$this->companyCol = $this->endLine - 150;
		$this->titleCol = $this->leftMargin + 100;
		$this->lineHeight = 12;
		$this->fontSize = $fontsize;
		$this->currency = '';
		$this->rtl = ($_SESSION['language']->dir == 'rtl');
		// for quick testing
		//$this->rtl = true;
		$this->Cpdf($this->size);
	}

	function Font($style = 'normal')
	{
		global $path_to_root;
		$own = false;
		$locale = $path_to_root . "lang/" . $_SESSION['language']->code . "/locale.inc";
		if (file_exists($locale))
		{
			$fontinclude = true;
			include($locale);
		}	
		//if ($this->rtl) // this is good for presentation only
		if (!$own)
		{
			if (user_price_dec() == 3) // only for presentation of farsi
			{
				$this->rtl = true;
				$path = $path_to_root . 'reporting/fonts/farsi_1.afm';
			}		
			elseif ($style == 'italic')
				$path = $path_to_root . 'reporting/fonts/Helvetica-Oblique.afm';
			elseif ($style == 'bold')
				$path = $path_to_root . 'reporting/fonts/Helvetica-Bold.afm';
			else // even take misspelled styles
				$path = $path_to_root . 'reporting/fonts/Helvetica.afm';
		}		
		$this->selectFont($path, 'WinAnsiEncoding');
	}

	function Info($params, $cols, $headers, $aligns,
		$cols2 = null, $headers2 = null, $aligns2 = null)
	{
		global $app_title, $version, $power_by, $power_url, $path_to_root,	$db_connections;

		$this->addinfo('Title', $this->title);
		$this->addinfo('Subject', $this->title);
		$this->addinfo('Author', $app_title . ' ' . $version);
		$this->addinfo('Creator',$power_by . ' - ' . $power_url);
		$year = get_current_fiscalyear();
		if ($year['closed'] == 0)
			$how = _("Active");
		else
			$how = _("Closed");
		$this->fiscal_year = sql2date($year['begin']) . " - " . sql2date($year['end']) . "  " . "(" . $how . ")";
		$this->company = get_company_prefs();
		$this->user = $_SESSION["wa_current_user"]->name;
		//$this->host = $db_connections[$_SESSION["wa_current_user"]->company]["host"];
		$this->host = $_SERVER['SERVER_NAME'];
		$this->params = $params;
		$this->cols = $cols;
		for ($i = 0; $i < count($this->cols); $i++)
			$this->cols[$i] += $this->leftMargin;
		$this->headers = $headers;
		$this->aligns = $aligns;
		$this->cols2 = $cols2;
		if ($this->cols2 != null)
		{
			for ($i = 0; $i < count($this->cols2); $i++)
				$this->cols2[$i] += $this->leftMargin;
		}
		$this->headers2 = $headers2;
		$this->aligns2 = $aligns2;
	}

	function Header()
	{
		$this->pageNumber++;
		if ($this->pageNumber > 1)
			$this->newPage();
		$this->row = $this->pageHeight - $this->topMargin;

		$this->SetDrawColor(128, 128, 128);
		$this->Line($this->row + 5, 1);

		$this->NewLine();

		$this->fontSize += 4;
		$this->Font('bold');
		$this->Text($this->leftMargin, $this->title, $this->companyCol);
		$this->Font();
		$this->fontSize -= 4;
		$this->Text($this->companyCol, $this->company['coy_name']);
		$this->row -= ($this->lineHeight + 4);

		$str = _("Print Out Date") . ':';
		$this->Text($this->leftMargin, $str, $this->titleCol);
		$str = Today() . '   ' . Now();
		$this->Text($this->titleCol, $str, $this->companyCol);
		$this->Text($this->companyCol, $this->host);

		$this->NewLine();
		$str = _("Fiscal Year") . ':';
		$this->Text($this->leftMargin, $str, $this->titleCol);
		$str = $this->fiscal_year;
		$this->Text($this->titleCol, $str, $this->companyCol);
		$this->Text($this->companyCol, $this->user);
		for ($i = 1; $i < count($this->params); $i++)
		{
			if ($this->params[$i]['from'] != '')
			{
				$this->NewLine();
				$str = $this->params[$i]['text'] . ':';
				$this->Text($this->leftMargin, $str, $this->titleCol);
				$str = $this->params[$i]['from'];
				if ($this->params[$i]['to'] != '')
					$str .= " - " . $this->params[$i]['to'];
				$this->Text($this->titleCol, $str, $this->companyCol);
			}
		}	
		if ($this->params[0] != '') // Comments
		{
			$this->NewLine();
			$str = _("Comments") . ':';
			$this->Text($this->leftMargin, $str, $this->titleCol);
			$this->Font('bold');
			$this->Text($this->titleCol, $this->params[0], $this->endLine - 35);
			$this->Font();
		}
		$str = _("Page") . ' ' . $this->pageNumber;
		$this->Text($this->endLine - 35, $str);
		$this->Line($this->row - 5, 1);

		$this->row -= ($this->lineHeight + 6);
		$this->Font('italic');
		if ($this->headers2 != null)
		{
			$count = count($this->headers2);
			for ($i = 0; $i < $count; $i++)
				$this->TextCol2($i, $i + 1,	$this->headers2[$i]);
			$this->NewLine();
		}
		$count = count($this->headers);
		for ($i = 0; $i < $count; $i++)
			$this->TextCol($i, $i + 1, $this->headers[$i]);
		$this->Font();
		$this->Line($this->row - 5, 1);

		$this->NewLine(2);
	}

	function Header2($myrow, $branch, $sales_order, $bankaccount, $doctype)
	{
		global $path_to_root, $print_as_quote;
		
		$this->pageNumber++;
		if ($this->pageNumber > 1)
			$this->newPage();
		$header2type = true;
		if ($this->currency != $myrow['curr_code'])
		{
			include($path_to_root . "reporting/includes/doctext2.inc");			
		}	
		else
		{
			include($path_to_root . "reporting/includes/doctext.inc");			
		}	
		$this->row = $this->pageHeight - $this->topMargin;
		
		$upper = $this->row - 2 * $this->lineHeight;
		$lower = $this->bottomMargin + 6 * $this->lineHeight;
		$iline1 = $upper - 4 * $this->lineHeight;
		$iline2 = $iline1 - 8 * $this->lineHeight;
		$iline3 = $iline2 - 8 * $this->lineHeight;
		$iline4 = $iline3 - 2 * $this->lineHeight;
		$iline5 = $lower + 5 * $this->lineHeight;
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
			$logo = $path_to_root . "themes/default/images/" . $this->company['coy_logo'];
			$this->AddImage($logo, $ccol, $iline1 + 5, 220, 40);
		}
		else
		{
			$this->fontSize += 4;
			$this->Font('bold');
			$this->Text($ccol, $this->company['coy_name'], $icol);
			$this->Font();
			$this->fontSize -= 4;
		}
		if ($doctype == 8) // PO
			$this->Text($mcol, $myrow['order_no'], $mcol + 90);
		else if ($doctype == 9) // SO
			$this->Text($mcol, $myrow['order_no'] ." ".$myrow['customer_ref'], $mcol + 90);
		else // INV/CRE/STA	
			$this->Text($mcol, $myrow['reference'], $mcol + 90);
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
			//$adr = ?;
		}	
		else
		{
			if ($doctype == 8)
				$this->Text($ccol, $myrow['supp_name'], $icol);
			else	
				$this->Text($ccol, $myrow['DebtorName'], $icol);
			$adr = explode("\n", $myrow['address']);
		}	
		for ($i = 0; $i < count($adr); $i++)
		{	
			$this->NewLine();
			$this->Text($ccol, $adr[$i], $icol);
		}
		if ($sales_order != NULL)
		{
			$this->row = $temp;
			if ($doctype == 8)
				$this->Text($mcol, $this->company['coy_name']);
			else	
				$this->Text($mcol, $sales_order['deliver_to']);
			$adr = explode("\n", $sales_order['delivery_address']);
			for ($i = 0; $i < count($adr); $i++)
			{	
				$this->NewLine();
				$this->Text($mcol, $adr[$i]);
			}
		}
		$this->row = $iline2 - 2 * $this->lineHeight;
		$this->Text($ccol, $doc_Shipping_Company . ":", $ccol2);
		if ($doctype != 8)
			$this->Text($ccol2, $myrow['shipper_name'], $mcol);
		$this->Text($mcol, $doc_Due_Date . ":", $mcol2);
		if ($doctype == 9)
			$this->Text($mcol2, sql2date($myrow['delivery_date']));
		else if ($doctype != 8)	
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
		if ($doctype != 8)
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
		$this->Text($mcol2, $myrow['order_']);
		
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
		if ($this->params['comments'] != '')
		{
			$this->NewLine(2);
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
		
		$adr = explode("\n", $this->company['postal_address']);
		for ($i = 0; $i < count($adr); $i++)
		{	
			$this->NewLine();
			$this->Text($ccol, $adr[$i], $ccol2 + 40);
		}
		$this->row = $adrline; 
		$this->Text($ccol2 + 30, $this->company['phone'], $mcol);
		$this->NewLine();
		$this->Text($ccol2 + 30, $this->company['fax'], $mcol);
		$this->NewLine();
		$this->Text($ccol2 + 30, $this->company['email'], $mcol);
		// fetch this later
		$this->row = $adrline; 
		$this->Text($mcol, $bankaccount['bank_name'], $mcol2);
		$adr = explode("\n", $bankaccount['bank_address']);
		for ($i = 0; $i < count($adr); $i++)
		{	
			$this->NewLine();
			$this->Text($mcol, $adr[$i], $mcol2);
		}
		
		$this->row = $adrline; 
		$this->Text($mcol2, $bankaccount['bank_account_name']);
		$this->NewLine();
		$this->Text($mcol2, $bankaccount['bank_account_number']);
		$this->row = $temp;
	}

	function AddImage($logo, $x, $y, $w, $h)
	{
		if ($this->rtl)
			$x = $this->pageWidth - $x - $w;
		if (strpos($logo, ".png") || strpos($logo, ".PNG"))
			$this->addPngFromFile($logo, $x, $y, $w, $h);
		else
			$this->addJpegFromFile($logo, $x, $y, $w, $h);
	}	

	function SetDrawColor($r, $g, $b)
	{
		$this->setStrokeColor($r / 255, $g / 255, $b / 255);
	}
	
	function SetTextColor($r, $g, $b)
	{
		$this->setColor($r / 255, $g / 255, $b / 255);
	}

	function Text($c, $txt, $n=0, $corr=0, $r=0)
	{
		if ($n == 0)
			$n = $this->pageWidth - $this->rightMargin;

		return $this->TextWrap($c, $this->row - $r, $n - $c + $corr, $txt, 'left'); 
	}
	
	function TextWrap($xpos, $ypos, $len, $str, $align = 'left') 
	{
		if ($this->rtl)
		{
			$str = strrev($str);
			$xpos = $this->pageWidth - $xpos - $len;
			if ($align == 'left')
				$align = 'right';
			elseif ($align == 'right')
				$align = 'left';
		}	
		return $this->addTextWrap($xpos, $ypos, $len, $this->fontSize, $str, $align);
	}

	function TextCol($c, $n, $txt, $corr=0, $r=0)
	{
		return $this->TextWrap($this->cols[$c], $this->row - $r, $this->cols[$n] - $this->cols[$c] + $corr, $txt, $this->aligns[$c]);
	}
	
	function TextCol2($c, $n, $txt, $corr=0, $r=0)
	{
		return $this->TextWrap($this->cols2[$c], $this->row - $r, $this->cols2[$n] - $this->cols2[$c] + $corr, $txt, $this->aligns2[$c]);
	}
	
	function TextColLines($c, $n, $txt, $corr=0)
	{
		$str = Explode("\n", $txt);
		for ($i = 0; $i < count($str); $i++)
		{
			$l = $str[$i];
			do 
			{
				$l = $this->TextCol($c, $n, $l, $corr); 
				$this->NewLine();
			}
			while ($l != '');
		}	
	}

	function LineTo($from, $row, $to, $row2) 
	{
		Cpdf::line($from, $row, $to, $row2);
	}

	function Line($row, $height = 0) 
	{
		$this->setLineStyle($height + 1);
		Cpdf::line($this->pageWidth - $this->rightMargin, $row ,$this->leftMargin, $row);
	}

	function NewLine($l=1, $np=0)
	{
		$this->row -= ($l * $this->lineHeight);
		if ($np > 0 && 	$this->row < $this->bottomMargin + ($np * $this->lineHeight))
			$this->Header();
	}		

	function End($email=0, $subject=null, $myrow=null, $doctype = 0)
	{
		global $go_debug, $path_to_root;

		// this is no good in IE so has been replaced, see down under
		// but good for debugging purposes in IE
		//session_write_close();
		
		if ($go_debug == 1) 
		{
			$buf = $this->output(1);
			$len = strlen($buf);
			$pdfcode = $buf;
			$pdfcode = str_replace("\n", "\n<br>", htmlspecialchars($pdfcode));
			echo '<html><body>';
			echo trim($pdfcode);
			echo '</body></html>';
			//header("Content-Length: $len");
			//header("Content-Disposition: inline; filename=" . $this->filename);
			//header('Expires: 0');
			//header('Cache-Control: must-revalidate, post-check=0, pre-check=0');
			//header('Pragma: public');

			//$this->pdf->stream();
		} 
		else 
		{
			$buf = $this->output();
			$len = strlen($buf);
			$dir = './pdf_files';
			//save the file
			if (!file_exists($dir))
			{
				mkdir ($dir,0777);
			}
			$fname = $dir . '/' . $this->filename;
			$fp = fopen($fname,'w');
			fwrite($fp,$buf);
			fclose($fp);
			if ($email == 1)
			{
				$emailtype = true;
				if ($this->currency != $myrow['curr_code'])
				{
					include("doctext2.inc");			
				}	
				else
				{
					include("doctext.inc");			
				}	
				require_once($path_to_root . "reporting/includes/class.mail.inc");
    			$mail = new email($this->company['coy_name'], $this->company['email']);
    			$from = $this->company['coy_name'] . " <" . $this->company['email'] . ">";
    			$to = $myrow['DebtorName'] . " <" . $myrow['email'] . ">";
    			$msg = $doc_Dear_Sirs . ",\n\n" . $doc_AttachedFile . " " . $subject . 
    				"\n\n" . $doc_Kindest_regards . "\n\n";
    			$sender = $this->user . "\n" . $this->company['coy_name'];
    			$mail->to($to);
    			$mail->subject($subject);
    			$mail->text($msg . $sender);
    			$mail->attachment($fname);
    			$ret = $mail->send();
    			if (1 == 1) // just for fun and for debugging purposes!!
    			{
    				$from = str_replace("<", "(", $from);
    				$from = str_replace(">", ")", $from);
    				$to = str_replace("<", "(", $to);
    				$to = str_replace(">", ")", $to);
    				$msg2 = "<br>From: " . $from;
    				$msg2 .= "<br>To: " . $to;
    				$msg2 .= "<br>Subject: " . $subject;
    				$msg2 .= "<br>Msg: " . nl2br($msg);
    				$msg2 .= nl2br($sender) . "<br>";
    				$msg2 .= "<br>Filepath: " . $fname . "<br>Filename: " . $this->filename . "<br>";
    			}
    			if ($ret)
    				$str = "<br>" . $this->title . " " . $myrow['reference'] . " " . _("sent to") . " ";
    			else
    				$str = "<br>" . $this->title . " " . $myrow['reference'] . " " . _("NOT sent to") . " ";
    			$msg2 .= $str . $myrow['DebtorName'] . " - " . $myrow['email'];
    			echo "<html><body>";
    			echo $msg2;
    			echo "</body></html>";
			}
			else
			{
				echo '<html>
						<head>
					  	  <SCRIPT LANGUAGE="JavaScript"><!--
					      function go_now () { window.location.href = "'.$fname.'"; }
					      //--></SCRIPT>
					  </head>
					  <body onLoad="go_now()"; >
					    <a href="'.$fname.'">click here</a> if you are not re-directed.
					  </body>
				    </html>';
			}	    
			// also have a look through the directory, and remove the files that are older than a week
			// rather want to save 'em
			/*if ($d = @opendir($dir)) {
				while (($file = readdir($d)) !== false) {
					// then check to see if this one is too old
					$ftime = filemtime($dir.'/'.$file);
					if (time()-$ftime > 3600*24*7){
						unlink($dir.'/'.$file);
					}
				}
				closedir($d);
			}*/
		}
	}
}

?>