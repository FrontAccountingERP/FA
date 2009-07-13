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
include_once($path_to_root . "/reporting/includes/Workbook.php");
include_once($path_to_root . "/admin/db/company_db.inc");
include_once($path_to_root . "/config.php");
// xls version
class FrontReport extends Spreadsheet_Excel_Writer_Workbook
{
	var $size;
	var $company;
	var $user;
	var $host;
	var $fiscal_year;
	var $title;
	var $filename;
	var $unique_name;
	var $path;
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
	
	var $formatTitle;
	var $formatDateTime;
	var $formatDate;
	var $formatHeaderLeft;
	var $formatHeaderRight;
	var $formatFooter;
	var $formatAmount = array();
	
	var $sheet;

	function FrontReport($title, $filename, $size = 'A4', $fontsize = 9)
	{
		global $comp_path, $dateseps, $page_security;
		if (!$_SESSION["wa_current_user"]->can_access_page($page_security))
		{
			display_error(_("The security settings on your account do not permit you to print this report"));
			end_page();
			exit;
		}
		$this->size = $size;
		$this->title = $title;
		$this->lineHeight = 12;
		$this->fontSize = $fontsize;
		$this->oldFontSize = 0;
		$this->y = 1;
		$this->currency = '';
		$rtl = ($_SESSION['language']->dir == 'rtl');
		$this->code = strtolower($_SESSION['language']->encoding);
		$this->filename = $filename.".xls";
		$this->unique_name = uniqid('').".xls";
		$this->path = $comp_path.'/'.user_company(). '/pdf_files';
		$this->Spreadsheet_Excel_Writer_Workbook($this->path."/".$this->unique_name);
		//$this->setCountry(48);
		if ($this->code != "iso-8859-1")
			$this->setVersion(8); // set biff version to 8 (0x0006 internal)
		$this->sheet =& $this->addWorksheet($this->title);
		if ($this->code != "iso-8859-1")
			$this->sheet->setInputEncoding($this->code); // set sheet encoding
		if ($rtl)
			$this->sheet->setRTL();
		$this->formatTitle =& $this->addFormat();	
		$this->formatTitle->setSize(16);
		$this->formatTitle->setBold();
		$this->formatTitle->setAlign($rtl ? 'right' : 'left');
		$this->formatTitle->setTop(2);
		$this->formatTitle->setTopColor('gray');

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
		$this->formatDateTime =& $this->addFormat();
		$this->formatDateTime->setNumFormat($dateformat_long);
		$this->formatDateTime->setAlign($rtl ? 'right' : 'left');
		$this->formatDate =& $this->addFormat();
		$this->formatDate->setNumFormat($dateformat);
		$this->formatDate->setAlign($rtl ? 'right' : 'left');
		$this->formatRight =& $this->addFormat();
		$this->formatRight->setAlign($rtl ? 'left' : 'right');
		$this->formatLeft =& $this->addFormat();
		$this->formatLeft->setAlign($rtl ? 'right' : 'left');
		
		$this->formatHeaderLeft =& $this->addFormat();
		$this->formatHeaderLeft->setItalic();
		$this->formatHeaderLeft->setTop(2);
		$this->formatHeaderLeft->setTopColor('gray');
		$this->formatHeaderLeft->setBottom(2);
		$this->formatHeaderLeft->setBottomColor('gray');
		$this->formatHeaderLeft->setAlign('vcenter');
		$this->formatDate->setAlign($rtl ? 'right' : 'left');
		$this->formatHeaderRight =& $this->addFormat();
		$this->formatHeaderRight->setItalic();
		$this->formatHeaderRight->setTop(2);
		$this->formatHeaderRight->setTopColor('gray');
		$this->formatHeaderRight->setBottom(2);
		$this->formatHeaderRight->setBottomColor('gray');
		$this->formatHeaderRight->setAlign('vcenter');
		$this->formatHeaderRight->setAlign('right');
		$this->formatFooter =& $this->addFormat();
		$this->formatFooter->setTop(2);
		$this->formatFooter->setTopColor('gray');
	}
	
