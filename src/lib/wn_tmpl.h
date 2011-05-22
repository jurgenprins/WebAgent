#ifndef WN_TMPL_H
#define WN_TMPL_H

#include "wn_fmt.h"

typedef struct _wn_tmpl_t {
	wn_fmt_t *defs;
	wn_fmt_t *vars;
	int defs_owner;
} wn_tmpl_t;

wn_tmpl_t *wn_tmpl_init (char *path);

int wn_tmpl_set (wn_tmpl_t *tmpl, char *name, char *value);

char *wn_tmpl_get (wn_tmpl_t *tmpl, char *name);

int wn_tmpl_free (wn_tmpl_t *tmpl);

#endif
