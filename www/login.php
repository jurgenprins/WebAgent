<?
	include "login_text.php";

	window_start ($txt_my_subscriptions, "", $text_color, $left_headercolor, $left_bgcolor);

	echo "<table cellpadding=0 cellspacing=0 border=0 width=100%>\n";
	echo "<tr><td bgcolor=$left_subbgcolor width=5><img src=img/trans.gif width=5 border=0></td>\n";
	echo "<td bgcolor=$left_subbgcolor>\n";

	if ($j) {
		if (!$e) {
			echo "<font face=$font_face size=-1>\n";
			echo "<br><b>$txt_err_no_email</b></td></tr>\n";
		} else {
			if ($j == 2) {
				if ($sent) {
					echo "<font face=$font_face size=-1>\n";
					echo "<br><b>$txt_mail_passwd_ok $e</b>\n";
				} else {
					echo "<font face=$font_face size=-1>\n";
					echo "<br><b>$txt_err_wrong_email</b>\n";
				}
			} else {
				if (!$pw) {
					echo "<font face=$font_face size=-1>\n";
					echo "<br><b>$txt_err_no_passwd</b>\n";
				} else if (strlen ($err)) {
					echo "<font face=$font_face size=-1>\n";
					echo "<br><b>$err</b>\n";
					$e = "";
				}
			}
		}
	} else if ($e || $pw) {
		echo "<font face=$font_face size=-1>\n";
		echo "<br><b>$txt_err_no_auth</b>\n";
	} 
	
	echo "<font face=$font_face color=$text2_color size=-2>\n";
	echo "<form method=post action=index.php>\n";
	echo "<input type=hidden name=d value=$d>\n";
	echo "<input type=hidden name=p value=$p>\n";
	echo "<input type=hidden name=s value=$s>\n";
	echo "Email<br><input type=text name=e size=12 maxlength=80 value=\"$e\"><br>\n";
	echo "$txt_passwd<br><input type=password name=pw size=8 maxlength=8><br><br>\n";
	echo "<font size=-2>$txt_member_intro<br><br>";
	echo "<font size=-1><b>$txt_no_membership_ask</b></font><br>$txt_no_membership_sol<br>\n";
	echo "<input type=radio name=j value=1> $txt_create_membership<br><br>\n";
	echo "<font size=-1><b>$txt_forgot_passwd_ask</b></font><br>$txt_forgot_passwd_sol<br>\n";
	echo "<input type=radio name=j value=2> $txt_send_passwd<br><br>\n";
	echo "$txt_already_member<br>\n";
	echo "<input type=radio name=j value=0 checked> $txt_show_memberpage<br><br>\n";
	echo "<div align=center><input type=submit value=\"       OK       \"></div></form>\n";

	echo "</td></tr></table>\n";

	window_end ($left_bgcolor);
?>
