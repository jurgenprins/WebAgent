<?
	include "user_text.php";

	$res = mysql_query ("select users.email from users where users.id = $uid", $conn);
	if (!mysql_num_rows ($res)) {
		echo "<blockquote>$txt_not_found</blockquote>\n";
		page_end ();
	}

	$email = stripslashes (mysql_result ($res, 0, "email"));

	window_start ("<a href=index.php?a=u><font color=#ffffff>$n</a> : $txt_personal_settings", $topbutton, $text_color, $right_headercolor, $right_bgcolor);

	echo "<table border=0 width=100% cellspacing=0 cellpadding=0>\n";

	echo "<form method=post action=index.php><tr><td>\n";
	echo "<input type=hidden name=a value=\"u2\">\n";
	echo "<input type=hidden name=t value=\"$d\">\n";
	echo "<tr><td colspan=2 bgcolor=$right_subheadercolor><font face=$font_face size=-3>&nbsp;$txt_passwd</td></tr>\n";
	vpad ($padheight, $right_subbgcolor, 2);
	echo "<tr><td bgcolor=$right_subbgcolor width=$padwidth><img src=img/trans.gif border=0 width=$padwidth></td><td bgcolor=$right_subbgcolor><font face=$font_face size=-1>\n";
	echo "$txt_passwd_intro<br><br>\n";
	echo "<input name=npw type=password size=8 maxlength=8> &nbsp;\n";
	echo "<input name=npw2 type=password size=8 maxlength=8> &nbsp;\n";
	echo "<input type=submit value=\"    OK    \"><br><br>";
	if ((isset ($npw) || isset ($npw2)) && $uerr) echo "<font color=red><b>$uerr</b></font><br>\n";
	echo "</td></tr></form>\n";
	echo "<form method=post action=index.php><tr><td>\n";
	echo "<input type=hidden name=a value=\"u2\">\n";
	echo "<input type=hidden name=t value=\"$d\">\n";

	vpad ($padheight, $right_bgcolor, 2);

	echo "<tr><td colspan=2 bgcolor=$right_subheadercolor><font face=$font_face size=-3>&nbsp;EMAIL</td></tr>\n";
	vpad ($padheight, $right_subbgcolor, 2);
	echo "<tr><td bgcolor=$right_subbgcolor width=$padwidth><img src=img/trans.gif border=0 width=$padwidth></td><td bgcolor=$right_subbgcolor><font face=$font_face size=-1>\n";
	echo "$txt_email_intro<br><br>\n";
	echo "<input type=text name=ne value=\"$email\" size=32 maxlength=64> &nbsp;\n";
	echo "<input type=submit value=\"    OK    \"><br><br>";
	if (isset ($ne) && $uerr) echo "<font color=red><b>$uerr</b></font><br>\n";
	echo "</td></tr></form>\n";

	vpad ($padheight, $right_bgcolor, 2);

	echo "<form method=post action=index.php><tr><td>\n";
	echo "<input type=hidden name=a value=\"u2\">\n";
	echo "<input type=hidden name=t value=\"$d\">\n";

	echo "<tr><td colspan=2 bgcolor=$right_subheadercolor><font face=$font_face size=-3>&nbsp;$txt_quit</td></tr>\n";
	vpad ($padheight, $right_subbgcolor, 2);
	echo "<tr><td bgcolor=$right_subbgcolor width=$padwidth><img src=img/trans.gif border=0 width=$padwidth></td><td bgcolor=$right_subbgcolor><font face=$font_face size=-1>\n";
	echo "$txt_quit_intro<br><br>\n";
	echo "<input type=checkbox name=qt value=1> $txt_quit_1 \n";
	echo "<input type=checkbox name=qt2 value=1> $txt_quit_2\n";
	echo "<input type=submit value=\"    OK    \"><br><br>";
	if ((isset ($qt) || isset ($qt2)) && $uerr) echo "<font color=red><b>$uerr</b></font></br>\n";
	echo "</td></tr></form></table>\n";

	window_end ($right_bgcolor);
?>
