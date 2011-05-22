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

void main (int argc, char *argv[]) {
	DIR *dir;
	struct dirent *dir_entry;
	struct stat dir_entry_info;
	char str[1024], path[1024];
	char *tmp, *email;
	int i, count = 0, did = 0;
	wn_db_t *db;
	wn_tmpl_t *tmpl;
	time_t t;

	if (argc < 3) {
		fprintf (stderr, "usage: %s <config_file> <num_errors> [<id>]\n", argv[0]);
		exit (0);
	}

	if ((count = atoi (argv[2])) < 0) 
		count = 0;

	if (!wn_cfg_load (argv[1])) {
		fprintf (stderr, "%s\n", wn_get_error());
		exit (1);
	}

	if (!(db = wn_db_init()) || !(wn_db_connect (db))) {
		fprintf (stderr, "%s\n", wn_get_error());
		exit (1);
	}

	if (!(tmpl = wn_tmpl_init (cfg_TEMPLATE_REMOVED))) {
		fprintf (stderr, "%s\n", wn_get_error());
		exit (1);
	}

	time (&t);
	t -= (atoi (argv[2]) * 3600);

	if (wn_db_query (db, 0, "select id from docs where watchers = 0")) {
		while (wn_db_next_result (db, 0) && (did = atoi (wn_db_value (db, 0, 0)))) {
 			if (wn_db_query (db, 1, "select count from errors where did = %i", did)) {
				if (wn_db_next_result (db, 1) && wn_db_value (db, 1, 0))
					wn_db_query (db, 1, "update errors set count = %i, status = %i where did = %i", count + 1, WE_ORPHAN, did);
				else
					wn_db_query (db, 1, "insert into errors (did, status, count) values (%i, %i, %i)", did, WE_ORPHAN, count + 1);
			}
		}
	}

	did = 0;
	if (argc >= 4) 
		did = atoi (argv[3]);

	if ((count == 0) && (did > 0))
		sprintf (str, "select docs.id, docs.title, docs.host, docs.uri, %i from docs where docs.id = %i", WE_CLEAN, did);
	else {
		sprintf (str, "select docs.id, docs.title, docs.host, docs.uri, errors.status from docs, errors where docs.id = errors.did and errors.count >= %i", count);
		if (did > 0) 
			sprintf (str, "%s and errors.did = %i", str, did);
	}			

	if (wn_db_query (db, 0, str)) {
		while (wn_db_next_result (db, 0) && (did = atoi (wn_db_value (db, 0, 0)))) {
			sprintf (str, "%i", did);
			strcpy (path, cfg_DATA_DIR);

			for (i = 0; i < strlen (str); i++)
				sprintf (path, "%s/%c", path, str[i]);

			if ((dir = opendir (path))) {
				while ((dir_entry = readdir (dir))) {
					sprintf (str, "%s/%s", path, dir_entry->d_name);

					if ((!(stat (str, &dir_entry_info))) &&
			 		   (dir_entry_info.st_mode & S_IFREG)) 
						unlink (str);
				}

				closedir (dir);
				rmdir (path);
			}

			if (wn_db_query (db, 1, "select uid, ubase from watch where did = %i", did)) {
				wn_tmpl_set (tmpl, "TITLE", wn_db_value (db, 0, 1));
				sprintf (str, "http://%s%s", wn_db_value (db, 0, 2), wn_db_value (db, 0, 3));
				wn_tmpl_set (tmpl, "URL", str);
				wn_tmpl_set (tmpl, "REASON", wn_get_errstr (atoi (wn_db_value (db, 0, 4))));

				if ((tmp = wn_tmpl_get (tmpl, "subject"))) {
					strcpy (str, tmp);
					free (tmp);
				} else
					strcpy (str, "WhatsNEW Remove Notification");

				if ((tmp = wn_tmpl_get (tmpl, "body"))) {
					while (wn_db_next_result (db, 1) && wn_db_value (db, 1, 0)) {
						if ((email = wn_db_email_by_uid (db, atoi (wn_db_value (db, 1, 0)), wn_db_value (db, 1, 1)))) {
							wn_mail_spool (cfg_MAIL_SPOOL, email, str, tmp);
							free (email);
						} else printf ("email: %s\n", wn_get_error());
					}
					free (tmp);
				}

				if (wn_db_query (db, 1, "select cid from docs_cats where did = %i", did)) {
					if (wn_db_next_result (db, 1) && wn_db_value (db, 1, 0) && atoi (wn_db_value (db, 1, 0))) 
						if (!wn_db_query (db, 1, "update cats set size = size - 1 where id = %i", atoi (wn_db_value (db, 1, 0))))
							printf ("cats: %s\n", wn_get_error());
				} else printf ("cid: %s\n", wn_get_error()); 

				if (wn_db_query (db, 1, "delete from docs_cats where did = %i", did)) {
					if (wn_db_query (db, 1, "delete from watch where did = %i", did)) {
						if (wn_db_query (db, 1, "delete from errors where did = %i", did)) {
							if (wn_db_query (db, 1, "delete from docs where id = %i", did)) {
								printf ("http://%s%s deleted, reason: %s\n", wn_db_value (db, 0, 2), wn_db_value (db, 0, 3), wn_get_errstr (atoi (wn_db_value (db, 0, 4))));
							}
						} else printf ("docs: %s\n", wn_get_error()); 
					} else printf ("watch: %s\n", wn_get_error()); 
				} else printf ("docs_cats: %s\n", wn_get_error()); 
			} else printf ("users: %s\n", wn_get_error());  
		} 
	}	

	wn_db_free (db);
}
