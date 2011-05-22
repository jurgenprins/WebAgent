#ifndef WN_NET_H
#define WN_NET_H
#include "wn_url.h"

#define HTTP_PORT 80
#define SMTP_PORT 25
#define DEFAULT_TIMEOUT 60

#undef USE_HTTP_1_1

#define HTTP_STAT_READING 0
#define HTTP_STAT_CONTD		1
#define HTTP_STAT_OK			2	
#define HTTP_STAT_SKIP		3
#define HTTP_STAT_RELOC		4
#define HTTP_STAT_ERROR		5
#define HTTP_STAT_CHUNKED	6

typedef struct _http_stat_t {
	int major_version;
	int minor_version;
	int result;
	int status;
	int header_len;
	long chunk_size;
} http_stat_t;

int wn_net_lookup (const char *addr);
long wn_net_fetch (wn_url_t *url, int timeout);

#endif
