<?php

$path_to_root="..";
$page_security = 5;

include_once($path_to_root . "/includes/session.inc");

page(_("View or Print Transactions"));

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/reporting/includes/reporting.inc");

//----------------------------------------------------------------------------------------

function viewing_controls()
{
    start_form(false, true);

    start_table("class='tablestyle_noborder'");
	start_row();

	systypes_list_cells(_("Type:"), 'filterType', null, true);

    ref_cells(_("from #:"), 'FromTransNo');

    ref_cells(_("to #:"), 'ToTransNo');

    submit_cells('ProcessSearch', _("Search"));

	end_row();
    end_table(1);

	end_form();
}

//----------------------------------------------------------------------------------------

function check_valid_entries()
{
	if (!is_numeric($_POST['FromTransNo']) OR $_POST['FromTransNo'] <= 0)
	{
		display_note(_("The starting transaction number is expected to be numeric and greater than zero."));
		return false;
	}

	if (!is_numeric($_POST['ToTransNo']) OR $_POST['ToTransNo'] <= 0)
	{
		echo _("The ending transaction number is expected to be numeric and greater than zero.");
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------

function handle_search()
{
	global $table_style;
	if (check_valid_entries()==true)
	{
		$db_info = get_systype_db_info($_POST['filterType']);

		$table_name = $db_info[0];
		$type_name = $db_info[1];
		$trans_no_name = $db_info[2];
		$trans_ref = $db_info[3];

		$sql = "SELECT DISTINCT $trans_no_name ";

		if ($trans_ref)
			$sql .= " ,$trans_ref ";

		$sql .= " FROM $table_name
			WHERE $trans_no_name >= " . $_POST['FromTransNo']. "
			AND  $trans_no_name <= " . $_POST['ToTransNo'];

		if ($type_name != null)
			$sql .= " AND $type_name = " . $_POST['filterType'];

		$sql .= " ORDER BY $trans_no_name";

		$result = db_query($sql, "could not query transactions on $table_name");

		if (db_num_rows($result) == 0) 
		{
			echo _("There are no transactions for the given parameters.");
			return;
		}

		start_table($table_style);
		if ($trans_ref)
			$th = array(_("#"), _("Reference"), _("View"), _("Print"));
		else	
			$th = array(_("#"), _("View"), _("Print"));
		table_header($th);	
		$k = 0;
		while ($line = db_fetch($result)) 
		{

			alt_table_row_color($k);

			label_cell($line[$trans_no_name]);
			if ($trans_ref)
				label_cell($line[$trans_ref]);
			label_cell(get_trans_view_str($_POST['filterType'],$line[$trans_no_name], _("View")));
        	label_cell(get_gl_view_str_cell($_POST['filterType'], $line[$trans_no_name], _("View GL")));

        	$forms = get_form_entries($_POST['filterType'], $line[$trans_no_name]);
        	while ($form_item = db_fetch($forms)) 
        	{

        		$param1 = $form_item['param1'];
        		$param2 = $form_item['param2'];

        		if ($_POST['filterType'] == systypes::bank_payment()
        			|| $_POST['filterType'] == systypes::bank_deposit()
        			|| $_POST['filterType'] == systypes::cust_payment()
        			|| $_POST['filterType'] == systypes::supp_payment()) 
        		{
                		$param1 = payment_person_types::type_name($form_item['param1']);
                		$param2 = payment_person_types::person_name($form_item['param1'], $form_item['param2'], false);
        		}

        		//label_cell(printTransaction(_("Print") . " " . getFormTypeName($form_item["form_type"]), $form_item['form_id'], $form_item['form_type'], $_POST['filterType'], $line[$trans_no_name], $line[$trans_ref], $param1, $param2));
        	}
        	end_row();

		}

		end_table();
	}
}

//----------------------------------------------------------------------------------------

if (isset($_POST['ProcessSearch']))
{
	if (!check_valid_entries())
		unset($_POST['ProcessSearch']);
}

//----------------------------------------------------------------------------------------

viewing_controls();

//echo getHiddenFieldScript();

handle_search();

br(2);

end_page();

?>
