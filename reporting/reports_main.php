<?php

$path_to_root="..";
$page_security = 5;
include_once($path_to_root . "/includes/session.inc");

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/data_checks.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/reporting/includes/reports_classes.inc");
$js = "";
if ($use_date_picker)
	$js .= get_js_date_picker();
page(_("Reports and Analysis"), false, false, "", $js, true);

$reports = new BoxReports;

$dim = get_company_pref('use_dimension');

$reports->addReportClass(_('Customer'));
$reports->addReport(_('Customer'),101,_('Customer Balances'),
	array(	new ReportParam(_('End Date'),'DATE'),
			new ReportParam(_('Customer'),'CUSTOMERS_NO_FILTER'),
			new ReportParam(_('Currency Filter'),'CURRENCY'),
			new ReportParam(_('Comments'),'TEXTBOX')));
$reports->addReport(_('Customer'),102,_('Aged Customer Analysis'),
	array(	new ReportParam(_('End Date'),'DATE'),
			new ReportParam(_('Customer'),'CUSTOMERS_NO_FILTER'),
			new ReportParam(_('Currency Filter'),'CURRENCY'),
			new ReportParam(_('Summary Only'),'YES_NO'),
			new ReportParam(_('Graphics'),'GRAPHIC'),
			new ReportParam(_('Comments'),'TEXTBOX')));
$reports->addReport(_('Customer'),103,_('Customer Detail Listing'),
	array(	new ReportParam(_('Activity Since'),'DATEBEGIN'),
			new ReportParam(_('Sales Areas'),'AREAS'),
			new ReportParam(_('Sales Folk'),'SALESMEN'), new ReportParam(_('Activity Greater Than'),'TEXT'), new ReportParam(_('Activity Less Than'),'TEXT'), new ReportParam(_('Comments'),'TEXTBOX')));
$reports->addReport(_('Customer'),104,_('Price Listing'),
	array(	new ReportParam(_('Currency Filter'),'CURRENCY'),
			new ReportParam(_('Inventory Category'),'CATEGORIES'),
			new ReportParam(_('Sales Types'),'SALESTYPES'),
			new ReportParam(_('Show Pictures'),'YES_NO'),
			new ReportParam(_('Show GP %'),'YES_NO'),
			new ReportParam(_('Comments'),'TEXTBOX')));
$reports->addReport(_('Customer'),105,_('Order Status Listing'),
	array(	new ReportParam(_('Start Date'),'DATEBEGINM'),
			new ReportParam(_('End Date'),'DATEENDM'),
			new ReportParam(_('Inventory Category'),'CATEGORIES'),
			new ReportParam(_('Stock Location'),'LOCATIONS'),
			new ReportParam(_('Back Orders Only'),'YES_NO'),
			new ReportParam(_('Comments'),'TEXTBOX')));
$reports->addReport(_('Customer'),106,_('Salesman Listing'),
	array(	new ReportParam(_('Start Date'),'DATEBEGINM'),
			new ReportParam(_('End Date'),'DATEENDM'),
			new ReportParam(_('Summary Only'),'YES_NO'),
			new ReportParam(_('Comments'),'TEXTBOX')));
$reports->addReport(_('Customer'),107,_('Print Invoices/Credit Notes'),
	array(	new ReportParam(_('From'),'INVOICE'),
			new ReportParam(_('To'),'INVOICE'),
			new ReportParam(_('Currency Filter'),'CURRENCY'),
			new ReportParam(_('Bank Account'),'BANK_ACCOUNTS'),
			new ReportParam(_('email Customers'),'YES_NO'),
			new ReportParam(_('Payment Link'),'PAYMENT_LINK'),
			new ReportParam(_('Comments'),'TEXTBOX')));
$reports->addReport(_('Customer'),110,_('Print Deliveries'),
	array(	new ReportParam(_('From'),'DELIVERY'),
			new ReportParam(_('To'),'DELIVERY'),
			new ReportParam(_('email Customers'),'YES_NO'),
			new ReportParam(_('Comments'),'TEXTBOX')));
$reports->addReport(_('Customer'),108,_('Print Statements'),
	array(	new ReportParam(_('Customer'),'CUSTOMERS_NO_FILTER'),
			new ReportParam(_('Currency Filter'),'CURRENCY'),
			new ReportParam(_('Bank Account'),'BANK_ACCOUNTS'),
			new ReportParam(_('Email Customers'),'YES_NO'),
			new ReportParam(_('Comments'),'TEXTBOX')));
