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

class fa2_5 extends fa_patch {
	var $previous = '2.4.1';		// applicable database version
	var $version = '2.5.0';	// version installed
	var $description;
	var $sql = 'alter2.5.sql';
	var	$max_upgrade_time = 900;
	
	function __construct() {
		parent::__construct();
		$this->description = _('Upgrade from version 2.4 to 2.5');
	}

    /*
	    Shows parameters to be selected before upgrade (if any)
	*/
    function show_params($company)
	{

		display_note(_('Check upgrade parameters below and start upgrade.'));
		start_table(TABLESTYLE);
		table_section(1);
		table_section_title(_("Shipments category default settings"));
		text_row(_("Category Name:"), 'shipping_cat_description', _('Shippment'), 30, 30);
		item_tax_types_list_row(_('Item Tax Type:'), 'shipping_tax_type');
		stock_units_list_row(_('Unit of measure:'), 'shipping_units');
		gl_all_accounts_list_row(_("Sales Account:"), 'shipping_sales_act', get_company_pref('freight_act'));
		gl_all_accounts_list_row(_("C.O.G.S. Account:"), 'shipping_cogs_act', get_company_pref('default_cogs_act'));
		end_table();
		br();

    }

    /*
	    Fetch & check upgrade parameters, check additional upgrade pre-conditions, set SQL variables.
		This function is run after successfull switching to target database connection, before sql upgrade script is run.
    */
	function prepare()
    {
    	// set upgrade script parameters
    	foreach( array('shipping_cat_description', 'shipping_tax_type', 'shipping_units', 'shipping_sales_act', 'shipping_cogs_act') as $name)
			db_query("SET @$name=".db_escape(get_post($name)));
		return true;
	}


	/*
		Install procedure. All additional changes 
		not included in sql file should go here.
	*/
	function install($company, $force=false)
	{
		return true;
	}

	/*
		Optional procedure done after upgrade fail, before backup is restored
	*/
	function post_fail($company)
	{
	}

}

$install = new fa2_5;
