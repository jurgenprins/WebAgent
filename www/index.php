<? 
	$font_face = "Arial,Geneva";
	$left_headercolor = "#003366";
	$left_bgcolor = "#666699";
	$left_subbgcolor = "#aeb3cc";
	$left_sub2bgcolor = "#cdcdcd";
    	$right_headercolor = "#993300";
	$right_subheadercolor = "#cc9999";
	$right_bgcolor = "#cc6633";
	$right_subbgcolor = "#ffcc9b";
	$text_color = "#ffffff";
	$text2_color = "#000000";
	$text3_color = "#333333";
   	$border_color = "#000000";
	$bf_bgcolor_normal = "#ffee9b";
	$bf_bgcolor_dimmed = "#ddcc9a";
	$padwidth = 7; $padheight = 10;
	$padbetween = 10;
	if (isset ($wn_lang) && $wn_lang)
		$lang = $wn_lang;
	else
		$lang = "nl";
	$max_title_len = 30;
	$title_ext = " ..";

	$banner[0] = "img/banner.gif";    $adurl[0] = "http://www.webagent.nl/";	
	$banner[1] = "img/bannerdds.gif"; $adurl[1] = "http://vacatures.dds.nl/";
	$banneridx = 0;

	srand((double)microtime()*1000000); 
	if (rand(0,4) < 1)
		$banneridx = 1;;

	include "db.php";

	$auth_ok = 0;

	function vpad ($height, $color, $colspan) {
		echo "<tr><td colspan=$colspan bgcolor=$color height=$height><img src=img/trans.gif border=0 height=$height></td></tr>\n";
	}

	function window_start ($title_left, $title_right, $header_textcolor, $left_headercolor, $content_bgcolor) {
		global $padwidth, $padheight;
		global $border_color;
	
		if (!strlen ($title_right))
			$title_right="&nbsp;";

		echo "<table border=0 cellpadding=0 cellspacing=0 width=100%>\n";

		echo "<tr><td colspan=3 bgcolor=$left_headercolor><table border=0 cellpadding=0 cellspacing=0 width=100%><tr><td bgcolor=$left_headercolor nowrap><font face=Arial,Geneva size=-1 color=$header_textcolor><b>&nbsp;$title_left</b></td>\n";
		echo "<td bgcolor=$left_headercolor valign=middle align=right nowrap>" . $title_right . "&nbsp;</td></tr></table></td></tr>\n";

		echo "<tr><td colspan=3 bgcolor=$content_bgcolor height=$padheight><img src=img/trans.gif height=$padheight></td></tr>\n";

		echo "<tr><td width=$padwidth bgcolor=$content_bgcolor><img src=img/trans.gif width=$padwidth border=0></td><td bgcolor=$content_bgcolor valign=top>\n";
	}

	function window_end ($content_bgcolor) {
		global $padheight, $padwidth;
		global $border_color;

		echo "</td><td width=$padwidth bgcolor=$content_bgcolor><img src=img/trans.gif width=$padwidth border=0></td></tr>\n";

		echo "<tr><td colspan=3 bgcolor=$content_bgcolor height=$padheight><img src=img/trans.gif height=$padheight></td></tr>\n";

		echo "</table>\n";
	}

	function page_end () {
		echo "</td></tr></table>\n";
		echo "</td></tr></table>\n";
		echo "</body>\n";
		exit;
	}

	if ($l) {
		SetCookie ("wn_lang", $l, time() +36000000, "/");
		$lang = $l;
	}

	include "index_text.php";

	if ($o) SetCookie ("wn_auth", "", "", "/");

	if (!$o && $wn_auth && (!($e && $pw))) {
		$koek = $wn_auth;
		for ($i = 0; $i < strlen ($koek); $i++)
			$koek[$i] = chr (ord ($koek[$i]) -1);
		$e = strtok ($koek, ":");
		$pw = strtok (":");
	}

	$err = "";
	if ($pw)
		$pw = substr ($pw, 0, 8);

	if (($j == 1) && $pw && $e) {
		if (!eregi ("(.*)@(.*)\.(.*)", $e))
			$err = $txt_err_email_invalid;
		else {
			$res = mysql_query ("select id from users where email = '$e'", $conn);
			if (mysql_num_rows ($res)) 
				$err = $txt_err_email_exists;
			else {
				$now = date ("U");
				$res = mysql_query ("insert into users values (0, '$e','$pw', $now)", $conn);
			}
		}
	}

	if (($j == 2) && $e) {
		$res = mysql_query ("select passwd from users where email = '$e'", $conn);
		if (mysql_num_rows ($res)) {
			mail ($e, $txt_mail_passwd_subject, $txt_mail_passwd_pre . "Email   : $e\nPassword: " . mysql_result ($res, 0, "passwd") . $txt_mail_passwd_post);
			$sent = 1;
		}
	}

	if ($e && $pw && ($err == "")) {
		$res = mysql_query ("select id, lastlogon from users where email = '$e' and passwd = '$pw'", $conn);
		if (mysql_num_rows ($res)) {
			$uid = 0 + mysql_result ($res, 0, "id");
			$lastlogon = 0 + mysql_result ($res, 0, "lastlogon");
			$koek = "$e:$pw";
			for ($i =0; $i < strlen ($koek); $i++) 
				$koek[$i] = chr (ord ($koek[$i]) + 1);
			SetCookie ("wn_auth", $koek, time()+36000000, "/");
			$auth_ok = 1;
		} 
	} 


	if ($auth_ok && $t && $a) {
		switch ($a) {
		case "a":
			$tmpl = 0;
			if ($t > 0) 
				$res = mysql_query ("select id from docs where id = $t");
			else {
				$url = ereg_replace ("http://", "", $url);
				$host = strtok ($url, "/");
				$uri = substr ($url, strlen ($host));
				if (!$uri) $uri = "/";
				if (strlen ($host)) {
					$res = mysql_query ("select id, alt_host, alt_uri from docs_banned where host like '%" . $host . "%' and (uri = '' or uri = '$uri')");
					if (!mysql_num_rows ($res)) {
						$res = mysql_query ("select id from docs where host = '$host' and uri = '$uri'");
						if (!mysql_num_rows ($res)) {
							$url = addslashes ($url);
							$res = mysql_query ("insert into docs (id, host, title, uri, updated, status, changed, changed_serv, watchers, change_freq) values (0, '$host', '$url', '$uri', 0, 0, 0, '', 0, 0)");
		
							$res = mysql_query ("select id from docs where host = '$host' and uri = '$uri'");
						}
					} else {
						$alt_host = mysql_result ($res, 0, "alt_host");	
						$alt_uri = mysql_result ($res, 0, "alt_uri");	
						if ($alt_host && strlen ($alt_host)) {
							if (!$alt_uri) $alt_uri = "/";
							$res = mysql_query ("select id from docs where host = '$alt_host' and uri = '$alt_uri'", $conn);

							if (!mysql_num_rows ($res)) {
								$alt_uri = addslashes ($alt_uri);
								$res = mysql_query ("insert into docs (id, host, title, uri, updated, status, changed, changed_serv, watchers, change_freq) values (0, '$alt_host', 'http://$alt_host$alt_uri', '$alt_uri', 0, 0, 0, '', 0, 0)");
			
								$res = mysql_query ("select id from docs where host = '$alt_host' and uri = '$alt_uri'");
							}
							$tmpl = 2;
						} else $aerr = $txt_doc_banned;
					} 
				} else $aerr = $txt_err_fill_all_fields;
			}

			if (!$aerr) {
				if (mysql_num_rows ($res)) {
					$did = 0 + mysql_result ($res, 0, "id");
					if ($cat_id) {
						$res = mysql_query ("select cid from docs_cats where did = $did");
						if (!mysql_num_rows ($res)) {
							$res = mysql_query ("insert into docs_cats (did, cid) values ($did, $cat_id)", $conn);
							$res = mysql_query ("update cats set size = size + 1 where id = $cat_id", $conn);
						}
					}
					$res = mysql_query ("select uid from watch where uid = $uid and did = $did");
					if (!mysql_num_rows ($res)) {
						$res = mysql_query ("insert into watch (uid, did, ubase, email_prio, keywords, letter_update, filter, title, bfilter, tmpl) values ($uid, $did, 0, 1, '', 0, 3, '', '', $tmpl)");
						$res = mysql_query ("update docs set watchers = watchers + 1 where id = $did");
					}

					$d = $did;
					$a = "s"; 
				} else $aerr = $txt_err_try_again;
			}
			break;
		case "b":
			$bfilter = "";	
			if (is_array ($bf_id)) {
				for ($i = 0; $i < count ($bf_id); $i++) {
					if ($bf_id[$i] < 256)
						$bfilter .= chr($bf_id[$i]);
				}
			}
			mysql_query ("update watch set bfilter = '$bfilter' where did = $t and uid = $uid", $conn);
			break;
		case "s2":
			$filter = 0;
			$filter += $filter_link;
			$filter += $filter_text;
			$filter += $filter_image;
			$keywords = addslashes ($keywords);
			$mail = 0;
			$letter = 0;
			if ($period > 1) 
				$letter = $period;
			else if ($period == 1)
				$mail = 1;
			if (!isset ($title))
				$title = "";
			else
				$title = addslashes ($title);
			$tmpl = 0 + $tmpl;
	
			mysql_query ("update watch set email_prio = $mail, letter_update = $letter, keywords = '$keywords', filter = $filter, title = '$title', tmpl = $tmpl where did = $t and uid = $uid", $conn);
			break;
		case "u2":
			if (isset ($npw) || isset ($npw2)) {
				if ((!($npw == $npw2)) || (!strlen ($npw))) {
					$uerr = $txt_err_passwd_not_same;
					$a = "u";
				} else {
					$res = mysql_query ("update users set passwd = '$npw' where id = $uid", $conn);
				}
			}

			if (isset ($ne)) {
				$res = mysql_query ("update users set email = '$ne' where id = $uid", $conn);
			}

			if (isset ($qt) || isset ($qt2)) {
				if (!($qt && $qt2)) {
					$uerr = $txt_err_quit_not_verified;
					$a = "u";
				} else {
					$res = mysql_query ("select did from watch where uid = $uid", $conn);
					for ($i = 0; $i < mysql_num_rows ($res); $i++) {
						$did = 0 + mysql_result ($res, $i, "did");
						if ($did)
							$res2 = mysql_query ("update docs set watchers = watchers - 1 where id = $did", $conn);
					}
					$res = mysql_query ("delete from watch where uid = $uid", $conn);
					$res = mysql_query ("delete from users where id = $uid", $conn);
				}
			}

			break;
		case "d":
			$res = mysql_query ("delete from watch where uid = $uid and did = $t");
			$res = mysql_query ("update docs set watchers = watchers - 1 where id = $t");
			break;
		case "m0":
			$res = mysql_query ("update watch set email_prio = 0 where uid = $uid and did = $t");
			break;
		case "m1":
			$res = mysql_query ("update watch set email_prio = 1 where uid = $uid and did = $t");
			break;
		case "d0":
			$res = mysql_query ("update watch set letter_update = 0 where uid = $uid and did = $t");
			break;
		case "d1":
			$res = mysql_query ("update watch set letter_update = 24 where uid = $uid and did = $t");
			break;
		case "b1":
			$res = mysql_query ("update watch set letter_update = 48 where uid = $uid and did = $t");
			break;
		case "w1":
			$res = mysql_query ("update watch set letter_update = 168 where uid = $uid and did = $t");
			break;
		case "t1":
			$res = mysql_query ("update watch set title = '$title' where uid = $uid and did = $t");
			break;
		default:
		}
	}

	if ($w) {
		exec ("/usr/local/bin/swish -w '$w' -m 10 -f /usr/local/wn/swish/swish.index", $res);
		for ($i = 0; $i < count ($res); $i++) {
			if (($res[$i][0] != "#") && (!strstr ($res[$i], "err"))) {
				$arr = split (" ", $res[$i]);
				$results[] = $arr[1];
			}
		}
	}

	$topbutton .= "<a href=index.php?d=0&c=0 onMouseOver=\"document.close_right.src = 'img/close_orange_rollover.gif';\" onMouseOut=\"document.close_right.src = 'img/close_orange_normal.gif';\" onMouseDown=\"document.close_right.src = 'img/close_orange_click.gif';\"><img name=close_right src=img/close_orange_normal.gif width=12 height=12 border=0 alt=\"$txt_alt_backtop\"></a>";

	if (strlen ($QUERY_STRING))
		$add = "?$QUERY_STRING";

	echo "<head>\n<title>WebAgent</title>\n";
	echo "<meta http-equiv=Refresh content=\"1800;URL=/$add\">\n";
	if ($lang == "nl") {
		echo "<meta name=\"keyword\" value=\"website updates, veranderingen, nieuwsbrief\">\n";
		echo "<meta name=\"description\" value=\"WebAgent registreert veranderingen in websites en mailt deze direkt of in een dagelijkse nieuwsbrief naar leden. Webmasters kunnen WebAgent als dienst in hun site opnemen.\">\n";
	} else {
		echo "<meta name=\"keywords\" value=\"website updates, update monitor, change detection, automatic newsletter\">\n";
		echo "<meta name=\"description\" value=\"WebAgent archives changes in websites and mails them directly or periodically in a newsletter to subscribers. Webmasters can integrate WebAgent in their sites as a service.\">\n";
	}
	echo "</head>\n<body background=img/bg.gif bgcolor=#999999 link=$text2_color alink=$text2_color vlink=$text2_color>\n";
