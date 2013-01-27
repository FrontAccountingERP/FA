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
	var $print_profile;
	var $rep_popup;
	var $pagesize; // for printing
	var $show_hints;
	var $query_size; // table pager page length
	var $graphic_links; // use graphic links
	var $sticky_date;	// save date on subsequent document entry
	var $startup_tab;  // default start-up menu tab
	
	function user_prefs($user=null)
	{
		if ($user == null) { 
			// set default values, used before login
			global $dflt_lang, $dflt_date_sep, $dflt_date_fmt;
			
			$this->date_sep = $dflt_date_sep;
			$this->date_format = $dflt_date_fmt;
			$this->tho_sep = 0;
			$this->dec_sep = 0;
			$this->price_dec = 2;
			$this->language = $dflt_lang;
			$this->theme = 'default';
			
		} else {
			global $path_to_root;
			$this->language = $user["language"];
			$_SESSION['language']->set_language($this->language);

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
			$this->print_profile = $user["print_profile"];
			$this->rep_popup = $user["rep_popup"];
			$this->query_size = $user["query_size"];
			$this->graphic_links = $user["graphic_links"];
			if (isset($user["sticky_doc_date"]))
			{
				$this->sticky_date = $user["sticky_doc_date"];
				$this->startup_tab = $user['startup_tab'];
			}
			else
			{
				$this->sticky_date = 0;
				$this->startup_tab = "orders";
			}
			if (!file_exists("$path_to_root/themes/$this->theme"))
				$this->theme = "default";
		}
	}
	
	function get_all()
	{
			return array(
			'language' => $this->language,
			'qty_dec' => $this->qty_dec,
			'prices_dec' => $this->price_dec,
			'rates_dec' => $this->exrate_dec,
			'percent_dec' => $this->percent_dec,
			'show_gl' => $this->show_gl_info,
			'show_codes' => $this->show_codes,
			'date_format' =>$this->date_format,
			'date_sep' => $this->date_sep,
			'tho_sep' => $this->tho_sep,
			'dec_sep' => $this->dec_sep,
			'theme' => $this->theme,
			'page_size' => $this->pagesize,
			'show_hints' => $this->show_hints,
			'print_profile' => $this->print_profile,
			'rep_popup' => $this->rep_popup,
			'query_size' => $this->query_size,
			'graphic_links' => $this->graphic_links,
			'sticky_doc_date' => $this->sticky_date,
			'startup_tab' => $this->startup_tab);
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
		elseif ($this->date_format == 2)
			return "Y".$sep."m".$sep."d";
		elseif ($this->date_format == 3)
			return "M".$sep."j".$sep."Y";
		elseif ($this->date_format == 4)
			return "j".$sep."M".$sep."Y";
		else
			return "Y".$sep."M".$sep."j";
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

	function print_profile() 
	{
		return $this->print_profile;
	}

	function rep_popup() 
	{
		return $this->rep_popup;
	}

	function query_size() 
	{
		return $this->query_size;
	}

	function graphic_links() 
	{
		return $this->graphic_links;
	}
	
	function sticky_date()
	{
		return $this->sticky_date;
	}
	
	function start_up_tab()
	{
		return $this->startup_tab;
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