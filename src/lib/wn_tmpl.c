#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wn_config.h"
#include "wn_err.h"
#include "wn_fmt.h"
#include "wn_tmpl.h"

wn_tmpl_t *wn_tmpl_init (char *path) {
	FILE *f;
	char str[MAX_SIZE];
	wn_tmpl_t *tmpl;
	wn_fmt_tag_t *tag = NULL;

	if (!(tmpl = (wn_tmpl_t *) malloc (sizeof (tmpl)))) {
		wn_report_error (WE_MEM, "wn_tmpl_init: insufficient memory");
		return NULL;
	}	

	if ((path && !(tmpl->defs = wn_fmt_init())) ||
	    (!(tmpl->vars = wn_fmt_init())))
		return NULL;

	if (!path) {
		tmpl->defs = NULL;
		tmpl->defs_owner = 0;
		return tmpl;
	}

	if (!(f = fopen (path, "r"))) {
		wn_report_error (WE_FILE, "wn_tmpl_init: could not open template %s", path);
		wn_tmpl_free (tmpl);	
		return NULL;
	}

	while (fgets (str, MAX_SIZE - 1, f)) {
		if (!tag && (strstr (str, " {"))) {
			if (!(tag = wn_fmt_add (tmpl->defs, WN_FMT_RAW, strtok (str, " "), NULL))) {
				wn_tmpl_free (tmpl);
				return NULL;
			}
				
			continue;
		}

		if (tag) {
			if (strstr (str, "}")) {
				wn_fmt_expand (tag, NULL, strtok (str, "}"));
				tag = NULL;
			} else 
				wn_fmt_expand (tag, NULL, str);
		}
	}

	fclose (f);

	tmpl->defs_owner = 1;
	return tmpl;
}

int wn_tmpl_set (wn_tmpl_t *tmpl, char *name, char *value) {
	wn_fmt_tag_t *tag;

	if (!tmpl || !name || !value) {
		wn_report_error (WE_PARAM, "wn_tmpl_set: insufficient parameters");
		return 0;
	}

	if ((tag = wn_fmt_find_by_param (tmpl->vars, name))) 
		return wn_fmt_update (tag, name, value);
	else
		return (wn_fmt_add (tmpl->vars, WN_FMT_RAW, name, value) != NULL);
}

char *wn_tmpl_get (wn_tmpl_t *tmpl, char *name) {
	wn_fmt_tag_t *tag, *tag2;
	int size = 0, i, j, k;
	char *str = NULL;
	char var_name[128];

	if (!tmpl || !name) {
		wn_report_error (WE_PARAM, "wn_tmpl_get: insufficient parameters");
		return NULL;
	}

	if (!tmpl->defs) {
		wn_report_error (WE_PARAM, "wn_tmpl_get: no template definition set (at init?)");
		return NULL;
	}

	if (!(tag = wn_fmt_find_by_param (tmpl->defs, name)))
		return NULL;

	if (tag->content && strlen (tag->content)) {
		str = (char *) malloc ((size = strlen (tag->content) * 2));

		str[0] = 0;
		var_name[0] = 0;
		j = 0;
		k = 0;

		for (i = 0; i <= strlen (tag->content); i++) {
			if (((tag->content)[i] == '$') && ((tag->content[i + 1] == '('))) {
				var_name[0] = (tag->content)[i + 2];
				j = 1;
				i += 2;
				continue;
			}

			if (j) {
				if (((tag->content)[i] == ')') || (j >= 127)) {
					var_name[j] = 0;
					if (((tag2 = wn_fmt_find_by_param (tmpl->vars, var_name))) && 
					    tag2->content) {
						if ((k + strlen (tag2->content) + 10) >= size)
							str = (char *) realloc (str, (size += (strlen (tag2->content) + 10)));	
						str[k] = 0;
						strcat (str, tag2->content);
						k += strlen (tag2->content);
					}
					j = 0;
				} else {
					var_name[j] = (tag->content)[i];
					j++;
				}
			} else {
				str[k] = (tag->content)[i];
				k++;
				if (k >= size) 
					str = (char *) realloc (str, (size += MAX_SIZE));
			}
		}
	}

	return str;
}

int wn_tmpl_free (wn_tmpl_t *tmpl) {
	if (!tmpl) 
		return 1;

	if (tmpl->defs_owner)
		wn_fmt_free (tmpl->defs);
	wn_fmt_free (tmpl->vars);
	free (tmpl);

	return 1;
}