?>
<table border=0 width=100% cellpadding=0 cellspacing=0>
<tr>
<td colspan=2 width=100% height=6 bgcolor=#336666><img src=img/trans.gif height=6></td>
</tr>
<tr>
<td valign=top width=276 bgcolor=#336666 align=right> 
<?
	echo "<a href=index.php?d=0&c=0><img src=img/logo.gif width=276 height=55 border=0 align=left alt=\"$txt_alt_backtop\"></a><br clear=all><table border=0 width=100% cellpadding=0 cellspacing=0><tr><td nowrap><font face=$font_face size=-4 color=$text_color>&nbsp;&nbsp;";
	if ($lang == "nl") {
		$qs = ereg_replace ("&(l=nl|l=en)", "", $QUERY_STRING);
		echo "<a href=\"index.php?$qs&l=en\"><font color=$text_color>EN</font></a>/";
	} else
		echo "<b>EN</b>/";
	if ($lang == "en") {
		$qs = ereg_replace ("&(l=en|l=nl)", "", $QUERY_STRING);
		echo "<a href=\"index.php?$qs&l=nl\"><font color=$text_color>NL</font></a>";
	} else
		echo "<b>NL</b>";
	echo "<td nowrap align=right><font face=$font_face size=-4 color=$text_color>&nbsp;&nbsp;$txt_intro_short</td></tr></table>\n";
