#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/time.h>
#include <dirent.h>
#include <mysql.h>
#include <wn.h>
#include "wnl_letter.h"

#define BLOCK_SIZE 	32768

void main (int argc, char *argv[]) {
	DIR *dir;
	struct dirent *dir_entry;
	struct stat dir_entry_info;
	char str[1024], path[1024];
	char *text = NULL, *tmp = NULL;
	int i, stamp, blocks = 0;
	time_t t;
	wn_fmt_t *fmt, *fmt_filtered;
	wn_db_t *db, *db2;
	wn_tmpl_t *tmpl;
	wnl_letter_t *letter;
	wnl_body_t *body;

	if (argc < 3) {
		fprintf (stderr, "usage: %s <config_file> <time_span_hours>\n", argv[0]);
		exit (0);
	}

	if (!wn_cfg_load (argv[1])) {
		fprintf (stderr, "%s\n", wn_get_error());
		exit (1);
	}

	if (!(db = wn_db_init()) || !(wn_db_connect (db)) ||
	    !(db2 = wn_db_init()) || !(wn_db_connect (db2))) {
		fprintf (stderr, "%s\n", wn_get_error());
		exit (1);
	}

	if (!(tmpl = wn_tmpl_init (cfg_TEMPLATE_LETTER))) {
		fprintf (stderr, "%s\n", wn_get_error());
		exit (1);
	}

	time (&t);
	t -= (atoi (argv[2]) * 3600);

	sprintf (str, "select users.id, docs.id, docs.title, docs.host, docs.uri, watch.filter, watch.ubase, watch.keywords, watch.bfilter, watch.title from users, watch, docs where users.id = watch.uid and watch.letter_update = %i and watch.did = docs.id and docs.changed > %li", atoi (argv[2]), (long)t);

	if (wn_db_query (db, 0, str)) {
		while (wn_db_next_result (db, 0)) {
			if (!(letter = wnl_letter_init (atoi (wn_db_value (db, 0, 0)), wn_db_email_by_uid (db, atoi (wn_db_value (db, 0, 0)), wn_db_value (db, 0, 6)))))
				printf ("%s\n", wn_get_error());

			if (wn_db_query (db2, 0, "select shw_host, shw_uri from docs_banned where alt_host like '%%%s%%' and (alt_uri = '' or alt_uri = '%s')", wn_db_value (db, 0, 3), wn_db_value (db, 0, 4)) && wn_db_next_result (db2, 0) && wn_db_value (db2, 0, 0) && wn_db_value (db2, 0, 1))
				sprintf (str, "http://%s%s", wn_db_value (db2, 0, 0), wn_db_value (db2, 0, 1));
			else
				sprintf (str, "http://%s%s", wn_db_value (db, 0, 3), wn_db_value (db, 0, 4));

			if (!(wnl_letter_add (letter, atoi (wn_db_value (db, 0, 1)), (wn_db_value (db, 0, 9) && strlen (wn_db_value (db, 0, 9))) ? wn_db_value (db, 0, 9) : wn_db_value (db, 0, 2), str, atoi (wn_db_value (db, 0, 5)), wn_db_value (db, 0, 7), wn_db_value (db, 0, 8))))
				printf ("%s\n", wn_get_error());
		}
	}	
			

	for (letter = wnl_letter_first (); letter; letter = letter->next) {
		if (wn_str_cat (&text, tmp = wn_tmpl_get (tmpl, "header"), &blocks))
			free (tmp);

		for (body = letter->first_body; body; body = body->next) {
			sprintf (str, "%i", body->did);
			strcpy (path, cfg_DATA_DIR);
			for (i = 0; i < strlen (str); i++) 
				sprintf (path, "%s/%c", path, str[i]);

			if ((dir = opendir (path))) {
				wn_tmpl_set (tmpl, "TITLE", wn_str_html2latin (body->title));
				wn_tmpl_set (tmpl, "URL", body->url);

				if (wn_str_cat (&text, tmp = wn_tmpl_get (tmpl, "docheader"), &blocks))
					free (tmp);

				while ((dir_entry = readdir (dir))) {
					sprintf (str, "%s/%s", path, dir_entry->d_name);

					if ((!(stat (str, &dir_entry_info))) &&
					    (dir_entry_info.st_mode & S_IFREG) &&
					    (!strstr (dir_entry->d_name, "new") &&
					    (stamp = atoi (dir_entry->d_name))) &&
					    (stamp >= t)) {
						if (((fmt = wn_fmt_load (str))) && (fmt_filtered = wn_fmt_filter (fmt, body->filter, body->words, body->bfilter)) && fmt_filtered->size) {
							wn_tmpl_set (tmpl, "DATE", ctime ((time_t *)&stamp));

							if (wn_str_cat (&text, tmp = wn_tmpl_get (tmpl, "itemheader"), &blocks))
								free (tmp);

							if (wn_str_cat (&text, tmp = wn_mail_fmt (tmpl, fmt_filtered), &blocks))
								free (tmp);

							wn_fmt_free (fmt_filtered);
							wn_fmt_free (fmt);
						}
					}
				}

				closedir (dir);
			}
		}

		if (wn_str_cat (&text, tmp = wn_tmpl_get (tmpl, "footer"), &blocks))
			free (tmp);

		if (wn_mail_spool (cfg_MAIL_SPOOL, letter->email, tmp = wn_tmpl_get (tmpl, "subject"), text)) {
			free (tmp);
			printf ("mail sent to %s (%i bytes)\n", letter->email, strlen (text));
		}

		free (text);
		text = NULL;
		blocks = 0;
	}

	wn_db_free (db);
	wn_db_free (db2);
}