$reports->addReport(_('Customer'),109,_('Print Sales Orders'),
	array(	new ReportParam(_('From'),'ORDERS'),
			new ReportParam(_('To'),'ORDERS'),
			new ReportParam(_('Currency Filter'),'CURRENCY'),
			new ReportParam(_('Bank Account'),'BANK_ACCOUNTS'),
			new ReportParam(_('Email Customers'),'YES_NO'),
			new ReportParam(_('Print as Quote'),'YES_NO'),
			new ReportParam(_('Comments'),'TEXTBOX')));

$reports->addReportClass(_('Supplier'));
$reports->addReport(_('Supplier'),201,_('Supplier Balances'),
	array(	new ReportParam(_('End Date'),'DATE'),
			new ReportParam(_('Supplier'),'SUPPLIERS_NO_FILTER'),
			new ReportParam(_('Currency Filter'),'CURRENCY'),
			new ReportParam(_('Comments'),'TEXTBOX')));
$reports->addReport(_('Supplier'),202,_('Aged Supplier Analyses'),
	array(	new ReportParam(_('End Date'),'DATE'),
			new ReportParam(_('Supplier'),'SUPPLIERS_NO_FILTER'),
			new ReportParam(_('Currency Filter'),'CURRENCY'),
			new ReportParam(_('Summary Only'),'YES_NO'),
			new ReportParam(_('Graphics'),'GRAPHIC'),
			new ReportParam(_('Comments'),'TEXTBOX')));
$reports->addReport(_('Supplier'),203,_('Payment Report'),
	array(	new ReportParam(_('End Date'),'DATE'),
			new ReportParam(_('Supplier'),'SUPPLIERS_NO_FILTER'),
			new ReportParam(_('Currency Filter'),'CURRENCY'),
			new ReportParam(_('Comments'),'TEXTBOX')));
$reports->addReport(_('Supplier'),204,_('Outstanding GRNs Report'),
	array(	new ReportParam(_('Supplier'),'SUPPLIERS_NO_FILTER'),
			new ReportParam(_('Comments'),'TEXTBOX')));
$reports->addReport(_('Supplier'),209,_('Print Purchase Orders'),
	array(	new ReportParam(_('From'),'PO'),
			new ReportParam(_('To'),'PO'),
			new ReportParam(_('Currency Filter'),'CURRENCY'),
			new ReportParam(_('Bank Account'),'BANK_ACCOUNTS'),
			new ReportParam(_('Email Customers'),'YES_NO'),
			new ReportParam(_('Comments'),'TEXTBOX')));

$reports->addReportClass(_('Inventory'));
$reports->addReport(_('Inventory'),301,_('Inventory Valuation Report'),
	array(	new ReportParam(_('Inventory Category'),'CATEGORIES'),
			new ReportParam(_('Location'),'LOCATIONS'),
			new ReportParam(_('Detailed Report'),'YES_NO'),
			new ReportParam(_('Comments'),'TEXTBOX')));
$reports->addReport(_('Inventory'),302,_('Inventory Planning Report'),
	array(	new ReportParam(_('Inventory Category'),'CATEGORIES'),
			new ReportParam(_('Location'),'LOCATIONS'),
			new ReportParam(_('Comments'),'TEXTBOX')));
$reports->addReport(_('Inventory'),303,_('Stock Check Sheets'),
	array(	new ReportParam(_('Inventory Category'),'CATEGORIES'),
			new ReportParam(_('Location'),'LOCATIONS'),
			new ReportParam(_('Show Pictures'),'YES_NO'),
			new ReportParam(_('Comments'),'TEXTBOX')));
$reports->addReport(_('Inventory'),304,_('Inventory Sales Report'),
	array(	new ReportParam(_('Start Date'),'DATEBEGINM'),
			new ReportParam(_('End Date'),'DATEENDM'),
			new ReportParam(_('Inventory Category'),'CATEGORIES'),
			new ReportParam(_('Location'),'LOCATIONS'),
			new ReportParam(_('Detailed Report'),'YES_NO'),
			new ReportParam(_('Comments'),'TEXTBOX')));

$reports->addReportClass(_('Manufactoring'));
$reports->addReport(_('Manufactoring'),401,_('Bill of Material Listing'),
	array(	new ReportParam(_('From component'),'ITEMS'),
			new ReportParam(_('To component'),'ITEMS'),
			new ReportParam(_('Comments'),'TEXTBOX')));
	$reports->addReportClass(_('Dimensions'));
