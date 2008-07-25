<?php

$page_security = 10;
$path_to_root="..";
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/manufacturing.inc");

include_once($path_to_root . "/manufacturing/includes/manufacturing_db.inc");
include_once($path_to_root . "/manufacturing/includes/manufacturing_ui.inc");

$js = "";
if ($use_date_picker)
	$js .= get_js_date_picker();
page(_("Work Order Release to Manufacturing"), false, false, "", $js);

if (isset($_GET["trans_no"]))
{
	$selected_id = $_GET["trans_no"];
}
elseif (isset($_POST["selected_id"]))
{
	$selected_id = $_POST["selected_id"];
}
else
{
	display_note("This page must be called with a work order reference");
	exit;
}

//------------------------------------------------------------------------------------

function can_process($myrow)
{
	if ($myrow['released'])
	{
		display_error(_("This work order has already been released."));
		set_focus('released');
		return false;
	}

	// make sure item has components
	if (!has_bom($myrow['stock_id']))
	{
		display_error(_("This Work Order cannot be released. The selected item to manufacture does not have a bom."));
		set_focus('stock_id');
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------
if (isset($_POST['release']))
{
	release_work_order($selected_id, $_POST['released_date'], $_POST['memo_']);

	display_note(_("The work order has been released to manufacturing."));

	hyperlink_no_params("search_work_orders.php", _("Select another work order"));

	end_page();

	exit;
}

//------------------------------------------------------------------------------------

start_form();

$myrow = get_work_order($selected_id);

$_POST['released'] = $myrow["released"];
$_POST['memo_'] = "";

if (can_process($myrow))
{
	start_table($table_style2);

    label_row(_("Work Order #:"), $selected_id);
    label_row(_("Work Order Reference:"), $myrow["wo_ref"]);

    date_row(_("Released Date") . ":", 'released_date');

    textarea_row(_("Memo:"), 'memo_', $_POST['memo_'], 40, 5);

    end_table(1);

    submit_center('release', _("Release Work Order"), true, '', false);

    hidden('selected_id', $selected_id);
    hidden('stock_id', $myrow['stock_id']);

}

end_form();

end_page();

?>