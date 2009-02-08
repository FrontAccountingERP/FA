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
// always use capitals in reserved words (for is_reserved_word comparisons)

$any_item = 'AN';
$any_number = -1;
$all_option = '';
$all_option_numeric = -1;

class reserved_words 
{
	
	function get_any() 
	{
		global $any_item;
		return $any_item;
	} 
	
	function get_any_numeric() 
	{
		global $any_number;
		return $any_number;
	}
	
	function get_all() 
	{
		global $all_option;
		return $all_option;
	}
	
	function get_all_numeric() 
	{
		global $all_option_numeric;
		return $all_option_numeric;
	}
	
	function is_reserved_word($str) 
	{
		$str = strtoupper($str);
		if ($str == get_any())
			return true;
		if ($str == get_all())
			return true;			
		return false;
	}
	
}

?>