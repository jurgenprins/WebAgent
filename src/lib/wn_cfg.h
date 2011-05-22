#ifndef WN_CFG_H
#define WN_CFG_H

#define MAX_MSG_SIZE 500


int wn_cfg_read (char *config_file);

char *wn_cfg_retr (char *name);

char *wn_cfg_get (char *name);

int wn_cfg_free ();

#endif
