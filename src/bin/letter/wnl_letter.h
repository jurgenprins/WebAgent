#ifndef WNL_LETTER_H
#define WNL_LETTER_H
#include <wn.h>

typedef struct _wnl_body_t {
	int did;
	char *title;
	char *url;
	char *data;
	int filter;
	char *words;
	char *bfilter;
	struct _wnl_body_t *next;
} wnl_body_t;

typedef struct _wnl_letter_t {
	int uid;
	char *email;
	wnl_body_t *first_body;
	wnl_body_t *last_body;
	struct _wnl_letter_t *next;
} wnl_letter_t;

wnl_letter_t *wnl_letter_init (int uid, char *email);
int wnl_letter_delete (int uid);
wnl_body_t *wnl_letter_add (wnl_letter_t *letter, int did, char *title, char *url, int filter, char *words, char *bfilter);
wnl_body_t *wnl_letter_body_by_did (wnl_letter_t *letter, int did);
wnl_letter_t *wnl_letter_by_uid (int uid);
wnl_letter_t *wnl_letter_first ();
int wnl_letter_count ();

#endif
