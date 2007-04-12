<?php

$page_security = 10;
$path_to_root="../..";

include_once($path_to_root . "/includes/session.inc");

page(_("View Dimension"), true);

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/dimensions/includes/dimensions_db.inc");
include_once($path_to_root . "/dimensions/includes/dimensions_ui.inc");

//-------------------------------------------------------------------------------------------------

if ($_GET['trans_no'] != "") 
{
	$id = $_GET['trans_no'];
}

display_heading(systypes::name(systypes::dimension()) . " # " . $id);

$myrow = get_dimension($id);

if (strlen($myrow[0]) == 0) 
{
	echo _("The work order number sent is not valid.");
    exit;
}

start_table($table_style);

$th = array(_("#"), _("Reference"), _("Name"), _("Type"), _("Date"), _("Due Date"));
table_header($th);

start_row();
label_cell($myrow["id"]);
label_cell($myrow["reference"]);
label_cell($myrow["name"]);
label_cell($myrow["type_"]);
label_cell(sql2date($myrow["date_"]));
label_cell(sql2date($myrow["due_date"]));
end_row();

comments_display_row(systypes::dimension(), $id);

end_table();

if ($myrow["closed"] == true) 
{
	echo "<br>" . _("This dimension is closed.") . "<br>";
}

display_dimension_payments($id);

br(1);

end_page(true);

?>
