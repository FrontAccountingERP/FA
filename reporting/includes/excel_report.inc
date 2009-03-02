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
/* $Revision$ */
$page_security = 8;
include_once($path_to_root . "/reporting/includes/ExcelWriterXML.php");
include_once($path_to_root . "/admin/db/company_db.inc");
include_once($path_to_root . "/config.php");

//class FrontReport extends ExcelWriterXML
class FrontReport
{
	var $xml;
	var $size;
	var $company;
	var $user;
	var $host;
	var $fiscal_year;
	var $title;
	var $filename;
	var $path;
	var $lineHeight;
	var $rtl;
	var $code;
	var $bottomMargin = 0;

	var $cols;
	var $params;
	var $headers;
	var $aligns;
	var $headers2;
	var $aligns2;
	var $cols2;
	var $fontSize;
	var $oldFontSize;
	var $currency;
	var $row = 9999999;
	var $y;
	
	var $sheet;
	var $formatTitle;
	var $formatDateTime;
	var $formatDate;
	var $formatRight;
	var $formatHeaderLeft;
	var $formatHeaderRight;
	var $formatAmount = array();
	var $formatFooter;

	function FrontReport($title, $filename, $size = 'A4', $fontsize = 9)
	{
		global $comp_path, $dateseps, $thoseps, $decseps;
		
		$this->size = $size;
		$this->title = $title;
		$this->lineHeight = 12;
		$this->fontSize = $fontsize;
		$this->oldFontSize = 0;
		$this->y = 1;
		$this->currency = '';
		$this->rtl = ($_SESSION['language']->dir === 'rtl' ? 'rtl' : 'ltr');
		$this->code = $_SESSION['language']->encoding;
		$this->filename = $filename;
		$this->path = $comp_path.'/'.user_company(). '/pdf_files/';
		//$this->ExcelWriterXML($this->filename);
		$this->xml = new ExcelWriterXML($this->filename);
		$this->xml->setCharSet($this->code);
		$this->xml->overwriteFile();
		$this->xml->showErrorSheet(true);
		$this->sheet = $this->xml->addSheet($this->title);
		if ($this->rtl === 'rtl')
			$this->sheet->displayRightToLeft();
		$this->formatTitle = $this->xml->addStyle('formatTitle');	
		$this->formatTitle->fontSize('18');
		$this->formatTitle->fontBold();
		$this->formatTitle->border('Top', '2', 'darkgray');
	
		$how = user_date_format();
		$sep = $dateseps[user_date_sep()];
		if ($sep == '.')
			$sep = "\\.";
		if ($how == 0)
		{
			$dateformat_long = "mm{$sep}dd{$sep}yyyy\ \ hh:mm\ am/pm";
			$dateformat = "mm{$sep}dd{$sep}yyyy";
		}	
		elseif ($how == 1)	
		{
			$dateformat_long = "dd{$sep}mm{$sep}yyyy\ \ hh:mm";
			$dateformat = "dd{$sep}mm{$sep}yyyy";
		}	
		else	
		{
			$dateformat_long = "yyyy{$sep}mm{$sep}dd\ \ hh:mm";
			$dateformat = "yyyy{$sep}mm{$sep}dd";
		}	
		$this->formatDateTime = $this->xml->addStyle('formatDateTime');
		$this->formatDateTime->numberFormatDateTime($dateformat_long);
		$this->formatDateTime->alignHorizontal('Left');
		$this->formatDate = $this->xml->addStyle('formatDate');
		$this->formatDate->numberFormatDateTime($dateformat);
		$this->formatDate->alignHorizontal('Left');
		$this->formatRight = $this->xml->addStyle('formatRight');
		$this->formatRight->alignHorizontal('Right');
		
		$this->formatHeaderLeft = $this->xml->addStyle("formatHeaderLeft");
		$this->formatHeaderLeft->fontItalic();
		$this->formatHeaderLeft->border('Top', '2', 'darkgray');
		$this->formatHeaderLeft->border('Bottom', '2', 'darkgray');
		$this->formatHeaderLeft->alignVertical('Center');
		$this->formatHeaderRight = $this->xml->addStyle("formatHeaderRight");
		$this->formatHeaderRight->fontItalic();
		$this->formatHeaderRight->alignHorizontal('Right');
		$this->formatHeaderRight->border('Top', '2', 'darkgray');
		$this->formatHeaderRight->border('Bottom', '2', 'darkgray');
		$this->formatHeaderRight->alignVertical('Center');
		$tsep = $thoseps[user_tho_sep()];
		$dsep = $decseps[user_dec_sep()];
		$amountformat = "###{$tsep}###{$tsep}###{$tsep}##0";
		$this->formatAmount[0] = $this->xml->addStyle("formatAmount0");
		$this->formatAmount[0]->numberFormat($amountformat);
		$this->formatAmount[0]->alignHorizontal('Right');
		$this->formatAmount[1] = $this->xml->addStyle("formatAmount1");
		$this->formatAmount[1]->numberFormat("$amountformat{$dsep}0");
		$this->formatAmount[1]->alignHorizontal('Right');
		$this->formatAmount[2] = $this->xml->addStyle("formatAmount2");
		$this->formatAmount[2]->numberFormat("$amountformat{$dsep}00");
		$this->formatAmount[2]->alignHorizontal('Right');
		$this->formatAmount[3] = $this->xml->addStyle("formatAmount3");
		$this->formatAmount[3]->numberFormat("$amountformat{$dsep}000");
		$this->formatAmount[3]->alignHorizontal('Right');
		$this->formatAmount[4] = $this->xml->addStyle("formatAmount4");
		$this->formatAmount[4]->numberFormat("$amountformat{$dsep}0000");
		$this->formatAmount[4]->alignHorizontal('Right');
		$this->formatFooter = $this->xml->addStyle("formatFooter");
		$this->formatFooter->border('Top', '2', 'darkgray');
	}

