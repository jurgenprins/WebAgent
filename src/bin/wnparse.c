#include <stdio.h>
#include <stdlib.h>
#include <wn.h>

void main (int argc, char *argv[]) {
	FILE *f = NULL;
	wn_url_t *url, *frame;
	wn_fmt_t *fmt;

	if (argc < 4) {
		printf ("usage: %s <host> <uri> <out_file> [<timeout> [<read_file [<if_modified_since]]]\n", argv[0]);
		exit (1);
	}

	if (argc >= 6) {
		if (!(f = fopen (argv[5], "w"))) {
			printf ("cannot open %s\n", argv[5]);
			exit (2);
		}
	}

	if (!(url = wn_url_init (0, argv[1], argv[2], (argc >= 7) ? argv[6] : NULL))) {
		printf ("cannot initialise url\n");
		if (f) fclose (f);
		exit (3);
	}

	if (!(fmt = wn_fmt_fetch (url, (argc >= 5) ? atoi (argv[4]) : 0, 1))) {
		printf ("%s\n", wn_get_error());
		if (f) fclose (f);
		wn_url_free (url);
		exit (4);
	}

	wn_fmt_save (fmt, argv[3]);

	if (f) {
		fprintf (f, url->data);

		frame = url->frame;
		while (frame) {
			fprintf (f, "*** FRAME http://%s%s\n", frame->host, frame->uri);
			fprintf (f, frame->data);
		
			frame = frame->frame;
		}

		fclose (f);
	}

	printf ("%i links, %i images, %i textblocks, %i total\n",wn_fmt_typecount (fmt, WN_FMT_LINK), wn_fmt_typecount (fmt, WN_FMT_IMAGE), wn_fmt_typecount (fmt, WN_FMT_TEXT), fmt->size);

	printf ("document last modified: %s\n", url->last_modified ? url->last_modified : "??");
	if (wn_get_errno () == WE_RELOC)
		printf ("%s\n", wn_get_error());

	wn_fmt_free (fmt);
	wn_url_free (url);
}
