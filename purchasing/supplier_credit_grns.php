<?php

$page_security = 5;
$path_to_root="..";

include_once($path_to_root . "/purchasing/includes/supp_trans_class.inc");
include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/purchasing/includes/purchasing_ui.inc");
include_once($path_to_root . "/purchasing/includes/purchasing_db.inc");

$js = "";
if ($use_date_picker)
	$js .= get_js_date_picker();
page(_("Select Received Items to Add"), false, false, "", $js);

if (!isset($_SESSION['supp_trans']))
{
	display_note(_("To enter supplier transactions the supplier must first be selected from the supplier selection screen, then the link to enter a supplier credit note must be clicked on."));
	exit;
	/*It all stops here if there aint no supplier selected and credit note initiated ie $_SESSION['supp_trans'] started off*/
}

//-----------------------------------------------------------------------------------------

display_heading($_SESSION['supp_trans']->supplier_name);
echo "<br>";

//-----------------------------------------------------------------------------------------

function check_data()
{
	if (!check_num('This_QuantityCredited', 0))
	{
		display_error(_("The quantity to credit must be numeric and greater than zero."));
		set_focus('This_QuantityCredited');
		return false;
	}

	if (!check_num('ChgPrice', 0))
	{
		display_error(_("The price is either not numeric or negative."));
		set_focus('ChgPrice');
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------------------

if (isset($_POST['AddGRNToTrans']))
{

	if (check_data())
	{
		$complete = False;

		//$_SESSION['supp_trans']->add_grn_to_trans($_POST['GRNNumber'],
    	//	$_POST['po_detail_item'], $_POST['item_code'],
    	//	$_POST['item_description'], $_POST['qty_recd'],
    	//	$_POST['prev_quantity_inv'], $_POST['This_QuantityCredited'],
    	//	$_POST['order_price'], $_POST['ChgPrice'], $complete,
    	//	$_POST['std_cost_unit'], $_POST['gl_code']);
		$_SESSION['supp_trans']->add_grn_to_trans($_POST['GRNNumber'],
    		$_POST['po_detail_item'], $_POST['item_code'],
    		$_POST['item_description'], $_POST['qty_recd'],
    		$_POST['prev_quantity_inv'], input_num('This_QuantityCredited'),
    		$_POST['order_price'], input_num('ChgPrice'), $complete,
    		$_POST['std_cost_unit'], "");
	}
}

//-----------------------------------------------------------------------------------------
$id = find_submit('Delete');
if ($id != -1)
{
	$_SESSION['supp_trans']->remove_grn_from_trans($id);
	$Ajax->activate('grn_items');
	$Ajax->activate('grn_table');
}

//-----------------------------------------------------------------------------------------
start_form(false,true);
display_grn_items($_SESSION['supp_trans'], 1);
end_form();
echo "<br>";

hyperlink_no_params("$path_to_root/purchasing/supplier_credit.php", _("Return to Credit Note Entry"));
echo "<hr>";

//-----------------------------------------------------------------------------------------

// get the supplier grns that have been invoiced
$result = get_grn_items(0, $_SESSION['supp_trans']->supplier_id, false, true);

if (db_num_rows($result) == 0)
{
	display_note(_("There are no received items for the selected supplier that have been invoiced."));
	display_note(_("Credits can only be applied to invoiced items."));

	echo "<br>";
	end_page();
	exit;
}

/*Set up a table to show the GRN items outstanding for selection */
start_form(false, true);

div_start('grn_table');
start_table("$table_style width=95%");
$th = array(_("Delivery"), _("Sequence #"), _("Order"), _("Item Code"), _("Description"),
	_("Delivered"), _("Total Qty Received"), _("Qty Already Invoiced"),
	_("Qty Yet To Invoice"), _("Order Price"), _("Line Value"));
table_header($th);
$i = $k =0;
while ($myrow = db_fetch($result))
{

	$grn_already_on_credit = False;

	foreach ($_SESSION['supp_trans']->grn_items as $entered_grn)
	{
		if ($entered_grn->id == $myrow["id"])
		{
			$grn_already_on_credit = True;
		}
	}
	if ($grn_already_on_credit == False)
	{

		alt_table_row_color($k);

		label_cell(get_trans_view_str(25, $myrow["grn_batch_id"]));
       	submit_cells('grn_item_id'.$myrow["id"], $myrow["id"], '', '', true);
		label_cell(get_trans_view_str(systypes::po(), $myrow["purch_order_no"]));
        label_cell($myrow["item_code"]);
        label_cell($myrow["description"]);
        label_cell(sql2date($myrow["delivery_date"]));
        $dec = get_qty_dec($myrow["item_code"]);
        qty_cell($myrow["qty_recd"], false, $dec);
        qty_cell($myrow["quantity_inv"], false, $dec);
        qty_cell($myrow["qty_recd"] - $myrow["quantity_inv"], false, $dec);
		amount_cell($myrow["unit_price"]);
		amount_cell(round($myrow["unit_price"] * $myrow["quantity_inv"],  user_price_dec()));
		end_row();
		$i++;
		if ($i > 15)
		{
			$i = 0;
			table_header($th);
		}
	}
}

end_table();
div_end();

//-----------------------------------------------------------------------------------------
$id = find_submit('grn_item_id');
if ($id || get_post('AddGRNToTrans'))
{
	$Ajax->activate('grn_selector');
}
if (get_post('AddGRNToTrans')) 
{
	$Ajax->activate('grn_table');
	$Ajax->activate('grn_items');
}


div_start('grn_selector');
$id = find_submit('grn_item_id');
if ($id != -1)
{

	$myrow = get_grn_item_detail($id);

	echo "<br>";
	display_heading2(_("Delivery Item Selected For Adding To A Supplier Credit Note"));
	start_table("$table_style width=80%");
	$th = array(_("Sequence #"), _("Item"), _("Qty Already Invoiced"),
		_("Quantity to Credit"), _("Order Price"), _("Credit Price"));
	table_header($th);

	start_row();
	label_cell($id);
    label_cell($myrow['item_code'] . " " . $myrow['description']);
    $dec = get_qty_dec($myrow['item_code']);
    qty_cell($myrow["quantity_inv"], false, $dec);
    qty_cells(null, 'This_QuantityCredited', number_format2(max($myrow['quantity_inv'],0), $dec), null, null, $dec);
    amount_cell($myrow['unit_price']);
    amount_cells(null, 'ChgPrice', price_format($myrow['unit_price']));
    end_row();
	end_table(1);

	submit_center('AddGRNToTrans', _("Add to Credit Note"), true, '', true);

	hidden('GRNNumber', $id);
	hidden('item_code', $myrow['item_code']);;
	hidden('item_description', $myrow['description']);
	hidden('qty_recd', $myrow['qty_recd']);
	hidden('prev_quantity_inv', $myrow['quantity_inv']);
	hidden('order_price', $myrow['unit_price']);
	hidden('std_cost_unit', $myrow['std_cost_unit']);

	hidden('po_detail_item', $myrow['po_detail_item']);
}
div_end();

end_form();
echo '<br>';
end_page(false, true);
?>
