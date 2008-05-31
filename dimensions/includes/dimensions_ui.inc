<?php

include_once($path_to_root . "/includes/ui.inc");

//--------------------------------------------------------------------------------------

function display_dimension_payments($id)
{
	global $path_to_root, $table_style;

    $result = get_bank_trans(null, null, payment_person_types::dimension(), $id);

    echo "<br>";

    if (db_num_rows($result) == 0)
    {
    	display_note(_("There are no transactions for this dimension."));
    }
    else
    {
		display_heading2(_("Transactions for this Dimension"));
		start_table($table_style);
		$th = array(_("Type"), _("#"), _("Reference"),
			_("Date"), _("Debit"), _("Credit"));
		table_header($th);

        $total = $k = 0;

        while ($myrow = db_fetch($result))
        {

			alt_table_row_color($k);

			// projects are always in home currency...get the home equivalent of the banktrans
			// from it's gl postings
			$home_value = get_gl_trans_value($myrow["bank_act"], $myrow["type"], $myrow["trans_no"]);

        	label_cell(systypes::name($myrow["type"]));
			label_cell(get_trans_view_str($myrow["type"], $myrow["trans_no"]));
			label_cell($myrow["ref"]);
			label_cell(sql2date($myrow["trans_date"]));
			display_debit_or_credit_cells($home_value);

			$total += $home_value;

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