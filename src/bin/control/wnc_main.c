#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <mysql.h>
#include <wn.h>
#include "wnc_doc.h"

#define WE_URL					200
#define MAX_SIZE				10240
#define WNC_MAILER_GO	 	-1
#define WNC_MAILER_IDLE	0

#define SIGHUP_NOT_OK

wn_db_t *db;
struct stat stat_buf;
FILE *log_file;
time_t t;
int sig_abort = 0;
int sig_reinit = 0;
int mailer_status = WNC_MAILER_IDLE;

wn_fmt_t *_wn_fmt_load (char *fname, int max_lines);

wnc_doc_t *wnc_doc_get_next () {
	wnc_doc_t *doc;
	char path[256];
	char sql[10240];
	int i;

	time (&t);

	strcpy (sql, "select id, host, uri, changed_serv from docs where 1=1");
	for (doc = wnc_doc_first(); doc; doc = doc->next) {
		if (doc->url)
			sprintf (sql, "%s and (id <> %i) ", sql, doc->url->id);
	}
	sprintf (sql, "%s and updated < %li order by updated limit 1", sql, t - cfg_DOC_MIN_AGE);

	if (!wn_db_query (db, 0, sql))
		return NULL;

	if (!wn_db_next_result (db, 0) || !wn_db_value (db, 0, 0)) {
		wn_report_error (WE_URL, "wnc_doc_get_next: no (more) URLs to fetch");
		return NULL;
	}

	if (chdir (cfg_DATA_DIR)) {
		wn_report_error (WE_FILE, "wnc_doc_get_next: could not cd to DATA_DIR: %s", strerror (errno));
		return NULL;
	}

	strcpy (path, cfg_DATA_DIR);

	sprintf (sql, "%i", atoi (wn_db_value (db, 0, 0)));
	for (i = 0; i < strlen (sql); i++) {
		sprintf (path, "%s/%c", path, sql[i]);
		if (chdir (path)) {
			if (mkdir (path, 0755) || chdir (path)) {
				wn_report_error (WE_FILE, "wn_doc_get_next: %s", strerror (errno));
				return NULL;
			}
		}
	}

	time (&t);
	sprintf (path, "%s/%li.new", path, (long)t);

	if (!(doc = wnc_doc_init (atoi (wn_db_value (db, 0, 0)), wn_db_value (db, 0, 1), wn_db_value (db, 0, 2), path, wn_db_value (db, 0, 3)))) {
		wn_report_error (WE_MEM, "wn_doc_get_next: could not initialise new document");
		return NULL;
	}

	return doc;
}

