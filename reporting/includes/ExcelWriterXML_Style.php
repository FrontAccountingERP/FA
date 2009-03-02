<?php
/**
 * File contains the class files for ExcelWriterXML_Style
 * @package ExcelWriterXML
 */

/**
 * Style class for generating Excel styles
 * @link http://msdn.microsoft.com/en-us/library/aa140066(office.10).aspx
 * @author Robert F Greer
 * @version 1.0
 * @package ExcelWriterXML
 */
class ExcelWriterXML_Style 
{
	// Private Variables
	/////////////////////
	// Options
	var $id;
	var $name;
	var $useAlignment = false;
	var $useFont = false;
	var $useBorder = false;
	var $useInterior = false;
	
	// Alignment
	var $valign;
	var $halign;
	var $rotate;
	var $shrinktofit = 0;
	var $verticaltext = 0;
	var $wraptext = 0;

	// Font
	var $fontColor = 'Automatic';
	var $fontName;
	var $fontFamily;
	var $fontSize;
	var $bold;
	var $italic;
	var $underline;
	var $strikethrough;
	var $shadow;
	var $outline;
	/////////////////////
	
	// Borders
	var $borderTop = array();
	var $borderBottom = array();
	var $borderLeft = array();
	var $borderRight = array();
	var $borderDL = array();
	var $borderDR = array();
	/////////////////////
	
	// Interior
	var $interiorColor;
	var $interiorPattern;
	var $interiorPatternColor;
	////////////////////
	
	// NumberFormat
	var $numberFormat;
	/////////////////////
	
