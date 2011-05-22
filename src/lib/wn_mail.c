#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <errno.h>
#include <setjmp.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include "wn_err.h"
#include "wn_net.h"
#include "wn_fmt.h"
#include "wn_str.h"
#include "wn_mail.h"
#include "wn_cfgload.h"
#include "wn_config.h"

int sockfd;
int is_open = 0;
int time_out = 0;
char *the_host = NULL;
char *the_domain = NULL;
jmp_buf env;

int wn_mail_chat (char **str, int max_len, int okval) {
	int n, offset = 0;
	fd_set rfds;
	struct timeval tv;
	char smtpval[4];

	if (!str) {
		wn_report_error (WE_PARAM, "wn_mail_chat: nothing to send (NULL)");
		return 0;
	}

	if (!is_open && (!the_host || !wn_mail_open (the_host, NULL, time_out)))
		return 0;

	while ((n = send (sockfd, &(*str)[offset], strlen (*str) - offset, 0))) {
		if (n < 0) {
			wn_report_error (WE_SEND, "wn_mail_chat: write error: %s", strerror (errno));
			return 0;
		}
		if ((offset += n) >= strlen (*str)) 
			break;
	}

#ifdef DEBUG
	printf ("send: %s\n", *str);
	fflush (stdout);
#endif

	if (okval) {
		FD_ZERO (&rfds);
		FD_SET (sockfd, &rfds);
		tv.tv_sec = time_out;
		tv.tv_usec = 0;
		(*str)[0] = 0;
		
		if ((n = select (sockfd + 1, &rfds, NULL, NULL, &tv))) {
			if (n < 0) {
				wn_report_error (WE_UNKNOWN, "wn_mail_chat: select error: %s", strerror (errno));
				return 0;
			}
	
			if ((n = recv (sockfd, *str, max_len - 1, 0))) {
				if (n < 0) {
					wn_report_error (WE_UNKNOWN, "wn_mail_chat: read error: %s", strerror (errno));
					return 0;
				}
				(*str)[n] = 0;
				strncpy (smtpval, *str, 3);		
				smtpval[3] = 0;
	
#ifdef DEBUG
				printf ("receive : %s\n", smtpval);
				fflush (stdout);
#endif
	
				if (atoi (smtpval) != okval) {
					wn_report_error (WE_SMTP, "wn_mail_chat: server says: %s", &(*str)[4]);
					return 0;
				}
			} 
		} else {
			wn_report_error (WE_TIMEOUT, "wn_mail_chat: timeout waiting for response");
			return 0;	
		}
	}

	return 1;
}

void wn_mail_abort (int signal) {
	longjmp (env, 1);
}

int wn_mail_open (char *host, char *domain, int timeout) {
	struct sockaddr_in serv_addr;
	char *str = NULL;

	if (!host) {
		wn_report_error (WE_PARAM, "wn_mail_open: host not specified");
		return 0;
	}

	if (setjmp (env)) {
		wn_report_error (WE_TIMEOUT, "wn_mail_open: failed to connect to server %s", host);
		return 0;
	}

	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		wn_report_error (WE_SOCKET, "wn_mail_open: cannot create socket..");
		return 0;
	}

	serv_addr.sin_family = AF_INET;
	if ((serv_addr.sin_addr.s_addr = wn_net_lookup (host)) == -1) {
		wn_report_error (WE_LOOKUP, "wn_mail_open: unable to resolve %s", host);
		return 0;
	}
	serv_addr.sin_port = htons (SMTP_PORT);

	signal (SIGPIPE, SIG_IGN);
	signal (SIGALRM, wn_mail_abort);

	alarm (timeout);
	if (connect (sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		wn_report_error (WE_CONNECT, "wn_mail_open: failed to connect to server %s", host);
		alarm (0);
		return 0;
	}
	alarm (0);

	if (!domain) 
		domain = strchr (host, '.') + 1;

	time_out = (timeout > 0) ? timeout : 0;
	the_host = host;
	the_domain = domain;
	is_open = 1;

	str = (char *) malloc (1024);
	strcpy (str, "");
	if (!wn_mail_chat (&str, 1024, 220)) {		
		wn_mail_close();
		free (str);
		return 0;
	}

	sprintf (str, "HELO %s\r\n", domain);
	if (!wn_mail_chat (&str, 1024, 250)) {
		wn_mail_close();
		free (str);
		return 0;
	}
		
	free (str);
	return 1;
}

