<?php

	class dimensions_app extends application 
	{
		function dimensions_app() 
		{
			$dim = get_company_pref('use_dimension');
			$this->application("proj",_("Dimensions"));

			if ($dim > 0)
			{
				$this->add_module(_("Transactions"));
				$this->add_lapp_function(0, _("Dimension Entry"),"dimensions/dimension_entry.php?");
				$this->add_lapp_function(0, _("Outstanding Dimensions"),"dimensions/inquiry/search_dimensions.php?OutstandingOnly=1");

				$this->add_module(_("Inquiries and Reports"));
				$this->add_lapp_function(1, _("Dimension Inquiry"),"dimensions/inquiry/search_dimensions.php?");

				$this->add_rapp_function(1, _("Dimension Reports"),"reporting/reports_main.php?Class=4");
			}
		}
	}


?>