void wnc_process (wnc_doc_t *doc, wn_tmpl_t *tmpl_prio) {
	DIR *dir;
	wn_db_t *db2 = NULL;

	struct dirent *dir_entry;
	wn_fmt_t *fmt, *fmtold, *fmtdiff, *fmtfiltered;
	wn_tmpl_t *tmpl;
	char str[MAX_SIZE];
	char *body = NULL, *tmp, *email, *title = NULL, *url = NULL;
	int i, found = 0, blocks = 0;

	if (!(fmt = wn_fmt_fetch (doc->url, cfg_DOC_TIMEOUT, 1))) {
		sprintf (str, "%i", wn_get_errno());
		fmt = wn_fmt_init();
		wn_fmt_add (fmt, WN_FMT_ERROR, str, wn_get_error());
		wn_fmt_save (fmt, doc->path);
		wn_fmt_free (fmt);
		exit (0);
	}

	if ((doc->url->title && strlen (doc->url->title)) ||
	    (doc->url->last_modified && strlen (doc->url->last_modified))) {
		if ((db2 = wn_db_init()) && (wn_db_connect (db2))) {
			if (doc->url->title && strlen (doc->url->title)) {
				if (doc->url->last_modified && strlen (doc->url->last_modified))
					wn_db_query (db2, 0, "update docs set title = '%s', changed_serv = '%s' where id = %i", wn_str_escape (&(doc->url->title)), doc->url->last_modified, doc->url->id);
				else
					wn_db_query (db2, 0, "update docs set title = '%s' where id = %i", wn_str_escape (&(doc->url->title)), doc->url->id);
			} else
				wn_db_query (db2, 0, "update docs set changed_serv = '%s' where id = %i", doc->url->last_modified, doc->url->id);
		}
	}

	strcpy (str, doc->path);
	for (i = strlen (str) - 1; i > 0; i--)
		if (str[i] == '/') {
			str[i] = 0;
			break;
		}

	if ((dir = opendir (str)) == NULL) {
		wn_db_free (db2);
		exit (2);
	}

	while ((dir_entry = readdir (dir))) {
		if (strstr (dir_entry->d_name, "new")) {
			sprintf (str, "%s/%s", str, dir_entry->d_name);
			found = 1;
			break;
		}
	}
	closedir (dir);

	if (!found) {
		wn_fmt_save (fmt, doc->path);
		wn_fmt_free (fmt);
		exit (0);
	}

	if ((fmtold = wn_fmt_load (str)) &&
	    (fmtdiff = wn_fmt_diff (fmtold, fmt))) {

		unlink (str);
		wn_fmt_save (fmt, doc->path);
		strcpy (str, doc->path);
		str[strlen (str) - 4] = 0;
		if (fmtdiff->size) {
			wn_fmt_save (fmtdiff, str);

			if ((db2 || (db2 = wn_db_init())) && (wn_db_connect (db2))) {
				if (wn_db_query (db2, 0, "select title from docs where id = %i", doc->url->id)) {
					if (wn_db_next_result (db2, 0) && wn_db_value (db2, 0, 0))
						title = strdup (wn_str_html2latin (wn_db_value (db2, 0, 0)));
				}
				if (!title)
					title = strdup ("<unknown title>");

				if (wn_db_query (db2, 0, "select shw_host, shw_uri from docs_banned where alt_host like '%%%s%%' and (alt_uri = '' or alt_uri = '%s')", doc->url->host, doc->url->uri) && wn_db_next_result (db2, 0) && wn_db_value (db2, 0, 0) && wn_db_value (db2, 0, 1)) {
					url = (char *) malloc (strlen (wn_db_value (db2, 0, 0)) + strlen (wn_db_value (db2, 0, 1)) + 10);
					sprintf (url, "http://%s%s", wn_db_value (db2, 0, 0), wn_db_value (db2, 0, 1));
				} else {
					url = (char *) malloc (strlen (doc->url->host) + strlen (doc->url->uri) + 10);
					sprintf (url, "http://%s%s", doc->url->host, doc->url->uri);
				}

				if (wn_db_query (db2, 0, "select uid, ubase, keywords, filter, title, bfilter, tmpl from watch where did = %i and email_prio = 1", doc->url->id)) {
					while (wn_db_next_result (db2, 0)) {
						body = NULL;
						tmpl = NULL;

						if (wn_db_value (db2, 0, 6) && (atoi (wn_db_value (db2, 0, 6)) > 0)) {
							sprintf (str, "%s.%i", cfg_TEMPLATE_UPDATED, atoi (wn_db_value (db2, 0, 6)));
							if (!(tmpl = wn_tmpl_init (str)))
								fprintf (log_file, "wnc_process: warning at template init: %s\n", wn_get_error());
						}

						if (!tmpl) {
							if (tmpl_prio && tmpl_prio->defs && (tmpl = wn_tmpl_init (NULL)))
								tmpl->defs = tmpl_prio->defs;
							else {
								fprintf (log_file, "wnc_process: template init: %s\n", wn_get_error());
								continue;
							}
						}

						wn_tmpl_set (tmpl, "URL", url);
						sprintf (str, "%i", doc->url->id);
						wn_tmpl_set (tmpl, "DID", str);

						if (wn_db_value (db2, 0, 4) && (wn_db_value (db2, 0, 4)[0] != 0) && (wn_db_value(db2, 0, 4)[0] != 32) && strlen (wn_db_value (db2, 0, 4)))
							wn_tmpl_set (tmpl, "TITLE", wn_str_html2latin (wn_db_value (db2, 0, 4)));
						else
							wn_tmpl_set (tmpl, "TITLE", title);

						if ((tmp = wn_tmpl_get (tmpl, "subject"))) {
							sprintf (str, tmp);
							free (tmp);
						} else
							sprintf (str, "WhatsNEW Update Notification");
						email = wn_db_email_by_uid (db2, atoi (wn_db_value (db2, 0, 0)), wn_db_value (db2, 0, 1));
						if (email && strlen (email)) {
							if (wn_str_cat (&body, tmp = wn_tmpl_get (tmpl, "header"), &blocks))
								free (tmp);

							fmtfiltered = NULL;
							wn_discard_error ();
							if (wn_db_value (db2, 0, 2) && strlen (wn_db_value (db2, 0, 2))) {
								if (wn_db_value (db2, 0, 3) && atoi (wn_db_value (db2, 0, 3)))
									fmtfiltered = wn_fmt_filter (fmtdiff, atoi (wn_db_value (db2, 0, 3)), wn_db_value (db2, 0, 2), wn_db_value (db2, 0, 5));
								else
									fmtfiltered = wn_fmt_filter (fmtdiff, WN_FMT_ALL, wn_db_value (db2, 0, 2), wn_db_value (db2, 0, 5));
							} else if (wn_db_value (db2, 0, 3) && atoi (wn_db_value (db2, 0, 3)))
								fmtfiltered = wn_fmt_filter (fmtdiff, atoi (wn_db_value (db2, 0, 3)), NULL, wn_db_value (db2, 0, 5));

							if (fmtfiltered) {
								if (fmtfiltered->size && wn_str_cat (&body, tmp = wn_mail_fmt (tmpl, fmtfiltered), &blocks)) {
									free (fmtfiltered);
									free (tmp);
								} else {
									free (fmtfiltered);
									if (body) free (body);
									fprintf (log_file, "wnc_process: filtered email for %s\n", email);
									continue;
								}
							} else {
								if (wn_get_errno() != WE_NONE) {
									if (body) free (body);
									fprintf (log_file, "wnc_process: filter not determined for %s\n", email);
									continue;
								} else {
									if (wn_str_cat (&body, tmp = wn_mail_fmt (tmpl, fmtdiff), &blocks))
										free (tmp);
								}
							}

							if (wn_str_cat (&body, tmp = wn_tmpl_get (tmpl, "footer"), &blocks))
								free (tmp);

							if (body) {
								if (!wn_mail_spool (cfg_MAIL_SPOOL, email, str, body))
									fprintf (log_file, "wnc_process: while spooling (%s)\n", wn_get_error());
								else
									fprintf (log_file, "wnc_process: mail to %s spooled\n", email);
								free (body);
							} else
								fprintf (log_file, "wnc_process: no body in mail for %s\n", email);
							free (email);
						} else fprintf (log_file, "wnc_process: email could not be determined (%s)\n", wn_get_error());
						wn_tmpl_free (tmpl);
					}
				} else fprintf (log_file, "wnc_process: subscriber list failed: %s\n", wn_get_error());
				free (url);
				free (title);
			} else fprintf (log_file, "wnc_process: db connect failed: %s\n", wn_get_error());
		}

		fflush (log_file);
		wn_fmt_free (fmtdiff);
		wn_fmt_free (fmtold);
	} else {
		wn_fmt_save (fmt, doc->path);
		wn_fmt_free (fmt);
		exit (0);
	}

	fflush (log_file);

	wn_db_free (db2);
	wn_fmt_free (fmt);
	exit (0);
}