char *wn_mail_fmt (wn_tmpl_t *tmpl, wn_fmt_t *fmt) {
	wn_fmt_tag_t *tag;
	char *str = NULL;
	char *def = NULL;
	char blockstr[64];
	int blocks = 0, nr;

	if (!tmpl || !fmt || !fmt->first) return strdup ("");

	nr = fmt->first->blocknr;
	for (tag = fmt->first; tag; tag = tag->next) {
		sprintf (blockstr, "%i", tag->blocknr);
		switch (tag->type) {
		case WN_FMT_LINK:
			wn_tmpl_set (tmpl, "BLOCK", blockstr);
			wn_tmpl_set (tmpl, "PARAM", (tag->param && (strcmp (tag->param, WN_FMT_CHAR_NULL))) ? tag->param : "");
			wn_tmpl_set (tmpl, "CONTENT", (tag->content && (strcmp (tag->content, WN_FMT_CHAR_NULL))) ? wn_str_html2latin (tag->content) : "");
			if ((def = wn_tmpl_get (tmpl, "linkitem"))) {
				if (!str) {
					str = (char *) malloc (MAX_SIZE * ++blocks);
					str[0] = 0;
				}

				while ((strlen (str) + strlen (def)) > (MAX_SIZE * blocks)) 
					str = (char *) realloc (str, MAX_SIZE * ++blocks);
					
				strcat (str, def);
				free (def);
			}
			if (tag->blocknr != nr) {
				if ((def = wn_tmpl_get (tmpl, "blockchange"))) {
					while ((strlen (str) + strlen (def)) > (MAX_SIZE * blocks))
						str = (char *) realloc (str, MAX_SIZE * ++blocks);
					strcat (str, def);
					free (def);
				}
				nr = tag->blocknr;
			}
			break;
		case WN_FMT_TEXT:
			wn_tmpl_set (tmpl, "BLOCK", blockstr);
			wn_tmpl_set (tmpl, "PARAM", "");
			wn_tmpl_set (tmpl, "CONTENT", (tag->content && (strcmp (tag->content, WN_FMT_CHAR_NULL))) ? wn_str_crop (wn_str_html2latin (tag->content), cfg_TEMPLATE_TEXT_CROP, cfg_TEMPLATE_TEXT_ADD) : "");

			if ((def = wn_tmpl_get (tmpl, "textitem"))) {
				if (!str) { 
					str = (char *) malloc (MAX_SIZE * ++blocks);
					str[0] = 0;
				}

				while ((strlen (str) + strlen (def)) > (MAX_SIZE * blocks)) 
					str = (char *) realloc (str, MAX_SIZE * ++blocks);

				strcat (str, def);
				free (def);
			}
			if (tag->blocknr != nr) {
				if ((def = wn_tmpl_get (tmpl, "blockchange"))) {
					while ((strlen (str) + strlen (def)) > (MAX_SIZE * blocks))
						str = (char *) realloc (str, MAX_SIZE * ++blocks);
					strcat (str, def);
					free (def);
				}
				nr = tag->blocknr;
			}
			break;
		case WN_FMT_IMAGE:
			wn_tmpl_set (tmpl, "BLOCK", blockstr);
			wn_tmpl_set (tmpl, "PARAM", (tag->param && (strcmp (tag->param, WN_FMT_CHAR_NULL))) ? tag->param : "");
			wn_tmpl_set (tmpl, "CONTENT", "");
			if ((def = wn_tmpl_get (tmpl, "imageitem"))) {
				if (!str) {
					str = (char *) malloc (MAX_SIZE * ++blocks);
					str[0] = 0;
				}

				while ((strlen (str) + strlen (def)) > (MAX_SIZE * blocks))
					str = (char *) realloc (str, MAX_SIZE * ++blocks);

				strcat (str, def);
				free (def);
			}
			if (tag->blocknr != nr) {
				if ((def = wn_tmpl_get (tmpl, "blockchange"))) {
					while ((strlen (str) + strlen (def)) > (MAX_SIZE * blocks))
						str = (char *) realloc (str, MAX_SIZE * ++blocks);
					strcat (str, def);
					free (def);
				}
				nr = tag->blocknr;
			}
			break;
		default:
		}
	}

	return str;
}

int wn_mail_spool (char *dir, char *to, char *subj, char *body) {
	int i = 0;
	time_t t;
	struct stat stat_buf;
	char fname[32];
	FILE *f;

	if (!dir || !to || !subj || !body) {
		wn_report_error (WE_PARAM, "wn_mail_spool: insufficient parameters");
		return 0;
	}
	
	time (&t);
	while (i < 100) {
		sprintf (fname, "%s/%s.%li.%i", dir, to, (long)t, i);	
		if (stat (fname, &stat_buf))
			break;
		i++;
	}

	if (i == 100) {
		wn_report_error (WE_FILE, "wn_mail_spool: spool directory overflow");
		return 0;
	}

	if (!(f = fopen (fname, "w"))) {
		wn_report_error (WE_FILE, "wn_mail_spool: unable to open spool file %s", fname);
		return 0;
	}

	fprintf (f, "To: %s\r\n", to);
	fprintf (f, "Subject: %s\r\n", subj);
	fprintf (f, "%s\r\n", body);     

	fclose (f);
	return 1;
}

