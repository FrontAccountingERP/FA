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
//$path_to_root="..";


if (function_exists('mysqli_connect'))
	include_once("connect_db_mysqli.inc");
else
	include_once("connect_db_mysql.inc"); // use deprecated mysql_* API as fallback on old servers

function db_get_version()
{
	$result = db_query("SELECT VERSION()");
	$row = db_fetch($result);
	return $row[0];
}

/*
	Converts encoding name to mysql standard.
*/
function get_mysql_encoding_name($encoding)
{
	$db_encoding = array(
		'UTF-8' => 'utf8',
		'ISO-8859-1' => 'latin1',
		'ISO-8859-2' => 'latin2',
		'ISO-8859-7' => 'greek',
		'ISO-8859-8' => 'hebrew',
		'ISO-8859-9' => 'latin5',
		'ISO-8859-13' => 'latin7',
		'KOI8-R' => 'koi8r',
		'KOI8-U' => 'koi8u',
		'CP850' => 'cp850',
		'CP866' => 'cp866',
		'CP932' => 'cp932',
		'CP1250' => 'cp1250',
		'CP1251' => 'cp1251',
		'CP1252' => 'latin1',
		'CP1256' => 'cp1256',
		'CP1257' => 'cp1257',
		'GB2312' => 'gb2312',
		'EUC-JP' => 'ujis',
		'EUC-KR' => 'euckr',
		'BIG5' => 'big5',
		'GBK' => 'gbk',
		'SHIFT_JIS' => 'sjis',
		'TIS-620' => 'tis620',
		'ASCII' => 'ascii',
	);
	$encoding = strtoupper($encoding);

	return isset($db_encoding[$encoding]) ? $db_encoding[$encoding] : null;
}

/*
	Returns 'best' collation for various locale language codes
*/
function get_mysql_collation($lang=null)
{
	if (!$lang)
		$lang = substr($_SESSION['language']->code, 0, 2);

	$db_collation = array(
		'is' => 'icelandic',
		'lv' => 'latvian',
		'ro' => 'romanian',
		'sl' => 'slovenian',
		'pl' => 'polish',
		'et' => 'estonian',
		'es' => 'spanish', // or 'spanish2',
		'sw' => 'swedish',
		'tr' => 'turkish',
		'cs' => 'czech',
		'da' => 'danish',
		'lt' => 'lithuanian',
		'sk' => 'slovak',
		'sp' => 'spanish2',
		'fa' => 'persian',
		'hu' => 'hungarian',
		'fr' => 'roman',
		'it' => 'roman',
	);

	return 'utf8_'.(isset($db_collation[$lang]) ? $db_collation[$lang] : 'general').'_ci';
}

/*
	Later we assume that database with version less than 2.4 is old database, 
	which is subject to invalid encodings on text columns,
	so no SET NAMES or equivalent should be used.
*/
function db_fixed()
{
	$result = db_query("SELECT value FROM ".TB_PREF."sys_prefs WHERE name='version_id'");
	$data = db_fetch($result);
	return !db_num_rows($result) // new database is fixed by default 
		|| ($data[0] > "2.3rc");
}

/*
	Check database default charset.
*/
function db_get_default_charset()
{
	$result = db_query("SELECT @@character_set_database");
	$var = db_fetch($result);
	return $var[0];
}

	
?>