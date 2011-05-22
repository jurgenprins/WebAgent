#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <wn.h>
#include "wnc_doc.h"

wnc_doc_t *first_doc = NULL;
wnc_doc_t *last_doc = NULL;
int num_docs = 0;

wnc_doc_t *wnc_doc_init (int id, char *host, char *uri, char *path, char *last_modified) {
	wnc_doc_t *the_doc;

	if (!id || !host || !uri || !path) {
		wn_report_error (WE_PARAM, "add_doc: No path or url given");
		return NULL;
	}

	the_doc = (wnc_doc_t *) malloc (sizeof (wnc_doc_t));
	the_doc->url = wn_url_init (id, host, uri, last_modified);
	the_doc->path = strdup (path);
	the_doc->pid = 0;
	time (&(the_doc->start));
	the_doc->next = NULL;

	if (!first_doc || !last_doc)
		first_doc = the_doc;
	else
		last_doc->next = the_doc;
	last_doc = the_doc;

	num_docs++;
	return last_doc;
}

int wnc_doc_delete (int id) {
	wnc_doc_t *doc;
	wnc_doc_t *doomed;

	if (first_doc && first_doc->url && (first_doc->url->id == id)) {
		doomed = first_doc;

		if (!first_doc->next)
			last_doc = NULL;

		first_doc = first_doc->next;

		wn_url_free (doomed->url);
		free (doomed->path);
		free (doomed);

		num_docs--;
		return 1;
	}

	for (doc = first_doc; doc->next; doc = doc->next) {
		if (doc->next->url && (doc->next->url->id == id)) {
			doomed = doc->next;
			if (last_doc && last_doc->url && (last_doc->url->id == id))
				last_doc = doc;

			doc->next = doc->next->next;

			wn_url_free (doomed->url);
			free (doomed->path);
			free (doomed);
		
			num_docs--;
			return 1;
		}
	}

	return 0;
}

wnc_doc_t *wnc_doc_by_path (char *path) {
	wnc_doc_t *doc;

	for (doc = first_doc; doc; doc = doc->next) {
		if (!strcmp (doc->path, path)) 
			return doc;
	}

	return NULL;
}

wnc_doc_t *wnc_doc_first () {
	return first_doc;
}

int wnc_doc_count () {
	return num_docs;
}
