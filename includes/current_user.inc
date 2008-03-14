<?php

include_once($path_to_root . "/includes/prefs/userprefs.inc");

//--------------------------------------------------------------------------

class current_user 
{

	var $loginname;
	var $username;
	var	$name;
	var $company;
	var $access;

	var $logged;

	var $prefs;

	function current_user()
	{
		$this->loginname = $username = $this->name = $this->company = "";
		$this->logged = false;

		$this->prefs = null;
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
		$this->set_company($company);

		$Auth_Result = get_user_for_login($loginname, $password);

		if (db_num_rows($Auth_Result) > 0)
		{
            $myrow = db_fetch($Auth_Result);

            $this->access = $myrow["full_access"];
            $this->name = $myrow["real_name"];
            $this->loginname = $loginname;
            $this->username = $this->loginname;
            $this->prefs = new user_prefs($myrow);

			update_user_visitdate($loginname);
			$this->logged = true;
		} 
		else 
		{
			$this->logged = false;
		}

		return $this->logged;
	}

	function check_user_access()
	{
		global $security_groups;
		return is_array($security_groups[$this->access]);
	}

	function can_access_page($page_level)
	{
		global $security_groups;
		return isset($page_level) && in_array($page_level, $security_groups[$this->access]);
	}

	function get_db_connection()
	{
    	global $db_connections;

    	$connection = $db_connections[$this->company];

    	//print_r($connection);

    	$db = mysql_connect($connection["host"] ,
    		$connection["dbuser"], $connection["dbpassword"]);
    	mysql_select_db($connection["dbname"],$db);
		
		if (!defined('TB_PREF'))
			define('TB_PREF', $connection["tbpref"]);
		
    	return $db;
	}

	function update_prefs($price_dec, $qty_dec, $exrate_dec, $percent_dec, $showgl, $showcodes,
		$date_format, $date_sep, $tho_sep, $dec_sep, $theme, $pagesize) {
		update_user_display_prefs($this->username, $price_dec, $qty_dec, $exrate_dec, $percent_dec, $showgl,
			$showcodes, $date_format, $date_sep, $tho_sep, $dec_sep, $theme, $pagesize);

		// re-read the prefs
		$user = get_user($this->username);
		$this->prefs = new user_prefs($user);
	}
}

//--------------------------------------------------------------------------

function number_format2($number, $decimals=0) 
{
	global $thoseps, $decseps;
	$tsep = $thoseps[$_SESSION["wa_current_user"]->prefs->tho_sep()];
	$dsep = $decseps[$_SESSION["wa_current_user"]->prefs->dec_sep()];
	return number_format($number, $decimals, $dsep,	$tsep);
}

function price_format($number) {
    return number_format2($number, 
	$_SESSION["wa_current_user"]->prefs->price_dec());
}

function qty_format($number) {
    return number_format2($number, 
	$_SESSION["wa_current_user"]->prefs->qty_dec());
}

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
    if($sep!='') $num = str_replace( $sep, '', $num);
	str_replace($sep, '', $num);
    $sep = $decseps[user_dec_sep()];
    if($sep!='.') $num = str_replace( $sep, '.', $num);
    
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

function set_user_prefs($price_dec, $qty_dec, $exrate_dec, $percent_dec, $showgl, $showcodes,
	$date_format, $date_sep, $tho_sep, $dec_sep, $theme, $pagesize) 
{

	$_SESSION["wa_current_user"]->update_prefs($price_dec, $qty_dec, $exrate_dec, $percent_dec, $showgl, $showcodes,
		$date_format, $date_sep, $tho_sep, $dec_sep, $theme, $pagesize);
}

//--------------------------------------------------------------------------

?>