void wnc_doc_done (wnc_doc_t *doc, int changed) {
	time_t t;
	int status = wn_get_errno();
	int last_status = 0;

	time (&t);

	if (cfg_LOG_LEVEL >= 2) {
		fprintf (log_file, "%s%s <- %i (%i%s%s)\n", doc->url->host, doc->url->uri, changed, status, (status != 0) ? " - " : "", (status != 0) ? wn_get_error() : "");
		fflush (log_file);
	}

	wn_discard_error();

	if ((status == WE_NONE) || (status == WE_SKIP))
		wn_db_query (db, 0, "delete from errors where did = %i", doc->url->id);

	if (status != WE_NONE) {
		unlink (doc->path);

		if ((status != WE_SKIP) && (status != WE_RELOC)) {
			if (wn_db_query (db, 0, "select status from docs where id = %i", doc->url->id)) {
				if (wn_db_next_result (db, 0) && wn_db_value (db, 0, 0))
					last_status = atoi (wn_db_value (db, 0, 0));
			}

			if (last_status == status) {
				if (wn_db_query (db, 0, "select count from errors where did = %i and status = %i", doc->url->id, status)) {
					if (wn_db_next_result (db, 0) && wn_db_value (db, 0, 0))
						wn_db_query (db, 0, "update errors set count = %i where did = %i and status = %i", atoi (wn_db_value (db, 0, 0)) + 1, doc->url->id, status);
					else
						wn_db_query (db, 0, "insert into errors (did, status, count) values (%i, %i, 1)", doc->url->id, status);
				}
			}
		}
	}

	if (changed) {
		if (mailer_status == WNC_MAILER_IDLE)
			mailer_status = WNC_MAILER_GO;

		wn_db_query (db, 0, "update docs set updated = %d, status = %i, changed = %d where id = %i", t, status, t, doc->url->id);
	} else
		wn_db_query (db, 0, "update docs set updated = %d, status = %i where id = %i", t, status, doc->url->id);

	if (wn_get_errno() && (cfg_LOG_LEVEL >= 1)) {
		fprintf (log_file, "%s\n", wn_get_error());
		fflush (log_file);
	}

	if (doc->pid)  {		/* make sure the child will vanish */
		kill (doc->pid, SIGKILL);
		waitpid (doc->pid, NULL, 0);
	}

	wnc_doc_delete (doc->url->id);
}

