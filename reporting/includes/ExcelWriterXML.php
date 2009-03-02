<?php
/**
 * ExcelWriterXML Package
 * Used the schema documentation from Microsoft
 * @link http://msdn.microsoft.com/en-us/library/aa140066(office.10).aspx
 * @package ExcelWriterXML
 */

/**
 * Includes the other class file to create Sheets
 */
include('ExcelWriterXML_Sheet.php');
/**
 * Includes the other class file to create Styles
 */
include('ExcelWriterXML_Style.php');

/**
 * Class for generating the initial Excel XML document
 * <code>
 * <?php
 * $xml = new ExcelWriterXML;
 * $format = $xml->addStyle('StyleHeader');
 * $format->fontBold();
 * $sheet = $xml->addSheet('Test Sheet');
 * $sheet->writeString (1,1,'Header1','StyleHeader');
 * $sheet->writeString(2,1,'My String');
 * $xml->sendHeaders();
 * $xml->writeData();
 * ?>
 * </code>
 * @link http://msdn.microsoft.com/en-us/library/aa140066(office.10).aspx
 * @author Robert F Greer
 * @version 1.0
 * @package ExcelWriterXML
 */
class ExcelWriterXML
{
	// Private Variables //
	var $styles = array();
	var $formatErrors = array();
	var $sheets = array();
	var $showErrorSheet = false;
	var $overwriteFile = false;
	var $docFileName;
	var $docTitle;
	var $docSubject;
	var $docAuthor;
	var $docCreated;
	var $docManager;
	var $docCompany;
	var $docVersion = 11.9999;
	var $charSet = "utf-8"; // 2009-02-28 Added by Joe Hunt, FA
	///////////////////////

	/**
     * Constructor for the ExcelWriterXML class.
     * A default style is created, a filename is generated (if not supplied) and
     * the create time of the document is stored.
     * @param string $fileName This is the filename that will be passed to the
     * browser.  If not present it will default to "file.xml"
     * @return ExcelWriterXML Instance of the class
     */
	function ExcelWriterXML($fileName = 'file.xml')
	{
		// Add default style
		$style = $this->addStyle('Default');
		$style->name('Normal');
		$style->alignVertical('Bottom');
		
		if ($fileName == '')
		{
			$fileName = 'file.xml';
			$this->_addError(__FUNCTION__,'File name was blank, default to "file.xml"');
		}
		
		$this->docFileName = $fileName;
		$this->docCreated = date('Y-m-d').'T'.date('H:i:s').'Z';
	}

	/**
	 * Whether or not to overwrite a file (when writing to disk)
	 * @param boolean $overwrite True or False
	 */
	function overwriteFile($overwrite = true)
	{
		if (!is_bool($overwrite))
		{
			$this->overwriteFile = false;
			return;
		}
		else
		{
			$this->overwriteFile = $overwrite;
		}
	}

	/**
	 * Whether or not to show the sheet containing the Formatting Errors
	 * @param boolean $show
	 */
	function showErrorSheet($show = true)
	{
		if (!is_bool($show))
		{
			$this->showErrorSheet = true;
			return;
		}
		else{
			$this->showErrorSheet = $show;
		}
	}

	/**
	 * Adds a format error.  When the document is generated if there are any
	 * errors they will be listed on a seperate sheet.
	 * @param string $function The name of the function that was called
	 * @param string $message Details of the error
	 */
	function _addError($function, $message)
	{
		$tmp = array(
			'function'	=> $function,
			'message'	=> $message,
		);
		$this->formatErrors[] = $tmp;
	}
	
	/**
     * Sends the HTML headers to the client.
     * This is only necessary if the XML doc is to be delivered from the server
     * to the browser.
     */
	function sendHeaders()
	{
		header('content-type: text/xml');
		header('Content-Disposition: attachment; filename="'.$this->docFileName.'"');
		header('Expires: 0');
		header('Cache-Control: must-revalidate, post-check=0,pre-check=0');
		header('Pragma: public');
	}

	/**
     * Gets the default style that was created by the contructor.
     * This is used when modifications to the default style are required.
     * @return ExcelWriterXML_Style Reference to a style class
     */
	function getDefaultStyle()
	{
		return($this->styles[0]);
	}

