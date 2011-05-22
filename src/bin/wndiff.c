/* The read/write test: outgoing should be same as ingoing.. */

#include <stdio.h>
#include <stdlib.h>
#include <wn.h>

void main (int argc, char *argv[]) {
	wn_fmt_t *fmt1, *fmt2, *fmtdiff;

	if (argc < 4) {
		printf ("usage: %s <file1> <file2> <out_file>\n", argv[0]);
		exit (0);
	}

	if (!(fmt1 = wn_fmt_load (argv[1]))) {
		printf ("%s\n", wn_get_error());
		exit (1);
	}

	if (!(fmt2 = wn_fmt_load (argv[2]))) {
		printf ("%s\n", wn_get_error());
		exit (1);
	}

	if (!(fmtdiff = wn_fmt_diff (fmt1, fmt2))) {
		printf ("%s\n", wn_get_error());
		exit (1);
	}

	wn_fmt_save (fmtdiff, argv[3]);

	wn_fmt_free (fmtdiff);
	wn_fmt_free (fmt2);
	wn_fmt_free (fmt1);
}
