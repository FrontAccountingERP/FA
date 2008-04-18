<?php

//--------------------------------------------------------------------------------------------------

function get_comments($type, $type_no)
{
	$sql = "SELECT * FROM ".TB_PREF."comments WHERE type=$type AND id=$type_no";

	return db_query($sql, "could not query comments transaction table");
}

//--------------------------------------------------------------------------------------------------

function add_comments($type, $type_no, $date_, $memo_)
{
	if ($memo_ != null && $memo_ != "")
	{
    	$date = date2sql($date_);
    	$sql = "INSERT INTO ".TB_PREF."comments (type, id, date_, memo_)
    		VALUES ($type, $type_no, '$date', ".db_quote($memo_).")";

    	db_query($sql, "could not add comments transaction entry");
	}
}

//--------------------------------------------------------------------------------------------------

function update_comments($type, $id, $date_, $memo_)
{
	if ($date_ == null)
	{
    	delete_comments($type, $id);
    	add_comments($type, $id, '', $memo_);
	}
	else
	{
		$date = date2sql($date_);
    	$sql = "UPDATE ".TB_PREF."comments SET memo_=".db_quote($memo_)." WHERE type=$type AND id=$id AND date_='$date'";
    	db_query($sql, "could not update comments");
	}
}

//--------------------------------------------------------------------------------------------------

function delete_comments($type, $type_no)
{
	$sql = "DELETE FROM ".TB_PREF."comments WHERE type=$type AND id=$type_no";

	db_query($sql, "could not delete from comments transaction table");
}

//--------------------------------------------------------------------------------------------------

?>