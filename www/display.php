<?
	include "display_text.php";

	$wtitle = "<a href=\"index.php?d=0&c=0\"><font color=#ffffff>$txt_top</font></a> ";

	$d = 0 + $d;
	$c = 0 + $c;
	$p_id = 0;
	$docdir = "/home/webagent/data/";

	if (!$conn) {
		window_start ($wtitle, $topbutton, $text_color, $right_headercolor, $right_bgcolor);

		echo "<font face=$font_face color=$text_color><br><blockquote><b>$txt_no_db<br><br>\n";

		window_end ($right_bgcolor);
		page_end ();
	}

	if ($c) {
		if ($lang) 
			$res = mysql_query ("select p_id, name_$lang from cats where id = $c", $conn);
		else
			$res = mysql_query ("select p_id, name from cats where id = $c", $conn);
		if (mysql_num_rows ($res)) {
			$p_id = 0 + mysql_result ($res, 0, "p_id");
			if ($lang)
				$name = stripslashes (mysql_result ($res, 0, "name_$lang"));
			else
				$name = stripslashes (mysql_result ($res, 0, "name"));
			if ($c > 0)
				$wtitle .= ": <a href=\"index.php?c=$c&d=0\"><font color=#ffffff>$name</font></a>";
			else 
				$wtitle .= ": $name";
		}
	}

	if ($d) {
		$vspace = 0;
		$max_count = 20;
		$max_shortlinks = 4;
		$dstr = "$d";
		for ($i = 0; $i < strlen ($dstr); $i++)
			$docdir .= $dstr[$i] . "/";

		if ($monitor[$d])
			$res = mysql_query ("select docs.host, docs.uri, docs.title, docs.status, docs.changed, watch.keywords, watch.filter, watch.title as wtitle, watch.bfilter from docs, watch where docs.id = $d and watch.did = $d and watch.uid = $uid");
		else
			$res = mysql_query ("select docs.host, docs.uri, docs.title, docs.status, docs.changed from docs where docs.id = $d");

		if (mysql_num_rows ($res)) {
			$host = mysql_result ($res, 0, "docs.host");
			$uri = mysql_result ($res, 0, "docs.uri");
			$title = stripslashes (mysql_result ($res, 0, "docs.title"));
			if (strlen ($title) > $max_title_len)
				$title = substr ($title, 0, $max_title_len) . $title_ext;
			$status = 0 + mysql_result ($res, 0, "docs.status");
			$changed = 0 + mysql_result ($res, 0, "docs.changed");
			$shw_alt = 0;
	
			$res2 = mysql_query ("select shw_host, shw_uri from docs_banned where alt_host like '%" . $host . "%' and (alt_uri = '' or alt_uri = '$uri')", $conn);
			if (mysql_num_rows ($res2)) {
				$host = mysql_result ($res2, 0, "shw_host");
				$uri = mysql_result ($res2, 0, "shw_uri");
				$shw_alt = 1;
			}
	
			$wbutton = "";

			if (!$monitor[$d]) {
				$wtitle .= " : <a href=\"http://$host$uri\" target=\"_x\"><font color=#ffffff>$title</font></a></b> ";
				if ($auth_ok) {
					$wbutton = "<a href=\"index.php?d=$d&c=$c&s=$s&t=$d&a=a\">";
					$wbutton .= "<img src=img/mailinst_toevoeg_n.gif border=0 width=17 height=12 alt=\"$txt_alt_add\"></a>&nbsp;";
				}
				$filter = 3;  // standaard geen images
			} else {
				$keywords = stripslashes (mysql_result ($res, 0, "watch.keywords"));
				$filter = 0 + mysql_result ($res, 0, "watch.filter");
				$watch_title = stripslashes (mysql_result ($res, 0, "wtitle"));
				$bfilter = mysql_result ($res, 0, "watch.bfilter");
				if (strlen ($watch_title))
					$wtitle .= " : <a href=\"http://$host$uri\" target=\"_x\"><font color=#ffffff>$watch_title</font></a></b> ";
				else
					$wtitle .= " : <a href=\"http://$host$uri\" target=\"_x\"><font color=#ffffff>$title</font></a></b> ";

				if ($keywords) {
					if ($m) 
						$wtitle .= "<a href=\"index.php?d=$d&c=$c&s=$s&m=0\"><b>";
					else
						$wtitle .= "<a href=\"index.php?d=$d&c=$c&s=$s&m=1\">";
					$wtitle .= "<font color=\"#ffffaa\">[$keywords]</font></a>\n";
				}
			}

			$wbutton .= $topbutton;

			window_start ($wtitle, $wbutton, $text_color, $right_headercolor, $right_bgcolor);
		} else {
			window_start ($wtitle, $topbutton, $text_color, $right_headercolor, $right_bgcolor);

			echo "<font face=$font_face color=$text_color><br><blockquote><b>$txt_not_exist<br><br>\n";

			window_end ($right_bgcolor);

			page_end ();
		}

		if (!$bf && !$changed) {
			echo "<font face=$font_face color=$text_color><br><blockquote><b>$txt_not_changed<br><br>\n";

			window_end ($right_bgcolor);

			page_end ();
		} 

		echo "<table border=0 cellpadding=0 cellspacing=0 width=100%><tr><td bgcolor=$right_bgcolor width=$padwidth><img src=img/trans.gif border=0 width=$padwidth></td>\n";
		echo "<td bgcolor=$right_bgcolor><font face=$font_face size=-1>\n";
		if ($dir = @opendir ($docdir)) {
			while (($e = @readdir ($dir)) != "") {
				if ($bf && strstr ($e, "new")) {
					$dirent[] = $e;
					break;
				}
				if (!strstr ($e, "new") && (strlen ($e) > 3)) 
					$dirent[] = $e;
			}
			closedir ($dir);
	
			if (is_array ($dirent)) {
				rsort ($dirent);
				reset ($dirent);
				$count = 0;
				$shortlinks = 0;
				for ($i = 0; $i < count ($dirent); $i++) {
					$e = $dirent[$i];
					if ($s && ($e > $s))
						continue;
					$date = date ("M j H:i", $e);
					if (!$bf && ($count >= $max_count)) {
						if ($shortlinks >= $max_shortlinks)
							break;
						if (!$bottom_printed) {
							if ($vspace) echo "</td></tr><tr><td colspan=2 bgcolor=$right_bgcolor height=$padheight><img src=img/trans.gif border=0 height=$padheight>\n"; else $vspace = 1;
							echo "</td></tr><tr><td colspan=2 bgcolor=$right_subheadercolor valign=top>\n";
							echo "&nbsp;<img src=img/arrow.gif border=0 width=15 height=14>\n";
							echo "<font face=$font_face size=-3>&nbsp;&nbsp;\n";
							$bottom_printed = 1;
						}
						echo "<a href=\"index.php?d=$d&c=$c&s=$e\"><font color=$text2_color>" . strtoupper ($date) . "</font></a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
						$shortlinks++;
					} else {

					if ($f = fopen ("$docdir/$e", "r")) {
						$first_link = 1;
						$header_printed = 0;
						while ($str = fgets ($f, 4096)) {
							if (eregi ("([0-9]+) ([A-Z]+) ([^ ]+) ([^\n\r]+)", $str, $regs)) {
								switch ($regs[2]) {
								case "TEXT":
									if (($filter && !($filter & 2)) || ($m && $keywords && !eregi ($keywords, $regs[4])))
										break;

									if (strlen ($regs[4]) < 3)
										break;

									$nr = 0 + $regs[1];

									if (!$bf && $bfilter && strstr ($bfilter, chr($nr)))
										break;

									if (!$header_printed) {
										if ($vspace) echo "<tr><td colspan=2 bgcolor=$right_bgcolor height=$padheight><img src=img/trans.gif border=0 height=$padheight></td></tr>\n"; else $vspace = 1;
										if ($bf) 
											echo "<tr><td bgcolor=$right_subheadercolor colspan=2><font face=$font_face size=-3>&nbsp;&nbsp;" . $txt_bf_header . "</td></tr>\n";
										else
											echo "<tr><td bgcolor=$right_subheadercolor colspan=2><table width=100% border=0 cellpadding=0 cellspacing=0><tr><td nowrap><font face=$font_face size=-3>&nbsp;&nbsp;" . strtoupper ($date) . "</td><td align=right><font face=$font_face size=-4><a href=\"http://$host$uri\" target=_x>http://" . strtolower($host) . strtolower($uri) . "</a>&nbsp;</td></tr></table></td></tr>\n";
										echo "<tr><td bgcolor=$right_subbgcolor width=$padwidth valign=top><img src=img/trans.gif border=0 width=$padwidth></td><td bgcolor=$right_subbgcolor width=99%><font face=$font_face size=-1>\n";
										if ($bf) {
											echo "<form method=post action=index.php>\n";
											echo "<input type=hidden name=d value=\"$d\">\n";
											echo "<input type=hidden name=c value=\"$c\">\n";
											echo "<input type=hidden name=t value=\"$d\">\n";
											echo "<input type=hidden name=a value=b>\n";
											echo "<input type=hidden name=bf value=1>\n";
											echo "<br>$txt_bf_intro<p>\n";
											if (strstr ($bfilter, chr($nr))) 
												$bf_bgcolor = $bf_bgcolor_dimmed;
											else
												$bf_bgcolor = $bf_bgcolor_normal;

											echo "<table border=0 cellpadding=0 cellspacing=0 width=100%><tr><td bgcolor=$bf_bgcolor align=center><input type=checkbox name=bf_id[] value=\"$nr\"";
											if (strstr ($bfilter, chr($nr)))
												echo " checked";
											echo "></td><td bgcolor=$bf_bgcolor><font face=$font_face size=-1>\n";
										}
										$header_printed = 1;
									}

									if (($nr != $old_nr) && !$first_link) {
										$old_nr = $nr;
										echo "<br>\n";
										if ($bf) {
											if (strstr ($bfilter, chr($nr))) 
												$bf_bgcolor = $bf_bgcolor_dimmed;
											else
												$bf_bgcolor = $bf_bgcolor_normal;
											echo "</td></tr><tr><td bgcolor=$right_subbgcolor colspan=2 height=5><img src=img/trans.gif border=0 height=5></td></tr>\n";
											echo "<tr><td bgcolor=$bf_bgcolor align=center>";
											if (!$bf_set[$nr]) {
												$bf_set[$nr] = 1;
												echo "<input type=checkbox name=bf_id[] value=\"$nr\"";
												if (strstr ($bfilter, chr($nr))) {
													echo " checked>";
	$bf_set[$nr] = 2;
												}
												
											} else {
												if ($bf_set[$nr] == 2)
													echo "<font size=-1><b>v</b></font>";
											}
											echo "</td><td bgcolor=$bf_bgcolor><font face=$font_face size=-1>\n";
										}
										$count++;
									}

									if (strlen ($regs[4]) > 70)
										echo substr ($regs[4], 0, 70) . "(..) ";
									else
										echo $regs[4] . " ";
									$count += count (split ("\n", $regs[4]));

									if ($first_link)
										$first_link = 0;
									break;
								case "LINK":
									if (($filter && !($filter & 1)) || ($m && $keywords && !eregi ($keywords, $regs[4])))
										break;

									if (strlen ($regs[4]) < 3)
										break;

									$nr = 0 + $regs[1];

									if (!$bf && $bfilter && strstr ($bfilter, chr($nr)))
										break;

									if (!$header_printed) {
										if ($vspace) echo "<tr><td colspan=2 bgcolor=$right_bgcolor height=$padheight><img src=img/trans.gif border=0 height=$padheight></td></tr>\n"; else $vspace = 1;
										if ($bf) 
											echo "<tr><td bgcolor=$right_subheadercolor colspan=2><font face=$font_face size=-3>&nbsp;&nbsp;" . $txt_bf_header . "</td></tr>\n";
										else
											echo "<tr><td bgcolor=$right_subheadercolor colspan=2><table width=100% border=0 cellpadding=0 cellspacing=0><tr><td nowrap><font face=$font_face size=-3>&nbsp;&nbsp;" . strtoupper ($date) . "</td><td align=right><font face=$font_face size=-4><a href=\"http://$host$uri\" target=_x>http://" . strtolower($host) . strtolower($uri) . "</a>&nbsp;</td></tr></table></td></tr>\n";
										echo "<tr><td bgcolor=$right_subbgcolor width=$padwidth valign=top><img src=img/trans.gif border=0 width=$padwidth></td><td bgcolor=$right_subbgcolor width=99%><font face=$font_face size=-1>\n";
										if ($bf) {
											echo "<form method=post action=index.php>\n";
											echo "<input type=hidden name=d value=\"$d\">\n";
											echo "<input type=hidden name=c value=\"$c\">\n";
											echo "<input type=hidden name=t value=\"$d\">\n";
											echo "<input type=hidden name=a value=b>\n";
											echo "<input type=hidden name=bf value=1>\n";
											echo "<br>$txt_bf_intro<p>\n";
											if (strstr ($bfilter, chr($nr))) 
												$bf_bgcolor = $bf_bgcolor_dimmed;
											else
												$bf_bgcolor = $bf_bgcolor_normal;
											echo "<table border=0 cellpadding=0 cellspacing=0 width=100%><tr><td bgcolor=$bf_bgcolor align=center><input type=checkbox name=bf_id[] value=\"$nr\"";
											if (strstr ($bfilter, chr($nr)))
												echo " checked";
											echo "></td><td bgcolor=$bf_bgcolor><font face=$font_face size=-1>\n";
										}
										$header_printed = 1;
									}

									if (($nr != $old_nr) && !$first_link) {
										$old_nr = $nr;
										echo "<br>\n";
										if ($bf) {
											if (strstr ($bfilter, chr($nr))) 
												$bf_bgcolor = $bf_bgcolor_dimmed;
											else
												$bf_bgcolor = $bf_bgcolor_normal;
											echo "</td></tr><tr><td bgcolor=$right_subbgcolor colspan=2 height=5><img src=img/trans.gif border=0 height=5></td></tr>\n";
											echo "<tr><td bgcolor=$bf_bgcolor align=center>";
											if (!$bf_set[$nr]) {
												$bf_set[$nr] = 1;
												echo "<input type=checkbox name=bf_id[] value=\"$nr\"";
												if (strstr ($bfilter, chr($nr))) {
													echo " checked>";
	$bf_set[$nr] = 2;
												}
												
											} else {
												if ($bf_set[$nr] == 2)
													echo "<font size=-1><b>v</b></font>";
											}
											echo "</td><td bgcolor=$bf_bgcolor><font face=$font_face size=-1>\n";
										}
										$count++;
									}

									if ($first_link) 
										$first_link = 0;
									else 
										echo "<br>";

									if ($shw_alt)
										echo "<a href=\"" . ereg_replace ("'", "", $regs[3]) . "\" target=_x>";
									else
										echo "<u>";

									if ($regs[4]) 
										echo $regs[4];
									else
										echo $regs[3];

									if ($shw_alt)
										echo "</a>&nbsp;\n";
									else
										echo "</u>&nbsp;\n";
									$count++;
									break;
								case "IMG":
									if (($filter && !($filter & 4)) || ($m && $keywords && !eregi ($keywords, $regs[4])))
										break;

									$nr = 0 + $regs[1];

									if (!$bf && $bfilter && strstr ($bfilter, chr($nr)))
										break;

									if (!$header_printed) {
										if ($vspace) echo "<tr><td colspan=2 bgcolor=$right_bgcolor height=$padheight><img src=img/trans.gif border=0 height=$padheight></td></tr>\n"; else $vspace = 1;
										if ($bf) 
											echo "<tr><td bgcolor=$right_subheadercolor colspan=2><font face=$font_face size=-3>&nbsp;&nbsp;" . $txt_bf_header . "</td></tr>\n";
										else
											echo "<tr><td bgcolor=$right_subheadercolor colspan=2><table width=100% border=0 cellpadding=0 cellspacing=0><tr><td nowrap><font face=$font_face size=-3>&nbsp;&nbsp;" . strtoupper ($date) . "</td><td align=right><font face=$font_face size=-4><a href=\"http://$host$uri\" target=_x>http://" . strtolower($host) . strtolower($uri) . "</a>&nbsp;</td></tr></table></td></tr>\n";
										echo "<tr><td bgcolor=$right_subbgcolor width=$padwidth valign=top><img src=img/trans.gif border=0 width=$padwidth></td><td bgcolor=$right_subbgcolor width=99%><font face=$font_face size=-1>\n";
										if ($bf) {
											echo "<form method=post action=index.php>\n";
											echo "<input type=hidden name=d value=\"$d\">\n";
											echo "<input type=hidden name=c value=\"$c\">\n";
											echo "<input type=hidden name=t value=\"$d\">\n";
											echo "<input type=hidden name=a value=b>\n";
											echo "<input type=hidden name=bf value=1>\n";
											echo "<br>$txt_bf_intro<p>\n";
											if (strstr ($bfilter, chr($nr))) 
												$bf_bgcolor = $bf_bgcolor_dimmed;
											else
												$bf_bgcolor = $bf_bgcolor_normal;
											echo "<table border=0 cellpadding=0 cellspacing=0 width=100%><tr><td bgcolor=$bf_bgcolor align=center><input type=checkbox name=bf_id[] value=\"$nr\"";
											if (strstr ($bfilter, chr($nr)))
												echo " checked";
											echo "></td><td bgcolor=$bf_bgcolor><font face=$font_face size=-1>\n";
										}
										$header_printed = 1;
									}

									if (($nr != $old_nr) && !$first_link) {
										echo "<br>\n";
										if ($bf) {
											if (strstr ($bfilter, chr($nr))) 
												$bf_bgcolor = $bf_bgcolor_dimmed;
											else
												$bf_bgcolor = $bf_bgcolor_normal;
											echo "</td></tr><tr><td bgcolor=$right_subbgcolor colspan=2 height=5><img src=img/trans.gif border=0 height=5></td></tr>\n";
											echo "<tr><td bgcolor=$bf_bgcolor align=center>";
											if (!$bf_set[$nr]) {
												$bf_set[$nr] = 1;
												echo "<input type=checkbox name=bf_id[] value=\"$nr\"";
												if (strstr ($bfilter, chr($nr))) {
													echo " checked>";
	$bf_set[$nr] = 2;
												}
												
											} else {
												if ($bf_set[$nr] == 2)
													echo "<font size=-1><b>v</b></font>";
											}
											echo "</td><td bgcolor=$bf_bgcolor><font face=$font_face size=-1>\n";
										}
										$count++;
									}

									echo "<img src=\"" . ereg_replace ("'", "", $regs[3]) . "\" border=0> \n";
									$count++;
									break;
								}
							}
						}
						fclose ($f);
					} 
					if ($header_printed)
						if ($bf) {
							echo "</td></tr></table><p>\n";
							echo "<font size=-3>$txt_bf_sub<p>$txt_bf_outtro</font><p>\n";
							echo "<div align=center><input type=submit value=\"   OK   \"></div></form>\n";
						} else 
							echo "<br><br>\n";
					}
				}
			} else {
				if ($bf)
					echo "<font face=$font_face color=$text_color><blockquote><b>$txt_not_visited<br><br>\n";
				else
					echo "<font face=$font_face color=$text_color><blockquote><b>$txt_not_changed<br><br>\n";

				window_end ($right_bgcolor);

				page_end ();
			}
		} else {
			echo "<font face=$font_face color=$text_color><blockquote><b>$txt_not_visited<br><br>\n";

			window_end ($right_bgcolor);

			page_end ();
		}

		echo "</td></tr></table>\n";

		if (($status != 0) && ($status != 13)) {
			echo "<font face=$font_face size=-1><hr noshade size=1>$txt_err_at_visit\n";

			switch ($status) {
			case 15:
				echo "the remote server denied us the document\n"; break;
			case 14:
				echo "the document was retrieved from a new location\n"; break;
			case 10:
				echo "there was a problem analyzing the content\n"; break;
			case 9:	
				echo "there was no content in this document\n"; break;
			case 8:
			case 7:
				echo "there was a problem analyzing the content\n"; break;
			case 6:
				echo "there was a problem saving the content\n"; break;
			case 4:
				echo "the retrieval was aborted because it took too long\n"; break;
			case 3:
				echo "the remote server was not accepting connections\n"; break;
			case 2:
				echo "the domain didn't seem to exist anymore\n"; break;
			case 1:
				echo "there was a problem using the network\n"; break;
			case -1:
				echo "there was an unexpected error\n"; break;
			default:
			}
		}

		window_end ($right_bgcolor);

		page_end ();
	} 

	if ($c < 0)
		$c = 0;

	if (!$c) 
		window_start ($wtitle, "", $text_color, $right_headercolor, $right_bgcolor);
	else
		window_start ($wtitle, $topbutton, $text_color, $right_headercolor, $right_bgcolor);

	echo "<table border=0 width=100% cellpadding=0 cellspacing=0>\n";


	switch ($b) {
	case 2:
		$res = mysql_query ("select docs.id, docs.title, docs.changed, docs.watchers, docs.change_freq from docs, docs_cats where docs_cats.cid = $c and docs_cats.did = docs.id order by docs.change_freq desc limit 40", $conn);
		break;
	case 1:
		$res = mysql_query ("select docs.id, docs.title, docs.changed, docs.watchers, docs.change_freq from docs, docs_cats where docs_cats.cid = $c and docs_cats.did = docs.id order by docs.watchers desc limit 40", $conn);
		break;
	default:
		$res = mysql_query ("select docs.id, docs.title, docs.changed, docs.watchers, docs.change_freq from docs, docs_cats where docs_cats.cid = $c and docs_cats.did = docs.id order by docs.changed desc limit 40", $conn);
		break;
	}

	if ($lang)
		$res2 = mysql_query ("select id, name_$lang, size from cats where p_id = $c and id > 0 order by name_$lang", $conn);
	else
		$res2 = mysql_query ("select id, name, size from cats where p_id = $c and id > 0 order by name", $conn);
	if (mysql_num_rows ($res2)) {
		echo "<tr><td colspan=4 bgcolor=$right_subheadercolor height=10><font face=$font_face size=-3>&nbsp;&nbsp;$txt_categories</font></td></tr>\n";
		echo "<tr><td bgcolor=$right_subbgcolor colspan=4><table border=0 width=100%>\n";

		$j = mysql_num_rows ($res2) / 2;
		for ($i = 0; $i < $j; $i++) {
			$id = 0 + mysql_result ($res2, $i, "id");
			if ($lang)
				$name = stripslashes (mysql_result ($res2, $i, "name_$lang"));
			else
				$name = stripslashes (mysql_result ($res2, $i, "name"));
			$size = 0 + mysql_result ($res2, $i, "size");

			echo "<tr><td bgcolor=$right_subbgcolor width=50%>&nbsp;&nbsp;<font face=$font_face size=-1><a href=\"index.php?d=0&c=$id\">$name</a> <font color=$text3_color><i>($size)</i></td>\n";

			if (($i + $j) < mysql_num_rows ($res2)) {
				$id = 0 + mysql_result ($res2, $i + $j, "id");
				if ($lang)
					$name = stripslashes (mysql_result ($res2, $i + $j, "name_$lang"));
				else
					$name = stripslashes (mysql_result ($res2, $i + $j, "name"));
				$size = 0 + mysql_result ($res2, $i + $j, "size");
		
				echo "<td bgcolor=$right_subbgcolor width=50%><font face=$font_face size=-1><a href=\"index.php?d=0&c=$id\">$name</a> <font color=$text3_color><i>($size)</i></tr></tr>\n";
			} else
				echo "<td bgcolor=$right_subbgcolor>&nbsp;</td></tr>\n";
		}

		echo "</table><br></td></tr>\n";
	} 

	if ($c) {
		echo "<tr><td bgcolor=$right_subheadercolor height=10 width=90%><font face=$font_face size=-3>&nbsp;&nbsp;$txt_documents</font></td>\n";
		echo "<td align=center nowrap bgcolor=$right_subheadercolor height=10 width=1%>";
		echo "<font face=$font_face size=-3><a href=index.php?d=0&c=$c&b=1>";
		if ($b == 1)
			echo "<font color=#000000><b>";
		else
			echo "<font color=#666600>";
		echo "$txt_subscr</a>&nbsp;</td>\n<td width=1% align=right nowrap bgcolor=$right_subheadercolor height=10><font face=$font_face size=-3><a href=index.php?d=0&c=$c&b=2>";
		if ($b == 2)
			echo "<font color=#000000><b>";
		else
			echo "<font color=#666600>";
		echo "$txt_chgswk</a>&nbsp;</td>\n<td width=1% align=right nowrap bgcolor=$right_subheadercolor height=10><font face=$font_face size=-3><a href=index.php?d=0&c=$c&b=0>";
		if ($b == 0)
			echo "<font color=#000000><b>";
		else
			echo "<font color=#666600>";
		echo "$txt_last_changed</a>&nbsp;</font></td></tr>\n";

		echo "<tr><td bgcolor=$right_subbgcolor height=$padheight colspan=4><img src=img/trans.gif height=$padheight></td></tr>\n";
	}

	if (mysql_num_rows ($res)) {
		for ($i = 0; $i < mysql_num_rows ($res); $i++) {
			$doc_id = 0 + mysql_result ($res, $i, "docs.id");
			$title = stripslashes (mysql_result ($res, $i, "docs.title"));
			if (strlen ($title) > $max_title_len)
				$title = substr ($title, 0, $max_title_len) . $title_ext;
			$watchers = 0 + mysql_result ($res, $i, "docs.watchers");
			$change_freq = 0 + mysql_result ($res, $i, "docs.change_freq");
			$changed = 0 + mysql_result ($res, $i, "docs.changed");
	
			echo "<tr><td nowrap bgcolor=$right_subbgcolor width=90%>\n";
			if ($auth_ok) {
				if (!$monitor[$doc_id]) {
				echo "&nbsp;<a href=\"index.php?d=$d&c=$c&s=$s&t=$doc_id&a=a\">";
				echo "<img src=img/mailinst_toevoeg_n.gif border=0 width=17 height=12 alt=\"$txt_alt_add\">";
				echo "</a>&nbsp;\n";
				} else {
					echo "&nbsp;<img src=img/trans.gif border=0 width=17 height=12>&nbsp;\n";
					$res3 = mysql_query ("select title from watch where did = $doc_id and uid = $uid", $conn);
					if (mysql_num_rows ($res3)) {
						$wtitle = stripslashes (mysql_result ($res3, 0, "title"));
						if (strlen ($wtitle))
							$title = $wtitle;
					}
				}
			} else echo "&nbsp;&nbsp;";
			
			echo "<font face=$font_face size=-1><a href=\"?d=$doc_id&c=$c&s=$s\">$title</a></td><td bgcolor=$right_subbgcolor nowrap width=1% align=right><font face=$font_face size=-1><i>";
			if ($b != 1)
				echo "<font color=#666600>";
			echo "$watchers&nbsp;</td>\n";
			echo "<td bgcolor=$right_subbgcolor nowrap width=1% align=center><font face=$font_face size=-1><i>";
			if ($b != 2)
				echo "<font color=#666600>";

			if ($change_freq)
				echo $change_freq;
			else
				echo "-";
			echo "&nbsp;</td>\n<td bgcolor=$right_subbgcolor nowrap width=1% align=right><font face=$font_face size=-1><i>";
			if ($b != 0)
				echo "<font color=#666600>";

			if ($changed)
				echo date ("M j H:i", $changed);
			else
				echo "-";
			echo "&nbsp;</td></tr>\n";
		}
	} else if ($c)
		echo "<tr><td bgcolor=$right_subbgcolor colspan=4><font face=$font_face size=-1>&nbsp;&nbsp;$txt_empty_category</td></tr>\n";

	echo "<tr><td bgcolor=$right_subbgcolor height=$padheight colspan=4><img src=img/trans.gif height=$padheight></td></tr>\n";
	echo "</table>\n";

	window_end ($right_bgcolor);

	if (!$c) {
		echo "<img src=img/trans.gif height=$padbetween border=0>\n";

		include "faq.php";
	}
?>
