<?php

class user_prefs 
{

	var $language;

	var $qty_dec;
	var $price_dec;
	var	$exrate_dec;
	var	$percent_dec;

	var	$show_gl_info;
	var	$show_codes;

	var $date_format;
	var $date_sep;
	var $tho_sep;
	var $dec_sep;
	var $theme;
	var $pagesize; // for printing
	var $show_hints;

	function user_prefs(&$user) 
	{

		$this->language = $user["language"];
		language::set_language($this->language);

		$this->qty_dec = $user["qty_dec"];
		$this->price_dec = $user["prices_dec"];
		$this->exrate_dec = $user["rates_dec"];
		$this->percent_dec = $user["percent_dec"];

		$this->show_gl_info = $user["show_gl"];
		$this->show_codes = $user["show_codes"];
		$this->date_format = $user["date_format"];
		$this->date_sep = $user["date_sep"];
		$this->tho_sep = $user["tho_sep"];
		$this->dec_sep = $user["dec_sep"];
		$this->theme = $user["theme"];
		$this->pagesize = $user["page_size"];
		$this->show_hints = $user["show_hints"];
	}

	function language() 
	{
		return $this->language;
	}

	function qty_dec() 
	{
		return $this->qty_dec;
	}

	function price_dec() 
	{
		return $this->price_dec;
	}

	function exrate_dec() 
	{
		return $this->exrate_dec;
	}

	function percent_dec() 
	{
		return $this->percent_dec;
	}

	function show_gl_info() 
	{
		return $this->show_gl_info;
	}

	function show_codes() 
	{
		return $this->show_codes;
	}

	function date_format() 
	{
		return $this->date_format;
	}

	function date_sep() 
	{
		return $this->date_sep;
	}

	function date_display() 
	{
		global $dateseps;
		$sep = $dateseps[$this->date_sep];
		if ($this->date_format == 0)
			return "m".$sep."d".$sep."Y";
		elseif ($this->date_format == 1)
			return "d".$sep."m".$sep."Y";
		else
			return "Y".$sep."m".$sep."d";
	}

	function tho_sep() 
	{
		return $this->tho_sep;
	}

	function dec_sep() 
	{
		return $this->dec_sep;
	}

	function get_theme() 
	{
		return $this->theme;
	}

	function get_pagesize() 
	{
		return $this->pagesize;
	}

	function show_hints() 
	{
		return $this->show_hints;
	}

	function set_dec($price_dec, $qty_dec, $exrate_dec, $percent_dec, $showgl, $showcodes) 
	{
		$this->price_dec = $price_dec;
		$this->qty_dec = $qty_dec;
		$this->exrate_dec = $exrate_dec;
		$this->percent_dec = $percent_dec;
		$this->show_gl_info = $showgl;
		$this->show_codes = $showcodes;
	}

	function set_format($date_format, $date_sep, $tho_sep, $dec_sep, $theme, $pagesize) 
	{
		$this->date_format = $date_format;
		$this->date_sep = $date_sep;
		$this->tho_sep = $tho_sep;
		$this->dec_sep = $dec_sep;
		$this->theme = $theme;
		$this->pagesize = $pagesize;
	}

}

?>