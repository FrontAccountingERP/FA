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
include_once($path_to_root . "/includes/prefs/userprefs.inc");

//--------------------------------------------------------------------------

class current_user
{
	var $user;
	var $loginname;
	var $username;
	var $name;
	var $company;
	var $pos;
	var $access;
	var $timeout;
	var $last_act;
	var $role_set = false;
	var $old_db;	
	var $logged;
	var $ui_mode = 0;
	
	var $prefs;

	function current_user()
	{
		global $def_coy;
		
		$this->loginname = $this->username = $this->name = "";
		$this->company = isset($def_coy)? $def_coy : 0;
		$this->logged = false;

		$this->prefs = new user_prefs();
	}

	function logged_in()
	{
		return $this->logged;
	}

	function set_company($company)
	{
		$this->company = $company;
	}

	function login($company, $loginname, $password)
	{
		global $security_areas, $security_groups, $security_headings, $path_to_root;
		
		$this->set_company($company);
	    $this->logged = false;

		$Auth_Result = get_user_for_login($loginname, $password);

		if (db_num_rows($Auth_Result) > 0)
		{
			$myrow = db_fetch($Auth_Result);
			$this->old_db = isset($myrow["full_access"]);
			if (! @$myrow["inactive"]) {
				if ($this->old_db) { 
					// Transition code:
					// db was not yet upgraded after source update to v.2.2
					// give enough access for admin user to continue upgrade
					if (!isset($security_groups) || !isset($security_headings)) {
						echo "<center><br><br><font size='5' color='red'><b>";
						echo _('Before software upgrade you have to include old $security_groups and $security_headings arrays from old config.php file to the new one.');
						echo '<br>'."<a href=$path_to_root/index.php>"._("Back")."</a>";
						echo "</b></font><br><br></center>";
						exit;
					}
	        	    $this->access = $myrow["full_access"];
	        	    if (in_array(20, $security_groups[$this->access]))
						// temporary access for admin users
						$this->role_set[] = $security_areas['SA_SOFTWAREUPGRADE'][0];
					else {
						echo "<center><br><br><font size='5' color='red'><b>";
						echo _('System is available for site admin only until full database upgrade');
						echo "</b></font><br><br></center>";
						exit;
					}
	        	} else {
					$this->role_set = array();
	        	    $this->access = $myrow["role_id"];
					// store area codes available for current user role
					$role = get_security_role($this->access);
					if (!$role) 
						return false;
					foreach( $role['areas'] as $code )
						// filter only area codes for enabled security sections
						if (in_array($code&~0xff, $role['sections'])) 
							$this->role_set[] = $code;
	        	}
        	    $this->name = $myrow["real_name"];
        	    $this->pos = $myrow["pos"];
        	    $this->loginname = $loginname;
        	    $this->username = $this->loginname;
        	    $this->prefs = new user_prefs($myrow);
        	    $this->user = @$myrow["id"];
		    	update_user_visitdate($this->username);
		    	$this->logged = true;
				$this->last_act = time();
				$this->timeout = session_timeout();
			}
		}
		return $this->logged;
	}

	function check_user_access()
	{
		global $security_groups;
		if ($this->old_db) {
			// notification after upgrade from pre-2.2 version
			return isset($security_groups) && is_array(@$security_groups[$this->access]);
		} else
			return !isset($security_groups) && is_array($this->role_set);
	}

	function can_access($page_level)
	{
		global $security_groups, $security_areas;
		if (isset($security_groups)) {
			return $this->company == 0 &&
				in_array(20, $security_groups[$this->access]);
		}

		if ($page_level === 'SA_OPEN') 
			return true;
		if ($page_level === 'SA_DENIED' || $page_level === '') 
			return false;

		$code = $security_areas[$page_level][0];

		// only first registered company has site admin privileges
		return $code && in_array($code, $this->role_set)
			&& ($this->company == 0 || (($code&~0xff) != SS_SADMIN));
	}

	function can_access_page($page_level)
	{
		return $this->can_access($page_level);
	}

	function get_db_connection($id=-1)
	{
    	global $db_connections;

    	$connection = $db_connections[$id == -1 ? $this->company : $id];

    	//print_r($connection);

    	$db = mysql_connect($connection["host"] ,
    		$connection["dbuser"], $connection["dbpassword"]);
    	mysql_select_db($connection["dbname"],$db);

		if (!defined('TB_PREF'))
			define('TB_PREF', $connection["tbpref"]);

    	return $db;
	}

