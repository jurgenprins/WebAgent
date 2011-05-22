#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../wn_err.h"
#include "../wn_db.h"

char *wn_db_email_by_uid (wn_db_t *db, int uid, char *ubase) {
	int idx = 0;
	char *(*wrapper_fun)() = NULL;
	
	if (!ubase) {
		wn_report_error (WE_PARAM, "wn_db_email_by_uid: ubase not given");
		return NULL;
	}

	while (wrappers[idx]) {
		if (atoi ((char *)wrappers[idx]) == atoi (ubase)) {
			wrapper_fun = wrappers[idx + 1];
			break;
		}
		idx += 2;
	}

	if (!wrapper_fun) {
		wn_report_error (WE_DB, "wn_db_email_by_uid: no wrapper available for user database %i", ubase);
		return NULL;
	}

	return wrapper_fun (db, uid);
}
