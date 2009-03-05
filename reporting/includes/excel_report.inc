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
include_once($path_to_root . "/reporting/includes/Workbook.php");
include_once($path_to_root . "/admin/db/company_db.inc");
include_once($path_to_root . "/config.php");

class FrontReport extends Workbook
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
		global $comp_path, $dateseps;
		
		$this->size = $size;
		$this->title = $title;
		$this->lineHeight = 12;
		$this->fontSize = $fontsize;
		$this->oldFontSize = 0;
		$this->y = 1;
		$this->currency = '';
		$this->rtl = ($_SESSION['language']->dir === 'rtl' ? 'rtl' : 'ltr');
		$this->code = $_SESSION['language']->encoding;
		$this->filename = $filename.".xls";
		$this->unique_name = uniqid('').".xls";
		$this->path = $comp_path.'/'.user_company(). '/pdf_files';
		$this->Workbook($this->path."/".$this->unique_name);
		
		$this->sheet =& $this->add_worksheet($this->title);
		if ($this->rtl === 'rtl')
			$this->sheet->set_rtl();
		$this->formatTitle =& $this->add_format();	
		$this->formatTitle->set_size(16);
		$this->formatTitle->set_bold();
		$this->formatTitle->set_top(2);
		$this->formatTitle->set_top_color('gray');
	
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
		$this->formatDateTime =& $this->add_format();
		$this->formatDateTime->set_num_format($dateformat_long);
		$this->formatDateTime->set_align('left');
		$this->formatDate =& $this->add_format();
		$this->formatDate->set_num_format($dateformat);
		$this->formatDate->set_align('left');
		$this->formatRight =& $this->add_format();
		$this->formatRight->set_align('right');
		
		$this->formatHeaderLeft =& $this->add_format();
		$this->formatHeaderLeft->set_italic();
		$this->formatHeaderLeft->set_top(2);
		$this->formatHeaderLeft->set_top_color('gray');
		$this->formatHeaderLeft->set_bottom(2);
		$this->formatHeaderLeft->set_bottom_color('gray');
		$this->formatHeaderLeft->set_align('vcenter');
		$this->formatHeaderLeft->set_align('left');
		$this->formatHeaderRight =& $this->add_format();
		$this->formatHeaderRight->set_italic();
		$this->formatHeaderRight->set_top(2);
		$this->formatHeaderRight->set_top_color('gray');
		$this->formatHeaderRight->set_bottom(2);
		$this->formatHeaderRight->set_bottom_color('gray');
		$this->formatHeaderRight->set_align('vcenter');
		$this->formatHeaderRight->set_align('right');
		$this->formatFooter =& $this->add_format();
		$this->formatFooter->set_top(2);
		$this->formatFooter->set_top_color('gray');
	}
	
	function NumFormat($dec) 
	{
		if (!isset($this->formatAmount[$dec]))
		{
			global $thoseps,$decseps;
			$dec = (int)$dec;
			$tsep = $thoseps[user_tho_sep()];
			$dsep = $decseps[user_dec_sep()];
			$format = "###{$tsep}###{$tsep}###{$tsep}##0";
			if ($dec>0)
				$format .= "{$dsep}".str_repeat('0',$dec);
			$this->formatAmount[$dec] =& $this->add_format();
			$this->formatAmount[$dec]->set_num_format($format);
			$this->formatAmount[$dec]->set_align('right');
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
		//$this->docTitle($this->title);
		//$this->docAuthor($app_title . ' ' . $version);
		//$this->docCompany($this->company['coy_name']);
		//$this->docManager($power_by . ' - ' . $power_url);
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
			$this->sheet->set_column($i, $i, $this->px2units($this->cols[$i + 1] - $this->cols[$i]));
	}

	function Header()
	{
		$this->y = 0;
		$tcol = $this->numcols - 1;
		$this->sheet->set_row($this->y, 20);
		for ($i = 0; $i < $this->numcols; $i++)
			$this->sheet->write_blank($this->y, $i, $this->formatTitle);
		$this->sheet->write_string($this->y, 0, $this->title, $this->formatTitle);
		$this->sheet->merge_cells($this->y, 0, $this->y, $tcol);
		$this->NewLine();

		$str = _("Print Out Date") . ':';
		$this->sheet->write_string($this->y, 0, $str);
		$this->sheet->write_string($this->y, 1, Today() . "  ".Now());
		$this->sheet->write_string($this->y, $tcol-1, $this->company['coy_name']);
		$this->sheet->merge_cells($this->y, $tcol-1, $this->y, $tcol);
		$this->NewLine();
		$str = _("Fiscal Year") . ':';
		$this->sheet->write_string($this->y, 0, $str);
		$str = $this->fiscal_year;
		$this->sheet->write_string($this->y, 1, $str);
		$this->sheet->write_string($this->y, $tcol-1, $this->host);
		$this->sheet->merge_cells($this->y, $tcol-1, $this->y, $tcol);
		for ($i = 1; $i < count($this->params); $i++)
		{
			if ($this->params[$i]['from'] != '')
			{
				$this->NewLine();
				$str = $this->params[$i]['text'] . ':';
				$this->sheet->write_string($this->y, 0, $str);
				$str = $this->params[$i]['from'];
				if ($this->params[$i]['to'] != '')
					$str .= " - " . $this->params[$i]['to'];
				$this->sheet->write_string($this->y, 1, $str);
				if ($i == 1)
				{
					$this->sheet->write_string($this->y, $tcol-1, $this->user);
					$this->sheet->merge_cells($this->y, $tcol-1, $this->y, $tcol);
				}	
			}
		}
		if ($this->params[0] != '') // Comments
		{
			$this->NewLine();
			$str = _("Comments") . ':';
			$this->sheet->write_string($this->y, 0, $str);
			$this->sheet->write_string($this->y, 1, $this->params[0]);
		}
		$this->NewLine();
		if ($this->headers2 != null)
		{
			for ($i = 0, $j = 0; $i < $this->numcols; $i++)
			{
				if ($this->cols2[$j] >= $this->cols[$i] && $this->cols2[$j] <= $this->cols[$i + 1])
				{
					if ($this->aligns2[$j] == "right")
						$this->sheet->write_string($this->y, $i, $this->headers2[$j], $this->formatHeaderRight);
					else	
						$this->sheet->write_string($this->y, $i, $this->headers2[$j], $this->formatHeaderLeft);
					$j++;	
				}
				else
					$this->sheet->write_string($this->y, $i, "", $this->formatHeaderLeft);
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
				$this->sheet->write_string($this->y, $i, $header, $this->formatHeaderRight);
			else	
				$this->sheet->write_string($this->y, $i, $header, $this->formatHeaderLeft);
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
			$this->sheet->write_string($this->y, $c, $txt, $this->formatRight);
		else	
			$this->sheet->write_string($this->y, $c, $txt);
		if ($n - $c > 1)
			$this->sheet->merge_cells($this->y, $c, $this->y, $n - 1);
	}

	function AmountCol($c, $n, $txt, $dec=0, $corr=0, $r=0) 
	{ 
		if (!is_numeric($txt))
			$txt = 0;
		$this->sheet->write_number($this->y, $c, $txt, $this->NumFormat($dec)); 
	}
	
	function DateCol($c, $n, $txt, $conv=false, $corr=0, $r=0) 
	{
		if (!$conv)
			$txt = date2sql($txt);
		list($year, $mo, $day) = explode("-", $txt);	
		$date = $this->ymd2date((int)$year, (int)$mo, (int)$day);
		//$date = $this->ymd2date(2009, 3, 2);
		$this->sheet->write_number($this->y, $c, $date, $this->formatDate);
		//$this->sheet->write_string($this->y, $c, $txt);
	}

	function TextCol2($c, $n, $txt, $corr=0, $r=0)
	{
		$this->sheet->write_string($this->y, $c, $txt);
		if ($n - $c > 1)
			$this->sheet->merge_cells($this->y, $c, $this->y, $n - 1);
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
  		/*	
  		$unit_offset_map = array(0, 36, 73, 109, 146, 182, 219);
  		$width_units = $excel_column_width_factor * ($px / $unit_offset_length);
  		$width_units += $unit_offset_map[($px % $unit_offset_length)];
  		return $width_units;
  	  	$pixels = (float)((float)$px / (float)$excel_column_width_factor) * $unit_offset_length;
    	$ofs = $px % $excel_column_width_factor;
    	$pixels += round((float)$ofs / ((float) $excel_column_width_factor / $unit_offset_length));
    	return (int)$pixels;
  		*/
  		return ($px / $unit_offset_length);
  	}	

	function End($email=0, $subject=null, $myrow=null, $doctype = 0)
	{
		for ($i = 0; $i < $this->numcols; $i++)
			$this->sheet->write_blank($this->y, $i, $this->formatFooter);
		$this->sheet->merge_cells($this->y, 0, $this->y, $this->numcols - 1);
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