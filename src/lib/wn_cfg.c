/* Een leuke configfile reader. Dat is toch wel weer handig. by JP. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wn_err.h"
#include "wn_fmt.h"
#include "wn_str.h"
#include "wn_cfg.h"

typedef struct _wn_cfg_t {
	char *name;
	char *value;
	struct _wn_cfg_t *next;
} wn_cfg_t;

wn_cfg_t *first_wn_cfg = NULL;
wn_cfg_t *last_wn_cfg = NULL;

int wn_cfg_add (char *name, char *value) {
	wn_cfg_t *new_wn_cfg;

	if (!name || !value) {
		wn_report_error (WE_PARAM, "wn_cfg_add: no name or value given");
		return 0;
	}

	new_wn_cfg = (wn_cfg_t *) malloc (sizeof (wn_cfg_t));

	if (!last_wn_cfg)
		first_wn_cfg = new_wn_cfg;
	else
		last_wn_cfg->next = new_wn_cfg;

	new_wn_cfg->name = wn_str_chop (strdup (name), 0);
	new_wn_cfg->value = wn_str_chop (strdup (value), 0);

	last_wn_cfg = new_wn_cfg;

	return 1;
}

int wn_cfg_read (char *file_name) {
	FILE *f;
	char *name;
	char *value;
	char *str;
	int i;

	str = (char *) alloca (MAX_MSG_SIZE);
	name = (char *) alloca (MAX_MSG_SIZE);

	if (!file_name) {
		wn_report_error (WE_PARAM, "read_config: filename not set");
		return 0;
	}

	if (!(f = fopen (file_name, "r"))) {
		wn_report_error (WE_FILE, "read_config: could not open %s", file_name);
		return 0;
	}

	while (fgets (str, MAX_MSG_SIZE - 1, f)) {
		if (strchr (str, '=')) {
			for (i = 0; i < strlen (str); i++) {
				name[i] = str[i];
				if (str[i] == '=') break;
			}

			for (i = i - 1; i > 0; i--)
				if (str[i] != 32) break;
			name[i + 1] = 0;

			for (i = i + 1; i < strlen (str); i++)
				if (str[i] == '=') break;
			
			for (i = i + 1; i < strlen (str); i++)
				if (str[i] != 32) break;

			value = (char *) alloca (strlen (str) - i + 1);
			strcpy (value, &str[i]);
			value [strlen (value) - 1] = 0;

			wn_cfg_add (name, value);

			str[0] = 0;
		}
	}

	fclose (f);
	return 1;
}

char *wn_cfg_retr (char *name) {
	wn_cfg_t *search_wn_cfg;

	for (search_wn_cfg = first_wn_cfg;
	     search_wn_cfg;
	     search_wn_cfg = search_wn_cfg->next) 
		if (!strcasecmp (search_wn_cfg->name, name)) 
			return search_wn_cfg->value;

	wn_report_error (WE_UNKNOWN, "retr_wn_cfg: '%s' is not set in configuration!", name);
	return strdup ("");
}

char *wn_cfg_get (char *name) {
	wn_cfg_t *search_wn_cfg;

	for (search_wn_cfg = first_wn_cfg;
	     search_wn_cfg;
	     search_wn_cfg = search_wn_cfg->next)
		if (!strcasecmp (search_wn_cfg->name, name))
			return search_wn_cfg->value;

	printf ("** FATAL: Variable '%s' is not set in configuration, cannot continue\n", name);
	fflush (stdout);

	exit (0);	
}

int wn_cfg_free () {
	wn_cfg_t *wn_cfg1, *wn_cfg2;

	wn_cfg1 = first_wn_cfg;
	while (wn_cfg1) {
		wn_cfg2 = wn_cfg1->next;
		free (wn_cfg1->name);
		free (wn_cfg1->value);
		free (wn_cfg1);
		wn_cfg1 = wn_cfg2;
	}

	return 1;
}
