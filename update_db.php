<?php
//ini_set("display_errors", "on");
$path_to_root = ".";
include_once($path_to_root. "/admin/db/maintenance_db.inc");
include_once($path_to_root. "/includes/db/connect_db.inc");
include_once($path_to_root. "/includes/ui/ui_view.inc");
include_once($path_to_root. "/includes/ui/ui_input.inc");

$js = get_js_png_fix();
$js .= get_js_set_focus("user");
$image = $path_to_root."/themes/default/images/logo_frontaccounting.png";
$title = "Update All Company Databases";

function get_js_set_focus($name)
{
	$js = "\n<script type=\"text/javascript\">\n"
		. "<!--\n"
		. "function setFocus()\n"
		. "{\n"
		. "	document.forms[0].$name.focus();\n"
		. "}\n"
		. "-->\n"
		. "</script>\n";
	return $js;
}

function display_error($msg, $center=true)
{
    echo "<center><table border='1' cellpadding='3' cellspacing='0' style='border-collapse: collapse' bordercolor='#CC3300' width='50%'>
      <tr>
        <td  " . ($center?"align='center' ":"") . " width='100%' bgcolor='#ffcccc'><font color='#dd2200'>$msg</font></td>
      </tr>
    </table></center><br>\n";
}

function display_notification($msg, $center=true)
{
    echo "<center><table border='1' cellpadding='3' cellspacing='0' style='border-collapse: collapse' bordercolor='#33cc00' width='50%'>
      <tr>
        <td " . ($center?"align='center' ":"") . " width='100%' bgcolor='#ccffcc'><font color='#007700'>$msg</font></td>
      </tr>
    </table></center><br>\n";
}


function db_open($conn)
{
	$db = mysql_connect($conn["host"] ,$conn["dbuser"], $conn["dbpassword"]);
	if (!$db)
		return false;
	if (!mysql_select_db($conn["dbname"], $db))
		return false;
	return $db;
}

echo "<html dir='ltr' >\n";
echo "<head><title>$title</title>\n";
echo "<meta http-equiv='Content-type' content='text/html'; charset='iso-8859-1'>\n";
echo "<link href='$path_to_root/themes/default/default.css' rel='stylesheet' type='text/css' />\n";
echo $js;
echo "</head> \n";
echo "<body style='background-color:#f9f9f9;' onload='setFocus();'>";

echo "<br><br><br><br>";
echo "<table align='center' width='50%' cellpadding=3 border=1 bordercolor='#cccccc' style='border-collapse: collapse'>\n";
echo "<tr><td align='center' valign='bottom'><img src='$image' alt='FrontAccounting' width='250' height='50' onload='fixPNG(this)' border='0' /></td></tr>\n";
echo "</table>\n";

echo "<br><br>";
echo "<center><span class='headingtext'>$title</span></center>\n";
echo "<br>";

if (isset($_POST["submit"]))
{
	if (!isset($_FILES['uploadfile']['tmp_name']) || !is_uploaded_file($_FILES['uploadfile']['tmp_name']))
	{
		display_error("You must select an SQL script for update");;
	}
	else
	{
		include_once($path_to_root."/config_db.php");
		if (!isset($_POST['user']) || !isset($_POST['passwd']) || $_POST['user'] == "")
		{
			display_error("You must select a user name and an optional password");
		}
		else
		{
			foreach($db_connections as $id => $conn)
			{
				$conn['dbuser'] = $_POST['user'];
				$conn['dbpassword'] = $_POST['passwd'];
				if (!($db = db_open($conn)))
				{
					display_error("Wrong user name or password - ".mysql_error());
				}
				else
				{
					if (!db_import($_FILES['uploadfile']['tmp_name'], $conn))
						display_error("Bad SQL file or you have already updated the company: "
							. $id . " " . $conn['name']." - ".mysql_error());
					else
						display_notification("Database has been updated for company: "
							. $id . " " .  $conn['name']);
				}
			}
		}
	}
}
if (!isset($_POST['passwd']))
	$_POST['passwd'] = "";

echo "<form enctype='multipart/form-data' method='post' action='".$_SERVER['PHP_SELF']."'>\n";

echo "<table align='center' width='50%' cellpadding=3 border=1 bordercolor='#cccccc' style='border-collapse: collapse'>\n";

text_row_ex("Database User", "user", 20);
label_row("Password", "<input name='passwd' type='password' value='".$_POST['passwd']."' />");
label_row("Upload Script", "<input name='uploadfile' type='file'>");
submit_row("submit", "Update");

echo "</table>\n";;

echo "<br><br>";
echo "<center><span>Choose from Database update scripts in SQL folder. No Datase is updated without a script.</span></center>\n";
echo "<br>";

echo "</form>\n";

echo "</body></html>\n";
?>