	/**
     * Creates a new style for the spreadsheet
     * @param string $id The name of the style to be referenced within the
     * spreadsheet
     * @return ExcelWriterXML_Style Reference to a new style class
     * @todo Parameter validation for styles, can't have duplicates
     */
	/*
	function addStyle($id)
	{
		$style =& new ExcelWriterXML_Style($id);
		$this->styles[] = $style;
		return ($style);
	}
	*/
	/**
     * Creates a new style within the spreadsheet.
     * Styles cannot have the same name as any other style. If a style has the
     * same name as another style then it will follow the default naming
     * convention as if $id was null
     * @param string $id The name of the style.  If left blank then the style
     * will default to "CustomStyle" + n (e.g. "CustomStyle1")
     * @return ExcelWriterXML_Style Reference to a new style class
     */
	function addStyle($id = null)
	{
		static $styleNum = 1;
		if (trim($id) == '') 
			$id = null;

		if ($id == null)
		{
			$id = 'CustomStyle'.$styleNum;
			$styleNum++;
			//$this->_addError(__FUNCTION__,'Style name was blank, renamed to "'.$id.'"');
		}

		while (!$this->_checkStyleID($id))
		{
			$old_id = $id;
			$id = 'CustomStyle'.$styleNum;
			$this->_addError(__FUNCTION__,'Style name was duplicate ("'.$old_id.'"), renamed to "'.$id.'"');
			$styleNum++;
		}
		
		$style =& new ExcelWriterXML_Style($id);
		$this->styles[] = $style;
		return ($style);
	}
	
	/**
     * Creates a new sheet within the spreadsheet
     * At least one sheet is required.
     * Additional sheets cannot have the same name as any other sheet.
     * If a sheet has the same name as another sheet then it will follow the
     * default naming convention as if $id was null
     * @param string $id The name of the sheet.  If left blank then the sheet
     * will default to "Sheet" + n (e.g. "Sheet1")
     * @return ExcelWriterXML_Sheet Reference to a new sheet class
     */
	function addSheet($id = null)
	{
		static $sheetNum = 1;
		if (trim($id) == '') 
			$id = null;

		if ($id == null)
		{
			$id = 'Sheet'.$sheetNum;
			$sheetNum++;
			$this->_addError(__FUNCTION__,'Sheet name was blank, renamed to "'.$id.'"');
		}

		while (!$this->_checkSheetID($id))
		{
			$old_id = $id;
			$id = 'Sheet'.$sheetNum;
			$this->_addError(__FUNCTION__,'Sheet name was duplicate ("'.$old_id.'"), renamed to "'.$id.'"');
			$sheetNum++;
		}
		
		$sheet =& new ExcelWriterXML_Sheet($id);
		$this->sheets[] = $sheet;
		return ($sheet);
	}
	
	/**
	 * Checks whether a proposed Sheet ID has already been used
	 * @param string $id The sheet id to be checked
	 * @return boolean True if the id is unique, false otherwise
	 */
	function _checkSheetID($id){
		foreach($this->sheets as $sheet)
		{
			$sheetID = $sheet->getID();
			if ($id == $sheetID)
			{
				return false;
			}
		}
		return true;
	}

	/**
	 * Checks whether a proposed Style ID has already been used
	 * @param string $id The style id to be checked
	 * @return boolean True if the id is unique, false otherwise
	 */
	function _checkStyleID($id)
	{
		foreach($this->styles as $style)
		{
			$styleID = $style->getID();
			if ($id == $styleID)
			{
				return false;
			}
		}
		return true;
	}

