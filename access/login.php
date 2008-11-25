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
	if (!isset($path_to_root) || isset($_GET['path_to_root']) || isset($_POST['path_to_root']))
		die("Restricted access");
	include_once($path_to_root . "/includes/ui/ui_view.inc");
	// Display demo user name and password within login form if "$allow_demo_mode" is true
	$demo_text = "";
	if ($allow_demo_mode == True)
	{
	    $demo_text = "Login as user: demouser and password: cooldemo";
	}
	else
	{
		$demo_text = "Please login here";
	}
	if (!isset($def_coy))
		$def_coy = 0;
	$def_theme = $path_to_root . '/themes/default';
?>
<html>
<head>
<?php echo get_js_png_fix(); ?>
<script type="text/javascript">
function defaultCompany()
{
	document.forms[0].company_login_name.options[<?php echo $def_coy; ?>].selected = true;
	document.getElementById('ui_mode').value = 1;
}
</script>
    <title><?php echo $app_title . " " . $version;?></title>
    <meta http-equiv="Content-type" content="text/html; charset=iso-8859-1" />
    <link rel="stylesheet" href="<?php echo $def_theme;?>/login.css" type="text/css" />
</head>

<body leftmargin="0" topmargin="0" marginwidth="0" marginheight="0" onload="defaultCompany()">
    <table width="100%" height="100%" border="0" cellpadding="0" cellspacing="0">
        <tr>
            <td align="center" valign="bottom"><a target="_blank" href="<?php $power_url; ?>"><img src="<?php echo $def_theme;?>/images/logo_frontaccounting.png" alt="FrontAccounting" width="250" height="50" onload="fixPNG(this)" border="0" /></a></td>
		</tr>

        <tr>
            <td align="center" valign="top">

		    <table border="0" cellpadding="0" cellspacing="0">
			<tr><td colspan=2 align="center"><font size=4><b><?php echo _("Version") . " " . $version . "   Build " . $build_version ?></b></font><br><br></td></tr>
		        <tr>
		            <td colspan="2" rowspan="2">
                    <table width="346" border="0" cellpadding="0" cellspacing="0">
					<form action="<?php echo $_SERVER['PHP_SELF'];?>" name="loginform" method="post">
						<input type="hidden" id=ui_mode name="ui_mode" value="0">
                        <tr>
                            <td colspan="5" bgcolor="#FFFFFF"><img src="<?php echo $def_theme; ?>/images/spacer.png" width="346" height="1" alt="" /></td>

						</tr>

                        <tr>



                            <td bgcolor="#367CB5"><img src="<?php echo $def_theme; ?>/images/spacer.png" width="12" height="200" alt="" /></td>

                            <!--<td background="<?php echo $def_theme; ?>/images/outline/bg.png" width="233" height="200" colspan="3" valign="top">-->
                            <td class="login" colspan="3" valign="top">
                                <table border="0" cellpadding="3" cellspacing="0" width="100%">
                                    <tr>
                                        <td align="right">
                                        <!--<span class="loginText">Client login<input name="external_login" type="checkbox" value="1" class="loginText"></span>-->
                                        <br /></td>
                                    </tr>

                                    <tr>
                                        <td width="90"></td><td class="loginText" width="283"><span>User name:</span><br />
                                         <input type="text" name="user_name_entry_field"/><br />
                                         <span>Password:</span><br />
                                         <input type="password" name="password">
                                         <br />
											<span>Company:</span><br />
											<!--<select name="company_login_name" onchange="setCookie()">-->
											<select name="company_login_name">
<?php
for ($i = 0; $i < count($db_connections); $i++)
{
	echo "<option value=$i>" . $db_connections[$i]["name"] . "</option>";
}
?>
											</select>
										<br /><br />
                                         <?php echo $demo_text;?>
                                        </td>
                                    </tr>

                                    <tr>
                                        <td></td><td align="left"><input type="submit" value=" Login -->" name="SubmitUser" /></td>
                                    </tr>
                                </table>
	                        </td>
                        </tr>
                        <tr>
                            <td colspan="5" bgcolor="#FFFFFF"><img src="<?php echo $def_theme; ?>/images/spacer.png" width="346" height="1" alt="" /></td>
                        </tr>
						</form>
                    </table>
		            </td>
		            <!--<td background="<?php echo $def_theme; ?>/images/outline/r.png" colspan="3" align="right" valign="top"><img src="<?php echo $def_theme; ?>/images/outline/tr.png" width="10" height="10" alt="" /></td>-->
		        </tr>
		        <tr>
		            <!--<td background="<?php echo $def_theme; ?>/images/outline/r.png"><img src="<?php echo $def_theme; ?>/images/outline/r.png" width="10" height="10" alt=""></td>-->
		        </tr>
		        <tr>
					<!--<td background="<?php echo $def_theme; ?>/images/outline/bm.png"><img src="<?php echo $def_theme; ?>/images/outline/bl.png" width="10" height="10" alt=""></td>-->
		            <!--<td background="<?php echo $def_theme; ?>/images/outline/bm.png"><img src="<?php echo $def_theme; ?>/images/outline/bm.png" width="10" height="10" alt=""></td>-->
		            <!--<td><img src="<?php echo $def_theme; ?>/images/outline/br.png" width="10" height="10" alt="" /></td>-->
		        </tr>
<tr><td>&nbsp;</td></tr><tr>
		<td align="center" class="footer"><font size=1><a target='_blank' style="text-decoration: none" HREF='<?php echo $power_url; ?>'><font color="#FFFF00" valign="top">&nbsp;&nbsp;<?php echo $power_by; ?></font></a></font></td>
	</tr>
<!--<tr><td>&nbsp;</td></tr><tr>
	<td align="center" class="footer"><a target="_blank" HREF="http://frontaccounting.com/"><img src="<?php echo $def_theme; ?>/images/logo_frontaccounting.png"  height="60" width="60" border="0"/></a></td>
</tr>-->
<?php
if ($allow_demo_mode == true)
{
    ?>
      <tr>
        <!--<td><br><div align="center"><a href="http://frontaccounting.com"><img src="<?php echo $def_theme; ?>/images/logo_frontaccounting.png"  border="0" align="middle" /></a></div></td>-->
      </tr>
    <?php
}
?>
		    </table>

            </td>
        </tr>
    </table>
    <script language="JavaScript" type="text/javascript">
    //<![CDATA[
            <!--
            document.forms[0].user_name_entry_field.select();
            document.forms[0].user_name_entry_field.focus();
            //-->
    //]]>
    </script>
</body>
</html>