int wnc_monitor() {
	wnc_doc_t *doc;
	wn_fmt_t *fmt;
	time_t now;
	char *str = NULL;

	time (&now);
	chdir (cfg_DATA_DIR);
	wn_discard_error();

	for (doc = wnc_doc_first(); doc; doc = doc->next) {
		if (!stat (doc->path, &stat_buf)) {
			if ((fmt = _wn_fmt_load (doc->path, 1))) {
				if (fmt->first) {
					if (fmt->first->type == WN_FMT_ERROR) {
						if (fmt->first->param && atoi (fmt->first->param)) {
							wn_report_error (atoi (fmt->first->param), fmt->first->content ? fmt->first->content : "unknown");
							wnc_doc_done (doc, 0);
						} else {
							wn_report_error (WE_UNKNOWN, "wnc_monitor: an error has occurred, but it is unknown");
							wnc_doc_done (doc, 0);
						}
					} else {
						str = strdup (doc->path);
						str[strlen (str) - 4] = 0;
						wnc_doc_done (doc, stat (str, &stat_buf) ? 0 : 1);
						free (str);
					}
				} else {
					wn_report_error (WE_EMPTY, "wnc_monitor: retrieved file is empty");
					wnc_doc_done (doc, 0);
				}
			} else
				wnc_doc_done (doc, 0);
			if (!(doc = wnc_doc_first())) break;
		} else {
			if (doc->start < (now - cfg_DOC_MAX_WAIT)) {
				wn_report_error (WE_TIMEOUT, "wnc_monitor: maximum execution time of child reached", doc->url->host, doc->url->uri);
				wnc_doc_done (doc, 0);
				if (!(doc = wnc_doc_first())) break;
			}
		}
	}

	return wnc_doc_count();
}

int wnc_license (char *pwdfile) {
	char str[10];
	FILE *in = NULL;

	wn_url_t *url = wn_url_init (0, "www.devq.net", "/wn.html", NULL);

	fprintf (stderr, "verifying run permissions.. ");
	fflush (stderr);

	if (pwdfile)
		in = fopen (pwdfile, "r");

	if (!url)
		return 0;

	if (!wn_net_fetch (url, 15)) {
		wn_url_free (url);

		if ((wn_get_errno() != WE_UNKNOWN) &&
		    (in = fopen (pwdfile, "r"))) {
			if (fgets (str, 9, in)) {
				if ((strlen (str) != 4) ||
				    (str[0] != 'w') ||
				    (str[1] != 'n') ||
				    (str[2] != 'z')) {
					fprintf (stderr, "and no valid pwd in %s, sorry\n", pwdfile);
					exit (1);
				} else {
					fprintf (stderr, "ok\n");
					return 1;
				}
			} else {
				fprintf (stderr, "and no pwd in %s, sorry\n", pwdfile);
				exit (1);
			}
		}
		return 0;
	}

	if (!strstr (url->data, "yes")) {
		wn_report_error (WE_UNKNOWN, "no license found, sorry\n");
		wn_url_free (url);
		return 0;
	}

	fprintf (stderr, "ok\n");

	wn_url_free (url);
	return 1;
}