	// Other Vars
	var $formatErrors = array();
	var $namedColorsIE = array (
		'aliceblue' => '#F0F8FF',
		'antiquewhite' => '#FAEBD7',
		'aqua' => '#00FFFF',
		'aquamarine' => '#7FFFD4',
		'azure' => '#F0FFFF',
		'beige' => '#F5F5DC',
		'bisque' => '#FFE4C4',
		'black' => '#000000',
		'blanchedalmond' => '#FFEBCD',
		'blue' => '#0000FF',
		'blueviolet' => '#8A2BE2',
		'brown' => '#A52A2A',
		'burlywood' => '#DEB887',
		'cadetblue' => '#5F9EA0',
		'chartreuse' => '#7FFF00',
		'chocolate' => '#D2691E',
		'coral' => '#FF7F50',
		'cornflowerblue' => '#6495ED',
		'cornsilk' => '#FFF8DC',
		'crimson' => '#DC143C',
		'cyan' => '#00FFFF',
		'darkblue' => '#00008B',
		'darkcyan' => '#008B8B',
		'darkgoldenrod' => '#B8860B',
		'darkgray' => '#A9A9A9',
		'darkgreen' => '#006400',
		'darkkhaki' => '#BDB76B',
		'darkmagenta' => '#8B008B',
		'darkolivegreen' => '#556B2F',
		'darkorange' => '#FF8C00',
		'darkorchid' => '#9932CC',
		'darkred' => '#8B0000',
		'darksalmon' => '#E9967A',
		'darkseagreen' => '#8FBC8F',
		'darkslateblue' => '#483D8B',
		'darkslategray' => '#2F4F4F',
		'darkturquoise' => '#00CED1',
		'darkviolet' => '#9400D3',
		'deeppink' => '#FF1493',
		'deepskyblue' => '#00BFFF',
		'dimgray' => '#696969',
		'dodgerblue' => '#1E90FF',
		'firebrick' => '#B22222',
		'floralwhite' => '#FFFAF0',
		'forestgreen' => '#228B22',
		'fuchsia' => '#FF00FF',
		'gainsboro' => '#DCDCDC',
		'ghostwhite' => '#F8F8FF',
		'gold' => '#FFD700',
		'goldenrod' => '#DAA520',
		'gray' => '#808080',
		'green' => '#008000',
		'greenyellow' => '#ADFF2F',
		'honeydew' => '#F0FFF0',
		'hotpink' => '#FF69B4',
		'indianred' => '#CD5C5C',
		'indigo' => '#4B0082',
		'ivory' => '#FFFFF0',
		'khaki' => '#F0E68C',
		'lavender' => '#E6E6FA',
		'lavenderblush' => '#FFF0F5',
		'lawngreen' => '#7CFC00',
		'lemonchiffon' => '#FFFACD',
		'lightblue' => '#ADD8E6',
		'lightcoral' => '#F08080',
		'lightcyan' => '#E0FFFF',
		'lightgoldenrodyellow' => '#FAFAD2',
		'lightgreen' => '#90EE90',
		'lightgrey' => '#D3D3D3',
		'lightpink' => '#FFB6C1',
		'lightsalmon' => '#FFA07A',
		'lightseagreen' => '#20B2AA',
		'lightskyblue' => '#87CEFA',
		'lightslategray' => '#778899',
		'lightsteelblue' => '#B0C4DE',
		'lightyellow' => '#FFFFE0',
		'lime' => '#00FF00',
		'limegreen' => '#32CD32',
		'linen' => '#FAF0E6',
		'magenta' => '#FF00FF',
		'maroon' => '#800000',
		'mediumaquamarine' => '#66CDAA',
		'mediumblue' => '#0000CD',
		'mediumorchid' => '#BA55D3',
		'mediumpurple' => '#9370DB',
		'mediumseagreen' => '#3CB371',
		'mediumslateblue' => '#7B68EE',
		'mediumspringgreen' => '#00FA9A',
		'mediumturquoise' => '#48D1CC',
		'mediumvioletred' => '#C71585',
		'midnightblue' => '#191970',
		'mintcream' => '#F5FFFA',
		'mistyrose' => '#FFE4E1',
		'moccasin' => '#FFE4B5',
		'navajowhite' => '#FFDEAD',
		'navy' => '#000080',
		'oldlace' => '#FDF5E6',
		'olive' => '#808000',
		'olivedrab' => '#6B8E23',
		'orange' => '#FFA500',
		'orangered' => '#FF4500',
		'orchid' => '#DA70D6',
		'palegoldenrod' => '#EEE8AA',
		'palegreen' => '#98FB98',
		'paleturquoise' => '#AFEEEE',
		'palevioletred' => '#DB7093',
		'papayawhip' => '#FFEFD5',
		'peachpuff' => '#FFDAB9',
		'peru' => '#CD853F',
		'pink' => '#FFC0CB',
		'plum' => '#DDA0DD',
		'powderblue' => '#B0E0E6',
		'purple' => '#800080',
		'red' => '#FF0000',
		'rosybrown' => '#BC8F8F',
		'royalblue' => '#4169E1',
		'saddlebrown' => '#8B4513',
		'salmon' => '#FA8072',
		'sandybrown' => '#F4A460',
		'seagreen' => '#2E8B57',
		'seashell' => '#FFF5EE',
		'sienna' => '#A0522D',
		'silver' => '#C0C0C0',
		'skyblue' => '#87CEEB',
		'slateblue' => '#6A5ACD',
		'slategray' => '#708090',
		'snow' => '#FFFAFA',
		'springgreen' => '#00FF7F',
		'steelblue' => '#4682B4',
		'tan' => '#D2B48C',
		'teal' => '#008080',
		'thistle' => '#D8BFD8',
		'tomato' => '#FF6347',
		'turquoise' => '#40E0D0',
		'violet' => '#EE82EE',
		'wheat' => '#F5DEB3',
		'white' => '#FFFFFF',
		'whitesmoke' => '#F5F5F5',
		'yellow' => '#FFFF00',
		'yellowgreen' => '#9ACD32',
	);
	/////////////////////
	
	// Public Variables
	/////////////////////
	
	// Constructor
	
	/**
     * Constructor for a style
     * @param string $id The named style referenced by Excel.  This is called by
     * ExcelWriterXML object when adding a style
     */
	function ExcelWriterXML_Style($id)
	{
		$this->id = $id;
	}
	/////////////////////
	
	/**
	 * Returns the named style for this style
	 * @return string $id The id for this style
	 */
	function getID()
	{
		return $this->id;
	}
	
