/* Network stuff. JP
 * Aug 28 1999: Added HTTP/1.1 parsing..
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <setjmp.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/param.h>
#include "wn_err.h"
#include "wn_url.h"
#include "wn_net.h"
#include "wn_str.h"
#include "wn_config.h"

jmp_buf env;

int wn_net_lookup (const char *addr) {
	uint r;
	struct hostent *host_info;

	r = inet_addr (addr);
	if (r == (uint) - 1) {
		host_info = gethostbyname (addr);
		if (host_info == 0) 
			return -1;
		memcpy ((char *) &r, host_info->h_addr, host_info->h_length);
	}

	return r;
}

int wn_net_scan_header (char *data, char *header, char **dst) {
	char *str = NULL;
	int i;

	if (!header) return 0;

	if ((str = strstr (data, header))) {
		if (*dst)
			free (*dst);
		*dst = (char *) malloc (strlen (str));
		strcpy (*dst, &str[strlen (header)]);
		for (i = 0; (*dst)[i] ; i++) {
			if (((*dst)[i] == '\r') || ((*dst)[i] == '\n') ||
			    ((*dst)[i] == '\t') || ((*dst)[i] == ';')) {
				(*dst)[i] = 0;
				return 1;
			}
		}
		free (*dst);
	}

	*dst = NULL;
	return 0;
}

int wn_net_http_chunk_move (wn_url_t *url, http_stat_t *http){
	int i = 0, j = 0;
	char chunk_size_str[64];

#ifndef USE_HTTP_1_1
	return 0;
#endif

	if (!url->data || !http || 
	    (!(http->major_version == 1) && (http->minor_version == 1)))
		return 0;

	while ((url->data[i] == '\n') || (url->data[i] == '\r'))
		i++;

	while ((url->data[i] != '\n') && (url->data[i] != '\r') && (j < 64)) {
		chunk_size_str[j] = url->data[i];
		i++; j++;
	}
	chunk_size_str[j] = 0;
	http->chunk_size = strtol (chunk_size_str, NULL, 16);
	
	while ((url->data[i] == '\n') || (url->data[i] == '\r'))
		i++;

	return wn_str_shift (url->data, i);
}

http_stat_t *wn_net_http_status (wn_url_t *url, http_stat_t *http) {
	char *http_status_header = NULL;
	int i = 0;

	if (!url) return 0;

	if (!http->result) {
		if (wn_net_scan_header (url->data, "TTP/", &http_status_header)) {
			if (http_status_header && (strlen (http_status_header) > 6)) {
				http->major_version = (int)http_status_header[0] - 48;
				http->minor_version = (int)http_status_header[2] - 48;
				http_status_header[7] = 0;
				if (((http->result = atoi (&http_status_header[4])) != 200) && 
				   	 ((http->result < 300) || (http->result >= 304)) && 
				   	 (http->result != 307)) {
					free (http_status_header);
					if (http->result == 304)
						http->status = HTTP_STAT_SKIP;
					else
						http->status = HTTP_STAT_ERROR;
	
					return http;
				}
				free (http_status_header);
			}
		}
	}

	if (!url->mime_type)
		wn_net_scan_header (url->data, "ype: ", &(url->mime_type));

	if (!url->location) 
		if (wn_net_scan_header (url->data, "ation: ", &(url->location)) &&
			  (((http->result >= 300) && (http->result < 304)) || (http->result == 307))) {
			http->status = HTTP_STAT_RELOC;
			return http;
		}

	// last_modified can already be set (from database), so make match_header 
	// overwrite that setting with the newer modification date (if it were
  // the same, we probably expected the http_status to be 304 and this
  // strip function already exited on the status 200 check)

	if (http->status != HTTP_STAT_CHUNKED)  // hack to check if it is already set
		wn_net_scan_header (url->data, "ified: ", &(url->last_modified));

	switch (http->status) {
	case HTTP_STAT_CHUNKED:
		if (http->chunk_size && (strlen (url->data) > http->chunk_size)) {
			url->data[http->chunk_size] = 0;
			http->status = HTTP_STAT_OK;
			return http;
		}
		break;
	case HTTP_STAT_CONTD:
		if (url->data[0] == '\r') {
			if (strlen (url->data) > 2) {
				http->header_len = wn_str_shift (url->data, 2);
				if (wn_net_http_chunk_move (url, http)) 
					http->status = HTTP_STAT_CHUNKED;
				else
					http->status = HTTP_STAT_OK;
				return http;
			} else {  // if this happens, the page data is probably empty.. anyway..
				i = strlen (url->data);
				url->data[0] = 0;	
				http->header_len = i;
				http->status = HTTP_STAT_OK;
				return http;
			}
		}
	case HTTP_STAT_READING:
		if ((http_status_header = strstr (url->data, "\n\r"))) {
			i = strlen (url->data) - strlen (http_status_header);
			if (i < strlen (url->data) - 2) {
				http->header_len = wn_str_shift (url->data, i + 3);
				if (wn_net_http_chunk_move (url, http)) 
					http->status = HTTP_STAT_CHUNKED;
				else
					http->status = HTTP_STAT_OK;
				return http;
			} else {
				i = strlen (url->data);
				url->data[0] = 0;  // indecisive stuff, just kill whole header
				http->header_len = i;
				http->status = HTTP_STAT_OK;
				return http;
			}
		}
		if (url->data[strlen (url->data) - 1]  == '\n') {
			// only resolve the case 'blabla\r\n<new block>\r\n' here..
			http->status = HTTP_STAT_CONTD;
			return http;
		}
		return http;
	default:
		http->status = HTTP_STAT_OK;   // should not be reached
		return http;
	}

	return http;
}

void wn_net_abort (int signal) {
	longjmp (env, 1);
}

long wn_net_fetch (wn_url_t *url, int timeout) {
	time_t start, now;
	int sockfd, n = 0, blocks = 0;
	char *htmlstr = NULL, *url_str = NULL;
	struct sockaddr_in serv_addr;
	http_stat_t http;

	if (!url || !url->host) {
		wn_report_error (WE_PARAM, "wn_net_fetch: no url initialised");
		return 0;
	}

	if (setjmp (env)) {
		wn_report_error (WE_CONNECT, "wn_net_fetch: failed to connect to %s within %i secs", url->host, timeout);
		return 0;
	}


	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		wn_report_error (WE_SOCKET, "wn_net_fetch: cannot create socket..");
		return 0;
	}

	serv_addr.sin_family = AF_INET;
	if ((serv_addr.sin_addr.s_addr = wn_net_lookup (url->host)) == -1) {
		wn_report_error (WE_LOOKUP, "wn_net_fetch: unable to resolve %s", url->host);
		return 0;
	}
	serv_addr.sin_port = htons (HTTP_PORT);

	signal (SIGPIPE, SIG_IGN);
	signal (SIGALRM, wn_net_abort);

	alarm (timeout);
	if (connect (sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		wn_report_error (WE_CONNECT, "wn_net_fetch: failed to connect to server %s", url->host);
		alarm (0);
		return 0;
	}
	alarm (0);

	if (!timeout) 
		timeout = DEFAULT_TIMEOUT;

	htmlstr = (char *) malloc (MAX_SIZE);
	strcpy (htmlstr, "GET ");
	strcat (htmlstr, (url->uri && strlen (url->uri)) ? url->uri : "/");
#ifdef USE_HTTP_1_1
	strcat (htmlstr, " HTTP/1.1\nAccept-Encoding: \nConnection: close\nHost: ");
#else
	strcat (htmlstr, " HTTP/1.0\nHost: ");
#endif
	strcat (htmlstr, url->host);
	if (url->last_modified && strlen (url->last_modified)) {
		strcat (htmlstr, "\nIf-Modified-Since: ");
		strcat (htmlstr, url->last_modified);
	}
	strcat (htmlstr, "\nUser-Agent: Netscape 3.0 Compatible (WhatsNew Robot)\n\n");

	send (sockfd, htmlstr, strlen (htmlstr), 0);

	fcntl (sockfd, F_SETFL, O_NONBLOCK);
	time (&start);
	http.status = HTTP_STAT_READING; http.result = 0; http.header_len = 0;
	http.major_version = 0; http.minor_version = 0;

	if (!url->data) {
		blocks = 2;
		url->data = (char *) malloc (MAX_SIZE * blocks);
		url->data[0] = 0;
	}

	while ((n = recv (sockfd, htmlstr, MAX_SIZE - 1, 0)) != 0) {
		if (n > 0) {
			htmlstr[n] = 0;
			if ((strlen (url->data) + n + http.header_len + MAX_SIZE) > (MAX_SIZE * blocks)) {
				http.header_len = 0;
				url->data = (char *) realloc (url->data, MAX_SIZE * ++blocks);	
			}
			strcat (url->data, htmlstr);
			if (http.status != HTTP_STAT_OK) {
				wn_net_http_status (url, &http);
				switch (http.status) {
				case HTTP_STAT_CHUNKED:
				case HTTP_STAT_READING:
				case HTTP_STAT_CONTD:
				case HTTP_STAT_OK:
					break; 	// just continue reading data
				case HTTP_STAT_SKIP:
					wn_report_error (WE_SKIP, "wn_net_fetch: not modified, so not retrieved");
					close (sockfd);
					free (htmlstr);
					return 0;	
				case HTTP_STAT_RELOC:
					if (url->location) {
						if (url->mime_type) { free (url->mime_type); url->mime_type = NULL; }
						if (url->last_modified) { free (url->last_modified); url->last_modified = NULL; }
						if (((url_str = wn_urlify (url, url->location))) &&
						    (free (url->host), free (url->uri), 1) &&
						    ((url->host = strtok (url->host = strdup (&url_str[7]), "/"))) &&
						    ((url->uri = strdup (&url_str[strlen (url->host) + 7])))) {
							free (url->location);
							url->location = NULL;
							url->data[0] = 0;
							close (sockfd);
							free (htmlstr);
		
							if (wn_net_fetch (url, timeout))
								wn_report_error (WE_RELOC, "wn_net_fetch: document is relocated at %s and is retrieved", url_str);	
							else
								wn_report_error (WE_HTTP, "wn_net_fetch: document is relocated but while fetching from new location: %s", wn_get_error());

							free (url_str);
							return 1;
						} else {
							wn_report_error (WE_HTTP, "wn_net_fetch: document is relocated but location could not be extracted from headers");	
						}
					} else
						wn_report_error (WE_HTTP, "wn_net_fetch: document is relocated but location was not found in headers");

					close (sockfd);
					free (htmlstr);
					return 0;
				default:
					wn_report_error (WE_HTTP, "wn_net_fetch: http error %i", http.result);
					close (sockfd);
					free (htmlstr);
					return 0;
				}
			}
		} else {
			time (&now);
			if (now > (start + timeout)) {
				htmlstr[0] = 0;
				close (sockfd);
				free (htmlstr);

				wn_report_error (WE_TIMEOUT, "wn_net_fetch: timeout retrieving http://%s%s", url->host, url->uri);
			
				return 0;
			} else if (errno != EAGAIN) {
					wn_report_error (WE_UNKNOWN, "wn_net_fetch: socket error: %s", strerror (errno));
					close (sockfd);
					free (htmlstr);
					return 0;
			}
		}
	}
	close (sockfd);
	free (htmlstr);

	if (!url->data || !strlen (url->data)) {
		wn_report_error (WE_EMPTY, "wn_net_fetch: no data retrieved");
		return 0;
	}

	return strlen (url->data);
}
