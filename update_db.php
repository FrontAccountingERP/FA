<?php
ini_set("display_errors", "on");
$path_to_root = ".";
include_once($path_to_root. "/admin/db/maintenance_db.inc");
include_once($path_to_root. "/includes/db/connect_db.inc");

$js = get_js_png_fix();
$js .= get_js_set_focus("user");
$image = $path_to_root."/themes/default/images/logo_frontaccounting.png";
$title = "Update All Company Databases";

$comp_subdirs = array('images', 'pdf_files', 'backup','js_cache');

function get_js_png_fix()
{
	$js = "<script type=\"text/javascript\">\n"
		. "function fixPNG(myImage)\n"
		. "{\n"
		. " var arVersion = navigator.appVersion.split(\"MSIE\")\n"
		. " var version = parseFloat(arVersion[1])\n"
    	. " if ((version >= 5.5) && (version < 7) && (document.body.filters))\n"
    	. " {\n"
       	. "  var imgID = (myImage.id) ? \"id='\" + myImage.id + \"' \" : \"\"\n"
	   	. "  var imgClass = (myImage.className) ? \"class='\" + myImage.className + \"' \" : \"\"\n"
	   	. "  var imgTitle = (myImage.title) ?\n"
		. "    \"title='\" + myImage.title  + \"' \" : \"title='\" + myImage.alt + \"' \"\n"
	   	. "  var imgStyle = \"display:inline-block;\" + myImage.style.cssText\n"
	   	. "  var strNewHTML = \"<span \" + imgID + imgClass + imgTitle\n"
        . "    + \" style=\\\"\" + \"width:\" + myImage.width\n"
        . "    + \"px; height:\" + myImage.height\n"
        . "    + \"px;\" + imgStyle + \";\"\n"
        . "    + \"filter:progid:DXImageTransform.Microsoft.AlphaImageLoader\"\n"
        . "    + \"(src=\'\" + myImage.src + \"\', sizingMethod='scale');\\\"></span>\"\n"
	   	. "  myImage.outerHTML = strNewHTML\n"
    	. " }\n"
		. "}\n"
		. "</script>\n";
	return $js;
}

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
	$perms_ok = is_writable($path_to_root.'/company') && is_writable($path_to_root.'/company/0');
	$checkdirs = $comp_subdirs;
	foreach ($checkdirs as $dir) {
		$perms_ok &= is_writable($path_to_root.'/company/0/'.$dir);
	}

	if (!$perms_ok) {
		display_error("'System 'company' directory or any of its subdirectories 
			is not writable.<br> Change webserver access permissions to those 
			directories.");
	} elseif (!isset($_FILES['uploadfile']['tmp_name']) || !is_uploaded_file($_FILES['uploadfile']['tmp_name']))
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
				$cdir = "$path_to_root/company/$id";
				if (!file_exists($cdir))
				{
					create_comp_dirs($cdir, $comp_subdirs);
				}
			}
		}
	}
}
if (!isset($_POST['passwd']))
	$_POST['passwd'] = "";
if (!isset($_POST['user']))
	$_POST['user'] = "";
	
echo "<form enctype='multipart/form-data' method='post' action='".$_SERVER['PHP_SELF']."'>\n";

echo "<table align='center' width='50%' cellpadding=3 border=1 bordercolor='#cccccc' style='border-collapse: collapse'>\n";

echo "<tr><td>Database User</td><td><input type='text' name='user' value='".$_POST['user']."' size='20' /></td></tr>";
echo "<tr><td>Password</td><td><input name='passwd' type='password' value='".$_POST['passwd']."' /></td></tr>";
echo "<tr><td>Upload Script</td><td><input name='uploadfile' type='file' /></td></tr>";
echo "<tr><td>&nbsp;</td><td><input type='submit' class='inputsubmit' name='submit' value='Update' /></td></tr>";

echo "</table>\n";;

echo "<br><br>";
echo "<center><span>Choose from Database update scripts in SQL folder. No Datase is updated without a script.</span></center>\n";
echo "<br>";

echo "</form>\n";

echo "</body></html>\n";
?>