if ($dim > 0)
{
	$reports->addReport(_('Dimensions'),501,_('Dimension Summary'),
	array(	new ReportParam(_('From Dimension'),'DIMENSION'),
			new ReportParam(_('To Dimension'),'DIMENSION'),
			new ReportParam(_('Show Balance'),'YES_NO'),
			new ReportParam(_('Comments'),'TEXTBOX')));
	//$reports->addReport(_('Dimensions'),502,_('Dimension Details'),
	//array(	new ReportParam(_('Dimension'),'DIMENSIONS'),
	//		new ReportParam(_('Comments'),'TEXTBOX')));
}
$reports->addReportClass(_('Banking'));
//$reports->addReport(_('Banking'),601,_('Bank Account Transactions'),
//	array(	new ReportParam(_('Bank Accounts'),'BANK_ACCOUNTS'),
//			new ReportParam(_('Start Date'),'DATE'),
//			new ReportParam(_('End Date'),'DATE'),
//			new ReportParam(_('Comments'),'TEXTBOX')));

$reports->addReportClass(_('General Ledger'));
$reports->addReport(_('General Ledger'),701,_('Chart of Accounts'),
	array(	new ReportParam(_('Show Balances'),'YES_NO'),
			new ReportParam(_('Comments'),'TEXTBOX')));
$reports->addReport(_('General Ledger'),702,_('List of Journal Entries'),
	array(	new ReportParam(_('Start Date'),'DATEBEGINM'),
			new ReportParam(_('End Date'),'DATEENDM'),
			new ReportParam(_('Type'),'SYS_TYPES'),
			new ReportParam(_('Comments'),'TEXTBOX')));
