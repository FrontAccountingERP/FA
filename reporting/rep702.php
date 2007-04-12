<?php

$page_security = 2;
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	List of Journal Entries
// ----------------------------------------------------------------
$path_to_root="../";

include_once($path_to_root . "includes/session.inc");
include_once($path_to_root . "includes/date_functions.inc");
include_once($path_to_root . "includes/data_checks.inc");
include_once($path_to_root . "gl/includes/gl_db.inc");
include_once($path_to_root . "includes/ui/ui_view.inc");

//----------------------------------------------------------------------------------------------------

// trial_inquiry_controls();
print_list_of_journal_entries();

//----------------------------------------------------------------------------------------------------

function print_list_of_journal_entries()
{
    global $path_to_root;

    include_once($path_to_root . "reporting/includes/pdf_report.inc");

    $from = $_POST['PARAM_0'];
    $to = $_POST['PARAM_1'];
    $systype = $_POST['PARAM_2'];
    $comments = $_POST['PARAM_3'];
    $dec = user_price_dec();

    $cols = array(0, 100, 240, 300, 400, 460, 520, 580);

    $headers = array(_('Type/Account'), _('Account Name'), _('Date/Dim.'), 
    	_('Person/Item/Memo'), _('Debit'), _('Credit'));
    
    $aligns = array('left', 'left', 'left', 'left', 'right', 'right');
    
    $params =   array( 	0 => $comments,
    				    1 => array('text' => _('Period'), 'from' => $from,'to' => $to),
                    	2 => array('text' => _('Type'), 'from' => systypes::name($systype),
                            'to' => ''));

    $rep = new FrontReport(_('List of Journal Entries'), "JournalEntries.pdf", user_pagesize());

    $rep->Font();
    $rep->Info($params, $cols, $headers, $aligns);
    $rep->Header();

    if ($systype == -1)
        $systype = null;

    $trans = get_gl_transactions($from, $to, -1, null, 0, $systype);

    $typeno = 0;
    while ($myrow=db_fetch($trans))
    {
        if ($typeno != $myrow['type_no'])
        {
            if ($typeno != 0)
            {
                $rep->Line($rep->row  + 4);
                $rep->NewLine();
            }
            $typeno = $myrow['type_no'];
            $TransName = systypes::name($myrow['type']);
            $rep->TextCol(0, 2, $TransName . " # " . $myrow['type_no']);
            $rep->TextCol(2, 3, sql2date($myrow['tran_date']));
            $coms =  payment_person_types::person_name($myrow["person_type_id"],$myrow["person_id"]);    
            $memo = get_comments_string($myrow['type'], $myrow['type_no']);
            if ($memo != '')
            	$coms .= ($coms!= "")?"/":"" . $memo;
            $rep->TextCol(3, 6, $coms);
            $rep->NewLine(2);
        }
        $rep->TextCol(0, 1, $myrow['account']);
        $rep->TextCol(1, 2, $myrow['account_name']);
        $dim_str = get_dimension_string($myrow['dimension_id']);
        $dim_str2 = get_dimension_string($myrow['dimension2_id']);
        if ($dim_str2 != "")
        	$dim_str .= "/".$dim_str2;
        $rep->TextCol(2, 3, $dim_str);
        $rep->TextCol(3, 4, $myrow['memo_']);
        if ($myrow['amount'] > 0.0)
            $rep->TextCol(4, 5, number_format2(abs($myrow['amount']), $dec));
        else
            $rep->TextCol(5, 6, number_format2(abs($myrow['amount']), $dec));
        $rep->NewLine(1, 2);    
    }
    $rep->Line($rep->row  + 4);
    $rep->End();
}

?>