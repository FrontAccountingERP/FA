<?php

//---------------------------------------------------------------------------------

function start_form($multi=false, $sid=false, $action="", $name="")
{
	if ($name != "")
		$name = "name='$name'";
	if ($action == "")
		$action = $_SERVER['PHP_SELF'];
	if ($sid)
	{
		if (strpos($action, "?"))
			$action .= "&" . SID;
		else	
			$action .= "?" . SID;
	}		
	if ($multi)
		echo "<form enctype='multipart/form-data' method='post' action='$action' $name>\n";	
	else
		echo "<form method='post' action='$action' $name>\n";	
}

//---------------------------------------------------------------------------------

function end_form($breaks=0)
{
	if ($breaks)
		br($breaks);
	echo "</form>\n";
}

function start_table($extra="", $padding='2', $spacing='0')
{
	echo "<center><table";
	if ($extra != "")
		echo " $extra";
	echo " cellpadding=$padding cellspacing=$spacing>\n";
}	

function end_table($breaks=0)
{
	echo "</table></center>\n";
	if ($breaks)
		br($breaks);
}

function meta_forward($forward_to, $params="")
{
	echo "<meta http-equiv='Refresh' content='0; url=$forward_to?$params'>\n";
	echo "<center><br>" . _("You should automatically be forwarded.");
	echo " " . _("If this does not happen") . " " . "<a href='$forward_to'>" . _("click here") . "</a> " . _("to continue") . ".<br><br>\n";
	exit;
}

//-----------------------------------------------------------------------------------

function hyperlink_back($center=true)
{
	echo  get_js_go_back();

	if ($center)
		echo "<center>";
	echo "<a href='javascript:goBack();'>"._("Back")."</a>\n";
	if ($center)
		echo "</center>";
	echo "<br>";	
}

function hyperlink_no_params($target, $label, $center=true)
{
	if ($center)
		echo "<br><center>";
	echo "<a href='$target?=" . SID . "'>$label</a>\n";
	if ($center)
		echo "</center>";
}

function hyperlink_no_params_td($target, $label)
{
	echo "<td>";
	hyperlink_no_params($target, $label);
	echo "</td>\n";
}

//-----------------------------------------------------------------------------------

function hyperlink_params($target, $label, $params, $center=true)
{
	if ($center)
		echo "<br><center>";
	echo "<a href='$target?$params'>$label</a>\n";
	if ($center)
		echo "</center>";
}

function hyperlink_params_td($target, $label, $params)
{
	echo "<td>";
	hyperlink_params($target, $label, $params);
	echo "</td>\n";
}

//-----------------------------------------------------------------------------------

function hyperlink_params_separate($target, $label, $params, $center=false)
{
	if ($center)
		echo "<br><center>";
	echo "<a target='_blank' href='$target?$params" . SID . "'>$label</a>\n";
	if ($center)
		echo "</center>";
}

function hyperlink_params_separate_td($target, $label, $params)
{
	echo "<td>";
	hyperlink_params_separate($target, $label, $params);
	echo "</td>\n";
}

//--------------------------------------------------------------------------------------------------

function alt_table_row_color(&$k)
{
	if ($k == 1)
	{
		echo "<tr class='oddrow'>\n";
		$k = 0;
	} 
	else 
	{
		echo "<tr class='evenrow'>\n";
		$k++;
	}
}

function table_section_title($msg) 
{
	echo "<tr><td colspan=2 class='tableheader'>$msg</td><tr>\n";
}

function table_header($labels) 
{
	start_row();
	foreach ($labels as $label)
		labelheader_cell($label);
	end_row();	
}

function edit_link_cell($param)	
{
	label_cell("<a href='" . $_SERVER['PHP_SELF']. "?" . "$param'>" . _("Edit") . "</a>", "nowrap");
}

function delete_link_cell($param)
{
	label_cell("<a href='" . $_SERVER['PHP_SELF']. "?" . "$param'>" . _("Delete") . "</a>", "nowrap");
}

//-----------------------------------------------------------------------------------

function start_row($param="") 
{
	if ($param != "")
		echo "<tr $param>\n";
	else	
		echo "<tr>\n";
}

function end_row() 
{
	echo "</tr>\n";
}	

function br($num=1)
{
	for ($i = 0; $i < $num; $i++)
		echo "<br>";
}		
?>