<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU Affero General Public License,
	AGPL, as published by the Free Software Foundation, either version 
	3 of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/agpl-3.0.html>.
***********************************************************************/
/* Author Rob Mallon */
$page_security = 8;
$path_to_root="..";
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include_once($path_to_root . "/gl/includes/gl_db.inc");
include_once($path_to_root . "/includes/banking.inc");

//Added 1/9/09
if (isset($_POST["command"]) && $_POST["command"]=="Reconcile") {
	//echo "Made it\n";
    $reconcile_id = strtok($_POST["reconcile_idtokens"], "|");
    while ($reconcile_id !== false) {
		$formfield="reconcile".$reconcile_id;
		if (!isset($_POST[$formfield]))
			$_POST[$formfield] = 0;
		$reconcile_value=$_POST[$formfield];
		$sql="UPDATE ".TB_PREF."bank_trans SET reconciled=$reconcile_value WHERE id=$reconcile_id";
		$result = db_query($sql,"Error setting reconcile flag on trans id $reconcile_id");
		$reconcile_id = strtok("|");
    }
    $sql2="UPDATE ".TB_PREF."bank_accounts SET last_reconciled_date='".date2sql($_POST["reconcile_date"])."',
        ending_reconcile_balance=".$_POST["reconcile_ending_balance"]." WHERE id=".$_POST["bank_account"];
	$result = db_query($sql2,"Error updating reconciliation information");
}