	function NumFormat($dec) 
	{
		if (!isset($this->formatAmount[$dec]))
		{
			//global $thoseps,$decseps;
			$dec = (int)$dec;
			//$tsep = $thoseps[user_tho_sep()];
			//$dsep = $decseps[user_dec_sep()];
			$tsep = ',';
			$dsep = '.';
			$format = "###{$tsep}###{$tsep}###{$tsep}##0";
			if ($dec>0)
				$format .= "{$dsep}".str_repeat('0',$dec);
			$this->formatAmount[$dec] =& $this->addFormat();
			$this->formatAmount[$dec]->setNumFormat($format);
			$this->formatAmount[$dec]->setAlign('right');
		}
		return $this->formatAmount[$dec];
	}

	function Font($style = 'normal')
	{
	}

	function Info($params, $cols, $headers, $aligns,
		$cols2 = null, $headers2 = null, $aligns2 = null)
	{
		global $app_title, $version, $power_by, $power_url;
		$this->company = get_company_prefs();
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
			$this->sheet->setColumn($i, $i, $this->px2units($this->cols[$i + 1] - $this->cols[$i]));
	}

	function Header()
	{
		$this->y = 0;
		$tcol = $this->numcols - 1;
		$this->sheet->setRow($this->y, 20);
		for ($i = 0; $i < $this->numcols; $i++)
			$this->sheet->writeBlank($this->y, $i, $this->formatTitle);
		$this->sheet->writeString($this->y, 0, $this->title, $this->formatTitle);
		$this->sheet->mergeCells($this->y, 0, $this->y, $tcol);
		$this->NewLine();
		$str = _("Print Out Date") . ':';
		$this->sheet->writeString($this->y, 0, $str, $this->formatLeft);
		$this->sheet->writeString($this->y, 1, Today() . "  ".Now(), $this->formatLeft);
		$this->sheet->writeString($this->y, $tcol-1, $this->company['coy_name'], $this->formatLeft);
		$this->sheet->mergeCells($this->y, $tcol-1, $this->y, $tcol);
		$this->NewLine();
		$str = _("Fiscal Year") . ':';
		$this->sheet->writeString($this->y, 0, $str, $this->formatLeft);
		$str = $this->fiscal_year;
		$this->sheet->writeString($this->y, 1, $str, $this->formatLeft);
		$this->sheet->writeString($this->y, $tcol-1, $this->host, $this->formatLeft);
		$this->sheet->mergeCells($this->y, $tcol-1, $this->y, $tcol);
		for ($i = 1; $i < count($this->params); $i++)
		{
			if ($this->params[$i]['from'] != '')
			{
				$this->NewLine();
				$str = $this->params[$i]['text'] . ':';
				$this->sheet->writeString($this->y, 0, $str);
				$str = $this->params[$i]['from'];
				if ($this->params[$i]['to'] != '')
					$str .= " - " . $this->params[$i]['to'];
				$this->sheet->writeString($this->y, 1, $str, $this->formatLeft);
				if ($i == 1)
				{
					$this->sheet->writeString($this->y, $tcol-1, $this->user, $this->formatLeft);
					$this->sheet->mergeCells($this->y, $tcol-1, $this->y, $tcol);
				}	
			}
		}
		if ($this->params[0] != '') // Comments
		{
			$this->NewLine();
			$str = _("Comments") . ':';
			$this->sheet->writeString($this->y, 0, $str, $this->formatLeft);
			$this->sheet->writeString($this->y, 1, $this->params[0], $this->formatLeft);
		}
		$this->NewLine();
		if ($this->headers2 != null)
		{
			for ($i = 0, $j = 0; $i < $this->numcols; $i++)
			{
				if ($this->cols2[$j] >= $this->cols[$i] && $this->cols2[$j] <= $this->cols[$i + 1])
				{
					if ($this->aligns2[$j] == "right")
						$this->sheet->writeString($this->y, $i, $this->headers2[$j], $this->formatHeaderRight);
					else	
						$this->sheet->writeString($this->y, $i, $this->headers2[$j], $this->formatHeaderLeft);
					$j++;	
				}
				else
					$this->sheet->writeString($this->y, $i, "", $this->formatHeaderLeft);
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
				$this->sheet->writeString($this->y, $i, $header, $this->formatHeaderRight);
			else	
				$this->sheet->writeString($this->y, $i, $header, $this->formatHeaderLeft);
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
			$this->sheet->writeString($this->y, $c, $txt, $this->formatRight);
		else	
			$this->sheet->writeString($this->y, $c, $txt, $this->formatLeft);
		if ($n - $c > 1)
			$this->sheet->mergeCells($this->y, $c, $this->y, $n - 1);
	}

	function AmountCol($c, $n, $txt, $dec=0, $corr=0, $r=0) 
	{ 
		if (!is_numeric($txt))
			$txt = 0;
		$this->sheet->writeNumber($this->y, $c, $txt, $this->NumFormat($dec)); 
	}
	
	function DateCol($c, $n, $txt, $conv=false, $corr=0, $r=0) 
	{
		if (!$conv)
			$txt = date2sql($txt);
		list($year, $mo, $day) = explode("-", $txt);	
		$date = $this->ymd2date((int)$year, (int)$mo, (int)$day);
		$this->sheet->writeNumber($this->y, $c, $date, $this->formatDate);
	}

	function TextCol2($c, $n, $txt, $corr=0, $r=0)
	{
		$this->sheet->writeString($this->y, $c, $txt, $this->formatLeft);
		if ($n - $c > 1)
			$this->sheet->mergeCells($this->y, $c, $this->y, $n - 1);
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

  	function ymd2Date($year, $mon, $day) // XLS internal date representation is a number between 1900-01-01 and 2078-12-31
  	{										// if we need the time part too, we have to add this value after a decimalpoint.
      	$mo = array(0,31,28,31,30,31,30,31,31,30,31,30,31);
      	$BASE = 1900;
  	  	$MAXYEAR = 2075;
  	  	if (($year % 4) == 0)
      		$mo[2]++;
      	if ($mon < 1)
      	    $mon = 1;
      	elseif ($mon > 12)
      	    $mon = 12;
      	if ($day < 1)
      	    $day = 1;
      	elseif ($day > $mo[$mon])
      	    $day = $mo[$mon];
      	if ($year < $BASE)
      	    $year = $BASE;
      	elseif ($year > $MAXYEAR)
      	    $year = $MAXYEAR;
      	$jul = (int)$day;
      	for ($n = 1; $n < $mon; $n++)
      	{
      	    $jul += $mo[$n];
      	}
      	for ($n = $BASE; $n < $year; $n++)
      	{
      	    $jul += 365;
      	    if (($n % 4) == 0)
      	    	$jul++;
      	}
      	return $jul;
  	}
  
  	function px2units($px) // XLS app conversion. Not bulletproof.
  	{
  		$excel_column_width_factor = 256;
  		$unit_offset_length = 6.5;
  		return ($px / $unit_offset_length);
  	}	

	function End($email=0, $subject=null, $myrow=null, $doctype = 0)
	{
		for ($i = 0; $i < $this->numcols; $i++)
			$this->sheet->writeBlank($this->y, $i, $this->formatFooter);
		$this->sheet->mergeCells($this->y, 0, $this->y, $this->numcols - 1);
		$this->close();
		// first have a look through the directory, 
		// and remove old temporary pdfs
		if ($d = @opendir($this->path)) {
			while (($file = readdir($d)) !== false) {
				if (!is_file($this->path.'/'.$file) || $file == 'index.php') continue;
				// then check to see if this one is too old
				$ftime = filemtime($this->path.'/'.$file);
			 	// seems 3 min is enough for any report download, isn't it?
				if (time()-$ftime > 180){
					unlink($this->path.'/'.$file);
				}
			}
			closedir($d);
		}
		meta_forward($_SERVER['PHP_SELF'], "xls=1&filename=$this->filename&unique=$this->unique_name");
		exit();
	}
}

?>