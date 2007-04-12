<?php

// always use capitals in reserved words (for is_reserved_word comparisons)

$any_item = 'AN';
$any_number = -1;
$all_option = "___ALL___";
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