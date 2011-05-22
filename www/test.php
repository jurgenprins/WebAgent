<?
	include "db.php";

	$res = mysql_query ("select bfilter from watch where uid = 1 and did = 1", $conn);
	if (mysql_num_rows ($res)) {
		$bfilter = mysql_result ($res, 0, "bfilter");
		for ($i = 0; $i < strlen($bfilter); $i++) {
			echo ord($bfilter[$i]) . " ";
		}
	}

?>
