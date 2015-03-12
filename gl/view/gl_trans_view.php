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
$page_security = 'SA_GLTRANSVIEW';
$path_to_root = "../..";
include_once($path_to_root . "/includes/session.inc");

page(_($help_context = "General Ledger Transaction Details"), true);

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");

include_once($path_to_root . "/gl/includes/gl_db.inc");

if (!isset($_GET['type_id']) || !isset($_GET['trans_no'])) 
{ /*Script was not passed the correct parameters */

	display_note(_("The script must be called with a valid transaction type and transaction number to review the general ledger postings for."));
	end_page();
}

function display_gl_heading($myrow)
{
	global $systypes_array;
	$trans_name = $systypes_array[$_GET['type_id']];
    start_table(TABLESTYLE, "width='95%'");
    $th = array(_("General Ledger Transaction Details"), _("Reference"),
    	_("Date"), _("Person/Item"));
    table_header($th);	
    start_row();	
    label_cell("$trans_name #" . $_GET['trans_no']);
    label_cell($myrow["reference"]);
	label_cell(sql2date($myrow["tran_date"]));
	label_cell(payment_person_name($myrow["person_type_id"],$myrow["person_id"]));
	
	end_row();

	comments_display_row($_GET['type_id'], $_GET['trans_no']);

    end_table(1);
}
$result = get_gl_trans($_GET['type_id'], $_GET['trans_no']);

if (db_num_rows($result) == 0)
{
    echo "<p><center>" . _("No general ledger transactions have been created for") . " " .$systypes_array[$_GET['type_id']]." " . _("number") . " " . $_GET['trans_no'] . "</center></p><br><br>";
	end_page(true);
	exit;
}

/*show a table of the transactions returned by the sql */
$dim = get_company_pref('use_dimension');

if ($dim == 2)
	$th = array(_("Account Code"), _("Account Name"), _("Dimension")." 1", _("Dimension")." 2",
		_("Debit"), _("Credit"), _("Memo"));
else if ($dim == 1)
	$th = array(_("Account Code"), _("Account Name"), _("Dimension"),
		_("Debit"), _("Credit"), _("Memo"));
else		
	$th = array(_("Account Code"), _("Account Name"),
		_("Debit"), _("Credit"), _("Memo"));
$k = 0; //row colour counter
$heading_shown = false;

$credit = $debit = 0;
while ($myrow = db_fetch($result)) 
{
	if ($myrow['amount'] == 0) continue;
	if (!$heading_shown)
	{
		display_gl_heading($myrow);
		start_table(TABLESTYLE, "width='95%'");
		table_header($th);
		$heading_shown = true;
	}	

	alt_table_row_color($k);
	
    label_cell($myrow['account']);
	label_cell($myrow['account_name']);
	if ($dim >= 1)
		label_cell(get_dimension_string($myrow['dimension_id'], true));
	if ($dim > 1)
		label_cell(get_dimension_string($myrow['dimension2_id'], true));

	display_debit_or_credit_cells($myrow['amount']);
	label_cell($myrow['memo_']);
	end_row();
    if ($myrow['amount'] > 0 ) 
    	$debit += $myrow['amount'];
    else 
    	$credit += $myrow['amount'];
}
if ($heading_shown)
{
    start_row("class='inquirybg' style='font-weight:bold'");
    label_cell(_("Total"), "colspan=2");
    if ($dim >= 1)
        label_cell('');
    if ($dim > 1)
        label_cell('');
    amount_cell($debit);
    amount_cell(-$credit);
    label_cell('');
    end_row();
	end_table(1);
}

//end of while loop

is_voided_display($_GET['type_id'], $_GET['trans_no'], _("This transaction has been voided."));

end_page(true, false, false, $_GET['type_id'], $_GET['trans_no']);

?>
