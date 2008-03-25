<?php

$page_security =10;
$path_to_root="..";
include($path_to_root . "/includes/session.inc");

page(_("Display Setup"));

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");

include_once($path_to_root . "/admin/db/company_db.inc");

//-------------------------------------------------------------------------------------------------

if (isset($_POST['setprefs'])) 
{
	$theme = user_theme();
	set_user_prefs($_POST['prices'], $_POST['Quantities'],
		$_POST['Rates'], $_POST['Percent'],
		check_value('show_gl'),
		check_value('show_codes'),
		$_POST['date_format'], $_POST['date_sep'],
		$_POST['tho_sep'], $_POST['dec_sep'],
		$_POST['theme'], $_POST['page_size']);

	language::set_language($_POST['language']);

	flush_dir($comp_path.'/'.user_company().'/js_cache');	

	if (user_theme() != $theme)
		reload_page("");

	display_notification_centered(_("Display settings have been updated."));
}

start_form();
start_table($table_style2);

table_section_title(_("Decimal Places"));

text_row_ex(_("Prices/Amounts:"), 'prices', 5, 5, user_price_dec());
text_row_ex(_("Quantities:"), 'Quantities', 5, 5, user_qty_dec());
text_row_ex(_("Exchange Rates:"), 'Rates', 5, 5, user_exrate_dec());
text_row_ex(_("Percentages:"), 'Percent',  5, 5, user_percent_dec());

table_section_title(_("Dateformat and Separators"));

dateformats_list_row(_("Dateformat:"), "date_format", user_date_format());

dateseps_list_row(_("Date Separator:"), "date_sep", user_date_sep());

/* The array $dateseps is set up in config.php for modifications
possible separators can be added by modifying the array definition by editing that file */

thoseps_list_row(_("Thousand Separator:"), "tho_sep", user_tho_sep());

/* The array $thoseps is set up in config.php for modifications
possible separators can be added by modifying the array definition by editing that file */

decseps_list_row(_("Decimal Separator:"), "dec_sep", user_dec_sep());

/* The array $decseps is set up in config.php for modifications
possible separators can be added by modifying the array definition by editing that file */

table_section_title(_("Miscellaneous"));

check_row(_("Show GL Information:"), 'show_gl', user_show_gl_info());

check_row(_("Show Item Codes:"), 'show_codes', user_show_codes());

themes_list_row(_("Theme:"), "theme", user_theme());

/* The array $themes is set up in config.php for modifications
possible separators can be added by modifying the array definition by editing that file */

pagesizes_list_row(_("Page Size:"), "page_size", user_pagesize());

/* The array $pagesizes is set up in config.php for modifications
possible separators can be added by modifying the array definition by editing that file */

table_section_title(_("Language"));

if (!isset($_POST['language']))
	$_POST['language'] = $_SESSION['language']->code;

languages_list_row(_("Language:"), 'language', $_POST['language']);

end_table(1);

submit_center('setprefs', _("Update"));

end_form(2);

//-------------------------------------------------------------------------------------------------

end_page();

?>