void wnc_reinit (int signal) {
	sig_reinit = 1;
	time (&t);
	fprintf (log_file, "** REINIT %s", ctime (&t));
#ifdef SIGHUP_NOT_OK
	fprintf (log_file, "**** THIS ACTION PROBABLY JUST LOCKED UP THE PROGRAM!\n**** IF NO MORE LOG ACTIVITY BELOW THIS POINT, KILL & RESTART..\n");
#endif
	fflush (log_file);
}

void wnc_abort (int signal) {
	sig_abort = 1;
	time (&t);
	if (signal != SIGSEGV) {
		fprintf (log_file, "** FINISHING (%i, %i waiting) %s", signal, wnc_doc_count(), ctime (&t));
		fflush (stdout);
	} else {
		fprintf (log_file, "** SEGMENTATION FAULT %s", ctime (&t));
		fclose (log_file);
		wn_db_free (db);
		unlink (cfg_PID_FILE);
		exit (0);
	}
}

int main (int argc, char *argv[]) {
	pid_t pid;
	wn_tmpl_t *tmpl;
	wnc_doc_t *doc;
	int num_childs = 0, child_status = 0;

	if (argc < 2) {
		fprintf (stderr, "usage: %s <config_file>\n", argv[0]);
		exit (1);
	}

	fprintf (stderr, "WhatsNEW Controller 1.0    (c) 1999 - 2000  JP\n\n");

	if (!wnc_license((argc > 2) ? argv[2] : NULL)) {
		fprintf (stderr, "%s\n", wn_get_error());
		exit (1);
	}

	fprintf (stderr, "running... \n");
	fflush (stderr);

	switch (fork()) {
	case -1:
		fprintf (stderr, "couldn't go background (%s)\n", strerror (errno));
		exit (errno);
	case 0:
		break;
	default:
		exit (0);
	}

	if (!wn_cfg_load (argv[1])) {
		fprintf (stderr, "%s\n", wn_get_error());
		exit (1);
	}

	if (cfg_DATA_DIR[0] != '/') {
		fprintf (stderr, "absolute path required for DATA_DIR (%s)\n", cfg_DATA_DIR);
		exit (1);
	}

	if (chdir (cfg_MAIL_SPOOL)) {
		fprintf (stderr, "unable to access spool directory %s\n", cfg_MAIL_SPOOL);
		exit (2);
	}

	if (chdir (cfg_DATA_DIR)) {
		fprintf (stderr, "unable to access data directory %s\n", cfg_DATA_DIR);
		exit (2);
	}

	if (!(db = wn_db_init ())) {
		fprintf (stderr, "%s\n", wn_get_error ());
		exit (2);
	}

	if (!(log_file = fopen (cfg_PID_FILE, "w"))) {
		fprintf (stderr, "unable to open pidfile %s\n", cfg_PID_FILE);
		exit (2);
	}

	fprintf (log_file, "%i", (int)getpid());
	fclose (log_file);

	if (!(log_file = fopen (cfg_LOG_FILE, cfg_LOG_APPEND ? "a" : "w"))) {
		fprintf (stderr, "unable to open logfile %s\n", cfg_LOG_FILE);
		exit (2);
	}

	if (!wn_db_init()) {
		fprintf (stderr, "%s\n", wn_get_error());
		exit (1);
	}

	if (!(tmpl = wn_tmpl_init (cfg_TEMPLATE_UPDATED))) {
		fprintf (stderr, "%s\n", wn_get_error());
		exit (1);
	}

	if (cfg_TIME_SLICE < 1)
		cfg_TIME_SLICE = 1;
	if (cfg_DB_MAX_RETRIES < 1)
		cfg_DB_MAX_RETRIES = 1;
	if (cfg_DOC_MAX_PARALLEL < 1)
		cfg_DOC_MAX_PARALLEL = 1;

#ifdef SIGHUP_NOT_OK
	signal (SIGHUP, SIG_IGN);
#else
	signal (SIGHUP, wnc_reinit);
#endif
	signal (SIGINT, SIG_IGN);
	signal (SIGQUIT, wnc_abort);
	signal (SIGABRT, wnc_abort);
	signal (SIGTERM, wnc_abort);
	signal (SIGSEGV, wnc_abort);
	signal (SIGBUS, wnc_abort);

	time (&t);
	fprintf (log_file, "** STARTUP %s", ctime (&t));
	fflush (log_file);

	while (1) {
		if (sig_reinit) {		/* sighup received */
			sig_reinit = 0;
			if (!wn_cfg_reload ()) {
				fprintf (log_file, "** FATAL: REINIT failed.. %s", ctime (&t));
				break;
			}
		}

		if ((num_childs = wnc_monitor()) < cfg_DOC_MAX_PARALLEL) {
			if (sig_abort) {		/* graceful shutdown */
				if (!num_childs)
					break;
				else
					cfg_TIME_SLICE = 1;		/* hurry up */
			} else {
				if ((doc = wnc_doc_get_next ())) {
					switch (pid = fork ()) {
					case -1:
						if (cfg_LOG_LEVEL >= 1) {
							fprintf (log_file, "wncontrol: %s\n", strerror (errno));
							fflush (log_file);
						}
						break;
					case 0:
						wnc_process (doc, tmpl);
						exit (0);
					default:
						doc->pid = pid;
						if (cfg_LOG_LEVEL >= 2) {
							fprintf (log_file, "%s%s -> %s\n", doc->url->host, doc->url->uri, doc->path);
							fflush (log_file);
						}
						break;
					}
				} else {
					if ((wn_get_errno() != WE_URL) && (cfg_LOG_LEVEL >= 1)) {
						fprintf (log_file, "wncontrol: %s\n", wn_get_error());
						fflush (log_file);
					}
				}
			}
		}

		if (mailer_status != WNC_MAILER_IDLE) {
			if (mailer_status == WNC_MAILER_GO) {
				switch (mailer_status = fork ()) {
				case -1:
					if (cfg_LOG_LEVEL >= 1) {
						fprintf (log_file, "wncontrol: %s\n", strerror (errno));
						fflush (log_file);
					}
					mailer_status = WNC_MAILER_IDLE;
					break;
				case 0:
					if (!wn_mail_open (cfg_MAIL_HOST, NULL, cfg_MAIL_TIMEOUT) || !wn_mail_process (cfg_MAIL_SPOOL, cfg_MAIL_SENDER, cfg_MAIL_FROM, cfg_MAIL_CHUNK, cfg_MAIL_DELAY) || !wn_mail_close())
						exit (wn_get_errno());
					else
						exit (0);
				default:
					fprintf (log_file, "wncontrol: mailer started\n");
					fflush (log_file);
					break;
				}
			} else {
				switch (waitpid (mailer_status, &child_status, WNOHANG)) {
				case -1:
					if (WEXITSTATUS(child_status) && (cfg_LOG_LEVEL >= 1)) {
						fprintf (log_file, "wncontrol: while processing mail: %s\n", wn_get_errstr (WEXITSTATUS(child_status)));
						fflush (log_file);
					}
					mailer_status = WNC_MAILER_IDLE;
				case 0:
					break;
				default:
					fprintf (log_file, "wncontrol: mailer exited\n");
					fflush (log_file);
					mailer_status = WNC_MAILER_IDLE;
				}
			}
		}

		sleep (cfg_TIME_SLICE);
	}

	time (&t);
	fprintf (log_file, "** SHUTDOWN %s", ctime (&t));

	fclose (log_file);
	wn_db_free (db);
	wn_tmpl_free (tmpl);
	unlink (cfg_PID_FILE);

	exit (0);
}
