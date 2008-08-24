<?php

$page_security =10;
$path_to_root="..";
include($path_to_root . "/includes/session.inc");

page(_("Forms Setup"));

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
start_table("class='tablestyle'");

$systypes = get_systypes();

$th = array(_("Form"), _("Next Reference"));
table_header($th);

while ($type = db_fetch($systypes)) 
{
	ref_row(systypes::name($type["type_id"]), 'id' . $type["type_id"], '', $type["next_reference"]);
}

end_table(1);

submit_center('setprefs', _("Update"), true, '', true);

end_form(2);

//-------------------------------------------------------------------------------------------------

end_page();

?>