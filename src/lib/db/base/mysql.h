#ifndef BASE_MYSQL_H
#define BASE_MYSQL_H

#include <mysql.h>

#define MAX_RES 	3
#define MAX_RETRY	5

typedef struct _wn_db_t {
	MYSQL conn;
	MYSQL_RES *res[MAX_RES];
	MYSQL_ROW row[MAX_RES];
	int has_result[MAX_RES];
	int connected;
} wn_db_t;

wn_db_t *wn_db_init ();

int wn_db_connect (wn_db_t *db);

int wn_db_query (wn_db_t *db, int res_idx, char *sql, ...);

int wn_db_next_result (wn_db_t *db, int res_idx);

char *wn_db_value (wn_db_t *db, int res_idx, int column);

int wn_db_release (wn_db_t *db, int res_idx);

int wn_db_free (wn_db_t *db);

#endif