	function Font($style = 'normal')
	{
	}

	function Info($params, $cols, $headers, $aligns,
		$cols2 = null, $headers2 = null, $aligns2 = null)
	{
		global $app_title, $version, $power_by, $power_url;

		$this->company = get_company_prefs();
		$this->xml->docTitle($this->title);
		$this->xml->docAuthor($app_title . ' ' . $version);
		$this->xml->docCompany($this->company['coy_name']);
		$this->xml->docManager($power_by . ' - ' . $power_url);
		$year = get_current_fiscalyear();
		if ($year['closed'] == 0)
			$how = _("Active");
		else
			$how = _("Closed");
		$this->fiscal_year = sql2date($year['begin']) . " - " . sql2date($year['end']) . "  " . "(" . $how . ")";
		$this->user = $_SESSION["wa_current_user"]->name;
		$this->host = $_SERVER['SERVER_NAME'];
		$this->params = $params;
		$this->cols = $cols;
		$this->headers = $headers;
		$this->aligns = $aligns;
		$this->cols2 = $cols2;
		$this->headers2 = $headers2;
		$this->aligns2 = $aligns2;
		for ($i = 0; $i < count($this->headers); $i++)
			$this->sheet->columnWidth($i+1, $this->cols[$i + 1] - $this->cols[$i]);
	}

	function Header()
	{
		$this->y = 0;
		$this->NewLine();
		$this->sheet->rowHeight($this->y, 22);
		$this->sheet->writeString($this->y, 1, $this->title, $this->formatTitle);
		$this->sheet->cellMerge($this->y, 1, count($this->headers)-1, 0);

		$this->NewLine();

		$str = _("Print Out Date") . ':';
		$this->sheet->writeString($this->y, 1, $str);
		$date = $this->sheet->convertMysqlDateTime(date('Y-m-d h:i:s'));
		$this->sheet->writeDateTime($this->y, 2, $date, $this->formatDateTime);
		$this->sheet->writeString($this->y, 4, $this->company['coy_name']);
		$this->sheet->cellMerge($this->y, 4, 1, 0);
		$this->NewLine();
		$str = _("Fiscal Year") . ':';
		$this->sheet->writeString($this->y, 1, $str);
		$str = $this->fiscal_year;
		$this->sheet->writeString($this->y, 2, $str);
		$this->sheet->writeString($this->y, 4, $this->host);
		$this->sheet->cellMerge($this->y, 4, 1, 0);
		for ($i = 1; $i < count($this->params); $i++)
		{
			if ($this->params[$i]['from'] != '')
			{
				$this->NewLine();
				$str = $this->params[$i]['text'] . ':';
				$this->sheet->writeString($this->y, 1, $str);
				$str = $this->params[$i]['from'];
				if ($this->params[$i]['to'] != '')
					$str .= " - " . $this->params[$i]['to'];
				$this->sheet->writeString($this->y, 2, $str);
				if ($i == 1)
				{
					$this->sheet->writeString($this->y, 4, $this->user);
					$this->sheet->cellMerge($this->y, 4, 1, 0);
				}	
			}
		}
		if ($this->params[0] != '') // Comments
		{
			$this->NewLine();
			$str = _("Comments") . ':';
			$this->sheet->writeString($this->y, 1, $str);
			$this->sheet->writeString($this->y, 2, $this->params[0]);
		}
		$this->NewLine();
		if ($this->headers2 != null)
		{
			$count = count($this->headers2);
			for ($i = 0; $i < $count; $i++)
			{
				if ($this->aligns2[$i] == "right")
					$this->sheet->writeString($this->y, $i + 1, $this->headers2[$i], $this->formatHeaderRight);
				else	
					$this->sheet->writeString($this->y, $i + 1, $this->headers2[$i], $this->formatHeaderLeft);
			}		
			$this->NewLine();
		}

		$count = count($this->headers);
		for ($i = 0; $i < $count; $i++)
		{
			if ($this->aligns[$i] == "right")
				$this->sheet->writeString($this->y, $i + 1, $this->headers[$i], $this->formatHeaderRight);
			else	
				$this->sheet->writeString($this->y, $i + 1, $this->headers[$i], $this->formatHeaderLeft);
		}		
		$this->NewLine();
	}