int wn_mail_process (char *dir, char *sender, char *from, int chunk, int delay) {
	FILE *f;
	DIR *d;
	struct dirent *e;
	struct stat stat_buf;
	char path[MAX_SIZE];
	char str[MAX_SIZE];
	char *to = NULL, *subj = NULL, *body = NULL;
	int blocks = 0, count = 0;

	if (!is_open && (!the_host || !wn_mail_open (the_host, NULL, time_out)))
		return 0;

	if (!(d = opendir (dir))) {
		wn_report_error (WE_FILE, "wn_mail_process: could not open directory %s", dir);
		return 0;
	}

	if (delay < 0)
		delay = 0;

	while ((e = readdir (d))) {
		if (chunk && (count++ >= chunk)) {
			sleep (delay);
			count = 0;
		}

		sprintf (path, "%s/%s", dir, e->d_name);

		if ((!stat (path, &stat_buf)) && (!(stat_buf.st_mode & S_IFDIR)) &&
		    (f = fopen (path, "r"))) {
			if (fgets (str, MAX_SIZE - 1, f) && !strncmp (str, "To:", 3)) {
				to = strdup (str + 4);
				to[strlen (to) - 2] = 0;
				if (fgets (str, MAX_SIZE - 1, f) && !strncmp (str, "Subject:", 8)) {
					subj = strdup (str + 9);
					subj[strlen (subj) - 1] = 0;
					while (fgets (str, MAX_SIZE - 1, f))
						wn_str_cat (&body, str, &blocks);

					if (wn_mail_send (sender, from, to, subj, body)) {
						if (body) 
							body[0] = 0;
						free (subj);
						free (to);
						fclose (f);
						unlink (path);
					} else {
						if (body) 
							body[0] = 0;
						free (subj);
						free (to);
						fclose (f);
						/* no unlinking, we try again later! */
					}
				} else {
					if (body) 
						body[0] = 0;
					free (to);
					fclose (f);
					unlink (path);
				}
			} else {
				if (body) 
					body[0] = 0;
				fclose (f);
				unlink (path);
			}	 	
		} 
	}

	if (body)
		free (body);
	closedir (d);
	return 1;
}

int wn_mail_send (char *sender, char *from, char *to, char *subj, char *body) {
	int len;
	char *str = NULL;

	if (!from || !to || !subj || !body) {
		wn_report_error (WE_PARAM, "wn_mail_send: insufficient parameters");
		return 0;
	}

	if (!is_open && (!the_host || !wn_mail_open (the_host, NULL, time_out))) 
		return 0;

	len = strlen (body) + 256;
	str = (char *) malloc (len);

	sprintf (str, "MAIL FROM: %s\r\n", from);
	if (!wn_mail_chat (&str, len, 250)) {
		wn_mail_close();
		free (str);
		return 0;
	}

	sprintf (str, "RCPT TO: %s\r\n", to);
	if (!wn_mail_chat (&str, len, 250)) {
		wn_mail_close();
		free (str);
		return 0;
	}
	
	sprintf (str, "DATA\r\n", to);
	if (!wn_mail_chat (&str, len, 354)) {
		wn_mail_close();
		free (str);
		return 0;
	}

	if (sender && strlen (sender))
		sprintf (str, "From: %s <%s>\r\nTo: %s\r\nSubject: %s\r\n%s", sender, from, to, subj, body);
	else 
		sprintf (str, "From: %s\r\nTo: %s\r\nSubject: %s\r\n%s", from, to, subj, body);

	wn_str_crlf (&str);

	if (!wn_mail_chat (&str, len, 0)) {
		wn_mail_close();
		free (str);
		return 0;
	}

	strcpy (str, "\r\n\r\n.\r\n");

	if (!wn_mail_chat (&str, len, 250)) {
		wn_mail_close();
		free (str);
		return 0;
	}

	free (str);
	return 1;
}

int wn_mail_close () {
	char str[10];

	if (is_open) {
		sprintf (str, "QUIT\n");
		write (sockfd, str, strlen (str));
		close (sockfd);
		is_open = 0;
	}

	return 1;
}
