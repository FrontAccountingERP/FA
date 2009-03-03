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

class FrontReport extends ExcelWriterXML
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
	var $rtl;
	var $code;
	var $bottomMargin = 0;
	var $lineHeight;
	var $leftMargin = 0;

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
	var $numcols;
	
	var $sheet;

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
		$this->ExcelWriterXML($this->filename);
		$this->setCharSet($this->code);
		$this->overwriteFile();
		$this->showErrorSheet(true);
		
		$this->sheet =& $this->addSheet($this->title);
		if ($this->rtl === 'rtl')
			$this->sheet->displayRightToLeft();
		$formatTitle =& $this->addStyle('formatTitle');	
		$formatTitle->fontSize('16');
		$formatTitle->fontBold();
		$formatTitle->border('Top', '2', 'darkgray');
	
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
		$formatDateTime =& $this->addStyle('formatDateTime');
		$formatDateTime->numberFormatDateTime($dateformat_long);
		$formatDateTime->alignHorizontal('Left');
		$formatDate =& $this->addStyle('formatDate');
		$formatDate->numberFormatDateTime($dateformat);
		$formatDate->alignHorizontal('Left');
		$formatRight =& $this->addStyle('formatRight');
		$formatRight->alignHorizontal('Right');
		
		$formatHeaderLeft =& $this->addStyle("formatHeaderLeft");
		$formatHeaderLeft->fontItalic();
		$formatHeaderLeft->border('Top', '2', 'darkgray');
		$formatHeaderLeft->border('Bottom', '2', 'darkgray');
		$formatHeaderLeft->alignVertical('Center');
		$formatHeaderRight =& $this->addStyle("formatHeaderRight");
		$formatHeaderRight->fontItalic();
		$formatHeaderRight->alignHorizontal('Right');
		$formatHeaderRight->border('Top', '2', 'darkgray');
		$formatHeaderRight->border('Bottom', '2', 'darkgray');
		$formatHeaderRight->alignVertical('Center');
		$formatFooter =& $this->addStyle("formatFooter");
		$formatFooter->border('Top', '2', 'darkgray');
	}
	
	function NumFormat($dec) 
	{
		global $thoseps,$decseps;
		
		$dec = (int)$dec;
		$stylename = 'formatAmount'.$dec;
		if (!isset($this->styles[$stylename])) 
		{
			$tsep = $thoseps[user_tho_sep()];
			$dsep = $decseps[user_dec_sep()];
			$format = "###{$tsep}###{$tsep}###{$tsep}##0";
			if ($dec>0)
				$format .= "{$dsep}".str_repeat('0',$dec);
			$style =& $this->addStyle($stylename);
			$style->numberFormat($format);
			$style->alignHorizontal('Right');
		}
		return $stylename;
	}

	function Font($style = 'normal')
	{
	}

	function Info($params, $cols, $headers, $aligns,
		$cols2 = null, $headers2 = null, $aligns2 = null)
	{
		global $app_title, $version, $power_by, $power_url;
		$this->company = get_company_prefs();
		$this->docTitle($this->title);
		$this->docAuthor($app_title . ' ' . $version);
		$this->docCompany($this->company['coy_name']);
		$this->docManager($power_by . ' - ' . $power_url);
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
		$this->numcols = count($this->headers);
		$tcols = count($this->headers2);
		if ($tcols > $this->numcols)
			$this->numcols = $tcols;
		for ($i = 0; $i < $this->numcols; $i++)
			$this->sheet->columnWidth($i+1, $this->cols[$i + 1] - $this->cols[$i]);
	}

	function Header()
	{
		$this->y = 0;
		$tcol = $this->numcols - 1;
		$this->NewLine();
		$this->sheet->rowHeight($this->y, 20);
		$this->sheet->writeString($this->y, 1, $this->title, 'formatTitle');
		$this->sheet->cellMerge($this->y, 1, $tcol, 0);

		$this->NewLine();

		$str = _("Print Out Date") . ':';
		$this->sheet->writeString($this->y, 1, $str);
		$date = $this->sheet->convertMysqlDateTime(date('Y-m-d H:i:s'));
		$this->sheet->writeDateTime($this->y, 2, $date, 'formatDateTime');
		$this->sheet->writeString($this->y, $tcol, $this->company['coy_name']);
		$this->sheet->cellMerge($this->y, $tcol, 1, 0);
		$this->NewLine();
		$str = _("Fiscal Year") . ':';
		$this->sheet->writeString($this->y, 1, $str);
		$str = $this->fiscal_year;
		$this->sheet->writeString($this->y, 2, $str);
		$this->sheet->writeString($this->y, $tcol, $this->host);
		$this->sheet->cellMerge($this->y, $tcol, 1, 0);
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
					$this->sheet->writeString($this->y, $tcol, $this->user);
					$this->sheet->cellMerge($this->y, $tcol, 1, 0);
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
			for ($i = 0, $j = 0; $i < $this->numcols; $i++)
			{
				if ($this->cols2[$j] >= $this->cols[$i] && $this->cols2[$j] <= $this->cols[$i + 1])
				{
					if ($this->aligns2[$j] == "right")
						$this->sheet->writeString($this->y, $i + 1, $this->headers2[$j], 'formatHeaderRight');
					else	
						$this->sheet->writeString($this->y, $i + 1, $this->headers2[$j], 'formatHeaderLeft');
					$j++;	
				}
				else
					$this->sheet->writeString($this->y, $i + 1, "", 'formatHeaderLeft');
			}		
			$this->NewLine();
		}

		for ($i = 0; $i < $this->numcols; $i++)
		{
			if (!isset($this->headers[$i]))
				$header = "";
			else
				$header = $this->headers[$i];
			if ($this->aligns[$i] == "right")
				$this->sheet->writeString($this->y, $i + 1, $header, 'formatHeaderRight');
			else	
				$this->sheet->writeString($this->y, $i + 1, $header, 'formatHeaderLeft');
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
			$this->sheet->writeString($this->y, $c + 1, $txt, 'formatRight');
		else	
			$this->sheet->writeString($this->y, $c + 1, $txt);
		if ($n - $c > 1)
			$this->sheet->cellMerge($this->y, $c + 1, $n - $c - 1, 0);
	}

	function AmountCol($c, $n, $txt, $dec=0, $corr=0, $r=0) 
	{ 
		if (!is_numeric($txt))
			$txt = 0;
		$this->sheet->writeNumber($this->y, $c + 1, $txt, $this->NumFormat($dec)); 
	}
	
	function DateCol($c, $n, $txt, $conv=false, $corr=0, $r=0) 
	{
		if (!$conv)
			$txt = date2sql($txt);
		$date = $this->sheet->convertMysqlDate($txt);
		$this->sheet->writeDateTime($this->y, $c + 1, $date, 'formatDate');
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
		$this->sheet->cellMerge($this->y, 1, $this->numcols - 1, 0);
		$this->sheet->writeString($this->y, 1, "", 'formatFooter');
		
		$dir =  $comp_path.'/'.user_company(). '/pdf_files';
		//save the file
		if (!file_exists($dir))
		{
			mkdir ($dir,0777);
		}
		if(in_ajax()) 
		{
			global $Ajax;
			// do not use standard filenames or your sensitive company data 
			// are world readable
			//$fname = $dir.'/'.uniqid('').'.xml';
			$fname = $dir.'/'."test".'.xml';
			$this->writeData($fname);
			if (user_rep_popup()) 
				$Ajax->popup($fname); // when embeded pdf viewer used
			else
				$Ajax->redirect($fname); // otherwise use faster method
		} 
		else 
		{
			$this->sendHeaders();
			$this->writeData();
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