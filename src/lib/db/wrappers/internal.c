#include <string.h>
#include "internal.h"

char *wrapper_internal_email_by_uid (wn_db_t *db, int uid) {
	int res_idx;
	char *email;

	for (res_idx = 0; res_idx < MAX_RES; res_idx++)
		if (!(db->has_result[res_idx])) 
			break;

	if ((!wn_db_query (db, res_idx, "select email from users where id = %i", uid)) ||
		  (!wn_db_next_result (db, res_idx)))
		return NULL;

	email = strdup (wn_db_value (db, res_idx, 0));

	wn_db_release (db, res_idx);

	return email;
}
