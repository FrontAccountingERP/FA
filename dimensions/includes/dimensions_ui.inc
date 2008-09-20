<?php

include_once($path_to_root . "/includes/ui.inc");

//--------------------------------------------------------------------------------------

function display_dimension_payments($id)
{
	global $path_to_root, $table_style;

	$sql = "SELECT * FROM ".TB_PREF."gl_trans WHERE dimension_id = $id OR dimension2_id = $id
		ORDER BY account,tran_date";
	$result = db_query($sql, "Transactions could not be calculated");

    echo "<br>";

    if (db_num_rows($result) == 0)
    {
    	display_note(_("There are no transactions for this dimension."));
    }
    else
    {
		display_heading2(_("Transactions for this Dimension"));
		start_table($table_style);
		$th = array(_("Type"), _("#"), _("Account"),
			_("Date"), _("Debit"), _("Credit"));
		table_header($th);

        $total = $k = 0;

        while ($myrow = db_fetch($result))
        {

			alt_table_row_color($k);

        	label_cell(systypes::name($myrow["type"]));
			label_cell(get_gl_view_str($myrow["type"], $myrow["type_no"], $myrow["type_no"], true));
			label_cell($myrow["account"]);
			label_cell(sql2date($myrow["tran_date"]));
			display_debit_or_credit_cells($myrow["amount"]);

			$total += $myrow["amount"];

			end_row();
		}

		start_row();
		label_cell(_("Total"), "colspan=4");
		display_debit_or_credit_cells($total);
		end_row();

		end_table();
    }
}

//--------------------------------------------------------------------------------------

?>