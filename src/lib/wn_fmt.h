#ifndef WN_FMT_H
#define WN_FMT_H

#include "wn_url.h"

#define WN_FMT_ERROR -1
#define WN_FMT_RAW 0
#define WN_FMT_LINK 1
#define WN_FMT_TEXT 2
#define WN_FMT_IMAGE 4
#define WN_FMT_ALL 7

#define WN_FMT_CHAR_NULL "\001"

typedef struct _wn_fmt_tag_t {
	int type;
	int blocknr;
	char *param;
	char *content;
	struct _wn_fmt_tag_t *next;
} wn_fmt_tag_t;

typedef struct _wn_fmt_t {
	wn_fmt_tag_t *first;
	wn_fmt_tag_t *last;
	int size;	
	int blocknr;
	int in_block;
	int link_count;
	int text_count;
	int image_count;
	int chopped_text;
} wn_fmt_t;

char *chop (char *str, int no_spaces);

wn_fmt_t *wn_fmt_init();
wn_fmt_tag_t *wn_fmt_add (wn_fmt_t *wn_fmt, int type, char *param, char *content);
int wn_fmt_update (wn_fmt_tag_t *wn_fmt_tag, char *param, char *content);
int wn_fmt_expand (wn_fmt_tag_t *wn_fmt_tag, char *param, char *content);
wn_fmt_tag_t *wn_fmt_find_by_param (wn_fmt_t *wn_fmt, char *param);
void wn_fmt_retract (wn_fmt_t *wn_fmt);
int wn_fmt_setblock (wn_fmt_t *wn_fmt);
int wn_fmt_unsetblock (wn_fmt_t *wn_fmt);
int wn_fmt_free (wn_fmt_t *wn_fmt);

wn_fmt_t *wn_fmt_parse (wn_url_t *url);
wn_fmt_t *wn_fmt_fetch (wn_url_t *url, int timeout, int recurse_frames);
wn_fmt_t *wn_fmt_diff (wn_fmt_t *wn_fmt_old, wn_fmt_t *wn_fmt_new);
wn_fmt_t *wn_fmt_filter (wn_fmt_t *wn_fmt, int type, char *pattern, char *blocks);

char *wn_fmt_type2str (int type);
int wn_fmt_type2int (char *type);
int wn_fmt_typecount (wn_fmt_t *wn_fmt, int type);

int wn_fmt_save (wn_fmt_t *wn_fmt, char *fname);  
wn_fmt_t *wn_fmt_load (char *fname);

#endif
