#ifndef WN_MAIL_H
#define WN_MAIL_H

#include "wn_fmt.h"
#include "wn_tmpl.h"

char *wn_mail_fmt (wn_tmpl_t *tmpl, wn_fmt_t *fmt);
int wn_mail_spool (char *dir, char *to, char *subj, char *body);
int wn_mail_process (char *dir, char *sender, char *from, int chunk, int delay);

int wn_mail_open (char *host, char *domain, int time_out);
int wn_mail_send (char *sender, char *from, char *to, char *subj, char *body);
int wn_mail_close();

#endif
