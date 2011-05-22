<?
	include "settings_text.php";

	$res = mysql_query ("select docs.title, docs.host, docs.uri, docs.change_freq, watch.email_prio, watch.letter_update, watch.keywords, watch.filter, watch.title as wtitle, watch.tmpl from docs, watch where docs.id = $d and watch.uid = $uid and watch.did = $d", $conn);
	if (!mysql_num_rows ($res)) {
		echo "<blockquote>$txt_not_found</blockquote>\n";
		page_end ();
	}

	$title = stripslashes (mysql_result ($res, 0, "title"));
	if (strlen ($title) > $max_title_len)
		$title = substr ($title, 0, $max_title_len) . $title_ext;
	$host = mysql_result ($res, 0, "host");
	$uri = mysql_result ($res, 0, "uri");
	$freq = 0 + mysql_result ($res, 0, "change_freq");
	$mail = 0 + stripslashes (mysql_result ($res, 0, "email_prio"));
	$letter = 0 + stripslashes (mysql_result ($res, 0, "letter_update"));
	$keywords = stripslashes (mysql_result ($res, 0, "keywords"));
	$filter = 0 + mysql_result ($res, 0, "filter");
	$wtitle = stripslashes (mysql_result ($res, 0, "wtitle"));
	$tmpl = 0 + mysql_result ($res, 0, "tmpl");
	$period = 0;
	if ($mail)
		$period = 1;
	else
		$period = $letter;

	if ($wtitle)
		window_start ("<a href=index.php?a=u><font color=#ffffff>$n</font></a> : <a href=index.php?d=$d&c=$c><font color=#ffffff>$wtitle</a></a> : $txt_email_settings", $topbutton, $text_color, $right_headercolor, $right_bgcolor);
	else
		window_start ("<a href=index.php?a=u><font color=#ffffff>$n</font></a> : <a href=index.php?d=$d&c=$c><font color=#ffffff>$title</a></a> : $txt_email_settings", $topbutton, $text_color, $right_headercolor, $right_bgcolor);

	echo "<table border=0 width=100% cellpadding=0 cellspacing=0>\n";

	echo "<form method=post action=index.php><tr><td>\n";
	echo "<input type=hidden name=a value=\"s2\">\n";
	echo "<input type=hidden name=t value=\"$d\">\n";
	echo "<input type=hidden name=d value=\"$d\">\n";
	echo "<input type=hidden name=c value=\"$c\">\n";
	echo "<tr><td colspan=2 bgcolor=$right_subheadercolor><font face=$font_face size=-3>&nbsp;$txt_title</td></tr>\n";
	vpad ($padheight, $right_subbgcolor, 2);              
	echo "<tr><td bgcolor=$right_subbgcolor width=$padwidth><img src=img/trans.gif border=0 width=$padwidth></td><td bgcolor=$right_subbgcolor><font face=$font_face size=-1>\n";
	echo $txt_title_intro . $title . $txt_title_intro_post . "<br><br>\n"; 
	echo "<font face=$font_face size=-1>\n";
	echo "<input type=text name=title value=\"$wtitle\" size=40 maxlength=64><br><br></td></tr>\n";
	echo "<tr><td height=$padheight bgcolor=$right_bgcolor><img src=img/trans.gif height=$padheight border=0></td></tr>\n";
	echo "<tr><td colspan=2 bgcolor=$right_subheadercolor><font face=$font_face size=-3>&nbsp;$txt_frequency</td></tr>\n";
	vpad ($padheight, $right_subbgcolor, 2);              
	echo "<tr><td bgcolor=$right_subbgcolor width=$padwidth><img src=img/trans.gif border=0 width=$padwidth></td><td bgcolor=$right_subbgcolor><font face=$font_face size=-1>\n";
	echo "$txt_freq_pre $freq $txt_freq_post<br><br>\n";
	echo "<table border=0><tr><td bgcolor=$right_subbgcolor>\n";
	echo "<font face=$font_face size=-2>$txt_recv_updates</td><td bgcolor=$right_subbgcolor width=1% nowrap>\n";
	echo "<font face=$font_face size=-2>$txt_mails_week</td></tr>\n";
	echo "<tr><td bgcolor=$right_subbgcolor><font face=$font_face size=-1>\n";
	echo "<img src=img/mailinst_direkt_n.gif border=0> <input type=radio name=period value=1 ";
	if ($period == 1) echo "checked><b> "; else echo "> ";
	echo "$txt_immediately</td><td bgcolor=$right_subbgcolor align=center><font face=$font_face size=-1>";
	if ($period == 1) echo "<b>";
	echo "$freq</td></tr>\n";
	echo "<tr><td bgcolor=$right_subbgcolor><font face=$font_face size=-1>\n";
	echo "<img src=img/mailinst_dag_n.gif border=0> <input type=radio name=period value=24 ";
	if ($period == 24) echo "checked><b> "; else echo "> ";
	echo "$txt_daily</td><td bgcolor=$right_subbgcolor align=center><font face=$font_face size=-1>";
	if ($period == 24) echo "<b>";
	if ($freq < 7) echo $freq;  else echo "7";
	echo "</td></tr><tr><td bgcolor=$right_subbgcolor><font face=$font_face size=-1>\n";
	echo "<img src=img/mailinst_2dag_n.gif border=0> <input type=radio name=period value=48 ";
	if ($period == 48) echo "checked><b> "; else echo "> ";
	echo "$txt_bidaily</td><td bgcolor=$right_subbgcolor align=center><font face=$font_face size=-1>";
	if ($period == 48) echo "<b>";
	if ($freq < 3) echo $freq; else echo "3";
	echo "</td></tr><tr><td bgcolor=$right_subbgcolor><font face=$font_face size=-1>\n";
	echo "<img src=img/mailinst_nooit_n.gif border=0> <input type=radio name=period value=0 ";
	if (!$period) echo "checked><b> "; else echo "> ";
	echo "$txt_never</td><td bgcolor=$right_subbgcolor align=center><font face=$font_face size=-1>";
	if (!$period) echo "<b>";
	echo "0</td></tr>\n";
	echo "</td></tr></table><br></td></tr>\n";
	echo "<tr><td height=$padheight bgcolor=$right_bgcolor><img src=img/trans.gif height=$padheight border=0></td></tr>\n";
	echo "<tr><td colspan=2 bgcolor=$right_subheadercolor><font face=$font_face size=-3>&nbsp;$txt_keywords</td></tr>\n";
	vpad ($padheight, $right_subbgcolor, 2);              
	echo "<tr><td bgcolor=$right_subbgcolor width=$padwidth><img src=img/trans.gif border=0 width=$padwidth></td><td bgcolor=$right_subbgcolor><font face=$font_face size=-1>\n";
	echo "$txt_keywords_intro<br><br>\n"; 
	echo "<font face=$font_face size=-1>\n";
	echo "<input type=text name=keywords value=\"$keywords\" size=20 maxlength=32><br><br></td></tr>\n";
	echo "<tr><td height=$padheight bgcolor=$right_bgcolor><img src=img/trans.gif height=$padheight border=0></td></tr>\n";
	echo "<tr><td colspan=2 bgcolor=$right_subheadercolor><font face=$font_face size=-3>&nbsp;$txt_content</td></tr>\n";
	vpad ($padheight, $right_subbgcolor, 2);              
	echo "<tr><td bgcolor=$right_subbgcolor width=$padwidth><img src=img/trans.gif border=0 width=$padwidth></td><td bgcolor=$right_subbgcolor><font face=$font_face size=-1>\n";
	echo "$txt_content_intro<br><br>\n";
	echo "<font face=$font_face size=-1>\n";
	echo "<input type=checkbox name=filter_link value=1 ";
	if ($filter & 1) echo "checked><b> "; else echo "> ";
	echo "$txt_links</b><br>\n";
	echo "<input type=checkbox name=filter_text value=2 ";
	if ($filter & 2) echo "checked><b> "; else echo "> ";
	echo "$txt_text</b><br>\n";
	echo "<input type=checkbox name=filter_image value=4 ";
	if ($filter & 4) echo "checked><b> "; else echo "> ";
	echo "$txt_images</b><br><br>\n";
	echo "</td></tr>\n";

	echo "<tr><td height=$padheight bgcolor=$right_bgcolor><img src=img/trans.gif height=$padheight border=0></td></tr>\n";
	echo "<tr><td colspan=2 bgcolor=$right_subheadercolor><font face=$font_face size=-3>&nbsp;$txt_layout</td></tr>\n";
	vpad ($padheight, $right_subbgcolor, 2);              
	echo "<tr><td bgcolor=$right_subbgcolor width=$padwidth><img src=img/trans.gif border=0 width=$padwidth></td><td bgcolor=$right_subbgcolor><font face=$font_face size=-1>\n";
	if ($period != 1) {
		echo "<font color=$text3_color>$txt_layout_na</font>\n";
		echo "<input type=hidden name=tmpl value=0><br><br>\n";
	} else {
		if ($tmpl > 1) {
			echo "<font color=$text3_color>$txt_layout_static</font>\n";
			echo "<input type=hidden name=tmpl value=$tmpl><br><br>\n";
		} else {
			echo "$txt_layout_intro<br><br>\n";
			echo "<font face=$font_face size=-1>\n";
			echo "<input type=radio name=tmpl value=0 ";
			if (!$tmpl) echo "checked><b> "; else echo "> ";
			echo "$txt_layout_normal</b><br>\n";
			echo "<input type=radio name=tmpl value=1 ";
			if ($tmpl == 1) echo "checked><b> "; else echo "> ";
			echo "$txt_layout_short</b><br><br>\n";
		}
	}
	echo "</td></tr>\n";

	echo "<tr><td height=5></td></tr>\n";
	echo "<tr><td align=center colspan=2><input type=submit value=\"    OK    \"></td></tr></form>\n";
	echo "<form method=post action=index.php>\n";
        echo "<input type=hidden name=d value=\"$d\">\n";
        echo "<input type=hidden name=c value=\"$c\">\n";
        echo "<input type=hidden name=bf value=1>\n";

	echo "<tr><td height=25 bgcolor=$right_bgcolor><img src=img/trans.gif height=25 border=0></td></tr>\n";
	echo "<tr><td colspan=2 bgcolor=$right_bgcolor><font face=$font_face size=-3 color=$text_color>&nbsp;$txt_advanced</td></tr>\n";
	echo "<tr><td colspan=2 bgcolor=$right_subheadercolor><font face=$font_face size=-3>&nbsp;$txt_location</td></tr>\n";
	vpad ($padheight, $right_subbgcolor, 2);              
	echo "<tr><td bgcolor=$right_subbgcolor width=$padwidth><img src=img/trans.gif border=0 width=$padwidth></td><td bgcolor=$right_subbgcolor><font face=$font_face size=-1>\n";
	echo "$txt_location_intro<br><br>\n";
	echo "<font face=$font_face size=-1>\n";
        echo "<input type=submit value=\" $txt_location_button \"><br><br></td></tr></form>\n";
	echo "<tr><td height=$padheight bgcolor=$right_bgcolor><img src=img/trans.gif height=$padheight border=0></td></tr>\n";
	echo "</table>\n";
	window_end ($right_bgcolor);
?>