	// 2009-02-28 Added by Joe Hunt, FA
    function setCharSet($charset)
    {
		$this->charSet = $charset;
	}	
	/**
     * Writes the XML data
     * @param string $target If left null the function will output to STD OUT
     * (e. g. browser or console)
     */
	function writeData($target = null)
	{
		$docTitle = '';
		$docSubject = '';
		$docAuthor = '';
		$docCreated = '';
		$docManager = '';
		$docCompany = '';
		$docVersion = 12;
		
		$errors = false;
		
		if ($this->showErrorSheet == true)
		{
			$format = $this->addStyle('formatErrorsHeader');
			$format->fontBold();
			$format->bgColor('red');
		}
		
		if (!empty($this->docTitle)) 
			$docTitle = '<Title>'.htmlspecialchars($this->docTitle).'</Title>'."\r";
		if (!empty($this->docSubject)) 
			$docSubject = '<Subject>'.htmlspecialchars($this->docSubject).'</Subject>'."\r";
		if (!empty($this->docAuthor)) 
			$docAuthor = '<Author>'.htmlspecialchars($this->docAuthor).'</Author>'."\r";
		if (!empty($this->docCreated)) 
			$docCreated = '<Created>'.htmlspecialchars($this->docCreated).'</Created>'."\r";
		if (!empty($this->docManager)) 
			$docManager = '<Manager>'.htmlspecialchars($this->docManager).'</Manager>'."\r";
		if (!empty($this->docCompany)) 
			$docCompany = '<Company>'.htmlspecialchars($this->docCompany).'</Company>'."\r";
			//$docCompany = '<Company>'.$this->docCompany.'</Company>'."\r";
		
		$xml = '<?xml version="1.0" encoding="'.$this->charSet.'"?>'."\r";  // 2009-02-28 Added by Joe Hunt, FA
		$xml .= '<?mso-application progid="Excel.Sheet"?>'."\r";
		$xml .= '<Workbook
			xmlns="urn:schemas-microsoft-com:office:spreadsheet"
			xmlns:o="urn:schemas-microsoft-com:office:office"
			xmlns:x="urn:schemas-microsoft-com:office:excel"
			xmlns:ss="urn:schemas-microsoft-com:office:spreadsheet"
			xmlns:html="http://www.w3.org/TR/REC-html40">'."\r";
		$xml .= '<DocumentProperties xmlns="urn:schemas-microsoft-com:office:office">'."\r";
		if (!empty($this->docTitle))	
			$xml .= '	'.$docTitle;
		if (!empty($this->docSubject))	
			$xml .= '	'.$docSubject;
		if (!empty($this->docAuthor))	
			$xml .= '	'.$docAuthor;
		if (!empty($this->docCreated))	
			$xml .= '	'.$docCreated;
		if (!empty($this->docManager))	
			$xml .= '	'.$docManager;
		if (!empty($this->docCompany))	
			$xml .= '	'.$docCompany;
		$xml .= '	<Version>'.$this->docVersion.'</Version>'."\r";
		$xml .= '</DocumentProperties>'."\r";
		$xml .= '<ExcelWorkbook xmlns="urn:schemas-microsoft-com:office:excel" />'."\r";
		$xml .= '<Styles>'."\r";
		foreach($this->styles as $style)
		{
			$xml .= $style->getStyleXML();
			if (count($style->getErrors()) > 0)
			{
				$errors = true;
			}
		}
		$xml .= '</Styles>'."\r";
		if (count($this->sheets) == 0)
		{
			$this->addSheet();
		}
		foreach($this->sheets as $sheet)
		{
			$xml .= $sheet->getSheetXML();
			if (count($sheet->getErrors()) > 0)
			{
				$errors = true;
			}
		}
		if (count($this->formatErrors) > 0)
		{
			$errors = true;
		}
		
		if ($errors == true && $this->showErrorSheet == true)
		{
			$sheet = $this->addSheet('formatErrors');
			$sheet->cellMerge(1,1,3,0);	// Merge the first three cells across in row 1
			$sheet->writeString(1,1,'Formatting Errors');
			$sheet->writeString(2,1,'Type','formatErrorsHeader');
			$sheet->writeString(2,2,'Function','formatErrorsHeader');
			$sheet->cellWidth(2,1,200);
			$sheet->cellWidth(2,2,200);
			$sheet->cellWidth(2,3,400);
			$sheet->writeString(2,3,'Error Message','formatErrorsHeader');
			$row = 3;
			foreach($this->formatErrors as $error)
			{
				$function = $error['function'];
				$message = $error['message'];
				$sheet->writeString($row,1,'Document');
				$sheet->writeString($row,2,$function);
				$sheet->writeString($row,3,$message);
				$row++;
			}
			foreach($this->styles as $styleObject)
			{
				$formatErrors = $styleObject->getErrors();
				$styleID = 'Style='.$styleObject->getID();
				foreach($formatErrors as $error)
				{
					$function = $error['function'];
					$message = $error['message'];
					$sheet->writeString($row,1,$styleID);
					$sheet->writeString($row,2,$function);
					$sheet->writeString($row,3,$message);
					$row++;
				}
			}		
			foreach($this->sheets as $sheetObject)
			{
				$formatErrors = $sheetObject->getErrors();
				$sheetID = 'Sheet='.$sheetObject->getID();
				foreach($formatErrors as $error)
				{
					$function = $error['function'];
					$message = $error['message'];
					$sheet->writeString($row,1,$sheetID);
					$sheet->writeString($row,2,$function);
					$sheet->writeString($row,3,$message);
					$row++;
				}
			}
			$xml .= $sheet->getSheetXML();
		}
		
		
		$xml .= '</Workbook>';
		
		if ($target == null)
		{
			// We aren't writing this file to disk, so echo back to the client.
			echo $xml;
			return true;
		}
		else
		{
			$fileExists = file_exists($target);
			if ($fileExists == true && $this->overwriteFile == false)
			{
				die('"'.$target.'" exists and "overwriteFile" is set to "false"');
			}
			$handle = fopen($target, 'w');
			if ($handle)
			{
				fwrite($handle,$xml);
				fclose($handle);
				return true;
			}
			else
			{
				echo('<br/>Not able to open "'.$target.'" for writing');
				return false;
			}
		}
	}
	
	/**
     * Sets the Title of the document
     * @param string $title Part of the properties of the document.
     */
	function docTitle($title = '') { $this->docTitle = $title; }

	/**
     * Sets the Subject of the document
     * @param string $subject Part of the properties of the document.
     */
	function docSubject($subject = '') { $this->docSubject = $subject; }

	/**
     * Sets the Author of the document
     * @param string $author Part of the properties of the document.
     */
	function docAuthor($author = '') { $this->docAuthor = $author; }

	/**
     * Sets the Manager of the document
     * @param string $manager Part of the properties of the document.
     */
	function docManager($manager = '') { $this->docManager = $manager; }

	/**
     * Sets the Company of the document
     * @param string $company Part of the properties of the document.
     */
	function docCompany($company = '') { $this->docCompany = $company; }
}


?>