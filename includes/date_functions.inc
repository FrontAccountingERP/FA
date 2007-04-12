<?php

/*
date validation and parsing functions

These functions refer to the global variable defining the date format
The date format is defined in config.php called DefaultDateFormat
this can be a string either "d/m/Y" for UK/Australia/New Zealand dates or
"m/d/Y" for US/Canada format dates

*/

function is_date($date_) 
{
	global $dateseps;

	if ($date_ == null || $date_ == "")
		return 0;
	$how = user_date_format();
	$sep = $dateseps[user_date_sep()];

	$date_ = trim($date_);
	$date_ = str_replace($sep, "", $date_);
	if (strlen($date_) == 6) 
	{
		if ($how == 0) 
		{
			$date_array[0] = substr($date_,2,2);
			$date_array[1] = substr($date_,0,2);
			$date_array[2] = substr($date_,4,2);
		} 
		elseif ($how == 1) 
		{
			$date_array[0] = substr($date_,0,2);
			$date_array[1] = substr($date_,2,2);
			$date_array[2] = substr($date_,4,2);
		} 
		else 
		{
			$date_array[0] = substr($date_,2,2);
			$date_array[1] = substr($date_,4,2);
			$date_array[2] = substr($date_,0,2);
		}
	} 
	elseif (strlen($date_) == 8) 
	{
		if ($how == 0) 
		{
			$date_array[0] = substr($date_,2,2);
			$date_array[1] = substr($date_,0,2);
			$date_array[2] = substr($date_,4,4);
		} 
		elseif ($how == 1) 
		{
			$date_array[0] = substr($date_,0,2);
			$date_array[1] = substr($date_,2,2);
			$date_array[2] = substr($date_,4,4);
		} 
		else 
		{
			$date_array[0] = substr($date_,6,2);
			$date_array[1] = substr($date_,4,2);
			$date_array[2] = substr($date_,0,4);
		}
	}
	if ((int)$date_array[2] > 9999) 
	{
		return 0;
	}


	if (is_long((int)$date_array[0]) && is_long((int)$date_array[1]) && is_long((int)$date_array[2]))
	{
		if (checkdate((int)$date_array[1],(int)$date_array[0],(int)$date_array[2]))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{ /*Can't be in an appropriate DefaultDateFormat */
		return 0;
	}
} //end of is_date function


function Today() 
{
	return date(user_date_display());
}

function Now() 
{
	if (user_date_format() == 0)
		return date("h:i a");
	else
		return date("H:i");
}

function is_date_in_fiscalyear($date, $convert=false)
{
	global $path_to_root;
	include_once($path_to_root . "/admin/db/company_db.inc");

	$myrow = get_current_fiscalyear();
	if ($myrow['closed'] == 1)
		return 0;
	if ($convert)
		$date2 = sql2date($date);
	else
		$date2 = $date;
	$begin = sql2date($myrow['begin']);
	$end = sql2date($myrow['end']);
	if (date1_greater_date2($begin, $date2) || date1_greater_date2($date2, $end))
	{
		return 0;
	}
	return 1;
}

function begin_fiscalyear()
{
	global $path_to_root;
	include_once($path_to_root . "/admin/db/company_db.inc");

	$myrow = get_current_fiscalyear();
	return sql2date($myrow['begin']);
}

function end_fiscalyear()
{
	global $path_to_root;
	include_once($path_to_root . "/admin/db/company_db.inc");

	$myrow = get_current_fiscalyear();
	return sql2date($myrow['end']);
}

function begin_month($date)
{
    $date_array = explode_date_to_dmy($date);
    return date(user_date_display(), Mktime(0,0,0, $date_array[1], 1, $date_array[2]));
}

function enc_month($date)
{
    $date_array = explode_date_to_dmy($date);
    return date(user_date_display(), Mktime(0,0,0, $date_array[1] + 1, 0, $date_array[2]));
}

function add_days($date, $days) // accepts - values as well
{
    $date_array = explode_date_to_dmy($date);
    return date(user_date_display(), Mktime(0,0,0, $date_array[1], $date_array[0] + $days, $date_array[2]));
}

function add_months($date, $months)
{
    $date_array = explode_date_to_dmy($date);
    return date(user_date_display(), Mktime(0,0,0, $date_array[1] + $months, $date_array[0], $date_array[2]));
}

function add_years($date, $years)
{
    $date_array = explode_date_to_dmy($date);
    return date(user_date_display(), Mktime(0,0,0, $date_array[1], $date_array[0], $date_array[2] + $years));
}

//_______________________________________________________________

function sql2date($date_) 
{
	global $dateseps;

	//for MySQL dates are in the format YYYY-mm-dd
	$date_array = array();
	$how = user_date_format();
	$sep = $dateseps[user_date_sep()];
	if ($date_ == null || strlen($date_) == 0)
		return "";

	if (strpos($date_, "/")) 
	{ // In MySQL it could be either / or -
		$date_array = explode("/",$date_);
	} 
	elseif (strpos ($date_, "-")) 
	{
		$date_array = explode("-",$date_);
	}

	if (strlen($date_array[2]) > 4) 
	{  /*chop off the time stuff */
		$date_array[2]= substr($date_array[2],0,2);
	}

	if ($how == 1)
	{
		return $date_array[2].$sep.$date_array[1].$sep.$date_array[0];
	} 
	elseif ($how == 0)
	{
		return $date_array[1].$sep.$date_array[2].$sep.$date_array[0];
	} 
	else 
	{
		return $date_array[0].$sep.$date_array[1].$sep.$date_array[2];
	}

} // end function sql2date


function date2sql($date_)
{
	global $dateseps;
/* takes a date in a the format specified in $DefaultDateFormat
and converts to a yyyy/mm/dd format */

	$how = user_date_format();
	$sep = $dateseps[user_date_sep()];

	if ($date_ == null || strlen($date_) == 0)
		return "";

	$date_ =trim($date_);
	$date_ = str_replace($sep, "", $date_);

	if (strlen($date_) == 6)
	{
		if ($how == 0)
		{
			$date_array[0] = substr($date_,2,2);
			$date_array[1] = substr($date_,0,2);
			$date_array[2] = substr($date_,4,2);
		} 
		elseif ($how == 1)
		{
			$date_array[0] = substr($date_,0,2);
			$date_array[1] = substr($date_,2,2);
			$date_array[2] = substr($date_,4,2);
		} 
		else
		{
			$date_array[0]= substr($date_,4,2);
			$date_array[1]= substr($date_,2,2);
			$date_array[2]= substr($date_,0,2);
		}
	}
	elseif (strlen($date_) == 8)
	{
		if ($how == 0)
		{
			$date_array[0] = substr($date_,2,2);
			$date_array[1] = substr($date_,0,2);
			$date_array[2] = substr($date_,4,4);
		} 
		elseif ($how == 1)
		{
			$date_array[0] = substr($date_,0,2);
			$date_array[1] = substr($date_,2,2);
			$date_array[2] = substr($date_,4,4);
		} 
		else
		{
			$date_array[0] = substr($date_,6,2);
			$date_array[1] = substr($date_,4,2);
			$date_array[2] = substr($date_,0,4);
		}
	}

//to modify assumption in 2030

	if ((int)$date_array[2] < 60)
	{
		$date_array[2] = "20".$date_array[2];
	} 
	elseif ((int)$date_array[2] > 59 && (int)$date_array[2] < 100)
	{
		$date_array[2] = "19".$date_array[2];
	}
	elseif ((int)$date_array[2] > 9999)
	{
		return 0;
	}
	return $date_array[2]."/".$date_array[1]."/".$date_array[0];
}// end of function

function date1_greater_date2 ($date1, $date2) 
{

/* returns 1 true if date1 is greater than date_ 2 */

	$date1 = date2sql($date1);
	$date2 = date2sql($date2);
	$date1_array = explode("/", $date1);
	$date2_array = explode("/", $date2);

	/*The 0 element of the array will be the year in either d/m/Y or m/d/Y formats */

	if ($date1_array[0] > $date2_array[0])
	{
		return 1;
	}
	elseif ($date1_array[0] == $date2_array[0])
	{
		if ($date1_array[1] > $date2_array[1])
		{
			return 1;
		}
		elseif ($date1_array[1] == $date2_array[1])
		{
			if ($date1_array[2] > $date2_array[2])
			{
				return 1;
			}
		}
	}
	return 0;
}


function date_diff ($date1, $date2, $period) 
{

/* expects dates in the format specified in $DefaultDateFormat - period can be one of 'd','w','y','m'
months are assumed to be 30 days and years 365.25 days This only works
provided that both dates are after 1970. Also only works for dates up to the year 2035 ish */

	$date1 = date2sql($date1);
	$date2 = date2sql($date2);
	$date1_array = explode("/", $date1);
	$date2_array = explode("/", $date2);

	$date1_Stamp = mktime(0,0,0, (int)$date1_array[1],(int)$date1_array[2],(int)$date1_array[0]);
	$date2_Stamp = mktime(0,0,0, (int)$date2_array[1],(int)$date2_array[2],(int)$date2_array[0]);
	$difference = $date1_Stamp - $date2_Stamp;

/* difference is the number of seconds between each date negative if date_ 2 > date_ 1 */

	switch ($period) 
	{
		case "d":
			return (int) ($difference/(24*60*60));
			break;
		case "w":
			return (int) ($difference/(24*60*60*7));
			break;
		case "m":
			return (int) ($difference/(24*60*60*30));
			break;
		case "s":
			return $difference;
			break;
		case "y":
			return (int) ($difference/(24*60*60*365.25));
			break;
		default:
			Return 0;
	}
}

Function explode_date_to_dmy($date_)
{
	$date_ = date2sql($date_);
	if ($date_ == "") 
	{
		$disp = user_date_display();
		echo "<br>Dates must be entered in the format $disp. Sent was $date_<br>";
		exit;
	}
	$date_array = explode("/", $date_);
	$temp = $date_array[0];
	$date_array[0] = $date_array[2];
	$date_array[2] = $temp;
	return $date_array;
}

Function get_day($date_)
{
	$date_array = explode_date_to_dmy($date_);

	return $date_array[0];
}

Function get_month($date_)
{
	$date_array = explode_date_to_dmy($date_);

	return $date_array[1];
}

Function get_year($date_)
{
	$date_array = explode_date_to_dmy($date_);

	return $date_array[2];
}




?>
