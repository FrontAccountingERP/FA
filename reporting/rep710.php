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
$page_security = 15;
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	List of Journal Entries
// ----------------------------------------------------------------
$path_to_root="..";

include_once($path_to_root . "/includes/session.inc");
include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/data_checks.inc");
include_once($path_to_root . "/gl/includes/gl_db.inc");
include_once($path_to_root . "/includes/ui/ui_view.inc");

//----------------------------------------------------------------------------------------------------

print_audit_trail();

function getTransactions($from, $to, $type, $user)
{
	$fromdate = date2sql($from);
	$todate = date2sql($to);

	$sql = "SELECT ".TB_PREF."audit_trail.*, ".TB_PREF."gl_trans.tran_date, 
		SUM(IF(".TB_PREF."gl_trans.amount > 0, ".TB_PREF."gl_trans.amount, 0)) AS amount,
		".TB_PREF."users.user_id,
		UNIX_TIMESTAMP(".TB_PREF."audit_trail.stamp) as unix_stamp
		FROM ".TB_PREF."audit_trail, ".TB_PREF."gl_trans, ".TB_PREF."users
		WHERE ".TB_PREF."audit_trail.type = ".TB_PREF."gl_trans.type
			AND ".TB_PREF."audit_trail.trans_no = ".TB_PREF."gl_trans.type_no 
			AND ".TB_PREF."audit_trail.user = ".TB_PREF."users.id ";
	if ($type != -1)
		$sql .= "AND ".TB_PREF."gl_trans.type=$type ";
	if ($user != -1)	
		$sql .= "AND ".TB_PREF."audit_trail.user='$user' ";
	$sql .= "AND DATE(".TB_PREF."audit_trail.stamp) >= '$fromdate'
			AND DATE(".TB_PREF."audit_trail.stamp) <= '$todate'
		GROUP BY ".TB_PREF."gl_trans.type_no,".TB_PREF."audit_trail.gl_seq,".TB_PREF."audit_trail.stamp	
		ORDER BY ".TB_PREF."audit_trail.stamp,".TB_PREF."audit_trail.gl_seq";
    return db_query($sql,"No transactions were returned");
}
//----------------------------------------------------------------------------------------------------

function print_audit_trail()
{
    global $path_to_root;

    $from = $_POST['PARAM_0'];
    $to = $_POST['PARAM_1'];
    $systype = $_POST['PARAM_2'];
    $user = $_POST['PARAM_3'];
    $comments = $_POST['PARAM_4'];
	$destination = $_POST['PARAM_5'];
	if ($destination)
		include_once($path_to_root . "/reporting/includes/excel_report.inc");
	else
		include_once($path_to_root . "/reporting/includes/pdf_report.inc");

    $dec = user_price_dec();

    $cols = array(0, 60, 120, 180, 240, 340, 400, 460, 520);

    $headers = array(_('Date'), _('Time'), _('User'), _('Trans Date'),
    	_('Type'), _('#'), _('Action'), _('Amount'));

    $aligns = array('left', 'left', 'left', 'left', 'left', 'left', 'left', 'right');

	$usr = get_user($user);
	$user_id = $usr['user_id'];
    $params =   array( 	0 => $comments,
    				    1 => array('text' => _('Period'), 'from' => $from,'to' => $to),
                    	2 => array('text' => _('Type'), 'from' => ($systype != -1 ? systypes::name($systype) : _('All')), 'to' => ''),
                    	3 => array('text' => _('User'), 'from' => ($user != -1 ? $user_id : _('All')), 'to' => ''));

    $rep = new FrontReport(_('Audit Trail'), "AuditTrail", user_pagesize());

    $rep->Font();
    $rep->Info($params, $cols, $headers, $aligns);
    $rep->Header();

    $trans = getTransactions($from, $to, $systype, $user);

    while ($myrow=db_fetch($trans))
    {
        $rep->TextCol(0, 1, sql2date($myrow['stamp']));
        if (user_date_format() == 0)
        	$rep->TextCol(1, 2, date("h:i:s a", $myrow['unix_stamp']));
        else	
        	$rep->TextCol(1, 2, date("H:i:s", $myrow['unix_stamp']));
        $rep->TextCol(2, 3, $myrow['user_id']);
        $rep->TextCol(3, 4, sql2date($myrow['tran_date']));
        $rep->TextCol(4, 5, systypes::name($myrow['type']));
        $rep->TextCol(5, 6, $myrow['trans_no']);
        if ($myrow['gl_seq'] == null)
        	$action = _('Changed');
        else
        	$action = _('Closed');
        $rep->TextCol(6, 7, $action);
        $rep->AmountCol(7, 8, $myrow['amount'], $dec);
        $rep->NewLine(1, 2);
    }
    $rep->Line($rep->row  + 4);
    $rep->End();
}

?>