	function Header2($myrow, $branch, $sales_order, $bankaccount, $doctype)
	{
		return;
	}

	function AddImage($logo, $x, $y, $w, $h)
	{
		return;
	}

	function SetDrawColor($r, $g, $b)
	{
		return;
	}

	function SetTextColor($r, $g, $b)
	{
		return;
	}

	function Text($c, $txt, $n=0, $corr=0, $r=0)
	{
		return;
	}

	function TextWrap($xpos, $ypos, $len, $str, $align = 'left')
	{
		return;
	}

	function TextCol($c, $n, $txt, $corr=0, $r=0)
	{
		if ($this->aligns[$c] == 'right')
			$this->sheet->writeString($this->y, $c + 1, $txt, $this->formatRight);
		else	
			$this->sheet->writeString($this->y, $c + 1, $txt);
		if ($n - $c > 1)
			$this->sheet->cellMerge($this->y, $c + 1, $n - $c - 1, 0);
	}

	function AmountCol($c, $n, $txt, $dec=0, $corr=0, $r=0) 
	{ 
		if (!is_numeric($txt))
			$txt = 0;
		$this->sheet->writeNumber($this->y, $c + 1, $txt, $this->formatAmount[$dec]); 
	}
	
	function DateCol($c, $n, $txt, $conv=false, $corr=0, $r=0) 
	{
		if (!$conv)
			$txt = date2sql($txt);
		$date = $this->sheet->convertMysqlDate($txt);
		$this->sheet->writeDate($this->y, $c + 1, $date, $this->formatDate);
	}

	function TextCol2($c, $n, $txt, $corr=0, $r=0)
	{
		$this->sheet->writeString($this->y, $c + 1, $txt);
		if ($n - $c > 1)
			$this->sheet->cellMerge($this->y, $c + 1, $n - $c - 1, 0);
	}

	function TextColLines($c, $n, $txt, $corr=0, $r=0)
	{
		return;
	}

	function TextWrapLines($c, $width, $txt, $align='left')
	{
		return;
	}

	function LineTo($from, $row, $to, $row2)
	{
		return;
	}

	function Line($row, $height = 0)
	{
		return;
	}

	function NewLine($l=1, $np=0)
	{
		$this->y += $l;
	}

	function End($email=0, $subject=null, $myrow=null, $doctype = 0)
	{
		global $comp_path;
		$this->sheet->cellMerge($this->y, 1, count($this->headers) - 1, 0);
		$this->sheet->writeString($this->y, 1, "", $this->formatFooter);
		
		$dir =  $comp_path.'/'.user_company(). '/pdf_files';
		//save the file
		if (!file_exists($dir))
		{
			mkdir ($dir,0777);
		}
		// do not use standard filenames or your sensitive company data 
		// are world readable
		//$fname = $dir.'/'.uniqid('').'.xml';
		$fname = $dir.'/'."test".'.xml';
		$this->xml->writeData($fname);
		if(in_ajax()) 
		{
			global $Ajax;
			if (user_rep_popup()) 
				$Ajax->popup($fname); // when embeded pdf viewer used
			else
				$Ajax->redirect($fname); // otherwise use faster method
		} 
		else 
		{
			$this->xml->sendHeaders();
			$this->xml->writeData();
		}	
		// first have a look through the directory, 
		// and remove old temporary pdfs
		/*
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
		*/
	}
}

?>