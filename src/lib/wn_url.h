#ifndef WN_URL_H
#define WN_URL_H

typedef struct _wn_url_t {
	int id;
	char *host;
	char *uri;
	char *data;
	char *title;
	char *mime_type;
	char *last_modified;
	char *location;
	struct _wn_url_t *frame;
} wn_url_t;

wn_url_t *wn_url_init (int id, char *host, char *uri, char *last_modified);
wn_url_t *wn_url_addframe (wn_url_t *url, char *src);
char *wn_urlify (wn_url_t *url, char *str);
int wn_url_free (wn_url_t *url);

#endif
