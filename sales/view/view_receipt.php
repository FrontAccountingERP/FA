<?php

$page_security = 1;
$path_to_root="../..";
include_once($path_to_root . "/includes/session.inc");

page(_("View Customer Payment"), true);

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/sales/includes/sales_db.inc");

if (isset($_GET["trans_no"]))
{
	$trans_id = $_GET["trans_no"];
}

$receipt = get_customer_trans($trans_id, systypes::cust_payment());

display_heading(_("Customer Payment") . " #$trans_id");

echo "<br>";
start_table("$table_style width=80%");
start_row();
label_cells(_("From Customer"), $receipt['DebtorName'], "class='tableheader2'");
label_cells(_("Into Bank Account"), $receipt['bank_account_name'], "class='tableheader2'");
label_cells(_("Date of Deposit"), sql2date($receipt['tran_date']), "class='tableheader2'");
end_row();
start_row();
label_cells(_("Payment Currency"), $receipt['curr_code'], "class='tableheader2'");
label_cells(_("Amount"), number_format2(-$receipt['ov_amount'], user_price_dec()), "class='tableheader2'");
label_cells(_("Discount"), number_format2(-$receipt['ov_discount'], user_price_dec()), "class='tableheader2'");
end_row();
start_row();
label_cells(_("Payment Type"), $receipt['BankTransType'], "class='tableheader2'");
label_cells(_("Reference"), $receipt['reference'], "class='tableheader2'", "colspan=4");
end_row();
comments_display_row(systypes::cust_payment(), $trans_id);

end_table(1);

$voided = is_voided_display(systypes::cust_payment(), $trans_id, _("This customer payment has been voided."));

if (!$voided)
{
	display_allocations_from(payment_person_types::customer(), $receipt['debtor_no'], systypes::cust_payment(), $trans_id, -$receipt['Total']);
}

end_page(true);
?>