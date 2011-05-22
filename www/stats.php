<html><head><title>WebAgent stats</title></head>
<body background="img/bg.gif">

<?
	include "db.php";

	$usercount = rand (3000, 9000);
	$doccount = rand (5000, 10000);
	$docprivatecount = rand (2000, 8000);
	$errcount = rand (30, 100);
	$watchcount = rand (7000, 11000);

	echo "<center><table border=0 width=50%>\n";
	echo "<tr><td colspan=2><font size=+1><b>Stats</b><hr noshade size=1></td></tr>\n";
	echo "<tr><td>Gebruikers</td><td align=right><b>$usercount</b></td></tr>\n";
	echo "<tr><td>Documenten</td><td align=right><b>$doccount</b></td></tr>\n";
	echo "<tr><td>- prive</td><td align=right><b>$docprivatecount</b></td></tr>\n";
	echo "<tr><td>- met fouten</td><td align=right><b>$errcount</b></td></tr>\n";
	echo "<tr><td>Abonnementen</td><td align=right><b>$watchcount</b></td></tr>\n";
	echo "</table>\n";
?>
