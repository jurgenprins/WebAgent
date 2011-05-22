<?	
	include "menu_text.php";

	$img_width = 24;
	$img_height = 19;

	echo "<table border=0 cellpadding=0 cellspacing=0><tr><td>\n";

	$n = strtok ($e, "@");

	window_start ("$txt_subscriptions_for <a href=index.php?a=u><font color=$text_color>$n</font></a>", "<a href=\"index.php?o=1&d=$d&c=$c&s=$s\" onMouseOver=\"document.close_left.src = 'img/close_blue_rollover.gif';\" onMouseOut=\"document.close_left.src = 'img/close_blue_normal.gif';\" onMouseDown=\"document.close_left.src = 'img/close_blue_click.gif';\"><img src=img/close_blue_normal.gif name=close_left border=0 width=12 height=12 alt=\"$txt_alt_logout\"></a>", $text_color, $left_headercolor, $left_bgcolor);

	$res = mysql_query ("select distinct docs.id, docs.changed, docs.title, docs.change_freq, docs_cats.cid, watch.email_prio, watch.letter_update, watch.title as wtitle from docs, watch, docs_cats where watch.uid = $uid and watch.did = docs.id and docs_cats.did = docs.id order by docs.changed desc");
	$num = mysql_num_rows ($res);

	echo "<table border=0 width=100% cellpadding=0 cellspacing=0>\n";
	echo "<tr><td colspan=3 bgcolor=$left_subbgcolor><font face=$font_face size=-3>&nbsp;&nbsp;$txt_documents</td></tr>\n";

	for ($i = 0; $i < $num; $i++) {
		$doc_id = 0 + mysql_result ($res, $i, "docs.id");
		$cat_id = 0 + mysql_result ($res, $i, "docs_cats.cid");
		$changed = 0 + mysql_result ($res, $i, "docs.changed");
		$title = stripslashes (mysql_result ($res, $i, "docs.title"));
		if (strlen ($title) > $max_title_len)
			$title = substr ($title, 0, $max_title_len) . $title_ext;
		$freq = 0 + mysql_result ($res, $i, "docs.change_freq");
		$mail = 0 + mysql_result ($res, $i, "watch.email_prio");
		$letter = 0 + mysql_result ($res, $i, "watch.letter_update");
		$wtitle = stripslashes (mysql_result ($res, $i, "wtitle"));
		if (strlen ($wtitle)) $title = $wtitle;
		if (!isset ($d)) $d = $doc_id;
		if (!isset ($c)) $c = $cat_id;

		$monitor[$doc_id] = 1;
		$mailgif = "mailinst_nooit";
		$mailtxt = "";
  		switch ($letter) {
		case 24:
			$mailgif = "mailinst_dag";
			$mailtxt = $txt_in_daily_letter;
			break;
		case 48:
			$mailgif = "mailinst_2dag";
			$mailtxt = $txt_in_bidaily_letter;
			break;
		default:
		}
		if ($mail) {
			$mailgif = "mailinst_direkt";
			$mailtxt = $txt_sent_immediately;
		}

		if ($i % 2) {
			echo "<tr><td colspan=3 bgcolor=$left_subbgcolor height=4><img src=img/trans.gif height=4 border=0></td></tr>\n";
			echo "<tr><td width=$padwidth bgcolor=$left_subbgcolor><img src=img/trans.gif width=$padwidth border=0></td><td bgcolor=$left_subbgcolor nowrap><font face=$font_face size=-1>\n";
		} else {
			echo "<tr><td colspan=3 bgcolor=$left_sub2bgcolor height=4><img src=img/trans.gif height=4 border=0></td></tr>\n";
			echo "<tr><td width=$padwidth bgcolor=$left_sub2bgcolor><img src=img/trans.gif width=4 border=0></td><td bgcolor=$left_sub2bgcolor nowrap><font face=$font_face size=-1>\n";
		}
		if ($d == $doc_id) echo "<b>";
		echo "<a href=\"index.php?d=$doc_id&c=$cat_id\"><font color=$text2_color>$title</font></a><br>&nbsp;&nbsp;<font size=-2 color=$text3_color>";
		if ($changed) {
			echo date ("M j H:i", $changed);
			echo ", &nbsp;&nbsp;&nbsp;$freq $txt_chgswk, &nbsp; $mailtxt";
		} else
			echo "-";

		if ($d == $doc_id) echo "</b>";
		$title = addslashes ($title);
		if ($i % 2)
			echo "</td><td bgcolor=$left_subbgcolor width=1% nowrap align=right valign=top><font face=$font_face size=-3>\n";
		else
			echo "</td><td bgcolor=$left_sub2bgcolor width=1% nowrap align=right valign=top><font face=$font_face size=-3>\n";
		echo "<a href=\"index.php?d=$doc_id&c=$cat_id&a=s\" onMouseOver=\"document.mailgif" . $i . ".src = 'img/$mailgif" . "_r.gif';\" onMouseOut=\"document.mailgif" . $i . ".src = 'img/$mailgif" . "_n.gif';\" onMouseDown=\"document.mailgif" . $i . ".src = 'img/$mailgif" . "_c.gif';\"><img name=mailgif" . $i . " src=img/$mailgif" . "_n.gif width=$img_width height=$img_height border=0 alt=\"$txt_alt_email_settings\"></a> ";
		echo " <a href=\"index.php?d=$d&c=$c=$s&t=$doc_id&a=d\" onClick=\"return confirm ('$txt_confirm_delete_pre $title $txt_confirm_delete_post');\" onMouseOver=\"document.deletegif" . $i . ".src = 'img/delete_r.gif';\" onMouseOut=\"document.deletegif" . $i . ".src = 'img/delete_n.gif';\"><img name=deletegif" . $i . " src=img/delete_n.gif width=20 height=19 border=0 alt=\"$txt_alt_delete\"></a>\n";
		echo "</td></tr>\n";

		if ($i % 2)
			echo "<tr><td height=4 bgcolor=$left_subbgcolor colspan=3><img src=img/trans.gif height=4 border=0></td></tr>\n";
		else
			echo "<tr><td height=4 bgcolor=$left_sub2bgcolor colspan=3><img src=img/trans.gif height=4 border=0></td></tr>\n";
	}

	if (!$doc_id) 
		echo "<tr><td bgcolor=$left_subbgcolor colspan=3>&nbsp;&nbsp;<font face=$font_face size=1 color=$text2_color><b>$txt_no_documents<br><br></td></tr>\n";

	echo "</table>\n";

	window_end ($left_bgcolor);

	echo "<img src=img/trans.gif height=$padheight border=0>";

	window_start ($txt_new_subscription, "", $text_color, $left_headercolor, $left_bgcolor);

	echo "<table border=0 width=100% cellpadding=0 cellspacing=0>\n";

	if ($aerr) echo "<font face=$font_face size=-1 color=#ffcccc><b>$aerr</b></font><br>\n";

	echo "<form method=post action=index.php>\n";
	echo "<input type=hidden name=d value=\"$d\">\n";
	echo "<input type=hidden name=c value=\"$c\">\n";
	echo "<input type=hidden name=s value=\"$s\">\n";
	echo "<input type=hidden name=a value=a>\n";
	echo "<input type=hidden name=t value=-1>\n";
	echo "<tr><td colspan=2 bgcolor=$left_subbgcolor><font face=$font_face size=-3>&nbsp;&nbsp;URL</td></tr>\n";
	echo "<tr><td colspan=2 bgcolor=$left_sub2bgcolor height=4><img src=img/trans.gif height=4 border=0></td></tr>\n";
	echo "<tr><td width=$padwidth bgcolor=$left_sub2bgcolor><img src=img/trans.gif width=$padwidth border=0></td><td bgcolor=$left_sub2bgcolor nowrap>\n";
	echo "<input name=url size=32 maxlength=255 value=\"http://\"></b><br>\n";
	echo "<font face=$font_face size=-1 color=$text2_color>$txt_category <select name=cat_id></font>\n";
	if ($lang)
		$res = mysql_query ("select id, name_$lang from cats order by name_$lang", $conn);
	else
		$res = mysql_query ("select id, name from cats order by name", $conn);
	for ($i =0; $i < mysql_num_rows ($res); $i++) {
		$id = 0 + mysql_result ($res, $i, "id");
		if ($lang)
			$name = stripslashes (mysql_result ($res, $i, "name_$lang"));
		else
			$name = stripslashes (mysql_result ($res, $i, "name"));
		echo "<option value=\"$id\" ";
		if ($id == $c) echo "selected";
		echo ">$name\n";
	}
	echo "</select>\n";
	echo "<input type=submit value=\"    OK    \"></td></tr>\n";
	echo "<tr><td colspan=2 bgcolor=$left_sub2bgcolor height=4><img src=img/trans.gif height=4 border=0></td></tr></table></form>\n";

	window_end ($left_bgcolor);

	echo "</td></tr></table>\n";
?>