	function update_prefs($price_dec, $qty_dec, $exrate_dec, $percent_dec, 
		$showgl, $showcodes, $date_format, $date_sep, $tho_sep, $dec_sep, 
		$theme, $pagesize, $show_hints, $profile, $rep_popup, $query_size, 
		$graphic_links, $lang, $stickydate, $startup_tab)
	{
		global $allow_demo_mode;

		$user = array(
			'prices_dec' => $price_dec, 	'qty_dec' => $qty_dec,
			'rates_dec' => $exrate_dec,		'percent_dec' => $percent_dec,
			'show_gl' => $showgl,			'show_codes' => $showcodes,
			'date_format' => $date_format,	'date_sep' => $date_sep,
			'tho_sep' => $tho_sep,			'dec_sep' => $dec_sep,
			'theme' => $theme,				'page_size' => $pagesize,
			'show_hints' => $show_hints,	'print_profile' => $profile,
			'rep_popup' => $rep_popup,		'query_size' => $query_size,
			'graphic_links' => $graphic_links,	'language' => $lang,
			'sticky_doc_date' => $stickydate,	'startup_tab' => $startup_tab
		);

		if(!$allow_demo_mode) {
			update_user_display_prefs($this->user, $price_dec, 
				$qty_dec, $exrate_dec, $percent_dec, $showgl, $showcodes, 
				$date_format, $date_sep, $tho_sep, $dec_sep, $theme, $pagesize, 
				$show_hints, $profile, $rep_popup, $query_size, $graphic_links, $lang, $stickydate, $startup_tab);
		}
		$this->prefs = new user_prefs(get_user($this->user));
	}
}

//--------------------------------------------------------------------------

function round2($number, $decimals=0)
{
	$delta = ($number < 0 ? -.0000000001 : .0000000001);
	return round($number+$delta, $decimals);
}

function number_format2($number, $decimals=0)
{
	global $thoseps, $decseps;
	$tsep = $thoseps[$_SESSION["wa_current_user"]->prefs->tho_sep()];
	$dsep = $decseps[$_SESSION["wa_current_user"]->prefs->dec_sep()];
	//return number_format($number, $decimals, $dsep,	$tsep);
	$delta = ($number < 0 ? -.0000000001 : .0000000001);
	return number_format($number+$delta, $decimals, $dsep,	$tsep);
}
//
//	Current ui mode.
//
function fallback_mode() {
    return $_SESSION["wa_current_user"]->ui_mode==0;
}

function price_format($number) {
    return number_format2($number,
	$_SESSION["wa_current_user"]->prefs->price_dec());
}

function price_decimal_format($number, &$dec)
{
	$dec = user_price_dec();
	$str = strval($number);
	$pos = strpos($str, '.');
	if ($pos !== false)
	{
		$len = strlen(substr($str, $pos + 1));
		if ($len > $dec)
			$dec = $len;
	}
	return number_format2($number, $dec);
}
// function money_format doesn't exist in OS Win.
if (!function_exists('money_format'))
{
	function money_format($format, $number) 
	{
		return price_format($number);
	} 
}	

// 2008-06-15. Added extra parameter $stock_id and reference for $dec
//--------------------------------------------------------------------
function qty_format($number, $stock_id=null, &$dec) {
	$dec = get_qty_dec($stock_id);
    return number_format2($number, $dec);
}
// and get_qty_dec
function get_qty_dec($stock_id=null)
{
	global $path_to_root;
	include_once($path_to_root."/inventory/includes/db/items_units_db.inc");
	if ($stock_id != null)
		$dec = get_unit_dec($stock_id);
	if ($stock_id == null || $dec == -1 || $dec == null)
		$dec = $_SESSION["wa_current_user"]->prefs->qty_dec();
	return $dec;
}
//-------------------------------------------------------------------
function exrate_format($number) {
    return number_format2($number,
	$_SESSION["wa_current_user"]->prefs->exrate_dec());
}

function percent_format($number) {
    return number_format2($number,
	$_SESSION["wa_current_user"]->prefs->percent_dec());
}

function user_numeric($input) {
    global $decseps, $thoseps;

    $num = trim($input);
    $sep = $thoseps[user_tho_sep()];
    if ($sep!='')
    	$num = str_replace( $sep, '', $num);

    $sep = $decseps[user_dec_sep()];
    if ($sep!='.')
    	$num = str_replace( $sep, '.', $num);

    if (!is_numeric($num))
	  return false;
    $num = (float)$num;
    if ($num == (int)$num)
	  return (int)$num;
    else
	  return $num;
}

function user_company()
{
	return $_SESSION["wa_current_user"]->company;
}

function user_pos()
{
	return $_SESSION["wa_current_user"]->pos;
}

function user_language()
{
	return $_SESSION["wa_current_user"]->prefs->language();
}

function user_qty_dec()
{
	return $_SESSION["wa_current_user"]->prefs->qty_dec();
}

function user_price_dec()
{
	return $_SESSION["wa_current_user"]->prefs->price_dec();
}

function user_exrate_dec()
{
	return $_SESSION["wa_current_user"]->prefs->exrate_dec();
}

