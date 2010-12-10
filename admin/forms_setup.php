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
$page_security = 'SA_FORMSETUP';
$path_to_root = "..";
include($path_to_root . "/includes/session.inc");

page(_($help_context = "Forms Setup"));

include($path_to_root . "/includes/ui.inc");

//-------------------------------------------------------------------------------------------------

if (isset($_POST['setprefs'])) 
{

	$systypes = get_systypes();

	begin_transaction();

    while ($type = db_fetch($systypes)) 
    {
    	save_next_reference($type["type_id"], $_POST['id' . $type["type_id"]]);
    }

    commit_transaction();

	display_notification_centered(_("Forms settings have been updated."));
}

start_form();

start_outer_table(TABLESTYLE2);

$systypes = get_systypes();
table_section(1);

$th = array(_("Form"), _("Next Reference"));
table_header($th);
$i = 0;
while ($type = db_fetch($systypes)) 
{
	if ($i++ == ST_CUSTCREDIT)
	{
		table_section(2);
		table_header($th);
	}	
	ref_row($systypes_array[$type["type_id"]], 'id' . $type["type_id"], '', $type["next_reference"]);
}

end_outer_table(1);

submit_center('setprefs', _("Update"), true, '', 'default');

end_form(2);

//-------------------------------------------------------------------------------------------------

end_page();

?>