//$reports->addReport(_('General Ledger'),703,_('GL Account Group Summary'),
//	array(	new ReportParam(_('Comments'),'TEXTBOX')));
if ($dim == 2)
{
	$reports->addReport(_('General Ledger'),704,_('GL Account Transactions'),
	array(	new ReportParam(_('Start Date'),'DATEBEGINM'),
			new ReportParam(_('End Date'),'DATEENDM'),
			new ReportParam(_('From Account'),'GL_ACCOUNTS'),
			new ReportParam(_('To Account'),'GL_ACCOUNTS'),
			new ReportParam(_('Dimension')." 1", 'DIMENSIONS1'),
			new ReportParam(_('Dimension')." 2", 'DIMENSIONS2'),
			new ReportParam(_('Comments'),'TEXTBOX')));
	$reports->addReport(_('General Ledger'),705,_('Annual Expense Breakdown'),
	array(	new ReportParam(_('Year'),'TRANS_YEARS'),
			new ReportParam(_('Dimension')." 1", 'DIMENSIONS1'),
			new ReportParam(_('Dimension')." 2", 'DIMENSIONS2'),
			new ReportParam(_('Comments'),'TEXTBOX')));
	$reports->addReport(_('General Ledger'),706,_('Balance Sheet'),
	array(	new ReportParam(_('Start Date'),'DATEBEGIN'),
			new ReportParam(_('End Date'),'DATEENDM'),
			new ReportParam(_('Dimension')." 1", 'DIMENSIONS1'),
			new ReportParam(_('Dimension')." 2", 'DIMENSIONS2'),
			new ReportParam(_('Graphics'),'GRAPHIC'),
			new ReportParam(_('Comments'),'TEXTBOX')));
	$reports->addReport(_('General Ledger'),707,_('Profit and Loss Statement'),
	array(	new ReportParam(_('Start Date'),'DATEBEGINM'),
			new ReportParam(_('End Date'),'DATEENDM'),
			new ReportParam(_('Compare to'),'COMPARE'),
			new ReportParam(_('Dimension')." 1", 'DIMENSIONS1'),
			new ReportParam(_('Dimension')." 2", 'DIMENSIONS2'),
			new ReportParam(_('Graphics'),'GRAPHIC'),
			new ReportParam(_('Comments'),'TEXTBOX')));
	$reports->addReport(_('General Ledger'),708,_('Trial Balance'),
	array(	new ReportParam(_('Start Date'),'DATEBEGINM'),
			new ReportParam(_('End Date'),'DATEENDM'),
			new ReportParam(_('Zero values'),'YES_NO'),
			new ReportParam(_('Dimension')." 1", 'DIMENSIONS1'),
			new ReportParam(_('Dimension')." 2", 'DIMENSIONS2'),
			new ReportParam(_('Comments'),'TEXTBOX')));
}
else if ($dim == 1)
{
	$reports->addReport(_('General Ledger'),704,_('GL Account Transactions'),
	array(	new ReportParam(_('Start Date'),'DATEBEGINM'),
			new ReportParam(_('End Date'),'DATEENDM'),
			new ReportParam(_('From Account'),'GL_ACCOUNTS'),
			new ReportParam(_('To Account'),'GL_ACCOUNTS'),
			new ReportParam(_('Dimension'), 'DIMENSIONS1'),
			new ReportParam(_('Comments'),'TEXTBOX')));
	$reports->addReport(_('General Ledger'),705,_('Annual Expense Breakdown'),
	array(	new ReportParam(_('Year'),'TRANS_YEARS'),
			new ReportParam(_('Dimension'), 'DIMENSIONS1'),
			new ReportParam(_('Comments'),'TEXTBOX')));
	$reports->addReport(_('General Ledger'),706,_('Balance Sheet'),
	array(	new ReportParam(_('Start Date'),'DATEBEGIN'),
			new ReportParam(_('End Date'),'DATEENDM'),
			new ReportParam(_('Dimension'), 'DIMENSIONS1'),
			new ReportParam(_('Graphics'),'GRAPHIC'),
			new ReportParam(_('Comments'),'TEXTBOX')));
	$reports->addReport(_('General Ledger'),707,_('Profit and Loss Statement'),
	array(	new ReportParam(_('Start Date'),'DATEBEGINM'),
			new ReportParam(_('End Date'),'DATEENDM'),
			new ReportParam(_('Compare to'),'COMPARE'),
			new ReportParam(_('Dimension'), 'DIMENSIONS1'),
			new ReportParam(_('Graphics'),'GRAPHIC'),
			new ReportParam(_('Comments'),'TEXTBOX')));
	$reports->addReport(_('General Ledger'),708,_('Trial Balance'),
	array(	new ReportParam(_('Start Date'),'DATEBEGINM'),
			new ReportParam(_('End Date'),'DATEENDM'),
			new ReportParam(_('Zero values'),'YES_NO'),
			new ReportParam(_('Dimension'), 'DIMENSIONS1'),
			new ReportParam(_('Comments'),'TEXTBOX')));
}
else
{
	$reports->addReport(_('General Ledger'),704,_('GL Account Transactions'),
	array(	new ReportParam(_('Start Date'),'DATEBEGINM'),
			new ReportParam(_('End Date'),'DATEENDM'),
			new ReportParam(_('From Account'),'GL_ACCOUNTS'),
			new ReportParam(_('To Account'),'GL_ACCOUNTS'),
			new ReportParam(_('Comments'),'TEXTBOX')));
	$reports->addReport(_('General Ledger'),705,_('Annual Expense Breakdown'),
	array(	new ReportParam(_('Year'),'TRANS_YEARS'),
			new ReportParam(_('Comments'),'TEXTBOX')));
	$reports->addReport(_('General Ledger'),706,_('Balance Sheet'),
	array(	new ReportParam(_('Start Date'),'DATEBEGIN'),
			new ReportParam(_('End Date'),'DATEENDM'),
			new ReportParam(_('Graphics'),'GRAPHIC'),
			new ReportParam(_('Comments'),'TEXTBOX')));
	$reports->addReport(_('General Ledger'),707,_('Profit and Loss Statement'),
	array(	new ReportParam(_('Start Date'),'DATEBEGINM'),
			new ReportParam(_('End Date'),'DATEENDM'),
			new ReportParam(_('Compare to'),'COMPARE'),
			new ReportParam(_('Graphics'),'GRAPHIC'),
			new ReportParam(_('Comments'),'TEXTBOX')));
	$reports->addReport(_('General Ledger'),708,_('Trial Balance'),
	array(	new ReportParam(_('Start Date'),'DATEBEGINM'),
			new ReportParam(_('End Date'),'DATEENDM'),
			new ReportParam(_('Zero values'),'YES_NO'),
			new ReportParam(_('Comments'),'TEXTBOX')));
}
$reports->addReport(_('General Ledger'),709,_('Tax Report'),
	array(	new ReportParam(_('Start Date'),'DATEBEGINTAX'),
			new ReportParam(_('End Date'),'DATEENDTAX'),
			new ReportParam(_('Summary Only'),'YES_NO'),
			new ReportParam(_('Comments'),'TEXTBOX')));

echo "
<form method=post>
	<input type='hidden' name='REP_ID' value=''>
	<input type='hidden' name='PARAM_COUNT' value=''>
	<input type='hidden' name='PARAM_0' value=''>
	<input type='hidden' name='PARAM_1' value=''>
	<input type='hidden' name='PARAM_2' value=''>
	<input type='hidden' name='PARAM_3' value=''>
	<input type='hidden' name='PARAM_4' value=''>
	<input type='hidden' name='PARAM_5' value=''>
	<input type='hidden' name='PARAM_6' value=''>

	<script language='javascript'>
		function onWindowLoad() {
			showClass(" . $_GET['Class'] . ")
		}
	Behaviour.addLoadEvent(onWindowLoad);
	</script>
";
echo $reports->getDisplay();
echo "</form>";

end_page();
?>