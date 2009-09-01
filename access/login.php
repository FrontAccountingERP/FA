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
	if (!isset($path_to_root) || isset($_GET['path_to_root']) || isset($_POST['path_to_root']))
		die(_("Restricted access"));
	include_once($path_to_root . "/includes/ui/ui_view.inc");
	// Display demo user name and password within login form if "$allow_demo_mode" is true
	if ($allow_demo_mode == true)
	{
	    $demo_text = _("Login as user: demouser and password: password");
	}
	else
	{
		$demo_text = _("Please login here");
	}
	if (!isset($def_coy))
		$def_coy = 0;
	$def_theme = $path_to_root . '/themes/default';

$login_timeout = $_SESSION["wa_current_user"]->last_act;

	echo "<html>
		<head>";
if (!$login_timeout) { // page header
	echo '<script>'.get_js_png_fix().'</script>'; ?>
<script type="text/javascript">
function defaultCompany()
{
	document.forms[0].company_login_name.options[<?php
//	 echo $def_coy; 
	echo $_SESSION["wa_current_user"]->company;
	 ?>].selected = true;
	document.getElementById('ui_mode').value = 1;
}
</script>
    <title><?php echo $app_title . " " . $version;?></title>
    <meta http-equiv="Content-type" content="text/html; charset=<?php echo $_SESSION['language']->encoding;?>" />
    <link rel="stylesheet" href="<?php echo $def_theme;?>/login.css" type="text/css" />
</head>

 <body leftmargin="0" topmargin="0" marginwidth="0" marginheight="0" onload="defaultCompany()">
<?php
} else { // end page header
?>
    <title><?php echo _('Authorization timeout'); ?></title>
    <meta http-equiv="Content-type" content="text/html; charset=<?php echo $_SESSION['language']->encoding;?>" />
    <link rel="stylesheet" href="<?php echo $def_theme;?>/login.css" type="text/css" />
<?php
};?>
    <table width="100%" height="100%" border="0" cellpadding="0" cellspacing="0">
        <tr>
			<td align="center" valign="bottom">
  <?php
if (!$login_timeout) { // FA logo
?>          <a target="_blank" href="<?php $power_url; ?>"><img src="<?php echo $def_theme;?>/images/logo_frontaccounting.png" alt="FrontAccounting" width="250" height="50" onload="fixPNG(this)" border="0" /></a>
<?php } else { ?>
			<font size=5><?php echo _('Authorization timeout'); ?></font>
<?php }; ?>
			</td>
		</tr>

        <tr>
            <td align="center" valign="top">

		    <table border="0" cellpadding="0" cellspacing="0">
<?php
if (!$login_timeout) { // FA version info
?>			<tr><td colspan=2 align="center"><font size=4><b><?php echo _("Version") . " " . $version . "   Build " . $build_version ?></b></font><br><br></td></tr>
<?php
}; // end of FA version info
?>
		        <tr>
		            <td colspan="2" rowspan="2">
					<form action="<?php 
						echo $login_timeout ? $_SERVER['PHP_SELF'] : $_SESSION['timeout']['uri'];
					?>" name="loginform" method="post">
                    <table width="346" border="0" cellpadding="0" cellspacing="0">
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
								<td  align ='right'>
                                    <!--<span class="loginText">Client login<input name="external_login" type="checkbox" value="1" class="loginText"></span>-->
								<br /></td>
                                    </tr>

                                    <tr>
                                        <td width="90"></td><td class="loginText" width="283"><span><?php echo _("User name"); ?>:</span><br />
                                         <input type="text" name="user_name_entry_field" value="<?php echo $login_timeout ? $_SESSION['wa_current_user']->loginname : ($allow_demo_mode ? "demouser":""); ?>"/><br />
                                         <span><?php echo _("Password"); ?>:</span><br />
                                         <input type="password" name="password"  value="<?php echo $allow_demo_mode ? "password":""; ?>">
                                         <br />
<?php
	if ($login_timeout) {
		echo "<br><input type = 'hidden'  name='company_login_name' value='".
		$_SESSION["wa_current_user"]->company."'>";
	} else {
?>
			<span><?php echo _("Company"); ?>:</span><br />
			<!--<select name="company_login_name" onchange="setCookie()">-->
			<select name="company_login_name" <?php if($login_timeout) echo 'disabled';?>>
<?php
			for ($i = 0; $i < count($db_connections); $i++)
				echo "<option value=$i ".($i==$_SESSION['wa_current_user']->company ? 'selected':'') .">" . $db_connections[$i]["name"] . "</option>";
?>
			</select>
			<br /><br />
            <?php echo $demo_text;?>
<?php
}; // else in_ajax
?>                                   </td>
                                </td>
                                    </tr>

                                    <tr>
                                        <td></td><td align="left"><input type="submit" value= "<?php echo _("Login -->");?> " name="SubmitUser" /></td>
                                    </tr>
                                </table>
	                        </td>
                        </tr>
<?php
 if (!$login_timeout) 
 	echo "<tr>
 <td colspan='5' bgcolor='#FFFFFF'><img src='$def_theme/images/spacer.png' width='346' height='1' alt='' /></td>
         </tr>";

	foreach($_SESSION['timeout']['post'] as $p => $val) {
		// add all request variables to be resend together with login data
		if (!in_array($p, array('ui_mode', 'user_name_entry_field', 
			'password', 'SubmitUser', 'company_login_name'))) 
			echo "<input type='hidden' name='$p' value='$val'>";
	}
?>
                    </table>
					</form>
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
<tr><td>&nbsp;</td></tr>
<?php
if (!$login_timeout) {
?>
<tr>
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
