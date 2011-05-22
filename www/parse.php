<title>Parse</title>
<body bgcolor=#ccccaa link=#000000 alink=#000000 vlink=#000000>
<font face=Arial,Geneva>
<form method=post action=parse.php>
WebAgent URL Parser V0.9 - URL: <input name=url value=<? echo $url; ?>>
<input type=submit value="     OK     "></form>
<hr noshade size=1>
<form method=post action=parse.php>
<table border=0>

<?
	function process_id ($id) {
		global $old_id;
		global $color;
		if ($id != $old_id) {
			$old_id = $id;
			if ($color == "#FFFFCC")
				$color = "#CCFFFF";
			else
				$color = "#FFFFCC";
			echo "</tr><tr><td valign=top bgcolor=$color><input type=checkbox name=\"bid[]\" value=\"$id\"></td><td bgcolor=$color><font face=Arial>";
		}	
	}

	if (is_array ($bid)) {
		for ($i = 0; $i < count ($bid); $i++) {
			echo "bid = " . $bid[$i] . "<br>\n";
		}
	}

	if ($url) {
		if (substr ($url, 0, 7) == "http://")
			$url = substr ($url, 8);
		$host = strtok ($url, "/");
		$uri = strtok ("/");
		if (!$uri) $uri = "/";

		exec ("/home/w/e/webagent/bin/wnparse $host $uri /tmp/out");

		$f = fopen ("/tmp/out", "r");
		if (!$f)
			echo "no output..?\n";
		else {
			echo "<table border=0>\n";
			while ($str = fgets ($f, 4096)) {
				if (eregi ("([0-9]+) ([A-Z]+) ([^ ]+) ([^\n\r]+)", $str, $regs)) {
					process_id ($regs[1]);
					switch ($regs[2]) {
					case "TEXT":
						echo $regs[4] . " ";
						break;
					case "LINK":
						echo "<a href=\"" . $regs[3] . "\" target=_x>";
						if ($regs[4]) 
							echo $regs[4];
						else
							echo $regs[3];
						echo "</a> \n";
						break;
					case "IMG":
						echo "<img src=\"" . $regs[3] . "\" border=0> \n";
						break;
					}
				}
			}
			echo "</tr></table>\n";
			fclose ($f);
		}		
		echo "<hr noshade size=1>\n";
	}

	echo "</table>\n";
?>
<input type=submit value=" FILTEREN "></form>
