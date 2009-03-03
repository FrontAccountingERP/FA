<?php
/**
 * File contains the class files for ExcelWriterXML_Sheet
 * @package ExcelWriterXML
 */

/**
 * Class for generating sheets within the Excel document
 * @link http://msdn.microsoft.com/en-us/library/aa140066(office.10).aspx
 * @author Robert F Greer
 * @version 1.0
 * @package ExcelWriterXML
 * @uses ExcelWriterXML_Style::alignHorizontal()
 * @uses ExcelWriterXML_Style::alignRotate()
 * @uses ExcelWriterXML_Style::alignShrinktofit()
 * @uses ExcelWriterXML_Style::alignVertical()
 * @uses ExcelWriterXML_Style::alignVerticaltext()
 * @uses ExcelWriterXML_Style::alignWraptext()
 * @uses ExcelWriterXML_Style::bgColor()
 * @uses ExcelWriterXML_Style::bgPattern()
 * @uses ExcelWriterXML_Style::bgPatternColor()
 * @uses ExcelWriterXML_Style::border()
 * @uses ExcelWriterXML_Style::checkColor()
 * @uses ExcelWriterXML_Style::fontBold()
 * @uses ExcelWriterXML_Style::fontColor()
 * @uses ExcelWriterXML_Style::fontFamily()
 * @uses ExcelWriterXML_Style::fontItalic()
 * @uses ExcelWriterXML_Style::fontName()
 * @uses ExcelWriterXML_Style::fontOutline()
 * @uses ExcelWriterXML_Style::fontShadow()
 * @uses ExcelWriterXML_Style::fontStrikethrough()
 * @uses ExcelWriterXML_Style::fontUnderline()
 * @uses ExcelWriterXML_Style::getErrors()
 * @uses ExcelWriterXML_Style::getID()
 * @uses ExcelWriterXML_Style::getStyleXML()
 * @uses ExcelWriterXML_Style::name()
 * @uses ExcelWriterXML_Style::numberFormat()
 * @uses ExcelWriterXML_Style::numberFormatDate()
 * @uses ExcelWriterXML_Style::numberFormatDatetime()
 * @uses ExcelWriterXML_Style::numberFormatTime()
 */
class ExcelWriterXML_Sheet 
{
	// Private Variables
	var $id;
	var $cells = array();
	var $colWidth = array();
	var $rowHeight = array();
	var $URLs = array();
	var $mergeCells = array();
	var $comments = array();
	var $formatErrors = array();
	var $displayRightToLeft = false;
	/////////////////////
	
	// Public Variables
	/////////////////////
	
	// Constructor
	/**
     * Constructor for a new Sheet
     * @param string $id The name of the sheet to be referenced within the
     * spreadsheet
     */
	function ExcelWriterXML_Sheet($id)
	{
		$this->id = $id;
	}
	