$js = "";
if ($use_popup_windows)
	$js .= get_js_open_window(800, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();
page(_("Reconcile Bank Account"), false, false, "", $js);

?>
<script>
function handleReconcileClick(form,reconcile_value,reconcile_amount) {
       var val1=form.reconcile_difference.value*1;
       val1=(Math.round(val1*100))/100;
       var val2=reconcile_amount*1;
       val2=(Math.round(val2*100))/100;
       var val4=form.reconcile_amount.value*1;
       val4=(Math.round(val4*100))/100;
//Unchecking a line
       if (reconcile_value=='1') {
		val3=(Math.round((val1-val2)*100))/100;
               form.reconcile_difference.value=val3;
		val5=(Math.round((val4+val2)*100))/100;
               form.reconcile_amount.value=val5;
//Checking a line
       } else {
		val3=(Math.round((val1+val2)*100))/100;
               form.reconcile_difference.value=val3;
		val5=(Math.round((val4-val2)*100))/100;
               form.reconcile_amount.value=val5;
       }

}

function handleReconcileDifference(form) {
	form.reconcile_difference.value=Number(form.reconcile_ending_balance.value)-Number(form.reconcile_beginning_balance.value);
}

</script>
<?php

check_db_has_bank_accounts(_("There are no bank accounts defined in the system."));

//-----------------------------------------------------------------------------------
// Ajax updates
//
if (get_post('Show'))
{
       $Ajax->activate('trans_tbl');
}
//------------------------------------------------------------------------------------------------
//Added 1/9/09
start_form();
start_table("class='tablestyle_noborder'");
start_row();
bank_accounts_list_cells(_("Account:"), 'bank_account', null);

date_cells(_("From:"), 'TransAfterDate', '', null, -30);
date_cells(_("To:"), 'TransToDate');
//Added 1/8/09
if (!isset($_POST['ShowReconciled']))
	$_POST['ShowReconciled'] = 0;
$show_reconciled = $_POST['ShowReconciled'];	
yesno_list_cells(_("Show Reconciled:"), 'ShowReconciled');

submit_cells('Show',_("Show"),'','', false);
end_row();
end_table();
end_form();
//Added 1/9/09
$reconcile_sql="SELECT last_reconciled_date,ending_reconcile_balance FROM ".TB_PREF."bank_accounts WHERE id=".$_POST['bank_account'];
$reconcile_result = db_query($reconcile_sql,"Error");
if ($reconcile_row = db_fetch($reconcile_result)) {
   	echo "<hr>";
   	start_table("class='tablestyle_noborder'");
   	start_row();
   	label_cell("<b>Last Reconciled Date: </b>");
   	label_cell("<b>".sql2date($reconcile_row["last_reconciled_date"])."</b>");
   	label_cell("&nbsp;&nbsp;&nbsp;&nbsp;");
   	label_cell("<b>Last Reconciled Ending Balance: </b>");
   	amount_cell($reconcile_row["ending_reconcile_balance"],true);
   	end_row();
   	end_table();
} else {
    echo "Error: $reconcile_sql<br>";
}
start_form(false,false,"", 'reconcileform');
start_table($table_style);
$th = array(_("Reconcile Date"), _("Beginning<br>Balance"), _("Ending<br>Balance"), _("Reconciled<br>Amount"), _("Difference"));
table_header($th);
start_row();
date_cells("", "reconcile_date");
text_cells("", "reconcile_beginning_balance", $reconcile_row["ending_reconcile_balance"], 7, "", false, "", "", "onchange='handleReconcileDifference(this.form);'");
text_cells("", "reconcile_ending_balance", 0,7, "", false, "", "", "onchange='handleReconcileDifference(this.form);'");
text_cells("", "reconcile_amount", 0, 7, "", false, "", "", "READONLY");
text_cells("", "reconcile_difference", 0, 7, "", false, "", "", "READONLY");
end_row();
end_table();
echo "<hr>";
//------------------------------------------------------------------------------------------------

$date_after = date2sql($_POST['TransAfterDate']);
$date_to = date2sql($_POST['TransToDate']);

if (!isset($_POST['bank_account']))
    $_POST['bank_account'] = "";

//Modified 1/9/09
$sql = "SELECT ".TB_PREF."bank_trans.* FROM ".TB_PREF."bank_trans
	WHERE ".TB_PREF."bank_trans.bank_act = '" . $_POST['bank_account'] . "'
	AND trans_date >= '$date_after'
	AND trans_date <= '$date_to'";
//Added line 1/9/09
if ($show_reconciled=='0') 
	$sql .= " AND ".TB_PREF."bank_trans.reconciled=0 ";
$sql .= " ORDER BY trans_date,".TB_PREF."bank_trans.id";

$result = db_query($sql,"The transactions for '" . $_POST['bank_account'] . "' could not be retrieved");

div_start('trans_tbl');
$act = get_bank_account($_POST["bank_account"]);
display_heading($act['bank_account_name']." - ".$act['bank_curr_code']);

//Added 1/9/09

hidden('command', 'Reconcile');
hidden('Show', 'Show');
hidden('bank_account', $_POST["bank_account"]);
hidden('TransAfterDate',$_POST["TransAfterDate"]);
hidden('TransToDate',$_POST["TransToDate"]);
hidden('ShowReconciled',$_POST["ShowReconciled"]);

start_table($table_style);

$th = array(_("Type"), _("#"), _("Reference"), _("Date"),
       _("Debit"), _("Credit"), _("Person/Item"), "", "X");
table_header($th);
$idtokens="";
$k = 0; //row colour counter
while ($myrow = db_fetch($result))
{
   	$idtokens.=$myrow["id"]."|";
   	alt_table_row_color($k);

   	$trandate = sql2date($myrow["trans_date"]);
	//Added 1/9/09
    $reconcile_check_name="reconcile".$myrow["id"];
    label_cell(systypes::name($myrow["type"]));
    label_cell(get_trans_view_str($myrow["type"],$myrow["trans_no"]));
    label_cell(get_trans_view_str($myrow["type"],$myrow["trans_no"],$myrow['ref']));
    label_cell($trandate);
    display_debit_or_credit_cells($myrow["amount"]);
    //amount_cell($running_total);
    label_cell(payment_person_types::person_name($myrow["person_type_id"],$myrow["person_id"]));
    label_cell(get_gl_view_str($myrow["type"], $myrow["trans_no"]));
	//Added 1/9/09 I needed the javascript action onclick, so I made my own checkbox
    echo "<td>";
    echo "<input type='checkbox' name='$reconcile_check_name' ";
    if ($myrow["reconciled"]==1) echo "CHECKED ";
    echo "value='1' ";
    echo "onclick=\"javascript:handleReconcileClick(this.form,this.checked,'".$myrow["amount"]."');\">";
    echo "</td>";
    //check_cells('', $reconcile_check_name, $myrow["reconciled"], false);
    end_row();
    //Removed by Rob Mallon on 1/8/09
    //if ($j == 12)
    //{
    //      $j = 1;
    //      table_header($th);
    //}
    //$j++;
}
//end of while loop

end_table(2);
hidden('reconcile_idtokens',$idtokens);
submit_center('Reconcile',_("Reconcile"),true,'', false);

div_end();
end_form();

//------------------------------------------------------------------------------------------------

end_page();

?>