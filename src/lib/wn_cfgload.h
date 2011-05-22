#ifndef WN_CFGLOAD_H
#define WN_CFGLOAD_H

char *cfg_DATA_DIR;
char *cfg_PID_FILE;
char *cfg_LOG_FILE;
int cfg_LOG_APPEND;
int cfg_LOG_LEVEL;
char *cfg_DB_USER;
char *cfg_DB_PASS;
char *cfg_DB_NAME;
int cfg_DB_MAX_RETRIES;
int cfg_TIME_SLICE;
int cfg_DOC_MIN_AGE;
int cfg_DOC_MAX_PARALLEL;
int cfg_DOC_TIMEOUT;
int cfg_DOC_MAX_WAIT;
char *cfg_MAIL_HOST;
char *cfg_MAIL_FROM;
char *cfg_MAIL_SENDER;
int cfg_MAIL_TIMEOUT;
char *cfg_MAIL_SPOOL;
int cfg_MAIL_CHUNK;
int cfg_MAIL_DELAY;
char *cfg_TEMPLATE_UPDATED;
char *cfg_TEMPLATE_REMOVED;
char *cfg_TEMPLATE_LETTER;
int cfg_TEMPLATE_TEXT_CROP;
char *cfg_TEMPLATE_TEXT_ADD;

int wn_cfg_load (char *cfg_file);
int wn_cfg_reload ();

#endif
