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
function add_tax_type($name, $sales_gl_code, $purchasing_gl_code, $rate)
{
	$sql = "INSERT INTO ".TB_PREF."tax_types (name, sales_gl_code, purchasing_gl_code, rate)
		VALUES (".db_escape($name).", ".db_escape($sales_gl_code)
		.", ".db_escape($purchasing_gl_code).", $rate)";

	db_query($sql, "could not add tax type");
}

function update_tax_type($type_id, $name, $sales_gl_code, $purchasing_gl_code, $rate)
{
	$sql = "UPDATE ".TB_PREF."tax_types SET name=".db_escape($name).",
		sales_gl_code=".db_escape($sales_gl_code).",
		purchasing_gl_code=".db_escape($purchasing_gl_code).",
		rate=$rate
		WHERE id=$type_id";

	db_query($sql, "could not update tax type");
}

function get_all_tax_types()
{
	$sql = "SELECT ".TB_PREF."tax_types.*,
		Chart1.account_name AS SalesAccountName,
		Chart2.account_name AS PurchasingAccountName
		FROM ".TB_PREF."tax_types, ".TB_PREF."chart_master AS Chart1,
		".TB_PREF."chart_master AS Chart2
		WHERE ".TB_PREF."tax_types.sales_gl_code = Chart1.account_code
		AND ".TB_PREF."tax_types.purchasing_gl_code = Chart2.account_code";

	return db_query($sql, "could not get all tax types");
}

function get_all_tax_types_simple()
{
	$sql = "SELECT * FROM ".TB_PREF."tax_types";

	return db_query($sql, "could not get all tax types");
}

function get_tax_type($type_id)
{
	$sql = "SELECT ".TB_PREF."tax_types.*,
		Chart1.account_name AS SalesAccountName,
		Chart2.account_name AS PurchasingAccountName
		FROM ".TB_PREF."tax_types, ".TB_PREF."chart_master AS Chart1,
		".TB_PREF."chart_master AS Chart2
		WHERE ".TB_PREF."tax_types.sales_gl_code = Chart1.account_code
		AND ".TB_PREF."tax_types.purchasing_gl_code = Chart2.account_code AND id=$type_id";

	$result = db_query($sql, "could not get tax type");

	return db_fetch($result);
}

function get_tax_type_default_rate($type_id)
{
	$sql = "SELECT rate FROM ".TB_PREF."tax_types WHERE id=$type_id";

	$result = db_query($sql, "could not get tax type rate");

	$row = db_fetch_row($result);
	return $row[0];
}

function delete_tax_type($type_id)
{
	begin_transaction();

	$sql = "DELETE FROM ".TB_PREF."tax_types WHERE id=$type_id";

	db_query($sql, "could not delete tax type");

	// also delete any item tax exemptions associated with this type
	$sql = "DELETE FROM ".TB_PREF."item_tax_type_exemptions WHERE tax_type_id=$type_id";

	db_query($sql, "could not delete item tax type exemptions");

	commit_transaction();
}

?>