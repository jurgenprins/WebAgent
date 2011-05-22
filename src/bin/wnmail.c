#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wn.h>

#define MAX_SIZE 10240

void main (int argc, char *argv[]) {
	wn_tmpl_t *tmpl;
	wn_fmt_t *fmt;
	char *str = NULL, *body = NULL;
	int blocks = 0;

	if ((argc < 2) || (argc == 3)) {
		fprintf (stderr, "usage: %s <config_file> [<to> <fmt_file>]\n", argv[0]);
		exit (0);
	}

	if (!wn_cfg_load (argv[1])) {
		fprintf (stderr, "%s\n", wn_get_error());
		exit (1);
	}

	if (!wn_mail_open (cfg_MAIL_HOST, NULL, cfg_MAIL_TIMEOUT)) {
		fprintf (stderr, "%s\n", wn_get_error());
		exit (1);
	}

	if (argc > 3) {
		if (!(fmt = wn_fmt_load (argv[3]))) {
			fprintf (stderr, "%s\n", wn_get_error());
			exit (1);
		}

		if (!(tmpl = wn_tmpl_init (cfg_TEMPLATE_UPDATED))) {
			fprintf (stderr, "%s\n", wn_get_error());
			exit (1);
		}

		wn_tmpl_set (tmpl, "TITLE", "manual sent file");
		wn_tmpl_set (tmpl, "DID", "0");
		wn_tmpl_set (tmpl, "URL", "<no url>");

		if (wn_str_cat (&body, str = wn_tmpl_get (tmpl, "header"), &blocks))
			free (str);
	
		if (wn_str_cat (&body, str = wn_mail_fmt (tmpl, fmt), &blocks))
			free (str);
	
		if (wn_str_cat (&body, str = wn_tmpl_get (tmpl, "footer"), &blocks))
			free (str);
	
		wn_mail_send (cfg_MAIL_SENDER, cfg_MAIL_FROM, argv[2], str = wn_tmpl_get (tmpl, "subject"), body);
		fprintf (stderr, "%s\n", wn_get_error());

		free (str);
		free (body);
		wn_fmt_free (fmt);
		wn_tmpl_free (tmpl);
		exit (1);
	} else {
		wn_mail_process (cfg_MAIL_SPOOL, cfg_MAIL_SENDER, cfg_MAIL_FROM, cfg_MAIL_CHUNK, cfg_MAIL_DELAY);
		fprintf (stderr, "%s\n", wn_get_error());
	}

 	if (!wn_mail_close()) {
		fprintf (stderr, "%s\n", wn_get_error());
		exit (1);
	}
}
