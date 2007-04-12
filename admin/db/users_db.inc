<?php

function add_user($user_id, $real_name, $password, $phone, $email, $full_access, $language)
{
	$sql = "INSERT INTO ".TB_PREF."users (user_id, real_name, password, phone, email, full_access, language)
		VALUES ('$user_id', '$real_name', '" . $password ."', '$phone', '$email', $full_access, '$language')";

	db_query($sql, "could not add user for $user_id");
}

//-----------------------------------------------------------------------------------------------

function update_user_password($user_id, $password)
{
	$sql = "UPDATE ".TB_PREF."users SET password='" . $password . "'
		WHERE user_id = '$user_id'";

	db_query($sql, "could not update user password for $user_id");
}

//-----------------------------------------------------------------------------------------------

function update_user($user_id, $real_name, $phone, $email, $full_access, $language)
{
	$sql = "UPDATE ".TB_PREF."users SET real_name='$real_name', phone='$phone',
		email='$email',
		full_access=$full_access,
		language='$language'
		WHERE user_id = '$user_id'";

	db_query($sql, "could not update user for $user_id");
}

//-----------------------------------------------------------------------------------------------

function update_user_display_prefs($user_id, $price_dec, $qty_dec, $exrate_dec, $percent_dec, $showgl,
	$showcodes, $date_format, $date_sep, $tho_sep, $dec_sep, $theme, $pagesize)
{
	$sql = "UPDATE ".TB_PREF."users SET
		prices_dec=$price_dec,
		qty_dec=$qty_dec,
		rates_dec=$exrate_dec,
		percent_dec=$percent_dec,
		show_gl=$showgl,
		show_codes=$showcodes,
		date_format=$date_format,
		date_sep=$date_sep,
		tho_sep=$tho_sep,
		dec_sep=$dec_sep,
		theme='$theme',
		page_size='$pagesize'
		WHERE user_id = '$user_id'";

	db_query($sql, "could not update user display prefs for $user_id");
}

//-----------------------------------------------------------------------------------------------


function get_users()
{
	$sql = "SELECT * FROM ".TB_PREF."users";

	return db_query($sql, "could not get users");
}

//-----------------------------------------------------------------------------------------------

function get_user($user_id)
{
	$sql = "SELECT * FROM ".TB_PREF."users WHERE user_id = '$user_id'";

	$result = db_query($sql, "could not get user for $user_id");

	return db_fetch($result);
}

//-----------------------------------------------------------------------------------------------

function delete_user($user_id)
{
	$sql="DELETE FROM ".TB_PREF."users WHERE user_id='$user_id'";

	db_query($sql, "could not delete user $user_id");
}

//-----------------------------------------------------------------------------------------------

function get_user_for_login($user_id, $password)
{
	set_global_connection();

	$sql = "SELECT * FROM ".TB_PREF."users WHERE user_id = '$user_id' AND password='$password'";

	return db_query($sql, "could not get validate user login for $user_id");
}

//-----------------------------------------------------------------------------------------------

function update_user_visitdate($user_id)
{
	$sql = "UPDATE ".TB_PREF."users SET last_visit_date='". date("Y-m-d H:i:s") ."'
		WHERE user_id='$user_id'";

	db_query($sql, "could not update last visit date for user $user_id");
}

//-----------------------------------------------------------------------------------------------

?>