?>
</td><td valign=top width=500 bgcolor=#336666 align=center nowrap>
<img src=img/trans.gif width=12 border=0><a href=<? echo $adurl[$banneridx]; ?>><img src=<? echo $banner[$banneridx] ; ?> width=476 height=60 border=0 valign=middle></a><img src=img/trans.gif width=12 border=0>
</td></tr>
<tr>
<td colspan=2 width=100% height=3 bgcolor=#336666><img src=img/trans.gif height=3></td>
</tr>
</table>

<table border=0 width=100% cellpadding=0 cellspacing=0>
<? 	
	echo "<tr><td colspan=3 height=$padbetween><img src=img/trans.gif height=$padbetween border=0></td></tr>\n";
	echo "<tr><td valign=top height=40> \n";
	if ($auth_ok) {
		include "menu.php";
	} else {
		include "login.php";
	}
	echo "</td><td width=$padbetween><img src=img/trans.gif width=$padbetween border=0></td>\n";
	echo "<td align=left valign=top width=70%><font face=Arial,Geneva size=-1>\n";

	switch ($a) {
	case "s":
		if ($auth_ok)
			include "settings.php";
		break;
	case "u":
		if ($auth_ok)
			include "user.php";
		break;
	case "f":
		include "faq.php";
		break;
	default:
		include "display.php";
	}

	page_end ();
?>
