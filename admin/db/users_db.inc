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

function add_user($user_id, $real_name, $password, $phone, $email, $full_access, 
	$language, $profile, $rep_popup, $pos)
{
	$sql = "INSERT INTO ".TB_PREF."users (user_id, real_name, password"
		.", phone, email, full_access, language, pos, print_profile, rep_popup)
		VALUES (".db_escape($user_id).", 
		".db_escape($real_name).", ".db_escape($password) .",".db_escape($phone).",
		 ".db_escape($email).", $full_access, ".db_escape($language).",
		 $pos,".db_escape($profile).",$rep_popup)";

	db_query($sql, "could not add user for $user_id");
}

//-----------------------------------------------------------------------------------------------

function update_user_password($id, $user_id, $password)
{
	$sql = "UPDATE ".TB_PREF."users SET password=".db_escape($password) . ",
		user_id = ".db_escape($user_id). " WHERE id=".db_escape($id);

	db_query($sql, "could not update user password for $user_id");
}

//-----------------------------------------------------------------------------------------------

function update_user($id, $user_id, $real_name, $phone, $email, $full_access, 
	$language, $profile, $rep_popup, $pos)
{
	$sql = "UPDATE ".TB_PREF."users SET real_name=".db_escape($real_name).
	", phone=".db_escape($phone).",
		email=".db_escape($email).",
		full_access=$full_access,
		language=".db_escape($language).",
		print_profile=".db_escape($profile).",
		rep_popup=$rep_popup,
		pos=$pos,
		user_id = " . db_escape($user_id)
		. " WHERE id=" . db_escape($id);
	db_query($sql, "could not update user for $user_id");
}

//-----------------------------------------------------------------------------------------------

function update_user_display_prefs($id, $price_dec, $qty_dec, $exrate_dec, 
	$percent_dec, $showgl, $showcodes, $date_format, $date_sep, $tho_sep, 
	$dec_sep, $theme, $pagesize, $show_hints, $profile, $rep_popup, $query_size, 
	$graphic_links,	$lang, $stickydate, $startup_tab)
{
	$sql = "UPDATE ".TB_PREF."users SET
		prices_dec=".db_escape($price_dec).",
		qty_dec=".db_escape($qty_dec).",
		rates_dec=".db_escape($exrate_dec).",
		percent_dec=".db_escape($percent_dec).",
		show_gl=".db_escape($showgl).",
		show_codes=".db_escape($showcodes).",
		date_format=".db_escape($date_format).",
		date_sep=".db_escape($date_sep).",
		tho_sep=".db_escape($tho_sep).",
		dec_sep=".db_escape($dec_sep).",
		theme=".db_escape($theme).",
		page_size=".db_escape($pagesize).",
		show_hints=$show_hints,
		print_profile=".db_escape($profile).",
		rep_popup=$rep_popup,
		query_size=$query_size,
		graphic_links=$graphic_links,
		language=".db_escape($lang).",
		sticky_doc_date=".db_escape($stickydate).",
		startup_tab=".db_escape($startup_tab)."
		WHERE id = ".db_escape($id);

	db_query($sql, "could not update user display prefs for $id");
}

//-----------------------------------------------------------------------------------------------


function get_users($all=false)
{
	$sql = "SELECT * FROM ".TB_PREF."users";
	if (!$all) $sql .= " WHERE !inactive";

	return db_query($sql, "could not get users");
}

//-----------------------------------------------------------------------------------------------

function get_user($id)
{
	$sql = "SELECT * FROM ".TB_PREF."users WHERE id=".db_escape($id);

	$result = db_query($sql, "could not get user $id");

	return db_fetch($result);
}

//-----------------------------------------------------------------------------------------------
//	This function is necessary for admin prefs update after upgrade from 2.1
//
function get_user_by_login($user_id)
{
	$sql = "SELECT * FROM ".TB_PREF."users WHERE user_id=".db_escape($user_id);

	$result = db_query($sql, "could not get user $user_id");

	return db_fetch($result);
}

//-----------------------------------------------------------------------------------------------

function delete_user($id)
{
	$sql="DELETE FROM ".TB_PREF."users WHERE id=".db_escape($id);

	db_query($sql, "could not delete user $id");
}

//-----------------------------------------------------------------------------------------------

function get_user_for_login($user_id, $password)
{
	set_global_connection();

// do not exclude inactive records or you lost access after source upgrade
// on sites using pre 2.2 database
	$sql = "SELECT * FROM ".TB_PREF."users WHERE user_id = '$user_id' AND"
		." password='$password'";

	return db_query($sql, "could not get validate user login for $user_id");
}

//-----------------------------------------------------------------------------------------------

function update_user_visitdate($user_id)
{
	$sql = "UPDATE ".TB_PREF."users SET last_visit_date='". date("Y-m-d H:i:s") ."'
		WHERE user_id=".db_escape($user_id);

	db_query($sql, "could not update last visit date for user $user_id");
}

//-----------------------------------------------------------------------------------------------
function check_user_activity($id) 
{
	$sql = "SELECT COUNT(*) FROM ".TB_PREF."audit_trail WHERE audit_trail.user="
		. db_escape($id);
	$result = db_query($sql,"Cant check user activity");
	$ret = db_fetch($result);

	return $ret[0];
}
?>