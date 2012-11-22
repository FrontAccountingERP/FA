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
include_once($path_to_root . "/includes/date_functions.inc");

include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/banking.inc");

//----------------------------------------------------------------------------
// helper functions for script execution control
//
function processing_start()
{
	page_processing(false);
    processing_end();
    $_SESSION['Processing'] = $_SERVER['PHP_SELF'];
}

function processing_end()
{
	page_processing(true);
    unset($_SESSION['Processing']);
    if ( isset($_SESSION['Items']) ) {
		unset($_SESSION['Items']->line_items);
		unset($_SESSION['Items']);
    }
}

function processing_active()
{
    return (isset($_SESSION['Processing']) && $_SESSION['Processing']==$_SERVER['PHP_SELF']);
}
/*
	Check if the cart was not destroyed during opening the edition page in
	another browser tab.
*/
function check_edit_conflicts($cartname='Items')
{
	global $Ajax, $no_check_edit_conflicts;

	if ((!isset($no_check_edit_conflicts) || $no_check_edit_conflicts==0) && get_post('cart_id') && $_POST['cart_id'] != $_SESSION[$cartname]->cart_id) {
		display_error(_('This edit session has been abandoned by opening sales document in another browser tab. You cannot edit more than one sales document at once.'));
		$Ajax->activate('_page_body');
		display_footer_exit();
	}
}

?>