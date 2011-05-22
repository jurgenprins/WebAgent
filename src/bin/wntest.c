#include <stdio.h>
#include <stdlib.h>
#include <wn.h>

void main (int argc, char *argv[]) {
	wn_db_t *db = wn_db_init();
	wn_fmt_t *fmt, *fmt_filtered;
	wn_fmt_tag_t *wn_fmt_tag;

	if (argc < 5) {
		printf ("usage: %s <config_file> <file> <did> <uid>\n", argv[0]);
		exit (1);
	}

	if (!wn_cfg_load (argv[1])) {
		fprintf (stderr, "%s\n", wn_get_error());
		exit (1);
	}

	if (!(fmt = wn_fmt_load (argv[2])) || !wn_db_connect(db)) {
		printf ("%s\n", wn_get_error());
		exit (1);
	}

	if (!(wn_db_query (db, 0, "select filter, keywords, bfilter from watch where did = %i and uid = %i", atoi (argv[3]), atoi (argv[4])))) {
		printf ("%s\n", wn_get_error());
		exit (1);
	}

	if (!wn_db_next_result (db, 0)) {
		printf ("no watch record found for did %s, uid %s\n", argv[3], argv[4]);
		exit (1);
	}

	printf ("filter = %i, keywords = %s, bfilter = %s\n\n", atoi (wn_db_value (db, 0, 0)), wn_db_value (db, 0, 1), wn_db_value (db, 0, 2)); 

	if (!(fmt_filtered = wn_fmt_filter (fmt, atoi (wn_db_value (db, 0, 0)), wn_db_value (db, 0, 1), wn_db_value (db, 0, 2)))) {
		printf("%s\n", wn_get_error());
		exit (1);
	}

	for (wn_fmt_tag = fmt_filtered->first; wn_fmt_tag; wn_fmt_tag = wn_fmt_tag->next) {
		if (wn_fmt_tag->type && (wn_fmt_tag->param || wn_fmt_tag->content))
			printf ( "%i %s %s %s\n", wn_fmt_tag->blocknr, wn_fmt_type2str (wn_fmt_tag->type), wn_fmt_tag->param ? wn_fmt_tag->param : WN_FMT_CHAR_NULL, wn_fmt_tag->content ? wn_fmt_tag->content : WN_FMT_CHAR_NULL);
	}		

	wn_db_free (db);
	wn_fmt_free (fmt_filtered);
	wn_fmt_free (fmt);
}
