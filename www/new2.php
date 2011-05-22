<?
	$font_face = "Arial,Geneva";
	$left_headercolor = "#003366";
	$left_bgcolor = "#666699";
	$left_subbgcolor = "#aeb3cc";
	$left_sub2bgcolor = "#cdcdcd";
	$text_color = "#ffffff";
	$text2_color = "#000000";

	echo "<head>\n<title>WebAgent</title>\n";
	echo "<body background=img/bg.gif bgcolor=#999999 link=$text2_color alink=$text2_color vlink=$text2_color>\n";

	if (!$msg_big)
		$msg_big = "Aanmelding geslaagd";
	if (!$msg)
		$msg = "Vanaf nu wordt je op de hoogte gehouden van alle veranderingen op $url<p>Je emailadres is <b>$e</b><p>Je wachtwoord is <b>$pw</b><p>Het archief en je persoonlijke instellingen (o.a. wachtwoord wijzigen) kun je vinden op<p><b><a href=\"http://www.webagent.nl/?d=$did\">http://www.webagent.nl/?d=$did</a></b>";
?>
<table border=0 width=100% cellpadding=0 cellspacing=0>
<tr>
<td colspan=2 width=100% height=6 bgcolor=#336666><img src=img/trans.gif height=6></td>
</tr>
<tr>
<td valign=top width=276 bgcolor=#336666 align=right>
<?
	echo "<a href=\"http://www.webagent.nl/\"><img src=img/logo.gif width=276 height=55 border=0 align=left><br clear=all><font face=$font_face size=-4 color=$text_color></a>&nbsp;&nbsp;$txt_intro_short\n";
?>
</td><td valign=top width=500 bgcolor=#336666 align=center nowrap>
<img src=img/trans.gif width=12 border=0><a href="http://www.webagent.nl/"><img src=img/banner.gif width=476 height=60 border=0 valign=middle></a><img src=img/trans.gif width=12 border=0>
</td></tr>
<tr>
<td colspan=2 width=100% height=3 bgcolor=#336666><img src=img/trans.gif height=3></td>
</tr>
</table>
<div align=center><br>
<table border=0 cellpadding=7 cellspacing=0>
<?
	echo "<tr><td bgcolor=$left_headercolor><font face=$font_face color=$text_color size=3><b>$msg_big</td></tr>\n";
	echo "<tr><td bgcolor=$left_subbgcolor><font face=$font_face color=$text2_color size=2><br>$msg<br><br></td></tr>\n";
	if ($msg == $txt_err_email_exists) {
		echo "<tr><td bgcolor=$left_subbgcolor><font face=$font_face color=$text2_color size=2>Heb jij dit emailadres geregistreerd bij WebAgent? Vul dan hier je wachtwoord in:\n<blockquote>";
		echo "<form method=post action=new.php>\n";
		echo "<input type=hidden name=url value=\"$url\">\n";
		echo "<input type=hidden name=referer value=\"$referer\">\n";
		echo "<input type=hidden name=e value=\"$e\">\n";
		echo "<input type=password name=pw size=8 maxlength=8>\n";
		echo "<input type=submit value=\" OK \"></form>\n";
		echo "</blockquote></td></tr>\n";
	}
	echo "</table><p>\n";
	echo "<font face=$font_face><b><a href=\"$referer\">terug</a></b>\n";
?>
</body>
