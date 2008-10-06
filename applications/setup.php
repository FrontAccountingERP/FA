<?php

	include_once("./modules/installed_modules.php");
	class setup_app extends application
	{
		function setup_app()
		{
			global $installed_modules;
			$this->application("system",_("Setup"));

			$this->add_module(_("Company Setup"));
			$this->add_lapp_function(0, _("&Company Setup"),"admin/company_preferences.php?");
			$this->add_lapp_function(0, _("&User Accounts Setup"),"admin/users.php?", 15);
			$this->add_lapp_function(0, "","");
			$this->add_lapp_function(0, _("&Display Setup"),"admin/display_prefs.php?");
			$this->add_lapp_function(0, _("&Forms Setup"),"admin/forms_setup.php?");
			$this->add_rapp_function(0, _("&Taxes"),"taxes/tax_types.php?");
			$this->add_rapp_function(0, _("Tax &Groups"),"taxes/tax_groups.php?");
			$this->add_rapp_function(0, "","");
			$this->add_rapp_function(0, _("System and &General GL Setup"),"admin/gl_setup.php?");
			$this->add_rapp_function(0, _("&Fiscal Years"),"admin/fiscalyears.php?");

			$this->add_module(_("Miscellaneous"));
			$this->add_lapp_function(1, _("Pa&yment Terms"),"admin/payment_terms.php?");
			$this->add_lapp_function(1, _("Shi&pping Company"),"admin/shipping_companies.php?");

			$this->add_module(_("Maintanance"));
			$this->add_lapp_function(2, _("&Void a Transaction"),"admin/void_transaction.php?");
			$this->add_lapp_function(2, _("View or &Print Transactions"),"admin/view_print_transaction.php?");
			$this->add_rapp_function(2, _("&Backup and Restore"),"admin/backups.php?", 15);
			$this->add_rapp_function(2, _("Create/Update &Companies"),"admin/create_coy.php?", 14);
			$this->add_rapp_function(2, _("Install/Update &Languages"),"admin/inst_lang.php?", 14);
			$this->add_rapp_function(2, _("Install/Update &Modules"),"admin/inst_module.php?", 15);
			if (count($installed_modules) > 0)
			{
				foreach ($installed_modules as $mod)
				{
					if ($mod["tab"] == "system")
						$this->add_rapp_function(2, $mod["name"], "modules/".$mod["path"]."/".$mod["filename"]."?");
				}
			}
		}
	}


?>