	/**
     * Retrieves the XML string data for a style.
     * Called by ExcelWriterXML object
     * @return string Returns the formatted XML data <style>...</style>
     */
	function getStyleXML()
	{
		$name = '';
		$valign = '';
		$halign = '';
		$rotate = '';
		$shrinktofit = '';
		$verticaltext = '';
		$wraptext = '';
		
		$bold = '';
		$italic = '';
		$strikethrough = '';
		$underline = '';
		$outline = '';
		$shadow = '';
		$fontName = '';
		$fontFamily = '';
		$fontSize = '';
		
		$borders = '';
		
		$interior = '';
		$interiorColor = '';
		$interiorPattern = '';
		$interiorPatternColor = '';
		
		$numberFormat = '';
		
		if (empty($this->id)) 
			throw new exception;
		if (!empty($this->name)){$name = 'ss:Name="'.$this->name.'"';}
		
		// Alignment
		if ($this->useAlignment)
		{
			if (!empty($this->valign)) {$valign = 'ss:Vertical="'.$this->valign.'"';}
			if (!empty($this->halign)) {$halign = 'ss:Horizontal="'.$this->halign.'"';}
			if (!empty($this->rotate)) {$rotate = 'ss:Rotate="'.$this->rotate.'"';}
			if (!empty($this->shinktofit)) {$shrinktofit = 'ss:ShrinkToFit="1"';}
			if (!empty($this->verticaltext)) {$verticaltext = 'ss:VerticalText="1"';}
			if (!empty($this->wraptext)) {$wraptext = 'ss:WrapText="1"';}
		}
		
		// Font
		if ($this->useFont)
		{
			if (!empty($this->fontColor)) {$fontColor = 'ss:Color="'.$this->fontColor.'"';}
			if (!empty($this->bold)) {$bold = 'ss:Bold="1"';}
			if (!empty($this->italic)) {$italic = 'ss:Italic="1"';}
			if (!empty($this->strikethrough)) {$strikethrough = 'ss:StrikeThrough="'.$this->strikethrough.'"';}
			if (!empty($this->underline)) {$underline = 'ss:Underline="'.$this->underline.'"';}
			if (!empty($this->outline)) {$outline = 'ss:Outline="1"';}
			if (!empty($this->shadow)) {$shadow = 'ss:Shadow="1"';}
			if (!empty($this->fontName)) {$fontName = 'ss:FontName="'.$this->fontName.'"';}
			if (!empty($this->fontFamily)) {$fontFamily = 'x:Family="'.$this->fontFamily.'"';}
			if (!empty($this->fontSize)) {$fontSize = 'ss:Size="'.$this->fontSize.'"';}
		}
		// Border
		if ($this->useBorder)
		{
			$borders = '		<Borders>'."\r";
			$positions = array(
				'Top'			=> $this->borderTop,
				'Bottom'		=> $this->borderBottom,
				'Left'			=> $this->borderLeft,
				'Right'			=> $this->borderRight,
				'DiagonalLeft'	=> $this->borderDL,
				'DiagonalRight'	=> $this->borderDR,

			);
			foreach($positions as $position => $pData)
			{
				if (empty($pData)) 
					continue;
				$bLinestyle = isset($pData['LineStyle'])
					? 'ss:LineStyle="'.$pData['LineStyle'].'"'
					: '';
				$bColor = isset($pData['Color'])
					? 'ss:Color="'.$pData['Color'].'"'
					: '';
				$bWeight = isset($pData['Weight'])
					? 'ss:Weight="'.$pData['Weight'].'"'
					: '';
				$borders .= '			<Border ss:Position="'.$position.'" '.$bLinestyle.' '.$bColor.' '.$bWeight.'/>'."\r";
			}
			$borders .= '		</Borders>'."\r";
		}
		
		if ($this->useInterior)
		{
			if (!empty($this->interiorColor)) {$interiorColor = 'ss:Color="'.$this->interiorColor.'"';}
			if (!empty($this->interiorPattern)) {$interiorPattern = 'ss:Pattern="'.$this->interiorPattern.'"';}
			if (!empty($this->interiorPatternColor)) {$interiorPatternColor = 'ss:PatternColor="'.$this->interiorPatternColor.'"';}
			$interior = '		<Interior '.$interiorColor.' '.$interiorPattern.' '.$interiorPatternColor.'/>'."\r";
		}
		
		if (!empty($this->numberFormat)) 
		{
			$numberFormat = '		<NumberFormat ss:Format="'.$this->numberFormat.'"/>'."\r";
		}
		else 
			$numberFormat = '		<NumberFormat/>'."\r";
		
		$xml = '	<Style ss:ID="'.$this->id.'" '.$name.'>'."\r";
		if ($this->useAlignment) 
			$xml .= '		<Alignment '.$valign.' '.$halign.' '.$rotate.' '.$shrinktofit.' '.$wraptext.' '.$verticaltext.'/>'."\r";
		if ($this->useBorder) 
			$xml .= $borders;
		if ($this->useFont) 
			$xml .= '		<Font '.$fontSize.' '.$fontColor.' '.$bold.' '.$italic.' '.$strikethrough.' '.$underline.' '.$shadow.' '.$outline.' '.$fontName.' '.$fontFamily.'/>'."\r";
		if ($this->useInterior) 
			$xml .= $interior;
		$xml .= $numberFormat;
		$xml .= '		<Protection/>'."\r";
		$xml .= '	</Style>'."\r";
		return($xml);
	}
	
