<?
	include "db.php";
	include "new_text.php";

	if (!$referer)
		$referer = $HTTP_REFERER;

	$msg = "";
	if (!$url) 
		$msg = $txt_err_no_url;
	if (!$e)
		$msg = $txt_err_no_email;
	if (!$pw) {
		$pw = "";
		for ($i = 0; $i < 6; $i++)
			$pw .= chr(rand(48,57));
	}

	$uid = 0;
	if (!$msg) {
		if (!eregi ("(.*)@(.*)\.(.*)", $e))
			$msg = $txt_err_email_invalid;
		else {
			$res = mysql_query ("select id, passwd from users where email = '$e'", $conn);
			if (mysql_num_rows ($res)) {
				if ($pw == mysql_result ($res, 0, "passwd")) 
					$uid = 0 + mysql_result ($res, 0, "id");
				else
					$msg = $txt_err_email_exists;
			}
		}
	}

	if (!$msg) {
		$tmpl = 0;
		$url = ereg_replace ("http://", "", $url);
		$host = strtok ($url, "/");
		$uri = substr ($url, strlen ($host));
		if (!$uri) $uri = "/";
		if (strlen ($host)) {
			$res = mysql_query ("select id, alt_host, alt_uri from docs_banned where host like '%" . $host . "%'  and (uri = '' or uri = '$uri')");
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
				} else $msg = $txt_doc_banned;
			}
		} else $msg = $txt_err_fill_all_fields;
	}

	if (!$msg) {
		$now = date ("U");
		if (!$uid)
			$res2 = mysql_query ("insert into users values (0, '$e','$pw', $now)", $conn);

		$res2 = mysql_query ("select id, lastlogon from users where email = '$e' and passwd = '$pw'", $conn);
		if (mysql_num_rows ($res2)) {
			$uid = 0 + mysql_result ($res2, 0, "id");
			$lastlogon = 0 + mysql_result ($res2, 0, "lastlogon");
			$koek = "$e:$pw";
			for ($i =0; $i < strlen ($koek); $i++) 
			$koek[$i] = chr (ord ($koek[$i]) + 1);
			SetCookie ("wn_auth", $koek, time()+36000000, "/");
		} 

		if (mysql_num_rows ($res)) {
			$did = 0 + mysql_result ($res, 0, "id");
			if (!$cat_id) 
				$cat_id = -1;

			$res = mysql_query ("select cid from docs_cats where did = $did");
			if (!mysql_num_rows ($res)) {
				$res = mysql_query ("insert into docs_cats (did, cid) values ($did, $cat_id)", $conn);
				$res = mysql_query ("update cats set size = size + 1 where id = $cat_id", $conn);
			}
			$res = mysql_query ("select uid from watch where uid = $uid and did = $did");
			if (!mysql_num_rows ($res)) {
				$res = mysql_query ("insert into watch (uid, did, ubase, email_prio, keywords, letter_update, filter, tmpl) values ($uid, $did, 0, 1, '', 0, 3, $tmpl)");
				$res = mysql_query ("update docs set watchers = watchers + 1 where id = $did");
			}

			if ($url_ok) {
				header ("HTTP/1.0 301 Redirected");
				header ("Location: $url_ok");
				exit;
			}

			include "new2.php";
			exit;
		} else $msg = $txt_err_try_again;
	}

	if ($url_fail) {
		header ("HTTP/1.0 301 Redirected");
		header ("Location: $url_fail");
		exit;
	}

	$msg_big = "Er ging iets mis..";
	include "new2.php";
?>
