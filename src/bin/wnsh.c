#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <wn.h>
#include <mysql.h>

#define MAX_SIZE 10240

void main (int argc, char *argv[]) {
	MYSQL mysql;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char str[MAX_SIZE];
	char sql[MAX_SIZE];
	time_t t;

	if (!wn_cfg_load ("/usr/local/wn/etc/wncontrol.easy")) {
		fprintf (stderr, "%s\n", wn_get_error());
		exit (1);
	}

	printf ("\n\nWelcome to WhatsNEW!\n\n");

#if MYSQL_VERSION_ID >= 32200
	if (mysql_init (&mysql) && mysql_real_connect (&mysql, NULL, cfg_DB_USER, cfg_
DB_PASS, cfg_DB_NAME, 0, NULL, 0)) {
#else
	if (mysql_connect (&mysql, NULL, cfg_DB_USER, cfg_DB_PASS) && !mysql_select_db
 (&mysql, cfg_DB_NAME)) {
#endif

	printf ("wn> ");
	while (fgets (str, MAX_SIZE - 1, stdin)) {
		strtok (str, "\r\n\t");
		if ((str[0] == 0) || (str[0] == '?')) {
			printf ("Type (part of) the URL of the website and I will tell you when it was changed\n");
		} else  {
			sprintf (sql, "select docs.id, docs.host, docs.uri, docs.changed from docs where docs.title like '%%%s%%' or docs.host like '%%%s%%'", str, str);

	    if (!mysql_query (&mysql, sql)) {
 	     if ((res = mysql_store_result (&mysql))) {
 	       while ((row = mysql_fetch_row (res)) && row[0] && row[1] && row[2]) {
						t = atoi (row[3]);
						strtok (strcpy (str, ctime (&t)), "\n\r");	
						printf ("%i\t%s\thttp://%s%s\n", atoi (row[0]), str, row[1], row[2]);
					}
				} else printf ("database error: %s\n", mysql_error (&mysql));
			} else printf ("database error: %s\n", mysql_error (&mysql));
		}

		printf ("\nwn> ");
	}

	} else printf ("The database is unavailable right now, try again later!\n\n");
}