	/**
	 * Checks whether a color is valid for the spreadsheet
	 * @param string $color Named color from MS or web color in HEX format (e.g.
     * #ff00ff
     * @return mixed Either the valid color in HEX format or false if the color
     * is not valid
	 */
	function checkColor($color)
	{
		$pattern = "/[0-9a-f]{6}/";
		if (preg_match($pattern, $color, $matches)) 
		{
			$color = '#'.$matches[0];
			return($color);
		}
		elseif (isset($this->namedColorsIE[strtolower($color)]))
		{
			$color = $this->namedColorsIE[strtolower($color)];
			return($color);
		}
		else
		{
			$this->_addError(__FUNCTION__,'Supplied color was not valid "'.$color.'"');
			return(false);
		}
	}
	
	/**
	 * Adds a format error.  When the document is generated if there are any
	 * errors they will be listed on a seperate sheet.
	 * @param string $namedStyle The style in which the error occurred
	 * @param string $function The name of the function that was called
	 * @param string $message Details of the error
	 */
	function _addError($function, $message)
	{
		$tmp = array(
			'style'		=> $this->id,
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

	
	// Change Options
	
	/**
     * Changes the name of the named style
     * @param string $name The named style referenced by Excel.
     */
	function name($name) {$this->name = $name; }
	////////////////////////
	
	// Change Alignment
	/**
     * Changes the vertical alignment setting for the style
     * @param string $valign The value for the vertical alignment.
     * Acceptable values are "Automatic" "Top" "Bottom" "Center"
     */
	function alignVertical($valign)
	{
		// Automatic, Top, Bottom, Center
		if ($valign != 'Automatic'
			&& $valign != 'Top'
			&& $valign != 'Bottom'
			&& $valign != 'Center')
		{
			$this->_addError(__FUNCTION__,'vertical alignment was not valid "'.$valign.'"');
			return;
		}
		$this->valign = $valign;
		$this->useAlignment = true;
	}
	
	/**
     * Changes the horizontal alignment setting for the style
     * @param string $halign The value for the horizontal alignment. Acceptable
     * values are "Automatic" "Left" "Center" "Right"
     */
	function alignHorizontal($halign)
	{
		// Automatic, Left, Center, Right
		if ($halign != 'Automatic'
			&& $halign != 'Left'
			&& $halign != 'Center'
			&& $halign != 'Right')
		{
			$this->_addError(__FUNCTION__,'horizontal alignment was not valid "'.$halign.'"');
			$halign = 'Automatic';
		}
		$this->halign = $halign;
		$this->useAlignment = true;
	}
	
	/**
     * Changes the rotation setting for the style
     * @param mixed $rotate The value for the Rotation.  Value must be a
     * number between -90 and 90
     */
	function alignRotate($rotate)
	{
		// Degrees to rotate the text
		if (!is_numeric($rotate))
		{
			$this->_addError(__FUNCTION__,'rotation was not numeric "'.$rotate.'"');
			return;
		}
		if (abs($rotate) > 90)
		{
			$rotate = $rotate % 90;
			$this->_addError(__FUNCTION__,'rotation was greater than 90, defaulted to "'.$rotate.'"');
		}
		$this->rotate = $rotate;
		$this->useAlignment = true;
	}
	
	/**
     * Changes the Shrink To Fit setting for the style
     * ShrinkToFit shrinks the text so that it fits within the cell.
     * This doesn't actually work.
     */
	function alignShrinktofit()
	{
		$this->shrinktofit = 1;
		$this->useAlignment = true;
	}
	
	/**
     * Changes the Vertical Text setting for the style.
     * Text will be displayed vertically.
     */
	function alignVerticaltext()
	{
		$this->verticaltext = 1;
		$this->useAlignment = true;
	}
	
	/**
     * Changes the Wrap Text setting for the style.
     */
	function alignWraptext()
	{
		$this->wraptext = 1;
		$this->useAlignment = true;
	}
	/////////////////////////
	
	// Change Font
	/**
     * Changes the size of the font
     * @param string $fontSize The value for the Size. Value must be greater
     * than zero
     */
	function fontSize($fontSize = 10)
	{
		if (!is_numeric($fontSize))
		{
			$fontSize = 10;
			$this->_addError(__FUNCTION__,'font size was not a number, defaulted to 10 "'.$fontSize.'"');
		}
		if ($fontSize <= 0)
		{
			$fontSize = 10;
			$this->_addError(__FUNCTION__,'font size was less than zero, defaulted to 10 "'.$fontSize.'"');
		}
		$this->fontSize = $fontSize;
		$this->useFont = true;
	}
	
	/**
     * Changes the color for the font
     * @param string $fontColor The value for the Color.
     * This can be a MS named color or a Hex web color.
     */
	function fontColor($fontColor = 'Automatic')
	{
		$pattern = "/[0-9a-f]{6}/";
		$fontColor = $this->checkColor($fontColor);		
		if ($fontColor === false)
		{
			$this->_addError(__FUNCTION__,'font color was not valid "'.$fontColor.'"');
			$fontColor = 'Automatic';
		}
		$this->fontColor = $fontColor;
		$this->useFont = true;
	}
	
	/**
     * Changes the font for the cell
     * @param string $fontName The value for the font name. This should be a
     * standard windows font available on most systems.
     */
	function fontName($fontName = 'Arial')
	{
		$this->fontName = $fontName;
		$this->useFont = true;
	}
	
	/**
     * Changes the family for the font
     * @param string $fontFamily The value for the font family. Not really sure
     * what this does.
     * Win32-dependant font family.
     * Values can be "Automatic" "Decorative"
     * "Modern" "Roman" "Script" "Swiss"
     */
	function fontFamily($fontFamily = 'Swiss')
	{
		// Win32-dependent font family.  
		// Automatic, Decorative, Modern, Roman, Script, and Swiss 
		if ($fontFamily != 'Automatic'
			&& $fontFamily != 'Decorative'
			&& $fontFamily != 'Modern'
			&& $fontFamily != 'Roman'
			&& $fontFamily != 'Script'
			&& $fontFamily != 'Swiss')
		{
			$this->_addError(__FUNCTION__,'font family was not valid "'.$fontFamily.'"');
			return;
		}
		$this->fontFamily = $fontFamily;
		$this->useFont = true;
	}
	
	/**
     * Makes the font bold for the named style
     */
	function fontBold()
	{
		$this->bold = 1;
		$this->useFont = true;
	}
	
	/**
     * Makes the font italic for the named style
     */
	function fontItalic()
	{
		$this->italic = 1;
		$this->useFont = true;
	}
	
	/**
     * Makes the font strikethrough for the named style
     */
	function fontStrikethrough()
	{
		$this->strikethrough = 1;
		$this->useFont = true;
	}
	
	/**
     * Makes the font underlined for the named style
     * @param string $uStyle The type of underlining for the style.
     * Acceptable values are "None" "Single" "Double" "SingleAccounting"
     * "DoubleAccounting"
     */
	function fontUnderline($uStyle = 'Single')
	{
		// None, Single, Double, SingleAccounting, and DoubleAccounting
		if ($uStyle != 'None'
			&& $uStyle != 'Single'
			&& $uStyle != 'Double'
			&& $uStyle != 'SingleAccounting'
			&& $uStyle != 'DoubleAccounting')
		{
			$this->_addError(__FUNCTION__,'underline type was not valid "'.$uStyle.'"');
			return;
		}
		$this->underline = $uStyle;
		$this->useFont = true;
	}
	
	/**
     * Makes the font shadowed for the named style
     */
	function fontShadow()
	{
		$this->shadow = 1;
		$this->useFont = true;
	}

	/**
     * Makes the font outlines for the named style
     */
	function fontOutline()
	{
		$this->outline = 1;
		$this->useFont = true;
	}
	//////////////////////////
	
	// Change Border
	
	/**
     * Sets the border for the named style.
     * This function can be called multiple times to set different sides of the
     * cell or set all sides the same at once.
     * @param string $position Sets which side of the cell should be modified.
     * Acceptable values are "All" "Left" "Top" "Right" "Bottom" "DiagonalLeft"
     * "DiagonalRight"
     * @param integer $weight Thickness of the border.  Default is 1 "Thin"
     * @param string $color Color of the border. Default is "Automatic" but any
     * 6-hexadecimal digit number in "#rrggbb" format or it can be any of the
     * Microsoft® Internet Explorer named colors
     * @param string $linestyle Type of line to use on the border.
     * Default is "Continuous".  Acceptable balues are "None" "Continuous"
     * "Dash" "Dot" "DashDot" "DashDotDot" "SlantDashDot" "Double"
     */
	function border(
		$position = 'All'			// All, Left, Top, Right, Bottom, DiagonalLeft, DiagonalRight
		,$weight = '1'				// 0—Hairline, 1—Thin, 2—Medium, 3—Thick
		,$color = 'Automatic'		// Automatic, 6-hexadecimal digit number in "#rrggbb" format or it can be any of the Microsoft® Internet Explorer named colors
		,$linestyle = 'Continuous'	// None, Continuous, Dash, Dot, DashDot, DashDotDot, SlantDashDot, Double
		)
	{
		if ($position != 'All'
			&& $position != 'Left'
			&& $position != 'Top'
			&& $position != 'Right'
			&& $position != 'Bottom'
			&& $position != 'DiagonalLeft'
			&& $position != 'DiagonalRight')
		{
			$this->_addError(__FUNCTION__,'border position was not valid, defaulted to All "'.$position.'"');
			$position = 'All';
		}
		
		if (is_numeric($weight))
		{
			if (abs($weight) > 3)
			{
				$this->_addError(__FUNCTION__,'line weight greater than 3, defaulted to 3 "'.$weight.'"');
				$weight = 3;
			}
		}
		else
		{
			$this->_addError(__FUNCTION__,'line weight not numeric, defaulted to 3 "'.$weight.'"');
			$weight = 1;
		}

		$color = $this->checkColor($color);		
		if ($color === false)
		{
			$this->_addError(__FUNCTION__,'border color was not valid, defaulted to Automatic "'.$weight.'"');
			$color = 'Automatic';
		}

		if ($linestyle != 'None'
			&& $linestyle != 'Continuous'
			&& $linestyle != 'Dash'
			&& $linestyle != 'Dot'
			&& $linestyle != 'DashDot'
			&& $linestyle != 'DashDotDot'
			&& $linestyle != 'SlantDashDot'
			&& $linestyle != 'Double')
		{
			$linestyle = 'Continuous';
			$this->_addError(__FUNCTION__,'line style was not valid, defaulted to Continuous "'.$linestyle.'"');
		}

		
		$tmp = array(
			'LineStyle'	=> $linestyle,
			'Color'		=> $color,
			'Weight'	=> $weight,
		);
		if ($position == 'Top'		|| $position == 'All') $this->borderTop = $tmp;
		if ($position == 'Bottom'	|| $position == 'All') $this->borderBottom = $tmp;
		if ($position == 'Left'		|| $position == 'All') $this->borderLeft = $tmp;
		if ($position == 'Right'	|| $position == 'All') $this->borderRight = $tmp;
		if ($position == 'DiagonalLeft'	)					$this->borderDL = $tmp;
		if ($position == 'DiagonalRight'	)				$this->borderDR = $tmp;
		
		$this->useBorder = true;
	}
	//////////////////////////
	
	// Change Interior
	/**
     * Sets the background style of a style
     * @param string $color Named color from MS or web color in HEX format (e.g.
     * #ff00ff
     * @param string $pattern Defaults to a None if not supplied.
     * @param string $patternColor Defaults to a Automatic if not supplied.
     */
	function bgColor($color = 'Yellow',$pattern = 'Solid', $patternColor = null)
	{
		// 6-hexadecimal digit number in "#rrggbb" format
		// Or it can be any of the Internet Explorer named colors
		$color = $this->checkColor($color);
		if ($color === false)
		{
			$color = 'Yellow';
			$this->_addError(__FUNCTION__,'cell color not valid, defaulted to Yellow "'.$color.'"');
		}
		$this->interiorColor = $color;
		if ($pattern != 'None')
		{
			$this->bgPattern($pattern, $patternColor);
		}
		$this->useInterior = true;
	}

	/**
     * Sets the background pattern of a style.
     * @see bgColor()
     * @param string $color Named color from MS or web color in HEX format (e.g.
     * #ff00ff
     * @param string $pattern Defaults to a solid if not supplied.
     */
	function bgPattern($pattern = 'None', $color = null)
	{
		// None, Solid, Gray75, Gray50, Gray25, Gray125, Gray0625,
		// HorzStripe, VertStripe, ReverseDiagStripe, DiagStripe,
		// DiagCross, ThickDiagCross, ThinHorzStripe, ThinVertStripe,
		// ThinReverseDiagStripe, ThinDiagStripe, ThinHorzCross, and ThinDiagCross
		if ($pattern != 'None'
			&& $pattern != 'Solid'
			&& $pattern != 'Gray75'
			&& $pattern != 'Gray50'
			&& $pattern != 'Gray25'
			&& $pattern != 'Gray125'
			&& $pattern != 'Gray0625'
			&& $pattern != 'HorzStripe'
			&& $pattern != 'VertStripe'
			&& $pattern != 'ReverseDiagStripe'
			&& $pattern != 'DiagStripe'
			&& $pattern != 'DiagCross'
			&& $pattern != 'ThickDiagCross'
			&& $pattern != 'ThinHorzStripe'
			&& $pattern != 'ThinVertStripe'
			&& $pattern != 'ThinReverseDiagStripe'
			&& $pattern != 'ThinDiagStripe'
			&& $pattern != 'ThinHorzCross'
			&& $pattern != 'ThinDiagCross')
		{
			$pattern = 'None';
			$this->_addError(__FUNCTION__,'cell pattern was not valid, defaulted to Solid "'.$pattern.'"');
		}

		$this->interiorPattern = $pattern;
		if ($color != null) 
			$this->bgPatternColor($color);
		$this->useInterior = true;
	}

	/**
     * Specifies the secondary fill color of the cell when Pattern does not equal Solid.
     * @see function bgPattern()
     * @param string $color Named color from MS or web color in HEX format (e.g.
     * #ff00ff
     * @param string $pattern Defaults to a solid if not supplied.
     */
	function bgPatternColor($color = 'Yellow')
	{
		// 6-hexadecimal digit number in "#rrggbb" format
		// Or it can be any of the Internet Explorer named colors
		if ($color != 'Automatic')
		{
			$color = $this->checkColor($color);		
			if ($color === false)
			{
				$color = 'Automatic';
				$this->_addError(__FUNCTION__,'cell pattern color was not valid, defaulted to Automatic "'.$color.'"');
			}
		}
		$this->interiorPatternColor = $color;
		$this->useInterior = true;
	}

	//////////////////////////
	
	// Number Formats

	/**
     * Sets the number format of a style
     * @param string $formatString Format string to be used by Excel for
     * displaying the number.
     */
	function numberFormat($formatString) { $this->numberFormat = $formatString; }

	/**
     * Sets a default date format for a style
     */
	function numberFormatDate($format=null) { $this->numberFormat($format==null?'mm/dd/yyyy':$format); }

	/**
     * Sets a default time format for a style
     */
	function numberFormatTime($format=null) { $this->numberFormat($format==null?'hh:mm:ss':$format); }

	/**
     * Sets a default date and time format for a style
     */
	function numberFormatDatetime($format=null) { $this->numberFormat($format==null?'mm/dd/yyyy\ hh:mm:ss':$format); }
	//////////////////////////
}
?>