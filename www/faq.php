<?
	include "faq_text.php";

	if (($f > 0) && ($f < 7))
		window_start ($txt_faq, $topbutton, $text_color, $right_headercolor, $right_bgcolor);
	else {
		$f = 0;
		window_start ($txt_faq, "", $text_color, $right_headercolor, $right_bgcolor);
	}

	echo "<table border=0 cellpadding=0 cellspacing=0 width=100%>\n";

	$header_name = "txt_header$f";
	$header_text = strtoupper ($$header_name);

	echo "<tr><td bgcolor=$right_subheadercolor colspan=2 height=10><font face=$font_face size=-3>&nbsp;&nbsp;$header_text</font></td></tr>\n";

	if ($f == 0) {
		echo "<tr><td colspan=2 bgcolor=$right_subbgcolor><table border=0>\n";
		echo "<tr><td>&nbsp;&nbsp;<font face=$font_face size=-1><a href=index.php?a=f&f=1>$txt_header1</a></td></tr>\n";
		echo "<tr><td>&nbsp;&nbsp;<font face=$font_face size=-1><a href=index.php?a=f&f=2>$txt_header2</a></td></tr>\n";
		echo "<tr><td>&nbsp;&nbsp;<font face=$font_face size=-1><a href=index.php?a=f&f=3>$txt_header3</a></td></tr>\n";
		echo "<tr><td>&nbsp;&nbsp;<font face=$font_face size=-1><a href=index.php?a=f&f=4>$txt_header4</a></td></tr>\n";
		echo "<tr><td>&nbsp;&nbsp;<font face=$font_face size=-1><a href=index.php?a=f&f=5>$txt_header5</a></td></tr>\n";
		echo "<tr><td>&nbsp;&nbsp;<font face=$font_face size=-1><a href=index.php?a=f&f=6>$txt_header6</a></td></tr>\n";
		echo "</table></td></tr>\n";
		echo "<tr><td colspan=2 bgcolor=$right_subbgcolor height=$padheight><img src=img/trans.gif height=$padheight border=0></td></tr>\n";
	} else {
		$content_name = "txt_content$f";
		$content_text = $$content_name;

		echo "<tr><td colspan=2 bgcolor=$right_subbgcolor height=$padheight><img src=img/trans.gif height=$padheight border=0></td></tr>\n";
		echo "<tr><td width=10 bgcolor=$right_subbgcolor>&nbsp;</td><td bgcolor=$right_subbgcolor valign=top><font face=$font_face size=-1>\n";
		echo $content_text;
		echo "</td></tr>\n";
		echo "<tr><td colspan=2 bgcolor=$right_subbgcolor height=$padheight><img src=img/trans.gif height=$padheight border=0></td></tr>\n";
	}

	echo "</table>\n";

	window_end ($right_bgcolor);
?>
