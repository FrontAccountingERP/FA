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
//include_once($path_to_root . "reporting/includes/class.pdf.inc");
include_once(dirname(__FILE__)."/class.pdf.inc");
include_once(dirname(__FILE__)."/printer_class.inc");
include_once($path_to_root . "/admin/db/company_db.inc");
include_once($path_to_root . "/admin/db/printers_db.inc");
include_once($path_to_root . "/config.php");
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
	//var $rtl;

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
	var $oldFontSize;
	var $currency;

	function FrontReport($title, $filename, $size = 'A4', $fontsize = 9)
	{
		global $page_security;
		if (!$_SESSION["wa_current_user"]->can_access_page($page_security))
		{
			display_error(_("The security settings on your account do not permit you to print this report"));
			end_page();
			exit;
		}
		switch ($size)
		{
			default:
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
		$this->filename = $filename.".pdf";
		$this->pageNumber = 0;
		$this->endLine = $this->pageWidth - $this->rightMargin;
		$this->companyCol = $this->endLine - 150;
		$this->titleCol = $this->leftMargin + 100;
		$this->lineHeight = 12;
		$this->fontSize = $fontsize;
		$this->oldFontSize = 0;
		$this->row = $this->pageHeight - $this->topMargin;
		$this->currency = '';
		$rtl = ($_SESSION['language']->dir === 'rtl' ? 'rtl' : 'ltr');
		$code = $_SESSION['language']->code;
		$enc = strtoupper($_SESSION['language']->encoding);
		// for the language array in class.pdf.inc
		$l = array('a_meta_charset' => $enc, 'a_meta_dir' => $rtl, 'a_meta_language' => $code, 'w_page' => 'page');
		$this->Cpdf($size, $l);
	}

	function Font($style = 'normal')
	{
		$this->selectFont("", $style);
	}

	function Info($params, $cols, $headers, $aligns,
		$cols2 = null, $headers2 = null, $aligns2 = null)
	{
		global $app_title, $version, $power_by, $power_url;

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
		if ($this->company['time_zone'])
			$str .= ' ' . date('O') . ' GMT';
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
		$this->Text($this->endLine - 38, $str);
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
		global $comp_path, $path_to_root, $print_as_quote, $print_invoice_no, $packing_slip;

		$this->pageNumber++;
		if ($this->pageNumber > 1)
			$this->newPage();
		$header2type = true;
		if (isset($myrow['curr_code']) && $this->currency != $myrow['curr_code'])
		{
			include($path_to_root . "/reporting/includes/doctext2.inc");
		}
		else
		{
			include($path_to_root . "/reporting/includes/doctext.inc");
		}

		include($path_to_root . "/reporting/includes/header2.inc");

		$this->row = $temp;
	}

	function AddImage($logo, $x, $y, $w, $h)
	{
		if (strpos($logo, ".png") || strpos($logo, ".PNG"))
			$this->addPngFromFile($logo, $x, $y, $w, $h);
		else
			$this->addJpegFromFile($logo, $x, $y, $w, $h);
	}

	function SetDrawColor($r, $g, $b)
	{
		$this->setStrokeColor($r, $g, $b);
	}

	function SetTextColor($r, $g, $b)
	{
		TCPDF::SetTextColor($r, $g, $b);
	}

	function Text($c, $txt, $n=0, $corr=0, $r=0)
	{
		if ($n == 0)
			$n = $this->pageWidth - $this->rightMargin;

		return $this->TextWrap($c, $this->row - $r, $n - $c + $corr, $txt, 'left');
	}

	function TextWrap($xpos, $ypos, $len, $str, $align = 'left')
	{
		if ($this->fontSize != $this->oldFontSize)
		{
			$this->SetFontSize($this->fontSize);
			$this->oldFontSize = $this->fontSize;
		}
		return $this->addTextWrap($xpos, $ypos, $len, $this->fontSize, $str, $align);
	}

	function TextCol($c, $n, $txt, $corr=0, $r=0)
	{
		return $this->TextWrap($this->cols[$c], $this->row - $r, $this->cols[$n] - $this->cols[$c] + $corr, $txt, $this->aligns[$c]);
	}
	
	function AmountCol($c, $n, $txt, $dec=0, $corr=0, $r=0)
	{
		return $this->TextCol($c, $n, number_format2($txt, $dec), $corr, $r);
	}

	function DateCol($c, $n, $txt, $conv=false, $corr=0, $r=0)
	{
		if ($conv)
			$txt = sql2date($txt);
		return $this->TextCol($c, $n, $txt, $corr, $r);
	}

	function TextCol2($c, $n, $txt, $corr=0, $r=0)
	{
		return $this->TextWrap($this->cols2[$c], $this->row - $r, $this->cols2[$n] - $this->cols2[$c] + $corr, $txt, $this->aligns2[$c]);
	}

	function TextColLines($c, $n, $txt, $corr=0, $r=0)
	{
		$this->row -= $r;
		$this->TextWrapLines($this->cols[$c], $this->cols[$n] - $this->cols[$c] + $corr, $txt, $this->aligns[$c]);
	}

	function TextWrapLines($c, $width, $txt, $align='left')
	{
		$str = Explode("\n", $txt);
		for ($i = 0; $i < count($str); $i++)
		{
			$l = $str[$i];
			do
			{
				$l = $this->TextWrap($c, $this->row , $width, $l, $align);
				$this->row -= $this->lineHeight;
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
		if ($this->row < $this->bottomMargin + ($np * $this->lineHeight))
			$this->Header();
	}

	function End($email=0, $subject=null, $myrow=null, $doctype = 0)
	{
		global $pdf_debug, $path_to_root, $comp_path;

		if ($pdf_debug == 1)
		{
			$pdfcode = $this->Output('','S');
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

			$dir =  $comp_path.'/'.user_company(). '/pdf_files';
			//save the file
			if (!file_exists($dir))
			{
				mkdir ($dir,0777);
			}
			// do not use standard filenames or your sensitive company data 
			// are world readable
			if ($email == 1)
				$fname = $dir.'/'.$this->filename;
			else	
				$fname = $dir.'/'.uniqid('').'.pdf';
			$this->Output($fname, 'F');
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
				require_once($path_to_root . "/reporting/includes/class.mail.inc");
    			$mail = new email($this->company['coy_name'], $this->company['email']);
				if (!isset($myrow['email']) || $myrow['email'] == '') 
					$myrow['email'] = isset($myrow['contact_email']) ? $myrow['contact_email'] : '';
    			$to = $myrow['DebtorName'] . " <" . $myrow['email'] . ">";
    			$msg = $doc_Dear_Sirs . " " . $myrow['DebtorName'] . ",\n\n" . $doc_AttachedFile . " " . $subject .
    				"\n\n";
				if (isset($myrow['dimension_id']) && $myrow['dimension_id'] > 0 && $doctype == 10) // helper for payment links
				{
					if ($myrow['dimension_id'] == 1)
					{
						$amt = number_format($myrow["ov_freight"] + $myrow["ov_gst"] +	$myrow["ov_amount"], user_price_dec());
						$txt = $doc_Payment_Link . " PayPal: ";
						$nn = urlencode($this->title . " " . $myrow['reference']);
						$url = "https://www.paypal.com/xclick/business=" . $this->company['email'] . "&item_name=" .
							$nn . "&amount=" . $amt . "&currency_code=" . $myrow['curr_code'];
						$msg .= $txt . $url . "\n\n";
					}
				}
    			$msg .= $doc_Kindest_regards . "\n\n";
    			$sender = $this->user . "\n" . $this->company['coy_name'] . "\n" . $this->company['postal_address'] . "\n" . $this->company['email'] . "\n" . $this->company['phone'];
    			$mail->to($to);
    			$mail->subject($subject);
    			$mail->text($msg . $sender);
    			$mail->attachment($fname);
    			$ret = $mail->send();
				if (!$ret)
					display_error(_("Sending document by email failed"));
				else
					display_notification($this->title . " " . $myrow['reference'] . " " 
						. _("has been sent by email."));
				unlink($fname);		
			}
			else
			{
				$printer = get_report_printer(user_print_profile(), $_POST['REP_ID']);
				if ($printer == false) {
					if(in_ajax()) {
						global $Ajax;

						if (user_rep_popup()) 
							$Ajax->popup($fname); // when embeded pdf viewer used
						else
							$Ajax->redirect($fname); // otherwise use faster method
					} else {
					//echo '<html>
					//		<head>
					//	  	  <SCRIPT LANGUAGE="JavaScript"><!--
					//	      function go_now () { window.location.href = "'.$fname.'"; }
					//	      //--></SCRIPT>
					//	  </head>
					//	  <body onLoad="go_now()"; >
					//	    <a href="'.$fname.'">click here</a> if you are not re-directed.
					//	  </body>
					//    </html>';
	                header('Content-type: application/pdf');
    	            header("Content-Disposition: inline; filename=$this->filename");
        	        header('Expires: 0');
            	    header('Cache-Control: must-revalidate, post-check=0, pre-check=0');
                	header('Pragma: public');

	                $this->Stream();
					}
				} else { // send report to network printer
					$prn = new remote_printer($printer['queue'],$printer['host'],
						$printer['port'], $printer['timeout']);
					$error = $prn->print_file($fname);
					if ($error)
						display_error($error);
					else
						display_notification(_('Report has been sent to network printer ').$printer['name']);
				}
			}
			// first have a look through the directory, 
			// and remove old temporary pdfs
			if ($d = @opendir($dir)) {
				while (($file = readdir($d)) !== false) {
					if (!is_file($dir.'/'.$file) || $file == 'index.php') continue;
				// then check to see if this one is too old
					$ftime = filemtime($dir.'/'.$file);
				 // seems 3 min is enough for any report download, isn't it?
					if (time()-$ftime > 180){
						unlink($dir.'/'.$file);
					}
				}
				closedir($d);
			}
		}
	}
}

?>