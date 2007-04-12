<?php

$page_security = 2;
// ----------------------------------------------------------------
// $ Revision:	2.0 $
// Creator:	Joe Hunt
// date_:	2005-05-19
// Title:	Bill Of Material
// ----------------------------------------------------------------
$path_to_root="../";

include_once($path_to_root . "includes/session.inc");
include_once($path_to_root . "includes/date_functions.inc");
include_once($path_to_root . "includes/data_checks.inc");
include_once($path_to_root . "gl/includes/gl_db.inc");
include_once($path_to_root . "inventory/includes/db/items_db.inc");

//----------------------------------------------------------------------------------------------------

// trial_inquiry_controls();
print_bill_of_material();

function getTransactions($from, $to)
{
	$sql = "SELECT ".TB_PREF."bom.parent,
			".TB_PREF."bom.component,
			".TB_PREF."stock_master.description as CompDescription,
			".TB_PREF."bom.quantity,
			".TB_PREF."bom.loc_code,
			".TB_PREF."bom.workcentre_added
		FROM
			".TB_PREF."stock_master,
			".TB_PREF."bom
		WHERE ".TB_PREF."stock_master.stock_id=".TB_PREF."bom.component
		AND ".TB_PREF."bom.parent >= '$from'
		AND ".TB_PREF."bom.parent <= '$to'
		ORDER BY
			".TB_PREF."bom.parent,
			".TB_PREF."bom.component";

    return db_query($sql,"No transactions were returned");
}

//----------------------------------------------------------------------------------------------------

function print_bill_of_material()
{
    global $path_to_root;

    include_once($path_to_root . "reporting/includes/pdf_report.inc");

    $frompart = $_POST['PARAM_0'];
    $topart = $_POST['PARAM_1'];
    $comments = $_POST['PARAM_2'];
    
    $dec = user_qty_dec();

	$cols = array(0, 50, 305, 375, 445,	515);

	$headers = array(_('Component'), _('Description'), _('Loc'), _('Wrk Ctr'), _('Quantity'));

	$aligns = array('left',	'left',	'left', 'left', 'right');

    $params =   array( 	0 => $comments,
    				    1 => array('text' => _('Component'), 'from' => $frompart, 'to' => $topart));

    $rep = new FrontReport(_('Bill of Material Listing'), "BillOfMaterial.pdf", user_pagesize());

    $rep->Font();
    $rep->Info($params, $cols, $headers, $aligns);
    $rep->Header();

	$res = getTransactions($frompart, $topart);
	$parent = '';
	while ($trans=db_fetch($res))
	{
		if ($parent != $trans['parent'])
		{
			if ($parent != '')
			{
				$rep->Line($rep->row - 2);
				$rep->NewLine(2, 3);
			}
			$rep->TextCol(0, 1, $trans['parent']);
			$desc = get_item($trans['parent']);
			$rep->TextCol(1, 2, $desc['description']);
			$parent = $trans['parent'];
			$rep->NewLine();
		}
		
		$rep->NewLine();
		$rep->TextCol(0, 1, $trans['component']);
		$rep->TextCol(1, 2, $trans['CompDescription']);
		$rep->TextCol(2, 3, $trans['loc_code']);
		$rep->TextCol(3, 4, $trans['workcentre_added']);
		$rep->TextCol(4, 5, number_format2($trans['quantity'], $dec));
	}
	$rep->Line($rep->row - 4);
    $rep->End();
}

?>