function user_percent_dec()
{
	return $_SESSION["wa_current_user"]->prefs->percent_dec();
}

function user_show_gl_info()
{
	return $_SESSION["wa_current_user"]->prefs->show_gl_info();
}

function user_show_codes()
{
	return $_SESSION["wa_current_user"]->prefs->show_codes();
}

function user_date_format()
{
	return $_SESSION["wa_current_user"]->prefs->date_format();
}

function user_date_display()
{
	return $_SESSION["wa_current_user"]->prefs->date_display();
}

function user_date_sep()
{
	return $_SESSION["wa_current_user"]->prefs->date_sep();
}

function user_tho_sep()
{
	return $_SESSION["wa_current_user"]->prefs->tho_sep();
}

function user_dec_sep()
{
	return $_SESSION["wa_current_user"]->prefs->dec_sep();
}

function user_theme()
{
	return $_SESSION["wa_current_user"]->prefs->get_theme();
}

function user_pagesize()
{
	return $_SESSION["wa_current_user"]->prefs->get_pagesize();
}

function user_hints()
{
	return $_SESSION["wa_current_user"]->prefs->show_hints();
}

function user_print_profile()
{
	return $_SESSION["wa_current_user"]->prefs->print_profile();
}

function user_rep_popup()
{
	return $_SESSION["wa_current_user"]->prefs->rep_popup();
}

function user_query_size()
{
	return $_SESSION["wa_current_user"]->prefs->query_size();
}

function user_graphic_links()
{
	return $_SESSION["wa_current_user"]->prefs->graphic_links();
}

function sticky_doc_date()
{
	return $_SESSION["wa_current_user"]->prefs->sticky_date();
}

function user_startup_tab()
{
	return $_SESSION["wa_current_user"]->prefs->start_up_tab();
}

function set_user_prefs($price_dec, $qty_dec, $exrate_dec, $percent_dec, $showgl, $showcodes,
	$date_format, $date_sep, $tho_sep, $dec_sep, $theme, $pagesize, $show_hints,
	$print_profile, $rep_popup, $query_size, $graphic_links, $lang, $stickydate, $startup_tab)
{

	$_SESSION["wa_current_user"]->update_prefs($price_dec, $qty_dec, $exrate_dec, $percent_dec, $showgl, $showcodes,
		$date_format, $date_sep, $tho_sep, $dec_sep, $theme, $pagesize, $show_hints,
		$print_profile, $rep_popup, $query_size, $graphic_links, $lang, $stickydate, $startup_tab);
}

function add_user_js_data() {
	global $path_to_root, $thoseps, $decseps, $date_system, $dateseps;

	$ts = $thoseps[user_tho_sep()];
	$ds = $decseps[user_dec_sep()];

    $js = "\n"
	  . "var user = {\n"
	  . "theme: '". $path_to_root . '/themes/'. user_theme().'/'."',\n"
	  . "loadtxt: '"._('Requesting data...')."',\n"
	  . "date: '".Today()."',\n"	// server date
	  . "datesys: ".$date_system.",\n"
	  . "datefmt: ".user_date_format().",\n"
	  . "datesep: '".$dateseps[user_date_sep()]."',\n"
	  . "ts: '$ts',\n"
	  . "ds: '$ds',\n"
	  . "pdec : " . user_price_dec() . "}\n";

  add_js_source($js);
}

//--------------------------------------------------------------------------

function session_timeout()
{
	$tout = @get_company_pref('login_tout'); // mask warning for db ver. 2.2
	return $tout ? $tout : ini_get('session.gc_maxlifetime');
}

//-----------------------------------------------------------------------------
//	Inserts $elements into $array at position $index.
//	$elements is list of any objects
//
function array_insert(&$array, $index, $elements)
{
	if (!is_array($elements)) $elements = array($elements);

	$head  = array_splice($array, 0, $index);
	$array = array_merge($head, $elements, $array);
}

function array_remove(&$array, $index, $len=1)
{
	array_splice($array, $index, $len);
}

function array_substitute(&$array, $index, $len, $elements)
{
	array_splice($array, $index, $len);
	array_insert($array, $index, $elements);
}

function array_append(&$array, $elements)
{
	foreach($elements as $key => $el) {
		if(is_int($key))
			$array[] = $el;
		else
			$array[$key] = $el;
	}
}

function array_search_value($needle, $haystack, $valuekey=null)
{
	foreach($haystack as $key => $value) {
		$val = isset($valuekey) ? $value[$valuekey] : $value;
		if ($needle == $val){
			return $value;
		}
	}
	return null;
}

function array_search_key($needle, $haystack, $valuekey=null)
{
	foreach($haystack as $key => $value) {
		$val = isset($valuekey) ? $value[$valuekey] : $value;
		if ($needle == $val){
			return $key;
		}
	}
	return null;
}
?>