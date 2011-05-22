#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include "../../wn_config.h"
#include "../../wn_err.h"
#include "../../wn_cfgload.h"
#include "mysql.h"

wn_db_t *wn_db_init () {
	int i;
	wn_db_t *db = (wn_db_t *) malloc (sizeof (wn_db_t));

#if MYSQL_VERSION_ID >= 32000
	if (!mysql_init (&(db->conn))) {
		wn_report_error (WE_DB, "wn_db_init: could not initialize connection");
		free (db);
		return NULL;
	}
#endif
		
	db->connected = 0;

	for (i = 0; i < MAX_RES; i++)
		db->has_result[i] = 0;

	return db;
}

int wn_db_connect (wn_db_t *db) {
	int i = 0;

	if (!db) {
		wn_report_error (WE_PARAM, "wn_db_connect: invalid db connection");
		return 0;
	}

	if (db->connected)
		return 1;

	while (i < MAX_RETRY) {
#if MYSQL_VERSION_ID >= 32200
	  if (mysql_real_connect (&(db->conn), NULL, cfg_DB_USER, cfg_DB_PASS, cfg_DB_NAME, 0, NULL, 0)) {
#else
 	 if (mysql_connect (&(db->conn), NULL, cfg_DB_USER, cfg_DB_PASS) && !mysql_select_db (&(db->conn), cfg_DB_NAME)) {
#endif
			db->connected = 1;
			return 1;
		}

		sleep (1);
		i++;
	}

	wn_report_error (WE_DB, "wn_db_connect: %s\n", mysql_error (&(db->conn)));
	return 0;
}

int wn_db_query (wn_db_t *db, int res_idx, char *sql, ...) {
	char buf[MAX_SIZE];
	va_list ap;

	if (!db || !sql) {
		wn_report_error (WE_PARAM, "wn_db_query: database or query not given");
		return 0;
	}

	if ((res_idx < 0) || (res_idx >= MAX_RES)) {
		wn_report_error (WE_PARAM, "wn_db_query: result index out of bounds");
		return 0;
	}

	if (!db->connected) 
		wn_db_connect (db);

	if (!db->connected) {
		wn_report_error (WE_DB, "wn_db_query: database is not connected");
		return 0;
	}
		
	wn_db_release (db, res_idx);

	va_start (ap, sql);
	vsprintf (buf, sql, ap);
	va_end (ap);

	if (mysql_query (&(db->conn), buf)) {
		if (strstr (mysql_error (&(db->conn)), "away") ||
			  strstr (mysql_error (&(db->conn)), "connect")) {
			mysql_close (&(db->conn));
			db->connected = 0;
			if (!wn_db_connect (db) || (mysql_query (&(db->conn), buf))) {
				wn_report_error (WE_DB, "wn_db_query: %s", mysql_error (&(db->conn)));
				return 0;
			}
		} else {
			wn_report_error (WE_DB, "wn_db_query: %s", mysql_error (&(db->conn)));
			return 0;
		}
	}
	
	if ((!strncmp (buf, "update", 6)) || (!strncmp (buf, "insert", 6)) ||
	    (!strncmp (buf, "delete", 6))) 
		return 1;

	if (!(db->res[res_idx] = mysql_store_result (&(db->conn)))) {
		if (mysql_num_fields (&(db->conn))) {
			wn_report_error (WE_DB, "%s", mysql_error (&(db->conn)));
			return 0;
		} else
			return 0;  /* however, here it is not an error.. (hm?) */
	}

	db->has_result[res_idx] = 1;
	return 1;
}

int wn_db_next_result (wn_db_t *db, int res_idx) {
	if (!db) {
		wn_report_error (WE_PARAM, "wn_db_next_result: invalid db connection");
		return 0;
	}

	if ((res_idx < 0) || (res_idx >= MAX_RES)) {
		wn_report_error (WE_PARAM, "wn_db_next_result: result index out of bounds");
		return 0;
	}

	if (!db->has_result[res_idx] || !db->res[res_idx]) 
		return 0;

	if ((db->row[res_idx] = mysql_fetch_row (db->res[res_idx]))) 
		return 1;

	return 0;
}

char *wn_db_value (wn_db_t *db, int res_idx, int column) {
	if (!db) {
		wn_report_error (WE_PARAM, "wn_db_value: invalid db connection");
		return NULL;
	}

	if ((res_idx < 0) || (res_idx >= MAX_RES)) {
		wn_report_error (WE_PARAM, "wn_db_query: result index out of bounds");
		return 0;
	}

	if (!db->row[res_idx]) 
		wn_db_next_result (db, res_idx);

	if (!db->row) 
		return NULL;

	return (char *)((db->row[res_idx])[column]);
}

int wn_db_release (wn_db_t *db, int res_idx) {
	if (db->has_result[res_idx]) {
		if (db->res[res_idx])
			mysql_free_result (db->res[res_idx]);
		db->has_result[res_idx] = 0;
	}
	return 1;
}

int wn_db_free (wn_db_t *db) {
	int i;

	if (!db) 
		return 1;

	for (i = 0; i < MAX_RES; i++) 
		if (db->has_result[i] && db->res[i])
			mysql_free_result (db->res[i]);

	if (db->connected)
		mysql_close (&(db->conn));

	free (db);
	return 1;
}
