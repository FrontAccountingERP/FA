<?php

//-------------------------------------------------------------------

function begin_transaction()
{
	db_query("BEGIN", "could not start a transaction");
}

function commit_transaction()
{
	db_query("COMMIT", "could not commit a transaction");
}

function cancel_transaction()
{
	db_query("ROLLBACK", "could not cancel a transaction");	
}

?>
