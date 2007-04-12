<?php

function display_error($msg, $center=true)
{
    echo "<center><table border='1' cellpadding='3' cellspacing='0' style='border-collapse: collapse' bordercolor='#CC3300' width='98%'>
      <tr>
        <td  " . ($center?"align='center' ":"") . " width='100%' bgcolor='#ffcccc'><font color='#dd2200'>$msg</font></td>
      </tr>
    </table></center><br>\n";	
    	
	//echo "<span class='errortext'><B>" . _("ERROR :") . "</B> $msg</span><BR>";
}

function display_notification($msg, $center=true)
{
    echo "<center><table border='1' cellpadding='3' cellspacing='0' style='border-collapse: collapse' bordercolor='#33cc00' width='98%'>
      <tr>
        <td " . ($center?"align='center' ":"") . " width='100%' bgcolor='#ccffcc'><font color='#007700'>$msg</font></td>
      </tr>
    </table></center><br>\n";	
}

function display_notification_centered($msg)
{
	display_notification($msg, true);
}

function display_heading($msg)
{
	echo "<center><span class='headingtext'>$msg</span></center>\n";	
}

function display_heading2($msg)
{
	echo "<center><span class='headingtext2'>$msg</span></center>\n";	
}

function display_note($msg, $br=0, $br2=0, $extra="")
{
	for ($i = 0; $i < $br; $i++)
		echo "<br>";
	if ($extra != "")	
		echo "<center><span $extra>$msg</span></center>\n";	
	else	
		echo "<center><span>$msg</span></center>\n";	
	for ($i = 0; $i < $br2; $i++)
		echo "<br>";
}

function stock_item_heading($stock_id)
{
	if ($stock_id != "") 
	{
		$result = db_query("SELECT description, units FROM ".TB_PREF."stock_master WHERE stock_id='$stock_id'");		
        $myrow = db_fetch_row($result);
        
    	display_heading("$stock_id - $myrow[0]"); 
    	$units = $myrow[1];
    	display_heading2(_("in units of : ") . $units);        	
	}
}

?>