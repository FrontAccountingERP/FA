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
	if (!isset($path_to_root) || isset($_GET['path_to_root']) || isset($_POST['path_to_root']))
		die(_("Restricted access"));
	include_once($path_to_root . "/includes/ui.inc");
	include_once($path_to_root . "/includes/page/header.inc");

	$js = "<script language='JavaScript' type='text/javascript'>
function defaultCompany()
{
	// [CUSTOM] default company is carried by the hidden company_login_name field;
	// company selection is now a single search+select input (no <select> dropdown).
}
</script>";

	add_js_file('login.js');
	// Display demo user name and password within login form if allow_demo_mode option is true
	if ($SysPrefs->allow_demo_mode == true)
	{
	    $demo_text = _("Login as user: demouser and password: password");
	}
	else
	{
		$demo_text = _("Please login here");
    	if (@$SysPrefs->allow_password_reset) {
      		$demo_text .= " "._("or")." <a href='$path_to_root/index.php?reset=1'>"._("request new password")."</a>";
    	}
	}

	if (check_faillog())
	{
		$blocked = true;

	    $js .= "<script>setTimeout(function() {
	    	document.getElementsByName('SubmitUser')[0].disabled=0;
	    	document.getElementById('log_msg').innerHTML='$demo_text'}, 1000*".$SysPrefs->login_delay.");</script>";
	    $demo_text = '<span class="redfg">'._('Too many failed login attempts.<br>Please wait a while or try later.').'</span>';
	} elseif ($_SESSION["wa_current_user"]->login_attempt > 1) {
		$demo_text = '<span class="redfg">'._("Invalid password or username. Please, try again.").'</span>';
	}

	flush_dir(user_js_cache());
	if (!isset($def_coy))
		$def_coy = 0;
	$def_theme = "default";

	$login_timeout = $_SESSION["wa_current_user"]->last_act;

	$title = $login_timeout ? _('Authorization timeout') : $SysPrefs->app_title." ".$version." - "._("Login");
	$encoding = isset($_SESSION['language']->encoding) ? $_SESSION['language']->encoding : "iso-8859-1";
	$rtl = isset($_SESSION['language']->dir) ? $_SESSION['language']->dir : "ltr";
	$onload = !$login_timeout ? "onload='defaultCompany()'" : "";

	echo "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n";
	echo "<html dir='$rtl' >\n";
	echo "<head profile=\"http://www.w3.org/2005/10/profile\"><title>$title</title>\n";
   	echo "<meta http-equiv='Content-type' content='text/html; charset=$encoding' >\n";
	echo "<link href='$path_to_root/themes/$def_theme/default.css' rel='stylesheet' type='text/css'> \n";
 	echo "<link href='$path_to_root/themes/default/images/favicon.ico' rel='icon' type='image/x-icon'> \n";
	send_scripts();
	if (!$login_timeout)
	{
		echo $js;
	}

	echo "</head>\n";

	echo "<body id='loginscreen' $onload>\n";

	echo "<table class='titletext'><tr><td>$title</td></tr></table>\n";
	
	div_start('_page_body');
	br();br();
	start_form(false, false, $_SESSION['timeout']['uri'], "loginform");
	start_table(false, "class='login'");

	start_row();
	echo "<td align='center' colspan=2>";
	if (!$login_timeout) { // FA logo
    	echo "<a target='_blank' href='".$SysPrefs->power_url."'><img src='$path_to_root/themes/$def_theme/images/logo_frontaccounting.png' alt='FrontAccounting' height='50' onload='fixPNG(this)' border='0' ></a>";
	} else { 
		echo "<font size=5>"._('Authorization timeout')."</font>";
	} 
	echo "</td>\n";
	end_row();
	if (!$login_timeout)
		table_section_title(_("Version")." $version   Build ".$SysPrefs->build_version." - "._("Login"));

	$value = $login_timeout ? $_SESSION['wa_current_user']->loginname : ($SysPrefs->allow_demo_mode ? "demouser":"");

	$allow = SECURE_ONLY !== true ? true : (!empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] !== 'off') || $_SERVER['SERVER_NAME'] === "localhost";

	if ($allow) {

		text_row(_("User name"), "user_name_entry_field", $value, 20, 30);

		$password = $SysPrefs->allow_demo_mode ? "password":"";

		password_row(_("Password:"), 'password', $password);

		if ($login_timeout) {
			hidden('company_login_name', user_company());
		} else {
			$coy =  user_company();
			if (!isset($coy))
				$coy = $def_coy;
			if (isset($db_connections)) {
				if (!@$SysPrefs->text_company_selection) {
					// [CUSTOM] single search+select company control (input + datalist),
					// resolving to the company index in a hidden field.
					// [CUSTOM] the company field starts EMPTY on every visit: no default
					// company is pre-selected or pre-typed, so the operator always makes
					// an explicit choice (the fleet has 135 companies and the previous
					// company was easy to submit by accident).
					$def_name = '';
					echo "<tr><td>"._("Company")."</td><td>";
					echo "<input type='text' id='company_search' list='company_list' autocomplete='off' "
						."value='' "
						."placeholder='"._("Type to search & select...")."' "
						."style='width:100%' oninput='syncCompany()' onchange='syncCompany(true)' />";
					echo "<datalist id='company_list'>\n";
					for ($i = 0; $i < count($db_connections); $i++)
						echo "<option value='".$db_connections[$i]["name"]."' data-idx='$i'>";
					echo "</datalist>\n";
					echo "<input type='hidden' name='company_login_name' id='company_login_name' value='' />";
					echo "</td></tr>";
					echo "<script>
					function syncCompany(commit){
						var el = document.getElementById('company_search');
						var hid = document.getElementById('company_login_name');
						if (!el || !hid) return true;
						var v = el.value.replace(/^\\s+|\\s+$/g,'').toLowerCase();
						var opts = document.getElementById('company_list').options;
						// empty box = nothing selected (never silently fall back to
						// a default company)
						if (v === '') { hid.value = ''; return false; }
						var i, hits = 0, hitidx = '', hitval = '';
						for (i=0;i<opts.length;i++){
							if (opts[i].value.toLowerCase() === v){
								hid.value = opts[i].getAttribute('data-idx');
								return true;
							}
						}
						// no exact match: accept an unambiguous partial (substring)
						// match so typing a few letters is enough. The visible text is
						// only rewritten to the full name on commit (blur/login), never
						// on every keystroke, which would fight the typist.
						for (i=0;i<opts.length;i++){
							if (opts[i].value.toLowerCase().indexOf(v) !== -1){
								hits++;
								hitidx = opts[i].getAttribute('data-idx');
								hitval = opts[i].value;
							}
						}
						if (hits === 1) {
							hid.value = hitidx;
							if (commit) el.value = hitval;
							return true;
						}
						// no (or ambiguous) match: clear the index so a stale company is
						// never silently submitted instead of what was typed
						hid.value = '';
						return false;
					}
					function checkCompany(){
						var el = document.getElementById('company_search');
						if (!el) return true;
						if (syncCompany(true)) return true;
						alert('".addslashes(_("Please select a company from the list."))."');
						el.focus();
						return false;
					}
					</script>";
				} else {
					text_row(_("Company"), "company_login_nickname", "", 20, 50);
				}
			}
		}
	}
 	else {
		$demo_text = '<span class="redfg">'._("HTTP access is not allowed on this site. This is unsecure. If you really want to access this unsecure site then set the SECURE_ONLY to false in /includes/session.inc file.").'</span>';
	}
	start_row();
	label_cell($demo_text, "colspan=2 align='center' id='log_msg'");
	end_row();
	end_table(1);
	echo "<input type='hidden' id=ui_mode name='ui_mode' value='".!fallback_mode()."' >\n";
	if ($allow) {
		$onclick = in_ajax() ? 'retry();' : 'set_fullmode();';
		// validate the company search control before the form is submitted
		$onclick = 'if(typeof checkCompany==="function" && !checkCompany()) return false; '.$onclick;
		echo "<center><input type='submit' value='&nbsp;&nbsp;"._("Login -->")."&nbsp;&nbsp;' name='SubmitUser'"
			." onclick='$onclick'".(isset($blocked) ? " disabled" : '')." ></center>\n";
	}		
	foreach($_SESSION['timeout']['post'] as $p => $val) {
		// add all request variables to be resend together with login data
		if (!in_array($p, array('ui_mode', 'user_name_entry_field', 
			'password', 'SubmitUser', 'company_login_name'))) 
			if (!is_array($val))
				echo "<input type='hidden' name='$p' value='$val'>";
			else
				foreach($val as $i => $v)
					echo "<input type='hidden' name='{$p}[$i]' value='$v'>";
	}
	end_form(1);
	$Ajax->addScript(true, "if (document.forms.length) document.forms[0].password.focus();");

    echo "<script language='JavaScript' type='text/javascript'>
    //<![CDATA[
            <!--
            document.forms[0].user_name_entry_field.select();
            document.forms[0].user_name_entry_field.focus();
            //-->
    //]]>
    </script>";
    div_end();
	echo "<table class='bottomBar'>\n";
	echo "<tr>";
	if (isset($_SESSION['wa_current_user'])) 
		$date = Today() . " | " . Now();
	else	
		$date = date("m/d/Y") . " | " . date("h.i am");
	echo "<td class='bottomBarCell'>$date</td>\n";
	echo "</tr></table>\n";
	echo "<table class='footer'>\n";
	echo "<tr>\n";
	echo "<td><a target='_blank' href='".$SysPrefs->power_url."' tabindex='-1'>".$SysPrefs->app_title." $version - " . _("Theme:") . " " . $def_theme . "</a></td>\n";
	echo "</tr>\n";
	echo "<tr>\n";
	echo "<td><a target='_blank' href='".$SysPrefs->power_url."' tabindex='-1'>".$SysPrefs->power_by."</a></td>\n";
	echo "</tr>\n";
	echo "</table><br><br>\n";
	echo "</body></html>\n";

