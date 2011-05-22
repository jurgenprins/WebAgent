#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/time.h>
#include <dirent.h>
#include <wn.h>

#ifndef MAXPATHLEN
#define MAXPATHLEN 128
#endif

long entries_pruned = 0;
long bytes_pruned = 0;
int period = 604800; // a week

wn_db_t *db;

void wn_prune_printsp (int level) {
	while (level--)
		printf ("  ");
}

void wn_prune_dir (char *path, int min_age, long min_bytes, int min_entries, int is_test, int level) {
	DIR *dir;
	struct dirent *dir_entry;
	struct stat dir_entry_info;
	int stamp, old_stamp = 0, diff_total = 0, diff_count = 0;
	int i, run, num_entries = 0;
	int extra_filter = ((min_entries > 0) || (min_bytes > 0));
	long num_bytes = 0;
	char *entry_path, *doc_id_str;
	char stamp_str[MAXPATHLEN];

	if (!(dir = opendir (path)))
		return;

	entry_path = (char *) alloca (MAXPATHLEN);
	doc_id_str = (char *) alloca (strlen (path));
	run = extra_filter ? 2 : 1;

	strcpy (doc_id_str, &path[strlen (path) - (level * 2)]);
	for (i = 0; doc_id_str[i]; i++) {
		if (doc_id_str[i] == '/') {
			_wn_str_shift (doc_id_str, i, 1);
			i--;
		}
	}

	if (is_test) {
		wn_prune_printsp (level);
		printf ("%s (%s) {\n", path, doc_id_str);
	}

	while (run) {
		rewinddir (dir);
		while ((dir_entry = readdir (dir))) {
			if (path[strlen (path) - 1] != '/')
				sprintf (entry_path, "%s/%s", path, dir_entry->d_name);
			else
				sprintf (entry_path, "%s%s", path, dir_entry->d_name);

			if ((stat (entry_path, &dir_entry_info))){
				wn_prune_printsp (level + 1);
				printf ("wn_prune_dir: could not get info for %s\n", entry_path);
				closedir (dir);
				return;
			}

			if (dir_entry_info.st_mode & S_IFDIR) {
				if ((run == 1) &&
				    strcmp (dir_entry->d_name, ".") &&
				    strcmp (dir_entry->d_name, "..")) {
					wn_prune_dir (entry_path, min_age, min_bytes, min_entries, is_test, level + 1);
				}
			} else if (dir_entry_info.st_mode & S_IFREG) {
				strcpy(stamp_str, dir_entry->d_name);
				if (strstr(stamp_str, "new"))
					stamp_str[strlen(stamp_str) - 4] = 0;
				if (stamp = atoi (stamp_str)) {
					if (run == 2) {
							num_entries++;
							num_bytes += dir_entry_info.st_size;
					} else {
						if (extra_filter) {
							num_entries--;
							num_bytes -= dir_entry_info.st_size;
						}

						if (stamp < min_age) {
							if (extra_filter) {
								if (num_bytes < min_bytes) {
									if (is_test) {
										wn_prune_printsp (level + 1);
										printf ("%s kept (only %ld bytes would be left)\n", dir_entry->d_name, num_bytes);
									}
									if (old_stamp && (old_stamp < stamp)) {
										diff_total += stamp - old_stamp;
										diff_count++;
									}
									old_stamp = stamp;
									continue;
								}

								if (num_entries < min_entries) {
									if (is_test) {
										wn_prune_printsp (level + 1);
										printf ("  %s kept (only %i entries would be left)\n", dir_entry->d_name, num_entries);
									}
									if (old_stamp && (old_stamp < stamp)) {
										diff_total += stamp - old_stamp;
										diff_count++;
									}
									old_stamp = stamp;
									continue;
								}
							}

							if (is_test) {
								wn_prune_printsp (level + 1);
								printf ("%s pruned\n", dir_entry->d_name);
							} else {
								unlink (entry_path);
							}
							entries_pruned++;
							bytes_pruned += dir_entry_info.st_size;
							continue;
						}

						if (old_stamp && (old_stamp < stamp)) {
							diff_total += stamp - old_stamp;
							diff_count++;
						}
						old_stamp = stamp;
					}
				}
			}
		}
		run--;
	}

	if (is_test) {
		wn_prune_printsp (level);
		printf ("} %i\n", diff_count ? (diff_total / diff_count) : 0);
	}

	if (atoi (doc_id_str))
		wn_db_query (db, 0, "update docs set change_freq = %i where id = %i", (diff_count && (diff_total / diff_count)) ? (period / (diff_total / diff_count)) : 0, atoi (doc_id_str));

	closedir (dir);
}

void main (int argc, char *argv[]) {
	time_t t;
	long min_bytes = 0;
	int min_entries = 0;

	if (argc < 3) {
		fprintf (stderr, "usage: %s <config_file> <max_days> [<min_size> [<min_entries [<freq_days> [test]]]]\n", argv[0]);
		exit (1);
	}

	if (!wn_cfg_load (argv[1])) {
		fprintf (stderr, "%s\n", wn_get_error());
		exit (1);
	}

	if (chdir (cfg_DATA_DIR)) {
		fprintf (stderr, "directory %s could not be opened\n", argv[1]);
		exit (2);
	}

	if (!(db = wn_db_init()) || !(wn_db_connect (db))) {
		fprintf (stderr, "%s\n", wn_get_error());
		exit (1);
	}

	time (&t);
	if (atoi (argv[2]) >= 0)
		t -= (atoi (argv[2]) * 86400);


	if ((argc >= 4) && argv[3])
		min_bytes = atoi (argv[3]) * 1000;

	if ((argc >= 5) && argv[4])
		min_entries = atoi (argv[4]);

	if (!atoi (argv[2]) && !min_bytes && !min_entries) {
		printf (".. this would delete ALL files!\nif you really intend to do this, use the regular system commands\n");
		exit (1);
	}

	if ((argc >= 6) && argv[5] && atoi (argv[5]))
		period = 86400 * atoi (argv[5]);

	if (argc >= 7)
		printf ("wnprune running in test mode (verbose messages, do nothing)\n");

	wn_prune_dir (cfg_DATA_DIR, t, (min_bytes > 0) ? min_bytes : 0, (min_entries > 0) ? min_entries : 0, (argc >= 7) ? 1 : 0, 0);

	wn_db_free (db);

	time (&t);
	printf ("%ld files (%ld bytes) %s pruned %s\n", entries_pruned, bytes_pruned, (argc >= 6) ? "would be" : "are", ctime (&t));
}
