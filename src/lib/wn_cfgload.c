#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "wn_cfg.h"
#include "wn_cfgload.h"

char cur_cfg_file[256];

int wn_cfg_load (char *cfg_file) {
	if (cfg_file) 
		strcpy (cur_cfg_file, cfg_file);

	if (!wn_cfg_read (cur_cfg_file))
		return 0;
	
	cfg_DATA_DIR = wn_cfg_get("DATA_DIR");
	cfg_PID_FILE = wn_cfg_get("PID_FILE");
	cfg_LOG_FILE = wn_cfg_get("LOG_FILE");
	cfg_LOG_APPEND = atoi(wn_cfg_get("LOG_APPEND"));
	cfg_LOG_LEVEL = atoi(wn_cfg_get("LOG_LEVEL"));
	cfg_DB_USER = wn_cfg_get("DB_USER");
	cfg_DB_PASS = wn_cfg_get("DB_PASS");
	cfg_DB_NAME = wn_cfg_get("DB_NAME");
	cfg_DB_MAX_RETRIES = atoi(wn_cfg_get("DB_MAX_RETRIES"));
	cfg_TIME_SLICE = atoi(wn_cfg_get("TIME_SLICE"));
	cfg_DOC_MIN_AGE = atoi(wn_cfg_get("DOC_MIN_AGE"));
	cfg_DOC_MAX_PARALLEL = atoi(wn_cfg_get("DOC_MAX_PARALLEL"));
	cfg_DOC_TIMEOUT = atoi(wn_cfg_get("DOC_TIMEOUT"));
	cfg_DOC_MAX_WAIT = atoi(wn_cfg_get("DOC_TIMEOUT"));
	cfg_MAIL_HOST = wn_cfg_get("MAIL_HOST");
	cfg_MAIL_FROM = wn_cfg_get("MAIL_FROM");
	cfg_MAIL_SENDER = wn_cfg_get("MAIL_SENDER");
	cfg_MAIL_TIMEOUT = atoi(wn_cfg_get("MAIL_TIMEOUT"));
	cfg_MAIL_SPOOL = wn_cfg_get("MAIL_SPOOL");
	cfg_MAIL_CHUNK = atoi(wn_cfg_get("MAIL_CHUNK"));
	cfg_MAIL_DELAY = atoi(wn_cfg_get("MAIL_DELAY"));
	cfg_TEMPLATE_UPDATED = wn_cfg_get("TEMPLATE_UPDATED");
	cfg_TEMPLATE_REMOVED = wn_cfg_get("TEMPLATE_REMOVED");
	cfg_TEMPLATE_LETTER = wn_cfg_get("TEMPLATE_LETTER");
	cfg_TEMPLATE_TEXT_CROP = atoi(wn_cfg_get("TEMPLATE_TEXT_CROP"));
	cfg_TEMPLATE_TEXT_ADD = wn_cfg_get("TEMPLATE_TEXT_ADD");

	return 1;
}

int wn_cfg_reload () {
	if (cur_cfg_file && strlen (cur_cfg_file)) {
		wn_cfg_free();
		return wn_cfg_load (NULL);
	}

	return 0;
}
