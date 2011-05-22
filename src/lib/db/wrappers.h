#ifndef DB_WRAPPERS_H
#define DB_WRAPPERS_H

#include "wrappers/internal.h"
#include "wrappers/dds4.h"

#define WRAPPER_INTERNAL 	"0"
#define WRAPPER_DDS4			"1"

static void *wrappers[] = 
{
	WRAPPER_INTERNAL, wrapper_internal_email_by_uid,
	WRAPPER_DDS4, wrapper_dds4_email_by_uid,
	NULL
};

char *wn_db_email_by_uid (wn_db_t *db, int uid, char *ubase);

#endif
