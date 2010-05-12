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
$page_security = 'SA_SETUPDISPLAY';
$path_to_root="..";
include($path_to_root . "/includes/session.inc");

page(_($help_context = "Display Setup"));

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");

include_once($path_to_root . "/admin/db/company_db.inc");

//-------------------------------------------------------------------------------------------------

if (isset($_POST['setprefs'])) 
{
	if (!is_numeric($_POST['query_size']) || ($_POST['query_size']<1))
	{
		display_error($_POST['query_size']);
		display_error( _("Query size must be integer and greater than zero."));
		set_focus('query_size');
	} else {
		$chg_theme = user_theme() != $_POST['theme'];
		$chg_lang = $_SESSION['language']->code != $_POST['language'];

		set_user_prefs($_POST['prices'], $_POST['Quantities'],
			$_POST['Rates'], $_POST['Percent'],
			check_value('show_gl'),
			check_value('show_codes'),
			$_POST['date_format'], $_POST['date_sep'],
			$_POST['tho_sep'], $_POST['dec_sep'],
			$_POST['theme'], $_POST['page_size'], check_value('show_hints'),
			$_POST['profile'], check_value('rep_popup'), 
			(int)($_POST['query_size']), check_value('graphic_links'), 
			$_POST['language'], check_value('sticky_doc_date'), $_POST['startup_tab']);

		if ($chg_lang)
			$_SESSION['language']->set_language($_POST['language']);
			// refresh main menu

		flush_dir($comp_path.'/'.user_company().'/js_cache');	

		if ($chg_theme && $allow_demo_mode)
			$_SESSION["wa_current_user"]->prefs->theme = $_POST['theme'];

		if ($chg_theme || $chg_lang)
			meta_forward($_SERVER['PHP_SELF']);

		
		if ($allow_demo_mode)  
			display_warning(_("Display settings have been updated. Keep in mind that changed settings are restored on every login in demo mode."));
		else
			display_notification_centered(_("Display settings have been updated."));
	}
}

start_form();

start_outer_table($table_style2);

table_section(1);
table_section_title(_("Decimal Places"));

text_row_ex(_("Prices/Amounts:"), 'prices', 5, 5, '', user_price_dec());
text_row_ex(_("Quantities:"), 'Quantities', 5, 5, '', user_qty_dec());
text_row_ex(_("Exchange Rates:"), 'Rates', 5, 5, '', user_exrate_dec());
text_row_ex(_("Percentages:"), 'Percent',  5, 5, '', user_percent_dec());

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
if (!isset($_POST['language']))
	$_POST['language'] = $_SESSION['language']->code;

table_section_title(_("Language"));

languages_list_row(_("Language:"), 'language', $_POST['language']);

table_section(2);
table_section_title(_("Miscellaneous"));

check_row(_("Show hints for new users:"), 'show_hints', user_hints());

check_row(_("Show GL Information:"), 'show_gl', user_show_gl_info());

check_row(_("Show Item Codes:"), 'show_codes', user_show_codes());

themes_list_row(_("Theme:"), "theme", user_theme());

/* The array $themes is set up in config.php for modifications
possible separators can be added by modifying the array definition by editing that file */

pagesizes_list_row(_("Page Size:"), "page_size", user_pagesize());

tab_list_row(_("Start-up Tab"), 'startup_tab', user_startup_tab());

/* The array $pagesizes is set up in config.php for modifications
possible separators can be added by modifying the array definition by editing that file */

if (!isset($_POST['profile']))
	$_POST['profile'] = user_print_profile();

print_profiles_list_row(_("Printing profile"). ':', 'profile', 
	null, _('Browser printing support'));

check_row(_("Use popup window to display reports:"), 'rep_popup', user_rep_popup(),
	false, _('Set this option to on if your browser directly supports pdf files'));

check_row(_("Use icons instead of text links:"), 'graphic_links', user_graphic_links(),
	false, _('Set this option to on for using icons instead of text links'));

text_row_ex(_("Query page size:"), 'query_size',  5, 5, '', user_query_size());

check_row(_("Remember last document date:"), 'sticky_doc_date', sticky_doc_date(),
	false, _('If set document date is remembered on subsequent documents, otherwise default is current date'));

end_outer_table(1);

submit_center('setprefs', _("Update"), true, '',  'default');

end_form(2);

//-------------------------------------------------------------------------------------------------

end_page();

?>