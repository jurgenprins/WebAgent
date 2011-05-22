#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wn_url.h"

wn_url_t *wn_url_init (int id, char *host, char *uri, char *last_modified) {
	wn_url_t *url;

	if (!host || !uri) return NULL;

	if ((url = (wn_url_t *) malloc (sizeof (wn_url_t))) == NULL) return NULL;
	url->id = id;
	url->host = strdup (host);
	url->uri = strdup (uri);
	url->data = NULL;
	url->title = NULL;
	url->mime_type = NULL;
	url->location = NULL;
	if (last_modified && strlen (last_modified))
		url->last_modified = strdup (last_modified);
	else
		url->last_modified = NULL;
	url->frame = NULL;

	return url;
}

wn_url_t *wn_url_addframe (wn_url_t *url, char *src) {
	wn_url_t *frame;
	char *url_str = NULL, *host = NULL;

	if (!url || !src) return NULL;

	if (!(url_str	= wn_urlify (url, src))) return NULL;
	if (!(host = strtok ((host = strdup (&url_str[7])), "/"))) {
		free (url_str);
		return NULL;
	}
	
	frame = url->frame;
	if (frame) {
		while (frame->frame) 
			frame = frame->frame;
		frame->frame = wn_url_init (0, host, &url_str[strlen (host) + 7], NULL);	
		free (url_str);
		free (host);
		return frame->frame;
	} 

	url->frame = wn_url_init (0, host, &url_str[strlen (host) + 7], NULL);
	free (url_str);
	free (host);
	return url->frame;
}

char *wn_urlify (wn_url_t *url, char *str) {
	char *url_str = NULL;
	int i;

	if (str[0] == '/') {
		url_str = (char *) malloc (strlen (str) + strlen (url->host) + 10);
		sprintf (url_str, "http://%s%s", url->host, str);
	} else if (!strstr (str, "://") &&
	           strncmp (str, "mailto:", 7) &&
	           strncmp (str, "news:", 5) &&
	           strncmp (str, "javascript:", 11)) {
		url_str = (char *) malloc (strlen (str) + strlen (url->host) + strlen (url->uri) + 10);
		sprintf (url_str, "http://%s%s", url->host, (url->uri && strlen (url->uri)) ? url->uri : "/");
		for (i = strlen (url_str) - 1; i >= 0; i--) {
			if (url_str[i] == '/') {
				url_str[i + 1] = 0;
				break;
			}
		}
		strcat (url_str, str);
	} else {
		url_str = strdup (str);
	}

	return url_str;
}

int wn_url_free (wn_url_t *url) {
	wn_url_t *frame, *frame2;
	if (!url) return 1;

	if (url->host) free (url->host);
	if (url->uri) free (url->uri);
	if (url->data) free (url->data);
	if (url->title) free (url->title);
	if (url->mime_type) free (url->mime_type);
	if (url->last_modified) free (url->last_modified);
	if (url->location) free (url->location);
	frame = url->frame;
	while (frame) {
		frame2 = frame->frame;
		if (frame->host) free (frame->host);
		if (frame->uri) free (frame->uri);
		if (frame->data) free (frame->data);
		if (frame->title) free (frame->title);
		if (frame->mime_type) free (frame->mime_type);
		if (frame->last_modified) free (frame->last_modified);
		if (frame->location) free (frame->location);
		free (frame);
		frame = frame2;
	}

	free (url);
	url = NULL;

	return 1;
}