	/**
	 * Function to get the named value of the Sheet
	 * @return string Name of the Sheet
	 */
	function getID()
	{
		return $this->id;
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
			'sheet'		=> $this->id,
			'function'	=> $function,
			'message'	=> $message,
		);
		$this->formatErrors[] = $tmp;
	}
	
	/**
	 * Returns any errors found in the sheet
	 * @return mixed Array of errors if they exist, otherwise false
	 */
	function getErrors()
	{
		return($this->formatErrors);
	}

	/**
     * Converts a MySQL type datetime field to a value that can be used within
     * Excel.
     * If the passed value is not valid then the passed string is sent back.
     * @param string $datetime Value must in in the format "yyyy-mm-dd hh:ii:ss"
     * @return string Value in the Excel format "yyyy-mm-ddThh:ii:ss.000"
     */
	function convertMysqlDatetime($datetime)
	{
		$datetime = trim($datetime);
		$pattern = "/[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}/";
		if (preg_match($pattern, $datetime, $matches)) 
		{
			$datetime = $matches[0];
			list($date,$time) = explode(' ',$datetime);
			return($date.'T'.$time.'.000');
		}
		else
		{
			return($datetime);
		}
	}
	
	/**
     * Converts a MySQL type date field to a value that can be used within Excel
     * If the passed value is not valid then the passed string is sent back.
     * @param string $datetime Value must in in the format "yyyy-mm-dd hh:ii:ss"
     * or "yyyy-mm-dd"
     * @return string Value in the Excel format "yyyy-mm-ddT00:00:00.000"
     */
	function convertMysqlDate($datetime)
	{
		$datetime = trim($datetime);
		$pattern1 = "/[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}/";
		$pattern2 = "/[0-9]{4}-[0-9]{2}-[0-9]{2}/";
		if (preg_match($pattern1, $datetime, $matches)) 
		{
			$datetime = $matches[0];
			list($date,$time) = explode(' ',$datetime);
			return($date.'T'.$time.'.000');
		}
		elseif (preg_match($pattern2, $datetime, $matches)) 
		{
			$date = $matches[0];
			return($date.'T00:00:00.000');
		}
		else
		{
			return($datetime);
		}
	}
	
	/**
     * Converts a MySQL type time field to a value that can be used within Excel
     * If the passed value is not valid then the passed string is sent back.
     * @param string $datetime Value must in in the format "yyyy-mm-dd hh:ii:ss"
     * or "hh:ii:ss"
     * @return string Value in the Excel format "1899-12-31Thh:ii:ss.000"
     */
	function convertMysqlTime($datetime)
	{
		$datetime = trim($datetime);
		$pattern1 = "/[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}/";
		$pattern2 = "/[0-9]{2}:[0-9]{2}:[0-9]{2}/";
		if (preg_match($pattern1, $datetime, $matches)) 
		{
			$datetime = $matches[0];
			list($date,$time) = explode(' ',$datetime);
			return($date.'T'.$time.'.000');
		}
		elseif (preg_match($pattern2, $datetime, $matches)) 
		{
			$time = $matches[0];
			return('1899-12-31T'.$time.'.000');
		}
		else
		{
			return($datetime);
		}
	}
	

	/**
     * Writes a formula to a cell
     * From MS
     * Specifies the formula stored in this cell. All formulas are persisted in
     * R1C1 notation because they are significantly easier to parse and generate
     * than A1-style formulas. The formula is calculated upon reload unless
     * calculation is set to manual. Recalculation of the formula overrides the
     * value in this cell's Value attribute.
     * @see writeFormula()
     * @param string $dataType Type of data that the formula should generate,
     * "String" "Number" "DateTime"
     * @param integer $row Row, based upon a "1" based array
     * @param integer $column Column, based upon a "1" based array
     * @param string $data Formula data to be written to a cell
     * @param mixed $style Named style, or style reference to be applied to the
     * cell
     */
	function writeFormula($dataType,$row,$column,$data,$style = null)
	{
		if ($dataType != 'String'
			&& $dataType != 'Number'
			&& $dataType != 'DateTime')
		{
			$this->_addError(__FUNCTION__,'('.$row.','.$column.') DataType for formula was not valid "'.$dataType.'"');
			$halign = 'Automatic';
		}

		$this->_writeData('String',$row,$column,'',$style,$data);
	}

	/**
     * Writes a string to a cell
     * @see writeData()
     * @param integer $row Row, based upon a "1" based array
     * @param integer $column Column, based upon a "1" based array
     * @param string $data String data to be written to a cell
     * @param mixed $style Named style, or style reference to be applied to the
     * cell
     */
	function writeString($row,$column,$data,$style = null)
	{
		$this->_writeData('String',$row,$column,$data,$style);
	}
	
	/**
     * Writes a number to a cell.
     * If the data is not numeric then the function will write the data as a
     * string.
     * @see writeData()
     * @param integer $row Row, based upon a "1" based array
     * @param integer $column Column, based upon a "1" based array
     * @param mixed $data Number data to be written to a cell
     * @param mixed $style Named style, or style reference to be applied to the
     * cell
     */
	function writeNumber($row,$column,$data,$style = null)
	{
		if (!is_numeric($data))
		{
			$this->_writeData('String',$row,$column,$data,$style);
			$this->_addError(__FUNCTION__,'('.$row.','.$column.') Tried to write non-numeric data to type Number "'.$data.'"');
		}
		else
		{		
			$this->_writeData('Number',$row,$column,$data,$style);
		}
	}
	
	/**
     * Writes a Date/Time to a cell.
     * If data is not valid the function will write the passed value as a
     * string.
     * @see writeData()
     * @param integer $row Row, based upon a "1" based array
     * @param integer $column Column, based upon a "1" based array
     * @param string $data Date or Time data to be written to a cell.  This must
     * be in the format "yyyy-mm-ddThh:ii:ss.000" for Excel to recognize it.
     * @param mixed $style Named style, or style reference to be applied to the
     * cell
     */
	function writeDateTime($row,$column,$data,$style = null)
	{
		$pattern = "/[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}:[0-9]{2}:[0-9]{2}\.000/";
		if (preg_match($pattern, $data, $matches)) 
		{
			$data = $matches[0];
			$this->_writeData('DateTime',$row,$column,$data,$style);
		}
		else
		{
			$this->_writeData('String',$row,$column,$data,$style);
			$this->_addError(__FUNCTION__,'('.$row.','.$column.') Tried to write invalid datetime data to type DateTime "'.$data.'"');
		}
	}
	function _writeData($type,$row,$column,$data,$style = null,$formula = null)
	{
		if ($style != null)
		{
/*			if (gettype($style) == 'object')
			{
				if (get_class($style) == 'ExcelWriterXML_Style')
				{
					$styleID = $style->getID();
				}
				else
				{
					$this->_addError(__FUNCTION__,'('.$row.','.$column.') StyleID supplied was an object, but not a style object "'.get_class($style).'"');
					$styleID = null;
				}
			}
			else
*/			{
				$styleID = $style;
			}
		}
		else
		{
			$styleID = null;
		}
		
		$cell = array(
			'type'		=> $type,
			'style'		=> $styleID,
			'data'		=> $data,
			'formula'	=> $formula,
		);
		$this->cells[$row][$column] = $cell;
	}
	
	/**
	 * Displays the sheet in Right to Left format
	 */
	function displayRightToLeft()
	{
		$this->displayRightToLeft = true;
	}

	/**
     * Called by the ExcelWriterXML class to get the XML data for this object
     * @return string Contains only the XML data for the sheet
     */
	function getSheetXML()
	{
		ksort($this->cells);
		$displayRightToLeft = ($this->displayRightToLeft) ? 'ss:RightToLeft="1"' : '';
		
		$xml = '<Worksheet ss:Name="'.$this->id.'" '.$displayRightToLeft.'>'."\r";
		$xml .= '	<Table>'."\r";
		foreach($this->colWidth as $colIndex => $colWidth)
		{
			  $xml .= '		<Column ss:Index="'.$colIndex.'" ss:AutoFitWidth="0" ss:Width="'.$colWidth.'"/>'."\r";
		}
		foreach($this->cells as $row => $rowData)
		{
			ksort($rowData);
			if (isset($this->rowHeight[$row]))
			{
				$rowHeight = 'ss:AutoFitHeight="0" ss:Height="'.$this->rowHeight[$row].'"';
			}
			else
			{
				$rowHeight = '';
			}
			$xml .= '		<Row ss:Index="'.$row.'" '.$rowHeight.' >'."\r";
			foreach($rowData as $column => $cell)
			{
				if (!empty($cell['formula'])) 
					$formula = 'ss:Formula="'.$cell['formula'].'"';
				else 
					$formula = '';
				if (!empty($cell['style'])) 
					$style = 'ss:StyleID="'.$cell['style'].'"';
				else 
					$style = '';
				if (empty($this->URLs[$row][$column])) 
					$URL = '';
				else 
					$URL = 'ss:HRef="'.htmlspecialchars($this->URLs[$row][$column]).'"';
				if (empty($this->mergeCells[$row][$column])) 
					$mergeCell = '';
				else 
					$mergeCell = 'ss:MergeAcross="'.$this->mergeCells[$row][$column]['width'].'" ss:MergeDown="'.$this->mergeCells[$row][$column]['height'].'"';
				if (empty($this->comments[$row][$column])) 
					$comment = '';
				else
				{
					$comment = '					<Comment ss:Author="'.$this->comments[$row][$column]['author'].'">'."\r";
					$comment .= '					<ss:Data xmlns="http://www.w3.org/TR/REC-html40">'."\r";
					$comment .= '					<B><Font html:Face="Tahoma" x:CharSet="1" html:Size="8" html:Color="#000000">'.htmlspecialchars($this->comments[$row][$column]['author']).':</Font></B>'."\r";
					$comment .= '					<Font html:Face="Tahoma" x:CharSet="1" html:Size="8" html:Color="#000000">'.htmlspecialchars($this->comments[$row][$column]['comment']).'</Font>'."\r";
					$comment .= '					</ss:Data>'."\r";
					$comment .= '					</Comment>'."\r";
				}
				$type = $cell['type'];
				$data = $cell['data'];
				
				$xml .= '			<Cell '.$style.' ss:Index="'.$column.'" '.$URL.' '.$mergeCell.' '.$formula.'>'."\r";
				$xml .= '				<Data ss:Type="'.$type.'">';
				$xml .= htmlspecialchars($data);
				$xml .= '</Data>'."\r";
				$xml .= $comment;
				$xml .= '			</Cell>'."\r";
			}
			$xml .= '		</Row>'."\r";
		}
		$xml .= '	</Table>'."\r";
		$xml .= '</Worksheet>'."\r";
		return($xml);
	}

	/**
     * Alias for function columnWidth()
     */
	function cellWidth( $row, $col,$width = 48) { $this->columnWidth($col,$width); }

	/**
     * Sets the width of a cell.
     * Sets  the width of the column that the cell resides in.
     * Cell width of zero effectively hides the column
     * @param integer $row Row, based upon a "1" based array
     * @param integer $col Column, based upon a "1" based array
     * @param mixed $width Width of the cell/column, default is 48
     */
	function columnWidth( $col,$width = 48) { $this->colWidth[$col] = $width; }

	/**
     * Alias for function rowHeight()
     */
	function cellHeight( $row, $col,$height = 12.5) { $this->rowHeight($row,$height); }

	/**
     * Sets the height of a cell.
     * Sets  the height of the column that the cell resides in.
     * Cell height of zero effectively hides the row
     * @param integer $row Row, based upon a "1" based array
     * @param integer $col Column, based upon a "1" based array
     * @param mixed $height Height of the cell/column, default is 12.5
     */
	function rowHeight( $row,$height = 12.5) { $this->rowHeight[$row] = $height; }

	/**
     * Makes the target cell a link to a URL
     * @param integer $row Row, based upon a "1" based array
     * @param integer $col Column, based upon a "1" based array
     * @param string $URL The URL that the link should point to
     */
	function addURL( $row, $col,$URL) { $this->URLs[$row][$col] = $URL; }
	
	/**
     * Merges 2 or more cells.
     * The function acts like a bounding box, with the row and column defining
     * the upper left corner, and the width and height extending the box.
     * If width or height are zero (or ommitted) then the function does nothing.
     * @param integer $row Row, based upon a "1" based array
     * @param integer $col Column, based upon a "1" based array
     * @param integer $width Number of cells to the right to merge with
     * @param integer $height Number of cells down to merge with
     */
	function cellMerge($row,$col, $width = 0, $height = 0)
	{
		if ($width < 0 || $height < 0)
		{
			$this->_addError(__FUNCTION__,'('.$row.','.$col.') Tried to merge cells with width/height < 0 "(w='.$width.',h='.$height.')"');
			return;
		}
		
		$this->mergeCells[$row][$col] = array(
			'width'		=> $width,
			'height'	=> $height,
		);
		/* I don't think this code is necessary
		if (!isset($cells[$row][$col]))
		{
			$this->writeString($row,$col,'');
		}
		*/
	}
	
	/**
     * Adds a comment to a cell
     * @param integer $row Row, based upon a "1" based array
     * @param integer $col Column, based upon a "1" based array
     * @param string $comment The comment to be displayed on the cell
     * @param string $author The comment will show a bold header displaying the
     * author
     */
	function addComment( $row, $col,$comment,$author = 'SYSTEM')
	{
		$this->comments[$row][$col] = array(
			'comment'	=> $comment,
			'author'	=> $author,
		);
	}
}
?>