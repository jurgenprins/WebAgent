#ifndef WNC_DOC_H
#define WNC_DOC_H
#include <unistd.h>
#include <wn.h>

typedef struct _wnc_doc_t {
	wn_url_t *url;
	char *path;
	pid_t pid;
	time_t start;
	struct _wnc_doc_t *next;
} wnc_doc_t;

wnc_doc_t *wnc_doc_init (int id, char *host, char *uri, char *path, char *last_modified);
int wnc_doc_delete (int id);
wnc_doc_t *wnc_doc_by_path (char *path);
wnc_doc_t *wnc_doc_first ();
int wnc_doc_count ();

#endif
