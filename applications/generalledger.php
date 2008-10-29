<?php

	include_once("./modules/installed_modules.php");
	class general_ledger_app extends application 
	{
		function general_ledger_app() 
		{
			global $installed_modules;
			$this->application("GL",_("Banking and General Ledger"));

			$this->add_module(_("Transactions"));
			$this->add_lapp_function(0, _("&Payments"),"gl/gl_bank.php?NewPayment=Yes");
			$this->add_lapp_function(0, _("&Deposits"),"gl/gl_bank.php?NewDeposit=Yes");
			$this->add_lapp_function(0, _("Bank Account &Transfers"),"gl/bank_transfer.php?");
			$this->add_rapp_function(0, _("&Journal Entry"),"gl/gl_journal.php?NewJournal=Yes");
			$this->add_rapp_function(0, _("&Budget Entry"),"gl/gl_budget.php?");

			$this->add_module(_("Inquiries and Reports"));
			$this->add_lapp_function(1, _("Bank Account &Inquiry"),"gl/inquiry/bank_inquiry.php?");
			$this->add_lapp_function(1, _("GL Account &Inquiry"),"gl/inquiry/gl_account_inquiry.php?");
			$this->add_lapp_function(1, "","");
			$this->add_lapp_function(1, _("Trial &Balance"),"gl/inquiry/gl_trial_balance.php?");

			$this->add_rapp_function(1, _("Banking &Reports"),"reporting/reports_main.php?Class=5");
			$this->add_rapp_function(1, _("General Ledger &Reports"),"reporting/reports_main.php?Class=6");

			$this->add_module(_("Maintenance"));
			$this->add_lapp_function(2, _("Bank &Accounts"),"gl/manage/bank_accounts.php?");
			$this->add_lapp_function(2, _("Payment, Deposit and Transfer &Types"),"gl/manage/bank_trans_types.php?");
			$this->add_lapp_function(2, _("Quick Entries"),"gl/manage/gl_quick_entries.php?");
			$this->add_lapp_function(2, "","");
			$this->add_lapp_function(2, _("&Currencies"),"gl/manage/currencies.php?");
			$this->add_lapp_function(2, _("&Exchange Rates"),"gl/manage/exchange_rates.php?");

			$this->add_rapp_function(2, _("&GL Accounts"),"gl/manage/gl_accounts.php?");
			$this->add_rapp_function(2, _("GL Account &Groups"),"gl/manage/gl_account_types.php?");
			$this->add_rapp_function(2, _("GL Account &Classes"),"gl/manage/gl_account_classes.php?");
			if (count($installed_modules) > 0)
			{
				foreach ($installed_modules as $mod)
				{
					if ($mod["tab"] == "GL")
						$this->add_rapp_function(2, $mod["name"], "modules/".$mod["path"]."/".$mod["filename"]."?");
				}
			}	
		}
	}


?>